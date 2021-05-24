#include "servant/Communicator.h"
#include "StorageService.h"
#include "../StorageService/StorageServiceClient.h"
#include <boost/program_options.hpp>
#include <boost/program_options/variables_map.hpp>

using namespace bcostars;

void summary(const std::string &desc, clock_t cost, size_t successed,
             size_t failed) {
  std::cout << "===================================================="
            << std::endl;
  std::cout << desc << "request done!" << std::endl;
  std::cout << "Success: " << successed << std::endl;
  std::cout << "Failed: " << failed << std::endl;
  std::cout << "Total time cost: " << cost << " ms" << std::endl;
  std::cout << "TPS: " << ((double)successed / (double)(cost * 1000))
            << std::endl;
  std::cout << "===================================================="
            << std::endl;
}

void testPut(StorageServiceClient &client, size_t count) {
  size_t successed = 0;
  size_t failed = 0;

  clock_t start = clock();
  for (size_t i = 0; i < count; ++i) {
    std::string key =
        "/fisco-bcos/test/key-" + boost::lexical_cast<std::string>(i);
    std::string value('v', 64);

    client.asyncPut(
        "default", key, value,
        [&successed, &failed, count, start](const bcos::Error::Ptr &error) {
          if (error && error->errorCode()) {
            std::cout << "Error while put: " << error->errorCode() << " "
                      << error->errorMessage() << std::endl;
            ++failed;
          } else {
            ++successed;
          }

          if (successed + failed == count) {
            clock_t cost = (clock() - start) / 1000;

            summary("Put", cost, successed, failed);
          }
        });
  }
}

void testGet(StorageServiceClient &client, size_t count) {
  size_t successed = 0;
  size_t failed = 0;

  clock_t start = clock();
  for (size_t i = 0; i < count; ++i) {
    std::string key =
        "/fisco-bcos/test/key-" + boost::lexical_cast<std::string>(i);

    client.asyncGet(
        "default", key,
        [&successed, &failed, count, start](const bcos::Error::Ptr &error,
                                            const std::string &value) {
          if (error && error->errorCode()) {
            std::cout << "Error while put!" << std::endl;
            ++failed;
          } else {
            ++successed;

            std::string except('v', 64);
            if (value != except) {
              std::cout << "Value mismatch!" << std::endl;
            }
          }

          if (successed + failed == count) {
            clock_t cost = (clock() - start) / 1000;

            summary("Put", cost, successed, failed);
          }
        });
  }
}

int main(int argc, char *argv[]) {
  std::string ip;
  unsigned short port;
  size_t count;

  boost::program_options::options_description desc("Storage client options.");
  desc.add_options()("help", "produce help message")(
      "ip",
      boost::program_options::value<std::string>(&ip)->default_value(
          "127.0.0.1"),
      "StorageService server ip")(
      "port",
      boost::program_options::value<unsigned short>(&port)->default_value(0),
      "StorageService server port")(
      "count",
      boost::program_options::value<size_t>(&count)->default_value(1000),
      "Total request count");

  boost::program_options::variables_map vm;
  boost::program_options::store(
      boost::program_options::parse_command_line(argc, argv, desc), vm);

  boost::program_options::notify(vm);

  tars::CommunicatorPtr communicator = new tars::Communicator();
  StorageServicePrx proxy = communicator->stringToProxy<StorageServicePrx>(
      std::string(StorageServiceClient::servantName) + "@tcp -h " + ip +
      " -p " + boost::lexical_cast<std::string>(port));

  StorageServiceClient client(proxy);
  testPut(client, count);
  testGet(client, count);

  sleep(1000);
}