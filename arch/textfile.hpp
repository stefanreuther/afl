/**
  *  \file arch/textfile.hpp
  *  \brief System-dependant Part of afl/io/textfile.cpp
  */
#ifndef AFL_ARCH_TEXTFILE_HPP
#define AFL_ARCH_TEXTFILE_HPP

#include "afl/base/types.hpp"

#if TARGET_OS_POSIX
namespace {
    const uint8_t NEWLINE[] = { '\n' };
}
#elif TARGET_OS_WIN32
namespace {
    const uint8_t NEWLINE[] = { '\r', '\n' };
}
#else
# error Teach me about your text files
#endif

#endif
