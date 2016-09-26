/**
  *  \file afl/io/bufferedstream.hpp
  *  \brief Class afl::io::BufferedStream
  */
#ifndef AFL_AFL_IO_BUFFEREDSTREAM_HPP
#define AFL_AFL_IO_BUFFEREDSTREAM_HPP

#include "afl/io/stream.hpp"
#include "afl/base/memory.hpp"
#include "afl/base/uncopyable.hpp"
#include "afl/io/multiplexablestream.hpp"

namespace afl { namespace io {

    /** Buffered stream access.
        Provides buffered access to an underlying Stream object,
        so that small read() and write() calls do not immediately go down to the underlying stream,
        but go through a buffer, saving system calls.

        BufferedStream also enriches the Stream interface with methods for bytewise read. */
    class BufferedStream : public MultiplexableStream, public afl::base::Uncopyable {
     public:
        /** Constructor.
            \param s Underlying stream. Lifetime must be managed such that #s outlives the BufferedStream object. */
        BufferedStream(Stream& s);

        /** Destructor. */
        virtual ~BufferedStream();

        // Stream:
        virtual size_t read(Bytes_t m);
        virtual size_t write(ConstBytes_t m);
        virtual void setPos(FileSize_t pos);
        virtual FileSize_t getPos();
        virtual FileSize_t getSize();
        virtual uint32_t getCapabilities();
        virtual String_t getName();
        virtual afl::base::Ptr<FileMapping> createFileMapping(FileSize_t limit);

        /*
         *  BufferedStream methods:
         */

        /** Synchronize buffers.
            Makes sure that the underlying stream state matches the BufferedStream state:
            - pending writes are performed
            - if reading read too much, the file pointer is rewound (setPos).
            \throw FileProblemException if there is a problem */
        virtual void flush();

        /** Read a byte.
            If a byte can be read, returns a pointer to it and advance the read position.
            Otherwise, return null.
            The pointer is valid up to the next method call on the BufferedStream object.
            \return pointer */
        const uint8_t* readByte();

        /** Read a byte, but don't remove it from the stream.
            If a byte can be read, returns a pointer to it but do not advance the read position.
            Otherwise, return null.
            The pointer is valid up to the next method call on the BufferedStream object.
            \return pointer */
        const uint8_t* peekByte();

     private:
        /** Last operation. */
        enum Mode {
            /** No last operation. BufferedStream state matches underlying stream state. */
            Neutral,
            /** Last operation was read. m_buffer contains bytes that were already read
                from the underlying stream. */
            Reading,
            /** Last operation was write. m_buffer contains room for bytes that have not yet
                been written to the underlying stream; m_bufferFill contains the number of
                such bytes. */
            Writing
        };

        /** Last operation. */
        Mode m_mode;

        /** Unserlying stream. */
        Stream& m_stream;

        /** Buffer descriptor. Validity depends on m_mode. */
        afl::base::Bytes_t m_buffer;

        /** Buffer level for writing. Validity depends on m_mode. */
        size_t m_bufferFill;

        /** Buffer data. Accessed through m_buffer. */
        uint8_t m_rawBuffer[1024];

        /** Set mode. Prepares for the operation specified as #m.
            \param m New operation. */
        void setMode(Mode m);

        /** Refill buffer for reading.
            Call when reading, whenever #m_buffer is empty. */
        void refillBuffer();

        /** Flush buffer after writing. */
        void flushBuffer();
    };

} }

#endif
