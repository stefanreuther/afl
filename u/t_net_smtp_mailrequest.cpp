/**
  *  \file u/t_net_smtp_mailrequest.cpp
  *  \brief Test for afl::net::smtp::MailRequest
  */

#include "afl/net/smtp/mailrequest.hpp"

#include "t_net_smtp.hpp"
#include "afl/net/line/linesink.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/except/remoteerrorexception.hpp"
#include "afl/sys/log.hpp"

namespace {
   class LineSinkMock : public afl::net::line::LineSink {
     public:
        virtual void handleLine(const String_t& line)
           {
               m_content += line;
               m_content += "\r\n";
           }
       String_t extract()
           {
               String_t result = m_content;
               m_content.clear();
               return result;
           }
    private:
       String_t m_content;
   };
}

/** Test simple mail request.
    Performs a regular, one-receiver mail transaction.
    The protocol must execute as expected. */
void
TestNetSmtpMailRequest::testSimple()
{
    // Testee
    afl::sys::Log log;
    afl::net::smtp::Configuration config("h", "f@ro.m");
    const String_t to[] = {"t@o"};
    afl::net::smtp::MailRequest testee(config, to, afl::string::toBytes("From: body@from\r\n"
                                                                        "To: body@to\r\n"
                                                                        "\r\n"
                                                                        "Content\r\n"), log);

    // Operate
    LineSinkMock mock;
    TS_ASSERT(!testee.handleOpening(mock));
    TS_ASSERT_EQUALS(mock.extract(), "");

    TS_ASSERT(!testee.handleLine("220 ready", mock));
    TS_ASSERT_EQUALS(mock.extract(), "HELO h\r\n");

    TS_ASSERT(!testee.handleLine("250 ok", mock));
    TS_ASSERT_EQUALS(mock.extract(), "MAIL FROM:<f@ro.m>\r\n");

    TS_ASSERT(!testee.handleLine("250 ok", mock));
    TS_ASSERT_EQUALS(mock.extract(), "RCPT TO:<t@o>\r\n");

    TS_ASSERT(!testee.handleLine("250 accepted", mock));
    TS_ASSERT_EQUALS(mock.extract(), "DATA\r\n");

    TS_ASSERT(!testee.handleLine("354 enter message", mock));
    TS_ASSERT_EQUALS(mock.extract(),
                     "From: body@from\r\n"
                     "To: body@to\r\n"
                     "\r\n"
                     "Content\r\n"
                     ".\r\n");

    TS_ASSERT(!testee.handleLine("250 ok", mock));
    TS_ASSERT_EQUALS(mock.extract(), "QUIT\r\n");

    TS_ASSERT(testee.handleLine("221 bye", mock));
}

/** Test request with no receiver (error case).
    This should execute a single QUIT request. */
void
TestNetSmtpMailRequest::testNoReceiver()
{
    // Testee
    afl::sys::Log log;
    afl::net::smtp::Configuration config("h", "f@ro.m");
    afl::net::smtp::MailRequest testee(config, afl::base::Nothing, afl::string::toBytes("egal"), log);

    // Operate
    LineSinkMock mock;
    TS_ASSERT(!testee.handleOpening(mock));
    TS_ASSERT_EQUALS(mock.extract(), "");

    TS_ASSERT(!testee.handleLine("220 ready", mock));
    TS_ASSERT_EQUALS(mock.extract(), "QUIT\r\n");
    TS_ASSERT(testee.handleLine("221 bye", mock));
}

/** Test variant: multiple receivers, body does not end in CRLF.
    The protocol must be executed correctly, with all receivers being sent in order,
    and a CRLF added to the mail content. */
void
TestNetSmtpMailRequest::testVariant()
{
    // Testee
    afl::sys::Log log;
    afl::net::smtp::Configuration config("h", "f@ro.m");
    const String_t to[] = {"tx@one","tx@two","tx@three"};
    afl::net::smtp::MailRequest testee(config, to, afl::string::toBytes("From: body@from\r\n"
                                                                        "To: body@to\r\n"
                                                                        "\r\n"
                                                                        "Content"), log);

    // Operate
    LineSinkMock mock;
    TS_ASSERT(!testee.handleOpening(mock));
    TS_ASSERT_EQUALS(mock.extract(), "");

    TS_ASSERT(!testee.handleLine("220 ready", mock));
    TS_ASSERT_EQUALS(mock.extract(), "HELO h\r\n");

    TS_ASSERT(!testee.handleLine("250 ok", mock));
    TS_ASSERT_EQUALS(mock.extract(), "MAIL FROM:<f@ro.m>\r\n");

    TS_ASSERT(!testee.handleLine("250 ok", mock));
    TS_ASSERT_EQUALS(mock.extract(), "RCPT TO:<tx@one>\r\n");

    TS_ASSERT(!testee.handleLine("250 ok", mock));
    TS_ASSERT_EQUALS(mock.extract(), "RCPT TO:<tx@two>\r\n");

    TS_ASSERT(!testee.handleLine("250 ok", mock));
    TS_ASSERT_EQUALS(mock.extract(), "RCPT TO:<tx@three>\r\n");

    TS_ASSERT(!testee.handleLine("250 accepted", mock));
    TS_ASSERT_EQUALS(mock.extract(), "DATA\r\n");

    TS_ASSERT(!testee.handleLine("354 enter message", mock));
    TS_ASSERT_EQUALS(mock.extract(),
                     "From: body@from\r\n"
                     "To: body@to\r\n"
                     "\r\n"
                     "Content\r\n"
                     ".\r\n");

    TS_ASSERT(!testee.handleLine("250 ok", mock));
    TS_ASSERT_EQUALS(mock.extract(), "QUIT\r\n");

    TS_ASSERT(testee.handleLine("221 bye", mock));
}

/** Test closing connection.
    Simulate the server closing the connection in the middle of the transaction.
    This should report an exception. */
void
TestNetSmtpMailRequest::testClose()
{
    // Testee
    afl::sys::Log log;
    afl::net::smtp::Configuration config("h", "f@ro.m");
    const String_t to[] = {"t@o"};
    afl::net::smtp::MailRequest testee(config, to, afl::string::toBytes("From: body@from\r\n"
                                                                        "To: body@to\r\n"
                                                                        "\r\n"
                                                                        "Content\r\n"), log);

    // Operate
    LineSinkMock mock;
    TS_ASSERT(!testee.handleOpening(mock));
    TS_ASSERT_EQUALS(mock.extract(), "");

    TS_ASSERT(!testee.handleLine("220 ready", mock));
    TS_ASSERT_EQUALS(mock.extract(), "HELO h\r\n");

    TS_ASSERT(!testee.handleLine("250 ok", mock));
    TS_ASSERT_EQUALS(mock.extract(), "MAIL FROM:<f@ro.m>\r\n");

    // Pretend connection loss here
    TS_ASSERT_THROWS(testee.handleConnectionClose(), afl::except::FileProblemException);
}

/** Test multiline greeting.
    Perform a null transaction and exercise multiline responses. */
void
TestNetSmtpMailRequest::testMultilineGreeting()
{
    // Testee
    afl::sys::Log log;
    afl::net::smtp::Configuration config("h", "f@ro.m");
    afl::net::smtp::MailRequest testee(config, afl::base::Nothing, afl::string::toBytes("egal"), log);

    // Operate
    LineSinkMock mock;
    TS_ASSERT(!testee.handleOpening(mock));
    TS_ASSERT_EQUALS(mock.extract(), "");

    TS_ASSERT(!testee.handleLine("220-Hi", mock));
    TS_ASSERT(!testee.handleLine("220-I'm talkative today.", mock));
    TS_ASSERT(!testee.handleLine("220-One more line.", mock));
    TS_ASSERT_EQUALS(mock.extract(), "");
    TS_ASSERT(!testee.handleLine("220 Now I'm ready.", mock));
    TS_ASSERT_EQUALS(mock.extract(), "QUIT\r\n");
    TS_ASSERT(!testee.handleLine("221-Oh, you're going?", mock));
    TS_ASSERT(!testee.handleLine("221-*sniff*", mock));
    TS_ASSERT_EQUALS(mock.extract(), "");
    TS_ASSERT(testee.handleLine("221 bye", mock));
    TS_ASSERT_EQUALS(mock.extract(), "");
}

/** Test closing connection.
    Simulate the server closing the connection immediately after QUIT.
    Although this is a protocol violation, don't treat it as an error. */
void
TestNetSmtpMailRequest::testPrematureClose()
{
    // Testee
    afl::sys::Log log;
    afl::net::smtp::Configuration config("h", "f@ro.m");
    afl::net::smtp::MailRequest testee(config, afl::base::Nothing, afl::string::toBytes("egal"), log);

    // Operate
    LineSinkMock mock;
    TS_ASSERT(!testee.handleOpening(mock));
    TS_ASSERT_EQUALS(mock.extract(), "");

    TS_ASSERT(!testee.handleLine("220 ready", mock));
    TS_ASSERT_EQUALS(mock.extract(), "QUIT\r\n");

    TS_ASSERT_THROWS_NOTHING(testee.handleConnectionClose());
}

/** Test error response.
    If the server does not respond with the expected return code class,
    this is reported as an exception. */
void
TestNetSmtpMailRequest::testErrorResponse()
{
    // Testee
    afl::sys::Log log;
    afl::net::smtp::Configuration config("h", "f@ro.m");
    const String_t to[] = {"t"};
    afl::net::smtp::MailRequest testee(config, to, afl::string::toBytes("egal"), log);

    // Operate
    LineSinkMock mock;
    TS_ASSERT(!testee.handleOpening(mock));
    TS_ASSERT_EQUALS(mock.extract(), "");

    TS_ASSERT(!testee.handleLine("220 ready", mock));
    TS_ASSERT_EQUALS(mock.extract(), "HELO h\r\n");

    TS_ASSERT(!testee.handleLine("250 ok", mock));
    TS_ASSERT_EQUALS(mock.extract(), "MAIL FROM:<f@ro.m>\r\n");

    TS_ASSERT(!testee.handleLine("250 ok", mock));
    TS_ASSERT_EQUALS(mock.extract(), "RCPT TO:<t>\r\n");

    TS_ASSERT_THROWS(testee.handleLine("501 invalid address", mock), afl::except::RemoteErrorException);
}

