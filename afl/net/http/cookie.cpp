/**
  *  \file afl/net/http/cookie.cpp
  *  \brief Class afl::net::http::Cookie
  */

#include <sstream>
#include "afl/net/http/cookie.hpp"
#include "afl/net/headerconsumer.hpp"
#include "afl/sys/duration.hpp"
#include "afl/string/parse.hpp"
#include "afl/sys/parsedtime.hpp"
#include "afl/base/countof.hpp"

namespace {
    String_t getBasePath(String_t s)
    {
        String_t::size_type p = s.find('?');
        if (p != String_t::npos) {
            s.erase(p);
        }
        p = s.rfind('/');
        if (p != String_t::npos) {
            // RFC 6265 5.1.4: result is either "/" or something not ending in "/"
            if (p == 0) {
                s.erase(1);
            } else {
                s.erase(p);
            }
        }
        return s;
    }

    /*
     *  Matching cookies
     */

    bool matchHost(const String_t& userHost, const String_t& cookieHost, bool domainMatch)
    {
        if (cookieHost == userHost) {
            // Exact match
            return true;
        } else if (userHost.size() > cookieHost.size()) {
            // Possible domain match
            // - must be allowed
            // - host must not be an IP address (on embedded ":" for IPv6, not entirely digits for IPv4)
            // - tail must match
            // - match must leave off at a "."
            return domainMatch
                && userHost.find(':') == String_t::npos
                && userHost.find_first_not_of("0123456789.") != String_t::npos
                && userHost.compare(userHost.size() - cookieHost.size(), cookieHost.size(),
                                    cookieHost, 0, cookieHost.size()) == 0
                && ((cookieHost.size() > 0 && cookieHost[0] == '.')
                    || (userHost[userHost.size() - cookieHost.size() - 1] == '.'));
        } else {
            // No match
            return false;
        }
    }

    bool matchCookiePath(const String_t& userPath, const String_t& cookiePath)
    {
        if (userPath.size() < cookiePath.size()) {
            // Size mismatch.
            // This special case is not in the RFC.
            // If cookie is placed on "/foo/" using a Path attribute,
            // it will otherwise not match a request for "/foo/", because that has an implied path of "/foo".
            return userPath.size() + 1 == cookiePath.size()
                && userPath.compare(0, userPath.size(), cookiePath, 0, userPath.size()) == 0
                && cookiePath[userPath.size()] == '/';
        } else if (userPath.compare(0, cookiePath.size(), cookiePath, 0, cookiePath.size()) != 0) {
            // Prefix mismatch
            return false;
        } else if (userPath.size() == cookiePath.size()) {
            // Identical
            return true;
        } else if (cookiePath.size() > 0 && cookiePath[cookiePath.size()-1] == '/') {
            // Cookie path ends in "/", and user path is a proper prefix, so it's ok
            return true;
        } else if (userPath[cookiePath.size()] == '/') {
            // User path is longer than cookie path and continues with a "/", so it's ok
            return true;
        } else {
            // No match
            return false;
        }
    }

    /*
     *  Parsing the Expire value
     */

    bool isDelimiter(char ch)
    {
        return ch == 0x09
            || (ch >= 0x20 && ch <= 0x2F)
            || (ch >= 0x3B && ch <= 0x40)
            || (ch >= 0x5B && ch <= 0x60)
            || (ch >= 0x7B && ch <= 0x7E);
    }

    bool isDigit(char ch)
    {
        return (ch >= '0' && ch <= '9');
    }

    bool parseNumber(const String_t& str, String_t::size_type& n, size_t minDig, size_t maxDig, uint16_t& val)
    {
        size_t numDig  = 0;
        uint16_t v = 0;
        while (n < str.size() && isDigit(str[n])) {
            ++numDig;
            if (numDig > maxDig) {
                return false;
            }
            v = uint16_t(10*v + (str[n] - '0'));
            ++n;
        }
        if (numDig < minDig) {
            return false;
        }
        val = v;
        return true;
    }

    bool parseDaytime(const String_t& str, afl::sys::ParsedTime& pt)
    {
        // time       = hms-time ( non-digit *OCTET )
        // hms-time   = time-field ":" time-field ":" time-field
        // time-field = 1*2DIGIT
        String_t::size_type n = 0;
        if (!parseNumber(str, n, 1, 2, pt.m_hour)) {
            return false;
        }
        if (n >= str.size() || str[n] != ':') {
            return false;
        }
        ++n;
        if (!parseNumber(str, n, 1, 2, pt.m_minute)) {
            return false;
        }
        if (n >= str.size() || str[n] != ':') {
            return false;
        }
        ++n;
        if (!parseNumber(str, n, 1, 2, pt.m_second)) {
            return false;
        }
        return true;
    }

    bool parseDay(const String_t& str, afl::sys::ParsedTime& pt)
    {
        // day-of-month = 1*2DIGIT ( non-digit *OCTET )
        String_t::size_type n = 0;
        return parseNumber(str, n, 1, 2, pt.m_day);
    }

    bool parseMonth(const String_t& str, afl::sys::ParsedTime& pt)
    {
        // month = ( "jan" / "feb" / "mar" / "apr" /
        //           "may" / "jun" / "jul" / "aug" /
        //           "sep" / "oct" / "nov" / "dec" ) *OCTET
        static const char months[][4] = { "jan", "feb", "mar", "apr",
                                          "may", "jun", "jul", "aug",
                                          "sep", "oct", "nov", "dec" };
        const String_t m(str, 0, 3);
        for (size_t i = 0; i < countof(months); ++i) {
            if (afl::string::strCaseCompare(m, months[i]) == 0) {
                pt.m_month = uint16_t(i+1);
                return true;
            }
        }
        return false;
    }

    bool parseYear(const String_t& str, afl::sys::ParsedTime& pt)
    {
        // year = 2*4DIGIT ( non-digit *OCTET )
        String_t::size_type n = 0;
        return parseNumber(str, n, 2, 4, pt.m_year);
    }

    bool parseTime(const String_t& s, afl::sys::Time& t)
    {
        // Build a ParsedTime
        afl::sys::ParsedTime pt = { 0, 0, 0, 0, 0, 0, 0, 0 };

        // Algorithm according to RFC 6265 5.1.1
        String_t::size_type p = 0;
        bool foundTime    = false;
        bool foundDaytime = false;
        bool foundMonth   = false;
        bool foundYear    = false;
        while (1) {
            // Divide the cookie-date into date-tokens.
            // A delimiter is one of %x09 / %x20-2F / %x3B-40 / %x5B-60 / %x7B-7E,
            // a date-token is a sequence of non-delimiters.
            while (p < s.size() && isDelimiter(s[p])) {
                ++p;
            }
            String_t::size_type len = 0;
            while (p+len < s.size() && !isDelimiter(s[p+len])) {
                ++len;
            }
            if (len == 0) {
                break;
            }

            // Process each date-token sequentially in the order the date-tokens appear in the cookie-date:
            String_t token(s, p, len);
            if (!foundTime && parseDaytime(token, pt)) {
                // 1. time
                foundTime = true;
            } else if (!foundDaytime && parseDay(token, pt)) {
                // 2. day
                foundDaytime = true;
            } else if (!foundMonth && parseMonth(token, pt)) {
                // 3. month
                foundMonth = true;
            } else if (!foundYear && parseYear(token, pt)) {
                // 4. year
                foundYear = true;
            } else {
                // ignore
            }

            // Next
            p += len;
        }

        // Fail if any component is missing
        if (!foundTime || !foundDaytime || !foundMonth || !foundYear) {
            return false;
        }

        // Fix up the year
        if (pt.m_year < 100) {
            if (pt.m_year < 70) {
                pt.m_year = uint16_t(pt.m_year + 2000);
            } else {
                pt.m_year = uint16_t(pt.m_year + 1900);
            }
        }

        // Fail on range errors
        if (pt.m_day < 1 || pt.m_day > 31 || pt.m_year < 1601 || pt.m_hour > 23 || pt.m_minute > 59 || pt.m_second > 59) {
            return false;
        }

        // Success!
        t = afl::sys::Time(pt, afl::sys::Time::UniversalTime);
        return true;
    }

    /*
     *  Parsing a cookies.txt line
     */
    bool parseStringField(const String_t& line, String_t::size_type& n, String_t& result)
    {
        // Find tab-delimited field
        String_t::size_type end = n;
        while (end < line.size() && line[end] != '\t') {
            ++end;
        }

        // Produce output
        result.assign(line, n, end - n);

        // Update position
        n = (end < line.size() ? end+1 : end);
        return true;
    }

    bool parseBoolField(const String_t& line, String_t::size_type& n, bool& result)
    {
        String_t tmp;
        if (!parseStringField(line, n, tmp)) {
            return false;
        } else if (afl::string::strCaseCompare(tmp, "TRUE") == 0) {
            result = true;
            return true;
        } else if (afl::string::strCaseCompare(tmp, "FALSE") == 0) {
            result = false;
            return true;
        } else {
            return false;
        }
    }

    bool parseTimeField(const String_t& line, String_t::size_type& n, afl::sys::Time& result)
    {
        String_t tmp;
        int64_t val;
        if (!parseStringField(line, n, tmp) || !afl::string::strToInteger(tmp, val)) {
            return false;
        } else if (val < 0 || val > 0x10000000000LL) {
            // Refuse obvious garbage. 0x10000000000 is 1000 billions, i.e. 34000 years,
            // but still has plenty room for conversion to milliseconds.
            return false;
        } else {
            result = afl::sys::Time::fromUnixTime(val);
            return true;
        }
    }
}

afl::net::http::Cookie::Cookie()
    : m_sequenceNumber(0),
      m_host(),
      m_hostDomainMatch(false),
      m_path(),
      m_secure(),
      m_expirationTime(),
      m_name(),
      m_value(),
      m_persistent(false)
{ }

afl::net::http::Cookie::Cookie(const Url& url, afl::sys::Time now, const HeaderField& field)
    : m_sequenceNumber(0),
      m_host(),
      m_hostDomainMatch(false),
      m_path(),
      m_secure(),
      m_expirationTime(),
      m_name(),
      m_value(),
      m_persistent(false)
{
    init(url, now, field);
}

afl::net::http::Cookie::~Cookie()
{ }

bool
afl::net::http::Cookie::isValid() const
{
    return !m_name.empty()
        && !m_host.empty()
        && !m_path.empty()
        && m_path[0] == '/';
}

bool
afl::net::http::Cookie::isPersistent() const
{
    return m_persistent;
}

bool
afl::net::http::Cookie::isExpired(const afl::sys::Time now) const
{
    // Only persistent cookies can expire
    return m_persistent && m_expirationTime <= now;
}

bool
afl::net::http::Cookie::matchUrl(const Url& url) const
{
    return matchHost(url.getHost(), m_host, m_hostDomainMatch)
        && matchCookiePath(getBasePath(url.getPath()), m_path)
        && (!m_secure || url.getScheme() == "https");
}

bool
afl::net::http::Cookie::matchCookie(const Cookie& other) const
{
    // RFC 6265 5.3 step 11: a cookie is identified by host, path, and name.
    return m_host == other.m_host
        && m_path == other.m_path
        && m_name == other.m_name;
}

bool
afl::net::http::Cookie::parseFromString(const String_t& line)
{
    // A cookie from cookies.txt is persistent by default
    m_persistent = true;

    // Parse it. Each of these functions will abort the parse on problems.
    // The isValid() call will abort if the resulting cookie is not valid,
    // in particular if it has no name because of a shortened line.
    String_t::size_type n = 0;
    return !line.empty()
        && line[0] != '#'
        && parseStringField(line, n, m_host)
        && parseBoolField(line, n, m_hostDomainMatch)
        && parseStringField(line, n, m_path)
        && parseBoolField(line, n, m_secure)
        && parseTimeField(line, n, m_expirationTime)
        && parseStringField(line, n, m_name)
        && parseStringField(line, n, m_value)
        && isValid();
}

String_t
afl::net::http::Cookie::toString() const
{
    std::ostringstream ss;
    ss << m_host
       << '\t' << (m_hostDomainMatch ? "TRUE" : "FALSE")
       << '\t' << m_path
       << '\t' << (m_secure ? "TRUE" : "FALSE")
       << '\t' << m_expirationTime.getUnixTime()
       << '\t' << m_name
       << '\t' << m_value;
    return ss.str();
}

void
afl::net::http::Cookie::init(const Url& url, afl::sys::Time now, const HeaderField& field)
{
    // Parser options. RFC 6265 wants no special processing applied to the cookie value.
    const int options = HeaderField::NoComments | HeaderField::NoQuotes;

    // Initialize to defaults
    m_host            = url.getHost();
    m_hostDomainMatch = false;
    m_path            = getBasePath(url.getPath());
    m_secure          = false;
    m_expirationTime  = afl::sys::Time();
    m_name            = field.getName();
    m_value           = field.getPrimaryValue(options);
    m_persistent      = false;

    // Parse attributes
    class Parser : public HeaderConsumer {
     public:
        Parser(Cookie& self, afl::sys::Time now)
            : m_self(self),
              m_now(now),
              m_hadMaxAge(false)
            { }
        void handleHeader(String_t key, String_t value)
            {
                if (afl::string::strCaseCompare(key, "expires") == 0) {
                    // Expiration date in text format (defers to Max-Age)
                    if (!m_hadMaxAge) {
                        if (parseTime(value, m_self.m_expirationTime)) {
                            m_self.m_persistent = true;
                        }
                    }
                } else if (afl::string::strCaseCompare(key, "max-age") == 0) {
                    // Maximum age
                    if (!m_hadMaxAge) {
                        int64_t t;
                        if (afl::string::strToInteger(value, t)) {
                            m_self.m_expirationTime = m_now + afl::sys::Duration::fromSeconds(t);
                            m_hadMaxAge = true;
                            m_self.m_persistent = true;
                        }
                    }
                } else if (afl::string::strCaseCompare(key, "domain") == 0) {
                    // Domain limit
                    if (!value.empty()) {
                        m_self.m_host = afl::string::strLCase(value);
                        m_self.m_hostDomainMatch = true;
                    }
                } else if (afl::string::strCaseCompare(key, "path") == 0) {
                    // Path limit
                    if (!value.empty() && value[0] == '/') {
                        m_self.m_path = value;
                    }
                } else if (afl::string::strCaseCompare(key, "secure") == 0) {
                    // Limitation to HTTPS
                    m_self.m_secure = true;
                } else {
                    // Ignored attributes:
                    //   httponly (RFC 6265)
                    //   comment (RFC 2965)
                    //   commenturl (RFC 2965)
                    //   port (RFC 2965)
                    //   version (RFC 2965)
                }
            }
     private:
        Cookie& m_self;
        afl::sys::Time m_now;
        bool m_hadMaxAge;
    };
    Parser p(*this, now);
    field.enumerateSecondaryValues(p, options);

    // Fix up. Some hosts send IP addresses with port number in "Domain="
    // (e.g. PlanetsCentral on Apache via localhost).
    String_t::size_type n = m_host.find_last_of(":]");
    if (n != String_t::npos && m_host[n] == ':') {
        m_host.erase(n);
    }
}
