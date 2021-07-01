#include "servant/Application.h"
#include "ExecutorServiceServer.h"

using namespace bcostars;

class ExecutorServiceApp : public Application
{
public:
    virtual ~ExecutorServiceApp() override{};

    virtual void initialize() override
    {
        addConfig("config.ini");
        addServant<ExecutorServiceServer>(
            ServerConfig::Application + "." + ServerConfig::ServerName + ".ExecutorServiceObj");
    }

    virtual void destroyApp() override {}
};

int main(int argc, char* argv[])
{
    try
    {
        ExecutorServiceApp app;
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