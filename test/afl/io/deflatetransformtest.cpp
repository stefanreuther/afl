/**
  *  \file test/afl/io/deflatetransformtest.cpp
  *  \brief Test for afl::io::DeflateTransform
  */

#include "afl/io/deflatetransform.hpp"

#include "afl/checksums/md5.hpp"
#include "afl/except/unsupportedexception.hpp"
#include "afl/io/inflatetransform.hpp"
#include "afl/string/format.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    void save(afl::base::Bytes_t& out, afl::base::ConstBytes_t in)
    {
        out.split(in.size()).copyFrom(in);
    }

    /** Zip roundtrip test.
        Tests that data compresses and decompresses to the same data again.
        \param pers1,pers2 [in] Personalities to use
        \param header     [out] Initial part of compressed data will be produced here
        \param loops       [in] Number of iterations */
    bool testZipRoundtrip(afl::test::Assert a,
                          afl::io::DeflateTransform::Personality pers1,
                          afl::io::InflateTransform::Personality pers2,
                          afl::base::Bytes_t header,
                          int32_t loops)
    {
        if (!afl::io::DeflateTransform::isAvailable() || !afl::io::InflateTransform::isAvailable()) {
            // Test how it refuses being constructed.
            AFL_CHECK_THROWS(a("unsupported compression"), new afl::io::DeflateTransform(pers1),
                             afl::except::UnsupportedException);
            return false;
        }

        // Make a deflater and an inflater
        afl::io::DeflateTransform testee(pers1);
        afl::io::InflateTransform helper(pers2);

        // Make two checksummers. We'll be checksumming input and output data
        afl::checksums::MD5 inputMD5;
        afl::checksums::MD5 outputMD5;

        // Byte counters
        uint64_t inputBytes = 0;
        uint64_t compressedBytes = 0;
        uint64_t outputBytes = 0;

        // Generate some text and feed it into the compressor. Feed the result into the decompressor.
        for (int32_t i = 0; i < loops; ++i) {
            // Test text
            String_t text = afl::string::Format("test text %d\n", i);
            afl::base::ConstBytes_t input(afl::string::toBytes(text));
            inputMD5.add(input);
            inputBytes += input.size();

            // Compress
            while (!input.empty()) {
                uint8_t compressedBuffer[20];
                afl::base::Bytes_t compressed(compressedBuffer);
                testee.transform(input, compressed);
                compressedBytes += compressed.size();
                save(header, compressed);

                // Decompress
                afl::base::ConstBytes_t compressedInput(compressed);
                while (!compressedInput.empty()) {
                    uint8_t expandedBuffer[20];
                    afl::base::Bytes_t expanded(expandedBuffer);
                    helper.transform(compressedInput, expanded);
                    outputMD5.add(expanded);
                    outputBytes += expanded.size();
                }
            }
        }

        // Write end marker. Feed empty blocks into the compressor until we do not get anything more out.
        testee.flush();
        while (1) {
            afl::base::ConstBytes_t empty;
            uint8_t compressedBuffer[20];
            afl::base::Bytes_t compressed(compressedBuffer);
            testee.transform(empty, compressed);
            compressedBytes += compressed.size();
            save(header, compressed);
            if (compressed.empty()) {
                break;
            }

            // Decompress
            afl::base::ConstBytes_t compressedInput(compressed);
            while (!compressedInput.empty()) {
                uint8_t expandedBuffer[20];
                afl::base::Bytes_t expanded(expandedBuffer);
                helper.transform(compressedInput, expanded);
                outputMD5.add(expanded);
                outputBytes += expanded.size();
            }
        }

        // OK, done. Verify
        a.checkEqual("byte count", inputBytes, outputBytes);
        if (loops > 100) {
            a.check("compression effectiveness", compressedBytes < inputBytes);
        }

        uint8_t inputHashBuffer[20];
        uint8_t outputHashBuffer[20];
        a.check("data correctly passed through", inputMD5.getHash(inputHashBuffer).equalContent(outputMD5.getHash(outputHashBuffer)));
        return true;
    }
}

/*
 *  Test deflation in gzip format
 */

// No data; tests flushing without compressing
AFL_TEST("afl.io.DeflateTransform:gzip:no-data", a)
{
    uint8_t header[] = { 0, 0 };
    if (testZipRoundtrip(a, afl::io::DeflateTransform::Gzip, afl::io::InflateTransform::Gzip, header, 0)) {
        a.checkEqual("header[0]", header[0], 0x1F);
        a.checkEqual("header[1]", header[1], 0x8B);
    }
}

// Small data; initial data feed will not produce output
AFL_TEST("afl.io.DeflateTransform:gzip:small", a)
{
    uint8_t header[] = { 0, 0 };
    if (testZipRoundtrip(a, afl::io::DeflateTransform::Gzip, afl::io::InflateTransform::Gzip, header, 2)) {
        a.checkEqual("header[0]", header[0], 0x1F);
        a.checkEqual("header[1]", header[1], 0x8B);
    }
}

// Large data
AFL_TEST("afl.io.DeflateTransform:gzip:large", a)
{
    uint8_t header[] = { 0, 0 };
    if (testZipRoundtrip(a, afl::io::DeflateTransform::Gzip, afl::io::InflateTransform::Gzip, header, 100000)) {
        a.checkEqual("header[0]", header[0], 0x1F);
        a.checkEqual("header[1]", header[1], 0x8B);
    }
}

/*
 *  Test deflation in raw format
 */

AFL_TEST("afl.io.DeflateTransform:raw", a)
{
    // Variety of data sizes
    testZipRoundtrip(a("0"),      afl::io::DeflateTransform::Raw, afl::io::InflateTransform::Raw, afl::base::Bytes_t(), 0);
    testZipRoundtrip(a("1"),      afl::io::DeflateTransform::Raw, afl::io::InflateTransform::Raw, afl::base::Bytes_t(), 1);
    testZipRoundtrip(a("2"),      afl::io::DeflateTransform::Raw, afl::io::InflateTransform::Raw, afl::base::Bytes_t(), 2);
    testZipRoundtrip(a("10"),     afl::io::DeflateTransform::Raw, afl::io::InflateTransform::Raw, afl::base::Bytes_t(), 10);
    testZipRoundtrip(a("100"),    afl::io::DeflateTransform::Raw, afl::io::InflateTransform::Raw, afl::base::Bytes_t(), 100);
    testZipRoundtrip(a("1000"),   afl::io::DeflateTransform::Raw, afl::io::InflateTransform::Raw, afl::base::Bytes_t(), 1000);
    testZipRoundtrip(a("10000"),  afl::io::DeflateTransform::Raw, afl::io::InflateTransform::Raw, afl::base::Bytes_t(), 10000);
    testZipRoundtrip(a("100000"), afl::io::DeflateTransform::Raw, afl::io::InflateTransform::Raw, afl::base::Bytes_t(), 100000);
}

/*
 *  Test deflation in zlib format
 */

// No data; tests flushing without compressing
AFL_TEST("afl.io.DeflateTransform:zlib:no-data", a)
{
    uint8_t header[] = { 0, 0 };
    if (testZipRoundtrip(a, afl::io::DeflateTransform::Zlib, afl::io::InflateTransform::Zlib, header, 0)) {
        a.checkEqual("header[0]", header[0] & 15, 8);
    }
}

// Small data; initial data feed will not produce output
AFL_TEST("afl.io.DeflateTransform:zlib:small", a)
{
    uint8_t header[] = { 0, 0 };
    if (testZipRoundtrip(a, afl::io::DeflateTransform::Zlib, afl::io::InflateTransform::Zlib, header, 2)) {
        a.checkEqual("header[0]", header[0] & 15, 8);
    }
}

// Large data
AFL_TEST("afl.io.DeflateTransform:zlib:large", a)
{
    uint8_t header[] = { 0, 0 };
    if (testZipRoundtrip(a, afl::io::DeflateTransform::Zlib, afl::io::InflateTransform::Zlib, header, 100000)) {
        a.checkEqual("header[0]", header[0] & 15, 8);
    }
}
