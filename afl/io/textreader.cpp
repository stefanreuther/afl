/**
  *  \file afl/io/textreader.cpp
  *  \brief Interface afl::io::TextReader
  */

#include "afl/io/textreader.hpp"

afl::io::TextReader::TextReader()
    : m_lineNumber(0)
{ }

afl::io::TextReader::~TextReader()
{ }

bool
afl::io::TextReader::readLine(String_t& line)
{
    if (doReadLine(line)) {
        ++m_lineNumber;
        return true;
    } else {
        return false;
    }
}

afl::io::TextReader::LineNumber_t
afl::io::TextReader::getLineNumber() const
{
    return m_lineNumber;
}
