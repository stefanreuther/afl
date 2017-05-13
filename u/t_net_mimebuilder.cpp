/**
  *  \file u/t_net_mimebuilder.cpp
  *  \brief Test for afl::net::MimeBuilder
  */

#include "afl/net/mimebuilder.hpp"

#include "t_net.hpp"
#include "afl/io/internalsink.hpp"

/** Simple test. */
void
TestNetMimeBuilder::testSimple()
{
    // Build something
    afl::net::MimeBuilder testee("multipart/mixed");
    testee.addHeader("Name", "nobody");
    testee.addBoundary();
    testee.addHeader("Section", "one");
    testee.addLine("First section.");
    testee.addBoundary();
    testee.addHeader("Section", "two");
    testee.addLine("Second section.");
    testee.addBoundary();
    testee.finish();

    // Verify it
    afl::io::InternalSink sink;
    testee.write(sink, false);

    const char RESULT[] =
        "Content-Type: multipart/mixed; boundary=000\r\n"
        "Name: nobody\r\n"
        "\r\n"
        "--000\r\n"
        "Section: one\r\n"
        "\r\n"
        "First section.\r\n"
        "--000\r\n"
        "Section: two\r\n"
        "\r\n"
        "Second section.\r\n"
        "--000--\r\n";
    TS_ASSERT_EQUALS(testee.getTotalSize(), sizeof(RESULT)-1);
    TS_ASSERT_EQUALS(sink.getContent().size(), sizeof(RESULT)-1);
    TS_ASSERT_EQUALS(afl::string::fromBytes(sink.getContent()), RESULT);
}

/** Base64 test. */
void
TestNetMimeBuilder::testBase64()
{
    afl::net::MimeBuilder testee("multipart/mixed");
    testee.addHeader("Name", "nobody");
    testee.addBoundary();
    testee.addHeader("Section", "one");
    static const uint8_t seven[] = {0,0,0,0,0,0,0};
    for (int i = 0; i < 20; ++i) {
        testee.addBase64(seven);
    }
    testee.addBoundary();
    testee.finish();

    // Verify it
    afl::io::InternalSink sink;
    testee.write(sink, false);

    const char RESULT[] =
        "Content-Type: multipart/mixed; boundary=000\r\n"
        "Name: nobody\r\n"
        "\r\n"
        "--000\r\n"
        "Section: one\r\n"
        "\r\n"
        // 7x20 = 140 bytes; that's 2x 57 + 26 characters. 57 raw = 76 encoded.
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n"
        // 26 raw = 8 triplets + one partial triplet.
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=\r\n"
        "--000--\r\n";
    TS_ASSERT_EQUALS(testee.getTotalSize(), sizeof(RESULT)-1);
    TS_ASSERT_EQUALS(sink.getContent().size(), sizeof(RESULT)-1);
    TS_ASSERT_EQUALS(afl::string::fromBytes(sink.getContent()), RESULT);
}

/** Text contains potential boundary. */
void
TestNetMimeBuilder::testBoundaryConflict()
{
    // Build something
    afl::net::MimeBuilder testee("multipart/mixed");
    testee.addHeader("Name", "nobody");
    testee.addBoundary();
    testee.addLine("--000");
    testee.addLine("--200");
    testee.addLine("--100");
    testee.addBoundary();
    testee.finish();

    // Verify it
    afl::io::InternalSink sink;
    testee.write(sink, false);

    const char RESULT[] =
        "Content-Type: multipart/mixed; boundary=300\r\n"
        "Name: nobody\r\n"
        "\r\n"
        "--300\r\n"
        "\r\n"
        "--000\r\n"
        "--200\r\n"
        "--100\r\n"
        "--300--\r\n";
    TS_ASSERT_EQUALS(testee.getTotalSize(), sizeof(RESULT)-1);
    TS_ASSERT_EQUALS(sink.getContent().size(), sizeof(RESULT)-1);
    TS_ASSERT_EQUALS(afl::string::fromBytes(sink.getContent()), RESULT);
}

