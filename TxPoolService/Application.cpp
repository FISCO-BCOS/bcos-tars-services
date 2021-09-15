#include "../Common/TarsUtils.h"
#include "TxPoolServiceServer.h"
#include <tarscpp/servant/Application.h>

using namespace bcostars;

class TxPoolServiceApp : public Application
{
public:
    virtual ~TxPoolServiceApp() override{};

    virtual void initialize() override
    {
        addAllConfig(this);
        addServant<TxPoolServiceServer>(
            ServerConfig::Application + "." + ServerConfig::ServerName + "." + TXPOOL_SERVANT_NAME);
    }

    virtual void destroyApp() override {}
};

int main(int argc, char* argv[])
{
    try
    {
        bcos::initializer::initCommandLine(argc, argv);
        TxPoolServiceApp app;
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