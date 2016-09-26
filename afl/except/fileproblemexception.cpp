/**
  *  \file afl/except/fileproblemexception.cpp
  *  \brief Class afl::except::FileProblemException
  */

#include "afl/except/fileproblemexception.hpp"

afl::except::FileProblemException::FileProblemException(const String_t& fileName, const String_t& msg)
    : std::runtime_error(msg),
      m_fileName(fileName)
{ }

afl::except::FileProblemException::FileProblemException(afl::io::Stream& file, const String_t& msg)
    : std::runtime_error(msg),
      m_fileName(file.getName())
{ }

afl::except::FileProblemException::~FileProblemException() throw()
{ }
