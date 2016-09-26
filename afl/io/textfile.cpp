/**
  *  \file afl/io/textfile.cpp
  *  \brief Class afl::io::TextFile
  *
  *  From pcc-v2/io/textfile.cc 1.15.
  */

#include <cassert>
#include "afl/io/textfile.hpp"
#include "afl/charset/utf8charset.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/string/messages.hpp"
#include "arch/textfile.hpp"
#include "afl/charset/codepage.hpp"
#include "afl/charset/codepagecharset.hpp"


afl::io::TextFile::TextFile(Stream& s)
    : BufferedStream(s),
      m_charset(new afl::charset::CodepageCharset(afl::charset::g_codepageLatin1)),
      m_utf8Snoop(true),
      m_sysNewline(true)
{ }

afl::io::TextFile::~TextFile()
{ }

void
afl::io::TextFile::setCharsetNew(afl::charset::Charset* cs)
{
    assert(cs != 0);
    m_charset.reset(cs);
    m_utf8Snoop = false;
}

void
afl::io::TextFile::setSystemNewline(bool flag)
{
    m_sysNewline = flag;
}

void
afl::io::TextFile::setUtf8Snoop(bool flag)
{
    m_utf8Snoop = flag;
}

bool
afl::io::TextFile::doReadLine(String_t& line)
{
    // UTF-8 BOM
    static const char BOM[] = { char(0xEF), char(0xBB), char(0xBF) };

    // Read line
    String_t rawLine;
    const uint8_t* pch;
    while ((pch = readByte()) != 0) {
        if (*pch == 10) {
            // LF, ends the line
            break;
        } else if (*pch == 13) {
            // CR, could be CRLF; in any case, ends the line
            pch = peekByte();
            if (pch != 0 && *pch == 10) {
                readByte();
            }
            break;
        } else {
            // Process it
            rawLine.append(1, char(*pch));
            if (m_utf8Snoop && rawLine.size() == sizeof(BOM)) {
                // UTF-8 snooping
                if (rawLine.compare(0, sizeof(BOM), BOM, sizeof(BOM)) == 0) {
                    m_charset.reset(new afl::charset::Utf8Charset());
                    rawLine.clear();
                }
                m_utf8Snoop = false;
            }
        }
    }

    // We read a line, so turn off snooping now
    m_utf8Snoop = false;

    // If the file ends in Ctrl-Z, remove that
    if (pch == 0 && rawLine.size() != 0 && rawLine[rawLine.size()-1] == char(26)) {
        rawLine.erase(rawLine.size()-1);
    }

    // Produce result
    if (pch != 0 || rawLine.size() != 0) {
        line = m_charset->decode(afl::string::toMemory(rawLine));
        return true;
    } else {
        line.clear();
        return false;
    }
}

void
afl::io::TextFile::doWriteText(afl::string::ConstStringMemory_t text)
{
    String_t rawString = m_charset->encode(text);
    fullWrite(afl::string::toBytes(rawString));
}

void
afl::io::TextFile::doWriteNewline()
{
    if (m_sysNewline) {
        fullWrite(NEWLINE);
    } else {
        static const uint8_t NL[] = { '\n' };
        fullWrite(NL);
    }
}

void
afl::io::TextFile::doFlush()
{
    BufferedStream::flush();
}
