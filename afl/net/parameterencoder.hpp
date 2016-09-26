/**
  *  \file afl/net/parameterencoder.hpp
  *  \brief Class afl::net::ParameterEncoder
  */
#ifndef AFL_AFL_NET_PARAMETERENCODER_HPP
#define AFL_AFL_NET_PARAMETERENCODER_HPP

#include "afl/net/headerconsumer.hpp"

namespace afl { namespace net {

    /** URL Parameter Encoder.
        Implements a HeaderConsumer that encodes the given data as URL parameters.
        It can be used to build URLs.

        ParameterEncoder is stateful; you can call handleHeader() any number of times,
        for example, from multiple HeaderTable's. */
    class ParameterEncoder : public HeaderConsumer {
     public:
        /** Constructor.
            \param result [in/out] String to which the parameters will be appended (e.g. URL of a script)
            \param firstDelimiter [in,optional] First delimiter; defaults to '?'.

            The first call to handleHeader() will generate the given delimiter if nonzero.
            Subsequent calls will generate the intermediate '&' delimiter.
            Usecases therefore include:
            - firstDelimiter='?' (default): append parameters to a URL
            - firstDelimiter='&': continue appending to a list that already contains parameters.
              It is preferred to reuse the original ParameterEncoder, though, as that will avoid getting confused with the state.
            - firstDelimiter='\0': do not generate an initial delimiter, for generating HTTP POST form content. */
        ParameterEncoder(String_t& result, char firstDelimiter = '?');

        // HeaderConsumer:
        virtual void handleHeader(String_t key, String_t value);

     private:
        String_t& m_result;
        bool m_first;
        char m_firstDelimiter;
    };

} }

#endif
