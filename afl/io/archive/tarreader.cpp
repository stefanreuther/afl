/**
  *  \file afl/io/archive/tarreader.cpp
  *  \brief Class afl::io::archive::TarReader
  */

#include "afl/io/archive/tarreader.hpp"
#include "afl/io/stream.hpp"
#include "afl/io/directoryentry.hpp"
#include "afl/base/staticassert.hpp"
#include "afl/string/char.hpp"
#include "afl/except/fileformatexception.hpp"
#include "afl/string/messages.hpp"
#include "afl/except/filetooshortexception.hpp"
#include "afl/sys/time.hpp"
#include "afl/io/limitedstream.hpp"
#include "afl/string/posixfilenames.hpp"
#include "afl/io/filemapping.hpp"
#include "afl/io/unchangeabledirectoryentry.hpp"

using afl::io::Stream;

namespace {
    /*
     *  "ustar" header.
     *
     *  Specified on http://pubs.opengroup.org/onlinepubs/9699919799/utilities/pax.html
     *
     *  String fields are zero-terminated if they are less than their total size long.
     *  Numeric fields contain an octal number.
     */
    struct UstarHeader {
        char name[100];
        char mode[8];
        char uid[8];
        char gid[8];
        char size[12];
        char mtime[12];
        char chksum[8];
        char typeflag;
        char linkname[100];
        char magic[6];
        char version[2];
        char uname[32];
        char gname[32];
        char devmajor[8];
        char devminor[8];
        char prefix[155];
        char unused[12];
    };
    static_assert(sizeof(UstarHeader) == 512, "sizeof UstarHeader");

    /* Get string field.
       The standard says names are ASCII only.
       We are therefore just passing them through. */
    String_t getString(afl::string::ConstStringMemory_t m)
    {
        m.trim(m.find('\0'));
        return afl::string::fromMemory(m);
    }

    /* Get number field.
       The standard says leading-zero-filled; we allow for a little more flexibility. */
    uint64_t getNumber(afl::string::ConstStringMemory_t m, Stream& stream)
    {
        // Skip leading whitespace
        const char* p = m.eat();
        while (p != 0 && afl::string::charIsSpace(*p)) {
            p = m.eat();
        }

        // Parse number
        uint64_t result = 0;
        while (p != 0 && (*p >= '0' && *p <= '7')) {
            result = 8*result + (*p - '0');
            p = m.eat();
        }

        // Skip trailing whitespace
        while (p != 0 && afl::string::charIsSpace(*p)) {
            p = m.eat();
        }

        // If still not at end, fail
        if (p != 0 && *p != '\0') {
            throw afl::except::FileFormatException(stream, afl::string::Messages::invalidNumber());
        }
        return result;
    }

    /* Skip data in a stream by reading. */
    void skip(Stream& s, Stream::FileSize_t amount)
    {
        uint8_t buffer[4096];
        while (amount > 0) {
            afl::base::Bytes_t bytes(buffer);
            if (amount < bytes.size()) {
                bytes.trim(static_cast<size_t>(amount));
            }
            size_t n = s.read(bytes);
            if (n == 0) {
                throw afl::except::FileTooShortException(s);
            }
            amount -= n;
        }
    }
}

/** Index entry. */
struct afl::io::archive::TarReader::IndexEntry {
    String_t name;
    Stream::FileSize_t start;
    Stream::FileSize_t length;
    bool executable;
    afl::sys::Time modificationTime;

    IndexEntry(String_t name,
               Stream::FileSize_t start,
               Stream::FileSize_t length,
               bool executable,
               afl::sys::Time modificationTime)
        : name(name),
          start(start),
          length(length),
          executable(executable),
          modificationTime(modificationTime)
        { }
};


/** Tar "DirectoryEntry" implementation. */
class afl::io::archive::TarReader::TarDirEntry : public afl::io::UnchangeableDirectoryEntry {
 public:
    TarDirEntry(const IndexEntry& entry, afl::base::Ref<TarReader> parent);
    ~TarDirEntry();

    // DirectoryEntry:
    virtual String_t getTitle();
    virtual String_t getPathName();
    virtual afl::base::Ref<Stream> openFileForReading();
    virtual afl::base::Ref<Directory> openDirectory();
    virtual afl::base::Ref<Directory> openContainingDirectory();

    virtual void updateInfo(uint32_t requested);

 private:
    const IndexEntry& m_entry;
    afl::base::Ref<TarReader> m_parent;
};

/** Tar DirectoryEntry Enumerator implementation. */
class afl::io::archive::TarReader::TarDirEnum : public afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > {
 public:
    TarDirEnum(afl::base::Ref<TarReader> parent);
    ~TarDirEnum();

    virtual bool getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result);

 private:
    afl::base::Ref<TarReader> m_parent;
    size_t m_current;
};

/** Tar "Stream" implementation for a member.
    Essentially, this is a LimitedStream, plus some housekeeping. */
class afl::io::archive::TarReader::TarStoredMember : public afl::io::Stream {
 public:
    TarStoredMember(afl::base::Ref<Stream> file, const IndexEntry& entry);
    ~TarStoredMember();

    virtual String_t getName();
    virtual FileSize_t getSize();
    virtual FileSize_t getPos();
    virtual void setPos(FileSize_t pos);
    virtual void flush();
    virtual size_t write(ConstBytes_t m);
    virtual size_t read(Bytes_t m);
    virtual uint32_t getCapabilities();
    virtual afl::base::Ref<Stream> createChild();
    virtual afl::base::Ptr<FileMapping> createFileMapping(FileSize_t limit = FileSize_t(-1));

 private:
    afl::base::Ref<Stream> m_file;
    LimitedStream m_worker;
    String_t m_name;
};

/************************** TarReader::TarDirEntry *************************/

/** Constructor.
    \param entry Index entry for this directory entry
    \param parent Link to containing tar file. */
afl::io::archive::TarReader::TarDirEntry::TarDirEntry(const IndexEntry& entry, afl::base::Ref<TarReader> parent)
    : UnchangeableDirectoryEntry(afl::string::Messages::cannotModifyArchiveFile()),
      m_entry(entry),
      m_parent(parent)
{ }

afl::io::archive::TarReader::TarDirEntry::~TarDirEntry()
{ }

// DirectoryEntry:
String_t
afl::io::archive::TarReader::TarDirEntry::getTitle()
{
    return m_entry.name;
}

String_t
afl::io::archive::TarReader::TarDirEntry::getPathName()
{
    return String_t();
}

afl::base::Ref<afl::io::Stream>
afl::io::archive::TarReader::TarDirEntry::openFileForReading()
{
    return *new TarStoredMember(m_parent->m_file, m_entry);
}

afl::base::Ref<afl::io::Directory>
afl::io::archive::TarReader::TarDirEntry::openDirectory()
{
    throw afl::except::FileProblemException(m_parent->getTitle(), afl::string::Messages::cannotAccessDirectories());
}

afl::base::Ref<afl::io::Directory>
afl::io::archive::TarReader::TarDirEntry::openContainingDirectory()
{
    return m_parent;
}

void
afl::io::archive::TarReader::TarDirEntry::updateInfo(uint32_t /*requested*/)
{
    setFileType(tFile);
    setFileSize(m_entry.length);
    setModificationTime(m_entry.modificationTime);

    FileFlags_t flags;
    if (m_entry.executable) {
        flags += Executable;
    }
    setFlags(flags);
}

/************************* TarReader::TarDirEnum *************************/

/** Constructor.
    \param parent Link to containing tar file. */
afl::io::archive::TarReader::TarDirEnum::TarDirEnum(afl::base::Ref<TarReader> parent)
    : m_parent(parent),
      m_current(0)
{ }

afl::io::archive::TarReader::TarDirEnum::~TarDirEnum()
{ }

bool
afl::io::archive::TarReader::TarDirEnum::getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result)
{
    // Attempt to read sufficient elements
    while (m_parent->m_index.size() <= m_current) {
        if (!m_parent->readNextEntry()) {
            return false;
        }
    }

    result = new TarDirEntry(*m_parent->m_index[m_current], m_parent);
    ++m_current;
    return true;
}

/************************ TarReader::TarStoredMember ***********************/

/** Constructor.
    \param parent Link to containing tar file.
    \param entry Index entry for this member */
afl::io::archive::TarReader::TarStoredMember::TarStoredMember(afl::base::Ref<Stream> file, const IndexEntry& entry)
    : m_file(file),
      m_worker(file->createChild(), entry.start, entry.length),
      m_name(entry.name)
{ }

afl::io::archive::TarReader::TarStoredMember::~TarStoredMember()
{ }

String_t
afl::io::archive::TarReader::TarStoredMember::getName()
{
    return afl::string::PosixFileNames().makePathName(m_file->getName(), m_name);
}

afl::io::Stream::FileSize_t
afl::io::archive::TarReader::TarStoredMember::getSize()
{
    return m_worker.getSize();
}

afl::io::Stream::FileSize_t
afl::io::archive::TarReader::TarStoredMember::getPos()
{
    return m_worker.getPos();
}

void
afl::io::archive::TarReader::TarStoredMember::setPos(FileSize_t pos)
{
    m_worker.setPos(pos);
}

void
afl::io::archive::TarReader::TarStoredMember::flush()
{
    m_worker.flush();
}

size_t
afl::io::archive::TarReader::TarStoredMember::write(ConstBytes_t /*m*/)
{
    throw afl::except::FileProblemException(*this, afl::string::Messages::cannotWrite());
}

size_t
afl::io::archive::TarReader::TarStoredMember::read(Bytes_t m)
{
    return m_worker.read(m);
}

uint32_t
afl::io::archive::TarReader::TarStoredMember::getCapabilities()
{
    return m_file->getCapabilities() & (CanSeek | CanRead);
}

afl::base::Ref<afl::io::Stream>
afl::io::archive::TarReader::TarStoredMember::createChild()
{
    return m_worker.createChild();
}

afl::base::Ptr<afl::io::FileMapping>
afl::io::archive::TarReader::TarStoredMember::createFileMapping(FileSize_t limit)
{
    return m_worker.createFileMapping(limit);
}

/******************************* TarReader *******************************/


afl::base::Ref<afl::io::archive::TarReader>
afl::io::archive::TarReader::open(afl::base::Ref<Stream> file, int options)
{
    return *new TarReader(file, options);
}

afl::io::archive::TarReader::TarReader(afl::base::Ref<Stream> file, int options)
    : m_file(file),
      m_options(options),
      m_nextEntryPosition(file->getPos()),
      m_atEnd(false),
      m_index()
{ }

afl::io::archive::TarReader::~TarReader()
{ }

// Directory:
afl::base::Ref<afl::io::DirectoryEntry>
afl::io::archive::TarReader::getDirectoryEntryByName(String_t name)
{
    // Look in existing index
    size_t size = m_index.size();
    const IndexEntry* xe = findEntryByName(name, 0, size);

    // Not found, so read more
    while (xe == 0 && readNextEntry()) {
        size_t nsize = m_index.size();
        xe = findEntryByName(name, size, nsize);
        size = nsize;
    }

    // Found?
    if (xe == 0) {
        throw afl::except::FileProblemException(name, afl::string::Messages::fileNotFound());
    }
    return *new TarDirEntry(*xe, *this);
}

afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > >
afl::io::archive::TarReader::getDirectoryEntries()
{
    return *new TarDirEnum(*this);
}

afl::base::Ptr<afl::io::Directory>
afl::io::archive::TarReader::getParentDirectory()
{
    // FIXME: make it possible to pass in a Directory when opening the file?
    return 0;
}

String_t
afl::io::archive::TarReader::getDirectoryName()
{
    // Virtual directory, hence no name.
    return String_t();
}

String_t
afl::io::archive::TarReader::getTitle()
{
    return m_file->getName();
}

bool
afl::io::archive::TarReader::readNextEntry()
{
    if (!m_atEnd) {
        // Skip to next entry, if needed
        Stream::FileSize_t pos = m_file->getPos();
        if (pos != m_nextEntryPosition) {
            // Skip is needed
            // - if file is seekable, just set the position
            // - if the position is beyond the desired position (should not happen!), set the position and hope for the best
            // - otherwise, skip
            if (pos > m_nextEntryPosition || (m_file->getCapabilities() & Stream::CanSeek) != 0) {
                m_file->setPos(m_nextEntryPosition);
            } else {
                skip(*m_file, m_nextEntryPosition - pos);
            }
        }

        // Read a header
        UstarHeader hdr;
        if (m_file->read(afl::base::fromObject(hdr)) == sizeof(hdr)) {
            // Got something
            String_t name = getString(hdr.name);
            if (name.empty()) {
                // End of file reached. tar ends with an all-0 block
                m_atEnd = true;
            } else {
                // Block contains data. Parse the remainder.
                // - prefix
                String_t prefix = getString(hdr.prefix);
                if (!prefix.empty()) {
                    name = prefix + '/' + name;
                }
                normalizeName(name);

                // - attributes
                Stream::FileSize_t size = getNumber(hdr.size, *m_file);
                bool executable = (getNumber(hdr.mode, *m_file) & 0111) != 0;
                afl::sys::Time modificationTime = afl::sys::Time::fromUnixTime(getNumber(hdr.mtime, *m_file));

                // Compute next entry position
                Stream::FileSize_t thisEntryPosition = m_nextEntryPosition + sizeof(hdr);
                m_nextEntryPosition = thisEntryPosition + ((size + 511) & ~511);

                // So, is this a file after all? Then add it.
                switch (hdr.typeflag) {
                 case '0':      // regular file
                 case '\0':     // regular file, backwards compatibility
                 case '7':      // high-performance file
                    if (!findEntryByName(name, 0, m_index.size())) {
                        m_index.pushBackNew(new IndexEntry(name, thisEntryPosition, size, executable, modificationTime));
                    }
                    break;

                 case '1':      // hardlink
                 case '2':      // symlink
                 case '3':      // character device
                 case '4':      // block device
                 case '5':      // directory
                 case '6':      // FIFO
                 default:
                    break;
                }
            }
        } else {
            // End of file reached
            m_atEnd = true;
        }
    }
    return !m_atEnd;
}

const afl::io::archive::TarReader::IndexEntry*
afl::io::archive::TarReader::findEntryByName(const String_t& name, size_t start, size_t end) const
{
    for (size_t i = start; i < end; ++i) {
        if (m_index[i]->name == name) {
            return m_index[i];
        }
    }
    return 0;
}

void
afl::io::archive::TarReader::normalizeName(String_t& name) const
{
    if ((m_options & KeepPaths) == 0) {
        // We do not want to keep paths, thus just drop everything before a slash
        String_t::size_type n = name.rfind('/');
        if (n != name.npos) {
            name.erase(0, n+1);
        }
    } else {
        // We want to keep paths, but still drop some common "null" prefixes
        if (name.size() >= 2 && name[0] == '.' && name[1] == '/') {
            name.erase(0, 2);
        } else if (name.size() >= 1 && name[0] == '/') {
            name.erase(0, 1);
        } else {
            // keep
        }
    }
}
