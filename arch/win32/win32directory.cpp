/**
  *  \file arch/win32/win32directory.cpp
  *  \brief Class arch::win32::Win32Directory
  */

#if TARGET_OS_WIN32
#include "arch/win32/win32directory.hpp"

#undef NOMINMAX
#define NOMINMAX
#include <windows.h>
#include <wchar.h>
#include "arch/win32/win32filesystem.hpp"
#include "afl/io/directoryentry.hpp"
#include "arch/win32/win32.hpp"
#include "arch/win32/win32time.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/except/filesystemexception.hpp"
#include "afl/string/messages.hpp"
#include "afl/sys/error.hpp"

/** DirectoryEntry implementation for Win32. */
class arch::win32::Win32Directory::Entry : public afl::io::DirectoryEntry {
 public:
    Entry(afl::base::Ref<Win32Directory> parent, String_t name);
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

    void setFlagsAndFileType(DWORD attr);

 private:
    afl::base::Ref<Win32Directory> m_parent;
    String_t m_name;
};

/** DirectoryEntry enumerator implementation for Win32, Unicode version. */
class arch::win32::Win32Directory::EnumW : public afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > {
 public:
    EnumW(afl::base::Ref<Win32Directory> dir);
    virtual ~EnumW();
    virtual bool getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result);

 private:
    afl::base::Ref<Win32Directory> m_dir;
    WIN32_FIND_DATAW m_data;
    HANDLE m_handle;
    afl::base::Ptr<afl::io::DirectoryEntry> m_currentEntry;

    void next(bool advance);
};

/** DirectoryEntry enumerator implementation for Win32, ANSI version. */
class arch::win32::Win32Directory::EnumA : public afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > {
 public:
    EnumA(afl::base::Ref<Win32Directory> dir);
    virtual ~EnumA();
    virtual bool getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result);

 private:
    afl::base::Ref<Win32Directory> m_dir;
    WIN32_FIND_DATAA m_data;
    HANDLE m_handle;
    afl::base::Ptr<afl::io::DirectoryEntry> m_currentEntry;

    void next(bool advance);
};


/************************* Win32Directory::Entry *************************/

inline
arch::win32::Win32Directory::Entry::Entry(afl::base::Ref<Win32Directory> parent, String_t name)
    : m_parent(parent),
      m_name(name)
{ }

String_t
arch::win32::Win32Directory::Entry::getTitle()
{
    return m_name;
}

String_t
arch::win32::Win32Directory::Entry::getPathName()
{
    return Win32FileSystem().makePathName(m_parent->getDirectoryName(), m_name);
}

afl::base::Ref<afl::io::Stream>
arch::win32::Win32Directory::Entry::openFile(afl::io::FileSystem::OpenMode mode)
{
    return Win32FileSystem().openFile(getPathName(), mode);
}

afl::base::Ref<afl::io::Directory>
arch::win32::Win32Directory::Entry::openDirectory()
{
    return Win32FileSystem().openDirectory(getPathName());
}

afl::base::Ref<afl::io::Directory>
arch::win32::Win32Directory::Entry::openContainingDirectory()
{
    return m_parent;
}

void
arch::win32::Win32Directory::Entry::updateInfo(uint32_t requested)
{
    // Obtain information
    if ((requested & (InfoSize | InfoType | InfoModificationTime | InfoFlags)) != 0) {
        if (hasUnicodeSupport()) {
            // Convert name
            WStr wname;
            convertToUnicode(wname, afl::string::toMemory(getPathName()));
            wname.push_back(L'\0');

            // Do syscall
            WIN32_FIND_DATAW data;
            HANDLE h = FindFirstFileW(&wname[0], &data);
            if (h != INVALID_HANDLE_VALUE) {
                setFlagsAndFileType(data.dwFileAttributes);
                setFileSize((FileSize_t(data.nFileSizeHigh) << 32) + data.nFileSizeLow);
                setModificationTime(afl::sys::Time(Win32Time::fromFileTime(data.ftLastWriteTime)));
                FindClose(h);
            }
        } else {
            // Convert name
            String_t aname = convertToANSI(afl::string::toMemory(getPathName()));

            // Query kernel
            WIN32_FIND_DATAA data;
            HANDLE h = FindFirstFileA(aname.c_str(), &data);
            if (h != INVALID_HANDLE_VALUE) {
                setFlagsAndFileType(data.dwFileAttributes);
                setFileSize((FileSize_t(data.nFileSizeHigh) << 32) + data.nFileSizeLow);
                setModificationTime(afl::sys::Time(Win32Time::fromFileTime(data.ftLastWriteTime)));
                FindClose(h);
            }
        }
    }
}

void
arch::win32::Win32Directory::Entry::doRename(String_t newName)
{
    // Convert path names
    bool success;
    const afl::io::FileSystem::FileName_t utfOldName = getPathName();
    const afl::io::FileSystem::FileName_t utfNewName =
        Win32FileSystem().makePathName(m_parent->getDirectoryName(), newName);
    
    if (hasUnicodeSupport()) {
        WStr uniOldName, uniNewName;
        convertToUnicode(uniOldName, afl::string::toMemory(utfOldName));
        convertToUnicode(uniNewName, afl::string::toMemory(utfNewName));
        uniOldName.push_back(L'\0');
        uniNewName.push_back(L'\0');
        success = MoveFileW(&uniOldName[0], &uniNewName[0]);
    } else {
        success = MoveFileA(convertToANSI(afl::string::toMemory(utfOldName)).c_str(), convertToANSI(afl::string::toMemory(utfNewName)).c_str());
    }
    if (!success) {
        throw afl::except::FileSystemException(utfOldName, afl::sys::Error::current());
    }
}

void
arch::win32::Win32Directory::Entry::doErase()
{
    // This will try RemoveDirectory when the file does not exist,
    // because GetFileAttributes will then return 0xFFFFFFFF.
    // RemoveDirectory will hopefully produce a nice error message.
    bool success;
    const String_t utfName(getPathName());
    if (hasUnicodeSupport()) {
        WStr wname;
        convertToUnicode(wname, afl::string::toMemory(utfName));
        wname.push_back(L'\0');
        if (GetFileAttributesW(&wname[0]) & FILE_ATTRIBUTE_DIRECTORY) {
            success = RemoveDirectoryW(&wname[0]);
        } else {
            success = DeleteFileW(&wname[0]);
        }
    } else {
        String_t aname = convertToANSI(afl::string::toMemory(utfName));
        if (GetFileAttributesA(aname.c_str()) & FILE_ATTRIBUTE_DIRECTORY) {
            success = RemoveDirectoryA(aname.c_str());
        } else {
            success = DeleteFileA(aname.c_str());
        }
    }
    if (!success) {
        throw afl::except::FileSystemException(utfName, afl::sys::Error::current());
    }
}

void
arch::win32::Win32Directory::Entry::doCreateAsDirectory()
{
    bool success;
    const String_t utfName(getPathName());
    if (hasUnicodeSupport()) {
        WStr wname;
        convertToUnicode(wname, afl::string::toMemory(utfName));
        wname.push_back(L'\0');
        success = CreateDirectoryW(&wname[0], 0);
    } else {
        String_t aname = convertToANSI(afl::string::toMemory(utfName));
        success = CreateDirectoryA(aname.c_str(), 0);
    }
    if (!success) {
        throw afl::except::FileSystemException(utfName, afl::sys::Error::current());
    }
}

void
arch::win32::Win32Directory::Entry::doSetFlag(FileFlag flag, bool value)
{
    // FIXME: changing the "hidden" bit is actually supported
    switch (flag) {
     case Hidden:
     case Link:
     case Executable:
        throw afl::except::FileProblemException(getPathName(), afl::string::Messages::invalidOperation());
        break;
    }
}

void
arch::win32::Win32Directory::Entry::setFlagsAndFileType(DWORD attr)
{
    if (attr & FILE_ATTRIBUTE_DIRECTORY) {
        setFileType(tDirectory);
    } else {
        setFileType(tFile);
    }

    FileFlags_t flags;
    if (attr & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) {
        flags += Hidden;
    }
    setFlags(flags);
}


/************************** Win32Directory::Enum *************************/


inline
arch::win32::Win32Directory::EnumW::EnumW(afl::base::Ref<Win32Directory> dir)
    : m_dir(dir),
      m_data(),
      m_handle(INVALID_HANDLE_VALUE),
      m_currentEntry()
{
    WStr wname;
    convertToUnicode(wname, afl::string::toMemory(Win32FileSystem().makePathName(dir->getDirectoryName(), "*")));
    wname.push_back('\0');

    m_handle = FindFirstFileW(&wname[0], &m_data);
    if (m_handle == INVALID_HANDLE_VALUE) {
        throw afl::except::FileSystemException(dir->getDirectoryName(), afl::sys::Error::current());
    }
    next(false);
}

arch::win32::Win32Directory::EnumW::~EnumW()
{
    if (m_handle != INVALID_HANDLE_VALUE) {
        FindClose(m_handle);
    }
}

bool
arch::win32::Win32Directory::EnumW::getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result)
{
    result = m_currentEntry;
    next(true);
    return result.get() != 0;
}

void
arch::win32::Win32Directory::EnumW::next(bool advance)
{
    // Fetch an entry. Skip "." and "..".
    while (1) {
        // End reached?
        if (m_handle == INVALID_HANDLE_VALUE) {
            m_currentEntry = 0;
            break;
        }

        // Do we need to read an entry?
        if (advance && !FindNextFileW(m_handle, &m_data)) {
            // End of directory reached
            FindClose(m_handle);
            m_handle = INVALID_HANDLE_VALUE;
            m_currentEntry = 0;
            break;
        }

        // OK, we did read an entry. Do we like it?
        if (wcscmp(m_data.cFileName, L".") != 0 && wcscmp(m_data.cFileName, L"..") != 0) {
            // OK, create entry
            m_currentEntry = new Entry(m_dir, convertFromUnicode(afl::base::Memory<const wchar_t>::unsafeCreate(m_data.cFileName, wcslen(m_data.cFileName))));
            break;
        }
        advance = true;
    }
}


/************************** Win32Directory::EnumA *************************/


inline
arch::win32::Win32Directory::EnumA::EnumA(afl::base::Ref<Win32Directory> dir)
    : m_dir(dir),
      m_data(),
      m_handle(INVALID_HANDLE_VALUE),
      m_currentEntry()
{
    String_t aname = convertToANSI(afl::string::toMemory(Win32FileSystem().makePathName(dir->getDirectoryName(), "*")));

    m_handle = FindFirstFileA(aname.c_str(), &m_data);
    if (m_handle == INVALID_HANDLE_VALUE) {
        throw afl::except::FileSystemException(dir->getDirectoryName(), afl::sys::Error::current());
    }
    next(false);
}

arch::win32::Win32Directory::EnumA::~EnumA()
{
    if (m_handle != INVALID_HANDLE_VALUE) {
        FindClose(m_handle);
    }
}

bool
arch::win32::Win32Directory::EnumA::getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result)
{
    result = m_currentEntry;
    next(true);
    return result.get() != 0;
}

void
arch::win32::Win32Directory::EnumA::next(bool advance)
{
    // Fetch an entry. Skip "." and "..".
    while (1) {
        // End reached?
        if (m_handle == INVALID_HANDLE_VALUE) {
            m_currentEntry = 0;
            break;
        }

        // Do we need to read an entry?
        if (advance && !FindNextFileA(m_handle, &m_data)) {
            // End of directory reached
            FindClose(m_handle);
            m_handle = INVALID_HANDLE_VALUE;
            m_currentEntry = 0;
            break;
        }

        // OK, we did read an entry. Do we like it?
        if (strcmp(m_data.cFileName, ".") != 0 && strcmp(m_data.cFileName, "..") != 0) {
            // OK, create entry
            m_currentEntry = new Entry(m_dir, convertFromANSI(afl::string::toMemory(m_data.cFileName)));
            break;
        }
        advance = true;
    }
}


/***************************** Win32Directory ****************************/

arch::win32::Win32Directory::Win32Directory(String_t dirName)
    : afl::io::Directory(),
      m_dirName(dirName)
{ }

arch::win32::Win32Directory::~Win32Directory()
{ }

afl::base::Ref<afl::io::DirectoryEntry>
arch::win32::Win32Directory::getDirectoryEntryByName(String_t name)
{
    return *new Entry(*this, name);
}

afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > >
arch::win32::Win32Directory::getDirectoryEntries()
{
    if (hasUnicodeSupport()) {
        return *new EnumW(*this);
    } else {
        return *new EnumA(*this);
    }
}

afl::base::Ptr<afl::io::Directory>
arch::win32::Win32Directory::getParentDirectory()
{
    Win32FileSystem fs;
    String_t absName = fs.getAbsolutePathName(m_dirName);
    if (absName.size() <= 3) {
        return 0;
    } else {
        return new Win32Directory(fs.getCanonicalPathName(fs.makePathName(absName, "..")));
    }
}

String_t
arch::win32::Win32Directory::getDirectoryName()
{
    return m_dirName;
}

String_t
arch::win32::Win32Directory::getTitle()
{
    return Win32FileSystem().getFileName(m_dirName);
}
#else
int g_variableToMakeWin32DirectoryObjectFileNotEmpty;
#endif
