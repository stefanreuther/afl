/**
  *  \file afl/except/unsupportedexception.cpp
  *  \brief Class afl::except::UnsupportedException
  */

#include "afl/except/unsupportedexception.hpp"
#include "afl/string/messages.hpp"

afl::except::UnsupportedException::UnsupportedException(const afl::string::String_t& feature)
    : std::runtime_error(afl::string::Messages::unsupportedFeature() + ": " + feature)
{ }

afl::except::UnsupportedException::~UnsupportedException() throw()
{ }
