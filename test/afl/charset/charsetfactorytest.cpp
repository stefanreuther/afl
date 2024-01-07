/**
  *  \file test/afl/charset/charsetfactorytest.cpp
  *  \brief Test for afl::charset::CharsetFactory
  */

#include "afl/charset/charsetfactory.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST_NOARG("afl.charset.CharsetFactory")
{
    class Factory : public afl::charset::CharsetFactory {
     public:
        virtual afl::charset::Charset* createCharset(String_t /*name*/)
            { return 0; }
    };
    Factory f;
}
