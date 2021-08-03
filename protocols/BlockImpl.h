#pragma once

#include "Block.h"
#include "BlockHeaderImpl.h"
#include "Common.h"
#include "Transaction.h"
#include "TransactionImpl.h"
#include "TransactionReceipt.h"
#include "TransactionReceiptImpl.h"
#include "bcos-framework/interfaces/crypto/CryptoSuite.h"
#include "bcos-framework/interfaces/protocol/TransactionReceiptFactory.h"
#include <bcos-framework/interfaces/protocol/Block.h>
#include <bcos-framework/interfaces/protocol/BlockFactory.h>
#include <bcos-framework/interfaces/protocol/BlockHeader.h>
#include <bcos-framework/interfaces/protocol/BlockHeaderFactory.h>
#include <gsl/span>
#include <memory>

namespace bcostars
{
namespace protocol
{
class BlockImpl : public bcos::protocol::Block, public std::enable_shared_from_this<BlockImpl>
{
public:
    BlockImpl(bcos::protocol::TransactionFactory::Ptr _transactionFactory,
        bcos::protocol::TransactionReceiptFactory::Ptr _receiptFactory)
      : bcos::protocol::Block(_transactionFactory, _receiptFactory),
        m_inner(std::make_shared<bcostars::Block>())
    {
        m_blockHeader = std::make_shared<bcostars::protocol::BlockHeaderImpl>(
            m_transactionFactory->cryptoSuite(),
            [m_inner = this->m_inner]() mutable { return &m_inner->blockHeader; });
    }

    ~BlockImpl() override{};

    void decode(bcos::bytesConstRef _data, bool _calculateHash, bool _checkSig) override
    {
        tars::TarsInputStream<tars::BufferReader> input;
        input.setBuffer((const char*)_data.data(), _data.size());

        m_inner->readFrom(input);
    }

    void encode(bcos::bytes& _encodeData) const override
    {
        tars::TarsOutputStream<bcostars::protocol::BufferWriterByteVector> output;

        m_inner->writeTo(output);
        output.getByteBuffer().swap(_encodeData);
    }

    int32_t version() const override { return m_inner->blockHeader.version; }
    void setVersion(int32_t _version) override { m_inner->blockHeader.version = _version; }

    bcos::protocol::BlockType blockType() const override
    {
        return (bcos::protocol::BlockType)m_inner->type;
    }

    bcos::protocol::BlockHeader::Ptr blockHeader() override { return m_blockHeader; };

    bcos::protocol::Transaction::ConstPtr transaction(size_t _index) const override
    {
        return std::make_shared<bcostars::protocol::TransactionImpl>(
            m_transactionFactory->cryptoSuite(),
            [m_inner = this->m_inner, _index]() { return &(m_inner->transactions[_index]); });
    }

    bcos::protocol::TransactionReceipt::ConstPtr receipt(size_t _index) const override
    {
        return std::make_shared<bcostars::protocol::TransactionReceiptImpl>(
            m_transactionFactory->cryptoSuite(),
            [m_inner = this->m_inner, _index]() { return &(m_inner->receipts[_index]); });
    };

    bcos::crypto::HashType const& transactionHash(size_t _index) const override
    {
        return *(reinterpret_cast<const bcos::crypto::HashType*>(
            m_inner->transactionsHash[_index].data()));
    }

    void setBlockType(bcos::protocol::BlockType _blockType) override
    {
        m_inner->type = (int32_t)_blockType;
    }

    // set blockHeader
    void setBlockHeader(bcos::protocol::BlockHeader::Ptr _blockHeader) override
    {
        if (_blockHeader)
        {
            m_inner->blockHeader =
                std::dynamic_pointer_cast<bcostars::protocol::BlockHeaderImpl>(_blockHeader)
                    ->inner();
        }
    }

    void setTransaction(size_t _index, bcos::protocol::Transaction::Ptr _transaction) override
    {
        m_inner->transactions[_index] =
            std::dynamic_pointer_cast<bcostars::protocol::TransactionImpl>(_transaction)->inner();
    }
    void appendTransaction(bcos::protocol::Transaction::Ptr _transaction) override
    {
        m_inner->transactions.emplace_back(
            std::dynamic_pointer_cast<bcostars::protocol::TransactionImpl>(_transaction)->inner());
    }

    void setReceipt(size_t _index, bcos::protocol::TransactionReceipt::Ptr _receipt) override
    {
        if (_index >= m_inner->receipts.size())
        {
            m_inner->receipts.resize(m_inner->transactions.size());
        }
        auto innerReceipt =
            std::dynamic_pointer_cast<bcostars::protocol::TransactionReceiptImpl>(_receipt)
                ->inner();
        m_inner->receipts[_index] = innerReceipt;
    }
    void appendReceipt(bcos::protocol::TransactionReceipt::Ptr _receipt) override
    {
        m_inner->receipts.push_back(
            std::dynamic_pointer_cast<bcostars::protocol::TransactionReceiptImpl>(_receipt)
                ->inner());
    }

    void appendTransactionHash(bcos::crypto::HashType const& _txHash) override
    {
        m_inner->transactionsHash.emplace_back(_txHash.begin(), _txHash.end());
    }

    // get transactions size
    size_t transactionsSize() const override { return m_inner->transactions.size(); }
    size_t transactionsHashSize() const override { return m_inner->transactionsHash.size(); }
    // get receipts size
    size_t receiptsSize() const override { return m_inner->receipts.size(); }

    void setNonceList(bcos::protocol::NonceList const& _nonceList) override
    {
        m_inner->nonceList.clear();
        m_inner->nonceList.reserve(_nonceList.size());
        for (auto const& it : _nonceList)
        {
            m_inner->nonceList.emplace_back(boost::lexical_cast<std::string>(it));
        }

        m_nonceList.clear();
    }
    void setNonceList(bcos::protocol::NonceList&& _nonceList) override
    {
        m_inner->nonceList.clear();
        m_inner->nonceList.reserve(_nonceList.size());
        for (auto const& it : _nonceList)
        {
            m_inner->nonceList.emplace_back(boost::lexical_cast<std::string>(it));
        }

        m_nonceList.clear();
    }
    bcos::protocol::NonceList const& nonceList() const override
    {
        if (m_nonceList.empty())
        {
            m_nonceList.reserve(m_inner->nonceList.size());

            for (auto const& it : m_inner->nonceList)
            {
                if (it.empty())
                {
                    m_nonceList.push_back(bcos::protocol::NonceType(0));
                }
                else
                {
                    m_nonceList.push_back(boost::lexical_cast<bcos::u256>(it));
                }
            }
        }

        return m_nonceList;
    }

    const bcostars::Block& inner() const { return *m_inner; }
    void setInner(const bcostars::Block& inner) { *m_inner = inner; }
    void setInner(bcostars::Block&& inner) { *m_inner = std::move(inner); }

private:
    std::shared_ptr<bcostars::Block> m_inner;
    std::shared_ptr<BlockHeaderImpl> m_blockHeader;
    mutable bcos::protocol::NonceList m_nonceList;
};

class BlockFactoryImpl : public bcos::protocol::BlockFactory
{
public:
    BlockFactoryImpl(bcos::crypto::CryptoSuite::Ptr cryptoSuite,
        bcos::protocol::BlockHeaderFactory::Ptr blockHeaderFactory,
        bcos::protocol::TransactionFactory::Ptr transactionFactory,
        bcos::protocol::TransactionReceiptFactory::Ptr receiptFactory)
      : m_cryptoSuite(cryptoSuite),
        m_blockHeaderFactory(blockHeaderFactory),
        m_transactionFactory(transactionFactory),
        m_receiptFactory(receiptFactory){};

    ~BlockFactoryImpl() override {}
    bcos::protocol::Block::Ptr createBlock() override
    {
        return std::make_shared<BlockImpl>(m_transactionFactory, m_receiptFactory);
    }

    bcos::protocol::Block::Ptr createBlock(
        bcos::bytes const& _data, bool _calculateHash = true, bool _checkSig = true) override
    {
        return createBlock(bcos::ref(_data), _calculateHash, _checkSig);
    }

    bcos::protocol::Block::Ptr createBlock(
        bcos::bytesConstRef _data, bool _calculateHash = true, bool _checkSig = true) override
    {
        auto block = std::make_shared<BlockImpl>(m_transactionFactory, m_receiptFactory);
        block->decode(_data, _calculateHash, _checkSig);

        return block;
    }

    bcos::crypto::CryptoSuite::Ptr cryptoSuite() override { return m_cryptoSuite; }
    bcos::protocol::BlockHeaderFactory::Ptr blockHeaderFactory() override
    {
        return m_blockHeaderFactory;
    }
    bcos::protocol::TransactionFactory::Ptr transactionFactory() override
    {
        return m_transactionFactory;
    }
    bcos::protocol::TransactionReceiptFactory::Ptr receiptFactory() override
    {
        return m_receiptFactory;
    }

private:
    bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
    bcos::protocol::BlockHeaderFactory::Ptr m_blockHeaderFactory;
    bcos::protocol::TransactionFactory::Ptr m_transactionFactory;
    bcos::protocol::TransactionReceiptFactory::Ptr m_receiptFactory;
};

}  // namespace protocol
}  // namespace bcostars