#pragma once

#include "ExecutionResult.h"
#include "TransactionReceiptImpl.h"
#include "bcos-framework/interfaces/protocol/ExecutionResult.h"
#include "libutilities/Common.h"

namespace bcostars
{
namespace protocol
{
class ExecutionResultImpl : public bcos::protocol::ExecutionResult
{
public:
    ~ExecutionResultImpl() override {}

    Status status() const noexcept override { return (Status)m_inner->status; }

    bcos::protocol::TransactionReceipt::ConstPtr receipt() const noexcept override
    {
        std::shared_ptr<const bcostars::protocol::TransactionReceiptImpl> receipt =
            std::make_shared<const TransactionReceiptImpl>(
                m_cryptoSuite, [m_inner = this->m_inner]() { return &m_inner->receipt; });

        return receipt;
    }

    bcos::bytesConstRef to() const noexcept override
    {
        return bcos::bytesConstRef((bcos::byte*)m_inner->to.data(), m_inner->to.size());
    }

private:
    std::shared_ptr<bcostars::ExecutionResult> m_inner;
    bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};
}  // namespace protocol
}  // namespace bcostars