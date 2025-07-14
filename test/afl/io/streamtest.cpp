/**
  *  \file test/afl/io/streamtest.cpp
  *  \brief Test for afl::io::Stream
  *
  *  This actually mostly tests the underlying system implementation.
  */

#include "afl/io/stream.hpp"

#include <cstdio>
#include "afl/test/testrunner.hpp"
#include "afl/base/ptr.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/except/filetooshortexception.hpp"
#include "afl/io/filemapping.hpp"
#include "afl/io/internalstream.hpp"
#include "afl/string/string.hpp"

/** Simple test. */
AFL_TEST("afl.io.Stream", a)
{
    static const char FILENAME[] = "__test.tmp";

    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();

    {
        // Create a file
        afl::base::Ref<afl::io::Stream> ps = fs.openFile(FILENAME, fs.Create);
        a.check("01. CanRead", (ps->getCapabilities() & afl::io::Stream::CanRead) != 0);
        a.check("02. CanWrite", (ps->getCapabilities() & afl::io::Stream::CanWrite) != 0);
        a.check("03. CanSeek", (ps->getCapabilities() & afl::io::Stream::CanSeek) != 0);
        a.check("04. hasCapabilities", ps->hasCapabilities(afl::io::Stream::CanRead | afl::io::Stream::CanWrite | afl::io::Stream::CanSeek));

        a.checkEqual("05. getSize", ps->getSize(), 0U);
        a.checkEqual("06. getPos", ps->getPos(), 0U);

        // Write 4 bytes: 1 2 3 4
        static const uint8_t data[] = { 1,2,3,4 };
        a.checkEqual("11. write", ps->write(data), 4U);
        a.checkEqual("12. getSize", ps->getSize(), 4U);
        a.checkEqual("13. getPos", ps->getPos(), 4U);

        // Write 4 bytes at position 2: 1 2 1 2 3 4
        ps->setPos(2);
        a.checkEqual("21. getSize", ps->getSize(), 4U);
        a.checkEqual("22. getPos", ps->getPos(), 2U);

        a.checkEqual("23. write", ps->write(data), 4U);
        a.checkEqual("24. getSize", ps->getSize(), 6U);
        a.checkEqual("25. getPos", ps->getPos(), 6U);

        // Write 4 bytes at position 1: 1 1 2 3 4 4
        ps->setPos(1);
        a.checkEqual("31. write", ps->write(data), 4U);
        a.checkEqual("32. getSize", ps->getSize(), 6U);
        a.checkEqual("33. getPos", ps->getPos(), 5U);
    }

    {
        // We cannot create the file exclusively
        AFL_CHECK_THROWS(a("41. openFile"), fs.openFile(FILENAME, fs.CreateNew), afl::except::FileProblemException);
    }

    {
        // Read the file. It must be possible to open it twice.
        afl::base::Ref<afl::io::Stream> ps1 = fs.openFile(FILENAME, fs.OpenRead);
        afl::base::Ref<afl::io::Stream> ps2 = fs.openFile(FILENAME, fs.OpenRead);

        a.checkEqual("51. getSize", ps1->getSize(), 6U);
        a.checkEqual("52. getSize", ps2->getSize(), 6U);
        a.checkEqual("53. getPos", ps1->getPos(), 0U);
        a.checkEqual("54. getPos", ps2->getPos(), 0U);

        // Read
        uint8_t buf[20];
        a.checkEqual("61. read", ps1->read(buf), 6U);
        a.checkEqual("62. getPos", ps1->getPos(), 6U);
        a.checkEqual("63. data", buf[0], 1U);
        a.checkEqual("64. data", buf[1], 1U);
        a.checkEqual("65. data", buf[2], 2U);
        a.checkEqual("66. data", buf[3], 3U);
        a.checkEqual("67. data", buf[4], 4U);
        a.checkEqual("68. data", buf[5], 4U);

        a.checkEqual("71. getPos", ps2->getPos(), 0U);
        AFL_CHECK_THROWS(a("72. fullRead"), ps2->fullRead(buf), afl::except::FileTooShortException);
    }

    {
        // Remove the file.
        // FIXME: that assumes that the standard 'remove' is compatible with our file name interpretation.
        // For a name consisting just of ASCII characters, that's correct.
        a.checkEqual("81. remove", std::remove(FILENAME), 0);
    }

    {
        // Cannot open for reading or writing now
        AFL_CHECK_THROWS(a("91. OpenRead"), fs.openFile(FILENAME, fs.OpenRead), afl::except::FileProblemException);
        AFL_CHECK_THROWS(a("92. OpenWrite"), fs.openFile(FILENAME, fs.OpenWrite), afl::except::FileProblemException);
    }
}

/** Interface test. */
AFL_TEST_NOARG("afl.io.Stream:interface")
{
    class Tester : public afl::io::Stream {
     public:
        virtual size_t read(Bytes_t /*m*/)
            { return 0; }
        virtual size_t write(ConstBytes_t /*m*/)
            { return 0; }
        virtual void flush()
            { }
        virtual void setPos(FileSize_t /*pos*/)
            { }
        virtual FileSize_t getPos()
            { return FileSize_t(); }
        virtual FileSize_t getSize()
            { return FileSize_t(); }
        virtual uint32_t getCapabilities()
            { return 0; }
        virtual String_t getName()
            { return String_t(); }
        virtual afl::base::Ref<Stream> createChild(uint32_t /*flags*/)
            { throw "geht ned"; }
        virtual afl::base::Ptr<afl::io::FileMapping> createFileMapping(FileSize_t /*limit*/)
            { return 0; }
    };
    Tester t;
}

/** Test copyFrom. */
AFL_TEST("afl.io.Stream:copyFrom", a)
{
    afl::io::InternalStream in;
    in.write(afl::string::toBytes("hello world"));
    a.checkEqual("01. getPos", in.getPos(), 11U);
    a.checkEqual("02. getSize", in.getSize(), 11U);
    in.setPos(3);
    a.checkEqual("03. getPos", in.getPos(), 3U);
    a.checkEqual("04. getSize", in.getSize(), 11U);

    // Copy with given length
    afl::io::InternalStream out;
    out.copyFrom(in, 5);
    a.checkEqual("11. getPos", out.getPos(), 5U);
    a.checkEqual("12. getSize", out.getSize(), 5U);
    a.checkEqual("13. getPos", in.getPos(), 8U);

    // Copy all to end
    out.copyFrom(in);
    a.checkEqual("21. getPos", out.getPos(), 8U);
    a.checkEqual("22. getSize", out.getSize(), 8U);
    a.checkEqual("23. getPos", in.getPos(), 11U);

    a.check("31. content", out.getContent().equalContent(afl::string::toBytes("lo world")));
}
