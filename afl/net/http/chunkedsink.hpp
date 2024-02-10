/**
  *  \file afl/net/http/chunkedsink.hpp
  *  \brief Class afl::net::http::ChunkedSink
  */
#ifndef AFL_AFL_NET_HTTP_CHUNKEDSINK_HPP
#define AFL_AFL_NET_HTTP_CHUNKEDSINK_HPP

#include <memory>
#include "afl/io/datasink.hpp"
#include "afl/io/stream.hpp"
#include "afl/base/uncopyable.hpp"

namespace afl { namespace net { namespace http {

    /** HTTP "chunked" encoding decoder.
        Decodes a chunked body and gives the decoded data to another DataSink.
        Grammar according to RFC 2616:
        <pre>
               Chunked-Body   = *chunk
                                last-chunk
                                trailer
                                CRLF

               chunk          = chunk-size [ chunk-extension ] CRLF
                                chunk-data CRLF
               chunk-size     = 1*HEX
               last-chunk     = 1*("0") [ chunk-extension ] CRLF

               chunk-extension= *( ";" chunk-ext-name [ "=" chunk-ext-val ] )
               chunk-ext-name = token
               chunk-ext-val  = token | quoted-string
               chunk-data     = chunk-size(OCTET)
               trailer        = *(entity-header CRLF)
        </pre>
        chunk-extension and trailer are completely ignored. */
    class ChunkedSink : public afl::io::DataSink, private afl::base::Uncopyable {
     public:
        /** Constructor.
            \param peer Target sink that receives data. Lifetime must exceed that of the ChunkedSink. */
        ChunkedSink(afl::io::DataSink& peer);

        /** Destructor. */
        virtual ~ChunkedSink();

        // DataSink:
        virtual bool handleData(afl::base::ConstBytes_t& data);

     private:
        enum State {
            ChunkSize,              // reading hex digits
            Extension,              // skipping to EOL
            Payload,                // reading payload
            PayloadEnd,             // reading the CRLF after the payload
            Trailer,                // reading the file trailer
            TrailerHeader,          // reading a header in the file trailer
            Final                   // final state reached, not reading anymore
        };
        State m_state;
        afl::io::DataSink& m_peer;
        afl::io::Stream::FileSize_t m_size;
    };

} } }

#endif
