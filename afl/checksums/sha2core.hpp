/**
  *  \file afl/checksums/sha2core.hpp
  *  \brief Internal template class SHA2Core
  */
#ifndef AFL_AFL_CHECKSUMS_SHA2CORE_HPP
#define AFL_AFL_CHECKSUMS_SHA2CORE_HPP

#include <algorithm>
#include <cstring>
#include "afl/base/types.hpp"
#include "afl/bits/rotate.hpp"
#include "afl/bits/pack.hpp"
#include "afl/bits/uint32be.hpp"
#include "afl/bits/uint64be.hpp"
#include "afl/base/memory.hpp"

namespace afl { namespace checksums {

    /** Common part of SHA-2 hash functions.
        This implements the common part of SHA224, SHA256, SHA384, and SHA512.
        Those use essentially hte same algorithm, parameterized on word size and elementary functions,
        as well as different initialisation vectors.

        This class is not intended for public use.

        <b>Specification reference:</b> RFC 6234 "US Secure Hash Algorithms".
        This code is implemented from the pseudo-code in the document.

        <b>Speed:</b> This is a very straightforward implementation.
        Still it is less than 10% slower than the "sha512sum" etc. utilities.
        (And about 50% slower than openssl.) */
    template<typename T>
    class SHA2Core {
     public:
        typedef typename T::Word_t State_t[8];

        /** Initialize.
            Implements RFC 6234 p6.1, p6.3.
            \param init Initialisation vector */
        void init(const State_t& init);

        /** Add data.
            This implements blocking of the data, and eventually calls block(). */
        void add(const uint8_t* buf, size_t size);

        /** Pad message.
            Implements RFC 6234 p4.1 and p4.2. */
        void pad();

        /** Get state.
            Used to produce output.
            \return state */
        const State_t& getState() const
            { return m_hs; }

     private:
        State_t m_hs;

        uint64_t m_counterLo;
        uint64_t m_counterHi;
        size_t m_fill;
        uint8_t m_block[sizeof(typename T::Block_t)];

        /** Compute one step.
            Implements RFC 6234 p6.2, p6.4.
            \param hs [in/out] Initial state (input: H(i-1)0 .. H(i-1)7; output: H(i)0 .. H(i)7)
            \param ms [in] Message (M(i)t) */
        static void block(State_t& hs, const typename T::Block_t& ms);
    };

    template<typename T>
    void SHA2Core<T>::init(const State_t& init)
    {
        for (size_t i = 0; i < 8; ++i) {
            m_hs[i] = init[i];
        }
        m_counterLo = 0;
        m_counterHi = 0;
        m_fill = 0;
    }

    template<typename T>
    void SHA2Core<T>::add(const uint8_t* buf, size_t size)
    {
        while (size > 0) {
            size_t avail = sizeof(m_block) - m_fill;
            size_t now = std::min(size, avail);
            std::memcpy(&m_block[m_fill], buf, now);
            buf += now;
            size -= now;
            m_fill += now;
            m_counterLo += 8*now;
            if (m_counterLo < 8*now) {
                ++m_counterHi;
            }
            if (m_fill >= sizeof(m_block)) {
                typename T::Block_t b;
                T::unpack(b, m_block);
                block(m_hs, b);
                m_fill = 0;
            }
        }
    }

    template<typename T>
    void SHA2Core<T>::pad()
    {
        uint64_t len = m_counterLo;
        uint64_t len_hi = m_counterHi;

        // Storage for the size. We want this to be 1x8 bytes for 32 bit words,
        // and 2x8 bytes for 64 bit words.
        uint8_t len_bits[sizeof(typename T::Word_t)/4][8];

        // add single '0x80'
        uint8_t pad = 0x80;
        add(&pad, 1);

        // pad until m_fill = sizeof(m_block) - sizeof(len_bits)
        while (m_fill != sizeof(m_block)-sizeof(len_bits)) {
            uint8_t zero = 0;
            add(&zero, 1);
        }

        // add length
        T::pack_length(len_bits, len, len_hi);
        add(len_bits[0], sizeof len_bits);
    }

    template<typename T>
    void SHA2Core<T>::block(State_t& hs, const typename T::Block_t& ms)
    {
        // 1. Prepare the message schedule W:
        typedef typename T::Word_t Word_t;
        Word_t ws[T::ITERATIONS];
        for (size_t t = 0; t < 16; ++t) {
            ws[t] = ms[t];
        }
        for (size_t t = 16; t < T::ITERATIONS; ++t) {
            ws[t] = T::ssig1(ws[t-2]) + ws[t-7] + T::ssig0(ws[t-15]) + ws[t-16];
        }

        // 2. Initialize the working variables:
        Word_t a = hs[0];
        Word_t b = hs[1];
        Word_t c = hs[2];
        Word_t d = hs[3];
        Word_t e = hs[4];
        Word_t f = hs[5];
        Word_t g = hs[6];
        Word_t h = hs[7];

        // 3. Perform the main hash computation:
        for (size_t t = 0; t < T::ITERATIONS; ++t) {
            Word_t T1 = h + T::bsig1(e) + T::ch(e,f,g) + T::ks[t] + ws[t];
            Word_t T2 = T::bsig0(a) + T::maj(a,b,c);
            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;
        }

        // 4. Compute the intermediate hash value H(i)
        hs[0] += a;
        hs[1] += b;
        hs[2] += c;
        hs[3] += d;
        hs[4] += e;
        hs[5] += f;
        hs[6] += g;
        hs[7] += h;
    }


    /*
     *  32-bit Functions (SHA-224, SHA-256)
     *
     *  Implements RFC 6234 p5.1
     */

    struct SHA2_32 {
        typedef uint32_t Word_t;
        typedef Word_t Block_t[16];

        static inline Word_t ch(Word_t x, Word_t y, Word_t z)
            { return (x & y) ^ (~x & z); }

        static inline Word_t maj(Word_t x, Word_t y, Word_t z)
            { return (x&y) ^ (x&z) ^ (y&z); }

        static inline Word_t bsig0(Word_t x)
            { return afl::bits::rotateRight32(x, 2) ^ afl::bits::rotateRight32(x, 13) ^ afl::bits::rotateRight32(x, 22); }

        static inline Word_t bsig1(Word_t x)
            { return afl::bits::rotateRight32(x, 6) ^ afl::bits::rotateRight32(x, 11) ^ afl::bits::rotateRight32(x, 25); }

        static inline Word_t ssig0(Word_t x)
            { return afl::bits::rotateRight32(x, 7) ^ afl::bits::rotateRight32(x, 18) ^ (x >> 3); }

        static inline Word_t ssig1(Word_t x)
            { return afl::bits::rotateRight32(x, 17) ^ afl::bits::rotateRight32(x, 19) ^ (x >> 10); }

        static inline void unpack(Block_t& out, const uint8_t (&in)[sizeof(Block_t)])
            { afl::bits::unpackArray<afl::bits::UInt32BE>(out, in); }

        static inline void pack_length(uint8_t (&out)[1][8], uint64_t lo, uint64_t /*hi*/)
            { afl::bits::UInt64BE::pack(out[0], lo); }

        static const Word_t ks[];

        static const size_t ITERATIONS = 64;
    };


    /*
     *  64-bit Functions (SHA-384, SHA-512)
     *
     *  Implements RFC 6234 p5.2
     */

    struct SHA2_64 {
        typedef uint64_t Word_t;
        typedef Word_t Block_t[16];

        static inline Word_t ch(Word_t x, Word_t y, Word_t z)
            { return (x & y) ^ (~x & z); }

        static inline Word_t maj(Word_t x, Word_t y, Word_t z)
            { return (x&y) ^ (x&z) ^ (y&z); }

        static inline Word_t bsig0(Word_t x)
            { return afl::bits::rotateRight64(x, 28) ^ afl::bits::rotateRight64(x, 34) ^ afl::bits::rotateRight64(x, 39); }

        static inline Word_t bsig1(Word_t x)
            { return afl::bits::rotateRight64(x, 14) ^ afl::bits::rotateRight64(x, 18) ^ afl::bits::rotateRight64(x, 41); }

        static inline Word_t ssig0(Word_t x)
            { return afl::bits::rotateRight64(x, 1) ^ afl::bits::rotateRight64(x, 8) ^ (x >> 7); }

        static inline Word_t ssig1(Word_t x)
            { return afl::bits::rotateRight64(x, 19) ^ afl::bits::rotateRight64(x, 61) ^ (x >> 6); }

        static inline void unpack(Block_t& out, const uint8_t (&in)[sizeof(Block_t)])
            { afl::bits::unpackArray<afl::bits::UInt64BE>(out, in); }

        static inline void pack_length(uint8_t (&out)[2][8], uint64_t lo, uint64_t hi)
            {
                afl::bits::UInt64BE::pack(out[0], hi);
                afl::bits::UInt64BE::pack(out[1], lo);
            }

        static const Word_t ks[];

        static const size_t ITERATIONS = 80;
    };

} }

#endif
