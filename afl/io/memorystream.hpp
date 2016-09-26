/**
  *  \file afl/io/memorystream.hpp
  *  \brief Class afl::io::MemoryStream
  */
#ifndef AFL_AFL_IO_MEMORYSTREAM_HPP
#define AFL_AFL_IO_MEMORYSTREAM_HPP

#include "afl/io/multiplexablestream.hpp"

namespace afl { namespace io {

    /** Stream accessing a fixed Memory object.
        This class allows reading and writing to a byte sequence
        identified by a Memory<uint8_t> (Bytes_t).
        \see ConstMemoryStream */
    class MemoryStream : public MultiplexableStream {
     public:
        /** Constructor.
            \param mem Byte sequence to access */
        MemoryStream(Bytes_t mem);

        /** Destructor. */
        virtual ~MemoryStream();

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

     private:
        Bytes_t m_mem;
        size_t m_pos;
    };

} }

#endif
