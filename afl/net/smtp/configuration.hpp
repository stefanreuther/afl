/**
  *  \file afl/net/smtp/configuration.hpp
  *  \brief Structure afl::net::smtp::Configuration
  */
#ifndef AFL_AFL_NET_SMTP_CONFIGURATION_HPP
#define AFL_AFL_NET_SMTP_CONFIGURATION_HPP

#include "afl/string/string.hpp"

namespace afl { namespace net { namespace smtp {

    /** Configuration structure for SMTP. */
    struct Configuration {
        /** Constructor.
            \param hello Host identifier for HELO.
            \param from  Sender mail address for MAIL FROM. */
        Configuration(String_t hello, String_t from);

        /** Host identifier for HELO. */
        String_t hello;

        /** Sender mail address for MAIL FROM. */
        String_t from;
    };

} } }

inline
afl::net::smtp::Configuration::Configuration(String_t hello, String_t from)
    : hello(hello),
      from(from)
{ }

#endif
