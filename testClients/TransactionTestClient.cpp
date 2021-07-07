#include "../StorageService/StorageServiceClient.h"
#include "../TxPoolService/TxPoolServiceClient.h"
#include "../protocols/BlockHeaderImpl.h"
#include "../protocols/BlockImpl.h"
#include "Block.h"
#include "TxPoolService.h"
#include "bcos-ledger/ledger/Ledger.h"
#include <bcos-crypto/encrypt/AESCrypto.h>
#include <bcos-crypto/encrypt/SM4Crypto.h>
#include <bcos-crypto/hash/Keccak256.h>
#include <bcos-crypto/hash/SM3.h>
#include <bcos-crypto/signature/secp256k1/Secp256k1Crypto.h>
#include <bcos-crypto/signature/sm2/SM2Crypto.h>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

bcostars::protocol::BlockFactoryImpl::Ptr initBlockFactory()
{
    auto hashImpl = std::make_shared<bcos::crypto::Keccak256>();
    auto signatureImpl = std::make_shared<bcos::crypto::Secp256k1Crypto>();
    auto encryptImpl = std::make_shared<bcos::crypto::AESCrypto>();
    auto cryptoSuite =
        std::make_shared<bcos::crypto::CryptoSuite>(hashImpl, signatureImpl, encryptImpl);

    auto transactionFactory =
        std::make_shared<bcostars::protocol::TransactionFactoryImpl>(cryptoSuite);
    auto receiptFactory =
        std::make_shared<bcostars::protocol::TransactionReceiptFactoryImpl>(cryptoSuite);
    auto blockHeaderFactory =
        std::make_shared<bcostars::protocol::BlockHeaderFactoryImpl>(cryptoSuite);
    auto blockFactory = std::make_shared<bcostars::protocol::BlockFactoryImpl>(
        cryptoSuite, blockHeaderFactory, transactionFactory, receiptFactory);
}

int main(int argc, char* argv[])
{
    std::string ip;
    unsigned short port;
    std::string prefix;
    size_t count;

    boost::program_options::options_description desc("Transaction client options");

    // clang-format off
    desc.add_options()
        ("help", "Produce help message")
        ("ip", boost::program_options::value<std::string>(&ip)->default_value("127.0.0.1"), "Tarsregistry server ip")
        ("port", boost::program_options::value<unsigned short>(&port)->default_value(17890), "Tarsregistry server port")
        ("prefix", boost::program_options::value<std::string>(&prefix)->default_value("bcostars.TxPoolService"), "BCOS prefix")
        ("count", boost::program_options::value<size_t>(&count)->default_value(1), "Transaction count")
    ;
    // clang-format on

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);

    boost::program_options::notify(vm);
    if (vm.find("help") != vm.end())
    {
        desc.print(std::cout);
        return 0;
    }

    CommunicatorPtr c = new Communicator();
    c->setProperty("locator", "tars.tarsregistry.QueryObj@tcp -h " + ip + " -p " +
                                  boost::lexical_cast<std::string>(port));
    auto storageProxy =
        c->stringToProxy<bcostars::StorageServicePrx>(prefix + ".StorageServiceObj");
    auto txpoolProxy = c->stringToProxy<bcostars::TxPoolServicePrx>(prefix + ".TxPoolServiceObj");
    auto blockFactory = initBlockFactory();

    auto ledger = std::make_shared<bcos::ledger::Ledger>(
        blockFactory, std::make_shared<bcostars::StorageServiceClient>(storageProxy));
    auto txpool =
        std::make_shared<bcostars::TxPoolServiceClient>(txpoolProxy, blockFactory->cryptoSuite());

    std::cout << "Start send transaction" << std::endl;

    for (size_t i = 0; i < count; ++i)
    {
        ledger->asyncGetBlockNumber([ledger, txpool](bcos::Error::Ptr error, bcos::protocol::BlockNumber number) {
            // auto tx = 
        });
    }
    // txpool->asyncSubmit(bcos::bytesPointer _tx, bcos::protocol::TxSubmitCallback
    // _txSubmitCallback);

    return 0;
}