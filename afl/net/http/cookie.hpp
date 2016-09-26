/**
  *  \file afl/net/http/cookie.hpp
  *  \brief Class afl::net::http::Cookie
  */
#ifndef AFL_AFL_NET_HTTP_COOKIE_HPP
#define AFL_AFL_NET_HTTP_COOKIE_HPP

#include "afl/string/string.hpp"
#include "afl/base/types.hpp"
#include "afl/net/url.hpp"
#include "afl/sys/time.hpp"
#include "afl/net/headerfield.hpp"

namespace afl { namespace net { namespace http {

    /** HTTP Cookie.
        This represents a single cookie. */
    class Cookie {
     public:
        /** Default constructor.
            Makes an uninitialized, invalid cookie.
            \post !isValid() */
        Cookie();

        /** Construct from server cookie.
            \param url Request URL
            \param now Current time
            \param field Pre-parsed cookie as received from server (field name is cookie name,
            field value is cookie value and attributes); see http::ClientResponse::getResponseCookies(). */
        Cookie(const Url& url, afl::sys::Time now, const HeaderField& field);

        /** Destructor. */
        ~Cookie();

        /** Get sequence number.
            The sequence number defaults to 0 and can be set by a user, if so desired.
            \return sequence number */
        uint32_t getSequenceNumber() const;

        /** Set sequence number.
            \param sequenceNumber new sequence number
            \see getSequenceNumber() */
        void setSequenceNumber(uint32_t sequenceNumber);

        /** Get name of cookie.
            \return name */
        const String_t& getName() const;

        /** Get value of cookie.
            \return value */
        const String_t& getValue() const;

        /** Get host parameter of cookie.
            \return host or domain name */
        const String_t& getHost() const;

        /** Get path parameter of cookie.
            \return path */
        const String_t& getPath() const;

        /** Check validity.
            \retval true this is a well-formed, initialized cookie
            \retval false this instance has not been initialized or is not well-formed */
        bool isValid() const;

        /** Check persistence.
            \retval true this is a persistent cookie with an expiration date
            \retval false this is a session cookie without expiration date */
        bool isPersistent() const;

        /** Check expiration.
            Note that session cookies never expire, they should not be persisted; see isPersistent().
            \param now current time
            \retval true this cookie is expired and can be discarded
            \retval false this cookie is not expired and should be kept */
        bool isExpired(const afl::sys::Time now) const;

        /** Match URL.
            \param url URL to test
            \retval true This cookie should be sent with a request for this URL
            \retval false This cookie should not be sent */
        bool matchUrl(const Url& url) const;

        /** Match cookie.
            When the server sends a new cookie, we need to determine whether the new cookie
            is really a new one, or just updates an old one.
            \param other Other cookie to check
            \retval true This cookie replaces the other one
            \retval false These are different cookies */
        bool matchCookie(const Cookie& other) const;

        /** Parse from cookies.txt line.
            \param line Line from cookies.txt, not including final newline
            \retval true Line parsed successfully, cookie initialized
            \retval false Line could not be parsed, cookie is in indeterminate state */
        bool parseFromString(const String_t& line);

        /** Convert to cookies.txt line.
            \return Cookie in correct format for cookies.txt, not including final newline */
        String_t toString() const;

     private:
        uint32_t m_sequenceNumber;
        String_t m_host;
        bool     m_hostDomainMatch;
        String_t m_path;
        bool     m_secure;
        afl::sys::Time m_expirationTime;
        String_t m_name;
        String_t m_value;
        bool     m_persistent;

        void init(const Url& url, afl::sys::Time now, const HeaderField& field);
    };

} } }

inline uint32_t
afl::net::http::Cookie::getSequenceNumber() const
{
    return m_sequenceNumber;
}

inline void
afl::net::http::Cookie::setSequenceNumber(uint32_t sequenceNumber)
{
    m_sequenceNumber = sequenceNumber;
}

inline const String_t&
afl::net::http::Cookie::getName() const
{
    return m_name;
}

inline const String_t&
afl::net::http::Cookie::getValue() const
{
    return m_value;
}

inline const String_t&
afl::net::http::Cookie::getHost() const
{
    return m_host;
}

inline const String_t&
afl::net::http::Cookie::getPath() const
{
    return m_path;
}

#endif
