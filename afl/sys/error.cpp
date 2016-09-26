/**
  *  \file afl/sys/error.cpp
  *  \brief Class afl::sys::Error
  */

#include "afl/sys/error.hpp"
#include "arch/error.hpp"

afl::sys::Error::Error()
    : m_number()
{ }

afl::sys::Error::Error(ErrorNumber_t code)
    : m_number(code)
{ }

afl::sys::Error
afl::sys::Error::current()
{
    return Error(errorNumber());
}

String_t
afl::sys::Error::getText()
{
    return errorText(m_number);
}
