/**
  *  \file u/t_io_textreader.cpp
  *  \brief Test for afl::io::TextReader
  */

#include "afl/io/textreader.hpp"

#include "u/t_io.hpp"

/** Interface test. */
void
TestIoTextReader::testIt()
{
    class Tester : public afl::io::TextReader {
     public:
        Tester(int n)
            : m_n(n)
            { }

     protected:
        bool doReadLine(String_t& out)
            {
                if (m_n > 0) {
                    --m_n;
                    out = "x";
                    return true;
                } else {
                    out = "";
                    return false;
                }
            }

     private:
        int m_n;
    };

    String_t s;
    {
        Tester t(0);
        TS_ASSERT(!t.readLine(s));
    }

    {
        Tester t(3);
        TS_ASSERT_EQUALS(t.getLineNumber(), 0);
        TS_ASSERT(t.readLine(s));
        TS_ASSERT_EQUALS(t.getLineNumber(), 1);
        TS_ASSERT(t.readLine(s));
        TS_ASSERT(t.readLine(s));
        TS_ASSERT_EQUALS(t.getLineNumber(), 3);
        TS_ASSERT(!t.readLine(s));
        TS_ASSERT_EQUALS(t.getLineNumber(), 3);
    }
}
