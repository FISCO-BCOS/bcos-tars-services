#include "StorageServiceServer.h"

using namespace bcostars;

class StorageServiceApp : public Application {
public:
  virtual ~StorageServiceApp() override{};

  virtual void initialize() override {
    addConfig("bcos-storage.toml");

    addServant<StorageServiceServer>(ServerConfig::Application + "." + ServerConfig::ServerName + ".StorageServiceObj");
  }

  virtual void destroyApp() override {}
};

int main(int argc, char *argv[]) {
  try {
    StorageServiceApp app;
    app.main(argc, argv);
    app.waitForShutdown();

    return 0;
  } catch (std::exception &e) {
    cerr << "std::exception:" << e.what() << std::endl;
  } catch (...) {
    cerr << "unknown exception." << std::endl;
  }
  return -1;
}