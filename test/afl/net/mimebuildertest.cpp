/**
  *  \file test/afl/net/mimebuildertest.cpp
  *  \brief Test for afl::net::MimeBuilder
  */

#include "afl/net/mimebuilder.hpp"

#include "afl/io/internalsink.hpp"
#include "afl/net/headertable.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.net.MimeBuilder:simple", a)
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
    String_t b = testee.finish();

    // Verify it
    afl::io::InternalSink sink;
    testee.write(sink, false);

    static const char RESULT[] =
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
    a.checkEqual("boundary", b, "000");
    a.checkEqual("getTotalSize()", testee.getTotalSize(), sizeof(RESULT)-1);
    a.checkEqual("getContent() size", sink.getContent().size(), sizeof(RESULT)-1);
    a.checkEqual("content", afl::string::fromBytes(sink.getContent()), RESULT);
}

AFL_TEST("afl.net.MimeBuilder:addBase64", a)
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

    static const char RESULT[] =
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
    a.checkEqual("getTotalSize()", testee.getTotalSize(), sizeof(RESULT)-1);
    a.checkEqual("getContent() size", sink.getContent().size(), sizeof(RESULT)-1);
    a.checkEqual("content", afl::string::fromBytes(sink.getContent()), RESULT);
}

/** Text contains potential boundary. */
AFL_TEST("afl.net.MimeBuilder:boundary-conflict", a)
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

    static const char RESULT[] =
        "Content-Type: multipart/mixed; boundary=300\r\n"
        "Name: nobody\r\n"
        "\r\n"
        "--300\r\n"
        "\r\n"
        "--000\r\n"
        "--200\r\n"
        "--100\r\n"
        "--300--\r\n";
    a.checkEqual("getTotalSize()", testee.getTotalSize(), sizeof(RESULT)-1);
    a.checkEqual("getContent() size", sink.getContent().size(), sizeof(RESULT)-1);
    a.checkEqual("content", afl::string::fromBytes(sink.getContent()), RESULT);
}

/** Multiple addBoundary() add only one boundary. */
AFL_TEST("afl.net.MimeBuilder:multiple-boundaries", a)
{
    // Build something
    afl::net::MimeBuilder testee("multipart/mixed");
    testee.addBoundary();
    testee.addBoundary();
    testee.addLine("a");
    testee.addBoundary();
    testee.addBoundary();
    testee.addBoundary();
    testee.addLine("b");
    testee.addBoundary();
    testee.finish();

    // Verify it
    afl::io::InternalSink sink;
    testee.write(sink, false);

    static const char RESULT[] =
        "Content-Type: multipart/mixed; boundary=000\r\n"
        "\r\n"
        "--000\r\n"
        "\r\n"
        "a\r\n"
        "--000\r\n"
        "\r\n"
        "b\r\n"
        "--000--\r\n";
    a.checkEqual("getTotalSize()", testee.getTotalSize(), sizeof(RESULT)-1);
    a.checkEqual("getContent() size", sink.getContent().size(), sizeof(RESULT)-1);
    a.checkEqual("content", afl::string::fromBytes(sink.getContent()), RESULT);
}

/** Test building a form. */
AFL_TEST("afl.net.MimeBuilder:addFormField", a)
{
    // Build a form. For is built without MIME type because we won't use it anyway.
    afl::net::MimeBuilder testee("");
    // Standard case
    testee.addFormFieldValue("a", afl::string::toBytes("v1"));
    // Exercise field name quoting
    testee.addFormFieldValue("\"b\"", afl::string::toBytes("v2"));
    // Generic field
    testee.addFormField("c");
    testee.addHeader("Content-Type", "text/plain");
    testee.addLine("v3");
    // File
    testee.addFormFile("d", "d.dat");
    testee.addHeader("Content-Type", "application/octet-stream");
    testee.addRawData(afl::string::toBytes("v4"));
    // Final boundary must be given explicitly
    testee.addBoundary();
    testee.finish();
    testee.removeInitialHeaders();

    // Verify it
    afl::io::InternalSink sink;
    testee.write(sink, false);

    static const char RESULT[] =
        "--000\r\n"
        "Content-Disposition: form-data; name=\"a\"\r\n"
        "\r\n"
        "v1\r\n"
        "--000\r\n"
        "Content-Disposition: form-data; name=\"\\\"b\\\"\"\r\n"
        "\r\n"
        "v2\r\n"
        "--000\r\n"
        "Content-Disposition: form-data; name=\"c\"\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "v3\r\n"
        "--000\r\n"
        "Content-Disposition: form-data; name=\"d\"; filename=\"d.dat\"\r\n"
        "Content-Type: application/octet-stream\r\n"
        "\r\n"
        "v4\r\n"
        "--000--\r\n";
    a.checkEqual("getTotalSize()", testee.getTotalSize(), sizeof(RESULT)-1);
    a.checkEqual("getContent() size", sink.getContent().size(), sizeof(RESULT)-1);
    a.checkEqual("content", afl::string::fromBytes(sink.getContent()), RESULT);
}

/** Test building a form from a HeaderTable. */
AFL_TEST("afl.net.MimeBuilder:addFormFields", a)
{
    afl::net::HeaderTable tab;
    tab.add("h1", "v1");
    tab.add("h2", "v2");

    afl::net::MimeBuilder testee("multipart/form-data");
    testee.addFormFields(tab);
    testee.finish();

    // Verify it
    afl::io::InternalSink sink;
    testee.write(sink, false);

    static const char RESULT[] =
        "Content-Type: multipart/form-data; boundary=000\r\n"
        "\r\n"
        "--000\r\n"
        "Content-Disposition: form-data; name=\"h1\"\r\n"
        "\r\n"
        "v1\r\n"
        "--000\r\n"
        "Content-Disposition: form-data; name=\"h2\"\r\n"
        "\r\n"
        "v2\r\n"
        "--000--\r\n";
    a.checkEqual("getTotalSize()", testee.getTotalSize(), sizeof(RESULT)-1);
    a.checkEqual("getContent() size", sink.getContent().size(), sizeof(RESULT)-1);
    a.checkEqual("content", afl::string::fromBytes(sink.getContent()), RESULT);
}

/** Test addHeaders. */
AFL_TEST("afl.net.MimeBuilder:addHeaders", a)
{
    afl::net::HeaderTable tab;
    tab.add("h1", "v1");
    tab.add("h2", "v2");

    afl::net::MimeBuilder testee("multipart/mixed");
    testee.addHeaders(tab);
    testee.addBoundary();
    testee.addHeaders(tab);
    testee.addLine("xx");
    testee.addBoundary();

     testee.finish();

    // Verify it
    afl::io::InternalSink sink;
    testee.write(sink, false);
    static const char RESULT[] =
        "Content-Type: multipart/mixed; boundary=000\r\n"
        "h1: v1\r\n"
        "h2: v2\r\n"
        "\r\n"
        "--000\r\n"
        "h1: v1\r\n"
        "h2: v2\r\n"
        "\r\n"
        "xx\r\n"
        "--000--\r\n";
    a.checkEqual("getTotalSize()", testee.getTotalSize(), sizeof(RESULT)-1);
    a.checkEqual("getContent() size", sink.getContent().size(), sizeof(RESULT)-1);
    a.checkEqual("content", afl::string::fromBytes(sink.getContent()), RESULT);
}
