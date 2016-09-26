/**
  *  \file afl/checksums/crc32.cpp
  *  \brief Class afl::checksums::CRC32
  */

#include "afl/checksums/crc32.hpp"

afl::checksums::CRC32::CRC32(uint32_t poly)
{
    init(poly);
}

afl::checksums::CRC32::~CRC32()
{ }

uint32_t
afl::checksums::CRC32::add(Memory_t data, uint32_t prev) const
{
    uint32_t accum = ~prev;
    while (const uint8_t* p = data.eat()) {
        accum = m_table[(*p ^ accum) & 255] ^ (accum >> 8);
    }
    return ~accum;
}

size_t
afl::checksums::CRC32::bits() const
{
    return 32;
}

afl::checksums::CRC32&
afl::checksums::CRC32::getDefaultInstance()
{
    /* zlib polynomial: x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1 */
    static CRC32 defaultInstance(0xEDB88320);
    return defaultInstance;
}

uint32_t
afl::checksums::CRC32::addImpl(Memory_t data, uint32_t prev) const
{
    return add(data, prev);
}

size_t
afl::checksums::CRC32::bitsImpl() const
{
    return bits();
}

void
afl::checksums::CRC32::init(uint32_t poly)
{
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t n = i;
        for (int bit = 0; bit < 8; ++bit) {
            n = (n & 1) ? poly ^ (n >> 1) : (n >> 1);
        }
        m_table[i] = n;
    }
}
