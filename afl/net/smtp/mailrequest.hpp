/**
  *  \file afl/net/smtp/mailrequest.hpp
  *  \brief Class afl::net::smtp::MailRequest
  */
#ifndef AFL_AFL_NET_SMTP_MAILREQUEST_HPP
#define AFL_AFL_NET_SMTP_MAILREQUEST_HPP

#include "afl/base/memory.hpp"
#include "afl/net/line/linehandler.hpp"
#include "afl/net/smtp/configuration.hpp"
#include "afl/sys/loglistener.hpp"

namespace afl { namespace net { namespace smtp {

    /** LineHandler for sending mail.
        This implements a SMTP client connection that executes a single mail transaction.
        This transaction sends mail from a configured sender to a list of receivers.

        Right now, this implements a simple unauthenticated request (HELO/MAIL FROM/RCPT TO/DATA),
        that can be used against the system MTA of a Unix system. */
    class MailRequest : public afl::net::line::LineHandler {
     public:
        /** Constructor.
            \param config  Configuration
            \param to      Receivers. Should not be empty.
                           If you have no receivers, you shouldn't normally make a server connection at all.
                           Running a MailRequest with empty receiver list will just execute a QUIT command on the server.
            \param content Mail content. Should end in CRLF, have lines separated by CRLF,
                           and lines starting with dots must have them doubled.
                           Mail content produced by MimeBuilder will fulfill these criteria.
                           If the final CRLF is missing, MailRequest will add one, possibly causing the perceived content to change.
                           If the content contains non-escaped dots, this will probably violate the protocol and fail.
            \param log     Logger */
        MailRequest(const Configuration& config,
                    afl::base::Memory<const String_t> to,
                    afl::base::ConstBytes_t content,
                    afl::sys::LogListener& log);

        /** Destructor. */
        ~MailRequest();

        // LineHandler:
        virtual bool handleOpening(afl::net::line::LineSink& response);
        virtual bool handleLine(const String_t& line, afl::net::line::LineSink& response);
        virtual void handleConnectionClose();

     private:
        enum State {
            Greeting_2,         // receive greeting, "2xx", then send HELO
            Hello_2,            // receive HELO response, "2xx", then send MAIL FROM
            From_2,             // receive MAIL FROM or RCPT TO response, "2xx", then send RCPT TO
            Recipient_2,        // receive final RCPT TO response, "2xx", then send DATA
            Data_3,             // receive DATA response, "3xx", then send content
            Content_2,          // receive content response, "2xx", then send QUIT
            Quit_2              // receive QUIT response, then quit
        };

        Configuration m_config;
        afl::base::ConstBytes_t m_content;
        afl::base::Memory<const String_t> m_to;
        State m_state;
        afl::sys::LogListener& m_log;

        void sendLine(const String_t& line, afl::net::line::LineSink& response);
    };

} } }

#endif
