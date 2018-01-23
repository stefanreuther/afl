/**
  *  \file afl/io/internaltextwriter.cpp
  *  \brief Class afl::io::InternalTextWriter
  */

#include "afl/io/internaltextwriter.hpp"

afl::io::InternalTextWriter::InternalTextWriter()
    : TextWriter(),
      m_content()
{ }

afl::io::InternalTextWriter::~InternalTextWriter()
{ }

void
afl::io::InternalTextWriter::clear()
{
    m_content.clear();
}

afl::string::ConstStringMemory_t
afl::io::InternalTextWriter::getContent() const
{
    return m_content;
}

void
afl::io::InternalTextWriter::doWriteText(afl::string::ConstStringMemory_t data)
{
    m_content.append(data);
}

void
afl::io::InternalTextWriter::doWriteNewline()
{
    m_content.append('\n');
}

void
afl::io::InternalTextWriter::doFlush()
{ }
