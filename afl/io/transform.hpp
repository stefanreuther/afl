/**
  *  \file afl/io/transform.hpp
  *  \brief Interface afl::io::Transform
  */
#ifndef AFL_AFL_IO_TRANSFORM_HPP
#define AFL_AFL_IO_TRANSFORM_HPP

#include "afl/base/deletable.hpp"
#include "afl/base/memory.hpp"

namespace afl { namespace io {

    class Transform : public afl::base::Deletable {
     public:
        /** Transform data.
            Transforms as much input data as possible, producing as much output data as possible.
            As an invariant, this function should do one of the following:
            - consume all input data, partially filling the output buffer (if there's too much output buffer)
            - partially consume input data, completely filling the output buffer (if there's too little output buffer)

            In particular, if this function is called with nonempty input and nonempty output,
            it must at least consume one byte of input or produce one byte of output;
            it may not leave both pointers unchanged.

            \param in [in/out] On input, all input data. On return, contains unprocessed input data.
            \param out [in/out] In input, space for output data. On return, contains produced output data. */
        virtual void transform(afl::base::ConstBytes_t& in, afl::base::Bytes_t& out) = 0;

        /** Flush.
            Signals end of the input stream.
            You should call transform() with an empty input buffer to obtain pending buffered data
            (for example, an "end" packet). */
        virtual void flush() = 0;
    };

} }

#endif
