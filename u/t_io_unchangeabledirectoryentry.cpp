/**
  *  \file u/t_io_unchangeabledirectoryentry.cpp
  *  \brief Test for afl::io::UnchangeableDirectoryEntry
  */

#include "afl/io/unchangeabledirectoryentry.hpp"

#include "t_io.hpp"
#include "afl/io/constmemorystream.hpp"
#include "afl/except/fileproblemexception.hpp"

/** Test it. */
void
TestIoUnchangeableDirectoryEntry::testIt()
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
    TS_ASSERT_EQUALS(Tester("t", "p").getTitle(), "t");
    TS_ASSERT_EQUALS(Tester("t", "p").getPathName(), "p");

    // Test regular modifiers:
    TS_ASSERT_THROWS(Tester("a", "").renameTo("b"), afl::except::FileProblemException);
    TS_ASSERT_THROWS(Tester("a", "").erase(), afl::except::FileProblemException);
    TS_ASSERT_THROWS(Tester("a", "").createAsDirectory(), afl::except::FileProblemException);
    TS_ASSERT_THROWS(Tester("a", "").setFlag(afl::io::DirectoryEntry::Executable, true), afl::except::FileProblemException);
    TS_ASSERT_THROWS(Tester("a", "").openFile(afl::io::FileSystem::Create), afl::except::FileProblemException);

    // Test NT modifiers:
    TS_ASSERT(!Tester("a", "").renameToNT("b"));
    TS_ASSERT(!Tester("a", "").eraseNT());

    // Test successful opening
    TS_ASSERT_THROWS_NOTHING(Tester("a", "").openFile(afl::io::FileSystem::OpenRead));

    // Test correct exception content
    // - if path and title are known, uses the path
    try {
        Tester("t", "p").erase();
        TS_ASSERT(0);
    }
    catch (afl::except::FileProblemException& fpe) {
        TS_ASSERT_EQUALS(fpe.getFileName(), "p");
        TS_ASSERT_EQUALS(String_t(fpe.what()), "fail!");
    }
    catch (...) {
        TS_ASSERT(0);
    }

    // - if path is not known, uses the title
    try {
        Tester("t", "").erase();
        TS_ASSERT(0);
    }
    catch (afl::except::FileProblemException& fpe) {
        TS_ASSERT_EQUALS(fpe.getFileName(), "t");
        TS_ASSERT_EQUALS(String_t(fpe.what()), "fail!");
    }
    catch (...) {
        TS_ASSERT(0);
    }
}

