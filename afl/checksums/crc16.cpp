/**
  *  \file afl/checksums/crc16.cpp
  *  \brief Class afl::checksums::CRC16
  */

#include "afl/checksums/crc16.hpp"

afl::checksums::CRC16::CRC16(uint16_t poly)
{
    init(poly);
}

afl::checksums::CRC16::~CRC16()
{ }

uint16_t
afl::checksums::CRC16::add(Memory_t data, uint16_t prev) const
{
    uint16_t accum = prev;
    while (const uint8_t* p = data.eat()) {
        accum = uint16_t(m_table[(*p ^ accum) & 255] ^ (accum >> 8));
    }
    return accum;
}

size_t
afl::checksums::CRC16::bits() const
{
    return 16;
}

afl::checksums::CRC16&
afl::checksums::CRC16::getDefaultInstance()
{
    /* CCITT polynomial: x^16 + x^12 + x^5 + 1 */
    static CRC16 defaultInstance(0x8408);
    return defaultInstance;
}

uint32_t
afl::checksums::CRC16::addImpl(Memory_t data, uint32_t prev) const
{
    return add(data, uint16_t(prev));
}

size_t
afl::checksums::CRC16::bitsImpl() const
{
    return bits();
}

void
afl::checksums::CRC16::init(uint16_t poly)
{
    for (uint16_t i = 0; i < 256; ++i) {
        uint16_t n = i;
        for (int bit = 0; bit < 8; ++bit) {
            n = uint16_t((n & 1) ? poly ^ (n >> 1) : (n >> 1));
        }
        m_table[i] = n;
    }
}
