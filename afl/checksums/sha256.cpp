/**
  *  \file afl/checksums/sha256.cpp
  *  \brief Class afl::checksums::SHA256
  */

#include "afl/checksums/sha256.hpp"
#include "afl/bits/uint32be.hpp"
#include "afl/bits/pack.hpp"

namespace {
    const uint32_t INIT[] = {
        0x6a09e667,
        0xbb67ae85,
        0x3c6ef372,
        0xa54ff53a,
        0x510e527f,
        0x9b05688c,
        0x1f83d9ab,
        0x5be0cd19,
    };
}

const size_t afl::checksums::SHA256::HASH_SIZE;

afl::checksums::SHA256::SHA256()
{
    SHA256::clear();
}

afl::checksums::SHA256::~SHA256()
{ }

void
afl::checksums::SHA256::clear()
{
    m_state.init(INIT);
}

void
afl::checksums::SHA256::add(ConstBytes_t data)
{
    m_state.add(data.unsafeData(), data.size());
}

size_t
afl::checksums::SHA256::getHashSize() const
{
    return HASH_SIZE;
}

size_t
afl::checksums::SHA256::getBlockSize() const
{
    return sizeof(SHA2_32::Block_t);
}

afl::checksums::Hash::Bytes_t
afl::checksums::SHA256::getHash(Bytes_t data) const
{
    SHA2Core<SHA2_32> copy(m_state);
    copy.pad();

    afl::bits::packArray<afl::bits::UInt32BE>(data.trim(HASH_SIZE), copy.getState());
    return data;
}
