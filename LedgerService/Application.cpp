#include "LedgerServiceImp.h"

using namespace bcostars;

class LedgerServiceApp : public Application {
public:
  virtual ~LedgerServiceApp() override{};

  virtual void initialize() override {
    addServant<LedgerServiceImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".LedgerServiceObj");
  }

  virtual void destroyApp() override {}
};

int main(int argc, char *argv[]) {
  try {
    LedgerServiceApp app;
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