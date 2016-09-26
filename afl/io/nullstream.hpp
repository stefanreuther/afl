/**
  *  \file afl/io/nullstream.hpp
  *  \brief Class afl::io::NullStream
  */
#ifndef AFL_AFL_IO_NULLSTREAM_HPP
#define AFL_AFL_IO_NULLSTREAM_HPP

#include "afl/io/multiplexablestream.hpp"

namespace afl { namespace io {

    /** Null stream.
        This stream discards all data written to it.
        When read, it behaves like an empty file.
        It tracks a current position and size, so it can be used for measuring write operations. */
    class NullStream : public MultiplexableStream {
     public:
        NullStream();
        virtual ~NullStream();
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
        FileSize_t m_pos;
        FileSize_t m_size;
    };

} }

#endif
