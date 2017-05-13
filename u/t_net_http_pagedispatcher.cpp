/**
  *  \file u/t_net_http_pagedispatcher.cpp
  *  \brief Test afl::net::http::PageDispatcher
  */

#include "afl/net/http/pagedispatcher.hpp"

#include <memory>
#include "u/t_net_http.hpp"
#include "afl/net/http/page.hpp"
#include "afl/net/http/pageresponse.hpp"
#include "afl/net/http/request.hpp"
#include "afl/net/http/response.hpp"

namespace {
    // Simple test page
    class MyPage : public afl::net::http::Page {
     public:
        MyPage(bool withPath, String_t result)
            : m_withPath(withPath),
              m_result(result)
            { }
        bool isValidMethod(const String_t& method) const
            { return method == "GET"; }
        virtual bool isValidPath() const
            { return m_withPath; }
        virtual void handleRequest(afl::net::http::PageRequest& /*in*/, afl::net::http::PageResponse& out)
            {
                afl::base::ConstBytes_t bytes(afl::string::toBytes(m_result));
                out.body().handleData(bytes);
                out.headers().set("Content-Type", "text/plain");
            }
     private:
        bool m_withPath;
        String_t m_result;
    };

    // Pull data out of a Response and store it in a string
    String_t pullData(afl::net::http::Response& resp)
    {
        String_t result;
        while (1) {
            afl::base::ConstBytes_t data = resp.getData();
            if (data.empty()) {
                break;
            }
            result += afl::string::fromBytes(data);
        }
        return result;
    }
}

/** Simple test.
    This is just a simple functionality test; actual testing happens when this thing hits a browser... */
void
TestNetHttpPageDispatcher::testIt()
{
    using afl::net::http::Request;
    using afl::net::http::Response;

    afl::net::http::PageDispatcher disp("/root");
    disp.addNewPage("/one", new MyPage(false, "one"));
    disp.addNewPage("/two", new MyPage(true, "two"));
    disp.addNewPage("/two/sub", new MyPage(true, "sub"));

    // "/one": is not within root
    {
        afl::base::ConstBytes_t bytes(afl::string::toBytes("GET /one HTTP/1.0\r\n\r\n"));
        std::auto_ptr<Request> req(new Request());
        req->handleData(bytes);
        std::auto_ptr<Response> resp(disp.createNewResponse(req));
        TS_ASSERT(resp.get() == 0);
    }

    // "/root/zero": within root, but nonexistant
    {
        afl::base::ConstBytes_t bytes(afl::string::toBytes("GET /root/zero HTTP/1.0\r\n\r\n"));
        std::auto_ptr<Request> req(new Request());
        req->handleData(bytes);
        std::auto_ptr<Response> resp(disp.createNewResponse(req));
        TS_ASSERT(resp.get() != 0);
        resp->handleData(bytes);
        resp->handleDataComplete();
        String_t result = pullData(*resp);
        TS_ASSERT_EQUALS(result.substr(0, 24), "HTTP/1.0 404 Not Found\r\n");
    }

    // "/root/one": within root, exists
    {
        afl::base::ConstBytes_t bytes(afl::string::toBytes("GET /root/one HTTP/1.0\r\n\r\n"));
        std::auto_ptr<Request> req(new Request());
        req->handleData(bytes);
        std::auto_ptr<Response> resp(disp.createNewResponse(req));
        TS_ASSERT(resp.get() != 0);
        resp->handleData(bytes);
        resp->handleDataComplete();
        String_t result = pullData(*resp);
        TS_ASSERT_EQUALS(result.substr(0, 17), "HTTP/1.0 200 OK\r\n");
        TS_ASSERT(result.find("one") != String_t::npos);
    }

    // "/root/one/x": within root, does not exist
    {
        afl::base::ConstBytes_t bytes(afl::string::toBytes("GET /root/one/x HTTP/1.0\r\n\r\n"));
        std::auto_ptr<Request> req(new Request());
        req->handleData(bytes);
        std::auto_ptr<Response> resp(disp.createNewResponse(req));
        TS_ASSERT(resp.get() != 0);
        resp->handleData(bytes);
        resp->handleDataComplete();
        String_t result = pullData(*resp);
        TS_ASSERT_EQUALS(result.substr(0, 24), "HTTP/1.0 404 Not Found\r\n");
    }

    // "/root/one": bad method
    {
        afl::base::ConstBytes_t bytes(afl::string::toBytes("OPTIONS /root/one HTTP/1.0\r\n\r\n"));
        std::auto_ptr<Request> req(new Request());
        req->handleData(bytes);
        std::auto_ptr<Response> resp(disp.createNewResponse(req));
        TS_ASSERT(resp.get() != 0);
        resp->handleData(bytes);
        resp->handleDataComplete();
        String_t result = pullData(*resp);
        TS_ASSERT_EQUALS(result.substr(0, 19), "HTTP/1.0 405 Method");
    }

    // "/root/two": within root, exists
    {
        afl::base::ConstBytes_t bytes(afl::string::toBytes("GET /root/two HTTP/1.0\r\n\r\n"));
        std::auto_ptr<Request> req(new Request());
        req->handleData(bytes);
        std::auto_ptr<Response> resp(disp.createNewResponse(req));
        TS_ASSERT(resp.get() != 0);
        resp->handleData(bytes);
        resp->handleDataComplete();
        String_t result = pullData(*resp);
        TS_ASSERT_EQUALS(result.substr(0, 17), "HTTP/1.0 200 OK\r\n");
        TS_ASSERT(result.find("two") != String_t::npos);
    }

    // "/root/two/sub": within root, exists
    {
        afl::base::ConstBytes_t bytes(afl::string::toBytes("GET /root/two/sub HTTP/1.0\r\n\r\n"));
        std::auto_ptr<Request> req(new Request());
        req->handleData(bytes);
        std::auto_ptr<Response> resp(disp.createNewResponse(req));
        TS_ASSERT(resp.get() != 0);
        resp->handleData(bytes);
        resp->handleDataComplete();
        String_t result = pullData(*resp);
        TS_ASSERT_EQUALS(result.substr(0, 17), "HTTP/1.0 200 OK\r\n");
        TS_ASSERT(result.find("sub") != String_t::npos);
    }

    // "/root/two/a": within root, exists
    {
        afl::base::ConstBytes_t bytes(afl::string::toBytes("GET /root/two/a HTTP/1.0\r\n\r\n"));
        std::auto_ptr<Request> req(new Request());
        req->handleData(bytes);
        std::auto_ptr<Response> resp(disp.createNewResponse(req));
        TS_ASSERT(resp.get() != 0);
        resp->handleData(bytes);
        resp->handleDataComplete();
        String_t result = pullData(*resp);
        TS_ASSERT_EQUALS(result.substr(0, 17), "HTTP/1.0 200 OK\r\n");
        TS_ASSERT(result.find("two") != String_t::npos);
    }

    // "/root/two/z": within root, exists
    {
        afl::base::ConstBytes_t bytes(afl::string::toBytes("GET /root/two/z HTTP/1.0\r\n\r\n"));
        std::auto_ptr<Request> req(new Request());
        req->handleData(bytes);
        std::auto_ptr<Response> resp(disp.createNewResponse(req));
        TS_ASSERT(resp.get() != 0);
        resp->handleData(bytes);
        resp->handleDataComplete();
        String_t result = pullData(*resp);
        TS_ASSERT_EQUALS(result.substr(0, 17), "HTTP/1.0 200 OK\r\n");
        TS_ASSERT(result.find("two") != String_t::npos);
    }

    // "/root/two/z": within root, exists
    {
        afl::base::ConstBytes_t bytes(afl::string::toBytes("HEAD /root/two/z HTTP/1.0\r\n\r\n"));
        std::auto_ptr<Request> req(new Request());
        req->handleData(bytes);
        std::auto_ptr<Response> resp(disp.createNewResponse(req));
        TS_ASSERT(resp.get() != 0);
        resp->handleData(bytes);
        resp->handleDataComplete();
        String_t result = pullData(*resp);
        TS_ASSERT_EQUALS(result.substr(0, 17), "HTTP/1.0 200 OK\r\n");
        TS_ASSERT(result.find("two") == String_t::npos);
        TS_ASSERT(result.find("sub") == String_t::npos);
    }
}
