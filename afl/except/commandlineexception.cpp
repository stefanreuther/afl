/**
  *  \file afl/except/commandlineexception.cpp
  *  \brief Class afl::except::CommandLineException
  */

#include "afl/except/commandlineexception.hpp"

afl::except::CommandLineException::CommandLineException(const String_t& msg)
    : std::runtime_error(msg)
{ }
