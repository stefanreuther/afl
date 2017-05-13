/**
  *  \file afl/charset/defaultcharsetfactory.cpp
  *  \brief Class afl::charset::DefaultCharsetFactory
  */

#include "afl/charset/defaultcharsetfactory.hpp"
#include "afl/charset/codepage.hpp"
#include "afl/charset/codepagecharset.hpp"
#include "afl/charset/utf8charset.hpp"
#include "afl/string/string.hpp"


// Constructor.
afl::charset::DefaultCharsetFactory::DefaultCharsetFactory()
{ }

// Destructor.
afl::charset::DefaultCharsetFactory::~DefaultCharsetFactory()
{ }

// Create Charset instance.
afl::charset::Charset*
afl::charset::DefaultCharsetFactory::createCharset(String_t name)
{
    using afl::string::strCaseCompare;

    // Remove dashes. "ISO-8859-1" is the same as "iso88591".
    String_t::size_type n;
    while ((n = name.find('-')) != String_t::npos) {
        name.erase(n, 1);
    }

    // Check supported charsets
    if (strCaseCompare(name, "cp1250") == 0 || strCaseCompare(name, "windows1250") == 0) {
        return new CodepageCharset(g_codepage1250);
    } else if (strCaseCompare(name, "cp1251") == 0 || strCaseCompare(name, "windows1251") == 0) {
        return new CodepageCharset(g_codepage1251);
    } else if (strCaseCompare(name, "cp1252") == 0 || strCaseCompare(name, "windows1252") == 0) {
        return new CodepageCharset(g_codepage1252);
    } else if (strCaseCompare(name, "cp437") == 0 || strCaseCompare(name, "ibm437") == 0) {
        return new CodepageCharset(g_codepage437);
    } else if (strCaseCompare(name, "cp850") == 0 || strCaseCompare(name, "ibm850") == 0) {
        return new CodepageCharset(g_codepage850);
    } else if (strCaseCompare(name, "cp852") == 0 || strCaseCompare(name, "ibm852") == 0) {
        return new CodepageCharset(g_codepage852);
    } else if (strCaseCompare(name, "cp866") == 0 || strCaseCompare(name, "ibm866") == 0) {
        return new CodepageCharset(g_codepage866);
    } else if (strCaseCompare(name, "koi8r") == 0) {
        return new CodepageCharset(g_codepageKOI8R);
    } else if (strCaseCompare(name, "latin1") == 0 || strCaseCompare(name, "iso88591") == 0 || strCaseCompare(name, "ansi") == 0) {
        return new CodepageCharset(g_codepageLatin1);
    } else if (strCaseCompare(name, "latin2") == 0 || strCaseCompare(name, "iso88592") == 0) {
        return new CodepageCharset(g_codepageLatin2);
    } else if (strCaseCompare(name, "utf8") == 0) {
        return new Utf8Charset();
    } else {
        return 0;
    }
}
