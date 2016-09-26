/**
  *  \file afl/net/http/manager.cpp
  *  \brief Class afl::net::http::Manager
  */

#include <sstream>
#include "afl/net/http/manager.hpp"
#include "afl/net/http/clientrequest.hpp"
#include "afl/io/stream.hpp"
#include "afl/net/http/clientresponse.hpp"
#include "afl/net/http/downloadlistener.hpp"
#include "afl/net/http/client.hpp"
#include "afl/io/inflatedatasink.hpp"
#include "afl/net/http/cookiejar.hpp"

namespace afl { namespace net { namespace http { namespace {

    // FIXME: for testability, it makes sense to move this class elsewhere when it's done.
    class ManagedGetRequest : public ClientRequest {
     public:
        ManagedGetRequest(const Url& url, DownloadListener& listener, Manager& manager)
            : m_url(url),
              m_listener(listener),
              m_manager(manager),
              m_headerSent(false),
              m_restarted(false),
              m_headers()
            { }

        virtual Name getName() const
            {
                return m_url.getName("80");
            }

        virtual String_t getScheme() const
            {
                return m_url.getScheme();
            }

        virtual bool isHeadRequest() const
            {
                return false;
            }

        virtual afl::base::ConstBytes_t getRequestData()
            {
                afl::base::ConstBytes_t result;
                if (!m_headerSent) {
                    m_headerSent = true;
                    m_headers = "GET ";
                    m_headers += m_url.getPath();
                    m_headers += " HTTP/1.1\r\n";
                    m_headers += m_manager.getHeaders(m_url);
                    m_headers += "\r\n";
                    result = afl::string::toBytes(m_headers);
                }
                return result;
            }

        virtual bool restart()
            {
                bool result = false;
                if (!m_restarted) {
                    m_restarted = true;
                    m_headerSent = false;
                    result = true;
                }
                return result;
            }

        virtual void handleResponseHeader(ClientResponse& resp)
            {
                // FIXME: if redirect, re-post.
                // FIXME: notify user of errors(?)
                m_manager.addCookies(m_url, resp.getResponseCookies());
                m_pos = resp.getResponseOffset();
                m_listener.handleResponseHeader(resp);
            }

        virtual void handleResponseData(afl::base::ConstBytes_t data)
            {
                m_listener.handleResponseData(m_pos, data);
                m_pos += data.size();
            }

        virtual void handleFailure(FailureReason reason, const String_t& message)
            {
                m_listener.handleFailure(reason, message);
            }

        virtual void handleSuccess()
            {
                m_listener.handleSuccess();
            }

     private:
        const Url m_url;
        DownloadListener& m_listener;
        Manager& m_manager;

        bool m_headerSent;
        bool m_restarted;
        String_t m_headers;

        afl::io::Stream::FileSize_t m_pos;
    };

    // FIXME: for testability, it makes sense to move this class elsewhere when it's done.
    class ManagedPostRequest : public ClientRequest {
     public:
        ManagedPostRequest(const Url& url, const String_t& data, const String_t& dataType,
                           DownloadListener& listener, Manager& manager)
            : m_url(url),
              m_data(data),
              m_dataType(dataType),
              m_listener(listener),
              m_manager(manager),
              m_headerSent(false),
              m_dataSent(false),
              m_restarted(false),
              m_headers()
            { }

        virtual Name getName() const
            {
                return m_url.getName("80");
            }

        virtual String_t getScheme() const
            {
                return m_url.getScheme();
            }

        virtual bool isHeadRequest() const
            {
                return false;
            }

        virtual afl::base::ConstBytes_t getRequestData()
            {
                afl::base::ConstBytes_t result;
                if (!m_headerSent) {
                    m_headerSent = true;
                    std::ostringstream ss;
                    ss << "POST " << m_url.getPath() << " HTTP/1.1\r\n"
                       << m_manager.getHeaders(m_url)
                       << "Content-Type: " << m_dataType << "\r\n"
                       << "Content-Length: " << m_data.size() << "\r\n"
                       << "\r\n";
                    m_headers = ss.str();
                    result = afl::string::toBytes(m_headers);
                }
                if (result.empty() && !m_dataSent) {
                    m_dataSent = true;
                    result = afl::string::toBytes(m_data);
                }
                return result;
            }

        virtual bool restart()
            {
                bool result = false;
                if (!m_restarted) {
                    m_restarted = true;
                    m_headerSent = false;
                    m_dataSent = false;
                    result = true;
                }
                return result;
            }

        virtual void handleResponseHeader(ClientResponse& resp)
            {
                // FIXME: if redirect, re-post.
                // FIXME: notify user of errors(?)
                m_manager.addCookies(m_url, resp.getResponseCookies());
                m_pos = resp.getResponseOffset();
                m_listener.handleResponseHeader(resp);
            }

        virtual void handleResponseData(afl::base::ConstBytes_t data)
            {
                m_listener.handleResponseData(m_pos, data);
                m_pos += data.size();
            }

        virtual void handleFailure(FailureReason reason, const String_t& message)
            {
                m_listener.handleFailure(reason, message);
            }

        virtual void handleSuccess()
            {
                m_listener.handleSuccess();
            }

     private:
        const Url m_url;
        const String_t m_data;
        const String_t m_dataType;
        DownloadListener& m_listener;
        Manager& m_manager;

        bool m_headerSent;
        bool m_dataSent;
        bool m_restarted;
        String_t m_headers;

        afl::io::Stream::FileSize_t m_pos;
    };

} } } }

afl::net::http::Manager::Manager(Client& client)
    : m_client(client),
      m_portTable(),
      m_pCookies(0)
{
    m_portTable.set("http", "80");
}

void
afl::net::http::Manager::setCookieJar(CookieJar& cookies)
{
    m_pCookies = &cookies;
}

uint32_t
afl::net::http::Manager::getFile(const Url& url, DownloadListener& listener)
{
    return m_client.addNewRequest(new ManagedGetRequest(normalizeUrl(url), listener, *this));
}

uint32_t
afl::net::http::Manager::postFile(const Url& url, String_t data, String_t dataType, DownloadListener& listener)
{
    return m_client.addNewRequest(new ManagedPostRequest(normalizeUrl(url), data, dataType, listener, *this));
}

void
afl::net::http::Manager::cancelRequest(uint32_t id)
{
    m_client.cancelRequest(id);
}

afl::net::http::Client&
afl::net::http::Manager::client()
{
    return m_client;
}

String_t
afl::net::http::Manager::getHeaders(const Url& url)
{
    String_t result;

    // Defaults
    result += "Host: ";
    result += url.getHost();
    result += ":";
    result += url.getPort();
    result += "\r\n";
    if (afl::io::InflateDataSink::isAvailable()) {
        result += "Accept-Encoding: gzip\r\n";
    }

    // Cookies
    if (m_pCookies != 0) {
        class CookieEnumerator : public HeaderConsumer {
         public:
            void handleHeader(String_t name, String_t value)
                {
                    if (!m_result.empty()) {
                        m_result += "; ";
                    }
                    m_result += name;
                    m_result += "=";
                    m_result += value;
                }
            String_t m_result;
        };
        CookieEnumerator e;
        m_pCookies->enumerateCookies(url, e);
        if (!e.m_result.empty()) {
            result += "Cookie: ";
            result += e.m_result;
            result += "\r\n";
        }
    }

    return result;
}

void
afl::net::http::Manager::addCookies(const Url& url, const HeaderTable& cookies)
{
    if (m_pCookies != 0) {
        m_pCookies->addCookies(url, cookies);
    }
}

afl::net::Url
afl::net::http::Manager::normalizeUrl(Url url)
{
    // Normalize scheme (case-insensitive; downstream-code expects lower-case)
    const String_t scheme = afl::string::strLCase(url.getScheme());
    url.setScheme(scheme);

    // If no port was given, derive one from the scheme if we can.
    if (url.getPort().empty()) {
        if (const HeaderField* f = m_portTable.get(scheme)) {
            url.setPort(f->getValue());
        }
    }

    // Fragments are not relevant here
    url.setFragment(String_t());

    return url;
}
