/**
  *  \file u/t_net_securecontext.cpp
  *  \brief Test for afl::net::SecureContext
  */

#include "afl/net/securecontext.hpp"

#include <cstdlib>
#include <cassert>
#include <queue>
#include "u/t_net.hpp"
#include "afl/async/notifier.hpp"
#include "afl/async/operationlist.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/base/runnable.hpp"
#include "afl/except/unsupportedexception.hpp"
#include "afl/net/name.hpp"
#include "afl/net/securesocket.hpp"
#include "afl/sys/mutex.hpp"
#include "afl/sys/mutexguard.hpp"
#include "afl/sys/thread.hpp"

namespace {
    /*
     *  Imports
     */
    using afl::async::SendOperation;
    using afl::async::ReceiveOperation;
    using afl::async::Operation;
    using afl::async::Controller;
    using afl::base::Ptr;
    using afl::net::Socket;


    /*
     *  We need a pretty elaborate mock so we can test encryption independant of networking.
     *  This implements a simple, low-performance stream exchange.
     *  A 'Node' object operates as a common central point.
     *  - 'send' data is immediately pushed into a queue.
     *  - only 'receive' can block.
     *  - because it's bidirectional, all queues are doubled and indexed by 'int side',
     *    where the side identifies the receiving side.
     *  - two 'Socket' objects implement the front-end to the 'Node'.
     */
    typedef std::queue<uint8_t> Queue_t;

    class Node {
     public:
        Node()
            {
                m_active[0] = false;
                m_active[1] = false;
            }

        // Send data, synchronously.
        void send(int side, SendOperation& op)
            {
                assert(side == 0 || side == 1);
                afl::sys::MutexGuard g(m_mutex);
                afl::base::ConstBytes_t bytes = op.getUnsentBytes();
                while (const uint8_t* p = bytes.eat()) {
                    m_queues[side].push(*p);
                    op.addSentBytes(1);
                }
                doTransfers(side);
            }

        // Receive data, asynchronously.
        void receiveAsync(int side, Controller& ctl, ReceiveOperation& op)
            {
                assert(side == 0 || side == 1);

                // Index is sending side
                side ^= 1;

                // Process operation
                afl::sys::MutexGuard g(m_mutex);
                op.setController(&ctl);
                m_receives[side].pushBack(&op);
                doTransfers(side);
            }

        // Cancel an operation (should not be needed for test).
        void cancel(int side, Controller& ctl, Operation& op)
            {
                assert(side == 0 || side == 1);
                side ^= 1;
                {
                    afl::sys::MutexGuard g(m_mutex);
                    m_receives[side].remove(&op);
                }
                ctl.revertPost(op);
            }

     private:
        afl::sys::Mutex m_mutex;

        // Data queues. Index is sending side.
        Queue_t m_queues[2];

        // Receiver queues. Index is sending side.
        afl::async::OperationList<afl::async::ReceiveOperation> m_receives[2];

        // Active?
        // This flag prevents that when a Notifier callback posts new requests,
        // doTransfers is called recursively.
        // This limit is actually optional but limits possible stack depths.
        bool m_active[2];

        // If some new transfers can be done (data in queue and receiver available), do them.
        void doTransfers(int side)
            {
                if (!m_active[side]) {
                    m_active[side] = true;
                    while (afl::async::ReceiveOperation* p = m_receives[side].front()) {
                        // Transfer data.
                        while (!m_queues[side].empty() && !p->isCompleted()) {
                            uint8_t* pByte = p->getUnreceivedBytes().eat();
                            assert(pByte != 0);
                            *pByte = m_queues[side].front();
                            m_queues[side].pop();
                            p->addReceivedBytes(1);
                        }

                        // Finished?
                        if (p->getNumReceivedBytes() > 0) {
                            // Yes.
                            m_receives[side].extractFront();
                            p->getNotifier().notify(*p);
                        } else {
                            // We didn't make any progress. Stop the loop.
                            break;
                        }
                    }
                    m_active[side] = false;
                }
            }
    };

    class SocketMock : public Socket {
     public:
        SocketMock(Ptr<Node> pNode, int side)
            : m_pNode(pNode),
              m_side(side)
            { }

        // Socket:
        virtual void closeSend()
            { /* we don't need that yet */ }

        virtual afl::net::Name getPeerName()
            { return afl::net::Name("mock", uint16_t(m_side^1)); }

        // CommunicationObject:
        virtual bool send(Controller& /*ctl*/, SendOperation& op, afl::sys::Timeout_t /*timeout*/)
            {
                m_pNode->send(m_side, op);
                return true;
            }

        virtual void sendAsync(Controller& ctl, SendOperation& op)
            {
                send(ctl, op, 0);
                op.setController(&ctl);
                op.getNotifier().notify(op);
            }

        virtual bool receive(Controller& ctl, ReceiveOperation& op, afl::sys::Timeout_t timeout)
            {
                receiveAsync(ctl, op);
                if (!ctl.wait(op, timeout)) {
                    cancel(ctl, op);
                    return false;
                } else {
                    return true;
                }
            }

        virtual void receiveAsync(Controller& ctl, ReceiveOperation& op)
            { m_pNode->receiveAsync(m_side, ctl, op); }

        virtual void cancel(Controller& ctl, Operation& op)
            { m_pNode->cancel(m_side, ctl, op); }

        virtual String_t getName()
            { return "mock"; }

     private:
        const Ptr<Node> m_pNode;
        const int m_side;
    };

    const char PRIVATE_KEY[] =
        "-----BEGIN PRIVATE KEY-----\n"
        "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCg+GuijItRL4uK\n"
        "F6C0bkuYhxHrvWzu0TpcT92eVp+gzEbqs/gXGxh5sJSro4qVioDBlBuef4fdvi1T\n"
        "v3ls4bPxx/OAdQ+FaomkPy60hCjaEPvOn3Es8m+AxLjZxOBybxzJkSRojiYyzEZd\n"
        "70eMXz5AV41PuXQUeID6wevLekXY7SVuOWd9W7SpbHlIVU0cqe0n8CBr5xHfp/nV\n"
        "N900gTVKd4SenIt6ETm4zihxwMMsAKJXkE3VQSo2/5WyKBuMN+azAhBkIhW+HEnx\n"
        "Qs4uDlHxIJhGESKnszAfrkXrq4xuDCTJbCZXQTLha/joVt7XkaH9J+9WeND8PA9v\n"
        "vgpbVo/FAgMBAAECggEAR40E8xT73FOA+Bn2kh0wtxTa72mc4S7ZfSz2+qnQfL4w\n"
        "i6UBkuu1XmoSO5UPRddx0hoOiM5xSyx8o2xKAg71WHSjlSbzhj9L/VkkXpMcQYop\n"
        "E7glq2/8BkXrUMlFZzhimGpOv7bn3c6VPK3epNO1LQ59vsFVir/hvRSzxheMspZK\n"
        "QebDv3Uff5s85EdQetFGKKuAXxqRt403rTNSp98JnZSYfBFVsg7KjSy8x6zieVoQ\n"
        "9rBaXWg7O0hHFel4Am7oLfo91TLZp4/s4pZqLVD6F2U96vKksMwiruV36PP6tSRv\n"
        "OZmrzSTyFtbQjMv2P0YTKrvRuh7IHNqScDIocjN4gQKBgQDWMD/PdTXRutl2ghIg\n"
        "jB8G4kZhtp6wSnmX0s5Fgr0v8CrziAiVzjl+64lTivXLD9mv6WttjO4P6MnT+PGD\n"
        "6dppAXZ85BUmIo0yh4erCdYgOectsSUCmrgbgS9A3qMnq0VQsenSoo1dyAiAa0q4\n"
        "qokQFQ/MfUQHqbgOfZS/sz5M5QKBgQDAZK04rmg+g02FR2PL/kvzuhRHuTzhIQy/\n"
        "5UNXE0mgsNliIPDDCDC6Y5wLBcOqRgfMyPtPFRPDSIdOG7z6N7dS/oO/cc7izxGG\n"
        "ssX3TRKzlHKa7BfiHlsgwUJ9jjc6LHVljHWQPQx9yxtc094Hp0vqnP478nwHHPlL\n"
        "B6a3LNPpYQKBgQCfXhksiH66BbgmuFM6imfPnG9X5ySd8VwVvlGiYMvZrm6W82HH\n"
        "7UDUkfaFBcErDSkg/aHkSlcb2U7K/NDfIRJUzTTXanHeBRlgPn2QouoC4nKDieoK\n"
        "zQot1OpO7xkgvAwAJdDf2/kpjVveCXb4flMrdzBhK9eBiCOeDIywMnMpjQKBgDh+\n"
        "V48JR3rzu1Ojft5rsYnD3iihcAr7VW8Ttw5ej4Qr6ekymv/p68UPcsaZzra1EZ1E\n"
        "dubl3YO+TFzXMEqZCa/JJI6WsJVREGhpHlBbxV4N/QkYwnqXR1L67LdJh1YBsAc9\n"
        "k1fjVUOq3EIJxGyi51vg9ZZMwbak/17UxFONSjyBAoGAFSQ/HYzZZB3dBMLsiGLs\n"
        "D6JY6bBB7/jczXDHL1U+zZjUyIL9l3DLmjQ1a3eRQJYBx3KRGVdAbXGGjFyuF88M\n"
        "ORfKbBG02E9Jsmh4NgHBBbgVUiDPV0nDKiYUo9Z5poa2jkNSRdCdyN3ucFDTwuXA\n"
        "NgEzfMq7E1nq5cutGomxi+Q=\n"
        "-----END PRIVATE KEY-----\n";

    const char CERTIFICATE[] =
        "-----BEGIN CERTIFICATE-----\n"
        "MIIDmDCCAoACCQCqBKAhoAHZ9DANBgkqhkiG9w0BAQsFADCBjTELMAkGA1UEBhMC\n"
        "WEUxGTAXBgNVBAgMEEV4YW1wbGUgUHJvdmluY2UxFTATBgNVBAcMDEV4YW1wbGUg\n"
        "Q2l0eTEVMBMGA1UECgwMRXhhbXBsZSBMdGQuMRQwEgYDVQQDDAtleGFtcGxlLm9y\n"
        "ZzEfMB0GCSqGSIb3DQEJARYQdXNlckBleGFtcGxlLm9yZzAeFw0xNTA4MDkxMDM5\n"
        "MzdaFw00MjEyMjUxMDM5MzdaMIGNMQswCQYDVQQGEwJYRTEZMBcGA1UECAwQRXhh\n"
        "bXBsZSBQcm92aW5jZTEVMBMGA1UEBwwMRXhhbXBsZSBDaXR5MRUwEwYDVQQKDAxF\n"
        "eGFtcGxlIEx0ZC4xFDASBgNVBAMMC2V4YW1wbGUub3JnMR8wHQYJKoZIhvcNAQkB\n"
        "FhB1c2VyQGV4YW1wbGUub3JnMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKC\n"
        "AQEAoPhrooyLUS+LihegtG5LmIcR671s7tE6XE/dnlafoMxG6rP4FxsYebCUq6OK\n"
        "lYqAwZQbnn+H3b4tU795bOGz8cfzgHUPhWqJpD8utIQo2hD7zp9xLPJvgMS42cTg\n"
        "cm8cyZEkaI4mMsxGXe9HjF8+QFeNT7l0FHiA+sHry3pF2O0lbjlnfVu0qWx5SFVN\n"
        "HKntJ/Aga+cR36f51TfdNIE1SneEnpyLehE5uM4occDDLACiV5BN1UEqNv+Vsigb\n"
        "jDfmswIQZCIVvhxJ8ULOLg5R8SCYRhEip7MwH65F66uMbgwkyWwmV0Ey4Wv46Fbe\n"
        "15Gh/SfvVnjQ/DwPb74KW1aPxQIDAQABMA0GCSqGSIb3DQEBCwUAA4IBAQA+sn4F\n"
        "WipuyqmmPRQPSGHUuzBFmKuhQr0zAQl0PEWh33Uu3NxssErcB1H4+ib8FRZbSCQK\n"
        "dGAAM4Rl4HqQ9skYhhI84EUu9Tcey/cnnPqIHPVHDtJEa9kQI4r26Yklwfv2F1DL\n"
        "GvhhR7FWcBc+rETRJQBbhFKGWN3b2le5RfPI0O6veDJgAMSNJy83iOZWOXBi+auq\n"
        "Jz0Dyy+DLoIOZPcoHfyb9Ddp02SA92B1++kh5YteR6CLmFpPYVltUJU8FurkXPhN\n"
        "5WuENasfFTej4qc0jIbC0MrIobVuihPi/aQU7U745audLpH+0hob7YCUw/SfqW9u\n"
        "rwRgtRCPe8HMwKfr\n"
        "-----END CERTIFICATE-----\n";
}

/** Test the mock.
    This is just a sanity check to make sure our mock actually works. */
void
TestNetSecureContext::testMock()
{
    // Create virtual socket
    Ptr<Node> pNode(new Node());
    Ptr<Socket> sockA(new SocketMock(pNode, 0));
    Ptr<Socket> sockB(new SocketMock(pNode, 1));

    // Verify sockets work (this just verifies the mock!)
    afl::async::Controller ctl;
    static const uint8_t sendData[2] = { 'h', 'i' };
    afl::async::SendOperation sendOp;
    sendOp.setData(sendData);
    bool result = sockA->send(ctl, sendOp, 1000);
    TS_ASSERT(result);

    uint8_t receiveData[10];
    afl::async::ReceiveOperation receiveOp(receiveData);
    result = sockB->receive(ctl, receiveOp, 1000);
    TS_ASSERT(result);
    TS_ASSERT_EQUALS(receiveOp.getNumReceivedBytes(), 2U);
    TS_ASSERT_SAME_DATA(receiveData, "hi", 2);
}

/** Test wrapClient/wrapServer.
    This test fails if either of these operations does not perform proper flushing,
    which keeps one end of the connection in an intermediate status
    until the non-flushing side performs another operation. */
void
TestNetSecureContext::testConnect()
{
    // Create virtual socket
    Ptr<Node> pNode(new Node());
    Ptr<Socket> sockA(new SocketMock(pNode, 0));
    Ptr<Socket> sockB(new SocketMock(pNode, 1));
    afl::async::Controller ctl;

    // Create a SecureContext and configure it
    try {
        Ptr<afl::net::SecureContext> pContext(afl::net::SecureContext::create());
        pContext->setCertificate(afl::string::toBytes(CERTIFICATE));
        pContext->setPrivateKey(afl::string::toBytes(PRIVATE_KEY));

        // Convert both sockets to encrypted and verify that we can connect them.
        // We need a second thread for that.
        class ServerRunnable : public afl::base::Runnable {
         public:
            ServerRunnable(Ptr<afl::net::SecureContext> pContext, Ptr<Socket> socket)
                : m_pContext(pContext),
                  m_socket(socket)
                { }
            void run()
                {
                    Controller ctl;
                    m_socket = m_pContext->wrapServer(ctl, m_socket);
                }
         private:
            Ptr<afl::net::SecureContext> m_pContext;
            Ptr<Socket> m_socket;
        };
        ServerRunnable server(pContext, sockA);
        afl::sys::Thread serverThread("Server", server);
        serverThread.start();
        sockB = pContext->wrapClient(ctl, sockB);
        serverThread.join();
    }
    catch (afl::except::UnsupportedException& /*e*/) {
        // ok, unsupported
    }
    catch (...) {
        TS_ASSERT(0);
    }
}

/** Test basic data transfer.
    This tests that connecting works as well as send-after-wrapServer. */
void
TestNetSecureContext::testTransfer1()
{
    // Create virtual socket
    Ptr<Node> pNode(new Node());
    Ptr<Socket> sockA(new SocketMock(pNode, 0));
    Ptr<Socket> sockB(new SocketMock(pNode, 1));
    afl::async::Controller ctl;

    // Create a SecureContext and configure it
    try {
        Ptr<afl::net::SecureContext> pContext(afl::net::SecureContext::create());
        pContext->setCertificate(afl::string::toBytes(CERTIFICATE));
        pContext->setPrivateKey(afl::string::toBytes(PRIVATE_KEY));

        // Convert both sockets to encrypted and verify that we can connect and send data.
        // We need a second thread for that.
        class ServerRunnable : public afl::base::Runnable {
         public:
            ServerRunnable(Ptr<afl::net::SecureContext> pContext, Ptr<Socket> socket)
                : m_pContext(pContext),
                  m_socket(socket)
                { }
            void run()
                {
                    Controller ctl;
                    m_socket = m_pContext->wrapServer(ctl, m_socket);

                    // Send data
                    uint8_t dataToSend[] = {'a','b','c'};
                    afl::async::SendOperation op(dataToSend);
                    bool result = m_socket->send(ctl, op, 5000);
                    TS_ASSERT(result);
                    TS_ASSERT_EQUALS(op.getNumSentBytes(), 3U);
                }
         private:
            Ptr<afl::net::SecureContext> m_pContext;
            Ptr<Socket> m_socket;
        };
        ServerRunnable server(pContext, sockA);
        afl::sys::Thread serverThread("Server", server);
        serverThread.start();
        sockB = pContext->wrapClient(ctl, sockB);

        // Receive data
        uint8_t buf[10];
        afl::async::ReceiveOperation op(buf);
        bool result = sockB->receive(ctl, op, 5000);
        TS_ASSERT(result);
        TS_ASSERT_EQUALS(op.getNumReceivedBytes(), 3U);
        TS_ASSERT_EQUALS(buf[0], 'a');
        TS_ASSERT_EQUALS(buf[1], 'b');
        TS_ASSERT_EQUALS(buf[2], 'c');

        serverThread.join();
    }
    catch (afl::except::UnsupportedException& e) {
        // ok, unsupported
    }
    catch (...) {
        TS_ASSERT(0);
    }
}

/** Test data transfer.
    This tests that connecting works as well as send-after-wrapClient and send-after-receive. */
void
TestNetSecureContext::testTransfer2()
{
    // Create virtual socket
    Ptr<Node> pNode(new Node());
    Ptr<Socket> sockA(new SocketMock(pNode, 0));
    Ptr<Socket> sockB(new SocketMock(pNode, 1));
    afl::async::Controller ctl;

    // Create a SecureContext and configure it
    try {
        Ptr<afl::net::SecureContext> pContext(afl::net::SecureContext::create());
        pContext->setCertificate(afl::string::toBytes(CERTIFICATE));
        pContext->setPrivateKey(afl::string::toBytes(PRIVATE_KEY));

        // Convert both sockets to encrypted and verify that we can connect and send data.
        // We need a second thread for that.
        class ServerRunnable : public afl::base::Runnable {
         public:
            ServerRunnable(Ptr<afl::net::SecureContext> pContext, Ptr<Socket> socket)
                : m_pContext(pContext),
                  m_socket(socket)
                { }
            void run()
                {
                    Controller ctl;
                    m_socket = m_pContext->wrapServer(ctl, m_socket);

                    // Receive data and echo it back
                    uint8_t buf[10];
                    afl::async::ReceiveOperation op(buf);
                    bool result = m_socket->receive(ctl, op, 5000);
                    TS_ASSERT(result);
                    TS_ASSERT_EQUALS(op.getNumReceivedBytes(), 3U);

                    afl::async::SendOperation op2(op.getReceivedBytes());
                    result = m_socket->send(ctl, op2, 5000);
                    TS_ASSERT(result);
                    TS_ASSERT_EQUALS(op2.getNumSentBytes(), 3U);
                }
         private:
            Ptr<afl::net::SecureContext> m_pContext;
            Ptr<Socket> m_socket;
        };
        ServerRunnable server(pContext, sockA);
        afl::sys::Thread serverThread("Server", server);
        serverThread.start();
        sockB = pContext->wrapClient(ctl, sockB);

        // Send data
        const uint8_t dataToSend[] = {'a','b','c'};
        afl::async::SendOperation op(dataToSend);
        bool result = sockB->send(ctl, op, 5000);
        TS_ASSERT(result);
        TS_ASSERT_EQUALS(op.getNumSentBytes(), 3U);

        // Receive echo
        uint8_t buf[10];
        afl::async::ReceiveOperation op2(buf);
        result = sockB->receive(ctl, op2, 5000);
        TS_ASSERT(result);
        TS_ASSERT_EQUALS(op2.getNumReceivedBytes(), 3U);
        TS_ASSERT_EQUALS(buf[0], 'a');
        TS_ASSERT_EQUALS(buf[1], 'b');
        TS_ASSERT_EQUALS(buf[2], 'c');

        serverThread.join();
    }
    catch (afl::except::UnsupportedException& e) {
        // ok, unsupported
    }
    catch (...) {
        TS_ASSERT(0);
    }
}
