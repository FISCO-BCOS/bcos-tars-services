#include "../protocols/BlockImpl.h"
#include "../protocols/TransactionImpl.h"
#include "../protocols/TransactionReceiptImpl.h"
#include "Transaction.h"
#include "bcos-framework/interfaces/crypto/CommonType.h"
#include "bcos-framework/interfaces/crypto/CryptoSuite.h"
#include "bcos-framework/interfaces/protocol/Transaction.h"
#include "bcos-framework/libprotocol/LogEntry.h"
#include "bcos-framework/libutilities/DataConvertUtility.h"
#include "bcos-framework/testutils/crypto/HashImpl.h"
#include "bcos-framework/testutils/crypto/SignatureImpl.h"
#include "interfaces/protocol/ProtocolTypeDef.h"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <memory>

namespace bcostars
{
namespace test
{
struct Fixture
{
    Fixture()
    {
        cryptoSuite =
            std::make_shared<bcos::crypto::CryptoSuite>(std::make_shared<bcos::test::Sm3Hash>(),
                std::make_shared<bcos::test::SM2SignatureImpl>(), nullptr);
    }

    bcos::crypto::CryptoSuite::Ptr cryptoSuite;
};

BOOST_FIXTURE_TEST_SUITE(TestProtocol, Fixture)

BOOST_AUTO_TEST_CASE(transaction)
{
    bcos::bytes to(bcos::asBytes("Target"));
    bcos::bytes input(bcos::asBytes("Arguments"));
    bcos::u256 nonce(800);

    bcostars::protocol::TransactionFactoryImpl factory(cryptoSuite);
    auto tx = factory.createTransaction(0, to, input, nonce, 100, "testChain", "testGroup", 1000, cryptoSuite->signatureImpl()->generateKeyPair());

    auto buffer = tx->encode(false);

    auto decodedTx = factory.createTransaction(buffer, true);

    BOOST_CHECK_EQUAL(tx->hash(), decodedTx->hash());
    BOOST_CHECK_EQUAL(tx->version(), 0);
    BOOST_CHECK_EQUAL(bcos::asString(tx->to()), bcos::asString(to));
    BOOST_CHECK_EQUAL(bcos::asString(tx->input()), bcos::asString(input));

    BOOST_CHECK_EQUAL(tx->nonce(), nonce);
    BOOST_CHECK_EQUAL(tx->blockLimit(), 100);
    BOOST_CHECK_EQUAL(tx->chainId(), "testChain");
    BOOST_CHECK_EQUAL(tx->groupId(), "testGroup");
    BOOST_CHECK_EQUAL(tx->importTime(), 1000);
}

BOOST_AUTO_TEST_CASE(transactionReceipt)
{
    bcos::crypto::HashType stateRoot(bcos::asBytes("root1"));
    bcos::u256 gasUsed(8858);
    bcos::bytes contractAddress(bcos::asBytes("contract Address!"));

    auto logEntries = std::make_shared<std::vector<bcos::protocol::LogEntry>>();
    for (auto i : {1, 2, 3})
    {
        bcos::h256s topics;
        for (auto j : {100, 200, 300})
        {
            topics.push_back(
                bcos::h256(bcos::asBytes("topic: " + boost::lexical_cast<std::string>(j))));
        }
        bcos::protocol::LogEntry entry(
            bcos::asBytes("Address: " + boost::lexical_cast<std::string>(i)), topics,
            bcos::asBytes("Data: " + boost::lexical_cast<std::string>(i)));
        logEntries->emplace_back(entry);
    }
    bcos::bytes output(bcos::asBytes("Output!"));

    bcostars::protocol::TransactionReceiptFactoryImpl factory(cryptoSuite);
    auto receipt = factory.createReceipt(gasUsed, contractAddress,
        std::make_shared<std::vector<bcos::protocol::LogEntry>>(*logEntries), 50, output, 888);

    bcos::bytes buffer;
    receipt->encode(buffer);

    auto decodedReceipt = factory.createReceipt(buffer);

    BOOST_CHECK_EQUAL(receipt->hash().hex(), decodedReceipt->hash().hex());
    BOOST_CHECK_EQUAL(receipt->version(), 0);
    BOOST_CHECK_EQUAL(receipt->gasUsed(), gasUsed);
    BOOST_CHECK_EQUAL(bcos::asString(receipt->contractAddress()), bcos::asString(contractAddress));
    BOOST_CHECK_EQUAL(receipt->logEntries().size(), logEntries->size());
    for (auto i = 0; i < receipt->logEntries().size(); ++i)
    {
        BOOST_CHECK_EQUAL(
            receipt->logEntries()[i].address().toString(), (*logEntries)[i].address().toString());
        BOOST_CHECK_EQUAL(
            receipt->logEntries()[i].topics().size(), (*logEntries)[i].topics().size());
        for (auto j = 0; j < receipt->logEntries()[i].topics().size(); ++j)
        {
            BOOST_CHECK_EQUAL(
                receipt->logEntries()[i].topics()[j].hex(), (*logEntries)[i].topics()[j].hex());
        }
        BOOST_CHECK_EQUAL(
            receipt->logEntries()[i].data().toString(), (*logEntries)[i].data().toString());
    }

    BOOST_CHECK_EQUAL(receipt->status(), 50);
    BOOST_CHECK_EQUAL(bcos::asString(receipt->output()), bcos::asString(output));
    BOOST_CHECK_EQUAL(receipt->blockNumber(), 888);
}

BOOST_AUTO_TEST_CASE(block)
{
    auto blockHeaderFactory =
        std::make_shared<bcostars::protocol::BlockHeaderFactoryImpl>(cryptoSuite);
    auto transactionFactory =
        std::make_shared<bcostars::protocol::TransactionFactoryImpl>(cryptoSuite);
    auto transactionReceiptFactory =
        std::make_shared<bcostars::protocol::TransactionReceiptFactoryImpl>(cryptoSuite);
    bcostars::protocol::BlockFactoryImpl blockFactory(
        cryptoSuite, blockHeaderFactory, transactionFactory, transactionReceiptFactory);

    auto block = blockFactory.createBlock();
    block->setVersion(883);
    block->setBlockType(bcos::protocol::WithTransactionsHash);

    bcos::bytes to(bcos::asBytes("Target"));
    bcos::bytes input(bcos::asBytes("Arguments"));
    bcos::u256 nonce(100);

    bcos::crypto::HashType stateRoot(bcos::asBytes("root1"));
    bcos::u256 gasUsed(8858);
    bcos::bytes contractAddress(bcos::asBytes("contract Address!"));

    // set the blockHeader
    auto header = block->blockHeader();
    header->setNumber(100);
    header->setGasUsed(1000);
    header->setStateRoot(bcos::crypto::HashType("62384386743874"));
    header->setTimestamp(500);

    auto logEntries = std::make_shared<std::vector<bcos::protocol::LogEntry>>();
    for (auto i : {1, 2, 3})
    {
        bcos::h256s topics;
        for (auto j : {100, 200, 300})
        {
            topics.push_back(
                bcos::h256(bcos::asBytes("topic: " + boost::lexical_cast<std::string>(j))));
        }
        bcos::protocol::LogEntry entry(
            bcos::asBytes("Address: " + boost::lexical_cast<std::string>(i)), topics,
            bcos::asBytes("Data: " + boost::lexical_cast<std::string>(i)));
        logEntries->emplace_back(entry);
    }
    bcos::bytes output(bcos::asBytes("Output!"));

    for (size_t i = 0; i < 1000; ++i)
    {
        auto transaction = transactionFactory->createTransaction(
            117, to, input, nonce, i, "testChain", "testGroup", 1000);
        block->appendTransaction(transaction);
        block->appendTransactionHash(transaction->hash());

        auto receipt = transactionReceiptFactory->createReceipt(1000, contractAddress,
            std::make_shared<std::vector<bcos::protocol::LogEntry>>(*logEntries), 50, output, i);
        block->appendReceipt(receipt);
    }

    bcos::bytes buffer;
    BOOST_CHECK_NO_THROW(block->encode(buffer));

    auto decodedBlock = blockFactory.createBlock(buffer);

    BOOST_CHECK_EQUAL(block->blockHeader()->number(), decodedBlock->blockHeader()->number());
    BOOST_CHECK_EQUAL(block->blockHeader()->gasUsed(), decodedBlock->blockHeader()->gasUsed());
    BOOST_CHECK_EQUAL(block->blockHeader()->stateRoot(), decodedBlock->blockHeader()->stateRoot());
    BOOST_CHECK_EQUAL(block->blockHeader()->timestamp(), block->blockHeader()->timestamp());

    BOOST_CHECK_EQUAL(block->version(), decodedBlock->version());
    BOOST_CHECK_EQUAL(block->blockType(), decodedBlock->blockType());

    BOOST_CHECK_EQUAL(block->transactionsSize(), decodedBlock->transactionsSize());
    for (size_t i = 0; i < block->transactionsSize(); ++i)
    {
        auto lhs = block->transaction(i);
        auto rhs = decodedBlock->transaction(i);

        BOOST_CHECK_EQUAL(lhs->hash().hex(), rhs->hash().hex());
        BOOST_CHECK_EQUAL(lhs->version(), rhs->version());
        BOOST_CHECK_EQUAL(lhs->to().toString(), rhs->to().toString());
        BOOST_CHECK_EQUAL(bcos::asString(lhs->input()), bcos::asString(rhs->input()));

        BOOST_CHECK_EQUAL(lhs->nonce(), rhs->nonce());
        BOOST_CHECK_EQUAL(lhs->blockLimit(), rhs->blockLimit());
        BOOST_CHECK_EQUAL(lhs->chainId(), rhs->chainId());
        BOOST_CHECK_EQUAL(lhs->groupId(), rhs->groupId());
        BOOST_CHECK_EQUAL(lhs->importTime(), rhs->importTime());
    }

    BOOST_CHECK_EQUAL(block->receiptsSize(), decodedBlock->receiptsSize());
    for (size_t i = 0; i < block->receiptsSize(); ++i)
    {
        auto lhs = block->receipt(i);
        auto rhs = decodedBlock->receipt(i);

        BOOST_CHECK_EQUAL(lhs->hash().hex(), rhs->hash().hex());
        BOOST_CHECK_EQUAL(lhs->version(), rhs->version());
        BOOST_CHECK_EQUAL(lhs->gasUsed(), rhs->gasUsed());
        BOOST_CHECK_EQUAL(
            bcos::asString(lhs->contractAddress()), bcos::asString(rhs->contractAddress()));
        BOOST_CHECK_EQUAL(lhs->logEntries().size(), rhs->logEntries().size());
        for (auto i = 0; i < lhs->logEntries().size(); ++i)
        {
            BOOST_CHECK_EQUAL(lhs->logEntries()[i].address().toString(),
                rhs->logEntries()[i].address().toString());
            BOOST_CHECK_EQUAL(
                lhs->logEntries()[i].topics().size(), rhs->logEntries()[i].topics().size());
            for (auto j = 0; j < lhs->logEntries()[i].topics().size(); ++j)
            {
                BOOST_CHECK_EQUAL(
                    lhs->logEntries()[i].topics()[j].hex(), rhs->logEntries()[i].topics()[j].hex());
            }
            BOOST_CHECK_EQUAL(
                lhs->logEntries()[i].data().toString(), rhs->logEntries()[i].data().toString());
        }

        BOOST_CHECK_EQUAL(lhs->status(), rhs->status());
        BOOST_CHECK_EQUAL(bcos::asString(lhs->output()), bcos::asString(rhs->output()));
        BOOST_CHECK_EQUAL(lhs->blockNumber(), rhs->blockNumber());
    }
}

BOOST_AUTO_TEST_CASE(blockHeader)
{
    auto blockHeaderFactory =
        std::make_shared<bcostars::protocol::BlockHeaderFactoryImpl>(cryptoSuite);

    auto header = blockHeaderFactory->createBlockHeader();

    BOOST_CHECK_EQUAL(header->gasUsed(), bcos::u256(0));

    header->setNumber(100);
    header->setTimestamp(200);

    bcos::u256 gasUsed(1000);
    header->setGasUsed(gasUsed);

    bcos::protocol::ParentInfo parentInfo;
    parentInfo.blockHash = bcos::crypto::HashType(10000);
    parentInfo.blockNumber = 2000;

    std::vector<bcos::protocol::ParentInfo> parentInfoList;
    parentInfoList.emplace_back(parentInfo);

    header->setParentInfo(std::move(parentInfoList));

    for (auto flag : { false, true })
    {
        auto buffer = header->encode(flag);

        auto decodedHeader = blockHeaderFactory->createBlockHeader(buffer);

        BOOST_CHECK_EQUAL(header->number(), decodedHeader->number());
        BOOST_CHECK_EQUAL(header->timestamp(), decodedHeader->timestamp());
        BOOST_CHECK_EQUAL(header->gasUsed(), decodedHeader->gasUsed());
        BOOST_CHECK_EQUAL(header->parentInfo().size(), decodedHeader->parentInfo().size());
        for(size_t i = 0; i < decodedHeader->parentInfo().size(); ++i) {
            BOOST_CHECK_EQUAL(bcos::toString(header->parentInfo()[i].blockHash), bcos::toString(decodedHeader->parentInfo()[i].blockHash));
            BOOST_CHECK_EQUAL(header->parentInfo()[i].blockNumber, decodedHeader->parentInfo()[i].blockNumber);
        }
    }

    BOOST_CHECK_NO_THROW(header->setExtraData(header->extraData().toBytes()));
}

BOOST_AUTO_TEST_CASE(emptyBlockHeader)
{
    auto blockHeaderFactory =
        std::make_shared<bcostars::protocol::BlockHeaderFactoryImpl>(cryptoSuite);
    auto transactionFactory =
        std::make_shared<bcostars::protocol::TransactionFactoryImpl>(cryptoSuite);
    auto transactionReceiptFactory =
        std::make_shared<bcostars::protocol::TransactionReceiptFactoryImpl>(cryptoSuite);
    bcostars::protocol::BlockFactoryImpl blockFactory(
        cryptoSuite, blockHeaderFactory, transactionFactory, transactionReceiptFactory);

    auto block = blockFactory.createBlock();

    BOOST_CHECK_NO_THROW(block->setBlockHeader(nullptr));
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace test
}  // namespace bcostars