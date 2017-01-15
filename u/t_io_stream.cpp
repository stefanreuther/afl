/**
  *  \file u/t_io_stream.cpp
  *  \brief Test for afl::io::Stream
  *
  *  This actually mostly tests the underlying system implementation.
  */

#include "afl/io/stream.hpp"

#include "u/t_io.hpp"

#include <cstdio>
#include "afl/base/ptr.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/except/filetooshortexception.hpp"
#include "afl/io/filemapping.hpp"
#include "afl/io/internalstream.hpp"
#include "afl/string/string.hpp"

/** Simple test. */
void
TestIoStream::testIt()
{
    static const char FILENAME[] = "__test.tmp";

    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();

    {
        // Create a file
        afl::base::Ref<afl::io::Stream> ps = fs.openFile(FILENAME, fs.Create);
        TS_ASSERT((ps->getCapabilities() & afl::io::Stream::CanRead) != 0);
        TS_ASSERT((ps->getCapabilities() & afl::io::Stream::CanWrite) != 0);
        TS_ASSERT((ps->getCapabilities() & afl::io::Stream::CanSeek) != 0);
        TS_ASSERT(ps->hasCapabilities(afl::io::Stream::CanRead | afl::io::Stream::CanWrite | afl::io::Stream::CanSeek));

        TS_ASSERT_EQUALS(ps->getSize(), 0U);
        TS_ASSERT_EQUALS(ps->getPos(), 0U);

        // Write 4 bytes: 1 2 3 4
        static const uint8_t data[] = { 1,2,3,4 };
        TS_ASSERT_EQUALS(ps->write(data), 4U);
        TS_ASSERT_EQUALS(ps->getSize(), 4U);
        TS_ASSERT_EQUALS(ps->getPos(), 4U);

        // Write 4 bytes at position 2: 1 2 1 2 3 4
        ps->setPos(2);
        TS_ASSERT_EQUALS(ps->getSize(), 4U);
        TS_ASSERT_EQUALS(ps->getPos(), 2U);

        TS_ASSERT_EQUALS(ps->write(data), 4U);
        TS_ASSERT_EQUALS(ps->getSize(), 6U);
        TS_ASSERT_EQUALS(ps->getPos(), 6U);

        // Write 4 bytes at position 1: 1 1 2 3 4 4
        ps->setPos(1);
        TS_ASSERT_EQUALS(ps->write(data), 4U);
        TS_ASSERT_EQUALS(ps->getSize(), 6U);
        TS_ASSERT_EQUALS(ps->getPos(), 5U);
    }

    {
        // We cannot create the file exclusively
        TS_ASSERT_THROWS(fs.openFile(FILENAME, fs.CreateNew), afl::except::FileProblemException);
    }

    {
        // Read the file. It must be possible to open it twice.
        afl::base::Ref<afl::io::Stream> ps1 = fs.openFile(FILENAME, fs.OpenRead);
        afl::base::Ref<afl::io::Stream> ps2 = fs.openFile(FILENAME, fs.OpenRead);

        TS_ASSERT_EQUALS(ps1->getSize(), 6U);
        TS_ASSERT_EQUALS(ps2->getSize(), 6U);
        TS_ASSERT_EQUALS(ps1->getPos(), 0U);
        TS_ASSERT_EQUALS(ps2->getPos(), 0U);

        // Read
        uint8_t buf[20];
        TS_ASSERT_EQUALS(ps1->read(buf), 6U);
        TS_ASSERT_EQUALS(ps1->getPos(), 6U);
        TS_ASSERT_EQUALS(buf[0], 1U);
        TS_ASSERT_EQUALS(buf[1], 1U);
        TS_ASSERT_EQUALS(buf[2], 2U);
        TS_ASSERT_EQUALS(buf[3], 3U);
        TS_ASSERT_EQUALS(buf[4], 4U);
        TS_ASSERT_EQUALS(buf[5], 4U);

        TS_ASSERT_EQUALS(ps2->getPos(), 0U);
        TS_ASSERT_THROWS(ps2->fullRead(buf), afl::except::FileTooShortException);
    }

    {
        // Remove the file.
        // FIXME: that assumes that the standard 'remove' is compatible with our file name interpretation.
        // For a name consisting just of ASCII characters, that's correct.
        TS_ASSERT_EQUALS(std::remove(FILENAME), 0);
    }

    {
        // Cannot open for reading or writing now
        TS_ASSERT_THROWS(fs.openFile(FILENAME, fs.OpenRead), afl::except::FileProblemException);
        TS_ASSERT_THROWS(fs.openFile(FILENAME, fs.OpenWrite), afl::except::FileProblemException);
    }
}

/** Interface test. */
void
TestIoStream::testInterface()
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
        virtual afl::base::Ref<Stream> createChild()
            { throw "geht ned"; }
        virtual afl::base::Ptr<afl::io::FileMapping> createFileMapping(FileSize_t /*limit*/)
            { return 0; }
    };
    Tester t;
}

/** Test copyFrom. */
void
TestIoStream::testCopy()
{
    afl::io::InternalStream in;
    in.write(afl::string::toBytes("hello world"));
    TS_ASSERT_EQUALS(in.getPos(), 11U);
    TS_ASSERT_EQUALS(in.getSize(), 11U);
    in.setPos(3);
    TS_ASSERT_EQUALS(in.getPos(), 3U);
    TS_ASSERT_EQUALS(in.getSize(), 11U);

    // Copy with given length
    afl::io::InternalStream out;
    out.copyFrom(in, 5);
    TS_ASSERT_EQUALS(out.getPos(), 5U);
    TS_ASSERT_EQUALS(out.getSize(), 5U);
    TS_ASSERT_EQUALS(in.getPos(), 8U);

    // Copy all to end
    out.copyFrom(in);
    TS_ASSERT_EQUALS(out.getPos(), 8U);
    TS_ASSERT_EQUALS(out.getSize(), 8U);
    TS_ASSERT_EQUALS(in.getPos(), 11U);

    TS_ASSERT(out.getContent().equalContent(afl::string::toBytes("lo world")));
}
