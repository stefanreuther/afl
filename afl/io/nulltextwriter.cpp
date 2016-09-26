/**
  *  \file afl/io/nulltextwriter.cpp
  *  \brief Class afl::io::NullTextWriter
  */

#include "afl/io/nulltextwriter.hpp"

void
afl::io::NullTextWriter::doWriteText(afl::string::ConstStringMemory_t /*data*/)
{ }

void
afl::io::NullTextWriter::doWriteNewline()
{ }

void
afl::io::NullTextWriter::doFlush()
{ }
