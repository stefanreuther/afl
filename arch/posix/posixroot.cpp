/**
  *  \file arch/posix/posixroot.cpp
  *  \brief Class arch::posix::PosixRoot
  */

#include <cstdlib>
#if TARGET_OS_POSIX
#include "arch/posix/posixroot.hpp"

#include <cassert>
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directoryentry.hpp"
#include "arch/posix/posixdirectory.hpp"
#include "afl/string/messages.hpp"


namespace {
    enum RootEntry {
        FileSystemRoot,
        UserHome,
        LastRootEntry
    };
}

/** DirectoryEntry implementation for POSIX root. */
class arch::posix::PosixRoot::Entry : public afl::io::DirectoryEntry {
 public:
    Entry(afl::base::Ref<PosixRoot> parent, RootEntry which);
    ~Entry();
    virtual String_t getTitle();
    virtual String_t getPathName();
    virtual afl::base::Ref<afl::io::Stream> openFile(afl::io::FileSystem::OpenMode mode);
    virtual afl::base::Ref<afl::io::Directory> openDirectory();
    virtual afl::base::Ref<afl::io::Directory> openContainingDirectory();
    virtual void updateInfo(uint32_t requested);
    virtual void doRename(String_t newName);
    virtual void doErase();
    virtual void doCreateAsDirectory();
    virtual void doSetFlag(FileFlag flag, bool value);
    virtual void doMoveTo(Directory& dir, String_t name);

 private:
    afl::base::Ref<PosixRoot> m_parent;
    RootEntry m_which;
    void fail();
};

/** DirectoryEntry enumerator implementation for POSIX root. */
class arch::posix::PosixRoot::Enum : public afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > {
 public:
    Enum(afl::base::Ref<PosixRoot> dir);
    virtual ~Enum();
    virtual bool getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result);

 private:
    RootEntry m_pos;
    afl::base::Ref<PosixRoot> m_dir;
};


/**************************** PosixRoot::Entry ***************************/

arch::posix::PosixRoot::Entry::Entry(afl::base::Ref<PosixRoot> parent, RootEntry which)
    : m_parent(parent),
      m_which(which)
{ }

arch::posix::PosixRoot::Entry::~Entry()
{ }

String_t
arch::posix::PosixRoot::Entry::getTitle()
{
    switch (m_which) {
     case FileSystemRoot:
        return afl::string::Messages::rootLinkTitle();
     case UserHome:
        return afl::string::Messages::homeLinkTitle();
     case LastRootEntry:
        // Cannot happen
        break;
    }
    return String_t();
}

String_t
arch::posix::PosixRoot::Entry::getPathName()
{
    return getLinkText();
}

afl::base::Ref<afl::io::Stream>
arch::posix::PosixRoot::Entry::openFile(afl::io::FileSystem::OpenMode /*mode*/)
{
    while (1) {
        fail();
    }
}

afl::base::Ref<afl::io::Directory>
arch::posix::PosixRoot::Entry::openDirectory()
{
    return *new PosixDirectory(getLinkText());
}

afl::base::Ref<afl::io::Directory>
arch::posix::PosixRoot::Entry::openContainingDirectory()
{
    return m_parent;
}

void
arch::posix::PosixRoot::Entry::updateInfo(uint32_t /*requested*/)
{
    switch (m_which) {
     case FileSystemRoot:
        setFileType(tDirectory);
        setFlags(FileFlags_t(Link));
        setLinkText("/");
        break;

     case UserHome:
        {
            const char* home = ::getenv("HOME");
            if (home == 0 || *home == '\0') {
                home = "/";
            }
            setFileType(tDirectory);
            setFlags(FileFlags_t(Link));
            setLinkText(home);
        }
        break;

     case LastRootEntry:
        break;
    }
}

void
arch::posix::PosixRoot::Entry::doRename(String_t /*newName*/)
{
    fail();
}

void
arch::posix::PosixRoot::Entry::doErase()
{
    fail();
}

void
arch::posix::PosixRoot::Entry::doCreateAsDirectory()
{
    fail();
}

void
arch::posix::PosixRoot::Entry::doSetFlag(FileFlag /*flag*/, bool /*value*/)
{
    fail();
}

void
arch::posix::PosixRoot::Entry::doMoveTo(Directory& /*dir*/, String_t /*name*/)
{
    fail();
}

void
arch::posix::PosixRoot::Entry::fail()
{
    throw afl::except::FileProblemException(m_parent->getTitle(), afl::string::Messages::cannotAccessFiles());
}



/**************************** PosixRoot::Enum ****************************/

arch::posix::PosixRoot::Enum::Enum(afl::base::Ref<arch::posix::PosixRoot> dir)
    : m_pos(FileSystemRoot),
      m_dir(dir)
{ }

arch::posix::PosixRoot::Enum::~Enum()
{ }

bool
arch::posix::PosixRoot::Enum::getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result)
{
    if (m_pos < LastRootEntry) {
        result = new Entry(m_dir, m_pos);
        m_pos = static_cast<RootEntry>(m_pos + 1);
        return true;
    } else {
        return false;
    }
}


/******************************* PosixRoot *******************************/

arch::posix::PosixRoot::PosixRoot()
{ }

arch::posix::PosixRoot::~PosixRoot()
{ }

afl::base::Ref<afl::io::DirectoryEntry>
arch::posix::PosixRoot::getDirectoryEntryByName(String_t name)
{
    throw afl::except::FileProblemException(name, afl::string::Messages::cannotAccessFiles());
}

afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > >
arch::posix::PosixRoot::getDirectoryEntries()
{
    return *new Enum(*this);
}

afl::base::Ptr<afl::io::Directory>
arch::posix::PosixRoot::getParentDirectory()
{
    return 0;
}

String_t
arch::posix::PosixRoot::getDirectoryName()
{
    return String_t();
}

String_t
arch::posix::PosixRoot::getTitle()
{
    return afl::string::Messages::rootDirTitle();
}
#else
int g_variableToMakePosixRootObjectFileNotEmpty;
#endif
