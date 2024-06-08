/**
  *  \file afl/string/win32filenames.hpp
  *  \brief Class afl::string::Win32FileNames
  */
#ifndef AFL_AFL_STRING_WIN32FILENAMES_HPP
#define AFL_AFL_STRING_WIN32FILENAMES_HPP

#include "afl/string/string.hpp"

namespace afl { namespace string {

    /** Win32 File Name Algorithms.
        Implements file name arithmetics as appropriate for Win32.
        You can use these even if your afl::io::FileSystem implements a different mode.

        All file names are represented as UTF-8.
        Method names correspond to the method names of afl::io::FileSystem. */
    class Win32FileNames {
     public:
        /** Typedef for filenames. */
        typedef String_t FileName_t;

        /** Path separator character. */
        static const char PATH_SEPARATOR = '\\';

        /** Flip slashes.
            Win32 allows "\" and "/" to be used interchangably, but "\" is considered the canonical name.
            This converts "/" to "\".
            \param name [in/out] Name to convert */
        void flipSlashes(FileName_t& name) const;

        /** Check for absolute path name.
            An absolute path name refers to the same for all programs on one machine,
            and is therefore suitable for storing in configuration files, for example.

            \param path Path name to check, UTF-8 encoded.
            \return true iff this is an absolute path */
        bool isAbsolutePathName(FileName_t path) const;

        /** Check for path separator character.
            Path separator characters cannot appear in file names.

            \param c Character to check
            \return true iff this is a path separator */
        bool isPathSeparator(char c) const;

        /** Make path name from path and subsequent file name.
            Interprets \c name as a relative file name as seen from \c path.
            Constructs a joint path name that points to that file.

            \param path Origin path, UTF-8 encoded.
            \param name Relative file name, UTF-8 encoded.
            \return Combined file name, UTF-8 encoded. */
        FileName_t makePathName(FileName_t path, FileName_t name) const;

        /** Get canonical path.
            Resolves "." and ".." components, if possible.

            \param name Path name, UTF-8 encoded.
            \return Canonical path name, UTF-8 encoded. */
        FileName_t getCanonicalPathName(FileName_t name) const;

        /** Get file name.
            Splits the specified path name and returns the file name component.
            \param name Path name, UTF-8 encoded.
            \return File name component, UTF-8 encoded. */
        FileName_t getFileName(FileName_t name) const;

        /** Get directory name.
            Splits the specified path name and returns the directory name component.
            \param name Path name, UTF-8 encoded.
            \return Directory name component, UTF-8 encoded. */
        FileName_t getDirectoryName(FileName_t name) const;
    };

} }

#endif
