#pragma once

#include "../Common/ErrorConverter.h"
#include "../Common/TarsUtils.h"
#include "../FrontService/FrontServiceClient.h"
#include "../libinitializer/ProtocolInitializer.h"
#include "libutilities/Common.h"
#include "libutilities/Log.h"
#include <bcos-crypto/signature/key/KeyFactoryImpl.h>
#include <bcos-framework/interfaces/crypto/KeyInterface.h>
#include <bcos-framework/libtool/NodeConfig.h>
#include <bcos-framework/libutilities/BoostLogInitializer.h>
#include <bcos-gateway/Gateway.h>
#include <bcos-gateway/GatewayConfig.h>
#include <bcos-gateway/GatewayFactory.h>
#include <bcos-tars-protocol/GatewayService.h>
#include <chrono>
#include <mutex>

#define GATEWAYSERVICE_LOG(LEVEL) BCOS_LOG(LEVEL) << "[GATEWAYSERVICE][INITIALIZER]"
#define GATEWAYSERVICE_BADGE "[GATEWAYSERVICE]"
namespace bcostars
{
class GatewayServiceServer : public bcostars::GatewayService
{
public:
    void initialize() override
    {
        try
        {
            init();
            m_running = true;
        }
        catch (tars::TC_Exception const& e)
        {
            TLOGERROR(GATEWAYSERVICE_BADGE << "init the GateWay exceptioned, exist now"
                                           << LOG_KV("error", e.what()) << std::endl);
            exit(0);
        }
        catch (std::exception const& e)
        {
            TLOGERROR(GATEWAYSERVICE_BADGE << "init the GateWay exceptioned, exist now"
                                           << LOG_KV("error", boost::diagnostic_information(e))
                                           << std::endl);
            exit(0);
        }
    }

    void init()
    {
        std::call_once(m_initFlag, []() {
            bcos::gateway::GatewayFactory factory;
            auto configPath = ServerConfig::BasePath + "config.ini";

            TLOGINFO(GATEWAYSERVICE_BADGE << LOG_DESC("initLog") << LOG_KV("configPath", configPath)
                                          << std::endl);
            boost::property_tree::ptree pt;
            boost::property_tree::read_ini(configPath, pt);
            m_logInitializer = std::make_shared<bcos::BoostLogInitializer>();
            // set the boost log into the tars log directory
            m_logInitializer->setLogPath(getLogPath());
            m_logInitializer->initLog(pt);
            TLOGINFO(GATEWAYSERVICE_BADGE << LOG_DESC("initLog success") << std::endl);

            TLOGINFO(GATEWAYSERVICE_BADGE << LOG_DESC("initGateWayConfig")
                                          << LOG_KV("configPath", configPath) << std::endl);
            auto gateWayConfig = std::make_shared<bcos::gateway::GatewayConfig>();
            gateWayConfig->setCertPath(ServerConfig::BasePath);
            gateWayConfig->setNodePath(ServerConfig::BasePath);
            gateWayConfig->initConfig(configPath);

            TLOGINFO(GATEWAYSERVICE_BADGE
                     << LOG_DESC("buildGateWay") << LOG_KV("certPath", gateWayConfig->certPath())
                     << LOG_KV("nodePath", gateWayConfig->nodePath()) << std::endl);

            m_gateway = factory.buildGateway(gateWayConfig);
            TLOGINFO(GATEWAYSERVICE_BADGE << "buildGateway success" << std::endl);

            TLOGINFO(GATEWAYSERVICE_BADGE << "load nodeConfig" << std::endl);
            auto nodeConfig = std::make_shared<bcos::tool::NodeConfig>();
            nodeConfig->loadConfig(configPath);
            TLOGINFO(GATEWAYSERVICE_BADGE << LOG_DESC("load nodeConfig success") << std::endl);

            GATEWAYSERVICE_LOG(INFO) << LOG_DESC("init protocol");
            auto protocolInitializer = std::make_shared<bcos::initializer::ProtocolInitializer>();
            protocolInitializer->init(nodeConfig);
            GATEWAYSERVICE_LOG(INFO) << LOG_DESC("init protocol success");

            GATEWAYSERVICE_LOG(INFO) << LOG_DESC("init the nodeID");
            auto privateKeyPath = ServerConfig::BasePath + "node.pem";
            protocolInitializer->loadKeyPair(privateKeyPath);
            GATEWAYSERVICE_LOG(INFO)
                << LOG_DESC("init the nodeID success")
                << LOG_KV("nodeID", protocolInitializer->keyPair()->publicKey()->shortHex());
            m_keyFactory = protocolInitializer->keyFactory();

            GATEWAYSERVICE_LOG(INFO) << LOG_DESC("create the frontService client");
            auto frontServiceProxy =
                Application::getCommunicator()->stringToProxy<bcostars::FrontServicePrx>(
                    getProxyDesc(FRONT_SERVICE_NAME));
            auto frontService = std::make_shared<bcostars::FrontServiceClient>(
                frontServiceProxy, protocolInitializer->keyFactory());
            GATEWAYSERVICE_LOG(INFO) << LOG_DESC("create the frontService client success");

            GATEWAYSERVICE_LOG(INFO) << LOG_DESC("register the frontService");
            m_gateway->registerFrontService(
                nodeConfig->groupId(), protocolInitializer->keyPair()->publicKey(), frontService);
            GATEWAYSERVICE_LOG(INFO) << LOG_DESC("register the frontService success");
            // start the gateway
            GATEWAYSERVICE_LOG(INFO) << LOG_DESC("start the gateway");
            m_gateway->start();
            GATEWAYSERVICE_LOG(INFO) << LOG_DESC("start the gateway success");
        });
    }

    void destroy() override
    {
        if (!m_running)
        {
            GATEWAYSERVICE_LOG(WARNING) << LOG_DESC("The GatewayService has already been stopped");
            return;
        }
        m_running = false;
        GATEWAYSERVICE_LOG(INFO) << LOG_DESC("Stop the GatewayService");
        if (m_gateway)
        {
            m_gateway->stop();
        }
        TLOGINFO(LOG_DESC("[GATEWAYSERVICE] Stop the GatewayService success") << std::endl);
    }

    bcostars::Error asyncSendBroadcastMessage(const std::string& groupID,
        const vector<tars::Char>& srcNodeID, const vector<tars::Char>& payload,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);
        auto bcosNodeID = m_keyFactory->createKey(
            bcos::bytesConstRef((const bcos::byte*)srcNodeID.data(), srcNodeID.size()));
        m_gateway->asyncSendBroadcastMessage(groupID, bcosNodeID,
            bcos::bytesConstRef((const bcos::byte*)payload.data(), payload.size()));

        async_response_asyncSendBroadcastMessage(current, toTarsError(nullptr));
        return bcostars::Error();
    }

    bcostars::Error asyncGetPeers(std::string&, tars::TarsCurrentPtr current) override
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        GATEWAYSERVICE_LOG(DEBUG) << LOG_DESC("asyncGetPeers") << LOG_DESC("request");
        current->setResponse(false);
        m_gateway->asyncGetPeers(
            [current, t1](const bcos::Error::Ptr _error, std::string const& peers) {
                auto t2 = std::chrono::high_resolution_clock::now();
                auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
                GATEWAYSERVICE_LOG(DEBUG) << LOG_DESC("asyncGetPeers") << LOG_KV("response", peers)
                                          << LOG_KV("cost", cost.count());
                async_response_asyncGetPeers(current, toTarsError(_error), peers);
            });
        return bcostars::Error();
    }

    bcostars::Error asyncSendMessageByNodeID(const std::string& groupID,
        const vector<tars::Char>& srcNodeID, const vector<tars::Char>& dstNodeID,
        const vector<tars::Char>& payload, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosSrcNodeID = m_keyFactory->createKey(
            bcos::bytesConstRef((const bcos::byte*)srcNodeID.data(), srcNodeID.size()));
        auto bcosDstNodeID = m_keyFactory->createKey(
            bcos::bytesConstRef((const bcos::byte*)dstNodeID.data(), dstNodeID.size()));

        m_gateway->asyncSendMessageByNodeID(groupID, bcosSrcNodeID, bcosDstNodeID,
            bcos::bytesConstRef((const bcos::byte*)payload.data(), payload.size()),
            [current](bcos::Error::Ptr error) {
                async_response_asyncSendMessageByNodeID(current, toTarsError(error));
            });
        return bcostars::Error();
    }

    bcostars::Error asyncSendMessageByNodeIDs(const std::string& groupID,
        const vector<tars::Char>& srcNodeID, const vector<vector<tars::Char>>& dstNodeID,
        const vector<tars::Char>& payload, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosSrcNodeID = m_keyFactory->createKey(
            bcos::bytesConstRef((const bcos::byte*)srcNodeID.data(), srcNodeID.size()));
        std::vector<bcos::crypto::NodeIDPtr> nodeIDs;
        nodeIDs.reserve(dstNodeID.size());
        for (auto const& it : dstNodeID)
        {
            nodeIDs.push_back(m_keyFactory->createKey(
                bcos::bytesConstRef((const bcos::byte*)it.data(), it.size())));
        }

        m_gateway->asyncSendMessageByNodeIDs(groupID, bcosSrcNodeID, nodeIDs,
            bcos::bytesConstRef((const bcos::byte*)payload.data(), payload.size()));

        async_response_asyncSendMessageByNodeIDs(current, toTarsError(nullptr));
        return bcostars::Error();
    }

    bcostars::Error asyncGetNodeIDs(const std::string& groupID, vector<vector<tars::Char>>& nodeIDs,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        m_gateway->asyncGetNodeIDs(
            groupID, [current](bcos::Error::Ptr _error,
                         std::shared_ptr<const bcos::crypto::NodeIDs> _nodeIDs) {
                // Note: the nodeIDs maybe null if no connections
                std::vector<std::vector<char>> tarsNodeIDs;
                if (!_nodeIDs)
                {
                    async_response_asyncGetNodeIDs(current, toTarsError(_error), tarsNodeIDs);
                    return;
                }
                tarsNodeIDs.reserve(_nodeIDs->size());
                for (auto const& it : *_nodeIDs)
                {
                    auto nodeIDData = it->data();
                    tarsNodeIDs.emplace_back(nodeIDData.begin(), nodeIDData.end());
                }
                async_response_asyncGetNodeIDs(current, toTarsError(_error), tarsNodeIDs);
            });

        return bcostars::Error();
    }

private:
    static std::once_flag m_initFlag;
    static bcos::gateway::Gateway::Ptr m_gateway;
    static bcos::crypto::KeyFactory::Ptr m_keyFactory;
    std::atomic_bool m_running = {false};
    static bcos::BoostLogInitializer::Ptr m_logInitializer;
};
}  // namespace bcostars