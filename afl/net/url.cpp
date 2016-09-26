/**
  *  \file afl/net/url.cpp
  *  \brief Class afl::net::Url
  *
  *  \see RFC 1630 Universal Resource Identifiers
  *
  *  FIXME: the UrlEncoding.decode calls can be written to avoid the copying!
  */

#include <algorithm>
#include "afl/net/url.hpp"
#include "afl/charset/urlencoding.hpp"
#include "afl/net/headerconsumer.hpp"

namespace {
    String_t
    mergePath(const String_t& left, const String_t& right)
    {
        // Result accumulator. Invariant: always ends in a slash, or is empty.
        String_t result(left);

        // Start with left.
        //    /foo/bar  --> path is /foo/
        //    /foo/bar/ --> path is /foo/bar/
        String_t::size_type n = result.rfind('/');
        if (n != String_t::npos) {
            result.erase(n+1);
        }

        // Add right, component-wise.
        n = 0;
        for (String_t::size_type p = right.find('/', n); p != String_t::npos; p = right.find('/', n)) {
            String_t::size_type length = p-n;
            if (length == 2 && right[n] == '.' && right[n+1] == '.') {
                // Drop a component
                String_t::size_type drop;
                if (result.size() >= 2 && (drop = result.rfind('/', result.size()-2)) != String_t::npos) {
                    result.erase(drop+1);
                } else {
                    result = "/";
                }
            } else if (length == 1 && right[n] == '.') {
                // Ignore this component
            } else {
                // Add a component (including the slash)
                result.append(right, n, length+1);
            }
            n = p+1;
        }

        // Add final component
        String_t::size_type rem = right.size() - n;
        if (rem == 2 && right[n] == '.' && right[n+1] == '.') {
            // Drop a component
            String_t::size_type drop;
            if (result.size() >= 2 && (drop = result.rfind('/', result.size()-2)) != String_t::npos) {
                result.erase(drop);
            } else {
                result = "/";
            }
        } else if (rem == 1 && right[n] == '.') {
            // Ignore
        } else {
            // Add a component
            result.append(right, n, rem);
        }

        return result;
    }
}



afl::net::Url::Url()
    : m_scheme(),
      m_user(),
      m_password(),
      m_host(),
      m_port(),
      m_path(),
      m_fragment()
{ }

afl::net::Url::~Url()
{ }

void
afl::net::Url::clear()
{
    m_scheme.clear();
    m_user.clear();
    m_password.clear();
    m_host.clear();
    m_port.clear();
    m_path.clear();
    m_fragment.clear();
}


bool
afl::net::Url::parse(const String_t& url)
{
    // Start blank
    clear();

    /*
      If "scheme:" given, parse that.

      If remainder starts with "//", parse as user:password@host:port/path#fragment.
      Otherwise, parse as path#fragment.
    */

    afl::charset::UrlEncoding ue;

    // If a scheme is given, parse that.
    String_t::size_type n = url.find_first_of("@:/#");
    if (n != String_t::npos && n != 0 && url[n] == ':') {
        // Found a nonzero scheme
        m_scheme = ue.decode(afl::string::toMemory(String_t(url, 0, n)));
        ++n;
    } else {
        // No scheme given
        n = 0;
    }

    // If remainder starts with "//", parse user:password@host:port, up to (but not including) a slash.
    bool forceRoot;
    if (url.size() - n >= 2 && url.compare(n, 2, "//", 2) == 0) {
        n += 2;

        // Check for password
        String_t::size_type atPos = url.find('@', n);
        if (atPos == n) {
            // "foo://@" is a syntax error.
            return false;
        }
        if (atPos != String_t::npos) {
            // We have user:password@ or user@.
            // (Because we also have "//", we're not dealing with a message-Id or email.)
            String_t::size_type colonPos = url.find(':', n);
            if (colonPos == n) {
                // "foo://:@" is a syntax error.
                return false;
            }
            if (colonPos < atPos) {
                // Found username + password
                m_user = ue.decode(afl::string::toMemory(String_t(url, n, colonPos-n)));
                m_password = ue.decode(afl::string::toMemory(String_t(url, colonPos+1, atPos-colonPos-1)));
            } else {
                m_user = ue.decode(afl::string::toMemory(String_t(url, n, atPos-n)));
                m_password.clear();
            }
            n = atPos+1;
        } else {
            m_user.clear();
            m_password.clear();
        }

        // Check for host name.
        // The host name might start with "[", then it is an IPv6 address.
        if (n < url.size() && url[n] == '[') {
            // Host name ends at "]"
            String_t::size_type bracketPos = url.find(']', n);
            if (bracketPos == String_t::npos || bracketPos == n) {
                // "foo://[" is a syntax error
                return false;
            }
            m_host = ue.decode(afl::string::toMemory(String_t(url, n+1, bracketPos-n-1)));
            n = bracketPos+1;
            if (n < url.size() && url[n] != ':' && url[n] != '/' && url[n] != '#') {
                // "foo://[...]bla" is a syntax error (but "foo://[...]" is not)
                return false;
            }
        } else {
            // Host name ends at ":" or "/"
            String_t::size_type endPos = url.find_first_of(":/#?", n);
            if (endPos < url.size()) {
                m_host = ue.decode(afl::string::toMemory(String_t(url, n, endPos-n)));
                n = endPos;
            } else {
                m_host = ue.decode(afl::string::toMemory(String_t(url, n, String_t::npos)));
                n = url.size();
            }
        }

        // Check for port number.
        if (n < url.size() && url[n] == ':') {
            ++n;
            String_t::size_type portEndPos = url.find_first_not_of("0123456789", n);
            if (portEndPos == n) {
                // "foo://host:/" is a syntax error
                return false;
            }
            if (portEndPos == String_t::npos) {
                m_port = ue.decode(afl::string::toMemory(String_t(url, n, String_t::npos)));
                n = url.size();
            } else {
                m_port = ue.decode(afl::string::toMemory(String_t(url, n, portEndPos-n)));
                n = portEndPos;
                if (url[n] != '/' && url[n] != '#' && url[n] != '?') {
                    // "foo://host:123bla" is a syntax error
                    return false;
                }
            }
        }

        // If no path follows, we must have a root path
        forceRoot = true;
    } else {
        forceRoot = false;
    }

    // What remains is a path and fragment.
    String_t::size_type fragPos = url.find('#', n);
    if (fragPos != String_t::npos) {
        m_path.assign(url, n, fragPos-n);
        m_fragment = ue.decode(afl::string::toMemory(String_t(url, fragPos)));
    } else {
        m_path.assign(url, n, String_t::npos);
        m_fragment.clear();
    }

    // Need root?
    if (forceRoot && (m_path.empty() || m_path[0] != '/')) {
        m_path.insert(0, "/", 1);
    }
    return true;
}

void
afl::net::Url::mergeFrom(const Url& other)
{
    bool keep = false;

    // Copy scheme (entirely optional)
    if (m_scheme.empty()) {
        m_scheme = other.m_scheme;
    } else {
        keep = true;
    }

    // Copy host. user/pass/port are associated with host, and are thus copied en block.
    // If host is empty, so are user/pass/port.
    if (m_host.empty() && !keep) {
        m_host = other.m_host;
        m_port = other.m_port;
        m_user = other.m_user;
        m_password = other.m_password;
    } else {
        keep = true;
    }

    // Merge path.
    if (!keep) {
        if (!m_path.empty() && m_path[0] == '/') {
            // absolute path, keep
        } else if (m_path.empty()) {
            // null path, copy
            m_path = other.m_path;
        } else {
            // combine paths
            m_path = mergePath(other.m_path, m_path);
        }
    }

    // Do not copy the fragment.
}


bool
afl::net::Url::isNull() const
{
    return m_scheme.empty() && m_path.empty() && m_fragment.empty();
}

bool
afl::net::Url::isValid() const
{
    return !m_scheme.empty() && !m_path.empty();
}

bool
afl::net::Url::isValidId() const
{
    return !m_scheme.empty() && !m_path.empty() && m_host.empty();
}

bool
afl::net::Url::isValidHost() const
{
    return !m_scheme.empty() && !m_path.empty() && !m_host.empty();
}

afl::net::Name
afl::net::Url::getName(const String_t& service) const
{
    return Name(m_host, m_port.size() ? m_port : service);
}

void
afl::net::Url::setName(const Name& name)
{
    m_host = name.getName();
    m_port = name.getService();
}

String_t
afl::net::Url::toString() const
{
    // FIXME: this should perform proper URL-encoding of fields.
    String_t result;
    if (!m_scheme.empty()) {
        result += m_scheme;
        result += ":";
    }
    if (!m_host.empty()) {
        result += "//";
        if (!m_user.empty()) {
            result += m_user;
            result += ":";
            result += m_password;
            result += "@";
        }
        if (m_host.find(':') != String_t::npos) {
            result += "[";
            result += m_host;
            result += "]";
        } else {
            result += m_host;
        }
        if (!m_port.empty()) {
            result += ":";
            result += m_port;
        }
        if (m_path.empty() || m_path[0] != '/') {
            result += "/";
        }
    }
    result += m_path;
    result += m_fragment;
    return result;
}

bool
afl::net::matchPath(const String_t& path, const String_t& prefix, String_t& suffix)
{
    if (path == prefix) {
        suffix.clear();
        return true;
    } else if (path.size() > prefix.size()
               && path.compare(0, prefix.size(), prefix) == 0
               && (path[prefix.size()] == '?' || path[prefix.size()] == '/'))
    {
        suffix.assign(path, prefix.size(), String_t::npos);
        return true;
    } else {
        return false;
    }
    
}

void
afl::net::matchArguments(String_t& path, HeaderConsumer& consumer)
{
    String_t::size_type p = path.find('?');
    if (p != String_t::npos) {
        // Process the arguments
        afl::charset::UrlEncoding ue;
        String_t::size_type n = p;
        while (n < path.size()) {
            // n points at a '?' or '&'. Skip that.
            ++n;

            // Look for next '&' and '='.
            String_t::size_type end = std::min(path.find('&', n), path.size());
            String_t::size_type eq = path.find('=', n);
            if (eq < end) {
                // ....&name=value&
                //      n   eq    end
                consumer.handleHeader(ue.decode(afl::string::toMemory(String_t(path, n, eq-n))),
                                      ue.decode(afl::string::toMemory(String_t(path, eq+1, end-eq-1))));
            } else if (end != n) {
                // ....&name&
                //      n   end   eq somewhere after
                consumer.handleHeader(ue.decode(afl::string::toMemory(String_t(path, n, end-n))),
                                      String_t());
            } else {
                // ....&
                //      n,end
            }
            n = end;
        }

        // Trim the path
        path.erase(p);
    }
}
