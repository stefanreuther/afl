/**
  *  \file afl/net/name.hpp
  *  \brief Class afl::net::Name
  */
#ifndef AFL_AFL_NET_NAME_HPP
#define AFL_AFL_NET_NAME_HPP

#include "afl/string/string.hpp"

namespace afl { namespace net {

    /** Network name.
        Represents the name of a network endpoint (name and port number/service name). */
    class Name {
     public:
        /** Constructor.
            Makes a blank (invalid) name. */
        Name();

        /** Constructor.
            \param name Host name
            \param service Service name or port number */
        Name(const String_t& name, const String_t& service);

        /** Constructor.
            \param name Host name
            \param port Port number */
        Name(const String_t& name, uint16_t port);

        /** Set host name.
            \param name Host name */
        void setName(const String_t& name);

        /** Get host name.
            \return Host name */
        const String_t& getName() const;

        /** Set service name.
            \param service Service name or port number */
        void setService(const String_t& service);

        /** Set service name as port number.
            \param port Port number */
        void setService(uint16_t port);

        /** Get service name.
            \return Service name or port number */
        const String_t& getService() const;

        /** Get string representation.
            \return a single string representing this name; could be passed to the constructor again. */
        String_t toString() const;

        /** Check validity.
            A name is valid if it has a nonempty name and service. */
        bool isValid() const;

        /** Compare for equality.
            \param rhs other name */
        bool operator==(const Name& rhs) const;

        /** Compare for inequality.
            \param rhs other name */
        bool operator!=(const Name& rhs) const;

        /** Parse an endpoint specification.
            \param name Name given by user, e.g. "localhost:8181", "127.0.0.1:8181", "[::1]:8181".
            \param defaultService Service name to use if the name does not contain one */
        static Name parse(const String_t& name, const String_t& defaultService);

     private:
        /** Host name. */
        String_t m_name;

        /** Service name. */
        String_t m_service;
    };

} }

inline
afl::net::Name::Name()
    : m_name(), m_service()
{ }

inline
afl::net::Name::Name(const String_t& name, const String_t& service)
    : m_name(name), m_service(service)
{ }

inline void
afl::net::Name::setName(const String_t& name)
{
    m_name = name;
}

inline const String_t&
afl::net::Name::getName() const
{
    return m_name;
}

inline void
afl::net::Name::setService(const String_t& service)
{
    m_service = service;
}

inline const String_t&
afl::net::Name::getService() const
{
    return m_service;
}

inline bool
afl::net::Name::isValid() const
{
    return !m_name.empty() && !m_service.empty();
}

inline bool
afl::net::Name::operator!=(const Name& rhs) const
{
    return !operator==(rhs);
}

#endif
