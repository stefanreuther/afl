/**
  *  \file u/t_except.cpp
  *  \brief Test for afl::except
  */

#include "afl/except/fileformatexception.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/except/filesystemexception.hpp"
#include "afl/except/filetooshortexception.hpp"
#include "afl/except/invaliddataexception.hpp"
#include "afl/except/remoteerrorexception.hpp"
#include "afl/except/systemexception.hpp"
#include "afl/except/unsupportedexception.hpp"

#include <cstring>
#include "u/t_except.hpp"
#include "afl/io/multiplexablestream.hpp"
#include "afl/io/filemapping.hpp"
#include "afl/except/commandlineexception.hpp"

namespace {
    class TestStream : public afl::io::MultiplexableStream {
        virtual size_t read(Bytes_t)
            { return 0; }
        virtual size_t write(ConstBytes_t)
            { return 0; }
        virtual void flush()
            { }
        virtual void setPos(FileSize_t)
            { }
        virtual FileSize_t getPos()
            { return 0; }
        virtual FileSize_t getSize()
            { return 0; }
        virtual uint32_t getCapabilities()
            { return 0; }
        virtual String_t getName()
            { return "TestStream"; }
        virtual afl::base::Ptr<afl::io::FileMapping> createFileMapping(FileSize_t /*limit*/)
            { return 0; }
    };
}

/** Test FileFormatException. */
void
TestExcept::testFileFormatException()
{
    {
        // Constructor 1
        afl::except::FileFormatException p1("FileName", "Message");
        TS_ASSERT_EQUALS(p1.what(), String_t("Message"));
        TS_ASSERT_EQUALS(p1.getFileName(), "FileName");
    }
    {
        // Constructor 2
        TestStream ts;
        afl::except::FileFormatException p1(ts, "Message 2");
        TS_ASSERT_EQUALS(p1.what(), String_t("Message 2"));
        TS_ASSERT_EQUALS(p1.getFileName(), "TestStream");
    }
    {
        // Copy/Inheritance
        afl::except::FileFormatException p1("FileName", "Message");
        afl::except::FileFormatException p2(p1);
        afl::except::FileProblemException& r(p1);
        (void) r;
    }
}

/** Test FileProblemException. */
void
TestExcept::testFileProblemException()
{
    {
        // Constructor 1
        afl::except::FileProblemException p1("FileName", "Message");
        TS_ASSERT_EQUALS(p1.what(), String_t("Message"));
        TS_ASSERT_EQUALS(p1.getFileName(), "FileName");
    }
    {
        // Constructor 2
        TestStream ts;
        afl::except::FileProblemException p1(ts, "Message 2");
        TS_ASSERT_EQUALS(p1.what(), String_t("Message 2"));
        TS_ASSERT_EQUALS(p1.getFileName(), "TestStream");
    }
    {
        // Copy/Inheritance
        afl::except::FileProblemException p1("FileName", "Message");
        afl::except::FileProblemException p2(p1);
        std::runtime_error& r(p1);
        (void) r;
    }
}

/** Test FileSystemException. */
void
TestExcept::testFileSystemException()
{
    {
        // Constructor 1
        afl::except::FileSystemException x("FileName", afl::sys::Error(99));
        TS_ASSERT_EQUALS(x.what(), afl::sys::Error(99).getText());
        TS_ASSERT_EQUALS(x.getError().getNumber(), afl::sys::Error::ErrorNumber_t(99));
        TS_ASSERT_EQUALS(x.getFileName(), String_t("FileName"));
    }
    {
        // Constructor 2
        TestStream ts;
        afl::except::FileSystemException x(ts, afl::sys::Error(99));
        TS_ASSERT_EQUALS(x.what(), afl::sys::Error(99).getText());
        TS_ASSERT_EQUALS(x.getError().getNumber(), afl::sys::Error::ErrorNumber_t(99));
        TS_ASSERT_EQUALS(x.getFileName(), "TestStream");
    }
    {
        // Copy/Inheritance
        afl::except::FileSystemException x("File Name", afl::sys::Error(99));
        afl::except::FileSystemException x2(x);
        std::runtime_error& r(x);
        (void) r;
    }
}

/** Test FileTooShortException. */
void
TestExcept::testFileTooShortException()
{
    {
        // Constructor 1
        afl::except::FileTooShortException p1("FileName");
        TS_ASSERT_DIFFERS(p1.what(), String_t(""));
        TS_ASSERT_EQUALS(p1.getFileName(), "FileName");
    }
    {
        // Constructor 2
        TestStream ts;
        afl::except::FileTooShortException p1(ts);
        TS_ASSERT_DIFFERS(p1.what(), String_t(""));
        TS_ASSERT_EQUALS(p1.getFileName(), "TestStream");
    }
    {
        // Copy/Inheritance
        afl::except::FileTooShortException p1("FileName");
        afl::except::FileTooShortException p2(p1);
        afl::except::FileFormatException& r(p1);
        (void) r;
    }
}

/** Test InvalidDataException. */
void
TestExcept::testInvalidDataException()
{
    {
        // Constructor
        afl::except::InvalidDataException x("fail!1");
        TS_ASSERT_EQUALS(x.what(), String_t("fail!1"));
    }
    {
        // Copy/Inheritance
        afl::except::InvalidDataException x("geht net");
        afl::except::InvalidDataException x2(x);
        std::runtime_error& r(x);
        (void) r;
    }
}

/** Test UnsupportedException. */
void
TestExcept::testUnsupportedException()
{
    {
        // Constructor
        afl::except::UnsupportedException x("fail!1");
        TS_ASSERT(std::strstr(x.what(), "fail!1") != 0);
    }
    {
        // Copy/Inheritance
        afl::except::UnsupportedException x("geht net");
        afl::except::UnsupportedException x2(x);
        std::runtime_error& r(x);
        (void) r;
    }
}

/** Test SystemException. */
void
TestExcept::testSystemException()
{
    {
        // Constructor 1
        afl::except::SystemException x(afl::sys::Error(99));
        TS_ASSERT_EQUALS(x.what(), afl::sys::Error(99).getText());
        TS_ASSERT_EQUALS(x.getError().getNumber(), afl::sys::Error::ErrorNumber_t(99));
        TS_ASSERT_EQUALS(x.getHint(), String_t(""));
    }
    {
        // Constructor 2
        afl::except::SystemException x(afl::sys::Error(99), "Hint");
        TS_ASSERT_EQUALS(x.what(), afl::sys::Error(99).getText());
        TS_ASSERT_EQUALS(x.getError().getNumber(), afl::sys::Error::ErrorNumber_t(99));
        TS_ASSERT_EQUALS(x.getHint(), String_t("Hint"));
    }
    {
        // Copy/Inheritance
        afl::except::SystemException x(afl::sys::Error(99));
        afl::except::SystemException x2(x);
        std::runtime_error& r(x);
        (void) r;
    }
}

/** Test RemoteErrorException. */
void
TestExcept::testRemoteErrorException()
{
    {
        // Constructor 1
        afl::except::RemoteErrorException x("FileName", "epic fail");
        TS_ASSERT_EQUALS(x.what(), String_t("epic fail"));
        TS_ASSERT_EQUALS(x.getFileName(), String_t("FileName"));
    }
    {
        // Constructor 2
        TestStream ts;
        afl::except::RemoteErrorException x(ts, "for the lulz");
        TS_ASSERT_EQUALS(x.what(), String_t("for the lulz"));
        TS_ASSERT_EQUALS(x.getFileName(), "TestStream");
    }
    {
        // Copy/Inheritance
        afl::except::RemoteErrorException x("File Name", "remote");
        afl::except::RemoteErrorException x2(x);
        afl::except::FileProblemException& f(x);
        std::runtime_error& r(x);
        (void) f;
        (void) r;
    }
}

/** Test CommandLineException. */
void
TestExcept::testCommandLineException()
{
    {
        // Constructor
        afl::except::CommandLineException x("nope");
        TS_ASSERT_EQUALS(x.what(), String_t("nope"));
    }
    {
        // Copy/Inheritance
        afl::except::CommandLineException x("nope");
        afl::except::CommandLineException x2(x);
        std::runtime_error& r(x);
        (void) r;
    }
}

/** Test modifying a FileProblemException descendant. */
void
TestExcept::testModifyFileProblemException()
{
    try {
        try {
            // Throw an exception...
            throw afl::except::FileTooShortException("a");
        }
        catch (afl::except::FileProblemException& fpe) {
            // ...catch it by base type and modify it...
            fpe.setFileName("b");

            // ..,rethrow...
            throw;
        }
    }
    catch (afl::except::FileTooShortException& e) {
        // ...and catch modified exception by original type.
        TS_ASSERT_EQUALS(e.getFileName(), "b");
    }
    catch (...) {
        TS_ASSERT(!"bad exception");
    }
}
