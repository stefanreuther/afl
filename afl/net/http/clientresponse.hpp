/**
  *  \file afl/net/http/clientresponse.hpp
  *  \brief Class afl::net::http::ClientResponse
  */
#ifndef AFL_AFL_NET_HTTP_CLIENTRESPONSE_HPP
#define AFL_AFL_NET_HTTP_CLIENTRESPONSE_HPP

#include "afl/base/memory.hpp"
#include "afl/net/headerconsumer.hpp"
#include "afl/net/headerparser.hpp"
#include "afl/io/stream.hpp"
#include "afl/net/headertable.hpp"
#include "afl/io/datasink.hpp"

namespace afl { namespace net { namespace http {

    class ClientRequest;

    /** HTTP Client: Response Parser.
        This class parses all headers of a HTTP response.
        In particular, it determines whether a message body is present
        and if so, how it should be parsed. */
    class ClientResponse : public afl::io::DataSink, private HeaderConsumer {
     public:
        /** Type for file sizes. */
        typedef afl::io::Stream::FileSize_t FileSize_t;

        /** Type of limitation of the response. */
        enum Limit {
            /** The server (or the protocol) gave an explicit byte limit. */
            ByteLimit,

            /** The stream is in chunked encoding which implicitly determines the end. */
            ChunkLimit,

            /** The server did not give an explicit limit.
                The response ends when the server closes the connection. */
            StreamLimit
        };

        /** Type of response encoding. */
        enum Encoding {
            /** Content is not encoded in any way. */
            IdentityEncoding,

            /** Content is encoded using gzip. */
            GzipEncoding,

            /** Content is encoded using deflate. */
            DeflateEncoding,

            /** Content is encoded in a way we do not know. */
            UnknownEncoding
        };

        /** Constructor.
            \param isHeadRequest true if this is the response to a HEAD request, false otherwise */
        ClientResponse(bool isHeadRequest);

        /** Destructor. */
        ~ClientResponse();

        /** Handle data.
            \param data [in/out] Reference to data to process
            \retval true Header parsing complete, do not call this method again
            \retval false Header parsing incomplete, call this method again */
        bool handleData(const String_t& name, afl::base::ConstBytes_t& data);

        /** Check for errors.
            Can be used after headers have been received to check for errors.
            An error is a protocol violation, that is, we do not understand what the server is trying to tell us.
            Therefore, if there are errors, the other accessors do not have reliable values.

            An error response provided by the server ("404 not found") is not an error.
            \retval true There were errors; close the connection
            \retval false No errors; proceed */
        bool hasErrors() const;

        /** Check for presence of a message body.
            \retval true This message includes a message body.
            \retval false This message does not include a message body (getResponseLength() will return 0) */
        bool hasBody() const;

        /** Get status code.
            \return HTTP status code ("200", "404") */
        uint32_t getStatusCode() const;

        /** Get status phrase.
            \return HTTP status phrase ("OK", "Not found"). */
        const String_t& getStatusPhrase() const;

        /** Get HTTP version indicated by server.
            \return version as a string ("HTTP/1.0") */
        const String_t& getVersion() const;

        /** Get type of response limit.
            This determines how the client can detect the end of the response,
            and is determined from the HTTP Transfer-Encoding header, among others.
            \return limit type */
        Limit getResponseLimitKind() const;

        /** Get encoding of response.
            This determines how the data is encoded (HTTP Content-Encoding).
            \return encoding */
        Encoding getResponseEncoding() const;

        /** Get response length.
            This determines the total length of the response data.
            If the data is encoded, this is the size of the encoded data.
            If a partial object was requested, it is the size of the part.
            If the response does not include data (e.g. for a HEAD request),
            this is zero.

            This value is valid only when getResponseLimitKind() == ByteLimit.
            \return data length in bytes */
        FileSize_t getResponseLength() const;

        /** Get response offset.
            If a partial object was requested, this determines the offset
            of the first byte returned (which may or may not be the first requested byte).
            \return byte offset */
        FileSize_t getResponseOffset() const;

        /** Get total length of object.
            This determines the total size of the object on the server.
            For example, for a HEAD request this is the file size.
            \return object length */
        FileSize_t getTotalLength() const;

        /** Check for connection-keepalive.
            \retval true keep the connection open
            \retval false close the connection after receiveing the body */
        bool isKeepalive() const;

        /** Get response headers. */
        const HeaderTable& getResponseHeaders();

        /** Get response cookies. */
        const HeaderTable& getResponseCookies();

     private:
        /*
         *  Parsing
         */
        enum State {
            ParseVersion,          // Looking for HTTP version ("HTTP/1.0")
            ParseStatusCode,       // Looking for HTTP response code ("404")
            ParseStatusPhrase,     // Looking for HTTP reason phrase ("Not found")
            ParseHeader,           // Parsing headers
            FinalError,            // Final state: protocol error
            FinalSuccess           // Final state: success
        };
        State m_state;             // Parser state
        String_t m_version;        // HTTP version, as string
        uint32_t m_statusCode;     // Status code
        String_t m_statusPhrase;   // Status phrase

        /*
         *  Request
         */
        bool m_isHeadRequest;      // true if this is the response to HEAD

        /*
         *  Response headers
         */
        // Content-Length header
        bool m_hasContentLength;
        afl::io::Stream::FileSize_t m_contentLength;

        // Content-Range header ("bytes start-end/total")
        bool m_hasContentRange;
        afl::io::Stream::FileSize_t m_contentRangeStart;
        afl::io::Stream::FileSize_t m_contentRangeLength;
        afl::io::Stream::FileSize_t m_contentRangeTotal;

        // Connection header ("keep-alive"/"close")
        bool m_hasConnection;
        bool m_isConnectionKeepalive;

        // Transfer-Encoding header ("chunked")
        bool m_hasChunkedTransferEncoding;

        // Content-Encoding ("gzip", ...)
        Encoding m_contentEncoding;

        // User headers
        HeaderTable m_headers;
        HeaderTable m_cookies;

        HeaderParser m_headerParser;

        // HeaderConsumer:
        virtual void handleHeader(String_t key, String_t value);
    };

} } }

inline uint32_t
afl::net::http::ClientResponse::getStatusCode() const
{
    return m_statusCode;
}

inline const String_t&
afl::net::http::ClientResponse::getStatusPhrase() const
{
    return m_statusPhrase;
}

inline const String_t&
afl::net::http::ClientResponse::getVersion() const
{
    return m_version;
}

inline const afl::net::HeaderTable&
afl::net::http::ClientResponse::getResponseHeaders()
{
    return m_headers;
}

inline const afl::net::HeaderTable&
afl::net::http::ClientResponse::getResponseCookies()
{
    return m_cookies;
}

#endif
