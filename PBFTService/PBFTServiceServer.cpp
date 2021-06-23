#include "PBFTServiceServer.h"

using namespace bcostars;

std::once_flag PBFTServiceServer::m_pbftFlag;
bcos::sealer::SealerInterface::Ptr PBFTServiceServer::m_sealer;
bcos::crypto::CryptoSuite::Ptr PBFTServiceServer::m_cryptoSuite;