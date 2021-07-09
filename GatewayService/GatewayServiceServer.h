#pragma once

#include "../Common/ErrorConverter.h"
#include "../Common/TarsUtils.h"
#include "../FrontService/FrontServiceClient.h"
#include "../libinitializer/ProtocolInitializer.h"
#include "GatewayService.h"
#include <bcos-crypto/signature/key/KeyFactoryImpl.h>
#include <bcos-framework/interfaces/crypto/KeyInterface.h>
#include <bcos-framework/libtool/NodeConfig.h>
#include <bcos-framework/libutilities/BoostLogInitializer.h>
#include <bcos-gateway/Gateway.h>
#include <bcos-gateway/GatewayConfig.h>
#include <bcos-gateway/GatewayFactory.h>
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
        std::call_once(m_initFlag, [this]() {
            bcos::gateway::GatewayFactory factory;
            auto configPath = ServerConfig::BasePath + "config.ini";

            TLOGINFO(GATEWAYSERVICE_BADGE << LOG_DESC("initLog") << LOG_KV("configPath", configPath)
                                          << std::endl);
            boost::property_tree::ptree pt;
            boost::property_tree::read_ini(configPath, pt);
            m_logInitializer = std::make_shared<bcos::BoostLogInitializer>();
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
            GATEWAYSERVICE_LOG(INFO) << LOG_DESC("start the frontService");
            m_gateway->start();
            GATEWAYSERVICE_LOG(INFO) << LOG_DESC("start the frontService success");
        });

        m_keyFactory = std::make_shared<bcos::crypto::KeyFactoryImpl>();
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
        if (m_logInitializer)
        {
            m_logInitializer->stopLogging();
        }
        GATEWAYSERVICE_LOG(INFO) << LOG_DESC("Stop the GatewayService success");
    }

    bcostars::Error asyncSendBroadcastMessage(const std::string& groupID,
        const vector<tars::UInt8>& srcNodeID, const vector<tars::UInt8>& payload,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosNodeID = m_keyFactory->createKey(srcNodeID);
        m_gateway->asyncSendBroadcastMessage(groupID, bcosNodeID, bcos::ref(payload));

        async_response_asyncSendBroadcastMessage(current, toTarsError(nullptr));
    }

    bcostars::Error asyncSendMessageByNodeID(const std::string& groupID,
        const vector<tars::UInt8>& srcNodeID, const vector<tars::UInt8>& dstNodeID,
        const vector<tars::UInt8>& payload, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosSrcNodeID = m_keyFactory->createKey(srcNodeID);
        auto bcosDstNodeID = m_keyFactory->createKey(dstNodeID);

        m_gateway->asyncSendMessageByNodeID(groupID, bcosSrcNodeID, bcosDstNodeID,
            bcos::ref(payload), [current](bcos::Error::Ptr error) {
                async_response_asyncSendMessageByNodeID(current, toTarsError(error));
            });
    }

    bcostars::Error asyncSendMessageByNodeIDs(const std::string& groupID,
        const vector<tars::UInt8>& srcNodeID, const vector<vector<tars::UInt8>>& dstNodeID,
        const vector<tars::UInt8>& payload, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosSrcNodeID = m_keyFactory->createKey(srcNodeID);
        std::vector<bcos::crypto::NodeIDPtr> nodeIDs;
        nodeIDs.reserve(dstNodeID.size());
        for (auto const& it : dstNodeID)
        {
            nodeIDs.push_back(m_keyFactory->createKey(it));
        }

        m_gateway->asyncSendMessageByNodeIDs(groupID, bcosSrcNodeID, nodeIDs, bcos::ref(payload));

        async_response_asyncSendMessageByNodeIDs(current, toTarsError(nullptr));
    }

private:
    static std::once_flag m_initFlag;
    static bcos::gateway::Gateway::Ptr m_gateway;
    static bcos::BoostLogInitializer::Ptr m_logInitializer;
    bcos::crypto::KeyFactory::Ptr m_keyFactory;
    std::atomic_bool m_running = {false};
};
}  // namespace bcostars