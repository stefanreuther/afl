/**
  *  \file afl/net/securecontext.cpp
  *  \brief class afl::net::SecureContext
  */

#include "afl/net/securecontext.hpp"
#include "afl/config.h"

#if HAVE_OPENSSL
/*
 *  OpenSSL variant
 */
# include "config/openssl/opensslcontext.hpp"
afl::base::Ptr<afl::net::SecureContext>
afl::net::SecureContext::create()
{
    return new config::openssl::OpenSSLContext();
}
#else
/*
 *  No SSL Library - dummy version
 */
# include "afl/except/unsupportedexception.hpp"
afl::base::Ptr<afl::net::SecureContext>
afl::net::SecureContext::create()
{
    throw afl::except::UnsupportedException("ssl");
}
#endif
