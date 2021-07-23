#include "servant/Application.h"
#include "../Common/TarsUtils.h"
#include "RpcServiceServer.h"

using namespace bcostars;

class RpcServiceApp : public Application
{
public:
    virtual ~RpcServiceApp() override{};

    virtual void initialize() override
    {
        addAllConfig(this);
        addServant<RpcServiceServer>(
            ServerConfig::Application + "." + ServerConfig::ServerName + "." + RPC_SERVANT_NAME);
    }

    virtual void destroyApp() override {}
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