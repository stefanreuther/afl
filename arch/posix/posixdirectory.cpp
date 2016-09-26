/**
  *  \file arch/posix/posixdirectory.cpp
  *  \brief Class arch::posix::PosixDirectory
  */

#if TARGET_OS_POSIX
#include "arch/posix/posixdirectory.hpp"

#include <cassert>
#include <cstring>
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>          // rename
#include <errno.h>
#include "arch/posix/posixfilesystem.hpp"
#include "afl/io/directoryentry.hpp"
#include "arch/posix/posix.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/except/filesystemexception.hpp"
#include "afl/sys/error.hpp"
#include "arch/posix/posixtime.hpp"
#include "afl/sys/time.hpp"

/** DirectoryEntry implementation for POSIX. */
class arch::posix::PosixDirectory::Entry : public afl::io::DirectoryEntry {
 public:
    Entry(afl::base::Ptr<PosixDirectory> parent, String_t name);
    virtual String_t getTitle();
    virtual String_t getPathName();
    virtual afl::base::Ptr<afl::io::Stream> openFile(afl::io::FileSystem::OpenMode mode);
    virtual afl::base::Ptr<afl::io::Directory> openDirectory();
    virtual afl::base::Ptr<afl::io::Directory> openContainingDirectory();
    virtual void updateInfo(uint32_t requested);
    virtual void doRename(String_t newName);
    virtual void doErase();
    virtual void doCreateAsDirectory();

 private:
    afl::base::Ptr<PosixDirectory> m_parent;
    String_t m_name;

    static FileType convertFileType(mode_t mode);
    static bool errorMeansIsDirectory(int err);
};

/** DirectoryEntry enumerator implementation for POSIX. */
class arch::posix::PosixDirectory::Enum : public afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > {
 public:
    Enum(afl::base::Ptr<PosixDirectory> dir);
    virtual ~Enum();
    virtual bool getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result);

 private:
    afl::base::Ptr<PosixDirectory> m_dir;
    DIR* m_dirHandle;
    afl::base::Ptr<afl::io::DirectoryEntry> m_currentEntry;

    void next();
};


/************************* PosixDirectory::Entry *************************/

inline
arch::posix::PosixDirectory::Entry::Entry(afl::base::Ptr<PosixDirectory> parent, String_t name)
    : m_parent(parent),
      m_name(name)
{ }

String_t
arch::posix::PosixDirectory::Entry::getTitle()
{
    return m_name;
}

String_t
arch::posix::PosixDirectory::Entry::getPathName()
{
    return PosixFileSystem().makePathName(m_parent->getDirectoryName(), m_name);
}

afl::base::Ptr<afl::io::Stream>
arch::posix::PosixDirectory::Entry::openFile(afl::io::FileSystem::OpenMode mode)
{
    return PosixFileSystem().openFile(getPathName(), mode);
}

afl::base::Ptr<afl::io::Directory>
arch::posix::PosixDirectory::Entry::openDirectory()
{
    return PosixFileSystem().openDirectory(getPathName());
}

afl::base::Ptr<afl::io::Directory>
arch::posix::PosixDirectory::Entry::openContainingDirectory()
{
    return m_parent;
}

void
arch::posix::PosixDirectory::Entry::updateInfo(uint32_t requested)
{
    // Convert path name
    const afl::io::FileSystem::FileName_t sysName = convertUtf8ToPathName(getPathName());

    // Obtain information
    if ((requested & (InfoSize | InfoType | InfoModificationTime | InfoFlags)) != 0) {
        // Read inode.
        struct stat st;
        bool isLink = false;
        bool ok = false;
        if (::lstat(sysName.c_str(), &st) == 0) {
            if (S_ISLNK(st.st_mode)) {
                isLink = true;
                ok = (::stat(sysName.c_str(), &st) == 0);
            } else {
                ok = true;
            }
        }

        if (ok) {
            setFileType(convertFileType(st.st_mode));
            setFileSize(st.st_size);
            setModificationTime(afl::sys::Time(PosixTime::fromSysTime(st.st_mtime, 0)));
        }

        // Build flags
        FileFlags_t flags;
        if (isLink) {
            flags += Link;
        }
        if (!m_name.empty() && m_name[0] == '.') {
            flags += Hidden;
        }
        setFlags(flags);
    }
    if ((requested & InfoLinkText) != 0) {
        // Read symbolic link.
        afl::base::GrowableMemory<char> buffer;
        if (readlinkWrap(sysName.c_str(), buffer)) {
            setLinkText(convertExternalToUtf8(buffer));
        }
    }
}

void
arch::posix::PosixDirectory::Entry::doRename(String_t newName)
{
    // Convert path names
    const afl::io::FileSystem::FileName_t uniOldName = getPathName();

    const afl::io::FileSystem::FileName_t sysOldName = convertUtf8ToPathName(uniOldName);
    const afl::io::FileSystem::FileName_t sysNewName =
        convertUtf8ToPathName(PosixFileSystem().makePathName(m_parent->getDirectoryName(), newName));

    // Do it
    if (::rename(sysOldName.c_str(), sysNewName.c_str()) != 0) {
        throw afl::except::FileSystemException(uniOldName, afl::sys::Error::current());
    }
}

void
arch::posix::PosixDirectory::Entry::doErase()
{
    // Convert path name
    const afl::io::FileSystem::FileName_t uniName = getPathName();
    const afl::io::FileSystem::FileName_t sysName = convertUtf8ToPathName(uniName);

    // Try unlinking. If that fails with an error indicating that it's a directory, try rmdir'ing.
    if (::unlink(sysName.c_str()) != 0
        && (!errorMeansIsDirectory(errno) || ::rmdir(sysName.c_str()) != 0))
    {
        throw afl::except::FileSystemException(uniName, afl::sys::Error::current());
    }
}

void
arch::posix::PosixDirectory::Entry::doCreateAsDirectory()
{
    // Convert path name
    const afl::io::FileSystem::FileName_t uniName = getPathName();
    const afl::io::FileSystem::FileName_t sysName = convertUtf8ToPathName(uniName);

    if (::mkdir(sysName.c_str(), 0777) != 0) {
        throw afl::except::FileSystemException(uniName, afl::sys::Error::current());
    }
}

inline arch::posix::PosixDirectory::Entry::FileType
arch::posix::PosixDirectory::Entry::convertFileType(mode_t mode)
{
    // Some of the S_ISXXX macros are not 100% portable.
    // For consistency, test all of them.
#ifdef S_ISREG
    if (S_ISREG(mode)) {
        return tFile;
    }
#endif
#ifdef S_ISDIR
    if (S_ISDIR(mode)) {
        return tDirectory;
    }
#endif
#ifdef S_ISCHR
    if (S_ISCHR(mode)) {
        return tDevice;
    }
#endif
#ifdef S_ISBLK
    if (S_ISBLK(mode)) {
        return tDevice;
    }
#endif
#ifdef S_ISFIFO
    if (S_ISFIFO(mode)) {
        // FIFOs are magic, treat them as devices
        return tDevice;
    }
#endif
#ifdef S_ISSOCK
    if (S_ISSOCK(mode)) {
        // Sockets are magic, treat them as devices
        return tDevice;
    }
#endif
    return tUnknown;
}

/** Check for error message meaning "this is a directory, use rmdir to get rid of it". */
inline bool
arch::posix::PosixDirectory::Entry::errorMeansIsDirectory(int err)
{
#ifdef EPERM
    // SUSv3 says unlink fails with EPERM when trying to unlink a directory.
    if (err == EPERM) {
        return true;
    }
#endif
#ifdef EISDIR
    // Linux says EISDIR in this case.
    if (err == EISDIR) {
        return true;
    }
#endif
    return false;
}



/************************** PosixDirectory::Enum *************************/

inline
arch::posix::PosixDirectory::Enum::Enum(afl::base::Ptr<PosixDirectory> dir)
    : m_dir(dir),
      m_dirHandle(0),
      m_currentEntry()
{
    // Convert path name
    const afl::io::FileSystem::FileName_t uniName = dir->getDirectoryName();
    const afl::io::FileSystem::FileName_t sysName = convertUtf8ToPathName(uniName);

    // Open directory
    m_dirHandle = ::opendir(sysName.c_str());
    if (m_dirHandle == 0) {
        throw afl::except::FileSystemException(uniName, afl::sys::Error::current());
    }

    // Make one step
    next();
}

inline
arch::posix::PosixDirectory::Enum::~Enum()
{
    if (m_dirHandle != 0) {
        ::closedir(m_dirHandle);
    }
}

bool
arch::posix::PosixDirectory::Enum::getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result)
{
    result = m_currentEntry;
    next();
    return result.get() != 0;
}

void
arch::posix::PosixDirectory::Enum::next()
{
    // Fetch an entry. Skip "." and "..".
    struct dirent* entry = 0;
    if (m_dirHandle != 0) {
        do {
            entry = ::readdir(m_dirHandle);
        } while (entry != 0 && (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0));
    }

    // Create associated directory entry
    if (entry == 0) {
        ::closedir(m_dirHandle);
        m_dirHandle = 0;
        m_currentEntry = 0;
    } else {
        m_currentEntry = new Entry(m_dir, convertExternalToUtf8(afl::string::toMemory(entry->d_name)));
    }
}


/***************************** PosixDirectory ****************************/

arch::posix::PosixDirectory::PosixDirectory(String_t dirName)
    : afl::io::Directory(),
      m_dirName(dirName)
{ }

arch::posix::PosixDirectory::~PosixDirectory()
{ }

afl::base::Ptr<afl::io::DirectoryEntry>
arch::posix::PosixDirectory::getDirectoryEntryByName(String_t name)
{
    return new Entry(this, name);
}

afl::base::Ptr<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > >
arch::posix::PosixDirectory::getDirectoryEntries()
{
    return new Enum(this);
}

afl::base::Ptr<afl::io::Directory>
arch::posix::PosixDirectory::getParentDirectory()
{
    PosixFileSystem fs;
    String_t absName = fs.getAbsolutePathName(m_dirName);
    if (absName.size() <= 1) {
        return 0;
    } else {
        return new PosixDirectory(fs.getCanonicalPathName(fs.makePathName(absName, "..")));
    }
}

String_t
arch::posix::PosixDirectory::getDirectoryName()
{
    return m_dirName;
}

String_t
arch::posix::PosixDirectory::getTitle()
{
    return PosixFileSystem().getFileName(m_dirName);
}
#else
int g_variableToMakePosixDirectoryObjectFileNotEmpty;
#endif
