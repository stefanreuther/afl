/**
  *  \file afl/net/tunnel/socks5networkstack.cpp
  *  \brief Class afl::net::tunnel::Socks5NetworkStack
  *
  *  SOCKS5:
  *  - https://tools.ietf.org/html/rfc1928
  *    - Client greeting: {5,n,method[n]}, server response {5,method}, followed by negotiation
  *    - CONNECT:
  *      - send {5,1,0,addrType,addr[*],port[2]}
  *        - addrType=1: 4 bytes IPv4
  *        - addrType=3: n bytes FQDN, 0
  *        - addrType=4: 16 bytes IPv6
  *    - BIND:
  *      - send {5,2,0,addrType,addr[*],port[2]}
  *    - UDP associate:
  *      - send {5,3,0,addrType,addr[*],port[2]}
  */

#include "afl/net/tunnel/socks5networkstack.hpp"
#include "afl/async/controller.hpp"
#include "afl/base/growablememory.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/except/unsupportedexception.hpp"
#include "afl/string/format.hpp"
#include "afl/string/messages.hpp"
#include "afl/string/parsememory.hpp"

namespace {
    const uint8_t SOCKS5_VERSION = 5;

    const uint8_t SOCKS5_AUTH_NONE = 0;
    // const uint8_t SOCKS5_AUTH_GSSAPI = 1;
    // const uint8_t SOCKS5_AUTH_USER_PASS = 2;
    // const uint8_t SOCKS5_AUTH_INACCEPTABLE = 0xFF;

    struct Socks5AuthReply {
        uint8_t version;
        uint8_t method;
    };

    uint8_t sendGreeting5(afl::async::Controller& ctl, afl::net::Socket& sock, afl::sys::Timeout_t timeout)
    {
        static const uint8_t GREETING[] = {SOCKS5_VERSION, 1, SOCKS5_AUTH_NONE};
        sock.fullSend(ctl, GREETING, timeout);

        Socks5AuthReply reply;
        sock.fullReceive(ctl, afl::base::fromObject(reply), timeout);
        if (reply.version != SOCKS5_VERSION) {
            throw afl::except::FileProblemException(sock.getName(), afl::string::Messages::syntaxError());
        }
        return reply.method;
    }

    typedef uint8_t PortNr_t[2];
    typedef uint8_t IPv4Address_t[4];
    typedef uint8_t IPv6Address_t[16];

    // FIXME: dupe
    bool parsePortNumber(afl::string::ConstStringMemory_t str, PortNr_t& out)
    {
        uint64_t parsed;
        if (afl::string::parseNumber(str, parsed) > 0
            && parsed > 0
            && parsed < 0x10000
            && str.empty())
        {
            out[0] = uint8_t(parsed >> 8);
            out[1] = uint8_t(parsed);
            return true;
        } else {
            return false;
        }
    }

    // FIXME: dupe
    bool parseOctet(afl::string::ConstStringMemory_t& str, uint8_t& out)
    {
        uint64_t parsed;
        if (afl::string::parseNumber(str, parsed)
            /*&& parsed >= 0*/
            && parsed < 256)
        {
            out = uint8_t(parsed);
            return true;
        } else {
            return false;
        }
    }

    // FIXME: dupe
    bool parseIPv4Address(afl::string::ConstStringMemory_t str, IPv4Address_t& out)
    {
        return parseOctet(str, out[0])
            && afl::string::parseChar(str, '.')
            && parseOctet(str, out[1])
            && afl::string::parseChar(str, '.')
            && parseOctet(str, out[2])
            && afl::string::parseChar(str, '.')
            && parseOctet(str, out[3])
            && str.empty();
    }

    struct Socks5Frame {
        uint8_t version;
        uint8_t code;
        uint8_t reserved;
        uint8_t addressType;
        // followed by address
        // followed by port
    };

    const uint8_t SOCKS5_OP_CONNECT = 1;
    // const uint8_t SOCKS5_OP_BIND = 2;
    // const uint8_t SOCKS5_OP_UDP_ASSOCIATE = 3;

    const uint8_t SOCKS5_ATYP_IPV4 = 1;
    const uint8_t SOCKS5_ATYP_DOMAIN = 3;
    const uint8_t SOCKS5_ATYP_IPV6 = 4;


    const char* getSocks5Message(uint8_t code)
    {
        switch (code) {
         case 0x01: return "General SOCKS server failure";
         case 0x02: return "Connection not allowed by ruleset";
         case 0x03: return "Network unreachable";
         case 0x04: return "Host unreachable";
         case 0x05: return "Connection refused";
         case 0x06: return "TTL expired";
         case 0x07: return "Command not supported";
         case 0x08: return "Address type not supported";
         default: return "?";
        }
    }

    bool buildRequest5(afl::base::GrowableBytes_t& buffer, uint8_t op, const afl::net::Name& name)
    {
        Socks5Frame frame;
        IPv4Address_t address;
        PortNr_t port;
        frame.version = SOCKS5_VERSION;
        frame.code = op;
        frame.reserved = 0;
        if (!parsePortNumber(afl::string::toMemory(name.getService()), port)) {
            return false;
        } else {
            if (parseIPv4Address(afl::string::toMemory(name.getName()), address)) {
                // IP given
                frame.addressType = SOCKS5_ATYP_IPV4;
                buffer.append(afl::base::fromObject(frame));
                buffer.append(afl::base::fromObject(address));
            } else {
                // Host name given
                frame.addressType = SOCKS5_ATYP_DOMAIN;
                buffer.append(afl::base::fromObject(frame));
                buffer.append(afl::string::toBytes(name.getName()));
                buffer.append(0);
            }
            buffer.append(afl::base::fromObject(port));
            return true;
        }
    }

    void sendConnectRequest5(afl::async::Controller& ctl, afl::net::Socket& socket, const afl::net::Name& name, afl::sys::Timeout_t timeout)
    {
        afl::base::GrowableBytes_t buffer;
        if (!buildRequest5(buffer, SOCKS5_OP_CONNECT, name)) {
            throw afl::except::FileProblemException(name.toString(), "bad port");
        }
        socket.fullSend(ctl, buffer, timeout);
    }

    void receiveConnectResponse5(afl::async::Controller& ctl, afl::net::Socket& socket, const afl::net::Name& name, afl::sys::Timeout_t timeout)
    {
        Socks5Frame frame;
        socket.fullReceive(ctl, afl::base::fromObject(frame), timeout);
        if (frame.version != 0 && frame.version != SOCKS5_VERSION) {
            // This is an error in the proxy; it didn't speak protocol
            throw afl::except::FileProblemException(socket.getName(), afl::string::Messages::syntaxError());
        }
        if (frame.code != 0) {
            // Proxy reported an error
            throw afl::except::FileProblemException(name.toString(), afl::string::Format("%s (SOCKS5 error code: %d)", getSocks5Message(frame.code), frame.code));
        }
        switch (frame.addressType) {
         case SOCKS5_ATYP_IPV4: {
            IPv4Address_t addr;
            socket.fullReceive(ctl, afl::base::fromObject(addr), timeout);
            break;
         }
         case SOCKS5_ATYP_DOMAIN: {
            uint8_t byte[1];
            do {
                socket.fullReceive(ctl, byte, timeout);
            } while (byte[0] != 0);
            break;
         }
         case SOCKS5_ATYP_IPV6: {
            IPv6Address_t addr;
            socket.fullReceive(ctl, afl::base::fromObject(addr), timeout);
            break;
         }
        }

        PortNr_t port;
        socket.fullReceive(ctl, afl::base::fromObject(port), timeout);
    }
}


afl::net::tunnel::Socks5NetworkStack::Socks5NetworkStack(NetworkStack& parent, const Name& serverAddress)
    : m_parent(parent),
      m_serverAddress(serverAddress)
{ }

afl::net::tunnel::Socks5NetworkStack::~Socks5NetworkStack()
{ }

afl::base::Ref<afl::net::Listener>
afl::net::tunnel::Socks5NetworkStack::listen(const Name& /*name*/, int /*backlogSize*/)
{
    throw afl::except::UnsupportedException("Socks5.listen");
}

afl::base::Ref<afl::net::Socket>
afl::net::tunnel::Socks5NetworkStack::connect(const Name& name, afl::sys::Timeout_t timeout)
{
    afl::base::Ref<Socket> result = m_parent.connect(m_serverAddress, timeout);
    afl::async::Controller ctl;

    // Greeting and authentification
    uint8_t authMode = sendGreeting5(ctl, *result, timeout);
    switch (authMode) {
     case SOCKS5_AUTH_NONE:
        break;

     default:
        throw afl::except::FileProblemException(name.toString(), "unsupported authentification");
    }

    // Connection request
    sendConnectRequest5(ctl, *result, name, timeout);
    receiveConnectResponse5(ctl, *result, name, timeout);
    return result;
}
