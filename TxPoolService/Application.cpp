#include "servant/Application.h"
#include "TxPoolServiceServer.h"

using namespace bcostars;

class TxPoolServiceApp : public Application
{
public:
    virtual ~TxPoolServiceApp() override{};

    virtual void initialize() override
    {
        addConfig("FrontService.conf");
        addServant<TxPoolServiceServer>(
            ServerConfig::Application + "." + ServerConfig::ServerName + ".TxPoolServiceObj");
    }

    virtual void destroyApp() override {}
};

int main(int argc, char* argv[])
{
    try
    {
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