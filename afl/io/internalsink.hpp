/**
  *  \file afl/io/internalsink.hpp
  *  \brief Class afl::io::InternalSink
  */
#ifndef AFL_AFL_IO_INTERNALSINK_HPP
#define AFL_AFL_IO_INTERNALSINK_HPP

#include "afl/io/datasink.hpp"
#include "afl/base/growablememory.hpp"

namespace afl { namespace io {

    /** Internal (In-Memory) Sink.
        Internally, the sink is represented as a GrowableMemory<uint8_t>, and grows with each append. */
    class InternalSink : public DataSink {
     public:
        /** Constructor. */
        InternalSink();

        // DataSink:
        virtual ~InternalSink();
        virtual bool handleData(const String_t& name, afl::base::ConstBytes_t& data);

        /** Get content of stream.
            \return Handle to the content of the sink.
            This handle is valid up to the next handleData() call. */
        afl::base::ConstBytes_t getContent() const;

        /** Clear. */
        void clear();

     private:
        afl::base::GrowableMemory<uint8_t> m_data;
    };

} }


#endif
