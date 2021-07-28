#include "../Common/TarsUtils.h"
#include "StorageServiceServer.h"

using namespace bcostars;

class StorageServiceApp : public Application
{
public:
    ~StorageServiceApp() override{};

    void initialize() override
    {
        addAllConfig(this);
        addServant<StorageServiceServer>(ServerConfig::Application + "." +
                                         ServerConfig::ServerName + "." + STORAGE_SERVANT_NAME);
    }

    void destroyApp() override {}
};

int main(int argc, char* argv[])
{
    try
    {
        bcos::initializer::initCommandLine(argc, argv);
        StorageServiceApp app;
        app.main(argc, argv);
        app.waitForShutdown();
        std::cout << "StorageService exit" << std::endl;
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