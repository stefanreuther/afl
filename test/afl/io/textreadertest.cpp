/**
  *  \file test/afl/io/textreadertest.cpp
  *  \brief Test for afl::io::TextReader
  */

#include "afl/io/textreader.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.io.TextReader", a)
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
        a.check("readLine empty", !t.readLine(s));
    }

    {
        Tester t(3);
        a.checkEqual("getLineNumber 0", t.getLineNumber(), 0);
        a.check("readLine 1", t.readLine(s));
        a.checkEqual("getLineNumber 1", t.getLineNumber(), 1);
        a.check("readLine 2", t.readLine(s));
        a.check("readLine 3", t.readLine(s));
        a.checkEqual("getLineNumber 3", t.getLineNumber(), 3);
        a.check("readLine 4", !t.readLine(s));
        a.checkEqual("getLineNumber last", t.getLineNumber(), 3);
    }
}
