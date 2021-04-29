#include "Transaction.h"
#include "bcos-framework/libutilities/DataConvertUtility.h"
// #define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>
#include "../protocols/TransactionTars.h"

namespace bcos {
namespace test {
BOOST_AUTO_TEST_CASE(TransactionTars) {
    bcostars::protocol::Transaction transaction;
    bcostars::Transaction innerTransaction;
    std::shared_ptr<bcostars::Transaction> ptr(&innerTransaction, [](bcostars::Transaction*){});

    bcos::bytes to(bcos::asBytes("Target"));
    bcos::bytes input(bcos::asBytes("Arguments"));
    bcos::u256 nonce(100);

    bcostars::protocol::TransactionFactory factory;
    auto tx = factory.createTransaction(0, to, input, nonce, 100, "testChain", "testGroup", 1000);
    
    bcos::bytes buffer;
    tx->encode(buffer);

    auto decodedTx = factory.createTransaction(buffer, false);

    // BOOST_CHECK(*tx == *decodedTx);

    transaction.setTransaction(ptr);
}
} // namespace test
} // namespace bcos