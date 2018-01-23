/**
  *  \file afl/checksums/sha224.cpp
  *  \brief Class afl::checksums::SHA224
  */

#include "afl/checksums/sha224.hpp"
#include "afl/bits/uint32be.hpp"
#include "afl/bits/pack.hpp"

namespace {
    const uint32_t INIT[] = {
        0xc1059ed8,
        0x367cd507,
        0x3070dd17,
        0xf70e5939,
        0xffc00b31,
        0x68581511,
        0x64f98fa7,
        0xbefa4fa4,
    };
}

const size_t afl::checksums::SHA224::HASH_SIZE;

afl::checksums::SHA224::SHA224()
{
    SHA224::clear();
}

afl::checksums::SHA224::~SHA224()
{ }

void
afl::checksums::SHA224::clear()
{
    m_state.init(INIT);
}

void
afl::checksums::SHA224::add(ConstBytes_t data)
{
    m_state.add(data.unsafeData(), data.size());
}

size_t
afl::checksums::SHA224::getHashSize() const
{
    return HASH_SIZE;
}

size_t
afl::checksums::SHA224::getBlockSize() const
{
    return sizeof(SHA2_32::Block_t);
}

afl::checksums::Hash::Bytes_t
afl::checksums::SHA224::getHash(Bytes_t data) const
{
    SHA2Core<SHA2_32> copy(m_state);
    copy.pad();

    afl::bits::packArray<afl::bits::UInt32BE>(data.trim(HASH_SIZE), copy.getState());
    return data;
}
