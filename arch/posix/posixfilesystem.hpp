/**
  *  \file arch/posix/posixfilesystem.hpp
  *  \brief Class arch::posix::PosixFileSystem
  */
#ifndef AFL_ARCH_POSIX_POSIXFILESYSTEM_HPP
#define AFL_ARCH_POSIX_POSIXFILESYSTEM_HPP

#include "afl/io/filesystem.hpp"
#include "afl/string/posixfilenames.hpp"

namespace arch { namespace posix {

    /** Implementation of FileSystem for POSIX. */
    class PosixFileSystem : public afl::io::FileSystem {
     public:
        virtual afl::base::Ptr<afl::io::Stream> openFile(FileName_t fileName, OpenMode mode);
        virtual afl::base::Ptr<afl::io::Directory> openDirectory(FileName_t dirName);
        virtual afl::base::Ptr<afl::io::Directory> openRootDirectory();

        /* File Name Operations */
        virtual bool isAbsolutePathName(FileName_t path);
        virtual bool isPathSeparator(char c);
        virtual FileName_t makePathName(FileName_t path, FileName_t name);
        virtual FileName_t getCanonicalPathName(FileName_t name);
        virtual FileName_t getAbsolutePathName(FileName_t name);
        virtual FileName_t getFileName(FileName_t name);
        virtual FileName_t getDirectoryName(FileName_t name);
        virtual FileName_t getWorkingDirectoryName();

     private:
        afl::string::PosixFileNames m_fileNames;
    };

} }

#endif
