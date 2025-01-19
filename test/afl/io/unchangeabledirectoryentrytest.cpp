/**
  *  \file test/afl/io/unchangeabledirectoryentrytest.cpp
  *  \brief Test for afl::io::UnchangeableDirectoryEntry
  */

#include "afl/io/unchangeabledirectoryentry.hpp"

#include "afl/except/fileproblemexception.hpp"
#include "afl/io/constmemorystream.hpp"
#include "afl/io/internaldirectory.hpp"
#include "afl/test/testrunner.hpp"

using afl::base::Ref;
using afl::except::FileProblemException;
using afl::io::InternalDirectory;

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
        virtual Ref<afl::io::Directory> openDirectory()
            { throw std::runtime_error("no ref"); }
        virtual Ref<afl::io::Directory> openContainingDirectory()
            { throw std::runtime_error("no ref"); }
        virtual Ref<afl::io::Stream> openFileForReading()
            { return *new afl::io::ConstMemoryStream(afl::base::Nothing); }
        virtual void updateInfo(uint32_t /*requested*/)
            { }
     private:
        String_t m_title;
        String_t m_pathName;
    };
    Ref<InternalDirectory> target = InternalDirectory::create("x");

    // Test mock accessors:
    a.checkEqual("getTitle",    Tester("t", "p").getTitle(), "t");
    a.checkEqual("getPathName", Tester("t", "p").getPathName(), "p");

    // Test regular modifiers:
    AFL_CHECK_THROWS(a("renameTo"),          Tester("a", "").renameTo("b"), FileProblemException);
    AFL_CHECK_THROWS(a("erase"),             Tester("a", "").erase(), FileProblemException);
    AFL_CHECK_THROWS(a("createAsDirectory"), Tester("a", "").createAsDirectory(), FileProblemException);
    AFL_CHECK_THROWS(a("setFlag"),           Tester("a", "").setFlag(afl::io::DirectoryEntry::Executable, true), FileProblemException);
    AFL_CHECK_THROWS(a("openFile"),          Tester("a", "").openFile(afl::io::FileSystem::Create), FileProblemException);
    AFL_CHECK_THROWS(a("moveTo"),            Tester("a", "").moveTo(*target, "x"), FileProblemException);

    // Test NT modifiers:
    a.check("renameToNT", !Tester("a", "").renameToNT("b"));
    a.check("eraseNT",    !Tester("a", "").eraseNT());
    a.check("moveToNT",   !Tester("a", "").moveToNT(*target, "x"));

    // Test successful opening
    AFL_CHECK_SUCCEEDS(a("openFile"), Tester("a", "").openFile(afl::io::FileSystem::OpenRead));

    // Test correct exception content
    // - if path and title are known, uses the path
    try {
        Tester("t", "p").erase();
        a.fail("erase should fail");
    }
    catch (FileProblemException& fpe) {
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
    catch (FileProblemException& fpe) {
        a.checkEqual("getFileName after erase 2", fpe.getFileName(), "t");
        a.checkEqual("what after erase 2", String_t(fpe.what()), "fail!");
    }
    catch (...) {
        a.fail("erase should throw FileProblemException 2");
    }
}
