/**
  *  \file afl/except/invaliddataexception.cpp
  *  \brief Class afl::except::InvalidDataException
  */

#include "afl/except/invaliddataexception.hpp"

afl::except::InvalidDataException::InvalidDataException(const afl::string::String_t& msg)
    : std::runtime_error(msg)
{ }

afl::except::InvalidDataException::~InvalidDataException() throw()
{ }
