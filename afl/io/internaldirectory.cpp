/**
  *  \file afl/io/internaldirectory.cpp
  *  \brief Class afl::io::InternalDirectory
  */

#include "afl/io/internaldirectory.hpp"
#include "afl/io/directoryentry.hpp"
#include "afl/io/internalstream.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/string/messages.hpp"

class afl::io::InternalDirectory::Entry : public afl::io::DirectoryEntry {
 public:
    Entry(afl::base::Ref<InternalDirectory> parent, const String_t& name);
    virtual String_t getTitle();
    virtual String_t getPathName();
    virtual afl::base::Ref<afl::io::Stream> openFile(afl::io::FileSystem::OpenMode mode);
    virtual afl::base::Ref<afl::io::Directory> openDirectory();
    virtual afl::base::Ref<afl::io::Directory> openContainingDirectory();
    virtual void updateInfo(uint32_t requested);
    virtual void doRename(String_t newName);
    virtual void doErase();
    virtual void doCreateAsDirectory();

 private:
    afl::base::Ref<InternalDirectory> m_parent;
    String_t m_name;
};


class afl::io::InternalDirectory::Enum : public afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > {
 public:
    Enum(afl::base::Ref<InternalDirectory> dir);
    virtual ~Enum();
    virtual bool getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result);

 private:
    afl::base::Ref<InternalDirectory> m_parent;
    afl::base::Ptr<DirectoryEntry> m_currentEntry;
    InternalDirectory::NodeList_t::iterator m_iter;

    void advance();
    void next();
};

/******************* afl::io::InternalDirectory::Entry *******************/

inline
afl::io::InternalDirectory::Entry::Entry(afl::base::Ref<InternalDirectory> parent, const String_t& name)
    : m_parent(parent),
      m_name(name)
{ }

String_t
afl::io::InternalDirectory::Entry::getTitle()
{
    return m_name;
}
String_t
afl::io::InternalDirectory::Entry::getPathName()
{
    return String_t();
}

afl::base::Ref<afl::io::Stream>
afl::io::InternalDirectory::Entry::openFile(afl::io::FileSystem::OpenMode mode)
{
    InternalDirectory::NodeList_t::iterator it = m_parent->findName(m_name);
    InternalDirectory::NodeList_t::iterator end = m_parent->m_content.end();

    afl::base::Ptr<Stream> result;
    switch (mode) {
     case FileSystem::OpenRead:
     case FileSystem::OpenWrite:
        if (it == end) {
            throw afl::except::FileProblemException(m_name, afl::string::Messages::fileNotFound());
        }
        result = (*it)->m_stream.asPtr();
        break;

     case FileSystem::Create:
        result = new afl::io::InternalStream();
        if (it != end) {
            (*it)->m_stream.reset(*result);
        } else {
            m_parent->m_content.pushBackNew(new Node(m_name, *result));
        }
        break;

     case FileSystem::CreateNew:
        if (it != end) {
            throw afl::except::FileProblemException(m_name, afl::string::Messages::fileExists());
        }
        result = new afl::io::InternalStream();
        m_parent->m_content.pushBackNew(new Node(m_name, *result));
        break;
    }

    // Create a child for independence
    if (result.get() != 0) {
        return result->createChild();
    } else {
        throw afl::except::FileProblemException(m_name, afl::string::Messages::fileNotFound());
    }
}

afl::base::Ref<afl::io::Directory>
afl::io::InternalDirectory::Entry::openDirectory()
{
    throw afl::except::FileProblemException(m_name, afl::string::Messages::cannotAccessDirectories());
}

afl::base::Ref<afl::io::Directory>
afl::io::InternalDirectory::Entry::openContainingDirectory()
{
    return m_parent;
}

void
afl::io::InternalDirectory::Entry::updateInfo(uint32_t /*requested*/)
{
    InternalDirectory::NodeList_t::iterator it = m_parent->findName(m_name);
    if (it != m_parent->m_content.end()) {
        setFileType(tFile);

        // Must create a child for proper interlocking
        setFileSize((*it)->m_stream->createChild()->getSize());
    }
}

void
afl::io::InternalDirectory::Entry::doRename(String_t newName)
{
    if (!m_parent->renameStream(m_name, newName)) {
        throw afl::except::FileProblemException(m_name, afl::string::Messages::fileNotFound());
    }
}

void
afl::io::InternalDirectory::Entry::doErase()
{
    if (!m_parent->eraseStream(m_name)) {
        throw afl::except::FileProblemException(m_name, afl::string::Messages::fileNotFound());
    }
}

void
afl::io::InternalDirectory::Entry::doCreateAsDirectory()
{
    throw afl::except::FileProblemException(m_name, afl::string::Messages::cannotAccessDirectories());
}


/******************** afl::io::InternalDirectory::Enum *******************/

afl::io::InternalDirectory::Enum::Enum(afl::base::Ref<InternalDirectory> dir)
    : m_parent(dir),
      m_currentEntry(),
      m_iter(dir->m_content.begin())
{
    next();
}

afl::io::InternalDirectory::Enum::~Enum()
{ }

bool
afl::io::InternalDirectory::Enum::getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result)
{
    result = m_currentEntry;
    advance();
    next();
    return result.get() != 0;
}

inline void
afl::io::InternalDirectory::Enum::advance()
{
    if (m_iter != m_parent->m_content.end()) {
        ++m_iter;
    }
}

void
afl::io::InternalDirectory::Enum::next()
{
    NodeList_t::iterator end = m_parent->m_content.end();
    while (m_iter != end && (*m_iter) == 0) {
        ++m_iter;
    }
    if (m_iter != end) {
        m_currentEntry = new Entry(m_parent, (*m_iter)->m_name);
    } else {
        m_currentEntry = 0;
    }
}


/******************** afl::io::InternalDirectory::Node *******************/

inline
afl::io::InternalDirectory::Node::Node(String_t name, afl::base::Ref<Stream> stream)
    : m_name(name),
      m_stream(stream)
{ }


/*********************** afl::io::InternalDirectory **********************/

inline
afl::io::InternalDirectory::InternalDirectory(String_t name)
    : Directory(),
      m_name(name),
      m_content()
{ }

afl::io::InternalDirectory::~InternalDirectory()
{ }

afl::base::Ref<afl::io::InternalDirectory>
afl::io::InternalDirectory::create(String_t name)
{
    return *new InternalDirectory(name);
}

afl::base::Ref<afl::io::DirectoryEntry>
afl::io::InternalDirectory::getDirectoryEntryByName(String_t name)
{
    return *new Entry(*this, name);
}

afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > >
afl::io::InternalDirectory::getDirectoryEntries()
{
    return *new Enum(*this);
}

afl::base::Ptr<afl::io::Directory>
afl::io::InternalDirectory::getParentDirectory()
{
    return 0;
}

String_t
afl::io::InternalDirectory::getDirectoryName()
{
    return String_t();
}

String_t
afl::io::InternalDirectory::getTitle()
{
    return m_name;
}

void
afl::io::InternalDirectory::addStream(String_t name, afl::base::Ref<Stream> stream)
{
    NodeList_t::iterator p = findName(name);
    if (p != m_content.end()) {
        (*p)->m_stream.reset(*stream);
    } else {
        m_content.pushBackNew(new Node(name, stream));
    }
}

afl::base::Ptr<afl::io::Stream>
afl::io::InternalDirectory::getStream(String_t name)
{
    NodeList_t::iterator p = findName(name);
    if (p != m_content.end()) {
        return (*p)->m_stream.asPtr();
    } else {
        return 0;
    }
}

bool
afl::io::InternalDirectory::eraseStream(String_t name)
{
    NodeList_t::iterator p = findName(name);
    if (p != m_content.end()) {
        // Found it. Remove it.
        m_content.erase(p);
        return true;
    } else {
        // Not found.
        return false;
    }
}

bool
afl::io::InternalDirectory::renameStream(String_t oldName, String_t newName)
{
    NodeList_t::iterator oldP = findName(oldName);
    NodeList_t::iterator newP = findName(newName);
    NodeList_t::iterator end = m_content.end();
    if (oldP == newP) {
        // Identical. Either both not found (error), or same name.
        return oldP != end;
    } else if (oldP == end) {
        // Source not found.
        return false;
    } else {
        // Source exists.
        if (newP != end) {
            // Renaming to a nonexistant name: delete that.
            m_content.erase(newP);
        }

        // FIXME: we should somehow rename the stream.
        (*oldP)->m_name = newName;
        return true;
    }
}

afl::io::InternalDirectory::NodeList_t::iterator
afl::io::InternalDirectory::findName(const String_t& name) const
{
    NodeList_t::iterator p = m_content.begin(), e = m_content.end();
    while (p != e && (*p)->m_name != name) {
        ++p;
    }
    return p;
}
