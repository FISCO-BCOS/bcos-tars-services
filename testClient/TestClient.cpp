#include "../tars/LedgerService.h"
#include "servant/CommunicatorFactory.h"

int main() {
  CommunicatorPtr c = new tars::Communicator();
  c->setProperty("locator",
                 "tars.tarsregistry.QueryObj@tcp -h 172.31.168.134 -p 17890");

  auto proxy = c->stringToProxy<bcostars::LedgerServicePrx>(
      "bcosNode1.LedgerService.LedgerServiceObj");

  auto ret = proxy->test();

  std::cout << "ret: " << ret << std::endl;
}