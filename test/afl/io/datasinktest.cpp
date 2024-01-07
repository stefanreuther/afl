/**
  *  \file test/afl/io/datasinktest.cpp
  *  \brief Test for afl::io::DataSink
  */

#include "afl/io/datasink.hpp"

#include "afl/except/fileproblemexception.hpp"
#include "afl/string/string.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    // Make a simple derivation
    class Tester : public afl::io::DataSink {
     public:
        Tester()
            : m_size(10)
            { }

        virtual bool handleData(afl::base::ConstBytes_t& data)
            {
                data.splitUpdate(m_size);
                return m_size == 0;
            }

     private:
        size_t m_size;
    };
}

AFL_TEST("afl.io.DataSink:hit-end-exactly", a)
{
    Tester t;
    AFL_CHECK_SUCCEEDS(a("handleFullData 1"), t.handleFullData(afl::string::toBytes("12345")));
    AFL_CHECK_SUCCEEDS(a("handleFullData 2"), t.handleFullData(afl::string::toBytes("12345")));
    AFL_CHECK_THROWS(a("handleFullData 3"), t.handleFullData(afl::string::toBytes("12345")), afl::except::FileProblemException);
}

AFL_TEST("afl.io.DataSink:inexact-hit", a)
{
    Tester t;
    AFL_CHECK_SUCCEEDS(a("handleFullData 1"), t.handleFullData(afl::string::toBytes("123456")));
    AFL_CHECK_THROWS(a("handleFullData 2"), t.handleFullData(afl::string::toBytes("123456")), afl::except::FileProblemException);
    AFL_CHECK_THROWS(a("handleFullData 3"), t.handleFullData(afl::string::toBytes("123456")), afl::except::FileProblemException);
}
