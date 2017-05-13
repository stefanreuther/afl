/**
  *  \file arch/win32/win32root.cpp
  *  \brief Class arch::win32::Win32Root
  */

#if TARGET_OS_WIN32
#include "arch/win32/win32root.hpp"

#undef NOMINMAX
#define NOMINMAX
#include <windows.h>
#include <shlobj.h>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <vector>
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directoryentry.hpp"
#include "arch/win32/win32directory.hpp"
#include "arch/win32/win32.hpp"
#include "afl/string/messages.hpp"

/** DirectoryEntry implementation for Win32 root. */
class arch::win32::Win32Root::Entry : public afl::io::DirectoryEntry {
 public:
    Entry(afl::base::Ref<Win32Root> parent, String_t title, String_t path, FileType type, FileFlags_t flags);
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

 private:
    afl::base::Ref<Win32Root> m_parent;
    String_t m_title;
    String_t m_path;
    FileType m_type;
    FileFlags_t m_flags;

    void fail();
};

/** DirectoryEntry enumerator implementation for Win32 root. */
class arch::win32::Win32Root::Enum : public afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > {
 public:
    Enum(afl::base::Ref<Win32Root> dir);
    virtual ~Enum();
    virtual bool getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result);

 private:
    enum State {
        sDesktop,
        sMyFiles,
        sDrives,
        sDone
    };
    State m_state;
    String_t m_desktopPath;
    String_t m_myFilesPath;
    std::vector<char> m_driveList;
    size_t m_driveListIndex;
    afl::base::Ref<Win32Root> m_dir;
    afl::base::Ptr<Entry> m_currentEntry;

    void next();
};


/**************************** Win32Root::Entry ***************************/

arch::win32::Win32Root::Entry::Entry(afl::base::Ref<Win32Root> parent, String_t title, String_t path, FileType type, FileFlags_t flags)
    : m_parent(parent),
      m_title(title),
      m_path(path),
      m_type(type),
      m_flags(flags)
{ }

arch::win32::Win32Root::Entry::~Entry()
{ }

String_t
arch::win32::Win32Root::Entry::getTitle()
{
    return m_title;
}

String_t
arch::win32::Win32Root::Entry::getPathName()
{
    return m_path;
}

afl::base::Ref<afl::io::Stream>
arch::win32::Win32Root::Entry::openFile(afl::io::FileSystem::OpenMode /*mode*/)
{
    while (1) {
        fail();
    }
}

afl::base::Ref<afl::io::Directory>
arch::win32::Win32Root::Entry::openDirectory()
{
    return *new Win32Directory(m_path);
}

afl::base::Ref<afl::io::Directory>
arch::win32::Win32Root::Entry::openContainingDirectory()
{
    return m_parent;
}

void
arch::win32::Win32Root::Entry::updateInfo(uint32_t /*requested*/)
{
    setFileType(m_type);
    setLinkText(m_path);
    setFlags(m_flags);
}

void
arch::win32::Win32Root::Entry::doRename(String_t /*newName*/)
{
    fail();
}

void
arch::win32::Win32Root::Entry::doErase()
{
    fail();
}

void
arch::win32::Win32Root::Entry::doCreateAsDirectory()
{
    fail();
}

void
arch::win32::Win32Root::Entry::doSetFlag(FileFlag /*flag*/, bool /*value*/)
{
    fail();
}

void
arch::win32::Win32Root::Entry::fail()
{
    throw afl::except::FileProblemException(m_parent->getTitle(), afl::string::Messages::cannotAccessFiles());
}



/**************************** Win32Root::Enum ****************************/

arch::win32::Win32Root::Enum::Enum(afl::base::Ref<arch::win32::Win32Root> dir)
    : m_state(sDesktop),
      m_desktopPath(getShellFolder(CSIDL_DESKTOPDIRECTORY)),
      m_myFilesPath(getShellFolder(CSIDL_PERSONAL)),
      m_driveList(),
      m_driveListIndex(),
      m_dir(dir),
      m_currentEntry()
{
    char dummy;
    DWORD count = GetLogicalDriveStrings(0, &dummy);
    m_driveList.resize(count+1);
    if (count != 0) {
        GetLogicalDriveStrings(count, &m_driveList[0]);
    }
    next();
}

arch::win32::Win32Root::Enum::~Enum()
{ }

bool
arch::win32::Win32Root::Enum::getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result)
{
    result = m_currentEntry;
    next();
    return result.get() != 0;
}

void
arch::win32::Win32Root::Enum::next()
{
    bool found = false;
    while (!found) {
        switch (m_state) {
         case sDesktop:
            if (!m_desktopPath.empty()) {
                m_currentEntry = new Entry(m_dir, afl::string::Messages::desktopLinkTitle(), m_desktopPath, Entry::tDirectory, Entry::FileFlags_t(Entry::Link));
                found = true;
            }
            m_state = sMyFiles;
            break;

         case sMyFiles:
            if (!m_myFilesPath.empty()) {
                m_currentEntry = new Entry(m_dir, afl::string::Messages::myFilesLinkTitle(), m_myFilesPath, Entry::tDirectory, Entry::FileFlags_t(Entry::Link));
                found = true;
            }
            m_state = sDrives;
            break;

         case sDrives:
            if (m_driveListIndex < m_driveList.size() && m_driveList[m_driveListIndex] != '\0') {
                // This is a hack to avoid including unready drives in the root list.
                // Doing so would stress the unready drives even more if some recursive walker would see the drive and examine it.
                // In the end, this even makes behaviour a little more consistent:
                // disconnected USB drives don't have a drive letter and are not listed, so why should "disconnected" floppies be shown?
                DWORD a, b, c, d;
                const char* driveName = &m_driveList[m_driveListIndex];
                if (GetDiskFreeSpace(driveName, &a, &b, &c, &d)) {
                    m_currentEntry = new Entry(m_dir, driveName, driveName, Entry::tRoot, Entry::FileFlags_t());
                    found = true;
                }
                m_driveListIndex += std::strlen(driveName)+1;
            } else {
                m_state = sDone;
            }
            break;

         case sDone:
            m_currentEntry = 0;
            found = true;
            break;
        }
    }
}


/******************************* Win32Root *******************************/

arch::win32::Win32Root::Win32Root()
{ }

arch::win32::Win32Root::~Win32Root()
{ }

afl::base::Ref<afl::io::DirectoryEntry>
arch::win32::Win32Root::getDirectoryEntryByName(String_t name)
{
    throw afl::except::FileProblemException(name, afl::string::Messages::cannotAccessFiles());
}

afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > >
arch::win32::Win32Root::getDirectoryEntries()
{
    return *new Enum(*this);
}

afl::base::Ptr<afl::io::Directory>
arch::win32::Win32Root::getParentDirectory()
{
    return 0;
}

String_t
arch::win32::Win32Root::getDirectoryName()
{
    return String_t();
}

String_t
arch::win32::Win32Root::getTitle()
{
    return afl::string::Messages::rootDirTitle();
}
#else
int g_variableToMakeWin32RootObjectFileNotEmpty;
#endif
