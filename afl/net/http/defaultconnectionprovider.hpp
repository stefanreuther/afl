/**
  *  \file afl/net/http/defaultconnectionprovider.hpp
  *  \brief Class afl::net::http::DefaultConnectionProvider
  */
#ifndef AFL_AFL_NET_HTTP_DEFAULTCONNECTIONPROVIDER_HPP
#define AFL_AFL_NET_HTTP_DEFAULTCONNECTIONPROVIDER_HPP

#include "afl/base/runnable.hpp"
#include "afl/base/uncopyable.hpp"
#include "afl/net/http/clientconnectionprovider.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/sys/mutex.hpp"
#include "afl/sys/semaphore.hpp"
#include "afl/sys/thread.hpp"

namespace afl { namespace net { namespace http {

    class Client;

    /** HTTP client default connection provider.
        This is a simple, default implementation of ClientConnectionProvider that suits most needs.
        It supports creating connections for a single URL scheme ("http").
        It works in a background thread which is started upon construction, and deleted upon destruction. */
    class DefaultConnectionProvider : public ClientConnectionProvider,
                                      private afl::base::Runnable,
                                      private afl::base::Uncopyable
    {
     public:
        /** Constructor.
            \param client Associated Client instance (you must call its setNewConnectionProvider!)
            \param stack [optional] Network stack to use. Default is the system stack.
            \param scheme [optional] Default URL scheme. Default is "http". Other URL schemes will be rejected */
        DefaultConnectionProvider(Client& client,
                                  NetworkStack& stack = NetworkStack::getInstance(),
                                  String_t scheme = "http");

        /** Destructor. */
        ~DefaultConnectionProvider();

        /** Request new connection. Called by Client. */
        void requestNewConnection();

     private:
        // Thread:
        void run();

        // Integration:
        Client& m_client;
        NetworkStack& m_networkStack;
        const String_t m_scheme;

        // Work:
        afl::sys::Thread m_thread;
        afl::sys::Semaphore m_wake;
        afl::sys::Mutex m_mutex;
        bool m_stop;
    };

} } }

#endif
