/**
  *  \file afl/io/filemapping.hpp
  *  \brief Class afl::io::FileMapping
  */
#ifndef AFL_AFL_IO_FILEMAPPING_HPP
#define AFL_AFL_IO_FILEMAPPING_HPP

#include "afl/base/memory.hpp"
#include "afl/base/deletable.hpp"
#include "afl/base/refcounted.hpp"

namespace afl { namespace io {

    /** Read-only file mapping.
        Maps (a section of) a file into memory for reading.
        Use Stream::createFileMapping() or Stream::createVirtualMapping() to create an instance of this class.
        - createVirtualMapping() will create a real, operating-system supported mapping if possible,
          or load the file into an internal buffer otherwise (InternalFileMapping).
        - createFileMapping() will create an operating-system supported mapping if possible,
          return null otherwise.

        Operating-system supported mappings will save memory because the data is accessed directly from the operating system's buffer cache,
        without having to copy it into this process's address space.
        On the downside, I/O errors may cause your process to terminate (SIGBUS etc.).

        This mechanism is intended for medium-sized files or file sections (dozen kilobytes up to a few megabytes).
        Small files are more efficiently accessed by reading normally (you could use InternalFileMapping directly);
        large files may eat up too much of the process' address space
        (or real memory, if an operating-system supported mapping is not available).

        There are no restrictions about sizes and positions;
        if unaligned mappings are attempted, those are enlarged to match page borders.
        It is unspecified, whether (and when) modifications to the underlying file show up in the mapping. */
    class FileMapping : public afl::base::Deletable, public afl::base::RefCounted {
     public:
        /** Get content of file mapping.
            \return Memory descriptor, describing content of the mapped file */
        virtual afl::base::ConstBytes_t get() const = 0;
    };

} }

#endif
