#include "StorageImp.h"
#include "servant/Application.h"

using namespace bcos_tars;

class StorageService : public Application {
public:
  virtual ~StorageService() override{};

  virtual void initialize() override {
  }

  virtual void destroyApp() override {}
};

int main(int argc, char *argv[]) {
  try {
    StorageService service;
    service.main(argc, argv);
    service.waitForShutdown();

    return 0;
  } catch (std::exception &e) {
    cerr << "std::exception:" << e.what() << std::endl;
  } catch (...) {
    cerr << "unknown exception." << std::endl;
  }
  return -1;
}