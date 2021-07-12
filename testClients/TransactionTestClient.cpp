#include "../Common/TarsUtils.h"
#include "../StorageService/StorageServiceClient.h"
#include "../TxPoolService/TxPoolServiceClient.h"
#include "../protocols/BlockHeaderImpl.h"
#include "../protocols/BlockImpl.h"
#include "Block.h"
#include "TxPoolService.h"
#include <bcos-crypto/encrypt/AESCrypto.h>
#include <bcos-crypto/encrypt/SM4Crypto.h>
#include <bcos-crypto/hash/Keccak256.h>
#include <bcos-crypto/hash/SM3.h>
#include <bcos-crypto/signature/secp256k1/Secp256k1Crypto.h>
#include <bcos-crypto/signature/sm2/SM2Crypto.h>
#include <bcos-ledger/libledger/Ledger.h>

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

    return blockFactory;
}

inline bcos::bytes fakeHelloWorldSet()
{
    return *bcos::fromHexString(
        "0x4ed3885e00000000000000000000000000000000000000000000000000000000000000200000000000000000"
        "000000000000000000000000000000000000000000000005666973636f00000000000000000000000000000000"
        "0000000000000000000000");
}

inline bcos::bytes fakeHelloWorldDeployInput()
{
    std::string helloBin =
        "0x60806040526040805190810160405280600181526020017f3100000000000000000000000000000000000000"
        "0000000000000000000000008152506001908051906020019061004f9291906100ae565b5034801561005c5760"
        "0080fd5b506040805190810160405280600d81526020017f48656c6c6f2c20576f726c64210000000000000000"
        "0000000000000000000000815250600090805190602001906100a89291906100ae565b50610153565b82805460"
        "0181600116156101000203166002900490600052602060002090601f016020900481019282601f106100ef5780"
        "5160ff191683800117855561011d565b8280016001018555821561011d579182015b8281111561011c57825182"
        "5591602001919060010190610101565b5b50905061012a919061012e565b5090565b61015091905b8082111561"
        "014c576000816000905550600101610134565b5090565b90565b6104ac806101626000396000f3006080604052"
        "60043610610057576000357c0100000000000000000000000000000000000000000000000000000000900463ff"
        "ffffff1680634ed3885e1461005c57806354fd4d50146100c55780636d4ce63c14610155575b600080fd5b3480"
        "1561006857600080fd5b506100c3600480360381019080803590602001908201803590602001908080601f0160"
        "208091040260200160405190810160405280939291908181526020018383808284378201915050505050509192"
        "9192905050506101e5565b005b3480156100d157600080fd5b506100da61029b565b6040518080602001828103"
        "825283818151815260200191508051906020019080838360005b8381101561011a578082015181840152602081"
        "0190506100ff565b50505050905090810190601f1680156101475780820380516001836020036101000a031916"
        "815260200191505b509250505060405180910390f35b34801561016157600080fd5b5061016a610339565b6040"
        "518080602001828103825283818151815260200191508051906020019080838360005b838110156101aa578082"
        "01518184015260208101905061018f565b50505050905090810190601f1680156101d757808203805160018360"
        "20036101000a031916815260200191505b509250505060405180910390f35b80600090805190602001906101fb"
        "9291906103db565b507f93a093529f9c8a0c300db4c55fcd27c068c4f5e0e8410bc288c7e76f3d71083e816040"
        "518080602001828103825283818151815260200191508051906020019080838360005b8381101561025e578082"
        "015181840152602081019050610243565b50505050905090810190601f16801561028b57808203805160018360"
        "20036101000a031916815260200191505b509250505060405180910390a150565b600180546001816001161561"
        "01000203166002900480601f016020809104026020016040519081016040528092919081815260200182805460"
        "0181600116156101000203166002900480156103315780601f1061030657610100808354040283529160200191"
        "610331565b820191906000526020600020905b81548152906001019060200180831161031457829003601f1682"
        "01915b505050505081565b606060008054600181600116156101000203166002900480601f0160208091040260"
        "200160405190810160405280929190818152602001828054600181600116156101000203166002900480156103"
        "d15780601f106103a6576101008083540402835291602001916103d1565b820191906000526020600020905b81"
        "54815290600101906020018083116103b457829003601f168201915b5050505050905090565b82805460018160"
        "0116156101000203166002900490600052602060002090601f016020900481019282601f1061041c57805160ff"
        "191683800117855561044a565b8280016001018555821561044a579182015b8281111561044957825182559160"
        "200191906001019061042e565b5b509050610457919061045b565b5090565b61047d91905b8082111561047957"
        "6000816000905550600101610461565b5090565b905600a165627a7a723058204736027ad6b97d7cd2685379ac"
        "b35b386dcb18799934be8283f1e08cd1f0c6ec0029";
    return *bcos::fromHexString(helloBin);
}

int main(int argc, char* argv[])
{
    std::string ip;
    unsigned short port;
    std::string app;
    size_t count;
    std::string chainID;
    std::string groupID;

    boost::program_options::options_description desc("Transaction client options");

    // clang-format off
    desc.add_options()
        ("help", "Produce help message")
        ("ip", boost::program_options::value<std::string>(&ip)->default_value("127.0.0.1"), "Tars locator server ip")
        ("port", boost::program_options::value<unsigned short>(&port)->default_value(17890), "Tars locator server port")
        ("app", boost::program_options::value<std::string>(&app)->default_value("bcostars"), "BCOS application name")
        ("count", boost::program_options::value<size_t>(&count)->default_value(1), "Transaction count")
        ("chain", boost::program_options::value<std::string>(&chainID), "Chain ID")
        ("group", boost::program_options::value<std::string>(&groupID), "Group ID")
    ;
    // clang-format on

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);

    boost::program_options::notify(vm);
    if (vm.find("help") != vm.end() || chainID.empty() || groupID.empty())
    {
        desc.print(std::cout);
        return 0;
    }

    CommunicatorPtr c = new Communicator();

    std::string locatorStr =
        "tars.tarsregistry.QueryObj@tcp -h " + ip + " -p " + boost::lexical_cast<std::string>(port);
    std::cout << "Using locator: " << locatorStr << std::endl;

    c->setProperty("locator", locatorStr);
    c->setProperty("modulename", "webank.TransactionTestClient");

    auto storageProxy =
        c->stringToProxy<bcostars::StorageServicePrx>(app + "." + bcostars::STORAGE_SERVICE_NAME);
    auto txpoolProxy =
        c->stringToProxy<bcostars::TxPoolServicePrx>(app + "." + bcostars::TXPOOL_SERVICE_NAME);

    auto blockFactory = initBlockFactory();

    auto ledger = std::make_shared<bcos::ledger::Ledger>(
        blockFactory, std::make_shared<bcostars::StorageServiceClient>(storageProxy));
    auto txpool =
        std::make_shared<bcostars::TxPoolServiceClient>(txpoolProxy, blockFactory->cryptoSuite());

    std::cout << "Start send transaction" << std::endl;

    auto keyPair = blockFactory->cryptoSuite()->signatureImpl()->generateKeyPair();

    for (size_t i = 0; i < count; ++i)
    {
        std::promise<std::tuple<bcos::Error::Ptr, bcos::protocol::BlockNumber>> getBlockPromise;
        ledger->asyncGetBlockNumber(
            [&getBlockPromise](bcos::Error::Ptr error, bcos::protocol::BlockNumber number) {
                getBlockPromise.set_value(std::make_tuple(error, number));
            });

        auto blockNumberResult = getBlockPromise.get_future().get();

        bcos::u256 nonce = bcos::utcTime();
        auto txBlockLimit = std::get<1>(blockNumberResult) + 500;

        auto tx = blockFactory->transactionFactory()->createTransaction(0, bcos::bytes(),
            fakeHelloWorldDeployInput(), nonce, txBlockLimit, chainID, groupID, 0, keyPair);

        std::promise<std::tuple<bcos::Error::Ptr, bcos::protocol::TransactionSubmitResult::Ptr>>
            submitPromise;

        auto encodedTxData = tx->encode();
        auto txData = std::make_shared<bcos::bytes>(encodedTxData.begin(), encodedTxData.end());
        txpool->asyncSubmit(txData, [&submitPromise](bcos::Error::Ptr error,
                                        bcos::protocol::TransactionSubmitResult::Ptr result) {
            submitPromise.set_value(std::make_tuple(error, result));
        });

        auto result = std::get<1>(submitPromise.get_future().get());
        std::cout << "Transaction status: " << result->status() << std::endl;
        std::cout << "Transaction hash: " << result->txHash() << std::endl;
        std::cout << "Block hash" << result->blockHash() << std::endl;
        std::cout << std::endl;
    }

    std::cout << "All transactions finished" << std::endl;

    return 0;
}