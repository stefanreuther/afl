/**
  *  \file afl/net/smtp/client.hpp
  *  \brief Class afl::net::smtp::Client
  */
#ifndef AFL_AFL_NET_SMTP_CLIENT_HPP
#define AFL_AFL_NET_SMTP_CLIENT_HPP

#include "afl/base/memory.hpp"
#include "afl/net/mimebuilder.hpp"
#include "afl/net/name.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/smtp/client.hpp"
#include "afl/net/smtp/configuration.hpp"
#include "afl/string/string.hpp"
#include "afl/sys/log.hpp"

namespace afl { namespace net { namespace smtp {

    /** SMTP client.
        This serves as a possibly-long-lived client for sending mail.
        Server and sender configuration is stored persistently in this object.
        Mails can be sent using send().

        Mail content is specified as full MIME entities including headers and possibly-encoded body.
        Client does not verify or modify the content given. */
    class Client {
     public:
        /** Constructor.
            \param stack  Network stack. Lifetime must exceed that of Client.
            \param name   Network name of server to connect to.
            \param config Configuration to use for all transactions */
        Client(NetworkStack& stack, const Name& name, const Configuration& config);

        /** Destructor. */
        ~Client();

        /** Send mail (given as byte array).
            \param to List of receiver email addresses
            \param content Mail content. Should end in CRLF.
            \throw afl::except::RemoteErrorException server reports an error
            \throw afl::except::FileSystemException network error */
        void send(afl::base::Memory<const String_t> to, afl::base::ConstBytes_t content);

        /** Send mail (given as MimeBuilder).
            \param to List of receiver email addresses
            \param content Mail content
            \throw afl::except::RemoteErrorException server reports an error
            \throw afl::except::FileSystemException network error */
        void send(afl::base::Memory<const String_t> to, const MimeBuilder& content);

        /** Access logger.
            You can attach your own logger or listener here to receive logs of the SMTP dialogs. */
        afl::sys::Log& log();

     private:
        NetworkStack& m_networkStack;
        Name m_name;
        Configuration m_config;
        afl::sys::Log m_log;
    };

} } }

#endif
