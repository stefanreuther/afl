/**
  *  \file test/afl/net/resp/clienttest.cpp
  *  \brief Test for afl::net::resp::Client
  */

#include "afl/net/resp/client.hpp"

#include <cstdlib>
#include "afl/async/communicationstream.hpp"
#include "afl/async/controller.hpp"
#include "afl/base/ptr.hpp"
#include "afl/base/ref.hpp"
#include "afl/base/stoppable.hpp"
#include "afl/data/defaultvaluefactory.hpp"
#include "afl/data/errorvalue.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/except/remoteerrorexception.hpp"
#include "afl/except/systemexception.hpp"
#include "afl/io/bufferedsink.hpp"
#include "afl/io/resp/parser.hpp"
#include "afl/io/resp/writer.hpp"
#include "afl/net/listener.hpp"
#include "afl/net/name.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/socket.hpp"
#include "afl/sys/semaphore.hpp"
#include "afl/sys/thread.hpp"
#include "afl/test/testrunner.hpp"
#include "afl/except/assertionfailedexception.hpp"

namespace {
    using afl::base::Ref;
    class ReconnectTester : public afl::base::Stoppable {
     public:
        ReconnectTester(afl::test::Assert a, Ref<afl::net::Listener> listener, bool readByte)
            : m_assert(a),
              m_listener(listener),
              m_running(true),
              m_readByte(readByte)
            { }

        void run()
            {
                afl::async::Controller ctl;
                int n = 0;
                bool running = true;
                while (running) {
                    // Accept a connection and wrap it in a communication stream
                    afl::async::CommunicationStream io(ctl, m_listener->accept());

                    // Read and parse data
                    afl::data::DefaultValueFactory factory;
                    afl::io::resp::Parser parser(factory);
                    while (1) {
                        uint8_t buffer[1024];
                        afl::base::Bytes_t bytes(buffer);
                        bytes.trim(io.read(bytes));

                        afl::base::ConstBytes_t constBytes(bytes);
                        if (parser.handleData(constBytes)) {
                            m_assert.check("handleData completed", constBytes.empty());
                            break;
                        }
                    }

                    // At this point, the other side is guaranteed to not modify m_running. Copy it.
                    running = m_running;

                    // Verify received data
                    std::auto_ptr<afl::data::Value> value(parser.extract());
                    m_assert.checkNonNull("value received", value.get());

                    // Send back as-is
                    afl::data::IntegerValue iv(++n);
                    afl::io::BufferedSink sink(io);
                    afl::io::resp::Writer(sink).visit(&iv);
                    sink.flush();

                    // Read one more byte if configured.
                    // This causes the other side to get the error after writing the next request
                    if (m_readByte) {
                        uint8_t byte[1];
                        io.read(byte);
                    }

                    // Wait an instant to increase chance that other side doesn't immediately notice our connection closing
                    afl::sys::Thread::sleep(50);
                }
            }

        void stop()
            { m_running = false; }

     private:
        afl::test::Assert m_assert;
        Ref<afl::net::Listener> m_listener;
        bool m_running;
        bool m_readByte;
    };
}

/** Simple test. */
AFL_TEST("afl.net.resp.Client:simple", a)
{
    class Tester : public afl::base::Stoppable {
     public:
        Tester(afl::test::Assert a, Ref<afl::net::Listener> listener)
            : m_assert(a),
              m_listener(listener),
              m_in(),
              m_out(),
              m_wake(0)
            { }

        void run()
            {
                // Accept one connection and wrap it in a communication stream
                afl::async::Controller ctl;
                afl::async::CommunicationStream io(ctl, m_listener->accept());

                // Read expected data
                while (1) {
                    // Wait for caller to provide data
                    m_wake.wait();
                    if (m_in.empty()) {
                        break;
                    }

                    // Wait for data
                    uint8_t buffer[1024];
                    m_assert.check("result fits in buffer", m_in.size() <= sizeof(buffer));
                    AFL_CHECK_SUCCEEDS(m_assert("fullRead"), io.fullRead(afl::base::Bytes_t(buffer).trim(m_in.size())));
                    m_assert.checkEqualContent("result content", afl::string::toBytes(m_in.data()), afl::base::ConstBytes_t(buffer).trim(m_in.size()));

                    // Send agreed response
                    AFL_CHECK_SUCCEEDS(m_assert("fullWrite"), io.fullWrite(afl::string::toBytes(m_out)));
                }
            }

        void stop()
            { }

        void setData(String_t in, String_t out)
            {
                m_in = in;
                m_out = out;
                m_wake.post();
            }

     private:
        afl::test::Assert m_assert;
        Ref<afl::net::Listener> m_listener;
        String_t m_in;
        String_t m_out;
        afl::sys::Semaphore m_wake;
    };

    // Set up network
    uint16_t portNr = static_cast<uint16_t>(20000 + std::rand() % 10000);
    afl::net::Name name("127.0.0.1", portNr);
    afl::net::NetworkStack& stack(afl::net::NetworkStack::getInstance());

    // Make a tester
    Tester testRunnable(a("Tester"), stack.listen(name, 5));
    afl::sys::Thread testThread("RespClientTest", testRunnable);
    testThread.start();

    // Make a client
    afl::net::resp::Client client(stack, name);

    // Test that we can do several roundtrips
    std::auto_ptr<afl::data::Value> result;
    for (int i = 0; i < 10; ++i) {
        testRunnable.setData("*2\r\n$5\r\nHello\r\n$5\r\nWorld\r\n", ":17\r\n");
        result.reset(client.call(afl::data::Segment().pushBackString("Hello").pushBackString("World")));
        a.checkNonNull("result", result.get());
        a.checkNonNull("result type", dynamic_cast<afl::data::IntegerValue*>(result.get()));
        a.checkEqual("result value", dynamic_cast<afl::data::IntegerValue*>(result.get())->getValue(), 17);
    }

    // Test that integers are transmitted as strings, null result returned
    testRunnable.setData("*2\r\n$3\r\nADD\r\n$2\r\n42\r\n", "$-1\r\n");
    result.reset(client.call(afl::data::Segment().pushBackString("ADD").pushBackInteger(42)));
    a.checkNull("call result", result.get());

    // Test error
    testRunnable.setData("*0\r\n", "-ERR\r\n");
    afl::data::Segment empty;
    AFL_CHECK_THROWS(a("call returns error"), client.call(empty), afl::except::RemoteErrorException);

    // Trigger shutdown
    testRunnable.setData("", "");
}

/** Test reconnect behaviour with a server that closes between requests. */
AFL_TEST("afl.net.resp.Client:reconnect", a)
{
    // Set up network
    uint16_t portNr = static_cast<uint16_t>(20000 + std::rand() % 10000);
    afl::net::Name name("127.0.0.1", portNr);
    afl::net::NetworkStack& stack(afl::net::NetworkStack::getInstance());

    // Make a tester
    ReconnectTester testRunnable(a("ReconnectTester"), stack.listen(name, 5), false);
    afl::sys::Thread testThread("RespClientTest", testRunnable);
    testThread.start();

    // Make a client
    afl::net::resp::Client client(stack, name);

    // Send a few commands
    std::auto_ptr<afl::data::Value> result;
    AFL_CHECK_SUCCEEDS(a("command 1"), result.reset(client.call(afl::data::Segment().pushBackString("hi"))));
    AFL_CHECK_SUCCEEDS(a("command 2"), result.reset(client.call(afl::data::Segment().pushBackString("ho"))));

    // Stop it
    testRunnable.stop();
    AFL_CHECK_SUCCEEDS(a("stop"), result.reset(client.call(afl::data::Segment().pushBackString("stop"))));
}

/** Test reconnect behaviour with a server that closes within a request. */
AFL_TEST("afl.net.resp.Client:reconnect2", a)
{
    // Set up network
    uint16_t portNr = static_cast<uint16_t>(20000 + std::rand() % 10000);
    afl::net::Name name("127.0.0.1", portNr);
    afl::net::NetworkStack& stack(afl::net::NetworkStack::getInstance());

    // Make a tester
    ReconnectTester testRunnable(a("ReconnectTester"), stack.listen(name, 5), true);
    afl::sys::Thread testThread("RespClientTest", testRunnable);
    testThread.start();

    // Make a client
    afl::net::resp::Client client(stack, name);

    // Send a few commands
    std::auto_ptr<afl::data::Value> result;
    AFL_CHECK_SUCCEEDS(a("command 1"), result.reset(client.call(afl::data::Segment().pushBackString("hi"))));
    AFL_CHECK_SUCCEEDS(a("command 2"), result.reset(client.call(afl::data::Segment().pushBackString("ho"))));

    // Stop it
    testRunnable.stop();
    AFL_CHECK_SUCCEEDS(a("stop"), result.reset(client.call(afl::data::Segment().pushBackString("stop"))));
}

/** Test that we properly handle that there is no other side. */
AFL_TEST("afl.net.resp.Client:not-connected", a)
{
    // Set up network
    uint16_t portNr = static_cast<uint16_t>(20000 + std::rand() % 10000);
    afl::net::Name name("127.0.0.1", portNr);
    afl::net::NetworkStack& stack(afl::net::NetworkStack::getInstance());

    // Make a client
    try {
        afl::net::resp::Client client(stack, name);
        a.fail("expected exception");
    }
    catch (afl::except::AssertionFailedException& ex) {
        throw;
    }
    catch (afl::except::FileProblemException&) {
        // ok
    }
    catch (afl::except::SystemException&) {
        // ok
    }
    catch (...) {
        a.fail("expected proper exception");
    }
}

/** Test that we properly handle remote-side errors (i.e\. no reconnect). */
AFL_TEST("afl.net.resp.Client:error", a)
{
    class ErrorTester : public afl::base::Stoppable {
     public:
        ErrorTester(afl::test::Assert a, Ref<afl::net::Listener> listener)
            : m_assert(a),
              m_listener(listener.asPtr())
            { }

        void run()
            {
                // Accept a connection and wrap it in a communication stream
                afl::async::Controller ctl;
                afl::async::CommunicationStream io(ctl, m_listener->accept());

                // Read and parse data
                afl::data::DefaultValueFactory factory;
                afl::io::resp::Parser parser(factory);
                while (1) {
                    uint8_t buffer[1024];
                    afl::base::Bytes_t bytes(buffer);
                    bytes.trim(io.read(bytes));

                    afl::base::ConstBytes_t constBytes(bytes);
                    if (parser.handleData(constBytes)) {
                        m_assert.check("handleData complete", constBytes.empty());
                        break;
                    }
                }

                // Verify received data
                std::auto_ptr<afl::data::Value> value(parser.extract());
                m_assert.checkNonNull("value received", value.get());

                // Send back an error
                afl::data::ErrorValue ev("source", "message");
                afl::io::BufferedSink sink(io);
                afl::io::resp::Writer(sink).visit(&ev);
                sink.flush();

                // Stop the listener so the operating system correctly fails subsequent connect()s.
                m_listener.reset();
            }

        void stop()
            { m_running = false; }

     private:
        afl::test::Assert m_assert;
        afl::base::Ptr<afl::net::Listener> m_listener;
        bool m_running;
    };

    // Set up network
    uint16_t portNr = static_cast<uint16_t>(20000 + std::rand() % 10000);
    afl::net::Name name("127.0.0.1", portNr);
    afl::net::NetworkStack& stack(afl::net::NetworkStack::getInstance());

    // Make a tester
    ErrorTester testRunnable(a("ErrorTester"), stack.listen(name, 5));
    afl::sys::Thread testThread("RespClientTest", testRunnable);
    testThread.start();

    // Send one single request. Must produce the exact ErrorValue we sent.
    // This fails/hangs if the resp::Client erroneously treats RemoteErrorException as a reconnect trigger.
    afl::net::resp::Client client(stack, name);
    try {
        client.callVoid(afl::data::Segment().pushBackString("hi"));
        a.fail("expected exception");
    }
    catch (afl::except::AssertionFailedException& ex) {
        throw;
    }
    catch (afl::except::RemoteErrorException& ex) {
        a.checkEqual("message content", String_t(ex.what()), "message");
    }
    catch (...) {
        a.fail("wrong exception");
    }

    // Try another call. This must produce a network exception as our server is gone.
    try {
        client.callVoid(afl::data::Segment().pushBackString("hi"));
        a.fail("expected exception 2");
    }
    catch (afl::except::AssertionFailedException& ex) {
        throw;
    }
    catch (afl::except::FileProblemException&) {
        // ok
    }
    catch (afl::except::SystemException&) {
        // ok
    }
    catch (...) {
        a.fail("expected proper exception 2");
    }
}

/** Test reconnect behaviour with Reconnect Once configuration. */
AFL_TEST("afl.net.resp.Client:setReconnectMode:once", a)
{
    // Set up network
    uint16_t portNr = static_cast<uint16_t>(20000 + std::rand() % 10000);
    afl::net::Name name("127.0.0.1", portNr);
    afl::net::NetworkStack& stack(afl::net::NetworkStack::getInstance());

    // Make a tester
    ReconnectTester testRunnable(a("ReconnectTester"), stack.listen(name, 5), true);
    afl::sys::Thread testThread("RespClientTest", testRunnable);
    testThread.start();

    // Make a client
    afl::net::resp::Client client(stack, name);
    client.setReconnectMode(afl::net::Reconnectable::Once);

    // Send a few commands
    std::auto_ptr<afl::data::Value> result;
    AFL_CHECK_SUCCEEDS(a("command 1"), result.reset(client.call(afl::data::Segment().pushBackString("hi"))));
    AFL_CHECK_THROWS(a("command 2 fails"), result.reset(client.call(afl::data::Segment().pushBackString("ho"))), std::exception);

    // Stop it
    testRunnable.stop();
    client.setReconnectMode(afl::net::Reconnectable::Always);
    AFL_CHECK_SUCCEEDS(a("stop"), result.reset(client.call(afl::data::Segment().pushBackString("stop"))));
}

