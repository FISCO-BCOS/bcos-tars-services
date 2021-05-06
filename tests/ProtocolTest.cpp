#include "CryptoTest.h"
#include "Transaction.h"
#include "bcos-framework/interfaces/crypto/CryptoSuite.h"
#include "bcos-framework/libutilities/DataConvertUtility.h"
#include <boost/test/tools/old/interface.hpp>

#define BOOST_TEST_MAIN

#include "../protocols/TransactionTars.h"
#include <boost/test/unit_test.hpp>

namespace bcostars {
namespace test {

struct Fixture {
  Fixture() {
    cryptoSuite = std::make_shared<bcos::crypto::CryptoSuite>(bcos::crypto::Hash::Ptr(new TarsHash()),
                                                              nullptr, nullptr);
  }

  bcos::crypto::CryptoSuite::Ptr cryptoSuite;
};

BOOST_FIXTURE_TEST_SUITE(f, Fixture)

BOOST_AUTO_TEST_CASE(transaction) {
  bcos::bytes to(bcos::asBytes("Target"));
  bcos::bytes input(bcos::asBytes("Arguments"));
  bcos::u256 nonce(100);

  bcostars::protocol::TransactionFactory factory;
  factory.setCryptoSuite(cryptoSuite);
  auto tx = factory.createTransaction(0, to, input, nonce, 100, "testChain",
                                      "testGroup", 1000);

  bcos::bytes buffer;
  tx->encode(buffer);

  auto decodedTx = factory.createTransaction(buffer, false);

  BOOST_CHECK_EQUAL(tx->hash(), decodedTx->hash());
  BOOST_CHECK_EQUAL(tx->version(), 0);
  BOOST_CHECK_EQUAL(bcos::asString(tx->to()), bcos::asString(to));
  BOOST_CHECK_EQUAL(bcos::asString(tx->input()), bcos::asString(input));
  // BOOST_CHECK_EQUAL(tx->nonce(), nonce);
  BOOST_CHECK_EQUAL(tx->blockLimit(), 100);
  BOOST_CHECK_EQUAL(tx->chainId(), "testChain");
  BOOST_CHECK_EQUAL(tx->groupId(), "testGroup");
  BOOST_CHECK_EQUAL(tx->importTime(), 1000);
}

BOOST_AUTO_TEST_CASE(block) {

}

BOOST_AUTO_TEST_SUITE_END()

} // namespace test
} // namespace bcostars