/**
  *  \file afl/net/http/cgihandler.cpp
  *  \brief Class afl::net::http::CGIHandler
  */

#include "afl/net/http/cgihandler.hpp"
#include "afl/base/countof.hpp"
#include "afl/base/inlinememory.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/bufferedstream.hpp"
#include "afl/net/http/page.hpp"
#include "afl/net/http/pagerequest.hpp"
#include "afl/net/http/pageresponse.hpp"
#include "afl/net/url.hpp"
#include "afl/string/format.hpp"
#include "afl/string/parse.hpp"

using afl::string::Format;
using afl::string::toBytes;

afl::net::http::CGIHandler::CGIHandler(afl::sys::Environment& env)
    : m_environment(env)
{ }

afl::net::http::CGIHandler::~CGIHandler()
{ }

int
afl::net::http::CGIHandler::handlePage(Page& page)
{
    try {
        // Build request
        String_t requestMethod = m_environment.getEnvironmentVariable("REQUEST_METHOD");
        String_t pathInfo      = m_environment.getEnvironmentVariable("PATH_INFO");
        PageRequest in("", m_environment.getEnvironmentVariable("SCRIPT_NAME"), pathInfo);
        PageResponse out;

        // Check how far we can parse
        bool wantUser;
        if (!page.isValidMethod(requestMethod)) {
            if (requestMethod == "HEAD" && page.isValidMethod("GET")) {
                // They only speak GET, but we want HEAD. Emulate it.
                in.setMethod("GET");
                wantUser = true;
            } else {
                // Generate "bad method"
                out.setStatusCode(PageResponse::METHOD_NOT_ALLOWED);
                out.finish();
                in.setIgnoreData();
                wantUser = false;
            }
        } else if (!in.getPath().empty() && !page.isValidPath()) {
            // Generate "404" (or redirect?)
            out.setStatusCode(PageResponse::NOT_FOUND);
            out.finish();
            in.setIgnoreData();
            wantUser = false;
        } else {
            // We cannot prove at this point that it's invalid, so let them process it
            wantUser = true;
        }

        // Do it
        buildPageRequest(in);

        // Process the request, building a response
        if (wantUser) {
            page.handleRequest(in, out);
        }
        out.finish();

        // Send output
        afl::base::Ref<afl::io::Stream> rawOutput = m_environment.attachStream(afl::sys::Environment::Output);
        afl::io::BufferedStream bufferedOutput(*rawOutput);
        if (requestMethod == "HEAD") {
            // Headers requested: if page did not provide a Content-Length, provide one.
            if (out.headers().get("Content-Length") == 0) {
                out.headers().add("Content-Length", Format("%d", out.body().getContent().size()));
            }
            sendHeaders(out, bufferedOutput);
        } else {
            // Normal case
            sendHeaders(out, bufferedOutput);
            bufferedOutput.fullWrite(out.body().getContent());
        }
        bufferedOutput.flush();
    }
    catch (std::exception& e) {
        // Write emergency page
        String_t result = "Status: 500 Internal Server Error\r\n"
            "Content-Type: text/plain\r\n\r\n";
        result += "Exception text: ";
        result += e.what();
        result += "\r\n";
        if (afl::except::FileProblemException* fpe = dynamic_cast<afl::except::FileProblemException*>(&e)) {
            result += "File name: ";
            result += fpe->getFileName();
            result += "\r\n";
        }
        
        m_environment.attachStream(afl::sys::Environment::Output)->fullWrite(toBytes(result));
    }
    return 0;
}

void
afl::net::http::CGIHandler::buildPageRequest(PageRequest& req)
{
    // Populate headers
    static const char*const HEADER_MAP[][2] = {
        { "CONTENT_TYPE",         "Content-Type" },
        { "HTTP_ACCEPT",          "Accept" },
        { "HTTP_ACCEPT_ENCODING", "Accept-Encoding" },
        { "HTTP_ACCEPT_LANGUAGE", "Accept-Language" },
        { "HTTP_CONNECTION",      "Connection" },
        { "HTTP_COOKIE",          "Cookie" },
        { "HTTP_HOST",            "Host" },
        { "HTTP_REFERER",         "Referer" },
        { "HTTP_USER_AGENT",      "User-Agent" },
    };
    for (size_t i = 0; i < countof(HEADER_MAP); ++i) {
        String_t value = m_environment.getEnvironmentVariable(HEADER_MAP[i][0]);
        if (!value.empty()) {
            req.headers().add(HEADER_MAP[i][1], value);
        }
    }

    // Populate query string
    String_t query = m_environment.getEnvironmentVariable("QUERY_STRING");
    if (!query.empty()) {
        query.insert(0, "?");
        matchArguments(query, req.arguments());
    }

    // Process method
    String_t method = m_environment.getEnvironmentVariable("REQUEST_METHOD");
    if (!method.empty()) {
        req.setMethod(method);
    }
    
    // Request body
    if (method == "POST") {
        size_t size = 0;
        if (afl::string::strToInteger(m_environment.getEnvironmentVariable("CONTENT_LENGTH"), size)) {
            // We have a recognized POST body.
            // Push data from standard input into the PageRequest.
            // PageRequest will parse it.
            afl::base::Ref<afl::io::Stream> in = m_environment.attachStream(afl::sys::Environment::Input);
            while (size > 0) {
                afl::base::InlineMemory<uint8_t,4096> bytes;
                bytes.trim(size);
                in->fullRead(bytes);
                req.handleData(bytes);
            }
        }
    }
}

void
afl::net::http::CGIHandler::sendHeaders(PageResponse& resp, afl::io::Stream& out)
{
    // Status code
    int statusCode = resp.getStatusCode();
    if (statusCode != PageResponse::OK) {
        out.fullWrite(toBytes(Format("Status: %d %s\r\n", statusCode, resp.getStatusText())));
    }

    // User headers
    resp.headers().writeHeaders(out);

    // Final newline
    out.fullWrite(toBytes("\r\n"));
}
