#include "Common/TarsUtils.h"
#include "RpcService/RpcInitializer.h"
#include "RpcService/RpcServiceServer.h"
#include <bcos-framework/libutilities/BoostLogInitializer.h>
#include <tarscpp/servant/Application.h>

using namespace bcostars;
class RpcServiceApp : public Application
{
public:
    RpcServiceApp() {}

    ~RpcServiceApp() override {}

    void initialize() override
    {
        m_iniConfigPath = ServerConfig::BasePath + "/config.ini";
        addConfig("config.ini");
        initService(m_iniConfigPath);
        RpcServiceParam param;
        param.rpcInitializer = m_rpcInitializer;
        addServantWithParams<RpcServiceServer, RpcServiceParam>(
            getProxyDesc(bcos::protocol::RPC_SERVANT_NAME), param);
    }
    void destroyApp() override {}

protected:
    virtual void initService(std::string const& _configPath)
    {
        // init the log
        boost::property_tree::ptree pt;
        boost::property_tree::read_ini(_configPath, pt);
        m_logInitializer = std::make_shared<bcos::BoostLogInitializer>();
        m_logInitializer->setLogPath(getLogPath());
        m_logInitializer->initLog(pt);
        // init rpc
        auto adapter = Application::getEpollServer()->getBindAdapter(
            getProxyDesc(bcos::protocol::RPC_SERVANT_NAME));
        std::string clientID = endPointToString(
            getProxyDesc(bcos::protocol::RPC_SERVANT_NAME), adapter->getEndpoint());
        BCOS_LOG(INFO) << LOG_DESC("begin init rpc") << LOG_KV("rpcID", clientID);
        m_rpcInitializer = std::make_shared<RpcInitializer>(_configPath, clientID);
        m_rpcInitializer->start();
    }


private:
    std::string m_iniConfigPath;
    bcos::BoostLogInitializer::Ptr m_logInitializer;
    RpcInitializer::Ptr m_rpcInitializer;
};

int main(int argc, char* argv[])
{
    try
    {
        RpcServiceApp app;
        app.main(argc, argv);
        app.waitForShutdown();

        return 0;
    }
    catch (std::exception& e)
    {
        cerr << "std::exception:" << e.what() << std::endl;
    }
    catch (...)
    {
        cerr << "unknown exception." << std::endl;
    }
    return -1;
}