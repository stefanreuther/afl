/**
  *  \file afl/except/systemexception.cpp
  *  \brief Class afl::except::SystemException
  */

#include "afl/except/systemexception.hpp"

afl::except::SystemException::SystemException(afl::sys::Error error)
    : std::runtime_error(error.getText()),
      m_hint(),
      m_error(error)
{ }

afl::except::SystemException::SystemException(afl::sys::Error error, const String_t& hint)
    : std::runtime_error(error.getText()),
      m_hint(hint),
      m_error(error)
{ }

afl::except::SystemException::~SystemException() throw()
{ }
