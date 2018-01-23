/**
  *  \file afl/net/line/linesink.hpp
  *  \brief Interface afl::net::line::LineSink
  */
#ifndef AFL_AFL_NET_LINE_LINESINK_HPP
#define AFL_AFL_NET_LINE_LINESINK_HPP

#include "afl/base/deletable.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace net { namespace line {

    /** Sink for text lines.
        This interface is used for LineHandler. */
    class LineSink : public afl::base::Deletable {
     public:
        /** Handle a line.
            This typically queues the line for sending.
            \param line Line */
        virtual void handleLine(const String_t& line) = 0;
    };

} } }

#endif
