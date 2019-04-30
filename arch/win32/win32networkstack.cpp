/**
  *  \file arch/win32/win32networkstack.cpp
  *  \brief Class arch::win32::Win32NetworkStack
  *
  *  This module is almost identical to the POSIX version, with just some oh-so-lovely subtle differences.
  *  One freedom I'm allowing myself is to assume that GetLastError and WSAGetLastError are the same
  *  (which they are in Winsock2).
  */

#if TARGET_OS_WIN32
#define _WIN32_WINNT 0x0501   // Windows XP; required to get getaddrinfo
#include <winsock2.h>         // Must be the very first include, even before other "harmless" C library includes
#include <ws2tcpip.h>         // getaddrinfo
#include <windows.h>

#include "arch/win32/win32networkstack.hpp"

#include <stdexcept>
#include <cstring>
#include "arch/win32/waitrequest.hpp"
#include "afl/except/filesystemexception.hpp"
#include "afl/except/systemexception.hpp"
#include "afl/sys/error.hpp"
#include "afl/sys/mutex.hpp"
#include "afl/sys/mutexguard.hpp"
#include "afl/string/messages.hpp"
#include "afl/async/operationlist.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/async/receiveoperation.hpp"
#include "arch/controller.hpp"   // afl::async::Controller + Implementation
#include "afl/net/acceptoperation.hpp"
#include "afl/async/notifier.hpp"
#include "afl/string/parse.hpp"

/*
 *  Utilities
 */

namespace {
    // Win32 does not have socklen_t, so give it one.
    typedef int socklen_t;

    // Helper to create sockets
    class SocketFactory {
     public:
        virtual ~SocketFactory();
        virtual DWORD process(SOCKET sock, const sockaddr* addr, socklen_t len) = 0;

        SOCKET toName(const afl::net::Name& name);
    };

    // Create a connect socket
    class Connect : public SocketFactory {
    public:
        Connect(afl::sys::Timeout_t timeout);
        virtual DWORD process(SOCKET sock, const sockaddr* addr, socklen_t len);
     private:
        afl::sys::Timeout_t m_timeout;
    };

    // Create a bind/listen socket
    class Bind : public SocketFactory {
    public:
        virtual DWORD process(SOCKET sock, const sockaddr* addr, socklen_t len);
    };

    // Helper for select() syscall
    enum WaitMode {
        WaitForAccept,
        WaitForConnect
    };
    bool waitReady(SOCKET fd, afl::sys::Timeout_t timeout, WaitMode mode);

    /*
     *  Access to name service
     *
     *  getaddrinfo is not available until Windows XP (Winsock 2.2).
     *  We therefore load them from the DLL if we can.
     */

    typedef int (WSAAPI *getaddrinfo_t)(PCSTR, PCSTR, const addrinfo*, addrinfo**);
    typedef void (WSAAPI *freeaddrinfo_t)(addrinfo*);

    // Helper to access name resolver
    class Resolver {
     public:
        virtual ~Resolver() { }
        virtual SOCKET toName(SocketFactory& f, const afl::net::Name& name) = 0;
    };

    // Legacy connector: using gethostbyname
    class LegacyResolver : public Resolver {
     public:
        LegacyResolver(afl::sys::Mutex& mtx);
        SOCKET toName(SocketFactory& f, const afl::net::Name& name);
     private:
        afl::sys::Mutex& m_mutex;
    };

    // Modern connector: using getaddrinfo
    class ModernResolver : public Resolver {
     public:
        ModernResolver(getaddrinfo_t gai, freeaddrinfo_t fai);
        SOCKET toName(SocketFactory& f, const afl::net::Name& name);
     private:
        getaddrinfo_t m_getAddrInfo;
        freeaddrinfo_t m_freeAddrInfo;
    };

    /*
     *  LegacyResolver
     */
    LegacyResolver::LegacyResolver(afl::sys::Mutex& mtx)
        : m_mutex(mtx)
    { }

    SOCKET LegacyResolver::toName(SocketFactory& f, const afl::net::Name& name)
    {
        // Must guard lookup requests because they use static data
        struct sockaddr_in in;
        {
            afl::sys::MutexGuard g(m_mutex);

            // Look up host
            struct hostent* h = ::gethostbyname(name.getName().c_str());
            if (h == 0) {
                throw afl::except::FileProblemException(name.toString(), afl::string::Messages::networkNameNotFound());
            }

            // Look up port
            short port = 0;
            if (!afl::string::strToInteger(name.getService(), port)) {
                throw afl::except::FileProblemException(name.toString(), afl::string::Messages::networkNameNotFound());
            }

            // Convert to sockaddr_in
            size_t sz = size_t(h->h_length);
            if (sz > sizeof(in.sin_addr)) {
                throw afl::except::FileProblemException(name.toString(), afl::string::Messages::unsupportedFeature());
            }
            ::memcpy(&in.sin_addr, h->h_addr_list[0], sz);
            in.sin_family = AF_INET;
            in.sin_port = ::htons(port);
        }

        // Make a socket
        SOCKET sock = ::socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            throw afl::except::FileSystemException(name.toString(), afl::sys::Error(GetLastError()));
        }

        // Check whether this socket is good for us.
        DWORD errorCode = f.process(sock, (sockaddr*)&in, sizeof(in));
        if (errorCode != 0) {
            ::closesocket(sock);
            throw afl::except::FileSystemException(name.toString(), afl::sys::Error(errorCode));
        }

        return sock;
    }

    /*
     *  ModernResolver
     */
    ModernResolver::ModernResolver(getaddrinfo_t gai, freeaddrinfo_t fai)
        : m_getAddrInfo(gai),
          m_freeAddrInfo(fai)
    { }

    SOCKET ModernResolver::toName(SocketFactory& f, const afl::net::Name& name)
    {
        // Where do we want to connect?
        // FIXME: these functions are documented to take ANSI, We give them UTF-8.
        // This should be mostly the same.
        struct addrinfo hints;
        struct addrinfo* result;
        std::memset(&hints, 0, sizeof(hints));

        hints.ai_socktype = SOCK_STREAM;
        DWORD ai = m_getAddrInfo(name.getName().c_str(), name.getService().c_str(), &hints, &result);
        if (ai != 0) {
            throw afl::except::FileSystemException(name.toString(), afl::sys::Error(ai));
        }

        // Make a socket
        SOCKET sock = INVALID_SOCKET;
        DWORD errorCode = 0;
        for (struct addrinfo* p = result; p != 0; p = p->ai_next) {
            sock = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if (sock != INVALID_SOCKET) {
                // Check whether this socket is good for us.
                errorCode = f.process(sock, p->ai_addr, p->ai_addrlen);
                if (errorCode == 0) {
                    break;
                }

                // Nope.
                ::closesocket(sock);
                sock = INVALID_SOCKET;
            }
        }

        // Clean up
        if (result != 0) {
            m_freeAddrInfo(result);
        }

        // Anything found?
        if (sock == INVALID_SOCKET) {
            if (errorCode == 0) {
                throw afl::except::FileProblemException(name.toString(), afl::string::Messages::networkNameNotFound());
            } else {
                throw afl::except::FileSystemException(name.toString(), afl::sys::Error(errorCode));
            }
        }
        return sock;
    }

    // Get current Resolver instance.
    Resolver& getResolver()
    {
        // Mutex to make sure only one thread attempts to set the pResolver variable.
        // That mutex is also used later on for the LegacyResolver, if required.
        static afl::sys::Mutex mtx;
        afl::sys::MutexGuard g(mtx);

        // Check for have getaddrinfo/freeaddrinfo in Winsock DLL
        static Resolver* pResolver = 0;
        if (pResolver == 0) {
            HMODULE ws2DLL = LoadLibrary("ws2_32.dll");
            getaddrinfo_t gai = ws2DLL ? (getaddrinfo_t) GetProcAddress(ws2DLL, "getaddrinfo") : 0;
            freeaddrinfo_t fai = ws2DLL ? (freeaddrinfo_t) GetProcAddress(ws2DLL, "freeaddrinfo") : 0;
            if (gai != 0 && fai != 0) {
                static ModernResolver mc(gai, fai);
                pResolver = &mc;
            } else {
                static LegacyResolver lc(mtx);
                pResolver = &lc;
            }
        }

        return *pResolver;
    }

    /***************************** SocketFactory *****************************/

    inline SocketFactory::~SocketFactory()
    { }

    SOCKET SocketFactory::toName(const afl::net::Name& name)
    {
        // Reject names with null bytes
        if (name.getName().find('\0') != String_t::npos || name.getService().find('\0') != String_t::npos) {
            throw afl::except::FileProblemException(name.toString(), afl::string::Messages::networkNameNotFound());
        }

        return getResolver().toName(*this, name);
    }

    /******************************** Connect ********************************/

    inline Connect::Connect(afl::sys::Timeout_t timeout)
        : SocketFactory(),
          m_timeout(timeout)
    { }

    DWORD Connect::process(SOCKET sock, const sockaddr* addr, socklen_t len)
    {
        // Make socket non-blocking
        unsigned long one = 1;
        ::ioctlsocket(sock, FIONBIO, &one);

        // Initiate connection
        if (::connect(sock, addr, len) == 0) {
            return 0;
        }

        // Handle asynchronous case
        DWORD err = GetLastError();
        if (err == WSAEWOULDBLOCK) {
            if (waitReady(sock, m_timeout, WaitForConnect)) {
                // Connect finished. Did it succeed?
                int status = 424242;
                int length = sizeof(status);
                if (::getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*) &status, &length) == 0 && status == 0) {
                    // Success
                    return 0;
                } else {
                    // Error
                    return status;
                }
            } else {
                // Timeout
                return WSAETIMEDOUT;
            }
        } else {
            // Error
            return err;
        }
    }

    /********************************** Bind *********************************/

    DWORD Bind::process(SOCKET sock, const sockaddr* addr, socklen_t len)
    {
        BOOL one = 1;
        ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*) &one, sizeof(one));
        if (::bind(sock, addr, len) == 0) {
            return 0;
        } else {
            return GetLastError();
        }
    }

    /******************************* waitReady *******************************/

    bool waitReady(SOCKET fd, afl::sys::Timeout_t timeout, WaitMode mode)
    {
        fd_set set, set2;
        FD_ZERO(&set);
        FD_ZERO(&set2);
        FD_SET(fd, &set);
        FD_SET(fd, &set2);

        // Prepare timeout
        struct timeval tv;
        struct timeval* ptv;
        if (timeout == afl::sys::INFINITE_TIMEOUT) {
            ptv = 0;
        } else {
            tv.tv_sec = timeout / 1000;
            tv.tv_usec = (timeout % 1000) * 1000;
            ptv = &tv;
        }

        // System call
        int result = 0;
        switch (mode) {
         case WaitForAccept:
            // accept: reports success if it becomes readable
            result = ::select(fd + 1, &set, 0, 0, ptv);
            break;
        case WaitForConnect:
            // connect: reports success by becoming writable, error by becoming exceptional
            result = ::select(fd + 1, 0, &set, &set2, ptv);
            break;
        }

        // Return
        if (result > 0 && FD_ISSET(fd, &set)) {
            return true;
        } else {
            ::SetLastError(WSAETIMEDOUT);
            return false;
        }
    }
}

/*********************** Win32NetworkStack Classes ***********************/

/** Implementation of afl::net::Listener for Win32. */
class arch::win32::Win32NetworkStack::Listener : public afl::net::Listener, private WaitRequest {
 public:
    Listener(SOCKET fd, const String_t& name);
    ~Listener();
    virtual afl::base::Ptr<afl::net::Socket> accept(afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);
    virtual void acceptAsync(afl::async::Controller& ctl, afl::net::AcceptOperation& op);
    virtual void cancel(afl::async::Controller& ctl, afl::async::Operation& op);

 private:
    const SOCKET m_fd;
    const HANDLE m_event;
    const String_t m_name;

    // Async stuff
    afl::sys::Mutex m_mutex;
    afl::async::OperationList<afl::net::AcceptOperation> m_pendingAccepts;

    // WaitRequest:
    virtual bool handleWaitReady();
};

/** Implementation of afl::net::Socket for Win32. */
class arch::win32::Win32NetworkStack::Socket : public afl::net::Socket, private WaitRequest {
 public:
    typedef afl::async::Controller Controller;
    typedef afl::async::Operation Operation;
    typedef afl::async::SendOperation SendOperation;
    typedef afl::async::ReceiveOperation ReceiveOperation;

    Socket(SOCKET fd, const String_t& name);
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

    // WaitRequest:
    virtual bool handleWaitReady();

 private:
    const SOCKET m_fd;
    const HANDLE m_event;
    const String_t m_name;
    afl::sys::Mutex m_mutex;
    afl::async::OperationList<SendOperation> m_pendingSends;
    afl::async::OperationList<ReceiveOperation> m_pendingReceives;
};

/******************************** Listener *******************************/

inline
arch::win32::Win32NetworkStack::Listener::Listener(SOCKET fd, const String_t& name)
    : m_fd(fd),
      m_event(CreateEvent(0, FALSE, FALSE, 0)),
      m_name(name),
      m_mutex(),
      m_pendingAccepts()
{
    WSAEventSelect(m_fd, m_event, FD_ACCEPT);
}

arch::win32::Win32NetworkStack::Listener::~Listener()
{
    ::closesocket(m_fd);
    CloseHandle(m_event);
}

afl::base::Ptr<afl::net::Socket>
arch::win32::Win32NetworkStack::Listener::accept(afl::sys::Timeout_t timeout)
{
    if (waitReady(m_fd, timeout, WaitForAccept)) {
        // Accept a socket
        SOCKET data = ::accept(m_fd, 0, 0);
        if (data != INVALID_SOCKET) {
            // Make socket non-blocking
            unsigned long one = 1;
            ::ioctlsocket(data, FIONBIO, &one);
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
arch::win32::Win32NetworkStack::Listener::acceptAsync(afl::async::Controller& ctl, afl::net::AcceptOperation& op)
{
    // Initialize
    op.setController(&ctl);

    // Try to accept right now.
    SOCKET data = ::accept(m_fd, 0, 0);
    if (data != INVALID_SOCKET) {
        // Make socket non-blocking
        unsigned long one = 1;
        ::ioctlsocket(data, FIONBIO, &one);
        op.setResult(new Socket(data, m_name));
        op.getNotifier().notifyDirect(op);
    } else {
        // No socket available immediately. Wait.
        afl::sys::MutexGuard g(m_mutex);
        m_pendingAccepts.pushBack(&op);
        ctl.getImplementation().addRequest(*this, m_event);
    }
}

void
arch::win32::Win32NetworkStack::Listener::cancel(afl::async::Controller& ctl, afl::async::Operation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    m_pendingAccepts.remove(&op);
    // FIXME: ctl.getImplementation().removeRequest(*this, m_fd);
    ctl.revertPost(op);
}

bool
arch::win32::Win32NetworkStack::Listener::handleWaitReady()
{
    // Try to satisfy accepts
    afl::sys::MutexGuard g(m_mutex);
    while (afl::net::AcceptOperation* op = m_pendingAccepts.front()) {
        SOCKET data = ::accept(m_fd, 0, 0);

        // We cannot report errors, so any error means stop, not just EWOULDBLOCK.
        if (data == INVALID_SOCKET) {
            break;
        }

        // Satisfy this read
        unsigned long one = 1;
        ::ioctlsocket(data, FIONBIO, &one);
        op->setResult(new Socket(data, m_name));
        op->getNotifier().notify(*op);
        m_pendingAccepts.extractFront();
    }

    return m_pendingAccepts.empty();
}

/********************************* Socket ********************************/

inline
arch::win32::Win32NetworkStack::Socket::Socket(SOCKET fd, const String_t& name)
    : m_fd(fd),
      m_event(CreateEvent(0, FALSE, FALSE, 0)),
      m_name(name),
      m_mutex(),
      m_pendingSends(),
      m_pendingReceives()
{
    WSAEventSelect(m_fd, m_event, FD_READ | FD_WRITE | FD_CLOSE);
}

arch::win32::Win32NetworkStack::Socket::~Socket()
{
    ::closesocket(m_fd);
    CloseHandle(m_event);
}

bool
arch::win32::Win32NetworkStack::Socket::send(Controller& ctl, SendOperation& op, afl::sys::Timeout_t timeout)
{
    sendAsync(ctl, op);
    if (!ctl.wait(op, timeout)) {
        cancel(ctl, op);
        return false;
    } else {
        return true;
    }
}

void
arch::win32::Win32NetworkStack::Socket::sendAsync(Controller& ctl, SendOperation& op)
{
    op.setController(&ctl);
    afl::sys::MutexGuard g(m_mutex);

    // Try to send right now.
    afl::base::ConstBytes_t buf = op.getUnsentBytes();
    int n = buf.empty() ? 0 : ::send(m_fd, (const char*) buf.unsafeData(), buf.size(), 0);
    if (n >= 0) {
        // Success case
        op.addSentBytes(n);
        op.getNotifier().notifyDirect(op);
    } else if (WSAGetLastError() == WSAEWOULDBLOCK) {
        // Asynchronous case
        m_pendingSends.pushBack(&op);
        ctl.getImplementation().addRequest(*this, m_event);
    } else {
        // Error case
        op.getNotifier().notifyDirect(op);
    }
}

bool
arch::win32::Win32NetworkStack::Socket::receive(Controller& ctl, ReceiveOperation& op, afl::sys::Timeout_t timeout)
{
    receiveAsync(ctl, op);
    if (!ctl.wait(op, timeout)) {
        cancel(ctl, op);
        return false;
    } else {
        return true;
    }
}

void
arch::win32::Win32NetworkStack::Socket::receiveAsync(Controller& ctl, ReceiveOperation& op)
{
    op.setController(&ctl);
    afl::sys::MutexGuard g(m_mutex);

    // Try to receive right now.
    afl::base::Bytes_t buf = op.getUnreceivedBytes();
    int n = buf.empty() ? 0 : ::recv(m_fd, (char*) buf.unsafeData(), buf.size(), 0);
    if (n >= 0) {
        // Success case
        op.addReceivedBytes(n);
        op.getNotifier().notifyDirect(op);
    } else if (WSAGetLastError() == WSAEWOULDBLOCK) {
        // Asynchronous case
        m_pendingReceives.pushBack(&op);
        ctl.getImplementation().addRequest(*this, m_event);
    } else {
        // Error case
        op.getNotifier().notifyDirect(op);
    }
}

void
arch::win32::Win32NetworkStack::Socket::cancel(Controller& ctl, Operation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    m_pendingSends.remove(&op);
    m_pendingReceives.remove(&op);
    // FIXME: ctl.getImplementation().removeRequest(*this, m_fd);
    ctl.revertPost(op);
}

String_t
arch::win32::Win32NetworkStack::Socket::getName()
{
    return m_name;
}

void
arch::win32::Win32NetworkStack::Socket::closeSend()
{
    ::shutdown(m_fd, SD_SEND);
}

afl::net::Name
arch::win32::Win32NetworkStack::Socket::getPeerName()
{
    union {
        struct sockaddr_storage ss;
        struct sockaddr         sa;
        struct sockaddr_in      sin;
        struct sockaddr_in6     sin6;
    } ss;
    int slen = sizeof(ss);
    int n = ::getpeername(m_fd, &ss.sa, &slen);

    afl::net::Name result;
    if (n != SOCKET_ERROR) {
        // inet_ntop is not available until Vista. A close equivalent is WSAAddressToString,
        // whose main difference is that it includes the port number.
        // This ultimately means that it produces just what Name::parse expects.
        char nameBuf[100];        // IPv4 = 15 bytes + 6 bytes port, IPv6 = 40 bytes + 6 bytes port
        DWORD nameLen = sizeof(nameBuf);
        if (WSAAddressToString(&ss.sa, slen, 0, nameBuf, &nameLen) == 0) {
            // Remove zero-termination
            if (const char* p = static_cast<const char*>(std::memchr(nameBuf, '\0', nameLen))) {
                nameLen = p - nameBuf;
            }

            // Build result
            result = afl::net::Name::parse(String_t(nameBuf, nameLen), "");
        }
    }
    return result;
}

bool
arch::win32::Win32NetworkStack::Socket::handleWaitReady()
{
    // FIXME: mutex?
    // Try to satisfy reads
    while (afl::async::ReceiveOperation* op = m_pendingReceives.front()) {
        // Read some data
        afl::base::Bytes_t buf = op->getUnreceivedBytes();
        int n = buf.empty() ? 0 : ::recv(m_fd, (char*) buf.unsafeData(), buf.size(), 0);
        if (n > 0) {
            op->addReceivedBytes(n);
        }

        // Check results
        if (n == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK) {
            break;
        }

        // Finish partial read
        op->getNotifier().notify(*op);
        m_pendingReceives.extractFront();
    }

    // Try to satisfy writes
    while (afl::async::SendOperation* op = m_pendingSends.front()) {
        // Write some data
        afl::base::ConstBytes_t buf = op->getUnsentBytes();
        int n = buf.empty() ? 0 : ::send(m_fd, (const char*) buf.unsafeData(), buf.size(), 0);
        if (n > 0) {
            op->addSentBytes(n);
        }

        // Check results
        if (n == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK) {
            break;
        }

        // Finish partial send
        op->getNotifier().notify(*op);
        m_pendingSends.extractFront();
    }

    // Ready?
    return m_pendingReceives.empty() && m_pendingSends.empty();
}

/*************************** Win32NetworkStack ***************************/

arch::win32::Win32NetworkStack::Win32NetworkStack()
{
    // This is a good place to initialize Winsock.
    static struct Initializer {
        Initializer() {
            // We request version 2.2 because we want getaddrinfo().
            WORD wVersionRequested = MAKEWORD(2, 2);
            WSADATA wsaData;
            if (WSAStartup(wVersionRequested, &wsaData) != 0 || LOBYTE(wsaData.wVersion) < 2) {
                // No suitable Winsock2 DLL found, abort.
                // (We don't ever expect to see this.)
                WSACleanup();
                throw afl::except::SystemException(afl::sys::Error::current(), "<WSAStartup>");
            }
        }
    } init;
}

afl::base::Ref<afl::net::Listener>
arch::win32::Win32NetworkStack::listen(const afl::net::Name& name, int backlogSize)
{
    // Create listener socket
    SOCKET fd = Bind().toName(name);
    ::listen(fd, backlogSize);

    // Make socket non-blocking
    unsigned long one = 1;
    ::ioctlsocket(fd, FIONBIO, &one);

    return *new Listener(fd, name.toString());
}

afl::base::Ref<afl::net::Socket>
arch::win32::Win32NetworkStack::connect(const afl::net::Name& name, afl::sys::Timeout_t timeout)
{
    SOCKET fd = Connect(timeout).toName(name);
    return *new Socket(fd, name.toString());
}
#else
int g_variableToMakeWin32NetworkStackObjectFileNotEmpty;
#endif
