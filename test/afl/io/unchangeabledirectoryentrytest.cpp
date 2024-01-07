/**
  *  \file test/afl/io/unchangeabledirectoryentrytest.cpp
  *  \brief Test for afl::io::UnchangeableDirectoryEntry
  */

#include "afl/io/unchangeabledirectoryentry.hpp"

#include "afl/except/fileproblemexception.hpp"
#include "afl/io/constmemorystream.hpp"
#include "afl/test/testrunner.hpp"


AFL_TEST("afl.io.UnchangeableDirectoryEntry", a)
{
    class Tester : public afl::io::UnchangeableDirectoryEntry {
     public:
        Tester(String_t title, String_t pathName)
            : UnchangeableDirectoryEntry("fail!"),
              m_title(title), m_pathName(pathName)
            { }
        virtual String_t getTitle()
            { return m_title; }
        virtual String_t getPathName()
            { return m_pathName; }
        virtual afl::base::Ref<afl::io::Directory> openDirectory()
            { throw std::runtime_error("no ref"); }
        virtual afl::base::Ref<afl::io::Directory> openContainingDirectory()
            { throw std::runtime_error("no ref"); }
        virtual afl::base::Ref<afl::io::Stream> openFileForReading()
            { return *new afl::io::ConstMemoryStream(afl::base::Nothing); }
        virtual void updateInfo(uint32_t /*requested*/)
            { }
     private:
        String_t m_title;
        String_t m_pathName;
    };

    // Test mock accessors:
    a.checkEqual("getTitle",    Tester("t", "p").getTitle(), "t");
    a.checkEqual("getPathName", Tester("t", "p").getPathName(), "p");

    // Test regular modifiers:
    AFL_CHECK_THROWS(a("renameTo"),          Tester("a", "").renameTo("b"), afl::except::FileProblemException);
    AFL_CHECK_THROWS(a("erase"),             Tester("a", "").erase(), afl::except::FileProblemException);
    AFL_CHECK_THROWS(a("createAsDirectory"), Tester("a", "").createAsDirectory(), afl::except::FileProblemException);
    AFL_CHECK_THROWS(a("setFlag"),           Tester("a", "").setFlag(afl::io::DirectoryEntry::Executable, true), afl::except::FileProblemException);
    AFL_CHECK_THROWS(a("openFile"),          Tester("a", "").openFile(afl::io::FileSystem::Create), afl::except::FileProblemException);

    // Test NT modifiers:
    a.check("renameToNT", !Tester("a", "").renameToNT("b"));
    a.check("eraseNT",    !Tester("a", "").eraseNT());

    // Test successful opening
    AFL_CHECK_SUCCEEDS(a("openFile"), Tester("a", "").openFile(afl::io::FileSystem::OpenRead));

    // Test correct exception content
    // - if path and title are known, uses the path
    try {
        Tester("t", "p").erase();
        a.fail("erase should fail");
    }
    catch (afl::except::FileProblemException& fpe) {
        a.checkEqual("getFileName after erase", fpe.getFileName(), "p");
        a.checkEqual("what after erase", String_t(fpe.what()), "fail!");
    }
    catch (...) {
        a.fail("erase should throw FileProblemException");
    }

    // - if path is not known, uses the title
    try {
        Tester("t", "").erase();
        a.fail("erase should fail 2");
    }
    catch (afl::except::FileProblemException& fpe) {
        a.checkEqual("getFileName after erase 2", fpe.getFileName(), "t");
        a.checkEqual("what after erase 2", String_t(fpe.what()), "fail!");
    }
    catch (...) {
        a.fail("erase should throw FileProblemException 2");
    }
}
