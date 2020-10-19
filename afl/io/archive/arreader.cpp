/**
  *  \file afl/io/archive/arreader.cpp
  *  \brief Class afl::io::archive::ArReader
  */

#include "afl/io/archive/arreader.hpp"
#include "afl/base/memory.hpp"
#include "afl/base/staticassert.hpp"
#include "afl/except/fileformatexception.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/except/filetooshortexception.hpp"
#include "afl/io/filemapping.hpp"
#include "afl/io/limitedstream.hpp"
#include "afl/io/unchangeabledirectoryentry.hpp"
#include "afl/string/messages.hpp"
#include "afl/string/posixfilenames.hpp"

using afl::io::Stream;

namespace {
    /* Maximum long-name table size.
       This is a simple anti-DoS measure. */
    const uint32_t MAX_LONG_NAME_TABLE_SIZE = 32UL*1024*1024;

    /* File magic, 8 bytes */
    const uint8_t FILE_MAGIC[] = {'!','<','a','r','c','h','>',10};
    static_assert(sizeof(FILE_MAGIC) == 8, "sizeof FILE_MAGIC");

    /* Member magic, 2 bytes */
    const uint8_t MEMBER_MAGIC[] = {96,10};
    static_assert(sizeof(MEMBER_MAGIC) == 2, sizeof(MEMBER_MAGIC));

    const size_t NAME_LEN = 16;
    const uint8_t NAME_SEP = 10;

    /* Contained-file header, 60 bytes */
    struct ArMemberHeader {
        /* File name, space-padded.
           "foo/"         (SysV, embedded spaces allowed)
           "foo"          (BSD)
           "/12"          (SysV, long file name, number is decimal offset into long-name-table
           " 12 rubbish   (long name, alternative version)
           "#1/18"        (alternative form of long file names, unsupported)
           "/"            (COFF armap)
                             u32be     #entries
                             u32be[]   offsets
                             char[]    null-terminated names
           "__.SYMDEF", "__.SYMDEF/" (BSD armap)
           "//", "ARFILENAMES/" (long-name-table; sequence of strings separated by \012 */
        char fileName[NAME_LEN];

        /* Modification time (decimal, space padded) */
        uint8_t modificationTime[12];

        /* uid, gid (decimal, space padded) */
        uint8_t uid[6];
        uint8_t gid[6];

        /* file mode (octal, space padded) */
        uint8_t mode[8];

        /* file size (decimal, space padded) */
        uint8_t size[10];

        /* magic */
        uint8_t magic[sizeof(MEMBER_MAGIC)];
    };
    static_assert(sizeof(ArMemberHeader) == 60, "sizeof ArMemberHeader");

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

    uint32_t parseNumber(afl::base::ConstBytes_t in, uint32_t base)
    {
        uint32_t result = 0;
        while (const uint8_t* p = in.eat()) {
            if (*p >= '0' && *p < '0'+base) {
                result = base*result + (*p-'0');
            } else {
                break;
            }
        }
        return result;
    }

    bool checkSpecialName(afl::base::Memory<const char> in, const char* name)
    {
        while (char ch = *name++) {
            const char* p = in.eat();
            if (p == 0 || *p != ch) {
                return false;
            }
        }
        while (const char* p = in.eat()) {
            if (*p != ' ') {
                return false;
            }
        }
        return true;
    }

    String_t getFileName(Stream& file, char (&name)[NAME_LEN], afl::base::ConstBytes_t longNameTable)
    {
        // Check for long file name
        if(name[0] == ' ' || name[0] == '/') {
            // Parse index; trim longNameTable to start with index
            size_t i = 1;
            size_t index = 0;
            while (i < sizeof(name) && name[i] >= '0' && name[i] <= '9') {
                index = 10*index + name[i++] - '0';
            }
            longNameTable.split(index);

            // Determine length; trim longNameTable to contain just name
            size_t nameLen = longNameTable.find(NAME_SEP);
            if (nameLen == 0 || nameLen == longNameTable.size()) {
                throw afl::except::FileFormatException(file, "Invalid archive member header (lfn table index out of range)");
            }

            // Remove possible trailing slash
            longNameTable.trim(nameLen);
            if (const uint8_t* p = longNameTable.atEnd(0)) {
                if (*p == '/') {
                    longNameTable.removeEnd(1);
                }
            }

            return afl::string::fromBytes(longNameTable);
        }

        // Pass 1: scan backward for a slash. SysV stores "filename.o" as "filename.o/" to allow for embedded spaces
        size_t i = sizeof(name);
        while (i != 0 && name[i-1] == ' ') {
            --i;
        }
        if (i != 0 && name[i-1] == '/') {
            return String_t(name, i-1);
        }

        /* pass 2: scan forward for a space. BSD stores file names this way */
        i = 0;
        while (i < sizeof(name) && name[i] != ' ') {
            ++i;
        }
        return String_t(name, i);
    }
}


/** Index entry. */
struct afl::io::archive::ArReader::IndexEntry {
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


/** "DirectoryEntry" implementation. */
class afl::io::archive::ArReader::ArDirEntry : public afl::io::UnchangeableDirectoryEntry {
 public:
    ArDirEntry(const IndexEntry& entry, afl::base::Ref<ArReader> parent);
    ~ArDirEntry();

    // DirectoryEntry:
    virtual String_t getTitle();
    virtual String_t getPathName();
    virtual afl::base::Ref<Stream> openFileForReading();
    virtual afl::base::Ref<Directory> openDirectory();
    virtual afl::base::Ref<Directory> openContainingDirectory();

    virtual void updateInfo(uint32_t requested);

 private:
    const IndexEntry& m_entry;
    afl::base::Ref<ArReader> m_parent;
};

/** Tar DirectoryEntry Enumerator implementation. */
class afl::io::archive::ArReader::ArDirEnum : public afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > {
 public:
    ArDirEnum(afl::base::Ref<ArReader> parent);
    ~ArDirEnum();

    virtual bool getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result);

 private:
    afl::base::Ref<ArReader> m_parent;
    size_t m_current;
};

/** Tar "Stream" implementation for a member.
    Essentially, this is a LimitedStream, plus some housekeeping. */
class afl::io::archive::ArReader::ArMember : public afl::io::Stream {
 public:
    ArMember(afl::base::Ref<Stream> file, const IndexEntry& entry);
    ~ArMember();

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

/************************** ArReader::ArDirEntry *************************/

/** Constructor.
    \param entry Index entry for this directory entry
    \param parent Link to containing tar file. */
afl::io::archive::ArReader::ArDirEntry::ArDirEntry(const IndexEntry& entry, afl::base::Ref<ArReader> parent)
    : UnchangeableDirectoryEntry(afl::string::Messages::cannotModifyArchiveFile()),
      m_entry(entry),
      m_parent(parent)
{ }

afl::io::archive::ArReader::ArDirEntry::~ArDirEntry()
{ }

// DirectoryEntry:
String_t
afl::io::archive::ArReader::ArDirEntry::getTitle()
{
    return m_entry.name;
}

String_t
afl::io::archive::ArReader::ArDirEntry::getPathName()
{
    return String_t();
}

afl::base::Ref<afl::io::Stream>
afl::io::archive::ArReader::ArDirEntry::openFileForReading()
{
    return *new ArMember(m_parent->m_file, m_entry);
}

afl::base::Ref<afl::io::Directory>
afl::io::archive::ArReader::ArDirEntry::openDirectory()
{
    throw afl::except::FileProblemException(m_parent->getTitle(), afl::string::Messages::cannotAccessDirectories());
}

afl::base::Ref<afl::io::Directory>
afl::io::archive::ArReader::ArDirEntry::openContainingDirectory()
{
    return m_parent;
}

void
afl::io::archive::ArReader::ArDirEntry::updateInfo(uint32_t /*requested*/)
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

/************************* ArReader::ArDirEnum *************************/

/** Constructor.
    \param parent Link to containing tar file. */
afl::io::archive::ArReader::ArDirEnum::ArDirEnum(afl::base::Ref<ArReader> parent)
    : m_parent(parent),
      m_current(0)
{ }

afl::io::archive::ArReader::ArDirEnum::~ArDirEnum()
{ }

bool
afl::io::archive::ArReader::ArDirEnum::getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result)
{
    // Attempt to read sufficient elements
    while (m_parent->m_index.size() <= m_current) {
        if (!m_parent->readNextEntry()) {
            return false;
        }
    }

    result = new ArDirEntry(*m_parent->m_index[m_current], m_parent);
    ++m_current;
    return true;
}

// /************************ ArReader::ArMember ***********************/

/** Constructor.
    \param parent Link to containing tar file.
    \param entry Index entry for this member */
afl::io::archive::ArReader::ArMember::ArMember(afl::base::Ref<Stream> file, const IndexEntry& entry)
    : m_file(file),
      m_worker(file->createChild(), entry.start, entry.length),
      m_name(entry.name)
{ }

afl::io::archive::ArReader::ArMember::~ArMember()
{ }

String_t
afl::io::archive::ArReader::ArMember::getName()
{
    return afl::string::PosixFileNames().makePathName(m_file->getName(), m_name);
}

afl::io::Stream::FileSize_t
afl::io::archive::ArReader::ArMember::getSize()
{
    return m_worker.getSize();
}

afl::io::Stream::FileSize_t
afl::io::archive::ArReader::ArMember::getPos()
{
    return m_worker.getPos();
}

void
afl::io::archive::ArReader::ArMember::setPos(FileSize_t pos)
{
    m_worker.setPos(pos);
}

void
afl::io::archive::ArReader::ArMember::flush()
{
    m_worker.flush();
}

size_t
afl::io::archive::ArReader::ArMember::write(ConstBytes_t /*m*/)
{
    throw afl::except::FileProblemException(*this, afl::string::Messages::cannotWrite());
}

size_t
afl::io::archive::ArReader::ArMember::read(Bytes_t m)
{
    return m_worker.read(m);
}

uint32_t
afl::io::archive::ArReader::ArMember::getCapabilities()
{
    return m_file->getCapabilities() & (CanSeek | CanRead);
}

afl::base::Ref<afl::io::Stream>
afl::io::archive::ArReader::ArMember::createChild()
{
    return m_worker.createChild();
}

afl::base::Ptr<afl::io::FileMapping>
afl::io::archive::ArReader::ArMember::createFileMapping(FileSize_t limit)
{
    return m_worker.createFileMapping(limit);
}

/******************************** ArReader *******************************/

afl::base::Ref<afl::io::archive::ArReader>
afl::io::archive::ArReader::open(afl::base::Ref<Stream> file)
{
    return *new ArReader(file);
}

afl::io::archive::ArReader::ArReader(afl::base::Ref<Stream> file)
    : m_file(file),
      m_nextEntryPosition(file->getPos()),
      m_atEnd(false),
      m_index(),
      m_longNameTable()
{
    readHeader();
}

afl::io::archive::ArReader::~ArReader()
{ }

afl::base::Ref<afl::io::DirectoryEntry>
afl::io::archive::ArReader::getDirectoryEntryByName(String_t name)
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
    return *new ArDirEntry(*xe, *this);
}


afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > >
afl::io::archive::ArReader::getDirectoryEntries()
{
    return *new ArDirEnum(*this);
}

afl::base::Ptr<afl::io::Directory>
afl::io::archive::ArReader::getParentDirectory()
{
    return 0;
}

String_t
afl::io::archive::ArReader::getDirectoryName()
{
    // Virtual directory, hence no name.
    return String_t();
}

String_t
afl::io::archive::ArReader::getTitle()
{
    return m_file->getName();
}

void
afl::io::archive::ArReader::readHeader()
{
    uint8_t tmp[sizeof(FILE_MAGIC)];
    m_file->fullRead(tmp);
    if (!afl::base::Bytes_t(tmp).equalContent(FILE_MAGIC)) {
        throw afl::except::FileFormatException(*m_file, afl::string::Messages::invalidFileHeader());
    }

    m_nextEntryPosition = m_file->getPos();
}

bool
afl::io::archive::ArReader::readNextEntry()
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

        // Read the header
        ArMemberHeader mem;
        if (m_file->read(afl::base::fromObject(mem)) == sizeof(mem)) {
            // OK, parse header
            uint32_t size = parseNumber(mem.size, 10);
            uint32_t mtime = parseNumber(mem.modificationTime, 10);
            uint32_t mode = parseNumber(mem.mode, 8);

            // Remember file positions
            Stream::FileSize_t thisEntryPos = m_file->getPos();
            m_nextEntryPosition = thisEntryPos + size + (size&1);

            // Check entry type
            if (checkSpecialName(mem.fileName, "/")) {
                // COFF armap; skip
            } else if (checkSpecialName(mem.fileName, "__.SYMDEF") || checkSpecialName(mem.fileName, "__.SYMDEF/")) {
                // BSD armap; skip
            } else if (checkSpecialName(mem.fileName, "//") || checkSpecialName(mem.fileName, "ARFILENAMES/")) {
                // long-name-table
                if (size > MAX_LONG_NAME_TABLE_SIZE) {
                    throw afl::except::FileFormatException(*m_file, afl::string::Messages::unsupportedFeature());
                }
                if (m_longNameTable.empty()) {
                    m_longNameTable.resize(size);
                    m_file->fullRead(m_longNameTable);
                }
            } else {
                // Regular member
                String_t name = getFileName(*m_file, mem.fileName, m_longNameTable);
                if (!name.empty()) {
                    m_index.pushBackNew(new IndexEntry(name, thisEntryPos, size, (mode & 0111) != 0, afl::sys::Time::fromUnixTime(mtime)));
                }
            }
        } else {
            // nok
            m_atEnd = true;
        }
    }
    return !m_atEnd;
}

const afl::io::archive::ArReader::IndexEntry*
afl::io::archive::ArReader::findEntryByName(const String_t& name, size_t start, size_t end) const
{
    for (size_t i = start; i < end; ++i) {
        if (m_index[i]->name == name) {
            return m_index[i];
        }
    }
    return 0;
}
