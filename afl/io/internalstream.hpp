/**
  *  \file afl/io/internalstream.hpp
  *  \brief Class afl::io::InternalStream
  */
#ifndef AFL_AFL_IO_INTERNALSTREAM_HPP
#define AFL_AFL_IO_INTERNALSTREAM_HPP

#include <vector>
#include "afl/io/multiplexablestream.hpp"
#include "afl/base/growablememory.hpp"

namespace afl { namespace io {

    /** Internal (In-Memory) Stream.
        Implements a virtual file that can be read and written.
        Internally, the file is represented as a GrowableBytes_t.

        When written, the file grows as needed.
        Writing can be disabled, making this class suitable as a "I give you this memory buffer to read from" stream
        that internally manages the memory it contains (unlike ConstMemoryStream). */
    class InternalStream : public MultiplexableStream {
     public:
        /** Constructor.
            Makes an empty stream. */
        InternalStream();

        /** Destructor. */
        virtual ~InternalStream();

        // Stream methods:
        virtual size_t read(Bytes_t m);
        virtual size_t write(ConstBytes_t m);
        virtual void flush();
        virtual void setPos(FileSize_t pos);
        virtual FileSize_t getPos();
        virtual FileSize_t getSize();
        virtual uint32_t getCapabilities();
        virtual String_t getName();
        virtual afl::base::Ptr<FileMapping> createFileMapping(FileSize_t limit);

        // InternalStream:

        /** Set name of this stream.
            This is the name that is reported using getName().
            \param name New name */
        void setName(const String_t& name);

        /** Get content of stream.
            \return Handle to the content of the stream. */
        ConstBytes_t getContent() const;

        /** Set write permission.
            \param allowWrite true to allow writing. This is the default. */
        void setWritePermission(bool allowWrite);

     private:
        String_t m_name;
        afl::base::GrowableBytes_t m_data;
        size_t m_pos;
        bool m_allowWrite;
    };

} }

#endif
