#include "../Common/TarsUtils.h"
#include "FrontServiceServer.h"
#include <tarscpp/servant/Application.h>

using namespace bcostars;

class FrontServiceApp : public Application
{
public:
    virtual ~FrontServiceApp() override{};

    virtual void initialize() override
    {
        addAllConfig(this);
        addServant<FrontServiceServer>(ServerConfig::Application + "." + ServerConfig::ServerName +
                                       "." + bcos::protocol::FRONT_SERVANT_NAME);
    }

    virtual void destroyApp() override {}
};

int main(int argc, char* argv[])
{
    try
    {
        bcos::initializer::initCommandLine(argc, argv);
        FrontServiceApp app;
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