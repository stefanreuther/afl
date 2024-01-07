/**
  *  \file test/afl/except/exceptiontest.cpp
  *  \brief Tests for exception classes
  */

#include "afl/except/commandlineexception.hpp"
#include "afl/except/fileformatexception.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/except/filesystemexception.hpp"
#include "afl/except/filetooshortexception.hpp"
#include "afl/except/invaliddataexception.hpp"
#include "afl/except/remoteerrorexception.hpp"
#include "afl/except/systemexception.hpp"
#include "afl/except/unsupportedexception.hpp"

#include <cstring>
#include "afl/io/filemapping.hpp"
#include "afl/io/multiplexablestream.hpp"
#include "afl/test/testrunner.hpp"

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


/*
 *  FileFormatException
 */

/** Test FileFormatException. */
AFL_TEST("afl.except.FileFormatException:create:string", a) {
    afl::except::FileFormatException p1("FileName", "Message");
    a.checkEqual("what", p1.what(), String_t("Message"));
    a.checkEqual("getFileName", p1.getFileName(), "FileName");
}

AFL_TEST("afl.except.FileFormatException:create:stream", a) {
    TestStream ts;
    afl::except::FileFormatException p1(ts, "Message 2");
    a.checkEqual("what", p1.what(), String_t("Message 2"));
    a.checkEqual("getFileName", p1.getFileName(), "TestStream");
}

AFL_TEST_NOARG("afl.except.FileFormatException:copy") {
    afl::except::FileFormatException p1("FileName", "Message");
    afl::except::FileFormatException p2(p1);
    afl::except::FileProblemException& r(p1);
    (void) r;
}

/*
 *  FileProblemException
 */

AFL_TEST("afl.except.FileProblemException:create:string", a) {
    afl::except::FileProblemException p1("FileName", "Message");
    a.checkEqual("what", p1.what(), String_t("Message"));
    a.checkEqual("getFileName", p1.getFileName(), "FileName");
}

AFL_TEST("afl.except.FileProblemException:create:stream", a) {
    TestStream ts;
    afl::except::FileProblemException p1(ts, "Message 2");
    a.checkEqual("what", p1.what(), String_t("Message 2"));
    a.checkEqual("getFileName", p1.getFileName(), "TestStream");
}

AFL_TEST_NOARG("afl.except.FileProblemException:copy")  {
    afl::except::FileProblemException p1("FileName", "Message");
    afl::except::FileProblemException p2(p1);
    std::runtime_error& r(p1);
    (void) r;
}

AFL_TEST("afl.except.FileProblemException:setFileName", a) {
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
        a.checkEqual("", e.getFileName(), "b");
    }
    catch (...) {
        a.fail("bad exception");
    }
}


/*
 *  FileSystemException
 */

AFL_TEST("afl.except.FileSystemException:create:string", a) {
    afl::except::FileSystemException x("FileName", afl::sys::Error(99));
    a.checkEqual("what", x.what(), afl::sys::Error(99).getText());
    a.checkEqual("getError", x.getError().getNumber(), afl::sys::Error::ErrorNumber_t(99));
    a.checkEqual("getFileName", x.getFileName(), String_t("FileName"));
}

AFL_TEST("afl.except.FileSystemException:create:stream", a) {
    TestStream ts;
    afl::except::FileSystemException x(ts, afl::sys::Error(99));
    a.checkEqual("what", x.what(), afl::sys::Error(99).getText());
    a.checkEqual("getError", x.getError().getNumber(), afl::sys::Error::ErrorNumber_t(99));
    a.checkEqual("getFileName", x.getFileName(), "TestStream");
}

AFL_TEST_NOARG("afl.except.FileSystemException:copy") {
    afl::except::FileSystemException x("File Name", afl::sys::Error(99));
    afl::except::FileSystemException x2(x);
    std::runtime_error& r(x);
    (void) r;
}

/*
 *  FileTooShortException
 */

AFL_TEST("afl.except.FileTooShortException:create:string", a) {
    afl::except::FileTooShortException p1("FileName");
    a.checkDifferent("what", p1.what(), String_t(""));
    a.checkEqual("getFileName", p1.getFileName(), "FileName");
}

AFL_TEST("afl.except.FileTooShortException:create:stream", a) {
    TestStream ts;
    afl::except::FileTooShortException p1(ts);
    a.checkDifferent("what", p1.what(), String_t(""));
    a.checkEqual("getFileName", p1.getFileName(), "TestStream");
}

AFL_TEST_NOARG("afl.except.FileTooShortException:copy") {
    afl::except::FileTooShortException p1("FileName");
    afl::except::FileTooShortException p2(p1);
    afl::except::FileFormatException& r(p1);
    (void) r;
}

/*
 *  InvalidDataException
 */

AFL_TEST("afl.except.InvalidDataException:create", a) {
    afl::except::InvalidDataException x("fail!1");
    a.checkEqual("", x.what(), String_t("fail!1"));
}

AFL_TEST_NOARG("afl.except.InvalidDataException:copy") {
    afl::except::InvalidDataException x("geht net");
    afl::except::InvalidDataException x2(x);
    std::runtime_error& r(x);
    (void) r;
}

/*
 *  UnsupportedException
 */

AFL_TEST("afl.except.UnsupportedException:create", a) {
    afl::except::UnsupportedException x("fail!1");
    a.check("", std::strstr(x.what(), "fail!1") != 0);
}

AFL_TEST_NOARG("afl.except.UnsupportedException:copy") {
    afl::except::UnsupportedException x("geht net");
    afl::except::UnsupportedException x2(x);
    std::runtime_error& r(x);
    (void) r;
}

/*
 *  SystemException
 */

AFL_TEST("afl.except.SystemException:create:unary", a) {
    afl::except::SystemException x(afl::sys::Error(99));
    a.checkEqual("what", x.what(), afl::sys::Error(99).getText());
    a.checkEqual("getError", x.getError().getNumber(), afl::sys::Error::ErrorNumber_t(99));
    a.checkEqual("getHint", x.getHint(), String_t(""));
}

AFL_TEST("afl.except.SystemException:create:binary", a) {
    afl::except::SystemException x(afl::sys::Error(99), "Hint");
    a.checkEqual("what", x.what(), afl::sys::Error(99).getText());
    a.checkEqual("getError", x.getError().getNumber(), afl::sys::Error::ErrorNumber_t(99));
    a.checkEqual("getHint", x.getHint(), String_t("Hint"));
}

AFL_TEST_NOARG("afl.except.SystemException:copy") {
    afl::except::SystemException x(afl::sys::Error(99));
    afl::except::SystemException x2(x);
    std::runtime_error& r(x);
    (void) r;
}

/*
 *  RemoteErrorException
 */

AFL_TEST("afl.except.RemoteErrorException:create:string", a) {
    afl::except::RemoteErrorException x("FileName", "epic fail");
    a.checkEqual("what", x.what(), String_t("epic fail"));
    a.checkEqual("getFileName", x.getFileName(), String_t("FileName"));
}

AFL_TEST("afl.except.RemoteErrorException:create:stream", a) {
    TestStream ts;
    afl::except::RemoteErrorException x(ts, "for the lulz");
    a.checkEqual("what", x.what(), String_t("for the lulz"));
    a.checkEqual("getFileName", x.getFileName(), "TestStream");
}

AFL_TEST_NOARG("afl.except.RemoteErrorException:copy") {
    // Copy/Inheritance
    afl::except::RemoteErrorException x("File Name", "remote");
    afl::except::RemoteErrorException x2(x);
    afl::except::FileProblemException& f(x);
    std::runtime_error& r(x);
    (void) f;
    (void) r;
}

/*
 *  CommandLineException
 */

AFL_TEST("afl.except.CommandLineException:create", a) {
    afl::except::CommandLineException x("nope");
    a.checkEqual("", x.what(), String_t("nope"));
}

AFL_TEST_NOARG("afl.except.CommandLineException:copy") {
    afl::except::CommandLineException x("nope");
    afl::except::CommandLineException x2(x);
    std::runtime_error& r(x);
    (void) r;
}
