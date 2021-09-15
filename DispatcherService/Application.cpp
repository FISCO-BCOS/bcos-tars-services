#include "../Common/TarsUtils.h"
#include "DispatcherServiceServer.h"
#include <tarscpp/servant/Application.h>

using namespace bcostars;

class DispatcherServiceApp : public Application
{
public:
    virtual ~DispatcherServiceApp() override{};

    virtual void initialize() override
    {
        addAllConfig(this);

        // Note: the tars packet name must be the same with ServerConfig::ServerName
        addServant<DispatcherServiceServer>(ServerConfig::Application + "." +
                                            ServerConfig::ServerName + "." +
                                            DISPATCHER_SERVANT_NAME);
    }

    virtual void destroyApp() override {}
};

int main(int argc, char* argv[])
{
    try
    {
        bcos::initializer::initCommandLine(argc, argv);
        DispatcherServiceApp app;
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