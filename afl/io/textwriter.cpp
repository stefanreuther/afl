/**
  *  \file afl/io/textwriter.cpp
  */

#include "afl/io/textwriter.hpp"

afl::io::TextWriter::~TextWriter()
{ }

void
afl::io::TextWriter::writeText(const String_t& text)
{
    writeText(afl::string::toMemory(text));
}

void
afl::io::TextWriter::writeText(const char* text)
{
    writeText(afl::string::toMemory(text));
}

void
afl::io::TextWriter::writeText(afl::string::ConstStringMemory_t data)
{
    while (!data.empty()) {
        size_t n = data.find('\n');
        doWriteText(data.split(n));
        if (data.empty()) {
            break;
        }
        doWriteNewline();
        data.split(1);
    }
}

void
afl::io::TextWriter::writeLine(const String_t& line)
{
    writeText(afl::string::toMemory(line));
    doWriteNewline();
}

void
afl::io::TextWriter::writeLine()
{
    doWriteNewline();
}

void
afl::io::TextWriter::flush()
{
    doFlush();
}
