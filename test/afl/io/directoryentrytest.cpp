/**
  *  \file test/afl/io/directoryentrytest.cpp
  *  \brief Test for afl::io::DirectoryEntry
  */

#include "afl/io/directoryentry.hpp"

#include <stdexcept>
#include "afl/base/optional.hpp"
#include "afl/base/ptr.hpp"
#include "afl/test/testrunner.hpp"

using afl::base::Optional;
using afl::io::Directory;
using afl::io::DirectoryEntry;
using afl::io::FileSystem;
using afl::io::Stream;

/** Interface test. */
AFL_TEST_NOARG("afl.io.DirectoryEntry:interface")
{
    class Tester : public DirectoryEntry {
     public:
        virtual String_t getTitle()
            { return String_t(); }
        virtual String_t getPathName()
            { return String_t(); }
        virtual afl::base::Ref<Stream> openFile(FileSystem::OpenMode /*mode*/)
            { throw std::runtime_error("no ref"); }
        virtual afl::base::Ref<Directory> openDirectory()
            { throw std::runtime_error("no ref"); }
        virtual afl::base::Ref<Directory> openContainingDirectory()
            { throw std::runtime_error("no ref"); }
        virtual void updateInfo(uint32_t /*requested*/)
            { }
        virtual void doRename(String_t /*newName*/)
            { }
        virtual void doErase()
            { }
        virtual void doCreateAsDirectory()
            { }
        virtual void doSetFlag(FileFlag /*flag*/, bool /*value*/)
            { }
        virtual void doMoveTo(Directory& /*dir*/, String_t /*name*/)
            { }
    };
    Tester t;
}

/*
 *  Test information acquisition
 */

namespace {
    class MyDirectoryEntry : public DirectoryEntry {
     public:
        // public methods
        virtual String_t getTitle()
            { return "title"; }
        virtual String_t getPathName()
            { return "path name"; }
        virtual afl::base::Ref<Stream> openFile(FileSystem::OpenMode /*mode*/)
            { throw std::runtime_error("unexpected: openFile"); }
        virtual afl::base::Ref<Directory> openDirectory()
            { throw std::runtime_error("unexpected: openDirectory"); }
        virtual afl::base::Ref<Directory> openContainingDirectory()
            { throw std::runtime_error("unexpected: openContainingDirectory"); }

        // protected methods
        virtual void updateInfo(uint32_t requested)
            {
                if ((requested & InfoType) != 0) {
                    if (const FileType* t = m_type.get()) {
                        setFileType(*t);
                    }
                }
                if ((requested & InfoSize) != 0) {
                    if (const FileSize_t* sz = m_fileSize.get()) {
                        setFileSize(*sz);
                    }
                }
                if ((requested & InfoLinkText) != 0) {
                    if (const String_t* lt = m_linkText.get()) {
                        setLinkText(*lt);
                    }
                }
                if ((requested & InfoModificationTime) != 0) {
                    if (const afl::sys::Time* t = m_modificationTime.get()) {
                        setModificationTime(*t);
                    }
                }
                if ((requested & InfoFlags) != 0) {
                    if (const FileFlags_t* fs = m_flags.get()) {
                        setFlags(*fs);
                    }
                }
            }
        virtual void doRename(String_t /*newName*/)
            { throw std::runtime_error("unexpected: doRename"); }
        virtual void doErase()
            { /* This method is not blocked; it is used for re-acquisition tests. */ }
        virtual void doCreateAsDirectory()
            { throw std::runtime_error("unexpected: doCreateAsDirectory"); }
        virtual void doSetFlag(FileFlag /*flag*/, bool /*value*/)
            { throw std::runtime_error("unexpected: doSetFlag"); }
        virtual void doMoveTo(Directory& /*dir*/, String_t /*name*/)
            { throw std::runtime_error("unexpected: doMoveTo"); }

        Optional<FileType> m_type;
        Optional<FileSize_t> m_fileSize;
        Optional<String_t> m_linkText;
        Optional<afl::sys::Time> m_modificationTime;
        Optional<FileFlags_t> m_flags;
    };
}

// Test with blank DirectoryEntry. Unset properties get defaults.
AFL_TEST("afl.io.DirectoryEntry:info:blank", a)
{
    MyDirectoryEntry de;
    a.check     ("getFileType", de.getFileType() == DirectoryEntry::tUnknown);
    a.checkEqual("getFileSize", de.getFileSize(), Stream::FileSize_t(0));
    a.checkEqual("getLinkText", de.getLinkText(), "");
    a.check     ("getModificationTime", de.getModificationTime() == afl::sys::Time());
    a.check     ("getFlags", de.getFlags().empty());
}

// Test with partially configured DirectoryEntry
AFL_TEST("afl.io.DirectoryEntry:info:partial", a)
{
    MyDirectoryEntry de;
    de.m_type = de.tFile;
    de.m_linkText = "behind you, a three-headed monkey";
    de.m_flags = DirectoryEntry::FileFlags_t(DirectoryEntry::Link);
    a.check     ("getFileSize", de.getFileType() == DirectoryEntry::tFile);
    a.checkEqual("getFileSize", de.getFileSize(), Stream::FileSize_t(0));
    a.checkEqual("getLinkText", de.getLinkText(), "behind you, a three-headed monkey");
    a.check     ("getModificationTime", de.getModificationTime() == afl::sys::Time());
    a.check     ("getFlags", de.getFlags() == DirectoryEntry::FileFlags_t(DirectoryEntry::Link));
}

// Test re-acquisition after modification
AFL_TEST("afl.io.DirectoryEntry:info:cache-invalidation", a)
{
    MyDirectoryEntry de;
    de.m_fileSize = 999;
    a.checkEqual("initial value", de.getFileSize(), Stream::FileSize_t(999));

    // No change to cached data
    de.m_fileSize = 888;
    a.checkEqual("cached value", de.getFileSize(), Stream::FileSize_t(999));

    // Modify; this will re-fetch data
    de.erase();
    a.checkEqual("re-acquired value", de.getFileSize(), Stream::FileSize_t(888));
}
