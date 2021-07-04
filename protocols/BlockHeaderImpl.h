#pragma once

#include "Block.h"
#include "Common.h"
#include "bcos-framework/interfaces/crypto/CryptoSuite.h"
#include <bcos-framework/interfaces/protocol/Block.h>
#include <bcos-framework/interfaces/protocol/BlockFactory.h>
#include <bcos-framework/interfaces/protocol/BlockHeader.h>
#include <bcos-framework/interfaces/protocol/BlockHeaderFactory.h>
#include <gsl/span>

namespace bcostars
{
namespace protocol
{
class BlockHeaderImpl : public bcos::protocol::BlockHeader
{
public:
    virtual ~BlockHeaderImpl() {}

    BlockHeaderImpl(bcos::crypto::CryptoSuite::Ptr cryptoSuite)
      : bcos::protocol::BlockHeader(cryptoSuite), m_inner(std::make_shared<bcostars::BlockHeader>())
    {}

    virtual void decode(bcos::bytesConstRef _data) override
    {
        m_buffer.assign(_data.begin(), _data.end());

        tars::TarsInputStream<tars::BufferReader> input;
        input.setBuffer((const char*)_data.data(), _data.size());

        m_inner->readFrom(input);
    }

    virtual void encode(bcos::bytes& _encodeData) const override
    {
        tars::TarsOutputStream<bcostars::protocol::BufferWriterByteVector> output;

        m_inner->writeTo(output);
        output.getByteBuffer().swap(_encodeData);
    }

    bcos::bytesConstRef encode(bool _onlyHashFieldsData = false) const override
    {
        if (_onlyHashFieldsData)
        {
            vector<Signature> emptyList;
            m_inner->signatureList.swap(emptyList);
            encode(m_buffer);
            emptyList.swap(m_inner->signatureList);
        }
        else
        {
            encode(m_buffer);
        }
        return bcos::ref(m_buffer);
    }

    void clear() override { m_inner = std::make_shared<bcostars::BlockHeader>(); }

    int32_t version() const override { return m_inner->version; }

    gsl::span<const bcos::protocol::ParentInfo> parentInfo() const override {}

    bcos::crypto::HashType const& txsRoot() const override
    {
        return *(reinterpret_cast<bcos::crypto::HashType*>(m_inner->txsRoot.data()));
    }
    bcos::crypto::HashType const& stateRoot() const override
    {
        return *(reinterpret_cast<const bcos::crypto::HashType*>(m_inner->stateRoot.data()));
    }
    bcos::crypto::HashType const& receiptsRoot() const override
    {
        return *(reinterpret_cast<const bcos::crypto::HashType*>(m_inner->receiptRoot.data()));
    }
    bcos::protocol::BlockNumber number() const override { return m_inner->blockNumber; }
    bcos::u256 const& gasUsed() override { return *((bcos::u256*)(m_inner->gasUsed.data())); }
    int64_t timestamp() override { return m_inner->timestamp; }
    int64_t sealer() override { return m_inner->sealer; }
    gsl::span<const bcos::bytes> sealerList() const override { return m_inner->sealerList; }
    bcos::bytesConstRef extraData() const override
    {
        return bcos::bytesConstRef(m_inner->extraData.data(), m_inner->extraData.size());
    }
    gsl::span<const bcos::protocol::Signature> signatureList() const override
    {
        return gsl::span(
            reinterpret_cast<const bcos::protocol::Signature*>(m_inner->signatureList.data()),
            m_inner->signatureList.size());
    }

    gsl::span<const uint64_t> consensusWeights() const override
    {
        return gsl::span(reinterpret_cast<uint64_t*>(m_inner->consensusWeights.data()),
            m_inner->consensusWeights.size());
    }

    void setVersion(int32_t _version) override { m_inner->version = _version; }

    void setParentInfo(gsl::span<const bcos::protocol::ParentInfo> const& _parentInfo) override
    {
        m_inner->parentInfo.clear();
        for (auto& it : _parentInfo)
        {
            ParentInfo parentInfo;
            parentInfo.blockNumber = it.blockNumber;
            *parentInfo.blockHash = *it.blockHash.data();
            m_inner->parentInfo.emplace_back(parentInfo);
        }
    }

    void setParentInfo(bcos::protocol::ParentInfoList&& _parentInfo) override
    {
        setParentInfo(gsl::span(_parentInfo.data(), _parentInfo.size()));
    }

    void setTxsRoot(bcos::crypto::HashType const& _txsRoot) override
    {
        m_inner->txsRoot.assign(_txsRoot.begin(), _txsRoot.end());
    }
    void setReceiptsRoot(bcos::crypto::HashType const& _receiptsRoot) override
    {
        m_inner->receiptRoot.assign(_receiptsRoot.begin(), _receiptsRoot.end());
    }
    void setStateRoot(bcos::crypto::HashType const& _stateRoot) override
    {
        m_inner->stateRoot.assign(_stateRoot.begin(), _stateRoot.end());
    }
    void setNumber(bcos::protocol::BlockNumber _blockNumber) override
    {
        m_inner->blockNumber = _blockNumber;
    }
    void setGasUsed(bcos::u256 const& _gasUsed) override {}
    void setTimestamp(int64_t _timestamp) override { m_inner->timestamp = _timestamp; }
    void setSealer(int64_t _sealerId) override { m_inner->sealer = _sealerId; }
    void setSealerList(gsl::span<const bcos::bytes> const& _sealerList) override
    {
        m_inner->sealerList.assign(_sealerList.begin(), _sealerList.end());
    }

    void setSealerList(std::vector<bcos::bytes>&& _sealerList) override
    {
        setSealerList(gsl::span(_sealerList.data(), _sealerList.size()));
    }

    void setConsensusWeights(gsl::span<const uint64_t> const& _weightList) override
    {
        m_inner->consensusWeights.assign(_weightList.begin(), _weightList.end());
    }

    void setConsensusWeights(std::vector<uint64_t>&& _weightList) override
    {
        setConsensusWeights(gsl::span(_weightList.data(), _weightList.size()));
    }

    void setExtraData(bcos::bytes const& _extraData) override { m_inner->extraData = _extraData; }
    void setExtraData(bcos::bytes&& _extraData) override { _extraData.swap(m_inner->extraData); }
    void setSignatureList(gsl::span<const bcos::protocol::Signature> const& _signatureList) override
    {
        for (auto& it : _signatureList)
        {
            Signature signature;
            signature.sealerIndex = it.index;
            signature.signature = it.signature;
            m_inner->signatureList.emplace_back(signature);
        }
    }

    void setSignatureList(bcos::protocol::SignatureList&& _signatureList) override
    {
        setSignatureList(gsl::span(_signatureList.data(), _signatureList.size()));
    }

    const bcostars::BlockHeader& inner() const { return *m_inner; }

    void setInner(const bcostars::BlockHeader& blockHeader) { *m_inner = blockHeader; }
    void setInner(const bcostars::BlockHeader&& blockHeader) { *m_inner = std::move(blockHeader); }

private:
    std::shared_ptr<bcostars::BlockHeader> m_inner;
    mutable bcos::bytes m_buffer;
};

class BlockHeaderFactoryImpl : public bcos::protocol::BlockHeaderFactory
{
public:
    BlockHeaderFactoryImpl(bcos::crypto::CryptoSuite::Ptr cryptoSuite) : m_cryptoSuite(cryptoSuite)
    {}
    ~BlockHeaderFactoryImpl() override {}
    bcos::protocol::BlockHeader::Ptr createBlockHeader() override
    {
        return std::make_shared<bcostars::protocol::BlockHeaderImpl>(m_cryptoSuite);
    }
    bcos::protocol::BlockHeader::Ptr createBlockHeader(bcos::bytes const& _data) override
    {
        return createBlockHeader(bcos::ref(_data));
    }
    bcos::protocol::BlockHeader::Ptr createBlockHeader(bcos::bytesConstRef _data) override
    {
        auto blockHeader = std::make_shared<BlockHeaderImpl>(m_cryptoSuite);
        blockHeader->decode(_data);

        return blockHeader;
    }
    virtual bcos::protocol::BlockHeader::Ptr createBlockHeader(
        bcos::protocol::BlockNumber _number) override
    {
        auto blockHeader = std::make_shared<BlockHeaderImpl>(m_cryptoSuite);
        blockHeader->setNumber(_number);

        return blockHeader;
    }

private:
    bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};

}  // namespace protocol
}  // namespace bcostars