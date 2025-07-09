/**
  *  \file afl/net/http/simpledownloadlistener.hpp
  *  \brief Class afl::net::http::SimpleDownloadListener
  */
#ifndef AFL_AFL_NET_HTTP_SIMPLEDOWNLOADLISTENER_HPP
#define AFL_AFL_NET_HTTP_SIMPLEDOWNLOADLISTENER_HPP

#include "afl/base/growablememory.hpp"
#include "afl/sys/semaphore.hpp"
#include "afl/sys/types.hpp"
#include "afl/net/http/downloadlistener.hpp"
#include "afl/net/headertable.hpp"

namespace afl { namespace net { namespace http {

    class Client;
    class Manager;

    /** Simple download listener.
        This implements DownloadListener for a quasi-synchronous file download into a memory buffer.

        Usage:
        - post request on Client (directly or via Manager)
        - call wait()
        After wait(), the result (if successful) can be obtained using getResponseHeaders() or getResponseData(). */
    class SimpleDownloadListener : public DownloadListener {
     public:
        /** Download status. */
        enum Status {
            /** Download succeeded.
                getResponseData() and getResponseHeaders() contain correct information.
                getStatusCode() contains HTTP status code. */
            Succeeded,

            /** Download failed.
                getFailureReason() and getFailureMessage() contain failure information. */
            Failed,

            /** Download did not complete within the given time limit. */
            TimedOut,

            /** Download exceeded the given data size limit. */
            LimitExceeded
        };

        /** Default constructor. */
        SimpleDownloadListener();

        /** Set data limit.
            This sets a cap to the number of bytes to download.
            If the server sends more data, the excess will be discarded.
            The default is no limit (FileSize_t(-1) aka maximum value).
            \param limit Size in bytes */
        void setDataLimit(afl::io::Stream::FileSize_t limit);

        /** Wait for completion.
            Returns after the request has been processed.
            \return Request status */
        Status wait();

        /** Wait with overall timeout.
            Returns after the request has been processed.
            If the request takes longer than the given timeout, cancels it and returns TimedOut.
            You need to specify client/requestId so the request can be cacneled.

            \param timeout Timeout in milliseconds
            \param client Underlying HTTP client
            \param requestId Request Id
            \return Request status */
        Status wait(afl::sys::Timeout_t timeout, Client& client, uint32_t requestId);

        /** Get response headers.
            Valid after wait() returned Succeeded.
            \return response headers */
        const HeaderTable& getResponseHeaders() const;

        /** Get status code.
            Valid after wait() returned Succeeded.
            \return status code */
        int getStatusCode() const;

        /** Get response data.
            Valid after wait() returned Succeeded.
            \return data */
        afl::base::ConstBytes_t getResponseData() const;

        /** Get failure reason.
            Valid after wait() returned Failed.
            \return failure reason */
        ClientRequest::FailureReason getFailureReason() const;

        /** Get failure message.
            Valid after wait() returned Failed.
            \return failure message */
        const String_t& getFailureMessage() const;


        // DownloadListener:
        virtual void handleResponseHeader(ClientResponse& resp);
        virtual void handleResponseData(afl::io::Stream::FileSize_t ofs, afl::base::ConstBytes_t data);
        virtual void handleFailure(ClientRequest::FailureReason reason, const String_t& message);
        virtual void handleSuccess();

     private:
        afl::sys::Semaphore m_semaphore;
        afl::io::Stream::FileSize_t m_sizeLimit;
        afl::base::GrowableBytes_t m_data;
        HeaderTable m_headers;
        int m_statusCode;
        Status m_result;
        ClientRequest::FailureReason m_failureReason;
        String_t m_failureMessage;
    };

    /** Convert SimpleDownloadListener::Status to string.
        The response is intended for technical users/logfiles and thus not translated. */
    String_t toString(SimpleDownloadListener::Status st);

} } }

#endif
