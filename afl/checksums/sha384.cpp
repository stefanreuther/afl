/**
  *  \file afl/checksums/sha384.cpp
  *  \brief Class afl::checksums::SHA384
  */

#include "afl/checksums/sha384.hpp"
#include "afl/bits/uint64be.hpp"
#include "afl/bits/pack.hpp"

namespace {
    const uint64_t INIT[] = {
        0xcbbb9d5dc1059ed8ULL,
        0x629a292a367cd507ULL,
        0x9159015a3070dd17ULL,
        0x152fecd8f70e5939ULL,
        0x67332667ffc00b31ULL,
        0x8eb44a8768581511ULL,
        0xdb0c2e0d64f98fa7ULL,
        0x47b5481dbefa4fa4ULL,
    };
}

const size_t afl::checksums::SHA384::HASH_SIZE;

afl::checksums::SHA384::SHA384()
{
    SHA384::clear();
}

afl::checksums::SHA384::~SHA384()
{ }

void
afl::checksums::SHA384::clear()
{
    m_state.init(INIT);
}

void
afl::checksums::SHA384::add(ConstBytes_t data)
{
    m_state.add(data.unsafeData(), data.size());
}

size_t
afl::checksums::SHA384::getHashSize() const
{
    return HASH_SIZE;
}

size_t
afl::checksums::SHA384::getBlockSize() const
{
    return sizeof(SHA2_64::Block_t);
}

afl::checksums::Hash::Bytes_t
afl::checksums::SHA384::getHash(Bytes_t data) const
{
    SHA2Core<SHA2_64> copy(m_state);
    copy.pad();

    afl::bits::packArray<afl::bits::UInt64BE>(data.trim(HASH_SIZE), copy.getState());
    return data;
}
