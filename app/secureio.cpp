/**
  *  \file app/secureio.cpp
  *  \brief Sample application: Secure Sockets
  *
  *  This is two programs in one:
  *  - a SSL server. Invoke as
  *        secureio listen 127.0.0.1:44444 PRIVKEY.pem CERTIFICATE.pem
  *    to listen for connections.
  *  - a SSL client. Invoke as
  *        secureio connect 127.0.0.1:44444
  *    to connect to a server.
  *
  *  It implements a simple, synchronous protocol: type a line into the client,
  *  and it is echoed back to the client.
  */

#include <cstring>
#include <iostream>
#include <vector>
#include "afl/async/controller.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/base/ptr.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/io/stream.hpp"
#include "afl/net/name.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/securecontext.hpp"
#include "afl/net/securenetworkstack.hpp"
#include "afl/net/securesocket.hpp"
#include "afl/net/socket.hpp"

/* Define this to 0 to use plain-text communication. */
#define SECURE 1

namespace {
    /** Read file into a vector.
        \param data [out] Vector
        \param fileName [in] File name */
    void readFile(std::vector<uint8_t>& data, const char* fileName)
    {
        using afl::base::Ref;
        using afl::io::FileSystem;
        using afl::io::Stream;

        data.clear();
        FileSystem& fs(FileSystem::getInstance());
        Ref<Stream> s = fs.openFile(fileName, FileSystem::OpenRead);
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

    /** Receive a line from a socket.
        Sort of a std::getline replacement.
        Simple unbuffered implementation.
        \param ctl [in] Controller
        \param sock [in] Socket
        \param line [out] Line */
    bool receiveLine(afl::async::Controller& ctl, afl::net::Socket& sock, std::string& line)
    {
        line.clear();
        while (1) {
            uint8_t ch[1];
            afl::async::ReceiveOperation op(ch);
            sock.receive(ctl, op);
            if (op.getNumReceivedBytes() == 0) {
                return false;
            }
            if (ch[0] == '\n') {
                return true;
            }
            line += static_cast<char>(ch[0]);
        }
    }

    /** Server.
        \param ctl Controller
        \param sock Connected socket */
    void doServer(afl::async::Controller& ctl, afl::net::Socket& sock)
    {
        std::string line;
        while (receiveLine(ctl, sock, line)) {
            // Got a line. Log it and send it back.
            std::cout << "> " << line << "\n";

            line += '\n';
            afl::async::SendOperation op(afl::string::toBytes(line));
            sock.send(ctl, op);
        }
    }

    /** Client.
        \param ctl Controller
        \param sock Connected socket */
    void doClient(afl::async::Controller& ctl, afl::net::Socket& sock)
    {
        std::string line;
        while (getline(std::cin, line)) {
            // Send a line
            line += '\n';
            afl::async::SendOperation op(afl::string::toBytes(line));
            sock.send(ctl, op);

            // Read a line
            if (!receiveLine(ctl, sock, line)) {
                break;
            }
            std::cout << "> " << line << "\n";
        }
    }
}

int main(int argc, char** argv)
{
    using afl::base::Ptr;
    using afl::base::Ref;
    using afl::net::Socket;
    using afl::net::Listener;
    using afl::net::NetworkStack;
    using afl::net::SecureContext;
    using afl::net::Name;
    using afl::async::Controller;

    if (argc == 5 && std::strcmp(argv[1], "listen") == 0) {
        try {
            Controller ctl;
            Ref<SecureContext> ctx(SecureContext::create());
            configureContext(*ctx, argv[3], argv[4]);

            Ref<Listener> listener(NetworkStack::getInstance().listen(Name::parse(argv[2], "44444"), 5));
            while (1) {
                // FIXME: null
                Ref<Socket> sock(*listener->accept());
                std::cout << "Got a connection.\n";
#if SECURE
                sock.reset(*ctx->wrapServer(ctl, sock));
                std::cout << "Encryption ok.\n";
#endif
                doServer(ctl, *sock);
                std::cout << "Connection closes.\n";
            }
        }
        catch (std::exception& e) {
            std::cerr << "exception: " << e.what() << "\n";
        }
    } else if (argc == 3 && std::strcmp(argv[1], "connect") == 0) {
        try {
            Controller ctl;
            Ref<SecureContext> ctx(SecureContext::create());
            Ref<Socket> sock(NetworkStack::getInstance().connect(Name::parse(argv[2], "44444")));
            std::cout << "Connected.\n";
#if SECURE
            sock.reset(*ctx->wrapClient(ctl, sock));
            std::cout << "Encryption ok.\n";
#endif
            doClient(ctl, *sock);
            std::cout << "Connection closes.\n";
        }
        catch (std::exception& e) {
            std::cerr << "exception: " << e.what() << "\n";
        }
    } else {
        std::cout << "Usage:\n"
                  << "    " << argv[0] << " listen HOST:PORT PRIVKEY.pem CERT.pem\n"
                  << "    " << argv[0] << " connect HOST:PORT\n";
    }
}
