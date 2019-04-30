/**
  *  \file arch/posix/posixnetworkstack.cpp
  *  \brief Class arch::posix::PosixNetworkStack
  *
  *  We currently have no dependencies to other modules,
  *  so we implement the whole NetworkStack as one big class.
  */

#if TARGET_OS_POSIX
#include "arch/posix/posixnetworkstack.hpp"

#include <cstring>
#include <cstdio>
#include <sys/types.h>          // socklen_t
#include <sys/socket.h>         // everything sockets
#include <arpa/inet.h>
#include <fcntl.h>              // fcntl
#include <netdb.h>              // getaddrinfo
#include <errno.h>              // errno
#include "afl/async/notifier.hpp"
#include "afl/except/filesystemexception.hpp"
#include "afl/net/acceptoperation.hpp"
#include "afl/string/messages.hpp"
#include "afl/sys/error.hpp"
#include "afl/sys/mutexguard.hpp"
#include "arch/controller.hpp"
#include "arch/posix/posixcontrollerimpl.hpp"
#include "arch/posix/posixfiledescriptor.hpp"

/*
 *  Utilities
 */

namespace {
    // Helper to create sockets using getaddrinfo
    class SocketFactory {
     public:
        virtual ~SocketFactory();
        virtual int process(int sock, const sockaddr* addr, socklen_t len) = 0;

        int toName(const afl::net::Name& name);
    };

    // Create a connect socket
    class Connect : public SocketFactory {
     public:
        Connect(afl::sys::Timeout_t timeout);
        virtual int process(int sock, const sockaddr* addr, socklen_t len);
     private:
        afl::sys::Timeout_t m_timeout;
    };

    // Create a bind/listen socket
    class Bind : public SocketFactory {
     public:
        virtual int process(int sock, const sockaddr* addr, socklen_t len);
    };


    /***************************** SocketFactory *****************************/

    inline SocketFactory::~SocketFactory()
    { }

    int SocketFactory::toName(const afl::net::Name& name)
    {
        // Reject names with null bytes
        if (name.getName().find('\0') != String_t::npos || name.getService().find('\0') != String_t::npos) {
            throw afl::except::FileProblemException(name.toString(), afl::string::Messages::networkNameNotFound());
        }

        // Where do we want to connect?
        struct addrinfo hints;
        struct addrinfo* result;
        std::memset(&hints, 0, sizeof(hints));

        hints.ai_socktype = SOCK_STREAM;
        int ai = ::getaddrinfo(name.getName().c_str(), name.getService().c_str(), &hints, &result);
        if (ai != 0) {
            throw afl::except::FileProblemException(name.toString(), gai_strerror(ai));
        }

        // Make a socket
        int sock = -1;
        int errorCode = 0;
        for (struct addrinfo* p = result; p != 0; p = p->ai_next) {
            sock = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if (sock >= 0) {
                // We never want to inherit sockets!
                ::fcntl(sock, F_SETFD, FD_CLOEXEC);

                // Check whether this socket is good for us.
                errorCode = process(sock, p->ai_addr, p->ai_addrlen);
                if (errorCode == 0) {
                    break;
                }

                // Nope.
                ::close(sock);
                sock = -1;
            }
        }

        // Clean up
        if (result != 0) {
            ::freeaddrinfo(result);
        }

        // Anything found?
        if (sock < 0) {
            if (errorCode == 0) {
                throw afl::except::FileProblemException(name.toString(), afl::string::Messages::networkNameNotFound());
            } else {
                throw afl::except::FileSystemException(name.toString(), afl::sys::Error(errorCode));
            }
        }
        return sock;
    }

    /******************************** Connect ********************************/

    inline Connect::Connect(afl::sys::Timeout_t timeout)
        : SocketFactory(),
          m_timeout(timeout)
    { }

    int Connect::process(int sock, const sockaddr* addr, socklen_t len)
    {
        // Make socket non-blocking
        ::fcntl(sock, F_SETFL, O_NONBLOCK);

        // Initiate connection
        if (::connect(sock, addr, len) == 0) {
            return 0;
        }

        // Handle asynchronous case
        if (errno == EINPROGRESS) {
            if (arch::posix::PosixFileDescriptor::waitReady(sock, m_timeout, arch::posix::PosixFileDescriptor::WaitForWrite)) {
                // Connect finished. Did it succeed?
                int status = 424242;
                socklen_t length = sizeof(status);
                if (::getsockopt(sock, SOL_SOCKET, SO_ERROR, &status, &length) == 0 && status == 0) {
                    // Success
                    return 0;
                } else {
                    // Error
                    return status;
                }
            } else {
                // Timeout
                return ETIMEDOUT;
            }
        } else {
            // Error
            return errno;
        }
    }

    /********************************** Bind *********************************/

    int Bind::process(int sock, const sockaddr* addr, socklen_t len)
    {
        int one = 1;
        ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
        if (::bind(sock, addr, len) == 0) {
            return 0;
        } else {
            return errno;
        }
    }
}

/*********************** PosixNetworkStack Classes ***********************/

/** Implementation of afl::net::Listener for POSIX. */
class arch::posix::PosixNetworkStack::Listener : public afl::net::Listener, public arch::posix::SelectRequest {
 public:
    Listener(int fd, const String_t& name);
    ~Listener();
    virtual afl::base::Ptr<afl::net::Socket> accept(afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);
    virtual void acceptAsync(afl::async::Controller& ctl, afl::net::AcceptOperation& op);
    virtual void cancel(afl::async::Controller& ctl, afl::async::Operation& op);

    virtual bool handleReadReady();
    virtual bool handleWriteReady();

 private:
    const int m_fd;
    const String_t m_name;

    // Async stuff
    afl::sys::Mutex m_mutex;
    afl::async::OperationList<afl::net::AcceptOperation> m_pendingAccepts;
};

/** Implementation of afl::net::Socket for POSIX. */
class arch::posix::PosixNetworkStack::Socket : public afl::net::Socket, private arch::posix::PosixFileDescriptor {
 public:
    typedef afl::async::Controller Controller;
    typedef afl::async::Operation Operation;
    typedef afl::async::SendOperation SendOperation;
    typedef afl::async::ReceiveOperation ReceiveOperation;

    Socket(int fd, const String_t& name);
    ~Socket();

    // CommunicationObject:
    virtual bool send(Controller& ctl, SendOperation& op, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);
    virtual void sendAsync(Controller& ctl, SendOperation& op);
    virtual bool receive(Controller& ctl, ReceiveOperation& op, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);
    virtual void receiveAsync(Controller& ctl, ReceiveOperation& op);
    virtual void cancel(Controller& ctl, Operation& op);
    virtual String_t getName();

    // Socket:
    virtual void closeSend();
    virtual afl::net::Name getPeerName();

 private:
    const String_t m_name;
};

/******************************** Listener *******************************/

inline
arch::posix::PosixNetworkStack::Listener::Listener(int fd, const String_t& name)
    : m_fd(fd),
      m_name(name),
      m_mutex(),
      m_pendingAccepts()
{ }

arch::posix::PosixNetworkStack::Listener::~Listener()
{
    ::close(m_fd);
}

afl::base::Ptr<afl::net::Socket>
arch::posix::PosixNetworkStack::Listener::accept(afl::sys::Timeout_t timeout)
{
    if (arch::posix::PosixFileDescriptor::waitReady(m_fd, timeout, arch::posix::PosixFileDescriptor::WaitForRead)) {
        // Accept a socket
        int data = ::accept(m_fd, 0, 0);
        if (data >= 0) {
            ::fcntl(data, F_SETFL, O_NONBLOCK);
            ::fcntl(data, F_SETFD, FD_CLOEXEC);
            return new Socket(data, m_name);
        }
    }

    // We end up here when there is a problem.
    // Our contract says we may not return null if timeout is INFINITE_TIMEOUT,
    // so throw an exception.
    if (timeout == afl::sys::INFINITE_TIMEOUT) {
        throw afl::except::FileProblemException(m_name, afl::string::Messages::networkError());
    }
    return 0;
}

void
arch::posix::PosixNetworkStack::Listener::acceptAsync(afl::async::Controller& ctl, afl::net::AcceptOperation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    arch::posix::PosixControllerImpl& pi = ctl.getImplementation();

    op.setController(&ctl);
    pi.addRequest(*this, op, m_fd, true);
    m_pendingAccepts.pushBack(&op);
}

void
arch::posix::PosixNetworkStack::Listener::cancel(afl::async::Controller& ctl, afl::async::Operation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    m_pendingAccepts.remove(&op);
    ctl.revertPost(op);
    ctl.getImplementation().removeRequest(op);

    // If the operation already has a result, discard that so it doesn't hang around longer than needed.
    static_cast<afl::net::AcceptOperation&>(op).setResult(0);
}

bool
arch::posix::PosixNetworkStack::Listener::handleReadReady()
{
    afl::sys::MutexGuard g(m_mutex);
    if (afl::net::AcceptOperation* op = m_pendingAccepts.front()) {
        // We have room for a socket. Try to accept one.
        int data = ::accept(m_fd, 0, 0);
        if (data >= 0) {
            ::fcntl(data, F_SETFL, O_NONBLOCK);
            ::fcntl(data, F_SETFD, FD_CLOEXEC);
            op->setResult(new Socket(data, m_name));
            op->getNotifier().notify(*op);
            m_pendingAccepts.extractFront();
        }
    }
    return m_pendingAccepts.empty();
}

bool
arch::posix::PosixNetworkStack::Listener::handleWriteReady()
{
    // Never happens, we're only waiting for read
    return false;
}

/********************************* Socket ********************************/

inline
arch::posix::PosixNetworkStack::Socket::Socket(int fd, const String_t& name)
    : afl::net::Socket(),
      PosixFileDescriptor(fd),
      m_name(name)
{ }

arch::posix::PosixNetworkStack::Socket::~Socket()
{ }

bool
arch::posix::PosixNetworkStack::Socket::send(Controller& ctl, SendOperation& op, afl::sys::Timeout_t timeout)
{
    return PosixFileDescriptor::send(ctl, op, timeout);
}

void
arch::posix::PosixNetworkStack::Socket::sendAsync(Controller& ctl, SendOperation& op)
{
    PosixFileDescriptor::sendAsync(ctl, op);
}

bool
arch::posix::PosixNetworkStack::Socket::receive(Controller& ctl, ReceiveOperation& op, afl::sys::Timeout_t timeout)
{
    return PosixFileDescriptor::receive(ctl, op, timeout);
}

void
arch::posix::PosixNetworkStack::Socket::receiveAsync(Controller& ctl, ReceiveOperation& op)
{
    PosixFileDescriptor::receiveAsync(ctl, op);
}

void
arch::posix::PosixNetworkStack::Socket::cancel(Controller& ctl, Operation& op)
{
    PosixFileDescriptor::cancel(ctl, op);
}

String_t
arch::posix::PosixNetworkStack::Socket::getName()
{
    return m_name;
}

void
arch::posix::PosixNetworkStack::Socket::closeSend()
{
    ::shutdown(getFileDescriptor(), SHUT_WR);
}

afl::net::Name
arch::posix::PosixNetworkStack::Socket::getPeerName()
{
    union {
        struct sockaddr_storage ss;
        struct sockaddr         sa;
        struct sockaddr_in      sin;
        struct sockaddr_in6     sin6;
    } ss;
    socklen_t slen = sizeof(ss);
    int n = ::getpeername(getFileDescriptor(), &ss.sa, &slen);

    afl::net::Name result;
    if (n >= 0) {
        if (ss.ss.ss_family == AF_INET) {
            char nameBuf[INET_ADDRSTRLEN];
            if (inet_ntop(ss.ss.ss_family, &ss.sin.sin_addr, nameBuf, sizeof(nameBuf))) {
                result = afl::net::Name(nameBuf, ntohs(ss.sin.sin_port));
            }
        }
        if (ss.ss.ss_family == AF_INET6) {
            char nameBuf[INET6_ADDRSTRLEN];
            if (inet_ntop(ss.ss.ss_family, &ss.sin6.sin6_addr, nameBuf, sizeof(nameBuf))) {
                result = afl::net::Name(nameBuf, ntohs(ss.sin6.sin6_port));
            }
        }
    }

    return result;
}

/*************************** PosixNetworkStack ***************************/

afl::base::Ref<afl::net::Listener>
arch::posix::PosixNetworkStack::listen(const afl::net::Name& name, int backlogSize)
{
    // Create listener socket
    int fd = Bind().toName(name);
    ::listen(fd, backlogSize);

    // Make socket non-blocking
    ::fcntl(fd, F_SETFL, O_NONBLOCK);
    return *new Listener(fd, name.toString());
}

afl::base::Ref<afl::net::Socket>
arch::posix::PosixNetworkStack::connect(const afl::net::Name& name, afl::sys::Timeout_t timeout)
{
    int fd = Connect(timeout).toName(name);
    return *new Socket(fd, name.toString());
}
#else
int g_variableToMakePosixNetworkStackObjectFileNotEmpty;
#endif
