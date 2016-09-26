/**
  *  \file afl/except/filetooshortexception.cpp
  *  \brief Class afl::except::FileTooShortException
  */

#include "afl/except/filetooshortexception.hpp"
#include "afl/string/messages.hpp"

afl::except::FileTooShortException::FileTooShortException(const String_t& fileName)
    : FileFormatException(fileName, afl::string::Messages::fileTooShort())
{ }

afl::except::FileTooShortException::FileTooShortException(afl::io::Stream& file)
    : FileFormatException(file, afl::string::Messages::fileTooShort())
{ }

afl::except::FileTooShortException::~FileTooShortException() throw()
{ }
