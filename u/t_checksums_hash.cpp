/**
  *  \file u/t_checksums_hash.cpp
  *  \brief Test for afl::checksums::Hash
  */

#include "afl/checksums/hash.hpp"

#include "u/t_checksums.hpp"
#include "afl/string/string.hpp"

/** Simple tests. */
void
TestChecksumsHash::testIt()
{
    /* This is an interface, so we instantiate it and check whether it works the way it's intented.
       Just a simple byte-sum hash: */
    class Tester : public afl::checksums::Hash {
     public:
        Tester()
            : m_sum(0)
            { }

        virtual void clear()
            { m_sum = 0; }

        virtual void add(ConstBytes_t data)
            {
                while (const uint8_t* p = data.eat()) {
                    m_sum = static_cast<uint8_t>(m_sum + *p);
                }
            }

        virtual size_t getHashSize() const
            { return 1; }

        virtual size_t getBlockSize() const
            { return 64; }

        virtual Bytes_t getHash(Bytes_t data) const
            {
                uint8_t value[1] = { m_sum };
                return data.copyFrom(value);
            }
     private:
        uint8_t m_sum;
    };

    // We can instantiate it
    Tester tt;

    // Basic properties
    uint8_t data[10];
    uint8_t expected[1];
    TS_ASSERT_EQUALS(tt.getHashSize(), 1U);
    expected[0] = 0;
    TS_ASSERT(tt.getHash(data).equalContent(expected));

    tt.add(afl::string::toBytes("ab"));
    expected[0] = 195;
    TS_ASSERT(tt.getHash(data).equalContent(expected));
    
    // Test HMAC
    const uint8_t key[]   = {1,2};
    const uint8_t value[] = {3,4};
    tt.computeHMAC(key, value);
    // makes k_ipad = 0x37,0x34,0x36,0x36,0x36,0x36....
    //   therefore inner hash = 0x37+0x34+62*0x36 + 3+4 = 0x86
    // makes k_opad = 0x5D,0x5E,0x5C,0x5C,0x5C,0x5C....
    //   therefore outer hash = 0x5D+0x5E+62*0x5C + 0x86 = 0x89
    expected[0] = 0x89;
    TS_ASSERT(tt.getHash(data).equalContent(expected));
}
