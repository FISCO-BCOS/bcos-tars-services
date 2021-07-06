#include "servant/Application.h"
#include "DispatcherServiceServer.h"
#include "../Common/TarsUtils.h"

using namespace bcostars;

class DispatcherServiceApp : public Application
{
public:
    virtual ~DispatcherServiceApp() override{};

    virtual void initialize() override
    {
        addAllConfig(this);

        addServant<DispatcherServiceServer>(
            ServerConfig::Application + "." + ServerConfig::ServerName + ".DispatcherServiceObj");
    }

    virtual void destroyApp() override {}
};

int main(int argc, char* argv[])
{
    try
    {
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