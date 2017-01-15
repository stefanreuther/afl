/**
  *  \file app/respserver.cpp
  *  \brief Sample application: RESP server
  *
  *  Invoke as "respserver host:port" to serve an InternalDatabase on the given address using a SimpleServer (single connection).
  *  Invoke as "respserver -multi host:port" to serve using a Server (multiple concurrent connection).
  *
  *  You can then use "respclient" or "redis-cli" to talk to it.
  *  To terminate, kill the process.
  *  This will not persist data.
  */

#include <iostream>
#include "afl/sys/environment.hpp"
#include "afl/base/ptr.hpp"
#include "afl/net/name.hpp"
#include "afl/net/redis/internaldatabase.hpp"
#include "afl/net/simpleserver.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/protocolhandlerfactory.hpp"
#include "afl/net/resp/protocolhandler.hpp"
#include "afl/net/server.hpp"
#include "afl/sys/loglistener.hpp"

namespace {
    /*
     *  Protocol Handler Factory for respserver.
     *  We do not implement per-session state.
     *  It therefore suffices to make all connections talk to the same CommandHandler.
     */
    class MyProtocolHandlerFactory : public afl::net::ProtocolHandlerFactory {
     public:
        MyProtocolHandlerFactory(afl::net::CommandHandler& ch)
            : m_ch(ch)
            { }

        virtual afl::net::ProtocolHandler* create()
            { return new afl::net::resp::ProtocolHandler(m_ch); }

     private:
        afl::net::CommandHandler& m_ch;
    };


    class MyLogger : public afl::sys::LogListener {
     public:
        virtual void handleMessage(const Message& msg)
            { std::cout << msg.m_message << "\n"; }
    };
}

int main(int /*argc*/, char** argv)
{
    // Fetch command line
    afl::base::Ref<afl::sys::Environment::CommandLine_t> cmdl = afl::sys::Environment::getInstance(argv).getCommandLine();

    // Single/Multi flag
    bool multi = false;

    // First element is network address
    String_t str;
    if (!cmdl->getNextElement(str)) {
        std::cout << "Missing network address.\n";
        return 1;
    }
    if (str == "-multi") {
        multi = true;
        if (!cmdl->getNextElement(str)) {
            std::cout << "Missing network address.\n";
            return 1;
        }
    }
    afl::net::Name name = afl::net::Name::parse(str, "6666");

    // Must be just one argument
    if (cmdl->getNextElement(str)) {
        std::cout << "Too many arguments.\n";
        return 1;
    }

    // Do it!
    try {
        afl::net::redis::InternalDatabase idb;
        MyProtocolHandlerFactory factory(idb);
        MyLogger logger;
        if (multi) {
            afl::net::Server server(afl::net::NetworkStack::getInstance().listen(name, 10), factory);
            server.log().addListener(logger);
            server.run();
        } else {
            afl::net::SimpleServer server(afl::net::NetworkStack::getInstance().listen(name, 10), factory);
            server.run();
        }
    }
    catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << "\n";
        return 1;
    }
    catch (...) {
        std::cerr << "unknown exception\n";
        return 0;
    }
}
