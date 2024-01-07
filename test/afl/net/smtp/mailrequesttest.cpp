/**
  *  \file test/afl/net/smtp/mailrequesttest.cpp
  *  \brief Test for afl::net::smtp::MailRequest
  */

#include "afl/net/smtp/mailrequest.hpp"

#include "afl/except/fileproblemexception.hpp"
#include "afl/except/remoteerrorexception.hpp"
#include "afl/net/line/linesink.hpp"
#include "afl/sys/log.hpp"
#include "afl/test/testrunner.hpp"

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
AFL_TEST("afl.net.smtp.MailRequest:simple", a)
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
    a.check("opening", !testee.handleOpening(mock));
    a.checkEqual("opening text", mock.extract(), "");

    a.check("helo", !testee.handleLine("220 ready", mock));
    a.checkEqual("helo text", mock.extract(), "HELO h\r\n");

    a.check("mail", !testee.handleLine("250 ok", mock));
    a.checkEqual("mail text", mock.extract(), "MAIL FROM:<f@ro.m>\r\n");

    a.check("rcpt", !testee.handleLine("250 ok", mock));
    a.checkEqual("rcpt text", mock.extract(), "RCPT TO:<t@o>\r\n");

    a.check("data", !testee.handleLine("250 accepted", mock));
    a.checkEqual("data text", mock.extract(), "DATA\r\n");

    a.check("message", !testee.handleLine("354 enter message", mock));
    a.checkEqual("message text", mock.extract(),
                 "From: body@from\r\n"
                 "To: body@to\r\n"
                 "\r\n"
                 "Content\r\n"
                 ".\r\n");

    a.check("quit", !testee.handleLine("250 ok", mock));
    a.checkEqual("quit text", mock.extract(), "QUIT\r\n");

    a.check("closing", testee.handleLine("221 bye", mock));
}

/** Test request with no receiver (error case).
    This should execute a single QUIT request. */
AFL_TEST("afl.net.smtp.MailRequest:no-receiver", a)
{
    // Testee
    afl::sys::Log log;
    afl::net::smtp::Configuration config("h", "f@ro.m");
    afl::net::smtp::MailRequest testee(config, afl::base::Nothing, afl::string::toBytes("egal"), log);

    // Operate
    LineSinkMock mock;
    a.check("opening", !testee.handleOpening(mock));
    a.checkEqual("opening text", mock.extract(), "");

    a.check("quit", !testee.handleLine("220 ready", mock));
    a.checkEqual("quit text", mock.extract(), "QUIT\r\n");
    a.check("closing", testee.handleLine("221 bye", mock));
}

/** Test variant: multiple receivers, body does not end in CRLF.
    The protocol must be executed correctly, with all receivers being sent in order,
    and a CRLF added to the mail content. */
AFL_TEST("afl.net.smtp.MailRequest:variant", a)
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
    a.check("opening", !testee.handleOpening(mock));
    a.checkEqual("opening text", mock.extract(), "");

    a.check("helo", !testee.handleLine("220 ready", mock));
    a.checkEqual("helo text", mock.extract(), "HELO h\r\n");

    a.check("mail", !testee.handleLine("250 ok", mock));
    a.checkEqual("mail text", mock.extract(), "MAIL FROM:<f@ro.m>\r\n");

    a.check("rcpt1", !testee.handleLine("250 ok", mock));
    a.checkEqual("rcpt1 text", mock.extract(), "RCPT TO:<tx@one>\r\n");

    a.check("rcpt2", !testee.handleLine("250 ok", mock));
    a.checkEqual("rcpt2 text", mock.extract(), "RCPT TO:<tx@two>\r\n");

    a.check("rcpt3", !testee.handleLine("250 ok", mock));
    a.checkEqual("rcpt3 text", mock.extract(), "RCPT TO:<tx@three>\r\n");

    a.check("data", !testee.handleLine("250 accepted", mock));
    a.checkEqual("data text", mock.extract(), "DATA\r\n");

    a.check("body", !testee.handleLine("354 enter message", mock));
    a.checkEqual("body text", mock.extract(),
                     "From: body@from\r\n"
                     "To: body@to\r\n"
                     "\r\n"
                     "Content\r\n"
                     ".\r\n");

    a.check("quit", !testee.handleLine("250 ok", mock));
    a.checkEqual("quit text", mock.extract(), "QUIT\r\n");

    a.check("closing", testee.handleLine("221 bye", mock));
}

/** Test closing connection.
    Simulate the server closing the connection in the middle of the transaction.
    This should report an exception. */
AFL_TEST("afl.net.smtp.MailRequest:connection-loss", a)
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
    a.check("opening", !testee.handleOpening(mock));
    a.checkEqual("opening text", mock.extract(), "");

    a.check("helo", !testee.handleLine("220 ready", mock));
    a.checkEqual("helo text", mock.extract(), "HELO h\r\n");

    a.check("mail", !testee.handleLine("250 ok", mock));
    a.checkEqual("mail text", mock.extract(), "MAIL FROM:<f@ro.m>\r\n");

    // Pretend connection loss here
    AFL_CHECK_THROWS(a("connection close"), testee.handleConnectionClose(), afl::except::FileProblemException);
}

/** Test multiline greeting.
    Perform a null transaction and exercise multiline responses. */
AFL_TEST("afl.net.smtp.MailRequest:multiline-greeting", a)
{
    // Testee
    afl::sys::Log log;
    afl::net::smtp::Configuration config("h", "f@ro.m");
    afl::net::smtp::MailRequest testee(config, afl::base::Nothing, afl::string::toBytes("egal"), log);

    // Operate
    LineSinkMock mock;
    a.check("opening", !testee.handleOpening(mock));
    a.checkEqual("opening text", mock.extract(), "");

    a.check("greeting 1", !testee.handleLine("220-Hi", mock));
    a.check("greeting 2", !testee.handleLine("220-I'm talkative today.", mock));
    a.check("greeting 3", !testee.handleLine("220-One more line.", mock));
    a.checkEqual("greeting text", mock.extract(), "");
    a.check("quit", !testee.handleLine("220 Now I'm ready.", mock));
    a.checkEqual("quit text", mock.extract(), "QUIT\r\n");
    a.check("closing 1", !testee.handleLine("221-Oh, you're going?", mock));
    a.check("closing 2", !testee.handleLine("221-*sniff*", mock));
    a.checkEqual("closing text", mock.extract(), "");
    a.check("closing 3", testee.handleLine("221 bye", mock));
    a.checkEqual("final text", mock.extract(), "");
}

/** Test closing connection.
    Simulate the server closing the connection immediately after QUIT.
    Although this is a protocol violation, don't treat it as an error. */
AFL_TEST("afl.net.smtp.MailRequest:premature-close", a)
{
    // Testee
    afl::sys::Log log;
    afl::net::smtp::Configuration config("h", "f@ro.m");
    afl::net::smtp::MailRequest testee(config, afl::base::Nothing, afl::string::toBytes("egal"), log);

    // Operate
    LineSinkMock mock;
    a.check("opening", !testee.handleOpening(mock));
    a.checkEqual("opening text", mock.extract(), "");

    a.check("quit", !testee.handleLine("220 ready", mock));
    a.checkEqual("quit text", mock.extract(), "QUIT\r\n");

    AFL_CHECK_SUCCEEDS(a("connection close before quit confirmation"), testee.handleConnectionClose());
}

/** Test error response.
    If the server does not respond with the expected return code class,
    this is reported as an exception. */
AFL_TEST("afl.net.smtp.MailRequest:error-response", a)
{
    // Testee
    afl::sys::Log log;
    afl::net::smtp::Configuration config("h", "f@ro.m");
    const String_t to[] = {"t"};
    afl::net::smtp::MailRequest testee(config, to, afl::string::toBytes("egal"), log);

    // Operate
    LineSinkMock mock;
    a.check("opening", !testee.handleOpening(mock));
    a.checkEqual("opening text", mock.extract(), "");

    a.check("helo", !testee.handleLine("220 ready", mock));
    a.checkEqual("helo text", mock.extract(), "HELO h\r\n");

    a.check("mail", !testee.handleLine("250 ok", mock));
    a.checkEqual("mail text", mock.extract(), "MAIL FROM:<f@ro.m>\r\n");

    a.check("rcpt", !testee.handleLine("250 ok", mock));
    a.checkEqual("rcpt text", mock.extract(), "RCPT TO:<t>\r\n");

    AFL_CHECK_THROWS(a("error respons"), testee.handleLine("501 invalid address", mock), afl::except::RemoteErrorException);
}
