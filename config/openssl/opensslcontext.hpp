/**
  *  \file config/openssl/opensslcontext.hpp
  *  \brief Class config::openssl::OpenSSLContext
  */
#ifndef AFL_CONFIG_OPENSSL_OPENSSLCONTEXT_HPP
#define AFL_CONFIG_OPENSSL_OPENSSLCONTEXT_HPP

#include <openssl/ssl.h>
#include "afl/net/securecontext.hpp"
#include "afl/base/uncopyable.hpp"
#include "afl/sys/mutex.hpp"

namespace config { namespace openssl {

    /** Size type.
        OpenSSL uses "int" as size, causing ugly warnings.
        We use this type to cast to, in case someone fixes this later. */
    typedef int Size_t;

    class OpenSSLContext : public afl::net::SecureContext, public afl::base::Uncopyable {
     public:
        OpenSSLContext();

        virtual ~OpenSSLContext();

        // Implementation of SecureContext methods:
        virtual afl::base::Ptr<afl::net::SecureSocket> wrapClient(afl::async::Controller& ctl, afl::base::Ptr<afl::net::Socket> peer);
        virtual afl::base::Ptr<afl::net::SecureSocket> wrapServer(afl::async::Controller& ctl, afl::base::Ptr<afl::net::Socket> peer);
        virtual void setCertificate(afl::base::ConstBytes_t data);
        virtual void setPrivateKey(afl::base::ConstBytes_t data);

        SSL* createSSL();

     private:
        afl::sys::Mutex m_mutex;
        SSL_CTX* m_ctx;
    };

} }

#endif
