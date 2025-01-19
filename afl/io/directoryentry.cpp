/**
  *  \file afl/io/directoryentry.cpp
  *  \brief Base Class afl::io::DirectoryEntry
  */

#include "afl/io/directoryentry.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directory.hpp"
#include "afl/string/messages.hpp"

afl::io::DirectoryEntry::DirectoryEntry()
    : afl::base::Deletable(),
      afl::base::RefCounted(),
      m_availableInfo(0),
      m_requestedInfo(0),
      m_fileSize(0),
      m_fileType(tUnknown),
      m_linkText(),
      m_modificationTime(),
      m_flags()
{ }

afl::io::DirectoryEntry::~DirectoryEntry()
{ }

afl::io::DirectoryEntry::FileType
afl::io::DirectoryEntry::getFileType()
{
    return fetchInfo(InfoType)
        ? m_fileType
        : tUnknown;
}

afl::io::DirectoryEntry::FileSize_t
afl::io::DirectoryEntry::getFileSize()
{
    return fetchInfo(InfoSize)
        ? m_fileSize
        : 0;
}

String_t
afl::io::DirectoryEntry::getLinkText()
{
    return fetchInfo(InfoLinkText)
        ? m_linkText
        : String_t();
}

afl::sys::Time
afl::io::DirectoryEntry::getModificationTime()
{
    return fetchInfo(InfoModificationTime)
        ? m_modificationTime
        : afl::sys::Time();
}

afl::io::DirectoryEntry::FileFlags_t
afl::io::DirectoryEntry::getFlags()
{
    return fetchInfo(InfoFlags)
        ? m_flags
        : FileFlags_t();
}

void
afl::io::DirectoryEntry::renameTo(String_t newName)
{
    // Renaming will invalidate stored information.
    clearInfo();
    doRename(newName);
}

bool
afl::io::DirectoryEntry::renameToNT(String_t newName)
{
    try {
        renameTo(newName);
        return true;
    }
    catch (afl::except::FileProblemException& e) {
        (void) e;
        return false;
    }
}

void
afl::io::DirectoryEntry::moveTo(Directory& dir, String_t newName)
{
    clearInfo();
    doMoveTo(dir, newName);
}

bool
afl::io::DirectoryEntry::moveToNT(Directory& dir, String_t newName)
{
    try {
        moveTo(dir, newName);
        return true;
    }
    catch (afl::except::FileProblemException& e) {
        (void) e;
        return false;
    }
}

void
afl::io::DirectoryEntry::moveFileByCopying(Directory& dir, String_t newName)
{
    FileType t = getFileType();
    if (t != tFile && t != tArchive) {
        throw afl::except::FileProblemException(getPathName(), afl::string::Messages::invalidOperation());
    }

    try {
        moveTo(dir, newName);
    }
    catch (afl::except::FileProblemException& e) {
        // Copy
        {
            afl::base::Ref<Stream> in = openFile(FileSystem::OpenRead);
            afl::base::Ref<Stream> out = dir.openFile(newName, FileSystem::Create);
            out->copyFrom(*in);
            out->flush();
        }

        // Delete this
        erase();
    }
}

void
afl::io::DirectoryEntry::erase()
{
    // Erasing will invalidate stored information.
    clearInfo();
    doErase();
}

bool
afl::io::DirectoryEntry::eraseNT()
{
    try {
        erase();
        return true;
    }
    catch (afl::except::FileProblemException& e) {
        (void) e;
        return false;
    }
}

void
afl::io::DirectoryEntry::createAsDirectory()
{
    // Creating will invalidate stored information.
    clearInfo();
    doCreateAsDirectory();
}

void
afl::io::DirectoryEntry::setFlag(FileFlag flag, bool value)
{
    // This will invalidate stored information.
    clearInfo();
    doSetFlag(flag, value);
}

void
afl::io::DirectoryEntry::setFileType(FileType type)
{
    m_fileType = type;
    m_availableInfo |= InfoType;
}

void
afl::io::DirectoryEntry::setFileSize(FileSize_t size)
{
    m_fileSize = size;
    m_availableInfo |= InfoSize;
}

void
afl::io::DirectoryEntry::setLinkText(String_t text)
{
    m_linkText = text;
    m_availableInfo |= InfoLinkText;
}

void
afl::io::DirectoryEntry::setModificationTime(afl::sys::Time modificationTime)
{
    m_modificationTime = modificationTime;
    m_availableInfo |= InfoModificationTime;
}

void
afl::io::DirectoryEntry::setFlags(FileFlags_t flags)
{
    m_flags = flags;
    m_availableInfo |= InfoFlags;
}

void
afl::io::DirectoryEntry::clearInfo()
{
    m_requestedInfo = 0;
    m_availableInfo = 0;
}

void
afl::io::DirectoryEntry::copyInfo(DirectoryEntry& other, uint32_t requested)
{
    // Tell other entry to update itself
    other.updateInfo(requested);

    // Limit to information we actually got
    requested &= other.m_availableInfo;

    // Copy
    if ((requested & InfoType) != 0) {
        setFileType(other.getFileType());
    }
    if ((requested & InfoSize) != 0) {
        setFileSize(other.getFileSize());
    }
    if ((requested & InfoLinkText) != 0) {
        setLinkText(other.getLinkText());
    }
    if ((requested & InfoModificationTime) != 0) {
        setModificationTime(other.getModificationTime());
    }
    if ((requested & InfoFlags) != 0) {
        setFlags(other.getFlags());
    }
}

bool
afl::io::DirectoryEntry::fetchInfo(uint32_t requested)
{
    if ((m_availableInfo & requested) == requested) {
        // Information is available.
        return true;
    } else {
        if ((m_requestedInfo & requested) != requested) {
            // Information has not yet been requested. Do so.
            m_requestedInfo |= requested;
            updateInfo(requested);
        }

        return (m_availableInfo & requested) == requested;
    }
}
