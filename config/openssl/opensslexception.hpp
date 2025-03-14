/**
  *  \file config/openssl/opensslexception.hpp
  *  \brief Class config::openssl::OpenSSLException
  */
#ifndef AFL_CONFIG_OPENSSL_OPENSSLEXCEPTION_HPP
#define AFL_CONFIG_OPENSSL_OPENSSLEXCEPTION_HPP

#include <stdexcept>

namespace config { namespace openssl {

    /** OpenSSL exception.
        Formats the OpenSSL error message into a sensible exception string. */
    class OpenSSLException : public std::runtime_error {
     public:
        /** Constructor.
            \param err Error code from ERR_get_error(). */
        explicit OpenSSLException(unsigned long err);

        /** Virtual destructor. */
        ~OpenSSLException() throw();
    };

} }

#endif
