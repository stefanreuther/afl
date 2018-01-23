/**
  *  \file afl/net/url.hpp
  *  \brief Class afl::net::Url
  */
#ifndef AFL_AFL_NET_URL_HPP
#define AFL_AFL_NET_URL_HPP

#include "afl/string/string.hpp"
#include "afl/net/name.hpp"

namespace afl { namespace net {

    class HeaderConsumer;

    /** URLs.
        This class contains a URL and functions to work with it.

        Attributes:
        - scheme   (optional)
        - user     (optional, only allowed when host (and therefore path) is present)
        - password (optional, only allowed when user (and therefore path, host) is present)
        - host     (optional, only allowed when path is present)
        - port     (optional, only allowed when host (and therefore path) is present)
        - path     (mandatory)
        - fragment (optional)

        Possible syntactic forms:
        - scheme:path[#fragment]
        - scheme://host[:port][path][#fragment]
        - scheme://user[:password]@host[:port][path][#fragment]
        - path[#fragment]

        The path includes a possible query string. */
    class Url {
     public:
        /** Constructor. Makes a blank (null) URL. */
        Url();

        /** Destructor. */
        ~Url();

        /** Clear URL.
            Resets the URL to its virgin state, as if it has just been constructed. */
        void clear();

        /** Parse URL.
            \param url String to parse, in UTF-8 encoding
            \return true on success, false if the string cannot be parsed as a URL */
        bool parse(const String_t& url);

        /** Update from another URL.
            Resolves this URL in the context of another one.
            \param other other URL */
        void mergeFrom(const Url& other);

        /** Check for null URL.
            A null URL contains no information whatsoever.
            \return true iff this is a null URL. */
        bool isNull() const;

        /** Check for valid URL.
            A valid URL contains at least a scheme and a path.
            \return true if URL has correct format */
        bool isValid() const;

        /** Check for valid Id URL.
            A valid Id URL contains a scheme and a path, but no host name.
            This applies to "mailto:", "cid:" and similar URLs.
            \return true if URL has correct format */
        bool isValidId() const;

        /** Check for valid host-based URL.
            A valid host-based URL contains a scheme, host name, and path.
            This applies to "http:", "ftp:" and similar URLs.
            \return true if URL has correct format */
        bool isValidHost() const;

        /** Get host/port as a Name.
            Returns the host/port part as a Name,
            ready for use with NetworkStack::connect() or NetworkStack::listen().
            \return name */
        Name getName(const String_t& service) const;

        /** Get URL scheme.
            \return URL scheme, blank if no scheme given. */
        const String_t& getScheme() const;

        /** Get user name.
            \return User name, blank if no user name given. */
        const String_t& getUser() const;

        /** Get password.
            \return Password, blank if no password given. */
        const String_t& getPassword() const;

        /** Get host name.
            \return Host name, blank if no host name given. */
        const String_t& getHost() const;

        /** Get port number.
            \return Port number as a string, blank if no port number given. */
        const String_t& getPort() const;

        /** Get path name.
            Unlike the other attributes, a path name can possibly contain URL-encoded characters.
            (In the case of HTTP, those are passed to the server.)

            If the URL contains a host name, the path name starts with "/".
            If the URL does not contain a host name (for example, in a "mailto:" URL or a relative path name),
            no restrictions apply to the path.

            \return path name, blank if none given. */
        const String_t& getPath() const;

        /** Get fragment identifier.
            \return fragment identifier (starting with "#"), blank if no fragment identifier given. */
        const String_t& getFragment() const;

        /** Set host/port from a Name.
            \param name Host/port to set */
        void setName(const Name& name);

        /** Set URL scheme.
            \param scheme New URL scheme */
        void setScheme(const String_t& scheme);

        /** Set user name.
            \param user User name */
        void setUser(const String_t& user);

        /** Set password.
            \param pass Password */
        void setPassword(const String_t& pass);

        /** Set host name.
            \param host Host name */
        void setHost(const String_t& host);

        /** Set port number.
            \param port Port number as a string */
        void setPort(const String_t& port);

        /** Set path name.
            Unlike the other attributes, a path name can possibly contain URL-encoded characters.
            (In the case of HTTP, those are passed to the server.)

            If the URL contains a host name, the path name must start with "/".
            If the URL does not contain a host name (for example, in a "mailto:" URL or a relative path name),
            no restrictions apply to the path.

            \param path path name */
        void setPath(const String_t& path);

        /** Set fragment identifier.
            \param fragment fragment identifier (must start with "#"). */
        void setFragment(const String_t& fragment);

        /** Get URL in string form.
            \return string */
        String_t toString() const;
        
     private:
        /** URL scheme.
            Always in decoded form.
            Blank if the URL has no scheme given. */
        String_t m_scheme;

        /** User name.
            Always in decoded form.
            Blank if the URL has no user name given. */
        String_t m_user;

        /** Password.
            Always in decoded form.
            Blank if the URL has no user name given. */
        String_t m_password;

        /** Host name.
            Always in decoded form.
            Blank if the URL has no host name given. */
        String_t m_host;

        /** Port number.
            Always in decoded form.
            Blank if the URL has no port number given. */
        String_t m_port;

        /** Path name.
            Always in <b>encoded</b> form.
            May or may not start with a slash.
            For message-Id and email URLs, this is the actual Id or address.
            Blank if the URL has no path given. */
        String_t m_path;

        /** Fragment identifier.
            Always in decoded form.
            Includes the "#".
            Blank if the URL has no fragment given. */
        String_t m_fragment;
    };


    /** Check for path match.
        The path is considered matching if it is
        - equal to the prefix
        - starts with the prefix, followed by a suffix starting with "/" or "?"

        For example, using the prefix "/dir", the following paths match:
        - "/dir" (with empty suffix)
        - "/dir/subdir (with suffix "/subdir")
        - "/dir?query (with suffix "?query")
        - "/dir/?query (with suffix "/?query")
        but the following don't:
        - "/"
        - "/dir1"

        \param prefix [in] Expected path prefix
        \param suffix [out] Path suffix
        \retval true match succeeded; \c suffix has been set
        \retval false match failed */
    bool matchPath(const String_t& path, const String_t& prefix, String_t& suffix);

    /** Check for query strings.
        If the path contains a query string ("?key=value&key=value..."),
        those are removed from the string and fed into the HeaderConsumer.

        Keys that don't have a value ("?key1&key2") will be treated as if they had an empty value ("?key1=&key2=").

        \param path [in/out] path with optional query string
        \param consumer received parsed arguments */
    void matchArguments(String_t& path, HeaderConsumer& consumer);

} }





/*
 *  Implementation
 */


inline const String_t&
afl::net::Url::getScheme() const
{
    return m_scheme;
}

inline const String_t&
afl::net::Url::getUser() const
{
    return m_user;
}

inline const String_t&
afl::net::Url::getPassword() const
{
    return m_password;
}

inline const String_t&
afl::net::Url::getHost() const
{
    return m_host;
}

inline const String_t&
afl::net::Url::getPort() const
{
    return m_port;
}

inline const String_t&
afl::net::Url::getPath() const
{
    return m_path;
}

inline const String_t&
afl::net::Url::getFragment() const
{
    return m_fragment;
}

inline void
afl::net::Url::setScheme(const String_t& scheme)
{
    m_scheme = scheme;
}

inline void
afl::net::Url::setUser(const String_t& user)
{
    m_user = user;
}

inline void
afl::net::Url::setPassword(const String_t& pass)
{
    m_password = pass;
}

inline void
afl::net::Url::setHost(const String_t& host)
{
    m_host = host;
}

inline void
afl::net::Url::setPort(const String_t& port)
{
    m_port = port;
}

inline void
afl::net::Url::setPath(const String_t& path)
{
    m_path = path;
}

inline void
afl::net::Url::setFragment(const String_t& fragment)
{
    m_fragment = fragment;
}

#endif
