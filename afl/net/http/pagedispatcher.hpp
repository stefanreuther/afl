/**
  *  \file afl/net/http/pagedispatcher.hpp
  *  \brief Class afl::net::http::PageDispatcher
  */
#ifndef AFL_AFL_NET_HTTP_PAGEDISPATCHER_HPP
#define AFL_AFL_NET_HTTP_PAGEDISPATCHER_HPP

#include "afl/net/http/dispatcher.hpp"
#include "afl/container/ptrmap.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace net { namespace http {

    class Page;

    /** Dispatcher implementation using a set of Page's.
        This serves the pages unser a given prefix path.

        Handling of nonexistant paths:
        - outside prefix: produce null response (caller handles)
        - inside prefix: by default, produce a 404 response.
          A null response can be produced optionally (setHandleNonexistant(false)),

        \todo Implement nice error pages
        \todo Make a base class with just addNewPage() and the page selection logic */
    class PageDispatcher : public Dispatcher {
     public:
        /** Constructor.
            \param prefix [optional] Page prefix. If nonempty, must start with "/" and should not end with "/"
            (this way, a page's effective path is formed by concatenating the prefix and the page path). */
        PageDispatcher(const String_t& prefix = "");

        ~PageDispatcher();

        /** Add new page.
            \param path Page path. Must start with "/" and should not end with "/".
            \param page Page. PageDispatcher will take ownership of this page.

            Note that the page path is a C string.
            Typically, it is a literal anyway.
            This way, a call to "addNewPage("/url", new Page(...))" has just one call that can possibly fail and throw an exception. */
        void addNewPage(const char* path, Page* page);

        /** Configure handling of nonexistant pages within our tree.
            \param flag true: produce an internal 404 response. false: produce no response to let caller handle it */
        void setHandleNonexistant(bool flag);

        // Dispatcher:
        virtual Response* createNewResponse(std::auto_ptr<Request>& request);

     private:
        String_t m_prefix;
        afl::container::PtrMap<String_t, Page> m_pages;
        bool m_handleNonexistant;

        class Handler;
    };

} } }

#endif
