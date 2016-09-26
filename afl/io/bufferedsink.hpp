/**
  *  \file afl/io/bufferedsink.hpp
  *  \brief Class afl::io::BufferedSink
  */
#ifndef AFL_AFL_IO_BUFFEREDSINK_HPP
#define AFL_AFL_IO_BUFFEREDSINK_HPP

#include "afl/io/datasink.hpp"

namespace afl { namespace io {

    /** Buffered data sink.
        Attempts to increase efficiency by writing larger blocks.
        Small data blocks are combined before being written. */
    class BufferedSink : public DataSink {
     public:
        /** Constructor.
            \param sink Data sink that receives the buffered data. Lifetime must exceed that of the BufferedSink. */
        BufferedSink(DataSink& sink);

        // DataSink:
        virtual ~BufferedSink();
        virtual bool handleData(const String_t& name, afl::base::ConstBytes_t& data);

        /** Flush buffered data.
            Writes all pending data to the output stream, even if the buffer is not yet filled. */
        void flush();

     private:
        /** Output data sink. */
        DataSink& m_sink;

        /** Name for reporting. */
        String_t m_lastName;

        /** Buffer descriptor. */
        afl::base::Bytes_t m_buffer;

        /** Buffer level for writing. */
        size_t m_bufferFill;

        /** Buffer data. Accessed through m_buffer. */
        uint8_t m_rawBuffer[1024];
    };

} }

#endif
