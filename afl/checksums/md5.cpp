/**
  *  \file afl/checksums/md5.cpp
  *  \brief Class afl::checksums::MD5
  *
  *  This file is derived (and heavily mangled) from file
  *     MD5C.C - RSA Data Security, Inc., MD5 message-digest algorithm
  *
  *  Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
  *  rights reserved.
  *
  *  License to copy and use this software is granted provided that it
  *  is identified as the "RSA Data Security, Inc. MD5 Message-Digest
  *  Algorithm" in all material mentioning or referencing this software
  *  or this function.
  *
  *  License is also granted to make and use derivative works provided
  *  that such works are identified as "derived from the RSA Data
  *  Security, Inc. MD5 Message-Digest Algorithm" in all material
  *  mentioning or referencing the derived work.
  *
  *  RSA Data Security, Inc. makes no representations concerning either
  *  the merchantability of this software or the suitability of this
  *  software for any particular purpose. It is provided "as is"
  *  without express or implied warranty of any kind.
  *
  *  These notices must be retained in any copies of any part of this
  *  documentation and/or software.
  */

#include <cstdlib>
#include "afl/checksums/md5.hpp"
#include "afl/bits/pack.hpp"
#include "afl/bits/uint32le.hpp"
#include "afl/bits/rotate.hpp"
#include "afl/bits/uint64le.hpp"

const size_t afl::checksums::MD5::HASH_SIZE;

namespace {
    // Constants for MD5Transform routine.
    const int S11 = 7;
    const int S12 = 12;
    const int S13 = 17;
    const int S14 = 22;
    const int S21 = 5;
    const int S22 = 9;
    const int S23 = 14;
    const int S24 = 20;
    const int S31 = 4;
    const int S32 = 11;
    const int S33 = 16;
    const int S34 = 23;
    const int S41 = 6;
    const int S42 = 10;
    const int S43 = 15;
    const int S44 = 21;

    // Padding
    static const uint8_t PADDING[64] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    /* F, G, H and I are basic MD5 functions. */
    inline uint32_t F(uint32_t x, uint32_t y, uint32_t z) {
        return (x & y) | (~x & z);
    }
    inline uint32_t G(uint32_t x, uint32_t y, uint32_t z) {
        return (x & z) | (y & ~z);
    }
    inline uint32_t H(uint32_t x, uint32_t y, uint32_t z) {
        return x ^ y ^ z;
    }
    inline uint32_t I(uint32_t x, uint32_t y, uint32_t z) {
        return y ^ (x | ~z);
    }

    /* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
       Rotation is separate from addition to prevent recomputation.  */
    inline uint32_t FF(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, int s, uint32_t ac) {
        a += F(b, c, d) + x + ac;
        a = afl::bits::rotateLeft32(a, s);
        a += b;
        return a;
    }
    inline uint32_t GG(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, int s, uint32_t ac) {
        a += G(b, c, d) + x + ac;
        a = afl::bits::rotateLeft32(a, s);
        a += b;
        return a;
    }
    inline uint32_t HH(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, int s, uint32_t ac) {
        a += H(b, c, d) + x + ac;
        a = afl::bits::rotateLeft32(a, s);
        a += b;
        return a;
    }
    inline uint32_t II(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, int s, uint32_t ac) {
        a += I(b, c, d) + x + ac;
        a = afl::bits::rotateLeft32(a, s);
        a += b;
        return a;
    }
}

/********************************** MD5 **********************************/

afl::checksums::MD5::MD5()
{
    MD5::clear();
}

afl::checksums::MD5::~MD5()
{ }


// Reset.
void
afl::checksums::MD5::clear()
{
    m_count = 0;

    m_state[0] = 0x67452301;
    m_state[1] = 0xefcdab89;
    m_state[2] = 0x98badcfe;
    m_state[3] = 0x10325476;
}

// Add bytes.
void
afl::checksums::MD5::add(ConstBytes_t data)
{
    // Compute buffer fill level
    size_t index = static_cast<size_t>(m_count & 0x3F);
    size_t remainder = sizeof(m_buffer) - index;

    // Update byte count
    m_count += data.size();

    if (data.size() >= remainder) {
        // We can complete one block
        Bytes_t(m_buffer).subrange(index).copyFrom(data.split(remainder));
        transform(m_state, m_buffer);

        // Process as many blocks as possible from input
        while (const Block_t* p = data.eatN<sizeof(Block_t)>()) {
            transform(m_state, *p);
        }
        index = 0;
    }

    // Save remainder
    Bytes_t(m_buffer).subrange(index).copyFrom(data);
}

// Get hash size in bytes.
size_t
afl::checksums::MD5::getHashSize() const
{
    return HASH_SIZE;
}

// Get block size in bytes.
size_t
afl::checksums::MD5::getBlockSize() const
{
    return 64;
}

// Get hash value.
afl::checksums::MD5::Bytes_t
afl::checksums::MD5::getHash(Bytes_t data) const
{
    // Make a clone of the object because finalizing the hash will modify it.
    MD5 clone(*this);

    // Save number of bits
    uint8_t bits[8];
    afl::bits::UInt64LE::pack(bits, m_count << 3);

    // Pad out to 56 mod 64 bytes
    const size_t index = static_cast<size_t>(m_count & 0x3f);
    const size_t padLen = (index < 56) ? (56 - index) : (120 - index);
    clone.add(ConstBytes_t(PADDING).trim(padLen));

    // Append length (before padding)
    clone.add(bits);

    // Produce output.
    // First put it into a temporary array to implement truncation semantics.
    // (packArray(data...) would zero-pad.)
    uint8_t digest[16];
    afl::bits::packArray<afl::bits::UInt32LE>(digest, clone.m_state);
    return data.copyFrom(digest);
}

// MD5 basic transformation.
void
afl::checksums::MD5::transform(State_t& state, const Block_t& block)
{
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3], x[16];

    // FIXME: optimisation:
    // - direct, unsafe implementation: 0.55s / 100 MiB
    // - this safe implementation:      0.79s / 100 MiB
    afl::bits::unpackArray<afl::bits::UInt32LE>(x, block);

    /* Round 1 */
    a = FF(a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
    d = FF(d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
    c = FF(c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
    b = FF(b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
    a = FF(a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
    d = FF(d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
    c = FF(c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
    b = FF(b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
    a = FF(a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
    d = FF(d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
    c = FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    b = FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    a = FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    d = FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    c = FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    b = FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

    /* Round 2 */
    a = GG(a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
    d = GG(d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
    c = GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    b = GG(b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
    a = GG(a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
    d = GG(d, a, b, c, x[10], S22,  0x2441453); /* 22 */
    c = GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    b = GG(b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
    a = GG(a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
    d = GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    c = GG(c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
    b = GG(b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
    a = GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    d = GG(d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
    c = GG(c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
    b = GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    a = HH(a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
    d = HH(d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
    c = HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    b = HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    a = HH(a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
    d = HH(d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
    c = HH(c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
    b = HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    a = HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    d = HH(d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
    c = HH(c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
    b = HH(b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
    a = HH(a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
    d = HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    c = HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    b = HH(b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

    /* Round 4 */
    a = II(a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
    d = II(d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
    c = II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    b = II(b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
    a = II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    d = II(d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
    c = II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    b = II(b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
    a = II(a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
    d = II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    c = II(c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
    b = II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    a = II(a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
    d = II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    c = II(c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
    b = II(b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}
