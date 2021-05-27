#include "Transaction.h"
#include "bcos-framework/interfaces/crypto/CommonType.h"
#include "bcos-framework/interfaces/crypto/CryptoSuite.h"
#include "bcos-framework/libprotocol/LogEntry.h"
#include "bcos-framework/libutilities/DataConvertUtility.h"
#include "bcos-framework/testutils/crypto/SignatureImpl.h"
#include "bcos-framework/testutils/crypto/HashImpl.h"
#include "../protocols/TransactionImpl.h"
#include "../protocols/TransactionReceiptImpl.h"
#include <boost/test/unit_test.hpp>
#include <memory>

namespace bcostars {
namespace test {

struct Fixture {
  Fixture() {
    cryptoSuite = std::make_shared<bcos::crypto::CryptoSuite>(
        std::make_shared<bcos::test::Sm3Hash>(), std::make_shared<bcos::test::SM2SignatureImpl>(), nullptr);
  }

  bcos::crypto::CryptoSuite::Ptr cryptoSuite;
};

BOOST_FIXTURE_TEST_SUITE(TestProtocol, Fixture)

BOOST_AUTO_TEST_CASE(transaction) {
  bcos::bytes to(bcos::asBytes("Target"));
  bcos::bytes input(bcos::asBytes("Arguments"));
  bcos::u256 nonce(100);

  bcostars::protocol::TransactionFactory factory;
  factory.setCryptoSuite(cryptoSuite);
  auto tx = factory.createTransaction(0, to, input, nonce, 100, "testChain",
                                      "testGroup", 1000);

  auto signature = cryptoSuite->signatureImpl()->sign(cryptoSuite->signatureImpl()->generateKeyPair(), tx->hash(), true);
  std::dynamic_pointer_cast<bcostars::protocol::Transaction>(tx)->setSignatureData(*signature);

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

BOOST_AUTO_TEST_CASE(transactionReceipt) {
  bcos::crypto::HashType stateRoot(bcos::asBytes("root1"));
  bcos::u256 gasUsed(8858);
  bcos::bytes contractAddress(bcos::asBytes("contract Address!"));

  auto logEntries = std::make_shared<std::vector<bcos::protocol::LogEntry>>();
  for (auto i : {1, 2, 3}) {
    bcos::h256s topics;
    for (auto j : {100, 200, 300}) {
      topics.push_back(bcos::h256(
          bcos::asBytes("topic: " + boost::lexical_cast<std::string>(j))));
    }
    bcos::protocol::LogEntry entry(
        bcos::asBytes("Address: " + boost::lexical_cast<std::string>(i)),
        topics, bcos::asBytes("Data: " + boost::lexical_cast<std::string>(i)));
    logEntries->emplace_back(entry);
  }
  bcos::bytes output(bcos::asBytes("Output!"));

  bcostars::protocol::TransactionReceiptFactory factory;
  factory.setCryptoSuite(cryptoSuite);
  auto receipt = factory.createReceipt(1000, stateRoot, gasUsed, contractAddress, std::make_shared<std::vector<bcos::protocol::LogEntry>>(*logEntries), 50, output, 888);

  bcos::bytes buffer;
  receipt->encode(buffer);

  auto decodedReceipt = factory.createReceipt(buffer);

  BOOST_CHECK_EQUAL(receipt->hash().hex(), decodedReceipt->hash().hex());
  BOOST_CHECK_EQUAL(receipt->version(), 1000);
  BOOST_CHECK_EQUAL(receipt->gasUsed(), gasUsed);
  BOOST_CHECK_EQUAL(bcos::asString(receipt->contractAddress()), bcos::asString(contractAddress));
  BOOST_CHECK_EQUAL(receipt->logEntries().size(), logEntries->size());
  for(auto i = 0; i < receipt->logEntries().size(); ++i) {
    BOOST_CHECK_EQUAL(receipt->logEntries()[i].address().toString(), (*logEntries)[i].address().toString());
    BOOST_CHECK_EQUAL(receipt->logEntries()[i].topics().size(), (*logEntries)[i].topics().size());
    for(auto j = 0; j < receipt->logEntries()[i].topics().size(); ++j) {
      BOOST_CHECK_EQUAL(receipt->logEntries()[i].topics()[j].hex(), (*logEntries)[i].topics()[j].hex());
    }
    BOOST_CHECK_EQUAL(receipt->logEntries()[i].data().toString(), (*logEntries)[i].data().toString());
  }

  BOOST_CHECK_EQUAL(receipt->status(), 50);
  BOOST_CHECK_EQUAL(bcos::asString(receipt->output()), bcos::asString(output));
  BOOST_CHECK_EQUAL(receipt->blockNumber(), 888);
}

BOOST_AUTO_TEST_CASE(block) {

}

BOOST_AUTO_TEST_CASE(blockHeader) {

}

BOOST_AUTO_TEST_SUITE_END()

} // namespace test
} // namespace bcostars