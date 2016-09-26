/**
  *  \file afl/io/nullfilesystem.hpp
  *  \brief Class afl::io::NullFileSystem
  */
#ifndef AFL_AFL_IO_NULLFILESYSTEM_HPP
#define AFL_AFL_IO_NULLFILESYSTEM_HPP

#include "afl/io/filesystem.hpp"
#include "afl/string/posixfilenames.hpp"

namespace afl { namespace io {

    /** Null file system.
        Implements the file system interface in a very simple way:
        - "open" functions fail by throwing a FileProblemException
        - path name arithmetic functions operate according to POSIX rules (afl::string::PosixFileNames)

        Use this whenever an interface requires a FileSystem but the component is not expected to actually access files. */
    class NullFileSystem : public FileSystem {
     public:
        /** Constructor. */
        NullFileSystem();

        /** Destructor. */
        ~NullFileSystem();

        // FileSystem:
        virtual afl::base::Ptr<Stream> openFile(FileName_t fileName, OpenMode mode);
        virtual afl::base::Ptr<Directory> openDirectory(FileName_t dirName);
        virtual afl::base::Ptr<Directory> openRootDirectory();
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
