/**
  *  \file afl/except/filesystemexception.cpp
  *  \brief Class afl::except::FileSystemException
  */

#include "afl/except/filesystemexception.hpp"

afl::except::FileSystemException::FileSystemException(const String_t& fileName, afl::sys::Error error)
    : FileProblemException(fileName, error.getText()),
      m_error(error)
{ }

afl::except::FileSystemException::FileSystemException(afl::io::Stream& file, afl::sys::Error error)
    : FileProblemException(file, error.getText()),
      m_error(error)
{ }

afl::except::FileSystemException::~FileSystemException() throw()
{ }
