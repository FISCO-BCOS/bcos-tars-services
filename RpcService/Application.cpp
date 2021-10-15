#include "../Common/TarsUtils.h"
#include "RpcInitializer.h"
#include "RpcServiceServer.h"
#include <bcos-framework/libutilities/BoostLogInitializer.h>
#include <tarscpp/servant/Application.h>

using namespace bcostars;
class RpcServiceApp : public Application
{
public:
    RpcServiceApp() : m_iniConfigPath(ServerConfig::BasePath + "config.ini")
    {
        addAppConfig("config.ini");
    }

    ~RpcServiceApp() override {}

    void initialize() override
    {
        initService(m_iniConfigPath);
        RpcServiceParam param;
        param.rpcInitializer = m_rpcInitializer;


        addServantWithParams<RpcServiceServer, RpcServiceParam>(
            ServerConfig::Application + "." + ServerConfig::ServerName + "." +
                bcos::protocol::RPC_SERVANT_NAME,
            param);
    }
    void destroyApp() override {}

protected:
    virtual void initService(std::string const& _configPath)
    {
        // init the log
        boost::property_tree::ptree pt;
        boost::property_tree::read_ini(_configPath, pt);
        m_logInitializer = std::make_shared<bcos::BoostLogInitializer>();
        m_logInitializer->initLog(pt);
        // init rpc
        m_rpcInitializer = std::make_shared<RpcInitializer>();
        m_rpcInitializer->init(_configPath);
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