/**
  *  \file afl/io/limiteddatasink.hpp
  *  \brief Class afl::io::LimitedDataSink
  */
#ifndef AFL_AFL_IO_LIMITEDDATASINK_HPP
#define AFL_AFL_IO_LIMITEDDATASINK_HPP

#include <memory>
#include "afl/io/datasink.hpp"
#include "afl/io/stream.hpp"

namespace afl { namespace io {

    /** Limited data sink.
        This limits the amount of data it accepts to a constant number of bytes. */
    class LimitedDataSink : public DataSink {
     public:
        /** Constructor.
            \param peer Target data sink. Lifetime must exceed that of the LimitedDataSink.
            \param limit Number of bytes to accept and pass on */
        LimitedDataSink(DataSink& peer, Stream::FileSize_t limit);

        /** Destructor. */
        ~LimitedDataSink();

        // DataSink:
        virtual bool handleData(const String_t& name, afl::base::ConstBytes_t& data);

     private:
        DataSink& m_peer;
        Stream::FileSize_t m_limit;
    };

} }

#endif
