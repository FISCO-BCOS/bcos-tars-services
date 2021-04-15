#include "LedgerImp.h"
#include "servant/Application.h"

using namespace bcos_tars;

class LedgerService : public Application {
public:
  virtual ~LedgerService() override{};

  virtual void initialize() override {}

  virtual void destroyApp() override {}
};

int main(int argc, char *argv[]) {
  try {
    LedgerService service;
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