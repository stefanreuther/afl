/**
  *  \file app/httpserver.cpp
  *  \brief Sample application: HTTP server
  *
  *  Invoke as "httpserver host:port /dir=localDir..." to serve local files.
  *  "/dir=localDir" means that the content of local directory "localDir" is published as "/dir" on HTTP.
  *
  *  To terminate, kill the process.
  */

#include <map>
#include <iostream>
#include "afl/base/ptr.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/io/stream.hpp"
#include "afl/net/http/dispatcher.hpp"
#include "afl/net/http/protocolhandler.hpp"
#include "afl/net/http/request.hpp"
#include "afl/net/http/response.hpp"
#include "afl/net/name.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/protocolhandlerfactory.hpp"
#include "afl/net/simpleserver.hpp"
#include "afl/string/format.hpp"
#include "afl/string/string.hpp"
#include "afl/sys/environment.hpp"

namespace {
    /*
     *  Utility Functions
     */

    bool endsWith(const String_t& a, const String_t& end)
    {
        return a.size() >= end.size()
            && a.compare(a.size() - end.size(), end.size(), end) == 0;
    }

    String_t pickType(String_t path)
    {
        if (endsWith(path, ".html") || endsWith(path, ".htm")) {
            return "text/html";
        } else if (endsWith(path, ".txt")) {
            return "text/plain";
        } else if (endsWith(path, ".jpg")) {
            return "image/jpeg";
        } else if (endsWith(path, ".png")) {
            return "image/png";
        } else if (endsWith(path, ".gif")) {
            return "image/gif";
        } else {
            return "application/octet-stream";
        }
    }                          

    /*
     *  Sample Response implementation. This sends a file (Stream).
     */

    class MyResponse : public afl::net::http::Response {
     public:
        // Constructor.
        MyResponse(afl::base::Ptr<afl::io::Stream> stream,
                   String_t mimeType,
                   std::auto_ptr<afl::net::http::Request> request)
            : m_wantHeader(request->isResponseHeaderRequested()),
              m_didHeader(false),
              m_headerBuffer(),
              m_wantBody(request->isResponseBodyRequested()),
              m_stream(stream),
              m_bodySize(0)
            {
                // Build the header
                m_bodySize = stream->getSize();
                m_headerBuffer = afl::string::Format("%s 200 OK\r\n"
                                                     "Content-Type: %s\r\n"
                                                     "Content-Length: %d\r\n\r\n",
                                                     request->getVersion(),
                                                     mimeType,
                                                     m_bodySize);
            }

        // Handle data. We ignore everything.
        virtual bool handleData(const String_t& /*name*/, afl::base::ConstBytes_t& /*data*/)
            { return false; }

        // Data completion. We ignore everything.
        virtual void handleDataComplete()
            { }

        // We perform keepalive operations as told by the client.
        virtual bool isKeepalive()
            { return true; }

        // Get data. Here is the main logic.
        virtual afl::base::ConstBytes_t getData()
            {
                // Send header?
                if (m_wantHeader && !m_didHeader) {
                    m_didHeader = true;
                    return afl::string::toBytes(m_headerBuffer);
                }

                // Send body part?
                if (m_wantBody && m_bodySize > 0) {
                    afl::base::Bytes_t buffer(m_bodyBuffer);
                    if (m_bodySize < buffer.size()) {
                        buffer.trim(static_cast<size_t>(m_bodySize));
                    }
                    m_stream->fullRead(buffer);
                    m_bodySize -= buffer.size();
                    return buffer;
                }

                // Nothing found
                return afl::base::ConstBytes_t();
            }
     private:
        bool m_wantHeader;
        bool m_didHeader;
        String_t m_headerBuffer;

        bool m_wantBody;
        uint8_t m_bodyBuffer[4096];

        afl::base::Ptr<afl::io::Stream> m_stream;
        afl::io::Stream::FileSize_t m_bodySize;
    };

    /*
     *  Sample Dispatcher implementation.
     *  Sends files according to a path name mapping.
     */

    class MyDispatcher : public afl::net::http::Dispatcher {
     public:
        // Create new request. This checks the path mapping and creates a matching MyResponse if possible.
        virtual MyResponse* createNewResponse(std::auto_ptr<afl::net::http::Request>& request)
            {
                // Shortcuts
                const String_t& path = request->getPath();
                afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();

                // Check for matching path entry
                for (PathMap_t::iterator it = m_paths.begin(); it != m_paths.end(); ++it) {
                    const size_t len = it->first.size();
                    if (path.size() > len
                        && path.compare(0, len, it->first) == 0
                        && path[len] == '/'
                        && path.find('/', len+1) == String_t::npos)
                    {
                        try {
                            return new MyResponse(fs.openFile(fs.makePathName(it->second, path.substr(len+1)), fs.OpenRead),
                                                  pickType(path),
                                                  request);
                        }
                        catch (...) { }
                    }
                }

                // No match
                return 0;
            }

        // Add path mapping.
        void addPath(String_t serverPath, String_t fsPath)
            { m_paths[serverPath] = fsPath; }
     private:
        typedef std::map<String_t, String_t> PathMap_t;
        PathMap_t m_paths;
    };

    /*
     *  Sample ProtocolHandlerFactory implementation.
     */

    class MyPHFactory : public afl::net::ProtocolHandlerFactory {
     public:
        MyPHFactory(MyDispatcher& dispatcher)
            : m_dispatcher(dispatcher)
            { }

        virtual afl::net::ProtocolHandler* create()
            {
                return new afl::net::http::ProtocolHandler(m_dispatcher);
            }
     private:
        MyDispatcher& m_dispatcher;
    };
}

int main(int /*argc*/, char** argv)
{
    // Fetch command line
    afl::base::Ptr<afl::sys::Environment::CommandLine_t> cmdl =
        afl::sys::Environment::getInstance(argv).getCommandLine();

    // First element is network address
    String_t str;
    if (!cmdl->getNextElement(str)) {
        std::cout << "Missing network address.\n";
        return 1;
    }
    afl::net::Name name = afl::net::Name::parse(str, "80");

    // Build webspace from following
    MyDispatcher webspace;
    String_t ele;
    while (cmdl->getNextElement(ele)) {
        String_t::size_type n = ele.find('=');
        if (n == String_t::npos) {
            std::cout << "Missing '=' in path assignment.\n";
            return 1;
        }
        webspace.addPath(ele.substr(0, n), ele.substr(n+1));
    }

    // Start server
    MyPHFactory factory(webspace);
    afl::net::SimpleServer server(afl::net::NetworkStack::getInstance().listen(name, 10), factory);
    server.run();
}
