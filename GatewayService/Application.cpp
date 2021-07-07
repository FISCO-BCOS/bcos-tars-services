#include "servant/Application.h"
#include "../Common/TarsUtils.h"
#include "GatewayServiceServer.h"

using namespace bcostars;

class GatewayServiceApp : public Application
{
public:
    ~GatewayServiceApp() override{};

    void initialize() override
    {
        addAllConfig(this);
        addServant<GatewayServiceServer>(ServerConfig::Application + "." +
                                         ServerConfig::ServerName + "." + GATEWAY_SERVANT_NAME);
    }

    void destroyApp() override {}
};

int main(int argc, char* argv[])
{
    try
    {
        GatewayServiceApp app;
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