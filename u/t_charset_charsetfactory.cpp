/**
  *  \file u/t_charset_charsetfactory.cpp
  *  \brief Test for afl::charset::CharsetFactory
  */

#include "afl/charset/charsetfactory.hpp"

#include "u/t_charset.hpp"

/** Test the interface. */
void
TestCharsetCharsetFactory::testIt()
{
    class Factory : public afl::charset::CharsetFactory {
     public:
        virtual afl::charset::Charset* createCharset(String_t /*name*/)
            { return 0; }
    };
    Factory f;
}
