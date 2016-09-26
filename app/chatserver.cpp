/**
  *  \file app/chatserver.cpp
  *  \brief Sample Application: Chat Server
  *
  *  This example demonstrates asynchronous networking.
  *  It listens for connections, and waits for data on existing connections,
  *  in a single thread.
  *
  *  Received data is sent out to all (other) connections.
  *  For simplicity, sending is synchronous.
  *  Sending asynchronously would be straightforward to add,
  *  but need some more elaborate buffering.
  */

#include <cstdio>
#include <vector>
#include "afl/async/controller.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/net/acceptoperation.hpp"
#include "afl/net/listener.hpp"
#include "afl/net/name.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/socket.hpp"
#include "afl/net/securecontext.hpp"
#include "afl/net/securenetworkstack.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/io/stream.hpp"

// Imports
using afl::net::Socket;
using afl::net::Listener;
using afl::base::Ptr;

namespace {
    /** Read file into a vector.
        \param data [out] Vector
        \param fileName [in] File name */
    void readFile(std::vector<uint8_t>& data, const char* fileName)
    {
        using afl::base::Ptr;
        using afl::io::FileSystem;
        using afl::io::Stream;

        data.clear();
        FileSystem& fs(FileSystem::getInstance());
        Ptr<Stream> s = fs.openFile(fileName, FileSystem::OpenRead);
        uint8_t buf[1024];
        while (size_t n = s->read(buf)) {
            data.insert(data.end(), &buf[0], &buf[n]);
        }
    }

    /** Configure a SecureContext.
        \param ctx [in/out] Context
        \param keyFile [in] Name of private key file
        \param certFile [in] Name of certificate file */
    void configureContext(afl::net::SecureContext& ctx, const char* keyFile, const char* certFile)
    {
        std::vector<uint8_t> data;
        readFile(data, keyFile);
        if (!data.empty()) {
            ctx.setPrivateKey(afl::base::ConstBytes_t::unsafeCreate(&data[0], data.size()));
        }

        readFile(data, certFile);
        if (!data.empty()) {
            ctx.setCertificate(afl::base::ConstBytes_t::unsafeCreate(&data[0], data.size()));
        }
    }

    // Data structure for a client.
    // - derives from ReceiveOperation so we can easily find it when the operation returns from wait()
    // - contains a socket and a buffer to read from that socket
    struct Client : public afl::async::ReceiveOperation {
        Client(Ptr<Socket> socket)
            : m_reading(false),
              m_socket(socket)
            { }
        bool m_reading;
        Ptr<Socket> m_socket;
        uint8_t m_buffer[256];
    };
}

int main(int argc, char** argv)
{
    // Parameter validation
    if (argc != 2 && argc != 4) {
        std::printf("usage: %s HOST:PORT [PRIVKEY.pem CERTIFICATE.pem]\n\n"
                    "This will listen on the specified HOST:PORT\n"
                    "and relay data to all network clients (e.g. telnet).\n",
                    argv[0]);
        return 1;
    }

    // Get network stack
    afl::net::NetworkStack* pStack = &afl::net::NetworkStack::getInstance();

    try {
        // If KEY/CERT given, make it secure.
        // (spStack is used to keep the SecureNetworkStack alive.)
        Ptr<afl::net::NetworkStack> spStack;
        if (argc == 4) {
            Ptr<afl::net::SecureContext> ctx(afl::net::SecureContext::create());
            configureContext(*ctx, argv[2], argv[3]);
            spStack = new afl::net::SecureNetworkStack(*pStack, ctx);
            pStack = spStack.get();
        }

        // Set up listener
        Ptr<Listener> listener = pStack->listen(afl::net::Name::parse(argv[1], "4242"), 10);

        // Operate
        std::vector<Ptr<Client> > clients;
        afl::async::Controller ctl;
        afl::net::AcceptOperation acceptOp;
        bool accepting = false;
        while (1) {
            // Post all operations
            if (!accepting) {
                listener->acceptAsync(ctl, acceptOp);
                accepting = true;
            }
            for (std::vector<Ptr<Client > >::iterator i = clients.begin(); i != clients.end(); ++i) {
                Client& c = **i;
                if (!c.m_reading) {
                    c.setData(c.m_buffer);
                    c.m_socket->receiveAsync(ctl, c);
                    c.m_reading = true;
                }
            }

            // Wait
            afl::async::Operation* op = ctl.wait();
            if (op == &acceptOp) {
                // Got a new client
                std::printf("Got a new connection from '%s'.\n", acceptOp.getResult()->getPeerName().toString().c_str());
                clients.push_back(new Client(acceptOp.getResult()));
                accepting = false;
            } else {
                // Got data
                Client& c = *static_cast<Client*>(op);
                if (c.getNumReceivedBytes() == 0) {
                    // EOF
                    std::printf("Closing a connection.\n");
                    for (std::vector<Ptr<Client > >::iterator i = clients.begin(); i != clients.end(); ++i) {
                        if (*i == &c) {
                            clients.erase(i);
                            break;
                        }
                    }
                } else {
                    // Got data. Send them on all clients.
                    for (std::vector<Ptr<Client > >::iterator i = clients.begin(); i != clients.end(); ++i) {
                        if (*i != &c) {
                            afl::async::SendOperation data(c.getReceivedBytes());
                            (*i)->m_socket->send(ctl, data, 500);
                        }
                    }
                    c.m_reading = false;
                }
            }
        }
    }
    catch (std::exception& e) {
        std::printf("exception: %s\n", e.what());
    }
}
