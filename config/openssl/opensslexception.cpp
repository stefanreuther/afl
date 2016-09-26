/**
  *  \file config/openssl/opensslexception.cpp
  *  \brief Class config::openssl::OpenSSLException
  */

#include "afl/config.h"
#ifdef HAVE_OPENSSL
# include <openssl/err.h>
# include "config/openssl/opensslexception.hpp"
# include "afl/string/format.hpp"

config::openssl::OpenSSLException::OpenSSLException(long err)
    : std::runtime_error(ERR_reason_error_string(err) != 0
                         ? afl::string::Format("%s, code: 0x%08X, in %s::%s",
                                               ERR_reason_error_string(err),
                                               err,
                                               ERR_lib_error_string(err),
                                               ERR_func_error_string(err))
                         : afl::string::Format("OpenSSL error 0x%08X", err))
{ }

config::openssl::OpenSSLException::~OpenSSLException() throw()
{ }
#endif
