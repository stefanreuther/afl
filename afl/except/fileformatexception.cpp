/**
  *  \file afl/except/fileformatexception.cpp
  *  \brief Class afl::except::FileFormatException
  */

#include "afl/except/fileformatexception.hpp"

afl::except::FileFormatException::FileFormatException(const String_t& fileName, const String_t& msg)
    : FileProblemException(fileName, msg)
{ }

afl::except::FileFormatException::FileFormatException(afl::io::Stream& file, const String_t& msg)
    : FileProblemException(file, msg)
{ }

afl::except::FileFormatException::~FileFormatException() throw()
{ }
