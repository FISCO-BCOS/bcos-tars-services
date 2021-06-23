#include "servant/Application.h"
#include "GatewayServiceServer.h"

using namespace bcostars;

class GatewayServiceApp : public Application {
public:
  virtual ~GatewayServiceApp() override{};

  virtual void initialize() override {
    addConfig("GatewayService.conf");
    addServant<GatewayServiceServer>(ServerConfig::Application + "." + ServerConfig::ServerName + ".GatewayServiceObj");
  }

  virtual void destroyApp() override {}
};

int main(int argc, char *argv[]) {
  try {
    GatewayServiceApp app;
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