/**
  *  \file afl/net/tunnel/socks4networkstack.cpp
  *  \brief Class afl::net::tunnel::Socks5NetworkStack
  *
  *  SOCKS4:
  *  - http://ftp.icm.edu.pl/packages/socks/socks4/SOCKS4.protocol
  *    - CONNECT:
  *      - send {4,1,port[2],ip[4],userId[*],0}
  *      - receive {0,nn,ignore[6]}, where nn is the status code
  *        (90=ok, 91=reject/fail, 92=cannot connect to identd, 93=identd disagrees with userId)
  *    - BIND:
  *      - send {4,2,port[2],ip[4],userId[*],0}
  *      - receive {0,nn,port[2],ip[4]}, TWICE, where nn is the status code as above.
  *        First reply is getsockname, second is getpeername (?)
  *  SOCKS4a:
  *  - https://www.openssh.com/txt/socks4a.protocol
  *    - CONNECT and BIND can be sent as
  *      {4,op,port[2],0,0,0,nonzero,userId[*],0,domain,0}
  *      to use the remote resolver.
  *
  *  Tested against:
  *  - OpenSSH_6.7p1 ("-D 5555"):
  *    - supports CONNECT, but not BIND
  *    - just closes the connection on error, does not send a reply
  *  - srelay 0.4.8:
  *    - supports CONNECT and BIND
  *    - BIND has restrictions: it will time out, it will not use SO_REUSEADDR, it will not
  *      necessarily bind to the port we requested (for example, when it's not yet reusable
  *      due to SO_REUSEADDR; making it unusable for offering services).
  */

#include <memory>
#include "afl/net/tunnel/socks4networkstack.hpp"
#include "afl/async/communicationobject.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/notifier.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/base/growablememory.hpp"
#include "afl/container/ptrvector.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/net/acceptoperation.hpp"
#include "afl/net/tunnel/socketwrapper.hpp"
#include "afl/string/format.hpp"
#include "afl/string/messages.hpp"
#include "afl/string/parsememory.hpp"
#include "afl/sys/mutexguard.hpp"

namespace {
    typedef uint8_t PortNr_t[2];
    typedef uint8_t IPv4Address_t[4];

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

    bool isDummyAddress(const IPv4Address_t& addr)
    {
        return addr[0] == 0 && addr[1] == 0 && addr[2] == 0 && addr[3] != 0;
    }

    void setDummyAddress(IPv4Address_t& addr)
    {
        addr[0] = 0;
        addr[1] = 0;
        addr[2] = 0;
        addr[3] = 1;
    }

    struct Socks4Frame {
        uint8_t version;
        uint8_t code;
        PortNr_t port;
        IPv4Address_t address;
        // followed by userId + NUL
        // [followed by hostname + NUL]
    };

    const uint8_t SOCKS4_VERSION = 4;
    const uint8_t SOCKS4_OP_CONNECT = 1;
    const uint8_t SOCKS4_OP_BIND = 2;
    const uint8_t SOCKS4_RESPONSE = 0;

    const char* getSocks4Message(uint8_t code)
    {
        switch (code) {
         case 91: return "Request rejected or failed";
         case 92: return "Authentication service not accessible";
         case 93: return "Authentication mismatch";
         default: return "?";
        }
    }

    bool buildRequest4(afl::base::GrowableBytes_t& buffer, uint8_t op, const afl::net::Name& name)
    {
        Socks4Frame frame;
        frame.version = SOCKS4_VERSION;
        frame.code = op;
        if (!parsePortNumber(afl::string::toMemory(name.getService()), frame.port)) {
            return false;
        } else {
            if (parseIPv4Address(afl::string::toMemory(name.getName()), frame.address) && !isDummyAddress(frame.address)) {
                // IP given (SOCKS4)
                buffer.append(afl::base::fromObject(frame));
                buffer.append(0);
            } else {
                // Host name given (SOCKS4a)
                setDummyAddress(frame.address);
                buffer.append(afl::base::fromObject(frame));
                buffer.append(0);
                buffer.append(afl::string::toBytes(name.getName()));
                buffer.append(0);
            }
            return true;
        }
    }

    void sendConnectRequest4(afl::async::Controller& ctl, afl::net::Socket& socket, const afl::net::Name& name, afl::sys::Timeout_t timeout)
    {
        afl::base::GrowableBytes_t buffer;
        if (!buildRequest4(buffer, SOCKS4_OP_CONNECT, name)) {
            throw afl::except::FileProblemException(name.toString(), "bad port");
        }
        socket.fullSend(ctl, buffer, timeout);
    }

    void receiveConnectResponse4(afl::async::Controller& ctl, afl::net::Socket& socket, const afl::net::Name& name, afl::sys::Timeout_t timeout)
    {
        Socks4Frame frame;
        socket.fullReceive(ctl, afl::base::fromObject(frame), timeout);
        if (frame.version != 0 && frame.version != SOCKS4_VERSION) {
            // This is an error in the proxy; it didn't speak protocol
            throw afl::except::FileProblemException(socket.getName(), afl::string::Messages::syntaxError());
        }
        if (frame.code != 90) {
            // Proxy reported an error
            throw afl::except::FileProblemException(name.toString(), afl::string::Format("%s (SOCKS4 error code: %d)", getSocks4Message(frame.code), frame.code));
        }
    }
}

/***************************** Nested Classes ****************************/

class afl::net::tunnel::Socks4NetworkStack::ListenerImpl : public afl::net::Listener {
 public:
    ListenerImpl(NetworkStack& parent, const Name& serverAddress, const Name& listenAddress);
    ~ListenerImpl();
    virtual afl::base::Ptr<Socket> accept(afl::sys::Timeout_t timeout);
    void acceptAsync(afl::async::Controller& ctl, AcceptOperation& op);
    void cancel(afl::async::Controller& ctl, afl::async::Operation& op);

    AcceptorImpl* extractAcceptor(afl::async::Operation& op);

 private:
    NetworkStack& m_parent;
    Name m_serverAddress;
    Name m_listenAddress;

    afl::sys::Mutex m_activeMutex;
    afl::container::PtrVector<AcceptorImpl> m_activeOperations;
};


class afl::net::tunnel::Socks4NetworkStack::AcceptorImpl : public afl::async::Notifier {
 public:
    AcceptorImpl(ListenerImpl& parent,
                 afl::async::Controller& ctl,
                 AcceptOperation& userOperation,
                 afl::base::Ref<Socket> socket,
                 const Name& listenAddress);

    void start();

    virtual void notify(afl::async::Operation& op);
    virtual void notifyDirect(afl::async::Operation& op);

    bool isOperation(const afl::async::Operation& op) const;
    void cancel();

 private:
    ListenerImpl& m_parent;
    afl::async::Controller& m_controller;
    AcceptOperation& m_userOperation;
    afl::base::Ref<Socket> m_socket;
    const Name& m_listenAddress;

    afl::base::GrowableBytes_t m_request;
    afl::async::SendOperation m_requestSend;

    Socks4Frame m_response[2];
    afl::async::ReceiveOperation m_responseReceive;

    enum State {
        Idle,
        Sending,
        Receiving,
        Finished
    };
    State m_state;

    void finish(afl::base::Ptr<Socket> result);
};

/****************************** ListenerImpl *****************************/

inline
afl::net::tunnel::Socks4NetworkStack::ListenerImpl::ListenerImpl(NetworkStack& parent, const Name& serverAddress, const Name& listenAddress)
    : m_parent(parent),
      m_serverAddress(serverAddress),
      m_listenAddress(listenAddress)
{ }

afl::net::tunnel::Socks4NetworkStack::ListenerImpl::~ListenerImpl()
{ }

afl::base::Ptr<afl::net::Socket>
afl::net::tunnel::Socks4NetworkStack::ListenerImpl::accept(afl::sys::Timeout_t timeout)
{
    afl::base::Ptr<Socket> result;
    {
        afl::async::Controller ctl;
        AcceptOperation op;
        acceptAsync(ctl, op);
        if (ctl.wait(op, timeout)) {
            if (timeout == afl::sys::INFINITE_TIMEOUT && op.getResult().get() == 0) {
                throw afl::except::FileProblemException(m_listenAddress.toString(), afl::string::Messages::networkError());
            }
            result = op.getResult();
        } else {
            cancel(ctl, op);
            result = 0;
        }
    }
    return result;
}

void
afl::net::tunnel::Socks4NetworkStack::ListenerImpl::acceptAsync(afl::async::Controller& ctl, AcceptOperation& op)
{
    // FIXME: would normally need asynchronous connect but we don't have that yet
    afl::base::Ref<Socket> result = m_parent.connect(m_serverAddress, 10000);
    AcceptorImpl* p;
    {
        afl::sys::MutexGuard g(m_activeMutex);
        p = m_activeOperations.pushBackNew(new AcceptorImpl(*this, ctl, op, result, m_listenAddress));
    }
    p->start();
}

void
afl::net::tunnel::Socks4NetworkStack::ListenerImpl::cancel(afl::async::Controller& ctl, afl::async::Operation& op)
{
    std::auto_ptr<AcceptorImpl> p(extractAcceptor(op));
    if (p.get() != 0) {
        p->cancel();
    }
    ctl.revertPost(op);
}

afl::net::tunnel::Socks4NetworkStack::AcceptorImpl*
afl::net::tunnel::Socks4NetworkStack::ListenerImpl::extractAcceptor(afl::async::Operation& op)
{
    afl::sys::MutexGuard g(m_activeMutex);
    std::auto_ptr<AcceptorImpl> p;
    for (size_t i = 0, n = m_activeOperations.size(); i < n; ++i) {
        if (m_activeOperations[i]->isOperation(op)) {
            p.reset(m_activeOperations.extractElement(i));
            m_activeOperations.erase(m_activeOperations.begin() + i);
            break;
        }
    }
    return p.release();
}


/****************************** AcceptorImpl *****************************/

inline
afl::net::tunnel::Socks4NetworkStack::AcceptorImpl::AcceptorImpl(ListenerImpl& parent,
                                                                 afl::async::Controller& ctl,
                                                                 AcceptOperation& userOperation,
                                                                 afl::base::Ref<Socket> socket,
                                                                 const Name& listenAddress)
    : m_parent(parent),
      m_controller(ctl),
      m_userOperation(userOperation),
      m_socket(socket),
      m_listenAddress(listenAddress),
      m_request(),
      m_requestSend(),
      m_response(),
      m_responseReceive(),
      m_state(Idle)
{
    m_userOperation.setController(&m_controller);
}

inline void
afl::net::tunnel::Socks4NetworkStack::AcceptorImpl::start()
{
    if (m_state == Idle) {
        // build packet to send
        if (!buildRequest4(m_request, SOCKS4_OP_BIND, m_listenAddress)) {
            finish(0);
        } else {
            m_requestSend.setData(m_request);
            m_requestSend.setNotifier(*this);
            m_state = Sending;
            m_socket->sendAsync(m_controller, m_requestSend);
        }
    }
}

void
afl::net::tunnel::Socks4NetworkStack::AcceptorImpl::notify(afl::async::Operation& /*op*/)
{
    switch (m_state) {
     case Idle:
        break;
     case Sending:
        if (m_requestSend.getUnsentBytes().empty()) {
            // Nothing more to send, proceed to Receiving
            m_state = Receiving;
            m_responseReceive.setData(afl::base::fromObject(m_response));
            m_responseReceive.setNotifier(*this);
            m_socket->receiveAsync(m_controller, m_responseReceive);
        } else if (m_requestSend.getNumSentBytes() == 0) {
            // Cannot send
            finish(0);
        } else {
            // Partially sent
            m_requestSend.setData(m_requestSend.getUnsentBytes());
            m_requestSend.setNotifier(*this);
            m_socket->sendAsync(m_controller, m_requestSend);
        }
        break;

     case Receiving:
        if (m_responseReceive.getUnreceivedBytes().empty()) {
            // Nothing more to receive
            if (m_response[0].code != 90 || m_response[1].code != 90) {
                finish(0);
            } else {
                finish(new SocketWrapper(m_socket, m_listenAddress,
                                         afl::net::Name(afl::string::Format("%d.%d.%d.%d",
                                                                            m_response[1].address[0],
                                                                            m_response[1].address[1],
                                                                            m_response[1].address[2],
                                                                            m_response[1].address[3]),
                                                        afl::string::Format("%d", 256*m_response[1].port[0] + m_response[1].port[1]))));
            }
        } else if (m_responseReceive.getNumReceivedBytes() == 0) {
            // Cannot receive
            finish(0);
        } else {
            // Partially received
            m_responseReceive.setData(m_responseReceive.getUnreceivedBytes());
            m_responseReceive.setNotifier(*this);
            m_socket->receiveAsync(m_controller, m_responseReceive);
        }
        break;

     case Finished:
        break;
    }
}

void
afl::net::tunnel::Socks4NetworkStack::AcceptorImpl::notifyDirect(afl::async::Operation& op)
{
    notify(op);
}

inline bool
afl::net::tunnel::Socks4NetworkStack::AcceptorImpl::isOperation(const afl::async::Operation& op) const
{
    return &op == &m_userOperation;
}

inline void
afl::net::tunnel::Socks4NetworkStack::AcceptorImpl::cancel()
{
    switch (m_state) {
     case Idle:
        break;
     case Sending:
        m_socket->cancel(m_controller, m_requestSend);
        break;
     case Receiving:
        m_socket->cancel(m_controller, m_responseReceive);
        break;
     case Finished:
        break;
    }
}

void
afl::net::tunnel::Socks4NetworkStack::AcceptorImpl::finish(afl::base::Ptr<Socket> result)
{
    std::auto_ptr<AcceptorImpl> a(m_parent.extractAcceptor(m_userOperation));
    m_state = Finished;
    m_userOperation.setResult(result);
    m_userOperation.getNotifier().notify(m_userOperation);
}

/*************************** Socks4NetworkStack **************************/

afl::net::tunnel::Socks4NetworkStack::Socks4NetworkStack(NetworkStack& parent, const Name& serverAddress)
    : NetworkStack(),
      m_parent(parent),
      m_serverAddress(serverAddress)
{ }

afl::net::tunnel::Socks4NetworkStack::~Socks4NetworkStack()
{ }

afl::base::Ref<afl::net::Listener>
afl::net::tunnel::Socks4NetworkStack::listen(const Name& name, int /*backlogSize*/)
{
    return *new ListenerImpl(m_parent, m_serverAddress, name);
}

afl::base::Ref<afl::net::Socket>
afl::net::tunnel::Socks4NetworkStack::connect(const Name& name, afl::sys::Timeout_t timeout)
{
    afl::base::Ref<Socket> result = m_parent.connect(m_serverAddress, timeout);
    afl::async::Controller ctl;
    sendConnectRequest4(ctl, *result, name, timeout);
    receiveConnectResponse4(ctl, *result, name, timeout);
    return result;
}
