/**
  *  \file afl/net/http/clientconnectionprovider.hpp
  *  \brief Interface afl::net::http::ClientConnectionProvider
  */
#ifndef AFL_AFL_NET_HTTP_CLIENTCONNECTIONPROVIDER_HPP
#define AFL_AFL_NET_HTTP_CLIENTCONNECTIONPROVIDER_HPP

#include "afl/base/deletable.hpp"

namespace afl { namespace net { namespace http {

    class Client;

    /** HTTP Client Connection Provider.
        This is a callout for the HTTP client to build connections.
        A ClientConnectionProvider instance is associated with every http::Client instance,
        see http::Client::setNewConnectionProvider(). */
    class ClientConnectionProvider : public afl::base::Deletable {
     public:
        /** Request new connections.
            This function should arrange for
            - http::Client::getUnsatisfiedTarget() to be called to obtain a connection request
            - a connection to be made
            - on success, http::Client::addNewConnection() to be called with the connection
            - on error, http::Client::cancelRequestsByTarget() to be called

            The connection (=DNS lookup, TCP handshake, SSL handshake, whatever)
            should be prepared in a separate thread, not directly in this method,
            to not block the HTTP client.

            There is not necessarily a 1:1 mapping between connection requests and calls to this function.
            This function might be called when multiple connections are needed,
            or the connections might no longer be needed because they have been cancelled in the meantime.
            This function should therefore loop until getUnsatisfiedTarget() no longer returns a request. */
        virtual void requestNewConnection() = 0;
    };

} } }

#endif
