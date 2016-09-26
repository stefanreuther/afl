/**
  *  \file afl/net/http/downloadlistener.hpp
  *  \brief Interface afl::net::http::DownloadListener
  */
#ifndef AFL_AFL_NET_HTTP_DOWNLOADLISTENER_HPP
#define AFL_AFL_NET_HTTP_DOWNLOADLISTENER_HPP

#include "afl/io/stream.hpp"
#include "afl/base/deletable.hpp"
#include "afl/net/http/clientrequest.hpp"
#include "afl/string/string.hpp"
#include "afl/base/memory.hpp"

namespace afl { namespace net { namespace http {

    class ClientResponse;

    /** Download listener.
        Listens to download requests submitted with a Manager.
        The DownloadListener object is owned by the user
        (unlike the ClientRequest object which is owned by the http::Client).
        It must live until the request completes, that is, either handleFailure()
        or handleSuccess() is called.

        Its methods are called from the http::Client's thread and should not block;
        blocking will block all network operation. */
    class DownloadListener : public afl::base::Deletable {
     public:
        /** Request processing: response header completed.
            This function is called when the response headers have been received.
            The receiver must check for a successful status (resp.getStatusCode() == 200).
            \param resp Response headers */
        virtual void handleResponseHeader(ClientResponse& resp) = 0;

        /** Request processing: response data.
            This function is called for each chunk of data received from the server.
            \param ofs File position of this data in downloaded file
            \param data Descriptor for the data */
        virtual void handleResponseData(afl::io::Stream::FileSize_t ofs, afl::base::ConstBytes_t data) = 0;

        /** Request finalisation: failure.
            This function is called if the request cannot be successfully completed.
            \param reason Failure reason
            \param message Additional message */
        virtual void handleFailure(ClientRequest::FailureReason reason, const String_t& message) = 0;

        /** Request finalisation: success.
            This function is called if the request completed successfully. */
        virtual void handleSuccess() = 0;
    };

} } }

#endif
