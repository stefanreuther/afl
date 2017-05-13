/**
  *  \file afl/checksums/sha1.cpp
  *  \brief Class afl::checksums::SHA1
  *
  *  This implementation is derived from the reference implementation
  *  in RFC 3174 which comes with the following copyright statement:
  *
  *  Copyright (C) The Internet Society (2001).  All Rights Reserved.
  *
  *  This document and translations of it may be copied and furnished to
  *  others, and derivative works that comment on or otherwise explain it
  *  or assist in its implementation may be prepared, copied, published
  *  and distributed, in whole or in part, without restriction of any
  *  kind, provided that the above copyright notice and this paragraph are
  *  included on all such copies and derivative works.  However, this
  *  document itself may not be modified in any way, such as by removing
  *  the copyright notice or references to the Internet Society or other
  *  Internet organizations, except as needed for the purpose of
  *  developing Internet standards in which case the procedures for
  *  copyrights defined in the Internet Standards process must be
  *  followed, or as required to translate it into languages other than
  *  English.
  *
  *  The limited permissions granted above are perpetual and will not be
  *  revoked by the Internet Society or its successors or assigns.
  *
  *  This document and the information contained herein is provided on an
  *  "AS IS" basis and THE INTERNET SOCIETY AND THE INTERNET ENGINEERING
  *  TASK FORCE DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING
  *  BUT NOT LIMITED TO ANY WARRANTY THAT THE USE OF THE INFORMATION
  *  HEREIN WILL NOT INFRINGE ANY RIGHTS OR ANY IMPLIED WARRANTIES OF
  *  MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
  */

#include <cassert>
#include "afl/checksums/sha1.hpp"
#include "afl/bits/rotate.hpp"
#include "afl/bits/pack.hpp"
#include "afl/bits/uint32be.hpp"
#include "afl/bits/uint64be.hpp"

// openssl 100 MB:     0.49
// original:           1.69
// getHash/packArray:  1.78 (?)
// m_bytes (not bits): 1.65
// unpackArray:        1.89
// add without copy:   1.47
// optimisations:      1.39        unrolled: 1.03

const size_t afl::checksums::SHA1::HASH_SIZE;

afl::checksums::SHA1::SHA1()
{
    SHA1::clear();
}

afl::checksums::SHA1::~SHA1()
{ }

// SHA1Reset
void
afl::checksums::SHA1::clear()
{
    m_bytes = 0;

    m_state[0] = 0x67452301;
    m_state[1] = 0xEFCDAB89;
    m_state[2] = 0x98BADCFE;
    m_state[3] = 0x10325476;
    m_state[4] = 0xC3D2E1F0;
}

// SHA1Input, heavily modified.
void
afl::checksums::SHA1::add(ConstBytes_t data)
{
    // Account for bytes
    size_t index = size_t(m_bytes % sizeof(m_block));
    m_bytes += data.size();

    // Try to fill one block
    size_t room = sizeof(m_block) - index;
    if (index != 0 && data.size() >= room) {
        // Data will complete one partial block
        Bytes_t(m_block).subrange(index).copyFrom(data.split(room));
        processBlock(m_block);
        index = 0;
    }

    // Process input directly from whole-block data
    while (const Block_t* p = data.eatN<sizeof(Block_t)>()) {
        processBlock(*p);
    }

    // Copy remainder into m_block
    Bytes_t(m_block).subrange(index).copyFrom(data);
}

size_t
afl::checksums::SHA1::getHashSize() const
{
    return HASH_SIZE;
}

// Get block size in bytes.
size_t
afl::checksums::SHA1::getBlockSize() const
{
    return 64;
}

// SHA1Result, heavily modified
afl::checksums::SHA1::Bytes_t
afl::checksums::SHA1::getHash(Bytes_t data) const
{
    SHA1 copy(*this);
    copy.padMessage();

    uint8_t buffer[HASH_SIZE];
    afl::bits::packArray<afl::bits::UInt32BE>(buffer, copy.m_state);
    return data.copyFrom(buffer);
}

// SHA1ProcessMessageBlock
void
afl::checksums::SHA1::processBlock(const Block_t& block)
{
    const uint32_t K0 = 0x5A827999;
    const uint32_t K1 = 0x6ED9EBA1;
    const uint32_t K2 = 0x8F1BBCDC;
    const uint32_t K3 = 0xCA62C1D6;

#if 1
    /*
     *  Simple version, close to RFC code
     */

    // Create temporary array
    uint32_t W[80];
    afl::bits::unpackArray<afl::bits::UInt32BE>(afl::base::Memory<uint32_t>::unsafeCreate(W, 16), block);
    for (size_t t = 16; t < 80; t++) {
        W[t] = afl::bits::rotateLeft32(W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16], 1);
    }

    // Save state into registers
    uint32_t A = m_state[0];
    uint32_t B = m_state[1];
    uint32_t C = m_state[2];
    uint32_t D = m_state[3];
    uint32_t E = m_state[4];

    for (size_t t = 0; t < 20; t++) {
        uint32_t tmp = afl::bits::rotateLeft32(A, 5) + ((B & C) | ((~B) & D)) + E + W[t] + K0;
        E = D;
        D = C;
        C = afl::bits::rotateLeft32(B, 30);
        B = A;
        A = tmp;
    }

    for (size_t t = 20; t < 40; t++) {
        uint32_t tmp = afl::bits::rotateLeft32(A, 5) + (B ^ C ^ D) + E + W[t] + K1;
        E = D;
        D = C;
        C = afl::bits::rotateLeft32(B, 30);
        B = A;
        A = tmp;
    }

    for (size_t t = 40; t < 60; t++) {
        // '(B|C)&D' used to be '(B & D) | (C & D)', which is equivalent but one more operation.
        uint32_t tmp = afl::bits::rotateLeft32(A, 5) + ((B & C) | ((B|C) & D)) + E + W[t] + K2;
        E = D;
        D = C;
        C = afl::bits::rotateLeft32(B, 30);
        B = A;
        A = tmp;
    }

    for (size_t t = 60; t < 80; t++) {
        uint32_t tmp = afl::bits::rotateLeft32(A, 5) + (B ^ C ^ D) + E + W[t] + K3;
        E = D;
        D = C;
        C = afl::bits::rotateLeft32(B, 30);
        B = A;
        A = tmp;
    }

    m_state[0] += A;
    m_state[1] += B;
    m_state[2] += C;
    m_state[3] += D;
    m_state[4] += E;
#else
    /*
     *  Fast, uglified, unrolled version.
     *  (as of 20150819, 30% faster but 3x the code size, on i386.)
     *  The idea is to give the compiler the possibility to keep everything in registers.
     */

    // Create temporary array
    uint32_t W[16];
    afl::bits::unpackArray<afl::bits::UInt32BE>(W, block);

    // Save state into registers
    uint32_t A = m_state[0];
    uint32_t B = m_state[1];
    uint32_t C = m_state[2];
    uint32_t D = m_state[3];
    uint32_t E = m_state[4];

#define UPDATE() W[t%16] = afl::bits::rotateLeft32(W[(t-3)%16] ^ W[(t-8)%16] ^ W[(t-14)%16] ^ W[(t-16)%16], 1)
#define LOOP5(tt, code)                         \
       {const size_t t = tt; code; UPDATE();}   \
       {const size_t t = tt+1; code; UPDATE();} \
       {const size_t t = tt+2; code; UPDATE();} \
       {const size_t t = tt+3; code; UPDATE();} \
       {const size_t t = tt+4; code; UPDATE();}
#define LOOP20(tt, code)                        \
       LOOP5(tt, code);                         \
       LOOP5(tt+5, code);                       \
       LOOP5(tt+10, code);                      \
       LOOP5(tt+15, code)

    LOOP20(0,
           uint32_t tmp = afl::bits::rotateLeft32(A, 5) + ((B & C) | ((~B) & D)) + E + W[t%16] + K0;
           E = D;
           D = C;
           C = afl::bits::rotateLeft32(B, 30);
           B = A;
           A = tmp;
           );

    LOOP20(20,
           uint32_t tmp = afl::bits::rotateLeft32(A, 5) + (B ^ C ^ D) + E + W[t%16] + K1;
           E = D;
           D = C;
           C = afl::bits::rotateLeft32(B, 30);
           B = A;
           A = tmp;
           );

    LOOP20(40,
           uint32_t tmp = afl::bits::rotateLeft32(A, 5) + ((B & C) | ((B|C) & D)) + E + W[t%16] + K2;
           E = D;
           D = C;
           C = afl::bits::rotateLeft32(B, 30);
           B = A;
           A = tmp;
           );

    LOOP20(60,
           uint32_t tmp = afl::bits::rotateLeft32(A, 5) + (B ^ C ^ D) + E + W[t%16] + K3;
           E = D;
           D = C;
           C = afl::bits::rotateLeft32(B, 30);
           B = A;
           A = tmp;
           );

    m_state[0] += A;
    m_state[1] += B;
    m_state[2] += C;
    m_state[3] += D;
    m_state[4] += E;
#endif
}

// SHA1PadMessage, heavily modified
void
afl::checksums::SHA1::padMessage()
{
    size_t index = size_t(m_bytes % sizeof(m_block));

    // Encode bit count (do this first, later add's modify it)
    uint8_t encodedSize[8];
    afl::bits::UInt64BE::pack(encodedSize, m_bytes << 3);

    // Pad to 56 mod 64, but add at least one padding byte
    static const Block_t pad = {
        0x80,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    size_t padSize = index > 55 ? 120 - index : 56 - index;
    add(ConstBytes_t(pad).subrange(0, padSize));

    // Store bit count
    add(encodedSize);
}
