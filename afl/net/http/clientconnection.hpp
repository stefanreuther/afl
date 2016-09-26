/**
  *  \file afl/net/http/clientconnection.hpp
  *  \brief Class afl::net::http::ClientConnection
  */
#ifndef AFL_AFL_NET_HTTP_CLIENTCONNECTION_HPP
#define AFL_AFL_NET_HTTP_CLIENTCONNECTION_HPP

#include <memory>
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/base/ptr.hpp"
#include "afl/net/name.hpp"
#include "afl/net/socket.hpp"
#include "afl/io/datasink.hpp"
#include "afl/base/uncopyable.hpp"
#include "afl/base/deleter.hpp"

namespace afl { namespace net { namespace http {

    class ClientRequest;
    class ClientResponse;

    /** Client connection.
        A ClientConnection handles one instance of a TCP connection
        that serves any number of HTTP requests.

        Objects of this type are created by ClientConnectionProvider upon request of the http::Client,
        and given to it using http::Client::setNewConnection(). */
    class ClientConnection : private afl::base::Uncopyable {
     public:
        enum Action {
            /** I have nothing to do.
                Call setNewRequest with a request when you have one for me. */
            WaitForRequest,

            /** I am transferring data.
                Call handleTransfer when you got a transfer. */
            Transferring,

            /** I want to shut down.
                Call extractRequest to salvage a possibly-pending request,
                then kill this object. */
            Shutdown
        };

        /** Constructor.
            \param name Network name of other end
            \param scheme URL scheme (protocol)
            \param socket Connected socket */
        ClientConnection(const Name& name, const String_t& scheme, afl::base::Ptr<Socket> socket);

        /** Destructor. */
        ~ClientConnection();

        /** Main loop callback.
            This function checks whether the given operation affects us,
            sets up new asynchronous operations,
            and returns an action for the main loop.
            \param ctl Async controller
            \param p Operation from Controller::wait
            \param elapsed Elapsed time in milliseconds
            \return action */
        Action handleEvent(afl::async::Controller& ctl, afl::async::Operation* p, uint32_t elapsed);

        /** Set request.
            Called by the main loop when we were waiting (WaitForRequest).
            The ClientConnection takes ownership of the request.
            \param req Request (must not be null) */
        void setNewRequest(ClientRequest* req);

        /** Extract request.
            Called by the main loop when we requested shutdown to salvage the request.
            \return request (null if none) */
        ClientRequest* extractRequest();

        /** Check for matching request.
            \param req Request
            \retval true This connection can handle this request
            \retval false This connection cannot handle this request */
        bool matchRequest(const ClientRequest& req) const;

        /** Cancel request by Id.
            If this ClientConnection is processing the request
            identified by the given Id, cancels that.
            \param ctl Async controller
            \param id request Id */
        void cancelRequest(afl::async::Controller& ctl, uint32_t id);

        /** Cancel whatever this connection is working on.
            Note that if this connection is not working on a request, this will not have any effect.
            \param ctl Async controller */
        void cancel(afl::async::Controller& ctl);

     private:
        // Connection data
        const Name m_name;
        const String_t m_scheme;
        const afl::base::Ptr<Socket> m_socket;

        // Current request
        std::auto_ptr<ClientRequest> m_request;
        std::auto_ptr<ClientResponse> m_response;
        afl::base::Deleter m_responseDeleter;
        afl::io::DataSink* m_pResponseSink;

        // Status
        enum State {
            WantWait,              // We do not have a request; wait for one
            BeforeSend,            // We may want to send (but haven't tried yet)
            DuringSend,            // We are sending
            BeforeReceiveHeader,   // We may want to receive headers (but haven't started yet)
            DuringReceiveHeader,   // We are receiving
            BeforeReceivePayload,  // We may want to receive payload (but haven't started yet)
            DuringReceivePayload,  // We are receiving payload
            WantClose              // We want to close
        };
        State m_state;
        uint32_t m_idleTime;

        // I/O data
        afl::async::SendOperation m_sendOperation;
        afl::async::ReceiveOperation m_receiveOperation;
        uint8_t m_receiveBuffer[4096];

        void handleReception(afl::base::ConstBytes_t data);
        void reset();
    };

} } }

#endif
