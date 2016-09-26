/**
  *  \file u/t_io_datasink.cpp
  *  \brief Test for afl::io::DataSink
  */

#include "afl/io/datasink.hpp"

#include "u/t_io.hpp"
#include "afl/string/string.hpp"
#include "afl/except/fileproblemexception.hpp"

/** Simple test. */
void
TestIoDataSink::testIt()
{
    // Make a simple derivation
    class Tester : public afl::io::DataSink {
     public:
        Tester()
            : m_size(10)
            { }

        virtual bool handleData(const String_t& /*name*/, afl::base::ConstBytes_t& data)
            {
                data.splitUpdate(m_size);
                return m_size == 0;
            }

     private:
        size_t m_size;
    };


    // Hit the end exactly
    {
        Tester t;
        TS_ASSERT_THROWS_NOTHING(t.handleFullData("TestIoDataSink", afl::string::toBytes("12345")));
        TS_ASSERT_THROWS_NOTHING(t.handleFullData("TestIoDataSink", afl::string::toBytes("12345")));
        TS_ASSERT_THROWS(t.handleFullData("TestIoDataSink", afl::string::toBytes("12345")), afl::except::FileProblemException);
    }

    // Inexact hit
    {
        Tester t;
        TS_ASSERT_THROWS_NOTHING(t.handleFullData("TestIoDataSink", afl::string::toBytes("123456")));
        TS_ASSERT_THROWS(t.handleFullData("TestIoDataSink", afl::string::toBytes("123456")), afl::except::FileProblemException);
        TS_ASSERT_THROWS(t.handleFullData("TestIoDataSink", afl::string::toBytes("123456")), afl::except::FileProblemException);
    }
}
