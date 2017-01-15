/**
  *  \file config/openssl/opensslcontext.cpp
  *  \brief Class config::openssl::OpenSSLContext
  */

#include "afl/config.h"
#ifdef HAVE_OPENSSL
# include <openssl/err.h>
# include "config/openssl/opensslcontext.hpp"
# include "config/openssl/opensslexception.hpp"
# include "config/openssl/opensslsocket.hpp"
# include "afl/sys/mutexguard.hpp"

namespace {
    // FIXME: where to place this best?
    struct Init {
        Init()
            {
                SSL_library_init();
                SSL_load_error_strings();
            }
        ~Init()
            {
                ERR_free_strings();
            }
    };
    Init gInit;

    BIO* createMemoryBIO(afl::base::ConstBytes_t mem)
    {
        // BIO_new_mem_buf takes a 'void*', but does not modify the memory.
        // Therefore, we need some ugly const_cast.
        BIO* result;
        if (mem.empty()) {
            result = BIO_new_mem_buf(const_cast<char*>(""), 0);
        } else {
            result = BIO_new_mem_buf(const_cast<uint8_t*>(mem.unsafeData()),
                                     config::openssl::Size_t(mem.size()));
        }
        if (result == 0) {
            throw config::openssl::OpenSSLException(ERR_get_error());
        }
        return result;
    }

}

config::openssl::OpenSSLContext::OpenSSLContext()
    : afl::net::SecureContext(),
      afl::base::Uncopyable(),
      m_mutex(),
      m_ctx(0)
{
    // "SSLv23" seems to be the generic all-flavour method that, despite its name,
    // also supports up to TLSv1.2.
    m_ctx = SSL_CTX_new(SSLv23_method());
    if (m_ctx == 0) {
        throw OpenSSLException(ERR_get_error());
    }
}

config::openssl::OpenSSLContext::~OpenSSLContext()
{
    SSL_CTX_free(m_ctx);
}

afl::base::Ref<afl::net::SecureSocket>
config::openssl::OpenSSLContext::wrapClient(afl::async::Controller& ctl, afl::base::Ref<afl::net::Socket> peer)
{
    afl::base::Ptr<OpenSSLSocket> result;

    {
        afl::sys::MutexGuard g(m_mutex);
        result = new OpenSSLSocket(*this, peer);
    }

    result->connect(ctl);
    return *result;
}

afl::base::Ref<afl::net::SecureSocket>
config::openssl::OpenSSLContext::wrapServer(afl::async::Controller& ctl, afl::base::Ref<afl::net::Socket> peer)
{
    afl::base::Ref<OpenSSLSocket> result(*new OpenSSLSocket(*this, peer));
    result->accept(ctl);
    return result;
}

void
config::openssl::OpenSSLContext::setCertificate(afl::base::ConstBytes_t data)
{
    // This is a re-implementation of SSL_CTX_use_certificate_file using a memory BIO.
    // The error generated below is the same error code as that generated by that function.
    BIO* b = createMemoryBIO(data);

    X509* x = PEM_read_bio_X509(b, 0, 0, 0);
    if (x == 0) {
        BIO_free(b);
        throw OpenSSLException(ERR_PACK(ERR_LIB_SSL, SSL_F_SSL_CTX_USE_CERTIFICATE_FILE, ERR_R_PEM_LIB));
    }

    int err;
    {
        afl::sys::MutexGuard g(m_mutex);
        err = SSL_CTX_use_certificate(m_ctx, x);
    }

    X509_free(x);
    BIO_free(b);

    if (!err) {
        throw OpenSSLException(ERR_get_error());
    }
}

void
config::openssl::OpenSSLContext::setPrivateKey(afl::base::ConstBytes_t data)
{
    // This is a re-implementation of SSL_CTX_use_PrivateKey_file using a memory BIO.
    // The error generated below is the same error code as that generated by that function.
    BIO* b = createMemoryBIO(data);

    EVP_PKEY* pkey = PEM_read_bio_PrivateKey(b, 0, 0 ,0);
    if (pkey == 0) {
        BIO_free(b);
        throw OpenSSLException(ERR_PACK(ERR_LIB_SSL, SSL_F_SSL_CTX_USE_PRIVATEKEY_FILE, ERR_R_PEM_LIB));
    }

    int err;
    {
        afl::sys::MutexGuard g(m_mutex);
        err = SSL_CTX_use_PrivateKey(m_ctx, pkey);
    }

    EVP_PKEY_free(pkey);
    BIO_free(b);

    if (!err) {
        throw OpenSSLException(ERR_get_error());
    }
}

SSL*
config::openssl::OpenSSLContext::createSSL()
{
    afl::sys::MutexGuard g(m_mutex);
    return SSL_new(m_ctx);
}

#endif
