/**
  *  \file afl/net/headertable.hpp
  *  \brief Class afl::net::HeaderTable
  */
#ifndef AFL_AFL_NET_HEADERTABLE_HPP
#define AFL_AFL_NET_HEADERTABLE_HPP

#include <vector>
#include "afl/net/headerconsumer.hpp"
#include "afl/net/headerfield.hpp"
#include "afl/io/datasink.hpp"

namespace afl { namespace net {

    /** Set of headers.
        This stores a list of headers.
        Headers are ordered and there can be duplicates. */
    class HeaderTable : public HeaderConsumer {
     public:
        /** Constructor. Makes a blank table. */
        HeaderTable();

        /** Destructor. */
        ~HeaderTable();

        /** Set header.
            If a header with the given name already exists, changes its value.
            Otherwise, adds a new one.
            \param name Header name
            \param value Header value */
        void set(const String_t& name, const String_t& value);

        /** Add header.
            Always adds a new header, even if one already exists.
            \param name Header name
            \param value Header value */
        void add(const String_t& name, const String_t& value);

        /** Add header.
            Always adds a new header, even if one already exists.
            \param field HeaderField containing name and value */
        void add(const HeaderField& field);

        /** Clear.
            Discards all content and invalidates all pointers. */
        void clear();

        /** Get header by name.
            If there are multiple headers of the same name, returns the first one.
            If there is none, returns null.
            \param name Name to find (case-insensitive)
            \return first matching HeaderField, null if none. Pointer is valid until next modification. */
        const HeaderField* get(const String_t& name) const;

        /** Get header by name.
            If there are multiple headers of the same name, returns the first one.
            If there is none, returns null.
            The return value is mutable, i.e. you can change the value.
            \param name Name to find (case-insensitive)
            \return first matching HeaderField, null if none. Pointer is valid until next modification. */
        HeaderField* get(const String_t& name);

        /** Enumerate headers.
            Calls HeaderConsumer::handleHeader for all headers.
            \param consumer Consumer */
        void enumerateHeaders(HeaderConsumer& consumer) const;

        /** Write headers to a DataSink.
            Writes the headers in standard "key: value\r\n" format.
            \param sink Data sink to write to */
        void writeHeaders(afl::io::DataSink& sink) const;

        // HeaderConsumer:

        /** Add header.
            This method allows using a HeaderTable as a HeaderConsumer.
            It does the same as add(), i.e. add a new header.
            \param name Header name
            \param value Header value */
        void handleHeader(String_t name, String_t value);

     private:
        std::vector<HeaderField> m_headers;
    };

} }

#endif
