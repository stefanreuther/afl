/**
  *  \file afl/io/archive/zipreader.cpp
  *  \brief Class afl::io::archive::ZipReader
  */

#include "afl/io/archive/zipreader.hpp"
#include "afl/base/growablememory.hpp"
#include "afl/base/staticassert.hpp"
#include "afl/bits/uint16le.hpp"
#include "afl/bits/uint32le.hpp"
#include "afl/bits/value.hpp"
#include "afl/charset/codepage.hpp"
#include "afl/charset/codepagecharset.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directoryentry.hpp"
#include "afl/io/filemapping.hpp"
#include "afl/io/inflatetransform.hpp"
#include "afl/io/limitedstream.hpp"
#include "afl/io/multiplexablestream.hpp"
#include "afl/io/unchangeabledirectoryentry.hpp"
#include "afl/string/messages.hpp"
#include "afl/string/posixfilenames.hpp"

namespace {
    // General flags
    const uint16_t gfEncrypted       = 1;
    const uint16_t gfMethodExtra1    = 2;
    const uint16_t gfMethodExtra2    = 4;
    const uint16_t gfUnicodeName     = 2048;
    const uint16_t gfUpdated         = 32768;         // not documented by PKWARE, but in use
    const uint16_t gfKnown           = gfEncrypted + gfMethodExtra1 + gfMethodExtra2 + gfUnicodeName + gfUpdated;

    // Compression methods
    const uint16_t cmStored   = 0;
    const uint16_t cmDeflated = 8;

    // Shortcuts
    typedef afl::bits::Value<afl::bits::UInt16LE> UInt16_t;
    typedef afl::bits::Value<afl::bits::UInt32LE> UInt32_t;
    using afl::base::Ptr;
    using afl::base::Ref;
    using afl::except::FileProblemException;
    using afl::string::Messages;
    using afl::string::PosixFileNames;

    // Local header file format
    struct LocalHeader {
        UInt16_t minVersion;
        UInt16_t flags;
        UInt16_t method;
        UInt32_t modificationTime;
        UInt32_t crc;
        UInt32_t compressedSize;
        UInt32_t uncompressedSize;
        UInt16_t nameLength;
        UInt16_t extraLength;
    };
    static_assert(sizeof(LocalHeader) == 26, "sizeof LocalHeader");

    String_t loadString(afl::io::Stream& in, size_t n)
    {
        // FIXME: move this into a library?
        afl::base::GrowableBytes_t buffer;
        buffer.resize(n);
        in.fullRead(buffer);
        return afl::string::fromBytes(buffer);
    }
}

/** Index entry. Stores information gathered from the file member header. */
struct afl::io::archive::ZipReader::IndexEntry {
    String_t name;
    uint16_t method;
    Stream::FileSize_t start;
    Stream::FileSize_t compressedSize;
    Stream::FileSize_t uncompressedSize;

    IndexEntry(const String_t& name, uint16_t method,
               Stream::FileSize_t start,
               Stream::FileSize_t compressedSize,
               Stream::FileSize_t uncompressedSize)
        : name(name),
          method(method),
          start(start),
          compressedSize(compressedSize),
          uncompressedSize(uncompressedSize)
        { }
};

/** Zip "DirectoryEntry" implementation. */
class afl::io::archive::ZipReader::ZipDirEntry : public afl::io::UnchangeableDirectoryEntry {
 public:
    ZipDirEntry(const IndexEntry& entry, const Ref<ZipReader>& parent);
    ~ZipDirEntry();

    // DirectoryEntry:
    virtual String_t getTitle();
    virtual String_t getPathName();
    virtual Ref<Stream> openFileForReading();
    virtual Ref<Directory> openDirectory();
    virtual Ref<Directory> openContainingDirectory();

    virtual void updateInfo(uint32_t requested);

 private:
    /** IndexEntry. Owned by ZipReader. */
    const IndexEntry& m_entry;
    /** Parent. Keeps m_entry alive. */
    const Ref<ZipReader> m_parent;
};

/** Zip DirectoryEntry Enumerator implementation. */
class afl::io::archive::ZipReader::ZipDirEnum : public afl::base::Enumerator<Ptr<afl::io::DirectoryEntry> > {
 public:
    ZipDirEnum(const Ref<ZipReader>& parent);
    ~ZipDirEnum();

    virtual bool getNextElement(Ptr<afl::io::DirectoryEntry>& result);

 private:
    const Ref<ZipReader> m_parent;
    size_t m_current;
};

/** Zip "Stream" implementation for a stored member.
    Essentially, this is a LimitedStream, plus some housekeeping. */
class afl::io::archive::ZipReader::ZipStoredMember : public afl::io::Stream {
 public:
    ZipStoredMember(const Ref<Stream>& file, const IndexEntry& entry);
    ZipStoredMember(ZipStoredMember& orig);
    ~ZipStoredMember();

    virtual String_t getName();
    virtual FileSize_t getSize();
    virtual FileSize_t getPos();
    virtual void setPos(FileSize_t pos);
    virtual void flush();
    virtual size_t write(ConstBytes_t m);
    virtual size_t read(Bytes_t m);
    virtual uint32_t getCapabilities();
    virtual Ref<Stream> createChild(uint32_t flags);
    virtual Ptr<FileMapping> createFileMapping(FileSize_t limit);

 private:
    const Ref<Stream> m_file;
    const Ref<Stream> m_worker;
    String_t m_name;
};

/** Zip "Stream" implementation for a deflated member.
    Uses a LimitedStream for housekeeping, plus a Transform for decoding.
    FIXME: this could be a generic class for reading compressed stuff */
class afl::io::archive::ZipReader::ZipDeflatedMember : public afl::io::MultiplexableStream {
 public:
    ZipDeflatedMember(const Ref<Stream>& file, const IndexEntry& entry);
    ~ZipDeflatedMember();

    virtual String_t getName();
    virtual FileSize_t getSize();
    virtual FileSize_t getPos();
    virtual void setPos(FileSize_t pos);
    virtual void flush();
    virtual size_t write(ConstBytes_t m);
    virtual size_t read(Bytes_t m);
    virtual uint32_t getCapabilities();
    virtual Ptr<FileMapping> createFileMapping(FileSize_t limit);

 private:
    const Ref<Stream> m_file;
    LimitedStream m_worker;
    InflateTransform m_transform;
    const String_t m_name;

    uint8_t m_buffer[4096];
    ConstBytes_t m_bufferDescriptor;
    FileSize_t m_bytesRead;
};

/************************** ZipReader::ZipDirEntry *************************/

/** Constructor.
    \param entry Index entry for this directory entry
    \param parent Link to containing zip file. */
afl::io::archive::ZipReader::ZipDirEntry::ZipDirEntry(const IndexEntry& entry, const Ref<ZipReader>& parent)
    : UnchangeableDirectoryEntry(Messages::cannotModifyArchiveFile()),
      m_entry(entry),
      m_parent(parent)
{ }

afl::io::archive::ZipReader::ZipDirEntry::~ZipDirEntry()
{ }

// DirectoryEntry:
String_t
afl::io::archive::ZipReader::ZipDirEntry::getTitle()
{
    return m_entry.name;
}

String_t
afl::io::archive::ZipReader::ZipDirEntry::getPathName()
{
    return String_t();
}

Ref<afl::io::Stream>
afl::io::archive::ZipReader::ZipDirEntry::openFileForReading()
{
    if (m_entry.method == cmStored) {
        return *new ZipStoredMember(m_parent->m_file, m_entry);
    } else if (m_entry.method == cmDeflated) {
        return *new ZipDeflatedMember(m_parent->m_file, m_entry);
    } else {
        throw FileProblemException(getTitle(), Messages::unsupportedCompressionMode());
    }
}

Ref<afl::io::Directory>
afl::io::archive::ZipReader::ZipDirEntry::openDirectory()
{
    throw FileProblemException(m_parent->getTitle(), Messages::cannotAccessDirectories());
}

Ref<afl::io::Directory>
afl::io::archive::ZipReader::ZipDirEntry::openContainingDirectory()
{
    return m_parent;
}

void
afl::io::archive::ZipReader::ZipDirEntry::updateInfo(uint32_t /*requested*/)
{
    setFileType(tFile);
    setFileSize(m_entry.uncompressedSize);
    // FIXME: setModificationTime, setFlags
}

/************************** ZipReader::ZipDirEnum **************************/

/** Constructor.
    \param parent Link to containing zip file. */
afl::io::archive::ZipReader::ZipDirEnum::ZipDirEnum(const Ref<ZipReader>& parent)
    : m_parent(parent),
      m_current(0)
{ }

afl::io::archive::ZipReader::ZipDirEnum::~ZipDirEnum()
{ }

bool
afl::io::archive::ZipReader::ZipDirEnum::getNextElement(Ptr<afl::io::DirectoryEntry>& result)
{
    // Attempt to read sufficient elements
    while (m_parent->m_index.size() <= m_current) {
        if (!m_parent->readNextEntry()) {
            return false;
        }
    }

    result = new ZipDirEntry(*m_parent->m_index[m_current], m_parent);
    ++m_current;
    return true;
}

/************************ ZipReader::ZipStoredMember ***********************/

/** Constructor.
    \param file Underlying file
    \param entry Index entry for this member */
afl::io::archive::ZipReader::ZipStoredMember::ZipStoredMember(const Ref<Stream>& file, const IndexEntry& entry)
    : m_file(file),
      m_worker(*new LimitedStream(file->createChild(0), entry.start, entry.compressedSize)),
      m_name(entry.name)
{ }

afl::io::archive::ZipReader::ZipStoredMember::ZipStoredMember(ZipStoredMember& orig)
    : m_file(orig.m_file),
      m_worker(orig.m_worker->createChild(0)),
      m_name(orig.m_name)
{ }

afl::io::archive::ZipReader::ZipStoredMember::~ZipStoredMember()
{ }

String_t
afl::io::archive::ZipReader::ZipStoredMember::getName()
{
    return PosixFileNames().makePathName(m_file->getName(), m_name);
}

afl::io::Stream::FileSize_t
afl::io::archive::ZipReader::ZipStoredMember::getSize()
{
    return m_worker->getSize();
}

afl::io::Stream::FileSize_t
afl::io::archive::ZipReader::ZipStoredMember::getPos()
{
    return m_worker->getPos();
}

void
afl::io::archive::ZipReader::ZipStoredMember::setPos(FileSize_t pos)
{
    m_worker->setPos(pos);
}

void
afl::io::archive::ZipReader::ZipStoredMember::flush()
{
    m_worker->flush();
}

size_t
afl::io::archive::ZipReader::ZipStoredMember::write(ConstBytes_t /*m*/)
{
    throw FileProblemException(*this, Messages::cannotWrite());
}

size_t
afl::io::archive::ZipReader::ZipStoredMember::read(Bytes_t m)
{
    return m_worker->read(m);
}

uint32_t
afl::io::archive::ZipReader::ZipStoredMember::getCapabilities()
{
    return CanRead | CanSeek;
}

Ref<afl::io::Stream>
afl::io::archive::ZipReader::ZipStoredMember::createChild(uint32_t /*flags*/)
{
    return *new ZipStoredMember(*this);
}

Ptr<afl::io::FileMapping>
afl::io::archive::ZipReader::ZipStoredMember::createFileMapping(FileSize_t limit)
{
    return m_worker->createFileMapping(limit);
}

/*********************** ZipReader::ZipDeflatedMember **********************/

afl::io::archive::ZipReader::ZipDeflatedMember::ZipDeflatedMember(const Ref<Stream>& file, const IndexEntry& entry)
    : m_file(file),
      m_worker(file->createChild(0), entry.start, entry.compressedSize),
      m_transform(InflateTransform::Raw),
      m_name(entry.name),
      m_buffer(),
      m_bufferDescriptor(),
      m_bytesRead(0)
{ }

afl::io::archive::ZipReader::ZipDeflatedMember::~ZipDeflatedMember()
{ }

String_t
afl::io::archive::ZipReader::ZipDeflatedMember::getName()
{
    return PosixFileNames().makePathName(m_file->getName(), m_name);
}

afl::io::Stream::FileSize_t
afl::io::archive::ZipReader::ZipDeflatedMember::getSize()
{
    return FileSize_t(-1);
}

afl::io::Stream::FileSize_t
afl::io::archive::ZipReader::ZipDeflatedMember::getPos()
{
    return m_bytesRead;
}

void
afl::io::archive::ZipReader::ZipDeflatedMember::setPos(FileSize_t pos)
{
    if (pos != m_bytesRead) {
        throw FileProblemException(*this, Messages::invalidOperation());
    }
}

void
afl::io::archive::ZipReader::ZipDeflatedMember::flush()
{ }

size_t
afl::io::archive::ZipReader::ZipDeflatedMember::write(ConstBytes_t /*m*/)
{
    throw FileProblemException(*this, Messages::cannotWrite());
}

size_t
afl::io::archive::ZipReader::ZipDeflatedMember::read(Bytes_t m)
{
    size_t did = 0;
    while (!m.empty()) {
        // Try to re-fill buffer if needed
        if (m_bufferDescriptor.empty()) {
            Bytes_t b(m_buffer);
            b.trim(m_worker.read(b));
            m_bufferDescriptor = b;
        }

        // Buffer still empty? End of file.
        if (m_bufferDescriptor.empty()) {
            break;
        }

        // Transform
        Bytes_t result = m;
        m_transform.transform(m_bufferDescriptor, result);
        m.split(result.size());
        did += result.size();
        m_bytesRead += result.size();
    }
    return did;
}

uint32_t
afl::io::archive::ZipReader::ZipDeflatedMember::getCapabilities()
{
    return CanRead;
}

Ptr<afl::io::FileMapping>
afl::io::archive::ZipReader::ZipDeflatedMember::createFileMapping(FileSize_t /*limit*/)
{
    return 0;
}

/******************************** ZipReader ********************************/

afl::base::Ref<afl::io::archive::ZipReader>
afl::io::archive::ZipReader::open(afl::base::Ref<Stream> file, int options)
{
    return *new ZipReader(file, options);
}

afl::io::archive::ZipReader::~ZipReader()
{ }

afl::base::Ref<afl::io::DirectoryEntry>
afl::io::archive::ZipReader::getDirectoryEntryByName(String_t name)
{
    // Look in existing index
    size_t size = m_index.size();
    IndexEntry* xe = findEntryByName(name, 0, size);

    // Not found, so read more
    while (xe == 0 && readNextEntry()) {
        size_t nsize = m_index.size();
        xe = findEntryByName(name, size, nsize);
        size = nsize;
    }

    // Found?
    if (xe == 0) {
        throw FileProblemException(name, Messages::fileNotFound());
    }
    return *new ZipDirEntry(*xe, *this);
}

afl::base::Ref<afl::base::Enumerator<Ptr<afl::io::DirectoryEntry> > >
afl::io::archive::ZipReader::getDirectoryEntries()
{
    return *new ZipDirEnum(*this);
}

afl::base::Ptr<afl::io::Directory>
afl::io::archive::ZipReader::getParentDirectory()
{
    // FIXME: make it possible to pass in a Directory when opening the file?
    return 0;
}

String_t
afl::io::archive::ZipReader::getDirectoryName()
{
    // Virtual directory, hence no name.
    return String_t();
}

String_t
afl::io::archive::ZipReader::getTitle()
{
    return m_file->getName();
}

void
afl::io::archive::ZipReader::flush()
{ }

/** Private constructor.
    Use create(Ptr<Stream>) to create instances of this class.
    \param file Underlying file
    \param options Options */
afl::io::archive::ZipReader::ZipReader(afl::base::Ref<Stream> file, int options)
    : m_file(file),
      m_view(file->createChild(0)),
      m_options(options),
      m_index(),
      m_indexerReachedEnd(false),
      m_hadUnsupportedFeature(false)
{
    // Read first entry to fail if we were not given a zip file
    readNextEntry();
}

/** Read next entry from Zip file.
    \retval true I have read something, successfully. It may or may not have been added to the index.
    \retval false I have stopped parsing, at end of file or at an item I don't understand */
bool
afl::io::archive::ZipReader::readNextEntry()
{
    /* Read block type (unless end already reached) */
    UInt32_t blockType;
    if (m_indexerReachedEnd || m_hadUnsupportedFeature || m_view->read(afl::base::fromObject(blockType)) != sizeof(blockType)) {
        m_indexerReachedEnd = true;
        return false;
    }
    const uint32_t block = blockType;

    if (block == 0x04034B50) {
        /* Local Header */
        LocalHeader header;
        m_view->fullRead(afl::base::fromObject(header));
        // uint16_t min_version = getUint16(buffer+0);
        uint16_t flags       = header.flags;
        uint16_t method      = header.method;
        // uint32_t mtime       = getUint32(buffer+6);
        // uint32_t crc         = getUint32(buffer+10);
        uint32_t compsize    = header.compressedSize;
        uint32_t uncompsize  = header.uncompressedSize;
        uint16_t namelength  = header.nameLength;
        uint16_t extraLength = header.extraLength;

        /* If this header is not supported, stop parsing */
        if ((flags & ~gfKnown) != 0) {
            m_hadUnsupportedFeature = true;
            return false;
        }

        /* Read file name */
        String_t name = loadString(*m_view, namelength);
        String_t::size_type n = name.rfind('/');
        if (n != name.npos && ((m_options & KeepPaths) == 0 || n+1 == name.size())) {
            /* Clear name if this is a directory, remove path part if configured so. */
            name.erase(0, n+1);
        }
        if ((flags & gfUnicodeName) == 0) {
            name = afl::charset::CodepageCharset(afl::charset::g_codepage437).decode(afl::string::toMemory(name).toBytes());
        }

        /* Skip extra field */
        if (extraLength != 0) {
            m_view->setPos(m_view->getPos() + extraLength);
        }

        /* If it is supported, build index entry */
        if (name.size() == 0) {
            /* Directory */
        } else if ((flags & gfEncrypted) == 0 && (method == cmStored || method == cmDeflated)) {
            /* Supported file */
            if (!findEntryByName(name, 0, m_index.size())) {
                m_index.pushBackNew(new IndexEntry(name, method, m_view->getPos(), compsize, uncompsize));
            }
        } else {
            /* Unsupported */
        }
        m_view->setPos(m_view->getPos() + compsize);
        return true;
    } else if (block == 0x02014B50 || block == 0x06054B50) {
        /* Central Directory, or End of Central Directory */
        m_indexerReachedEnd = true;
        return false;
    } else {
        m_hadUnsupportedFeature = true;
        return false;
    }
}

afl::io::archive::ZipReader::IndexEntry*
afl::io::archive::ZipReader::findEntryByName(const String_t& name, size_t start, size_t end)
{
    for (size_t i = start; i < end; ++i) {
        if (m_index[i]->name == name) {
            return m_index[i];
        }
    }
    return 0;
}
