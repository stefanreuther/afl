/**
  *  \file afl/checksums/hash.cpp
  *  \brief Class afl::checksums::Hash
  */

#include <cassert>
#include "afl/checksums/hash.hpp"
#include "afl/base/staticassert.hpp"
#include "afl/except/unsupportedexception.hpp"
#include "afl/bits/uint32be.hpp"
#include "afl/string/hex.hpp"

// Maximum size
const size_t afl::checksums::Hash::MAX_HASH_SIZE;
const size_t afl::checksums::Hash::MAX_BLOCK_SIZE;


// Compute HMAC
void
afl::checksums::Hash::computeHMAC(ConstBytes_t key, ConstBytes_t data)
{
    uint8_t k_ipad[MAX_BLOCK_SIZE];
    uint8_t k_opad[MAX_BLOCK_SIZE];
    uint8_t tmp[MAX_HASH_SIZE];

    // HMAC only works for hashes of up to 64 bytes
    const size_t hashSize = getHashSize();
    const size_t blockSize = getBlockSize();
    if (hashSize > MAX_HASH_SIZE || blockSize > MAX_BLOCK_SIZE || hashSize > blockSize) {
        throw afl::except::UnsupportedException("hmac");
    }

    // If key is longer than block size, replace it by hash
    if (key.size() > blockSize) {
        clear();
        add(key);
        key = getHash(tmp);
    }

    // Prepare k_ipad
    Bytes_t(k_ipad).fill(0);
    Bytes_t(k_ipad).copyFrom(key);
    for (size_t i = 0; i < sizeof(k_ipad); ++i) {
        k_ipad[i] ^= 0x36;
    }

    // Prepare k_opad
    Bytes_t(k_opad).fill(0);
    Bytes_t(k_opad).copyFrom(key);
    for (size_t i = 0; i < sizeof(k_opad); ++i) {
        k_opad[i] ^= 0x5C;
    }
    
    // Perform inner hash
    clear();
    add(Bytes_t(k_ipad).trim(blockSize));
    add(data);
    ConstBytes_t digest = getHash(tmp);

    // Perform outer hash
    clear();
    add(Bytes_t(k_opad).trim(blockSize));
    add(digest);
}

// Compute PBKDF2
void
afl::checksums::Hash::computePBKDF2(Bytes_t out, ConstBytes_t password, ConstBytes_t salt, uint32_t numIterations)
{
    // Make room for salt/count
    uint8_t saltAndIndexBuffer[MAX_BLOCK_SIZE + 4];
    Bytes_t saltAndIndex(saltAndIndexBuffer);
    saltAndIndex.trim(salt.size() + 4);
    saltAndIndex.copyFrom(salt);
    afl::bits::UInt32BE::Bytes_t* pIndex = saltAndIndex.subrange(salt.size()).eatN<4>();
    if (!pIndex) {
        throw afl::except::UnsupportedException("pbkdf2");
    }

    // Compute blocks of output (T_i) until no more room.
    uint32_t index = 0;
    while (!out.empty()) {
        // Append index to salt
        ++index;
        afl::bits::UInt32BE::pack(*pIndex, index);

        // Compute initial iteration and save as prev ('U_1')
        computeHMAC(password, saltAndIndex);
        uint8_t prevBuffer[MAX_HASH_SIZE];
        Bytes_t prev = getHash(prevBuffer);

        // Save U_1 in accumulator
        uint8_t accBuffer[MAX_HASH_SIZE];
        Bytes_t(accBuffer).copyFrom(prev);

        // Compute following iterations
        for (uint32_t i = 1; i < numIterations; ++i) {
            // Compute 'U_i+1 = PRF(password, U_i)'
            computeHMAC(password, prev);
            uint8_t nextBuffer[MAX_HASH_SIZE];
            ConstBytes_t next = getHash(nextBuffer);

            // Set 'acc ^= U_i+1' and new 'U_i' for next iteration
            assert(next.size() == prev.size());
            for (size_t i = 0, n = next.size(); i < n; ++i) {
                accBuffer[i] ^= nextBuffer[i];
                prevBuffer[i] = nextBuffer[i];
            }
        }

        // Store result
        assert(prev.size() != 0);
        out.split(prev.size()).copyFrom(accBuffer);
    }
}

// Get as hex string
String_t
afl::checksums::Hash::getHashAsHexString() const
{
    uint8_t buffer[MAX_HASH_SIZE];
    ConstBytes_t hash(getHash(buffer));

    String_t result;
    result.reserve(hash.size()*2);
    while (const uint8_t* p = hash.eat()) {
        afl::string::putHexByte(result, *p, afl::string::HEX_DIGITS_LOWER);
    }
    return result;
}
