#pragma once

#include "bcos-framework/interfaces/crypto/CommonType.h"
#include "bcos-framework/interfaces/crypto/CryptoSuite.h"
#include "bcos-framework/interfaces/crypto/Hash.h"
#include <openssl/sha.h>

namespace bcostars {
namespace test {
class TarsHash : public bcos::crypto::Hash {
public:
  ~TarsHash() override {}
  bcos::crypto::HashType hash(bcos::bytesConstRef _data) override {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, _data.data(), _data.size());
    SHA256_Final(hash, &sha256);
    return bcos::crypto::HashType(hash, sizeof(hash));
  }

  bcos::crypto::HashType emptyHash() override {
    return bcos::crypto::HashType();
  }

private:
};
} // namespace test
} // namespace bcostars