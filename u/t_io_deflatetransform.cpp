/**
  *  \file u/t_io_deflatetransform.cpp
  *  \brief Test for afl::io::DeflateTransform
  */

#include "afl/io/deflatetransform.hpp"

#include "t_io.hpp"
#include "afl/io/inflatetransform.hpp"
#include "afl/checksums/md5.hpp"
#include "afl/string/format.hpp"
#include "afl/except/unsupportedexception.hpp"

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
    bool testZipRoundtrip(afl::io::DeflateTransform::Personality pers1,
                          afl::io::InflateTransform::Personality pers2,
                          afl::base::Bytes_t header,
                          int32_t loops)
    {
        if (!afl::io::DeflateTransform::isAvailable() || !afl::io::InflateTransform::isAvailable()) {
            // Test how it refuses being constructed.
            TS_ASSERT_THROWS(new afl::io::DeflateTransform(pers1),
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
        TS_ASSERT_EQUALS(inputBytes, outputBytes);
        if (loops > 100) {
            TS_ASSERT(compressedBytes < inputBytes);
        }

        uint8_t inputHashBuffer[20];
        uint8_t outputHashBuffer[20];
        TS_ASSERT(inputMD5.getHash(inputHashBuffer).equalContent(outputMD5.getHash(outputHashBuffer)));
        return true;
    }
}

/** Test deflation in gzip format. */
void
TestIoDeflateTransform::testGzip()
{
    // No data; tests flushing without compressing
    {
        uint8_t header[] = { 0, 0 };
        if (testZipRoundtrip(afl::io::DeflateTransform::Gzip, afl::io::InflateTransform::Gzip, header, 0)) {
            TS_ASSERT_EQUALS(header[0], 0x1F);
            TS_ASSERT_EQUALS(header[1], 0x8B);
        }
    }
    // Small data; initial data feed will not produce output
    {
        uint8_t header[] = { 0, 0 };
        if (testZipRoundtrip(afl::io::DeflateTransform::Gzip, afl::io::InflateTransform::Gzip, header, 2)) {
            TS_ASSERT_EQUALS(header[0], 0x1F);
            TS_ASSERT_EQUALS(header[1], 0x8B);
        }
    }
    // Large data
    {
        uint8_t header[] = { 0, 0 };
        if (testZipRoundtrip(afl::io::DeflateTransform::Gzip, afl::io::InflateTransform::Gzip, header, 100000)) {
            TS_ASSERT_EQUALS(header[0], 0x1F);
            TS_ASSERT_EQUALS(header[1], 0x8B);
        }
    }
}

/** Test deflation in raw format. */
void
TestIoDeflateTransform::testRaw()
{
    // Variety of data sizes
    testZipRoundtrip(afl::io::DeflateTransform::Raw, afl::io::InflateTransform::Raw, afl::base::Bytes_t(), 0);
    testZipRoundtrip(afl::io::DeflateTransform::Raw, afl::io::InflateTransform::Raw, afl::base::Bytes_t(), 1);
    testZipRoundtrip(afl::io::DeflateTransform::Raw, afl::io::InflateTransform::Raw, afl::base::Bytes_t(), 2);
    testZipRoundtrip(afl::io::DeflateTransform::Raw, afl::io::InflateTransform::Raw, afl::base::Bytes_t(), 10);
    testZipRoundtrip(afl::io::DeflateTransform::Raw, afl::io::InflateTransform::Raw, afl::base::Bytes_t(), 100);
    testZipRoundtrip(afl::io::DeflateTransform::Raw, afl::io::InflateTransform::Raw, afl::base::Bytes_t(), 1000);
    testZipRoundtrip(afl::io::DeflateTransform::Raw, afl::io::InflateTransform::Raw, afl::base::Bytes_t(), 10000);
    testZipRoundtrip(afl::io::DeflateTransform::Raw, afl::io::InflateTransform::Raw, afl::base::Bytes_t(), 100000);
}

/** Test deflation in zlib format. */
void
TestIoDeflateTransform::testZlib()
{
    // No data; tests flushing without compressing
    {
        uint8_t header[] = { 0, 0 };
        if (testZipRoundtrip(afl::io::DeflateTransform::Zlib, afl::io::InflateTransform::Zlib, header, 0)) {
            TS_ASSERT_EQUALS(header[0] & 15, 8);
        }
    }
    // Small data; initial data feed will not produce output
    {
        uint8_t header[] = { 0, 0 };
        if (testZipRoundtrip(afl::io::DeflateTransform::Zlib, afl::io::InflateTransform::Zlib, header, 2)) {
            TS_ASSERT_EQUALS(header[0] & 15, 8);
        }
    }
    // Large data
    {
        uint8_t header[] = { 0, 0 };
        if (testZipRoundtrip(afl::io::DeflateTransform::Zlib, afl::io::InflateTransform::Zlib, header, 100000)) {
            TS_ASSERT_EQUALS(header[0] & 15, 8);
        }
    }
}

