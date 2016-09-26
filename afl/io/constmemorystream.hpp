/**
  *  \file afl/io/constmemorystream.hpp
  *  \brief Class afl::io::ConstMemoryStream
  */
#ifndef AFL_AFL_IO_CONSTMEMORYSTREAM_HPP
#define AFL_AFL_IO_CONSTMEMORYSTREAM_HPP

#include "afl/io/stream.hpp"

namespace afl { namespace io {

    /** Stream reading a fixed Memory object.
        This class allows reading from a byte sequence
        identified by a Memory<const uint8_t> (Stream::ConstMemory_t).
        \see MemoryStream */
    class ConstMemoryStream : public Stream {
     public:
        /** Constructor.
            \param mem Byte sequence to read from */
        ConstMemoryStream(ConstBytes_t mem);

        /** Destructor. */
        virtual ~ConstMemoryStream();

        // Stream methods:
        virtual size_t read(Bytes_t m);
        virtual size_t write(ConstBytes_t m);
        virtual void flush();
        virtual void setPos(FileSize_t pos);
        virtual FileSize_t getPos();
        virtual FileSize_t getSize();
        virtual uint32_t getCapabilities();
        virtual String_t getName();
        virtual afl::base::Ptr<Stream> createChild();
        virtual afl::base::Ptr<FileMapping> createFileMapping(FileSize_t limit);

     private:
        ConstBytes_t m_mem;
        size_t m_pos;
    };

} }

#endif
