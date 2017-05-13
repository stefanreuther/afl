/**
  *  \file afl/io/datasink.hpp
  *  \brief Interface afl::io::DataSink
  */
#ifndef AFL_AFL_IO_DATASINK_HPP
#define AFL_AFL_IO_DATASINK_HPP

#include "afl/base/deletable.hpp"
#include "afl/base/memory.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace io {

    /** Data sink base class.
        This is a simple interface for "push-style" data processing. */
    class DataSink : public afl::base::Deletable {
     public:
        /** Consume data.
            \param data [in/out] On input, data to process. On output, remaining unprocessed data.
            \retval false More data required (%data is empty)
            \retval true No more data required (%data has unprocessed data) */
        virtual bool handleData(afl::base::ConstBytes_t& data) = 0;

        /** Consume data, full version.
            Like handleData(), but throws a FileProblemException if not all data could be processed.
            \param data [in] Data to process. */
        void handleFullData(afl::base::ConstBytes_t data);
    };

} }

#endif
