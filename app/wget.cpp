/**
  *  \file app/wget.cpp
  *  \brief Sample application: HTTP download
  *
  *  When invoked with a set of URLs on the command line,
  *  posts GET requests and logs the status for each.
  */

#include <iostream>
#include <cstring>
#include "afl/base/deleter.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/net/http/client.hpp"
#include "afl/net/http/clientconnection.hpp"
#include "afl/net/http/clientconnectionprovider.hpp"
#include "afl/net/http/clientresponse.hpp"
#include "afl/net/http/cookiejar.hpp"
#include "afl/net/http/defaultconnectionprovider.hpp"
#include "afl/net/http/downloadlistener.hpp"
#include "afl/net/http/manager.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/securenetworkstack.hpp"
#include "afl/net/socket.hpp"
#include "afl/net/tunnel/tunnelablenetworkstack.hpp"
#include "afl/net/url.hpp"
#include "afl/string/messages.hpp"
#include "afl/sys/environment.hpp"
#include "afl/sys/semaphore.hpp"
#include "afl/sys/thread.hpp"

namespace http = afl::net::http;

namespace {
    class MyRequest : public http::DownloadListener {
     public:
        MyRequest(afl::sys::Semaphore& sem, afl::net::Url url)
            : m_sem(sem),
              m_url(url)
            { }

        virtual ~MyRequest()
            {
                std::cout << m_url.toString() << ": done\n";
            }

        virtual void handleResponseHeader(http::ClientResponse& resp)
            {
                class HeaderPrinter : public afl::net::HeaderConsumer {
                 public:
                    void handleHeader(String_t key, String_t value)
                        {
                            std::cout << "  " << key << ": " << value << "\n";
                        }
                };
                std::cout << m_url.toString() << ": response header\n";
                HeaderPrinter p;
                resp.getResponseHeaders().enumerateHeaders(p);
                switch (resp.getResponseLimitKind()) {
                 case http::ClientResponse::ByteLimit:
                    std::cout << "  Byte limit: " << resp.getResponseLength() << "\n";
                    break;
                 case http::ClientResponse::ChunkLimit:
                    std::cout << "  Chunked encoding (implicit limit)\n";
                    break;
                 case http::ClientResponse::StreamLimit:
                    std::cout << "  Stream (no limit)\n";
                    break;
                }
                switch (resp.getResponseEncoding()) {
                 case http::ClientResponse::IdentityEncoding:
                    std::cout << "  Identity encoding\n";
                    break;
                 case http::ClientResponse::GzipEncoding:
                    std::cout << "  Gzip encoding\n";
                    break;
                 case http::ClientResponse::DeflateEncoding:
                    std::cout << "  Deflate encoding\n";
                    break;
                 case http::ClientResponse::UnknownEncoding:
                    std::cout << "  Unknown encoding\n";
                    break;
                }
            }

        virtual void handleResponseData(afl::io::Stream::FileSize_t pos, afl::base::ConstBytes_t data)
            {
                afl::string::ConstStringMemory_t sm =
                    afl::string::ConstStringMemory_t::unsafeCreate(reinterpret_cast<const char*>(data.unsafeData()), data.size());
                std::cout << m_url.toString()
                          << ": response data " << pos << "\n"
                          << "\033[33m" << afl::string::fromMemory(sm.subrange(0, 100))
                          << "\033[0m\n";
            }

        virtual void handleFailure(afl::net::http::ClientRequest::FailureReason /*reason*/, const String_t& msg)
            {
                std::cout << m_url.toString() << ": failure (" << msg << ")\n";
                m_sem.post();
            }

        virtual void handleSuccess()
            {
                std::cout << m_url.toString() << ": success\n";
                m_sem.post();
            }

     private:
        afl::sys::Semaphore& m_sem;
        afl::net::Url m_url;
    };
}

int main(int /*argc*/, char** argv)
{
    // Make a HTTP client and run it
    http::Client client;
    afl::sys::Thread clientThread("HTTP Client", client);

    // Proxy
    afl::net::tunnel::TunnelableNetworkStack net(afl::net::NetworkStack::getInstance());
    String_t proxy = afl::sys::Environment::getInstance(argv).getEnvironmentVariable("PROXY");
    if (!proxy.empty()) {
        if (!net.add(proxy)) {
            std::cerr << "invalid PROXY setting has been ignored\n";
        }
    }

    // Let's play it simple: if the first URL is a https URL, do SSL; otherwise don't.
    if (argv[1] != 0 && std::strncmp(argv[1], "https:", 6) == 0) {
        static afl::net::SecureNetworkStack snsInstance(net);
        client.setNewConnectionProvider(new afl::net::http::DefaultConnectionProvider(client, snsInstance, "https"));
    } else {
        client.setNewConnectionProvider(new afl::net::http::DefaultConnectionProvider(client, net, "http"));
    }
    clientThread.start();

    // Load cookies
    http::CookieJar jar;
    try {
        afl::base::Ref<afl::io::Stream> f = afl::io::FileSystem::getInstance().openFile("cookies.txt", afl::io::FileSystem::OpenRead);
        jar.loadCookies(*f);
    }
    catch (...) { }

    // Make a manager
    http::Manager mgr(client);
    mgr.setCookieJar(jar);

    // Wait for termination
    afl::sys::Semaphore sem(0);
    afl::base::Deleter deleter;

    // Post URLs to it
    size_t count = 0;
    while (const char* p = *++argv) {
        afl::net::Url url;
        if (url.parse(p) && url.isValidHost()) {
            mgr.getFile(url, deleter.addNew(new MyRequest(sem, url)));
            ++count;
        } else {
            std::cerr << "wget: cannot understand URL '" << p << "'\n";
        }
    }

    // Wait
    while (count > 0) {
        sem.wait();
        --count;
    }

    // Stop
    client.stop();
    clientThread.join();

    // Save cookies
    try {
        afl::base::Ref<afl::io::Stream> f = afl::io::FileSystem::getInstance().openFile("cookies.txt", afl::io::FileSystem::Create);
        jar.saveCookies(*f);
    }
    catch (...) {
        std::cerr << "wget: cannot save cookies.\n";
    }
}
