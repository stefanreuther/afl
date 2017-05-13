/**
  *  \file u/t_io_directoryentry.cpp
  *  \brief Test for afl::io::DirectoryEntry
  */

#include "afl/io/directoryentry.hpp"

#include "u/t_io.hpp"
#include "afl/base/ptr.hpp"
#include "afl/base/optional.hpp"

using afl::io::FileSystem;
using afl::io::Stream;
using afl::io::Directory;

/** Interface test. */
void
TestIoDirectoryEntry::testIt()
{
    class Tester : public afl::io::DirectoryEntry {
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
    };
    Tester t;
}

/** Test information acquisition. */
void
TestIoDirectoryEntry::testInfo()
{
    using afl::base::Optional;
    using afl::io::DirectoryEntry;

    class MyDirectoryEntry : public DirectoryEntry {
     public:
        // public methods
        virtual String_t getTitle()
            { return "title"; }
        virtual String_t getPathName()
            { return "path name"; }
        virtual afl::base::Ref<Stream> openFile(FileSystem::OpenMode /*mode*/)
            {
                TS_ASSERT(0);
                throw std::runtime_error("no ref");
            }
        virtual afl::base::Ref<Directory> openDirectory()
            {
                TS_ASSERT(0);
                throw std::runtime_error("no ref");
            }
        virtual afl::base::Ref<Directory> openContainingDirectory()
            {
                TS_ASSERT(0);
                throw std::runtime_error("no ref");
            }

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
            { TS_ASSERT(0); }
        virtual void doErase()
            { /* This method is not blocked; it is used for re-acquisition tests. */ }
        virtual void doCreateAsDirectory()
            { TS_ASSERT(0); }
        virtual void doSetFlag(FileFlag /*flag*/, bool /*value*/)
            { TS_ASSERT(0); }

        Optional<FileType> m_type;
        Optional<FileSize_t> m_fileSize;
        Optional<String_t> m_linkText;
        Optional<afl::sys::Time> m_modificationTime;
        Optional<FileFlags_t> m_flags;
    };

    // Test with blank DirectoryEntry. Unset properties get defaults.
    {
        MyDirectoryEntry de;
        TS_ASSERT_EQUALS(de.getFileType(), DirectoryEntry::tUnknown);
        TS_ASSERT_EQUALS(de.getFileSize(), Stream::FileSize_t(0));
        TS_ASSERT_EQUALS(de.getLinkText(), "");
        TS_ASSERT_EQUALS(de.getModificationTime(), afl::sys::Time());
        TS_ASSERT_EQUALS(de.getFlags(), DirectoryEntry::FileFlags_t());
    }

    // Test with partially configured DirectoryEntry
    {
        MyDirectoryEntry de;
        de.m_type = de.tFile;
        de.m_linkText = "behind you, a three-headed monkey";
        de.m_flags = DirectoryEntry::FileFlags_t(DirectoryEntry::Link);
        TS_ASSERT_EQUALS(de.getFileType(), DirectoryEntry::tFile);
        TS_ASSERT_EQUALS(de.getFileSize(), Stream::FileSize_t(0));
        TS_ASSERT_EQUALS(de.getLinkText(), "behind you, a three-headed monkey");
        TS_ASSERT_EQUALS(de.getModificationTime(), afl::sys::Time());
        TS_ASSERT_EQUALS(de.getFlags(), DirectoryEntry::FileFlags_t(DirectoryEntry::Link));
    }

    // Test re-acquisition after modification
    {
        MyDirectoryEntry de;
        de.m_fileSize = 999;
        TS_ASSERT_EQUALS(de.getFileSize(), Stream::FileSize_t(999));

        // No change to cached data
        de.m_fileSize = 888;
        TS_ASSERT_EQUALS(de.getFileSize(), Stream::FileSize_t(999));

        // Modify; this will re-fetch data
        de.erase();
        TS_ASSERT_EQUALS(de.getFileSize(), Stream::FileSize_t(888));
    }
}
