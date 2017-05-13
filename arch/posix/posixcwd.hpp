/**
  *  \file arch/posix/posixcwd.hpp
  */
#ifndef AFL_ARCH_POSIX_POSIXCWD_HPP
#define AFL_ARCH_POSIX_POSIXCWD_HPP

#if TARGET_OS_POSIX
#include "afl/string/string.hpp"
#include "afl/io/filesystem.hpp"

namespace arch { namespace posix {

    afl::io::FileSystem::FileName_t getWorkingDirectoryName();
    afl::io::FileSystem::FileName_t getAbsolutePathName(afl::io::FileSystem::FileName_t name);

} }
#else
# error Invalid operating system
#endif

#endif
