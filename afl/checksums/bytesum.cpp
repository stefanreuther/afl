/**
  *  \file afl/checksums/bytesum.cpp
  *  \brief Class afl::checksums::ByteSum
  */

#include "afl/checksums/bytesum.hpp"

afl::checksums::ByteSum::ByteSum()
{ }

afl::checksums::ByteSum::~ByteSum()
{ }

uint32_t
afl::checksums::ByteSum::add(Memory_t data, uint32_t prev) const
{
#if 1
    const uint8_t* p = data.unsafeData();
    size_t n = data.size();
    while (n > 0) {
        prev += *p;
        ++p, --n;
    }
#else
    while (const uint8_t* p = data.eat()) {
        prev += *p;
    }
#endif
    return prev;
}

size_t
afl::checksums::ByteSum::bits() const
{
    return 32;
}

uint32_t
afl::checksums::ByteSum::addImpl(Memory_t data, uint32_t prev) const
{
    return add(data, prev);
}

size_t
afl::checksums::ByteSum::bitsImpl() const
{
    return bits();
}
