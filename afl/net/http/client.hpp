/**
  *  \file afl/net/http/client.hpp
  *  \brief Class afl::net::http::Client
  */
#ifndef AFL_AFL_NET_HTTP_CLIENT_HPP
#define AFL_AFL_NET_HTTP_CLIENT_HPP

#include <memory>
#include "afl/async/operation.hpp"
#include "afl/async/semaphore.hpp"
#include "afl/base/stoppable.hpp"
#include "afl/base/uncopyable.hpp"
#include "afl/container/ptrvector.hpp"
#include "afl/net/http/clientrequest.hpp"
#include "afl/net/name.hpp"
#include "afl/sys/mutex.hpp"

namespace afl { namespace net { namespace http {

    class ClientConnection;
    class ClientConnectionProvider;

    /** HTTP Client.
        This is an asynchronous, multi-connection HTTP client.

        The http::Client runs in a thread.
        It owns a ClientConnectionProvider (see setNewConnectionProvider()) which usually has another thread.
        Requests are added using addNewRequest(); the HTTP client takes ownership of them.

        Requests are processed in per-server FIFO order.
        They receive callbacks as the request progresses, and are guaranteed to receive a
        ClientRequest::handleFailure() or ClientRequest::handleSuccess() call.
        Because the server owns the ClientRequest objects,
        they must synchronize with their caller using some external means of communication. */
    class Client : public afl::base::Stoppable, private afl::base::Uncopyable {
     public:
        /** Constructor.
            The object will not be usable until you set the connection provider,
            see setNewConnectionProvider(). */
        Client();

        /** Destructor. */
        ~Client();

        /** Add a new connection.
            The Client takes ownership of the connection object.
            This function is intended to be called by ClientConnectionProvider.

            Can be called from any thread.

            \param conn Connection, must not be null */
        void addNewConnection(ClientConnection* conn);

        /** Add a new request.
            The Client takes ownership of the request object.
            You must not access the object after this call.

            Can be called from any thread.

            \param req Request, must not be null
            \return Id assigned to this request for cancelRequest(). */
        uint32_t addNewRequest(ClientRequest* req);

        /** Cancel a request.
            The associated request object, if any, is cancelled.
            Cancellation is not immediate; i.e. it is not guaranteed
            that the request is gone when this function returns.
            You must synchronize on the request's handleFailure()/handleSuccess().
            It is even possible that the request completed successfully
            (or failed for another reason) in the meantime.

            Can be called from any thread.

            \param id Request Id */
        void cancelRequest(uint32_t id);

        /** Set connection provider.
            The Client takes ownership of the connection provider object.
            This function should be called directly after construction, before starting the thread.
            \param provider Connection provider, must not be null */
        void setNewConnectionProvider(ClientConnectionProvider* provider);

        /** Thread entry point. */
        virtual void run();

        /** Request stop.
            Causes the main loop to exit, so you can join() the thread.
            Can be called from any thread. */
        virtual void stop();

        /** Cancel requests for a given target.
            This function cancels all outstanding requests.
            It does not cancel requests that have already started executing.

            This function is intended to be called by ClientConnectionProvider.
            When it has determined that the connection cannot be made,
            it can release all pending requests for it.

            Can be called from any thread.

            \param name Network name
            \param scheme Scheme (=protocol)
            \param reason Failure reason to report
            \param message Message */
        void cancelRequestsByTarget(const Name& name,
                                    const String_t& scheme,
                                    ClientRequest::FailureReason reason,
                                    const String_t& message);

        /** Get information about an unsatisfied target.
            This function is intended to be called by ClientConnectionProvider.
            If there is a pending request for a name/scheme that cannot be satisfied by an existing connection,
            return that request's name/scheme.

            Can be called from any thread.

            Note that this function will return the same return value until a connection for this name/scheme
            is provided or the request is cancelled.

            \param name [out] Network name
            \param scheme [out] Scheme (=protocol)
            \retval true There is an outstanding request, name/scheme has been filled in
            \retval false There is no outstanding request */
        bool getUnsatisfiedTarget(Name& name, String_t& scheme);

     private:
        std::auto_ptr<ClientConnectionProvider> m_provider;

        afl::sys::Mutex m_mutex;
        afl::async::Semaphore m_wake;
        afl::container::PtrVector<ClientConnection> m_connections;
        afl::container::PtrVector<ClientRequest> m_requests;
        std::vector<uint32_t> m_cancels;
        bool m_stop;
        bool m_needNewConnection;
        uint32_t m_requestIdCounter;

        void requestNewConnection();

        void processConnections(size_t i, afl::async::Controller& ctl, afl::async::Operation* op, uint32_t elapsed);
        ClientRequest* extractMatchingRequest(ClientConnection& conn);

        void processCancels(afl::async::Controller& ctl);
    };

} } }

#endif
