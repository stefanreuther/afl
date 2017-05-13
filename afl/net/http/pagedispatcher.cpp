/**
  *  \file afl/net/http/pagedispatcher.cpp
  *  \brief Class afl::net::http::PageDispatcher
  */

#include "afl/net/http/pagedispatcher.hpp"
#include "afl/net/http/page.hpp"
#include "afl/net/http/request.hpp"
#include "afl/net/url.hpp"
#include "afl/net/http/pageresponse.hpp"
#include "afl/net/http/pagerequest.hpp"
#include "afl/net/http/response.hpp"
#include "afl/io/internalsink.hpp"
#include "afl/string/format.hpp"

class afl::net::http::PageDispatcher::Handler : public afl::net::http::Response {
 public:
    Handler(Page* page, std::auto_ptr<Request> request, String_t root, String_t self, String_t path);

    virtual bool handleData(afl::base::ConstBytes_t& data);
    virtual void handleDataComplete();
    virtual bool isKeepalive();
    virtual afl::base::ConstBytes_t getData();

 private:
    void init();

    Page*const m_page;
    std::auto_ptr<Request> m_request;
    PageRequest m_pageRequest;
    PageResponse m_pageResponse;
    afl::io::InternalSink m_headerSink;

    enum OutputStatus {
        Pending,                // output not yet prepared
        Processed,              // output prepared, send headers next
        SendBody,               // headers sent, send body next
        Done                    // body send, finish the request
    };
    OutputStatus m_outputStatus;
};

/************************ PageDispatcher::Handler ************************/

afl::net::http::PageDispatcher::Handler::Handler(Page* page, std::auto_ptr<Request> request, String_t root, String_t self, String_t path)
    : Response(),
      m_page(page),
      m_request(request),
      m_pageRequest(root, self, path),
      m_pageResponse(),
      m_headerSink(),
      m_outputStatus(Pending)
{
    init();
}

void
afl::net::http::PageDispatcher::Handler::init()
{
    // Configure the request
    m_pageRequest.setMethod(m_request->getMethod());
    m_pageRequest.headers() = m_request->headers();        // FIXME: swap instead of copy? Must be careful with keepalive!

    // Process request
    if (m_page != 0) {
        // Process it
        if (!m_page->isValidMethod(m_request->getMethod())) {
            if (m_request->getMethod() == "HEAD" && m_page->isValidMethod("GET")) {
                // They only speak GET, but we want HEAD. Emulate it.
                m_pageRequest.setMethod("GET");
                m_outputStatus = Pending;
            } else {
                // Generate "bad method"
                m_pageResponse.setStatusCode(PageResponse::METHOD_NOT_ALLOWED);
                m_pageResponse.finish();
                m_pageRequest.setIgnoreData();
                m_outputStatus = Processed;
            }
        } else if (!m_pageRequest.getPath().empty() && !m_page->isValidPath()) {
            // Generate "404" (or redirect?)
            m_pageResponse.setStatusCode(PageResponse::NOT_FOUND);
            m_pageResponse.finish();
            m_pageRequest.setIgnoreData();
            m_outputStatus = Processed;
        } else {
            // We cannot prove at this point that it's invalid, so let them process it
            m_outputStatus = Pending;
        }
    } else {
        // Error handling
        // Generate "404".
        m_pageResponse.setStatusCode(PageResponse::NOT_FOUND);
        m_pageResponse.finish();
        m_pageRequest.setIgnoreData();
        m_outputStatus = Processed;
    }
}

bool
afl::net::http::PageDispatcher::Handler::handleData(afl::base::ConstBytes_t& data)
{
    m_pageRequest.handleData(data);
    return false;
}

void
afl::net::http::PageDispatcher::Handler::handleDataComplete()
{
    if (m_outputStatus == Pending) {
        m_pageRequest.finish();
        if (m_page != 0) {
            // Process it.
            m_page->handleRequest(m_pageRequest, m_pageResponse);
        } else {
            // Cannot happen. Make an Internal Server Error just in case.
            m_pageResponse.setStatusCode(PageResponse::INTERNAL_SERVER_ERROR);
        }
        m_pageResponse.finish();
        m_outputStatus = Processed;
    }
}

bool
afl::net::http::PageDispatcher::Handler::isKeepalive()
{
    return true;
}

afl::base::ConstBytes_t
afl::net::http::PageDispatcher::Handler::getData()
{
    afl::base::ConstBytes_t result;
    if (m_outputStatus == Processed) {
        // This means I have to send the headers, if any
        m_outputStatus = SendBody;
        if (m_request->isResponseHeaderRequested()) {
            // Build status line
            String_t statusLine(afl::string::Format("%s %d %s\r\n", m_request->getVersion(), m_pageResponse.getStatusCode(), m_pageResponse.getStatusText()));
            afl::base::ConstBytes_t bytes = afl::string::toBytes(statusLine);
            m_headerSink.handleData(bytes);

            // Add headers
            m_pageResponse.headers().writeHeaders(m_headerSink);

            // Add delimiter
            static const uint8_t CRLF[] = { 13, 10 };
            bytes = CRLF;
            m_headerSink.handleData(bytes);

            // Produce result
            result = m_headerSink.getContent();
            if (!result.empty()) {
                return result;
            }
        }
    }

    if (m_outputStatus == SendBody) {
        // Produce body, if any
        m_outputStatus = Done;
        if (m_request->isResponseBodyRequested()) {
            result = m_pageResponse.body().getContent();
            if (!result.empty()) {
                return result;
            }
        }
    }

    // We are done; produce (empty) result
    return result;
}

/***************************** PageDispatcher ****************************/

afl::net::http::PageDispatcher::PageDispatcher(const String_t& prefix)
    : Dispatcher(),
      m_prefix(prefix),
      m_pages(),
      m_handleNonexistant(true)
{ }

afl::net::http::PageDispatcher::~PageDispatcher()
{ }

void
afl::net::http::PageDispatcher::addNewPage(const char* path, Page* page)
{
    std::auto_ptr<Page> p(page);
    String_t s(path);
    m_pages.insertNew(s, p.release());
}

void
afl::net::http::PageDispatcher::setHandleNonexistant(bool flag)
{
    m_handleNonexistant = flag;
}

// Dispatcher:
afl::net::http::Response*
afl::net::http::PageDispatcher::createNewResponse(std::auto_ptr<Request>& request)
{
    String_t selfPath;
    if (request->matchPath(m_prefix, selfPath)) {
        // Matches our prefix. Find a matching Page.
        // Since pages are sorted lexically, the longest match is at the end. Therefore, search backward.
        String_t self;
        String_t path;
        Page* page = 0;
        for (afl::container::PtrMap<String_t, Page>::iterator it = m_pages.end(); it != m_pages.begin(); /* advance done in loop */) {
            --it;
            if (matchPath(selfPath, it->first, path)) {
                self = it->first;
                page = it->second;
                break;
            }
        }

        // If we didn't find anything, and do not want to handle file-not-found internally, reject.
        if (page == 0 && !m_handleNonexistant) {
            return 0;
        }

        // At this point we have decided that we will handle the error, so we can loot the request and build or own.
        return new Handler(page, request, m_prefix, self, path);
    } else {
        // Does not match our prefix, reject.
        return 0;
    }
}
