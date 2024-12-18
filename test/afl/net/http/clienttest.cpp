/**
  *  \file test/afl/net/http/clienttest.cpp
  *  \brief Test for afl::net::http::Client
  */

#include "afl/net/http/client.hpp"

#include <stdexcept>
#include "afl/async/controller.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/base/ptr.hpp"
#include "afl/net/http/clientconnection.hpp"
#include "afl/net/http/clientconnectionprovider.hpp"
#include "afl/net/internalnetworkstack.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/socket.hpp"
#include "afl/sys/semaphore.hpp"
#include "afl/sys/thread.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    class MyConnectionProvider : public afl::net::http::ClientConnectionProvider {
     public:
        MyConnectionProvider(afl::net::http::Client& client,
                             afl::net::Name& name,
                             afl::net::NetworkStack& stack)
            : m_client(client),
              m_name(name),
              m_stack(stack)
            { }
        virtual void requestNewConnection()
            {
                afl::net::Name name;
                String_t schema;
                while (m_client.getUnsatisfiedTarget(name, schema)) {
                    m_client.addNewConnection(new afl::net::http::ClientConnection(name, schema, m_stack.connect(m_name)));
                }
            }
     private:
        afl::net::http::Client& m_client;
        afl::net::Name& m_name;
        afl::net::NetworkStack& m_stack;
    };
}

/*
 *  Test operation against a stupid server that closes the connection after every request.
 *  Test using system and internal network stack.
 */
static void testClientSimpleServer(afl::net::NetworkStack& ns)
{
    afl::net::Name name("127.0.0.1", uint16_t(std::rand() % 10000 + 20000));
    afl::base::Ref<afl::net::Listener> listener = ns.listen(name, 10);

    // Create a server that serves N requests
    static const int NUM_REQUESTS = 20;
    class Server : public afl::base::Stoppable {
     public:
        Server(afl::net::Listener& listener)
            : m_listener(listener)
            { }
        void run()
            {
                for (int i = 0; i < NUM_REQUESTS; ++i) {
                    // Accept a connection
                    afl::base::Ptr<afl::net::Socket> s = m_listener.accept();

                    // Read a request. Request ends at \r\n\r\n
                    afl::async::Controller ctl;
                    int numNewlines = 0;
                    while (numNewlines < 2) {
                        uint8_t buf[1];
                        afl::async::ReceiveOperation rx(buf);
                        s->receive(ctl, rx);
                        if (rx.getNumReceivedBytes() == 0) {
                            // Syntax error on input
                            break;
                        }
                        if (buf[0] == '\r') {
                            // ignore
                        } else if (buf[0] == '\n') {
                            // count
                            ++numNewlines;
                        } else {
                            // reset
                            numNewlines = 0;
                        }
                    }

                    // Send a response
                    afl::async::SendOperation tx(afl::string::toBytes("HTTP/1.0 200 OK\r\n"
                                                                      "Content-Type: text/plain\r\n"
                                                                      "\r\n"
                                                                      "Hi there.\n"));
                    while (tx.getUnsentBytes().size() != 0) {
                        tx.setData(tx.getUnsentBytes());
                        s->send(ctl, tx);
                    }
                }
            }
        void stop()
            { }
     private:
        afl::net::Listener& m_listener;
    };
    Server server(*listener);
    afl::sys::Thread serverThread("serverThread", server);
    serverThread.start();

    // Set up a HTTP client
    afl::net::http::Client client;
    client.setNewConnectionProvider(new MyConnectionProvider(client, name, ns));
    afl::sys::Thread clientThread("clientThread", client);
    clientThread.start();

    // Request some files
    class MyRequest : public afl::net::http::ClientRequest {
     public:
        MyRequest(afl::net::Name name, afl::sys::Semaphore& sem)
            : m_name(name),
              m_sentRequest(false),
              m_semaphore(sem)
            { }
        virtual afl::net::Name getName() const
            { return m_name; }
        virtual String_t getScheme() const
            { return "http"; }
        virtual bool isHeadRequest() const
            { return false; }
        virtual afl::base::ConstBytes_t getRequestData()
            {
                if (!m_sentRequest) {
                    m_sentRequest = true;
                    return afl::string::toBytes("GET /foo HTTP/1.1\r\n\r\n");
                } else {
                    return afl::base::ConstBytes_t();
                }
            }
        virtual bool restart()
            {
                m_sentRequest = false;
                return true;
            }
        virtual void handleResponseHeader(afl::net::http::ClientResponse& /*resp*/)
            { }
        virtual void handleResponseData(afl::base::ConstBytes_t /*data*/)
            { }
        virtual void handleFailure(FailureReason /*reason*/, const String_t& /*message*/)
            { throw std::runtime_error("handleFailure unexpected"); }
        virtual void handleSuccess()
            { m_semaphore.post(); }
     private:
        afl::net::Name m_name;
        bool m_sentRequest;
        afl::sys::Semaphore& m_semaphore;
    };
    afl::sys::Semaphore sem(0);

    // "Parallel" requests
    for (int i = 0; i < NUM_REQUESTS/2; ++i) {
        client.addNewRequest(new MyRequest(name, sem));
    }
    for (int i = 0; i < NUM_REQUESTS/2; ++i) {
        sem.wait();
    }

    // Serial requests
    for (int i = NUM_REQUESTS/2; i < NUM_REQUESTS; ++i) {
        client.addNewRequest(new MyRequest(name, sem));
        sem.wait();
    }

    client.stop();
    clientThread.join();
    serverThread.join();
}

AFL_TEST_NOARG("afl.net.http.Client:simple-server:system")
{
    testClientSimpleServer(afl::net::NetworkStack::getInstance());
}

AFL_TEST_NOARG("afl.net.http.Client:simple-server:internal")
{
    testClientSimpleServer(*afl::net::InternalNetworkStack::create());
}

/*
 *  Test shutdown.
 *  Test using system and internal network stack.
 */
static void testClientShutdown(afl::test::Assert a, afl::net::NetworkStack& ns)
{
    // Set up network
    afl::net::Name name("127.0.0.1", uint16_t(std::rand() % 10000 + 20000));
    afl::base::Ref<afl::net::Listener> listener = ns.listen(name, 10);

    // Create a server that serves N requests
    class Server : public afl::base::Stoppable {
     public:
        Server(afl::net::Listener& listener,
               afl::sys::Semaphore& step)
            : m_listener(listener),
              m_step(step)
            { }
        void run()
            {
                // Accept a connection
                afl::base::Ptr<afl::net::Socket> s = m_listener.accept();

                // Wait, then close the connection
                m_step.wait();
            }
        void stop()
            { }
     private:
        afl::net::Listener& m_listener;
        afl::sys::Semaphore& m_step;
    };
    afl::sys::Semaphore step(0);
    Server server(*listener, step);
    afl::sys::Thread serverThread("serverThread", server);
    serverThread.start();

    // Set up a HTTP client
    afl::net::http::Client client;
    client.setNewConnectionProvider(new MyConnectionProvider(client, name, ns));
    afl::sys::Thread clientThread("clientThread", client);
    clientThread.start();

    // Request some files
    class MyRequest : public afl::net::http::ClientRequest {
     public:
        MyRequest(afl::test::Assert a, afl::net::Name name, afl::sys::Semaphore& sendSem, afl::sys::Semaphore& finishSem)
            : m_assert(a),
              m_name(name),
              m_sentRequest(false),
              m_sendSemaphore(sendSem),
              m_finishSemaphore(finishSem)
            { }
        virtual afl::net::Name getName() const
            { return m_name; }
        virtual String_t getScheme() const
            { return "http"; }
        virtual bool isHeadRequest() const
            { return false; }
        virtual afl::base::ConstBytes_t getRequestData()
            {
                if (!m_sentRequest) {
                    m_sentRequest = true;
                    m_sendSemaphore.post();
                    return afl::string::toBytes("GET /foo HTTP/1.1\r\n\r\n");
                } else {
                    return afl::base::ConstBytes_t();
                }
            }
        virtual bool restart()
            {
                m_sentRequest = false;
                return true;
            }
        virtual void handleResponseHeader(afl::net::http::ClientResponse& /*resp*/)
            { }
        virtual void handleResponseData(afl::base::ConstBytes_t /*data*/)
            { }
        virtual void handleFailure(FailureReason reason, const String_t& /*message*/)
            {
                m_assert.check("reason must be Cancelled", reason == Cancelled);
                m_finishSemaphore.post();
            }
        virtual void handleSuccess()
            { m_assert.fail("handleSuccess not expected"); }
     private:
        afl::test::Assert m_assert;
        afl::net::Name m_name;
        bool m_sentRequest;
        afl::sys::Semaphore& m_sendSemaphore;
        afl::sys::Semaphore& m_finishSemaphore;
    };
    afl::sys::Semaphore sendSem(0);
    afl::sys::Semaphore finishSem(0);

    // Post two requests
    client.addNewRequest(new MyRequest(a("first request"), name, sendSem, finishSem));
    client.addNewRequest(new MyRequest(a("second request"), name, sendSem, finishSem));

    // Wait until first request starts
    sendSem.wait();

    // Stop the client. At this time, the requests will cancel.
    client.stop();

    // Perform one step; this terminates the server
    step.post();

    // Wait for cancellations
    finishSem.wait();
    finishSem.wait();

    // Stop the threads
    clientThread.join();
    serverThread.join();
}

AFL_TEST("afl.net.http.Client:shutdown:system", a)
{
    testClientShutdown(a, afl::net::NetworkStack::getInstance());
}

AFL_TEST("afl.net.http.Client:shutdown:internal", a)
{
    testClientShutdown(a, *afl::net::InternalNetworkStack::create());
}
