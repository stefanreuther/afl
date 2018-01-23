/**
  *  \file u/t_net_mimeparser.cpp
  *  \brief Test for afl::net::MimeParser
  */

#include "afl/net/mimeparser.hpp"

#include "t_net.hpp"

/** Test initial state. */
void
TestNetMimeParser::testInit()
{
    afl::net::MimeParser testee;
    TS_ASSERT_EQUALS(testee.getBodyAsString(), "");
    TS_ASSERT_EQUALS(testee.getTrace(), "");
    TS_ASSERT_EQUALS(testee.getFileName().isValid(), false);

    class FailConsumer : public afl::net::HeaderConsumer {
     public:
        virtual void handleHeader(String_t /*name*/, String_t /*value*/)
            { TS_ASSERT(0); }
    };
    FailConsumer fc;
    testee.getHeaders().enumerateHeaders(fc);
}

/** A very simple test. */
void
TestNetMimeParser::testSimple()
{
    afl::net::MimeParser testee;

    // A simple mail, generated by an actual mail program, anonymized
    testee.handleFullData(afl::string::toBytes("From streu@example.org Fri Sep  8 18:31:03 2006\n"
                                               "Date: Fri, 8 Sep 2006 18:31:03 +0200\n"
                                               "From: SR <streu@example.org>\n"
                                               "To: voyager@example.net\n"
                                               "Subject: Re: subject goes here\n"
                                               "Message-ID: <20060908163103.GA1024@royale>\n"
                                               "References: <modmail.0I3A5J.DBN.0.0I3A5J@example.net>\n"
                                               "Mime-Version: 1.0\n"
                                               "Content-Type: text/plain; charset=us-ascii\n"
                                               "Content-Disposition: inline\n"
                                               "In-Reply-To: <modmail.0I3A5J.DBN.0.0I3A5J@example.net>\n"
                                               "User-Agent: Mutt/1.4.2.1i\n"
                                               "Status: RO\n"
                                               "Content-Length: 8\n"
                                               "Lines: 1\n"
                                               "\n"
                                               "approve\n"
                                               "\n"));
    testee.finish();

    // Verify content
    TS_ASSERT_EQUALS(testee.getHeader("To").orElse(""), "voyager@example.net");
    TS_ASSERT_EQUALS(testee.getHeader("content-length").orElse(""), "8");
    TS_ASSERT_EQUALS(testee.getHeader("FROM").orElse(""), "SR <streu@example.org>");
    TS_ASSERT_EQUALS(testee.getBodyAsString(), "approve\n\n");
    TS_ASSERT_EQUALS(testee.getBody().size(), 3U);
    TS_ASSERT_EQUALS(testee.getBody()[0], "approve");
    TS_ASSERT_EQUALS(testee.getBody()[1], "");
    TS_ASSERT_EQUALS(testee.getBody()[2], "");
    TS_ASSERT_EQUALS(testee.getFileName().isValid(), false);
    TS_ASSERT(testee.getParts().get() == 0);
}

/** A minimal example, using LF linefeeds. */
void
TestNetMimeParser::testMinLF()
{
    afl::net::MimeParser testee;

    // Add content
    testee.handleFullData(afl::string::toBytes("From: a\n"
                                               "To: b\n"
                                               "\n"
                                               "content\n"
                                               "more content\n"));
    testee.finish();

    // Verify
    TS_ASSERT_EQUALS(testee.getHeader("from").orElse(""), "a");
    TS_ASSERT_EQUALS(testee.getHeader("to").orElse(""), "b");
    TS_ASSERT_EQUALS(testee.getBodyAsString(), "content\nmore content\n");
}

/** A minimal example, using CRLF linefeeds. */
void
TestNetMimeParser::testMinCRLF()
{
    afl::net::MimeParser testee;

    // Add content
    testee.handleFullData(afl::string::toBytes("From: a\r\n"
                                               "To: b\r\n"
                                               "\r\n"
                                               "content\r\n"
                                               "more content\r\n"));
    testee.finish();

    // Verify
    TS_ASSERT_EQUALS(testee.getHeader("from").orElse(""), "a");
    TS_ASSERT_EQUALS(testee.getHeader("to").orElse(""), "b");
    TS_ASSERT_EQUALS(testee.getBodyAsString(), "content\nmore content\n");
}

/** Test header folding. */
void
TestNetMimeParser::testFoldedHeader()
{
    afl::net::MimeParser testee;

    // Add content [from actual mail]
    testee.handleFullData(afl::string::toBytes("MIME-Version: 1.0\n"
                                               "Content-Type: text/plain;\n"
                                               "        charset=\"iso-8859-1\"\n"
                                               "Content-Transfer-Encoding: 8bit\n"
                                               "X-Hamster-Scoring: Matching lines in mailfilt.hst\n"
                                               " 6: addscore(10) Bytes: %<59750\n"
                                               "\n"
                                               "foo\n"));
    testee.finish();

    // Verify
    // Note that only one the newline is removed, the space is preserved
    TS_ASSERT_EQUALS(testee.getHeader("Content-Type").orElse(""), "text/plain;        charset=\"iso-8859-1\"");
    TS_ASSERT_EQUALS(testee.getHeader("X-Hamster-Scoring").orElse(""), "Matching lines in mailfilt.hst 6: addscore(10) Bytes: %<59750");
}

/** Test getFileName(). */
void
TestNetMimeParser::testGetFileName()
{
    afl::net::MimeParser testee;

    // Add content [from actual mail]
    testee.handleFullData(afl::string::toBytes("Content-Type: application/zip;\n"
                                               "        name=\"player6.zip\"\n"
                                               "Content-Transfer-Encoding: base64\n"
                                               "Content-Disposition: attachment;\n"
                                               "        filename=\"player6.zip\"\n"
                                               "\n"));
    testee.finish();

    // Verify
    TS_ASSERT_EQUALS(testee.getFileName().orElse(""), "player6.zip");
}

/** Test getFileName(). Just a Content-Type header. */
void
TestNetMimeParser::testGetFileName1()
{
    afl::net::MimeParser testee;

    // Add content [from actual mail]
    testee.handleFullData(afl::string::toBytes("Content-Type: application/zip;\n"
                                               "        name=\"player6.zip\"\n"
                                               "\n"));
    testee.finish();

    // Verify
    TS_ASSERT_EQUALS(testee.getFileName().orElse(""), "player6.zip");
}

/** Test getFileName(). Just a Content-Disposition header. */
void
TestNetMimeParser::testGetFileName2()
{
    afl::net::MimeParser testee;

    // Add content [from actual mail]
    testee.handleFullData(afl::string::toBytes("Content-Disposition: attachment;\n"
                                               "        filename=\"player6.zip\"\n"
                                               "\n"));
    testee.finish();

    // Verify
    TS_ASSERT_EQUALS(testee.getFileName().orElse(""), "player6.zip");
}

/** Test quoted-printable body. */
void
TestNetMimeParser::testQPBody()
{
    afl::net::MimeParser testee;

    // Add content [from actual mail, redacted]
    testee.handleFullData(afl::string::toBytes("Content-Type: text/plain;\n"
                                               "        charset=\"iso-8859-1\"\n"
                                               "Content-Transfer-Encoding: quoted-printable\n"
                                               "\n"
                                               "Ich w=FCrde gerne =\n"
                                               "die=20\n"
                                               "Excel-Liste\n"));
    testee.finish();

    // Verify
    // Note that the text is NOT decoded
    TS_ASSERT_EQUALS(testee.getBodyAsString(), "Ich w\xFCrde gerne die \nExcel-Liste\n");
}

/** Test base64 body. */
void
TestNetMimeParser::testBase64Body()
{
    afl::net::MimeParser testee;

    testee.handleFullData(afl::string::toBytes("Content-Type: text/plain; charset=utf-8\n"
                                               "Content-Transfer-Encoding: base64\n"
                                               "\n"
                                               "cG9vZi0sIGEgaGFyc2ggbWV0aG9kIG9mIGVycm9yIGNvcnJlY3Rpb24hDQoNClNvIGl0IGlz\n"));
    testee.finish();

    // Verify
    // Note how \r is preserved.
    TS_ASSERT_EQUALS(testee.getBodyAsString(), "poof-, a harsh method of error correction!\r\n\r\nSo it is");
}

/** Test encoded header. */
void
TestNetMimeParser::testEncodedHeader()
{
    afl::net::MimeParser testee;

    // Data from actual mail (yes, spam).
    testee.handleFullData(afl::string::toBytes("Subject: =?utf-8?Q?Ihr_Handy-Tarif_ab_6,99_=E2=82=AC_mtl.?=\n\n"));
    testee.finish();

    // Verify
    TS_ASSERT_EQUALS(testee.getHeader("Subject").orElse(""), "Ihr Handy-Tarif ab 6,99 \xE2\x82\xAC mtl.");
}

/** Test encoded header, case 2. */
void
TestNetMimeParser::testEncodedHeader2()
{
    afl::net::MimeParser testee;

    // Data from actual mail (yes, spam).
    testee.handleFullData(afl::string::toBytes("Subject: Neu und nur =?UTF-8?Q?f=C3=BCr=20kurze=20Zeit=3A=20Verwandeln=20Sie=20Pun?=\n =?UTF-8?Q?kte=20in=20bares=20Geld=2E?=\n\n"));
    testee.finish();

    // Verify
    TS_ASSERT_EQUALS(testee.getHeader("Subject").orElse(""), "Neu und nur f\xC3\xBCr kurze Zeit: Verwandeln Sie Punkte in bares Geld.");
}

/** Test encoded header, case 3. */
void
TestNetMimeParser::testEncodedHeader3()
{
    afl::net::MimeParser testee;

    // Data from actual mail (yes, spam).
    testee.handleFullData(afl::string::toBytes("Subject: =?iso-8859-1?B?V2lyIHNjaGVua2VuIElobmVuIDEwLC0gRXVybyBm/HIgSWhyZSBu5GNo?=\n"
                                               " =?iso-8859-1?B?c3RlIFJlaXNlIQ==?=\n\n"));
    testee.finish();

    // Verify
    TS_ASSERT_EQUALS(testee.getHeader("Subject").orElse(""), "Wir schenken Ihnen 10,- Euro f\xC3\xBCr Ihre n\xC3\xA4""chste Reise!");
}

/** Test encoded header, case 4. */
void
TestNetMimeParser::testEncodedHeader4()
{
    afl::net::MimeParser testee;

    // Data from actual mail (PlanetsCentral diplomacy).
    testee.handleFullData(afl::string::toBytes("Subject: =?ISO-8859-1?Q?Re=3A_[PlanetsCentral]_Message=3A_Re=3A_Colo_ausr=E4uchern?=\n"
                                               " =?ISO-8859-1?Q?=2E=2E=2E?=\n\n"));
    testee.finish();

    // Verify
    TS_ASSERT_EQUALS(testee.getHeader("Subject").orElse(""), "Re: [PlanetsCentral] Message: Re: Colo ausr\xC3\xA4uchern...");
}

/** Test production of trace headers. */
void
TestNetMimeParser::testTrace()
{
    afl::net::MimeParser testee;

    // Data from actual mail
    testee.handleFullData(afl::string::toBytes("X-RZG-CLASS-ID: mi\n"
                                               "Received: from mout0.freenet.de ([IPv6:2001:748:100:40::2:2])\n"
                                               "        by mailin.rzone.de (josoe mi100) (RZmta 30.13 OK)\n"
                                               "        with ESMTP id p07de6o82HkvDm for <host@planetscentral.com>;\n"
                                               "        Sun, 2 Sep 2012 20:15:38 +0200 (CEST)\n"
                                               "Received: from [195.4.92.141] (helo=mjail1.freenet.de)\n"
                                               "        by mout0.freenet.de with esmtpa (ID s_waesche@freenet.de) (port 25) (Exim 4.76 #1)\n"
                                               "        id 1T8Eht-0007oT-TC\n"
                                               "        for host@planetscentral.com; Sun, 02 Sep 2012 20:15:37 +0200\n"
                                               "Received: from localhost ([::1]:59241 helo=mjail1.freenet.de)\n"
                                               "        by mjail1.freenet.de with esmtpa (ID s_waesche@freenet.de) (Exim 4.76 #1)\n"
                                               "        id 1T8Eht-000751-Jn\n"
                                               "        for host@planetscentral.com; Sun, 02 Sep 2012 20:15:37 +0200\n"
                                               "Received: from [195.4.92.11] (port=47401 helo=1.mx.freenet.de)\n"
                                               "        by mjail1.freenet.de with esmtpa (ID s_waesche@freenet.de) (Exim 4.76 #1)\n"
                                               "        id 1T8Efr-0006Cz-Pa\n"
                                               "        for host@planetscentral.com; Sun, 02 Sep 2012 20:13:31 +0200\n"
                                               "Received: from [117.5.8.17] (port=24142 helo=StefanPC)\n"
                                               "        by 1.mx.freenet.de with esmtpa (ID s_waesche@freenet.de) (port 25) (Exim 4.76 #1)\n"
                                               "        id 1T8Efq-0000BN-RF\n"
                                               "        for host@planetscentral.com; Sun, 02 Sep 2012 20:13:31 +0200\n"
                                               "Content-Language: de\n"
                                               "\n"
                                               "Ok,\n"));
    testee.finish();

    // Verify
    TS_ASSERT_EQUALS(testee.getTrace(), "IPv6:2001:748:100:40::2:2/195.4.92.141/::1/195.4.92.11/117.5.8.17");
}

/** Test header parsing. */
void
TestNetMimeParser::testHeader()
{
    afl::net::MimeParser testee;

    // Test data including Unix mail separator
    testee.handleFullData(afl::string::toBytes("From - Sun Sep 06 00:17:15 2015\n"
                                               "Status: RO\n"
                                               "Lines: 1\n"
                                               "\n"
                                               "hi\n"
                                               "\n"));
    testee.finish();

    // Verify content
    // This must produce exactly two headers
    class TestConsumer : public afl::net::HeaderConsumer {
     public:
        virtual void handleHeader(String_t name, String_t value)
            {
                TS_ASSERT_EQUALS(m_stuff.find(name), m_stuff.end());
                m_stuff[name] = value;
            }
        std::map<String_t, String_t> m_stuff;
    };

    TestConsumer tc;
    testee.getHeaders().enumerateHeaders(tc);
    TS_ASSERT_EQUALS(tc.m_stuff.size(), 2U);
    TS_ASSERT_EQUALS(tc.m_stuff["Status"], "RO");
    TS_ASSERT_EQUALS(tc.m_stuff["Lines"], "1");
}

/** Test clear(). */
void
TestNetMimeParser::testClear()
{
    afl::net::MimeParser testee;

    // Pushing incomplete data, then clear
    testee.handleFullData(afl::string::toBytes("From: me\nHi"));
    testee.clear();
    TS_ASSERT_EQUALS(testee.getBodyAsString(), "");
    TS_ASSERT_EQUALS(testee.getTrace(), "");

    // Pushing complete data. Must not continue the old data, but start new one.
    testee.handleFullData(afl::string::toBytes("Subject: test\n\nbody\n"));
    testee.finish();

    // Verify
    TS_ASSERT(testee.getHeaders().get("From") == 0);
    TS_ASSERT(testee.getHeaders().get("Subject") != 0);
    TS_ASSERT_EQUALS(testee.getHeaders().get("Subject")->getValue(), "test");
    TS_ASSERT_EQUALS(testee.getBodyAsString(), "body\n");
}

/** Test incomplete message. */
void
TestNetMimeParser::testIncomplete()
{
    afl::net::MimeParser testee;

    // Pushing incomplete data, then finish
    testee.handleFullData(afl::string::toBytes("From: me\nTo: you"));
    testee.finish();

    // Result must be two headers
    TS_ASSERT_EQUALS(testee.getHeader("From").orElse(""), "me");
    TS_ASSERT_EQUALS(testee.getHeader("To").orElse(""), "you");
}

/** Test simple multipart message. */
void
TestNetMimeParser::testMultipart()
{
    afl::net::MimeParser testee;

    // Add content [actual mail, slightly shortened]
    testee.handleFullData(afl::string::toBytes("From - Mon Feb 09 20:11:45 2004\n"
                                               "X-UIDL: c08p7e.ak.1\n"
                                               "Received: (qmail 31185 invoked by uid 65534); 9 Feb 2004 16:12:30 -0000\n"
                                               "Received: from mail.inf.tu-dresden.de (EHLO mail.inf.tu-dresden.de) (141.76.2.1)\n"
                                               "  by mx0.gmx.net (mx043) with SMTP; 09 Feb 2004 17:12:30 +0100\n"
                                               "Received: from irz750.frz.inf.tu-dresden.de (irz750.inf.tu-dresden.de [141.76.7.50])\n"
                                               "        by mail.inf.tu-dresden.de (8.12.10/8.12.10) with SMTP id i19GCQgT019288\n"
                                               "        for <streu@gmx.de>; Mon, 9 Feb 2004 17:12:28 +0100 (MET)\n"
                                               "Received: (nullmailer pid 6612 invoked by uid 2415);\n"
                                               "        Mon, 09 Feb 2004 16:12:26 -0000\n"
                                               "Date: Mon, 9 Feb 2004 17:12:25 +0100\n"
                                               "To: streu@gmx.de\n"
                                               "Mime-Version: 1.0\n"
                                               "Content-Type: multipart/mixed; boundary=\"xHFwDpU9dbj6ez1V\"\n"
                                               "Content-Disposition: inline\n"
                                               "User-Agent: Mutt/1.3.28i\n"
                                               "\n"
                                               "\n"
                                               "--xHFwDpU9dbj6ez1V\n"
                                               "Content-Type: text/plain; charset=us-ascii\n"
                                               "Content-Disposition: inline\n"
                                               "\n"
                                               "...\n"
                                               "\n"
                                               "--xHFwDpU9dbj6ez1V\n"
                                               "Content-Type: application/octet-stream\n"
                                               "Content-Disposition: attachment; filename=\"posted.gz\"\n"
                                               "Content-Transfer-Encoding: base64\n"
                                               "\n"
                                               "H4sIAEaxJ0ACA9xbW1PcxrZ+16/oolx7wJkRc4Vh4hCwgUBsbI4hsWvvpJLWqKVpj6Se3S0x\n"
                                               "\n"
                                               "--xHFwDpU9dbj6ez1V--\n"
                                               "\n"
                                               "\n"));
    testee.finish();

    // Verify content
    TS_ASSERT_EQUALS(testee.getHeader("From").isValid(), false);
    TS_ASSERT_EQUALS(testee.getHeader("To").orElse(""), "streu@gmx.de");

    afl::base::Ptr<afl::base::Enumerator<afl::net::MimeParser> > parts(testee.getParts());
    TS_ASSERT(parts.get() != 0);

    // Verify parts
    // - part 1
    afl::net::MimeParser p;
    TS_ASSERT_EQUALS(parts->getNextElement(p), true);
    TS_ASSERT_EQUALS(p.getHeader("Content-Type").orElse(""), "text/plain; charset=us-ascii");
    TS_ASSERT_EQUALS(p.getBodyAsString(), "...\n");

    // - part 2
    TS_ASSERT_EQUALS(parts->getNextElement(p), true);
    TS_ASSERT_EQUALS(p.getHeader("Content-Type").orElse(""), "application/octet-stream");
    TS_ASSERT_EQUALS(p.getFileName().orElse(""), "posted.gz");
    TS_ASSERT_EQUALS(p.getBodyAsString().substr(0, 10), String_t("\x1F\x8B\x08\x00\x46\xB1\x27\x40\x02\x03", 10));

    // end
    TS_ASSERT_EQUALS(parts->getNextElement(p), false);
}

/** Test bad invocation. */
void
TestNetMimeParser::testMultipartBad()
{
    afl::net::MimeParser testee;

    // Add content
    testee.handleFullData(afl::string::toBytes("Content-Type: multipart/mixed; boundary=\"xHFwDpU9dbj6ez1V\"\n"
                                               "Content-Disposition: inline\n"
                                               "\n"
                                               "--xHFwDpU9dbj6ez1V\n"
                                               "Content-Type: text/plain; charset=us-ascii\n"
                                               "Content-Disposition: inline\n"
                                               "\n"
                                               "...\n"
                                               "\n"
                                               "--xHFwDpU9dbj6ez1V--\n"
                                               "\n"
                                               "\n"));
    testee.finish();

    // Obtain iterator
    afl::base::Ptr<afl::base::Enumerator<afl::net::MimeParser> > parts(testee.getParts());
    TS_ASSERT(parts.get() != 0);

    // Invalid invocation is immediately rejected.
    // You cannot pass the origin MimeParser as the target.
    TS_ASSERT_EQUALS(parts->getNextElement(testee), false);
}

/** Test invocation on a multipart parser that is modified inbetween. */
void
TestNetMimeParser::testMultipartModif()
{
    afl::net::MimeParser testee;

    // Add content
    testee.handleFullData(afl::string::toBytes("Content-Type: multipart/mixed; boundary=\"xHFwDpU9dbj6ez1V\"\n"
                                               "Content-Disposition: inline\n"
                                               "\n"
                                               "--xHFwDpU9dbj6ez1V\n"
                                               "Content-Type: text/plain; charset=us-ascii\n"
                                               "Content-Disposition: inline\n"
                                               "\n"
                                               "...\n"
                                               "\n"
                                               "--xHFwDpU9dbj6ez1V--\n"
                                               "\n"
                                               "\n"));
    testee.finish();

    // Obtain iterator
    afl::base::Ptr<afl::base::Enumerator<afl::net::MimeParser> > parts(testee.getParts());
    TS_ASSERT(parts.get() != 0);

    // Clear the origin parser
    testee.clear();

    // Part retrieval is rejected (no crash)
    afl::net::MimeParser p;
    TS_ASSERT_EQUALS(parts->getNextElement(p), false);
}

/** Test multipart handling, missing content. */
void
TestNetMimeParser::testMultipartMissing()
{
    afl::net::MimeParser testee;

    // Add content
    testee.handleFullData(afl::string::toBytes("Content-Type: multipart/mixed; boundary=\"xHFwDpU9dbj6ez1V\"\n"
                                               "Content-Disposition: inline\n"
                                               "\n"
                                               "...\n"));
    testee.finish();

    // Obtain iterator
    afl::base::Ptr<afl::base::Enumerator<afl::net::MimeParser> > parts(testee.getParts());
    TS_ASSERT(parts.get() != 0);

    // Reports empty
    afl::net::MimeParser p;
    TS_ASSERT_EQUALS(parts->getNextElement(p), false);
}

/** Test handling of embedded messages. */
void
TestNetMimeParser::testMessage()
{
    afl::net::MimeParser testee;

    // Add content
    testee.handleFullData(afl::string::toBytes("Content-Type: message/rfc822\n"
                                               "Content-Disposition: inline\n"
                                               "To: a@host\n"
                                               "\n"
                                               "To: b@host\n"
                                               "Subject: test\n"
                                               "\n"
                                               "text\n"));
    testee.finish();

    // Verify
    TS_ASSERT_EQUALS(testee.getHeader("to").orElse(""), "a@host");

    // Obtain iterator
    afl::base::Ptr<afl::base::Enumerator<afl::net::MimeParser> > parts(testee.getParts());
    TS_ASSERT(parts.get() != 0);
    
    // Reports one part
    afl::net::MimeParser p;
    TS_ASSERT_EQUALS(parts->getNextElement(p), true);
    TS_ASSERT_EQUALS(p.getHeader("to").orElse(""), "b@host");
    TS_ASSERT_EQUALS(p.getBodyAsString(), "text\n");
}
