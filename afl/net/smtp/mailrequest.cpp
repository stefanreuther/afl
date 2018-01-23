/**
  *  \file afl/net/smtp/mailrequest.cpp
  *  \brief Class afl::net::smtp::MailRequest
  *
  *  Specification References:
  *
  *  RFC  821  SMTP
  *  RFC  822  Mail message format
  *  RFC 1321  MD5
  *  RFC 1869  ESMTP
  *              - specifies the EHLO command, and responses of the form
  *                         250-servername ESMTP hi there
  *                         250-FOO
  *                         250-BAR
  *                         250 HUHU
  *  RFC 2104  HMAC-MD5
  *  RFC 2195  CRAM-MD5
  *  RFC 2554  SMTP-Auth
  *  RFC 2595  AUTH PLAIN
  */

#include "afl/net/smtp/mailrequest.hpp"
#include "afl/except/remoteerrorexception.hpp"
#include "afl/net/line/linesink.hpp"
#include "afl/string/messages.hpp"

// Constructor.
afl::net::smtp::MailRequest::MailRequest(const Configuration& config,
                                         afl::base::Memory<const String_t> to,
                                         afl::base::ConstBytes_t content)
    : m_config(config),
      m_content(content),
      m_to(to),
      m_state(Greeting_2)
{
    // If the content ends in \r\n (which it should), remove that to avoid it getting doubled.
    const uint8_t* p;
    if ((p = m_content.atEnd(0)) != 0 && *p == '\n') {
        m_content.removeEnd(1);
        if ((p = m_content.atEnd(0)) != 0 && *p == '\r') {
            m_content.removeEnd(1);
        }
    }
}

afl::net::smtp::MailRequest::~MailRequest()
{ }

bool
afl::net::smtp::MailRequest::handleOpening(afl::net::line::LineSink& /*response*/)
{
    // Nothing to do, server talks first
    return false;
}

bool
afl::net::smtp::MailRequest::handleLine(const String_t& line, afl::net::line::LineSink& response)
{
    bool finish = false;
    if (line.size() >= 4 && line[3] != '-') {
        // It's a line that looks like the final line of a reply
        char expected = '?';
        switch (m_state) {
         case Greeting_2:
         case Hello_2:
         case From_2:
         case Recipient_2:
         case Content_2:
         case Quit_2:
             expected = '2';
             break;
         case Data_3:
             expected = '3';
             break;
        }
        if (line[0] != expected) {
            throw afl::except::RemoteErrorException("<smtp>", line);
        }

        // OK, send next data
        switch (m_state) {
         case Greeting_2:       // receive greeting, "2xx", then send HELO
            if (m_to.empty()) {
                // Error case, no recipient given
                response.handleLine("QUIT");
                m_state = Quit_2;
            } else {
                response.handleLine("HELO " + m_config.hello);
                m_state = Hello_2;
            }
            break;

         case Hello_2:          // receive HELO response, "2xx", then send MAIL FROM
            response.handleLine("MAIL FROM:<" + m_config.from + ">");
            m_state = From_2;
            break;

         case From_2:           // receive MAIL FROM or RCPT TO response, "2xx", then send RCPT TO
            response.handleLine("RCPT TO:<" + *m_to.eat() + ">");
            if (m_to.empty()) {
                m_state = Recipient_2;
            }
            break;

         case Recipient_2:      // receive final RCPT TO response, "2xx", then send DATA
            response.handleLine("DATA");
            m_state = Data_3;
            break;

         case Data_3:           // receive DATA response, "3xx", then send content
            response.handleLine(afl::string::fromBytes(m_content));
            response.handleLine(".");
            m_state = Content_2;
            break;

         case Content_2:        // receive content response, "2xx", then send quit
            response.handleLine("QUIT");
            m_state = Quit_2;
            break;

         case Quit_2:           // receive QUIT response, then quit
            finish = true;
            break;
        }
    }
    return finish;
}

void
afl::net::smtp::MailRequest::handleConnectionClose()
{
    switch (m_state) {
     case Greeting_2:
     case Hello_2:
     case From_2:
     case Recipient_2:
     case Data_3:
     case Content_2:
        throw afl::except::RemoteErrorException("<smtp>", afl::string::Messages::networkConnectionLost());

     case Quit_2:
        break;
    }
}
