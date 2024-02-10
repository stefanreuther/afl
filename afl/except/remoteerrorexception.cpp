/**
  *  \file afl/except/remoteerrorexception.cpp
  *  \brief Class afl::except::RemoteErrorException
  */

#include "afl/except/remoteerrorexception.hpp"

afl::except::RemoteErrorException::RemoteErrorException(const String_t& fileName, const String_t& msg)
    : FileProblemException(fileName, msg)
{ }

afl::except::RemoteErrorException::RemoteErrorException(afl::io::Stream& file, const String_t& msg)
    : FileProblemException(file, msg)
{ }

afl::except::RemoteErrorException::~RemoteErrorException() throw()
{ }
