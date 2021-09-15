#include "../StorageService/ProtocolConverter.h"
#include <bcos-framework/testutils/crypto/HashImpl.h>
#include <bcos-framework/testutils/crypto/SignatureImpl.h>
#include <boost/test/unit_test.hpp>

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

BOOST_FIXTURE_TEST_SUITE(TestStorageService, Fixture)

BOOST_AUTO_TEST_CASE(TableInfo)
{
    bcostars::TableInfo tarsTableInfo;
    tarsTableInfo._key = "a";
    tarsTableInfo.name = "b";
    for (int i = 0; i < 100; ++i)
    {
        tarsTableInfo.fields.push_back(boost::lexical_cast<std::string>(i));
    }

    auto bcosTableInfo = toBcosTableInfo(tarsTableInfo);

    BOOST_CHECK_EQUAL(bcosTableInfo->key, tarsTableInfo._key);
    BOOST_CHECK_EQUAL(bcosTableInfo->name, tarsTableInfo.name);
    BOOST_CHECK_EQUAL(bcosTableInfo->fields.size(), tarsTableInfo.fields.size());
    for (size_t i = 0; i < 100; ++i)
    {
        BOOST_CHECK_EQUAL(bcosTableInfo->fields[i], tarsTableInfo.fields[i]);
    }
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace test
}  // namespace bcostars