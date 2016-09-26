/**
  *  \file u/t_checksums_checksum.cpp
  *  \brief Test for afl::checksums::Checksum
  */

#include "afl/checksums/checksum.hpp"
#include "u/t_checksums.hpp"

/** Simple tests. */
void
TestChecksumsChecksum::testIt()
{
    /* Test that the interface is well-formed,
       i.e. allows us to derive a class that can be successfully created. */
    class Tester : public afl::checksums::Checksum {
     public:
        virtual uint32_t addImpl(Memory_t /*data*/, uint32_t /*prev*/) const
            { return 42; }
        virtual size_t bitsImpl() const
            { return 12; }
    };
    Tester t;

    TS_ASSERT_EQUALS(t.add(afl::base::ConstBytes_t(), 0), 42U);
    TS_ASSERT_EQUALS(t.bits(), 12U);
}
