/**
  *  \file afl/checksums/sha512.cpp
  *  \brief Class afl::checksums::SHA512
  */

#include "afl/checksums/sha512.hpp"
#include "afl/bits/uint64be.hpp"
#include "afl/bits/pack.hpp"

namespace {
    const uint64_t INIT[] = {
        0x6a09e667f3bcc908ULL,
        0xbb67ae8584caa73bULL,
        0x3c6ef372fe94f82bULL,
        0xa54ff53a5f1d36f1ULL,
        0x510e527fade682d1ULL,
        0x9b05688c2b3e6c1fULL,
        0x1f83d9abfb41bd6bULL,
        0x5be0cd19137e2179ULL,
    };
}

const size_t afl::checksums::SHA512::HASH_SIZE;

afl::checksums::SHA512::SHA512()
{
    SHA512::clear();
}

afl::checksums::SHA512::~SHA512()
{ }

void
afl::checksums::SHA512::clear()
{
    m_state.init(INIT);
}

void
afl::checksums::SHA512::add(ConstBytes_t data)
{
    m_state.add(data.unsafeData(), data.size());
}

size_t
afl::checksums::SHA512::getHashSize() const
{
    return HASH_SIZE;
}

size_t
afl::checksums::SHA512::getBlockSize() const
{
    return sizeof(SHA2_64::Block_t);
}

afl::checksums::Hash::Bytes_t
afl::checksums::SHA512::getHash(Bytes_t data) const
{
    SHA2Core<SHA2_64> copy(m_state);
    copy.pad();

    afl::bits::packArray<afl::bits::UInt64BE>(data.trim(HASH_SIZE), copy.getState());
    return data;
}
