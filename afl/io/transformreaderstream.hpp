/**
  *  \file afl/io/transformreaderstream.hpp
  *  \brief Class afl::io::TransformReaderStream
  */
#ifndef AFL_AFL_IO_TRANSFORMREADERSTREAM_HPP
#define AFL_AFL_IO_TRANSFORMREADERSTREAM_HPP

#include <memory>
#include "afl/io/stream.hpp"
#include "afl/io/multiplexablestream.hpp"

namespace afl { namespace io {

    class Transform;

    /** Transformation Reader.
        Provides a stream that allows sequential access to data produced by transforming another stream.
        For example, if a stream 'X' contains compressed data, TransformReaderStream('X', 'DecompressorTransform')
        will read the decompressed data.

        TransformReaderStream provides sequential access only.
        setPos() and createChild() requests are executed successfully only if they don't change the file position. */
    class TransformReaderStream : public MultiplexableStream {
     public:
        TransformReaderStream(Stream& stream, Transform& transform);
        ~TransformReaderStream();

        virtual size_t read(Bytes_t m);
        virtual size_t write(ConstBytes_t m);
        virtual void flush();
        virtual void setPos(FileSize_t pos);
        virtual FileSize_t getPos();
        virtual FileSize_t getSize();
        virtual uint32_t getCapabilities();
        virtual String_t getName();
        virtual afl::base::Ptr<FileMapping> createFileMapping(FileSize_t limit = FileSize_t(-1));

     private:
        Stream& m_stream;
        Transform& m_transform;

        uint8_t m_inputBuffer[2048];
        afl::base::ConstBytes_t m_input;

        FileSize_t m_numBytesRead;
        bool m_flushed;
    };

} }

#endif
