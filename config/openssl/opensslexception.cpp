/**
  *  \file config/openssl/opensslexception.cpp
  *  \brief Class config::openssl::OpenSSLException
  */

#include "afl/config.h"
#ifdef HAVE_OPENSSL
# include <openssl/err.h>
# include "config/openssl/opensslexception.hpp"
# include "afl/string/format.hpp"

config::openssl::OpenSSLException::OpenSSLException(unsigned long err)
    : std::runtime_error(ERR_reason_error_string(err) != 0
                         ? String_t(afl::string::Format("%s, code: 0x%08X, in %s",
                                                        ERR_reason_error_string(err),
                                                        err,
                                                        ERR_lib_error_string(err)))
                         : String_t(afl::string::Format("OpenSSL error 0x%08X", err)))
{ }

config::openssl::OpenSSLException::~OpenSSLException() throw()
{ }
#endif
