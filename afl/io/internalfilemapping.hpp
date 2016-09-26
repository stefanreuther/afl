/**
  *  \file afl/io/internalfilemapping.hpp
  *  \brief Class afl::io::InternalFileMapping
  */
#ifndef AFL_AFL_IO_INTERNALFILEMAPPING_HPP
#define AFL_AFL_IO_INTERNALFILEMAPPING_HPP

#include <vector>
#include "afl/io/filemapping.hpp"
#include "afl/io/stream.hpp"
#include "afl/base/growablememory.hpp"

namespace afl { namespace io {

    /** Internal (Pseudo) File Mapping.
        This implements the contract of FileMapping by reading into an internal buffer.
        This allows using code the needs a FileMapping for files that the operating system cannot map.

        You typically use FileMapping::create() to create these objects. */
    class InternalFileMapping : public FileMapping {
     public:
        /** Constructor.
            \param stream Input stream. Object needs to be valid for the duration of the constructor call, but no longer.
            \param limit Size limit. Load at most this many bytes. */
        InternalFileMapping(Stream& stream, Stream::FileSize_t limit = Stream::FileSize_t(-1));

        /** Destructor. */
        virtual ~InternalFileMapping();

        // FileMapping:
        virtual afl::base::ConstBytes_t get() const;

     private:
        afl::base::GrowableMemory<uint8_t> m_data;

        void init(Stream& stream, Stream::FileSize_t limit);
    };

} }

#endif
