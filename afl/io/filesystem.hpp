/**
  *  \file afl/io/filesystem.hpp
  *  \brief Class afl::io::FileSystem
  */
#ifndef AFL_AFL_IO_FILESYSTEM_HPP
#define AFL_AFL_IO_FILESYSTEM_HPP

#include "afl/base/ptr.hpp"
#include "afl/string/string.hpp"
#include "afl/base/deletable.hpp"

namespace afl { namespace io {

    class Stream;
    class Directory;

    /** File System.
        Contains functions to work with a real (or virtual) file system:
        - create Directory or Stream objects to access the files in the file system
        - functions to manipulate file names

        There is a global instance (getInstance()) describing the actual host file system.
        Alternative instances are possible to traverse non-native or pseudo file systems. */
    class FileSystem : public afl::base::Deletable {
     public:
        enum OpenMode {
            OpenRead,           ///< Open for reading only. Fail if does not exist.
            OpenWrite,          ///< Open for read and write. Fail if it does not exist.
            Create,             ///< Create anew and open for read and write. Truncate if it already exists.
            CreateNew           ///< Create anew. Fail if it already exists.
        };

        typedef String_t FileName_t;


        /*
         *  Opening
         */

        /** Open a file given its name.
            File names are provided in UTF-8, but otherwise use native syntax
            (e.g. "../foo" to open a file one-up from the current working directory).
            \param fileName Name of file, UTF-8.
            \param mode Mode
            \return Stream object, never null
            \throw FileProblemException if the file cannot be opened */
        virtual afl::base::Ptr<Stream> openFile(FileName_t fileName, OpenMode mode) = 0;

        /** Open a directory given its name.
            Directory names are provided in UTF-8, but otherwise use native syntax.
            \param dirName Name of directory, UTF-8
            \return Directory object, never null
            \throw FileProblemException if the directory cannot be opened
            (note, however, there's no guarantee that the error is detected upon construction). */
        virtual afl::base::Ptr<Directory> openDirectory(FileName_t dirName) = 0;

        /** Open virtual root directory.
            The virtual root directory contains references to actual, physical directories
            (or further virtual directories).

            This is intended to allow a platform-neutral enumeration of possible entry points into a file system.
            Entry points could be drive letters, home directories, etc.
            \return Directory object, never null */
        virtual afl::base::Ptr<Directory> openRootDirectory() = 0;


        /*
         *  File Name Operations
         */

        /** Check for absolute path name.
            An absolute path name refers to the same for all programs on one machine,
            and is therefore suitable for storing in configuration files, for example.

            Examples:
            - POSIX: "/usr/bin", "/", but not "../tmp"
            - Win32: "c:\windows", but not "..\tmp" or "c:tmp"

            \param path Path name to check, UTF-8 encoded.
            \return true iff this is an absolute path */
        virtual bool isAbsolutePathName(FileName_t path) = 0;

        /** Check for path separator character.
            Path separator characters cannot appear in file names.

            Examples:
            - POSIX: "/"
            - Win32: "/", "\", ":"

            \param c Character to check
            \return true iff this is a path separator */
        virtual bool isPathSeparator(char c) = 0;

        /** Make path name from path and subsequent file name.
            Interprets #name as a relative file name as seen from #path.
            Constructs a joint path name that points to that file,
            usually by concatenating both with a path separator inbetween
            (unless, for example, the #name already is an absolute name).

            The result is neither guaranteed to be canonical nor absolute.

            \param path Origin path, UTF-8 encoded.
            \param name Relative file name, UTF-8 encoded.
            \return Combined file name, UTF-8 encoded. */
        virtual FileName_t makePathName(FileName_t path, FileName_t name) = 0;

        /** Get canonical path.
            Resolves ambiguous/superfluous path components
            and returns the shortest possible equivalent form of the path name.
            For example, removes "." and ".." tokens from a path name.
            The result is, however, not necessarily absolute.

            \param name Path name, UTF-8 encoded.
            \return Canonical path name, UTF-8 encoded. */
        virtual FileName_t getCanonicalPathName(FileName_t name) = 0;

        /** Get absolute path.
            Adds missing path components to make the path name absolute
            (see isAbsolutePathName()).
            The result is absolute and canonical.

            \param name Path name, UTF-8 encoded.
            \return Absolute path, UTF-8 encoded. */
        virtual FileName_t getAbsolutePathName(FileName_t name) = 0;

        /** Get file name.
            Splits the specified path name and returns the file name component.
            \param name Path name, UTF-8 encoded.
            \return File name component, UTF-8 encoded. */
        virtual FileName_t getFileName(FileName_t name) = 0;

        /** Get directory name.
            Splits the specified path name and returns the directory name component.
            \param name Path name, UTF-8 encoded.
            \return Directory name component, UTF-8 encoded. */
        virtual FileName_t getDirectoryName(FileName_t name) = 0;

        /** Get name of current working directory.
            \return Working directory name, UTF-8 encoded. */
        virtual FileName_t getWorkingDirectoryName() = 0;

        /** Get instance.
            This instance provides access to the host's file system. */
        static FileSystem& getInstance();
    };

} }

#endif
