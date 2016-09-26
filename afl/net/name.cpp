/**
  *  \file afl/net/name.cpp
  *  \brief Class afl::net::Name
  */

#include <cstdio>
#include "afl/net/name.hpp"

afl::net::Name::Name(const String_t& name, uint16_t port)
    : m_name(name),
      m_service()
{
    setService(port);
}

void
afl::net::Name::setService(uint16_t port)
{
    char tmp[30];
    std::sprintf(tmp, "%u", static_cast<unsigned int>(port));
    m_service = tmp;
}

afl::net::Name
afl::net::Name::parse(const String_t& name, const String_t& defaultService)
{
    String_t::size_type n;
    if (!name.empty() && name[0] == '[') {
        n = name.find(']');
        if (n != name.npos && n+1 < name.size() && name[n+1] == ':') {
            // "[a:b:c]:port"
            return Name(String_t(name, 1, n-1),
                        String_t(name, n+2, name.npos));
        }
        if (n == name.size()-1) {
            // "[a:b:c]"
            return Name(String_t(name, 1, n-1), defaultService);
        }
    }
    if ((n = name.find(':')) != name.npos) {
        // "a.b.c:port"
        return Name(String_t(name, 0, n),
                    String_t(name, n+1, name.npos));
    }

    // Just a name
    return Name(name, defaultService);
}

String_t
afl::net::Name::toString() const
{
    String_t result;
    if (isValid()) {
        if (m_name.find(':') != m_name.npos) {
            result += '[';
            result += m_name;
            result += ']';
        } else {
            result += m_name;
        }
        result += ':';
        result += m_service;
    }
    return result;
}

bool
afl::net::Name::operator==(const Name& rhs) const
{
    return m_name == rhs.m_name
        && m_service == rhs.m_service;
}
