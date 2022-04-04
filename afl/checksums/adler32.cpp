/**
  *  \file afl/checksums/adler32.cpp
  *  \brief Class afl::checksums::Adler32
  */

#include "afl/checksums/adler32.hpp"

namespace {
    const uint32_t BASE = 65521;
}

afl::checksums::Adler32::Adler32()
{ }

afl::checksums::Adler32::~Adler32()
{ }

uint32_t
afl::checksums::Adler32::add(Memory_t data, uint32_t prev) const
{
    uint32_t s1 = (prev & 0xFFFF);
    uint32_t s2 = (prev >> 16);
    uint32_t counter = 0;
    while (const uint8_t* p = data.eat()) {
        // The most expensive operation in Adler32 is the division which should normally be done after each byte.
        // Since we're effectively computing everything modulo BASE, we can defer division as long as we don't overflow.
        // Assuming we always add 0xFF. This means after n bytes,
        //   s1' = s1 + n*0xFF                           // overflows at around n=2^24
        //   s2' = s2 + n*s1 + (n*(n-1))/2 * 0xFF        // overflows at around n=5550
        if (++counter > 5500) {
            s1 %= BASE;
            s2 %= BASE;
            counter = 0;
        }
        s1 += *p;
        s2 += s1;
    }
    s1 %= BASE;
    s2 %= BASE;
    return (s2 << 16) | s1;
}

size_t
afl::checksums::Adler32::bits() const
{
    return 32;
}

uint32_t
afl::checksums::Adler32::addImpl(Memory_t data, uint32_t prev) const
{
    return add(data, prev);
}

size_t
afl::checksums::Adler32::bitsImpl() const
{
    return bits();
}
