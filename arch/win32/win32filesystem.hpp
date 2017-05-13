/**
  *  \file arch/win32/win32filesystem.hpp
  *  \brief Class arch::win32::Win32FileSystem
  */
#ifndef AFL_ARCH_WIN32_WIN32FILESYSTEM_HPP
#define AFL_ARCH_WIN32_WIN32FILESYSTEM_HPP

#include "afl/io/filesystem.hpp"
#include "afl/string/win32filenames.hpp"

namespace arch { namespace win32 {

    /** FileSystem implementation for Win32.
        Implements all file operations for Windows,
        using ANSI or Unicode calls (Win95 vs. WinNT). */
    class Win32FileSystem : public afl::io::FileSystem {
     public:
        virtual afl::base::Ref<afl::io::Stream> openFile(FileName_t fileName, OpenMode mode);
        virtual afl::base::Ref<afl::io::Directory> openDirectory(FileName_t dirName);
        virtual afl::base::Ref<afl::io::Directory> openRootDirectory();

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
        afl::string::Win32FileNames m_fileNames;
    };

} }

#endif
