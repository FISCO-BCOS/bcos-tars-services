#include "../tars/LedgerService.h"
#include "servant/CommunicatorFactory.h"

int main() {
  CommunicatorPtr c = new tars::Communicator();
  c->setProperty("locator",
                 "tars.tarsregistry.QueryObj@tcp -h 172.31.168.134 -p 17890");

  auto proxy = c->stringToProxy<bcostars::LedgerServicePrx>(
      "bcostars.LedgerService.LedgerServiceObj");

  bcostars::Block block;
  auto ret = proxy->getBlockByNumber(100, block);

  std::cout << "ret: " << ret << std::endl;
  std::cout << "Block number: " << block.blockHeader.blockNumber << std::endl;
}