/**
  *  \file arch/posix/posixcwd.cpp
  */

#if TARGET_OS_POSIX
#include <vector>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include "arch/posix/posixcwd.hpp"
#include "afl/except/systemexception.hpp"
#include "arch/posix/posix.hpp" // convertExternalToUtf8
#include "afl/string/posixfilenames.hpp"

afl::io::FileSystem::FileName_t
arch::posix::getWorkingDirectoryName()
{
    // FIXME: maybe it makes sense to handle symlink-awareness of some shells here,
    // i.e.
    //   if ($PWD exists && $PWD and "." refer to same inode)
    //   then return $PWD
    //   else get physical directory name

    std::vector<char> buffer;
    while (1) {
        buffer.resize(buffer.size() + PATH_MAX);
        if (getcwd(&buffer[0], buffer.size())) {
            break;
        }
        if (errno != ERANGE) {
            throw afl::except::SystemException(afl::sys::Error::current(), "<getWorkingDirectoryName>");
        }
    }

    return convertExternalToUtf8(afl::string::toMemory(&buffer[0]));

}

afl::io::FileSystem::FileName_t
arch::posix::getAbsolutePathName(afl::io::FileSystem::FileName_t name)
{
    afl::string::PosixFileNames fs;
    if (fs.isAbsolutePathName(name)) {
        return fs.getCanonicalPathName(name);
    } else {
        return fs.getCanonicalPathName(fs.makePathName(getWorkingDirectoryName(), name));
    }
}

#else
int g_variableToMakePosixCwdObjectFileNotEmpty;
#endif
