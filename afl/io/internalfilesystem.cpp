/**
  *  \file afl/io/internalfilesystem.cpp
  *  \brief Class afl::io::InternalFileSystem
  */

#include "afl/io/internalfilesystem.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directoryentry.hpp"
#include "afl/io/internalstream.hpp"
#include "afl/string/messages.hpp"
#include "afl/string/posixfilenames.hpp"

using afl::base::Ptr;
using afl::base::Ref;
using afl::except::FileProblemException;
using afl::string::ConstStringMemory_t;
using afl::string::Messages;
using afl::string::PosixFileNames;

/*
 *  Reference structure:
 *
 *     InternalFileSystem -> NodeList -> DirectoryNode -> NodeList -> FileNode -> InternalStream
 *                               ^                            ^
 *                               |                            |
 *                              Dir <------(m_parent)------- Dir
 *                                                            ^
 *                                                            |
 *                                                        Entry, Enum
 *
 *  A `Dir` object will keep the entire tree alive because it allows navigating the entire tree,
 *  upwards (left) using getParentDirectory(), and downwards (right) using getDirectoryEntries(), openDirectory().
 *
 *  However, there are no cyclic references.
 */

namespace {
    /* Prepare file name: convert name into ConstStringMemory_t, consuming possible leading slash. */
    ConstStringMemory_t prepareFileName(const afl::io::FileSystem::FileName_t& name)
    {
        ConstStringMemory_t result(afl::string::toMemory(name));
        if (const char* ch = result.at(0)) {
            if (*ch == '/') {
                result.eat();
            }
        }
        return result;
    }

    /* Verify file name component. Checks that names does not contain "/", which would make the files inaccessible.
       Likewise, name must not be "." which is special. */
    void verifyFileNameComponent(const String_t& str)
    {
        if (str.empty() || str.find('/') != String_t::npos || str == ".") {
            throw FileProblemException(str, Messages::invalidFileName());
        }
    }

    /* Check for "." directory */
    bool isDot(ConstStringMemory_t name)
    {
        static const char DOT[] = {'.'};
        return name.equalContent(DOT);
    }
}

/* FileNode: stores an InternalStream */
struct afl::io::InternalFileSystem::FileNode : public Node {
    Ref<InternalStream> content;
    FileNode(const String_t& name)
        : Node(name),
          content(*new InternalStream())
        { }
};

/* DirectoryNode: stores a NodeList */
struct afl::io::InternalFileSystem::DirectoryNode : public Node {
    Ref<NodeList> content;
    DirectoryNode(const String_t& name)
        : Node(name), content(*new NodeList())
        { }
};

/* Implementation of Directory */
class afl::io::InternalFileSystem::Dir : public Directory {
 public:
    Dir(String_t name, Ref<NodeList> content, Ptr<Directory> parent);
    ~Dir();

    virtual Ref<DirectoryEntry> getDirectoryEntryByName(String_t name);
    virtual Ref<afl::base::Enumerator<Ptr<DirectoryEntry> > > getDirectoryEntries();
    virtual Ptr<Directory> getParentDirectory();
    virtual String_t getDirectoryName();
    virtual String_t getTitle();

    NodeList& content()
        { return *m_content; }

    Ref<Stream> open(const String_t& name, const String_t& fullName, OpenMode mode);

 private:
    String_t m_name;
    Ref<NodeList> m_content;
    Ptr<Directory> m_parent;
};

/* Implementation of DirectoryEntry */
class afl::io::InternalFileSystem::Entry : public DirectoryEntry {
 public:
    Entry(Ref<Dir> parent, const String_t& name);
    ~Entry();
    virtual String_t getTitle();
    virtual String_t getPathName();
    virtual Ref<Stream> openFile(FileSystem::OpenMode mode);
    virtual Ref<Directory> openDirectory();
    virtual Ref<Directory> openContainingDirectory();
    virtual void updateInfo(uint32_t requested);
    virtual void doRename(String_t newName);
    virtual void doErase();
    virtual void doCreateAsDirectory();
    virtual void doSetFlag(FileFlag flag, bool value);
    virtual void doMoveTo(Directory& dir, String_t name);

 private:
    Ref<Dir> m_parent;
    String_t m_name;
};

/* Implementation of Enumerator */
class afl::io::InternalFileSystem::Enum : public afl::base::Enumerator<Ptr<DirectoryEntry> > {
 public:
    Enum(Ref<Dir> parent)
        : m_parent(parent),
          m_iterator(parent->content().nodes.begin())
        { }
    virtual bool getNextElement(Ptr<afl::io::DirectoryEntry>& result);

 private:
    Ref<Dir> m_parent;
    afl::container::PtrMultiList<Node>::iterator m_iterator;
};

/* Implementation of RootDir */
class afl::io::InternalFileSystem::RootDir : public Directory {
 public:
    RootDir(Ref<NodeList> content);
    ~RootDir();

    virtual Ref<DirectoryEntry> getDirectoryEntryByName(String_t name);
    virtual Ref<afl::base::Enumerator<Ptr<DirectoryEntry> > > getDirectoryEntries();
    virtual Ptr<Directory> getParentDirectory();
    virtual String_t getDirectoryName();
    virtual String_t getTitle();

    const Ref<NodeList>& content() const
        { return m_content; }

 private:
    Ref<NodeList> m_content;
};

/* Implementation of DirectoryEntry for root */
class afl::io::InternalFileSystem::RootEntry : public DirectoryEntry {
 public:
    RootEntry(Ref<RootDir> parent);
    ~RootEntry();
    virtual String_t getTitle();
    virtual String_t getPathName();
    virtual Ref<Stream> openFile(FileSystem::OpenMode mode);
    virtual Ref<Directory> openDirectory();
    virtual Ref<Directory> openContainingDirectory();
    virtual void updateInfo(uint32_t requested);
    virtual void doRename(String_t newName);
    virtual void doErase();
    virtual void doCreateAsDirectory();
    virtual void doSetFlag(FileFlag flag, bool value);
    virtual void doMoveTo(Directory& dir, String_t name);

 private:
    Ref<RootDir> m_parent;
};

/* Implementation of Enumerator for root */
class afl::io::InternalFileSystem::RootEnum : public afl::base::Enumerator<Ptr<DirectoryEntry> > {
 public:
    RootEnum(Ref<RootDir> parent)
        : m_parent(parent), m_did(false)
        { }
    virtual bool getNextElement(Ptr<afl::io::DirectoryEntry>& result);

 private:
    Ref<RootDir> m_parent;
    bool m_did;
};


/*
 *  NodeList
 */

/* Given a path name, descend to the target directory.
   For example, given name="a/b/c", returns the Dir corresponding to "a/b", leaving name="c".
   Throws on error. */
Ref<afl::io::InternalFileSystem::Dir>
afl::io::InternalFileSystem::NodeList::findDirectory(ConstStringMemory_t& pathName, const String_t& fullName)
{
    size_t n;
    Ptr<Dir> meDir = new Dir("/", *this, 0);
    while ((n = pathName.find('/')) < pathName.size()) {
        // Save the name
        ConstStringMemory_t componentName = pathName.split(n);

        // Consume the "/"
        pathName.split(1);

        // Descend unless name is "."
        if (!isDot(componentName)) {
            // Locate the directory name
            DirectoryNode* dn = dynamic_cast<DirectoryNode*>(meDir->content().findNode(componentName));
            if (dn == 0) {
                throw FileProblemException(fullName, Messages::fileNotFound());
            }

            // Descend into directory
            meDir = new Dir(PosixFileNames().makePathName(meDir->getDirectoryName(), afl::string::fromMemory(componentName)), *dn->content, meDir);
        }
    }
    return *meDir;
}

/* Find node, given a component name. */
afl::io::InternalFileSystem::Node*
afl::io::InternalFileSystem::NodeList::findNode(ConstStringMemory_t componentName)
{
    afl::container::PtrMultiList<Node>::iterator it = findIterator(componentName);
    return it != nodes.end()
        ? *it
        : 0;
}

/* Find iterator, given a component name */
afl::container::PtrMultiList<afl::io::InternalFileSystem::Node>::iterator
afl::io::InternalFileSystem::NodeList::findIterator(ConstStringMemory_t componentName)
{
    afl::container::PtrMultiList<Node>::iterator it = nodes.begin();
    while (it != nodes.end()) {
        Node* n = *it;
        if (n != 0 && componentName.equalContent(afl::string::toMemory(n->name))) {
            break;
        }
        ++it;
    }
    return it;
}


/*
 *  InternalFileSystem::MyDirectory
 */

/* Create instance.
   @param name      Full name "/a/b/c"
   @param content   Content
   @param parent    Parent directory (null if this is the root) */
afl::io::InternalFileSystem::Dir::Dir(String_t name, Ref<NodeList> content, Ptr<Directory> parent)
    : m_name(name),
      m_content(content),
      m_parent(parent)
{ }

afl::io::InternalFileSystem::Dir::~Dir()
{ }

Ref<afl::io::DirectoryEntry>
afl::io::InternalFileSystem::Dir::getDirectoryEntryByName(String_t name)
{
    verifyFileNameComponent(name);
    return *new Entry(*this, name);
}

Ref<afl::base::Enumerator<Ptr<afl::io::DirectoryEntry> > >
afl::io::InternalFileSystem::Dir::getDirectoryEntries()
{
    return *new Enum(*this);
}

Ptr<afl::io::Directory>
afl::io::InternalFileSystem::Dir::getParentDirectory()
{
    return m_parent;
}

String_t
afl::io::InternalFileSystem::Dir::getDirectoryName()
{
    return m_name;
}

String_t
afl::io::InternalFileSystem::Dir::getTitle()
{
    return m_parent.get() == 0
        ? Messages::rootDirTitle()
        : PosixFileNames().getFileName(m_name);
}

/* Implementation for Directory::open() and FileSystem::openFile()
   @param name     Local file name
   @param fullName Full name, for exceptions
   @param mode     Mode */
Ref<afl::io::Stream>
afl::io::InternalFileSystem::Dir::open(const String_t& name, const String_t& fullName, OpenMode mode)
{
    // File name must be valid
    verifyFileNameComponent(name);

    // Locate node
    Node* node = m_content->findNode(afl::string::toMemory(name));
    FileNode* fileNode = dynamic_cast<FileNode*>(node);

    // If it exists but is not a file, that is an error in every mode
    if (node != 0 && fileNode == 0) {
        throw FileProblemException(fullName, Messages::fileExists());
    }

    // Continue depending on mode
    switch (mode) {
     case OpenRead:
     case OpenWrite:
        // For now, we cannot write-protect a stream
        if (fileNode == 0) {
            throw FileProblemException(fullName, Messages::fileNotFound());
        }
        return fileNode->content->createChild();

     case Create:
        // Create or re-use
        if (fileNode == 0) {
            fileNode = new FileNode(name);
            m_content->nodes.pushBackNew(fileNode);
        } else {
            fileNode->content.reset(*new InternalStream());
        }
        fileNode->content->setName(fullName);
        return fileNode->content->createChild();

     case CreateNew:
        // Always create
        if (fileNode != 0) {
            throw FileProblemException(fullName, Messages::fileExists());
        }

        fileNode = new FileNode(name);
        m_content->nodes.pushBackNew(fileNode);
        fileNode->content->setName(fullName);
        return fileNode->content->createChild();
    }

    // If we end up here, mode was out-of-range
    throw FileProblemException(fullName, Messages::invalidOperation());
}

/*
 *  InternalFileSystem::Entry
 */

inline
afl::io::InternalFileSystem::Entry::Entry(Ref<Dir> parent, const String_t& name)
    : m_parent(parent),
      m_name(name)
{ }

afl::io::InternalFileSystem::Entry::~Entry()
{ }

String_t
afl::io::InternalFileSystem::Entry::getTitle()
{
    return m_name;
}

String_t
afl::io::InternalFileSystem::Entry::getPathName()
{
    return PosixFileNames().makePathName(m_parent->getDirectoryName(), m_name);
}

Ref<afl::io::Stream>
afl::io::InternalFileSystem::Entry::openFile(FileSystem::OpenMode mode)
{
    return m_parent->open(m_name, getPathName(), mode);
}

Ref<afl::io::Directory>
afl::io::InternalFileSystem::Entry::openDirectory()
{
    Node* n = m_parent->content().findNode(afl::string::toMemory(m_name));
    if (DirectoryNode* dn = dynamic_cast<DirectoryNode*>(n)) {
        return *new Dir(getPathName(), *dn->content, m_parent.asPtr());
    }
    throw FileProblemException(getPathName(), Messages::fileNotFound());
}

Ref<afl::io::Directory>
afl::io::InternalFileSystem::Entry::openContainingDirectory()
{
    return m_parent;
}

void
afl::io::InternalFileSystem::Entry::updateInfo(uint32_t /*requested*/)
{
    Node* n = m_parent->content().findNode(afl::string::toMemory(m_name));
    if (dynamic_cast<DirectoryNode*>(n) != 0) {
        // Directory: just type known
        setFileType(tDirectory);
    } else if (FileNode* fn = dynamic_cast<FileNode*>(n)) {
        // File: type and size
        setFileType(tFile);
        setFileSize(fn->content->getSize());
    } else {
        // Unknown (nonexistant file) */
        setFileType(tUnknown);
    }
}

void
afl::io::InternalFileSystem::Entry::doRename(String_t newName)
{
    // Verify target file name
    verifyFileNameComponent(newName);

    // Look up existing node
    NodeList& list = m_parent->content();
    Node* n = list.findNode(afl::string::toMemory(m_name));
    if (n == 0) {
        // Node does not exist: fail no matter what type
        throw FileProblemException(getPathName(), Messages::fileNotFound());
    } else if (newName != m_name) {
        // Node exists and we want to rename
        if (dynamic_cast<FileNode*>(n) != 0) {
            // Can use a file to overwrite another file
            afl::container::PtrMultiList<Node>::iterator it = list.findIterator(afl::string::toMemory(newName));
            if (it != list.nodes.end()) {
                // '*it == n' cannot happen; checking anyway because bad things would happen if it does
                if (*it == n || dynamic_cast<FileNode*>(*it) == 0) {
                    throw FileProblemException(getPathName(), Messages::fileExists());
                }
                list.nodes.erase(it);
            }
            n->name = newName;
        } else if (dynamic_cast<DirectoryNode*>(n) != 0) {
            // Cannot overwrite anything with a directory
            if (list.findNode(afl::string::toMemory(newName)) != 0) {
                throw FileProblemException(getPathName(), Messages::fileExists());
            }
            n->name = newName;
        } else {
            // Don't know how to rename this
            throw FileProblemException(getPathName(), Messages::fileNotFound());
        }
    } else {
        // no-op
    }
}

void
afl::io::InternalFileSystem::Entry::doErase()
{
    NodeList& list = m_parent->content();
    afl::container::PtrMultiList<Node>::iterator it = list.findIterator(afl::string::toMemory(m_name));
    if (it == list.nodes.end()) {
        // File does not exist
        throw FileProblemException(getPathName(), Messages::fileNotFound());
    } else if (DirectoryNode* dn = dynamic_cast<DirectoryNode*>(*it)) {
        // Directory: must be empty
        if (!dn->content->nodes.empty()) {
            throw FileProblemException(getPathName(), Messages::fileExists());
        }
        list.nodes.erase(it);
    } else if (dynamic_cast<FileNode*>(*it) != 0) {
        // File: erase
        list.nodes.erase(it);
    } else {
        // Invaild (cannot happen)
        throw FileProblemException(getPathName(), Messages::fileNotFound());
    }
}

void
afl::io::InternalFileSystem::Entry::doCreateAsDirectory()
{
    // Check conflicting file/directory
    NodeList& list = m_parent->content();
    if (list.findNode(afl::string::toMemory(m_name)) != 0) {
        throw FileProblemException(getPathName(), Messages::fileExists());
    }

    // Create it
    list.nodes.pushBackNew(new DirectoryNode(m_name));
}

void
afl::io::InternalFileSystem::Entry::doSetFlag(FileFlag /*flag*/, bool /*value*/)
{
    throw FileProblemException(getPathName(), Messages::invalidOperation());
}

void
afl::io::InternalFileSystem::Entry::doMoveTo(Directory& dir, String_t name)
{
    // Validate destination directory
    Dir* d = dynamic_cast<Dir*>(&dir);
    if (d == 0) {
        throw FileProblemException(getPathName(), Messages::invalidOperation());
    }

    // Locate source file
    NodeList& srcList = m_parent->content();
    afl::container::PtrMultiList<Node>::iterator srcIt = srcList.findIterator(afl::string::toMemory(m_name));
    if (srcIt == srcList.nodes.end()) {
        // Source file does not exist
        throw FileProblemException(getPathName(), Messages::fileNotFound());
    }

    // Locate destination
    NodeList& destList = d->content();
    afl::container::PtrMultiList<Node>::iterator destIt = destList.findIterator(afl::string::toMemory(name));

    if (dynamic_cast<FileNode*>(*srcIt) != 0) {
        // It'a a file. Validate/remove destination file
        if (destIt != destList.nodes.end()) {
            // Destination file exists, can we overwrite it?
            if (dynamic_cast<FileNode*>(*destIt) == 0) {
                throw FileProblemException(PosixFileNames().makePathName(dir.getDirectoryName(), name), Messages::fileExists());
            }
            destList.nodes.erase(destIt);
        }
    } else {
        // Not a file. Can only move if target does not exist
        if (destIt != destList.nodes.end()) {
            throw FileProblemException(PosixFileNames().makePathName(dir.getDirectoryName(), name), Messages::fileExists());
        }
    }

    // Move it
    Node* destNode = destList.nodes.pushBackNew(srcList.nodes.extractElement(srcIt));
    destNode->name = name;
}


/*
 *  InternalFileSystem::Enum
 */

bool
afl::io::InternalFileSystem::Enum::getNextElement(Ptr<afl::io::DirectoryEntry>& result)
{
    while (1) {
        // End reached?
        if (m_iterator == m_parent->content().nodes.end()) {
            return false;
        }

        // Found an element?
        if (*m_iterator != 0) {
            result = new Entry(m_parent, (*m_iterator)->name);
            ++m_iterator;
            return true;
        }

        // Skip deleted element
        ++m_iterator;
    }
}

/*
 *  RootDir
 */

afl::io::InternalFileSystem::RootDir::RootDir(Ref<NodeList> content)
    : m_content(content)
{ }

afl::io::InternalFileSystem::RootDir::~RootDir()
{ }

Ref<afl::io::DirectoryEntry>
afl::io::InternalFileSystem::RootDir::getDirectoryEntryByName(String_t name)
{
    throw FileProblemException(name, Messages::cannotAccessFiles());
}

Ref<afl::base::Enumerator<Ptr<afl::io::DirectoryEntry> > >
afl::io::InternalFileSystem::RootDir::getDirectoryEntries()
{
    return *new RootEnum(*this);
}

Ptr<afl::io::Directory>
afl::io::InternalFileSystem::RootDir::getParentDirectory()
{
    return 0;
}

String_t
afl::io::InternalFileSystem::RootDir::getDirectoryName()
{
    return String_t();
}

String_t
afl::io::InternalFileSystem::RootDir::getTitle()
{
    return Messages::rootDirTitle();
}


/*
 *  RootEntry
 */

afl::io::InternalFileSystem::RootEntry::RootEntry(Ref<RootDir> parent)
    : m_parent(parent)
{ }

afl::io::InternalFileSystem::RootEntry::~RootEntry()
{ }

String_t
afl::io::InternalFileSystem::RootEntry::getTitle()
{
    return Messages::rootLinkTitle();
}

String_t
afl::io::InternalFileSystem::RootEntry::getPathName()
{
    return "/";
}

Ref<afl::io::Stream>
afl::io::InternalFileSystem::RootEntry::openFile(FileSystem::OpenMode /*mode*/)
{
    throw FileProblemException(m_parent->getTitle(), Messages::cannotAccessFiles());
}

Ref<afl::io::Directory>
afl::io::InternalFileSystem::RootEntry::openDirectory()
{
    return *new Dir("/", m_parent->content(), 0);
}

Ref<afl::io::Directory>
afl::io::InternalFileSystem::RootEntry::openContainingDirectory()
{
    return m_parent;
}

void
afl::io::InternalFileSystem::RootEntry::updateInfo(uint32_t /*requested*/)
{
    setFileType(tDirectory);
    setFlags(FileFlags_t(Link));
    setLinkText("/");
}

void
afl::io::InternalFileSystem::RootEntry::doRename(String_t /*newName*/)
{
    throw FileProblemException(m_parent->getTitle(), Messages::cannotAccessFiles());
}

void
afl::io::InternalFileSystem::RootEntry::doErase()
{
    throw FileProblemException(m_parent->getTitle(), Messages::cannotAccessFiles());
}

void
afl::io::InternalFileSystem::RootEntry::doCreateAsDirectory()
{
    throw FileProblemException(m_parent->getTitle(), Messages::cannotAccessDirectories());
}

void
afl::io::InternalFileSystem::RootEntry::doSetFlag(FileFlag /*flag*/, bool /*value*/)
{
    throw FileProblemException(m_parent->getTitle(), Messages::cannotAccessFiles());
}

void
afl::io::InternalFileSystem::RootEntry::doMoveTo(Directory& /*dir*/, String_t /*name*/)
{
    throw FileProblemException(m_parent->getTitle(), Messages::cannotAccessFiles());
}


/*
 *  RootEnum
 */

bool
afl::io::InternalFileSystem::RootEnum::getNextElement(Ptr<afl::io::DirectoryEntry>& result)
{
    if (!m_did) {
        result = new RootEntry(m_parent);
        m_did = true;
        return true;
    } else {
        return false;
    }
}


/*
 *  InternalFileSystem
 */

afl::io::InternalFileSystem::InternalFileSystem()
    : m_root(*new NodeList())
{ }

afl::io::InternalFileSystem::~InternalFileSystem()
{ }

void
afl::io::InternalFileSystem::createDirectory(FileName_t dirName)
{
    ConstStringMemory_t name(prepareFileName(dirName));

    // Find parent
    Ref<Dir> dir = m_root->findDirectory(name, dirName);

    // Create it
    verifyFileNameComponent(afl::string::fromMemory(name));
    Entry(dir, afl::string::fromMemory(name)).createAsDirectory();
}

// FileSystem:
afl::base::Ref<afl::io::Stream>
afl::io::InternalFileSystem::openFile(FileName_t fileName, OpenMode mode)
{
    ConstStringMemory_t name(prepareFileName(fileName));

    // Find parent
    Ref<Dir> dir = m_root->findDirectory(name, fileName);

    // Do it
    return dir->open(afl::string::fromMemory(name), fileName, mode);
}

afl::base::Ref<afl::io::Directory>
afl::io::InternalFileSystem::openDirectory(FileName_t dirName)
{
    ConstStringMemory_t name(prepareFileName(dirName));

    // Find parent
    Ref<Dir> dir = m_root->findDirectory(name, dirName);

    // If dirName ends in "/", name now is empty and we're done.
    // Likewise, if dirName ends in a "." component, which happens when someone does openDirectory(getDirectoryName("x")).
    // Otherwise, resolve final component.
    if (!name.empty() && !isDot(name)) {
        DirectoryNode* dn = dynamic_cast<DirectoryNode*>(dir->content().findNode(name));
        if (dn == 0) {
            throw FileProblemException(dirName, Messages::fileNotFound());
        }
        dir.reset(*new Dir(PosixFileNames().makePathName(dir->getDirectoryName(), afl::string::fromMemory(name)), *dn->content, dir.asPtr()));
    }

    return *dir;
}

afl::base::Ref<afl::io::Directory>
afl::io::InternalFileSystem::openRootDirectory()
{
    return *new RootDir(m_root);
}

bool
afl::io::InternalFileSystem::isAbsolutePathName(FileName_t path)
{
    return PosixFileNames().isAbsolutePathName(path);
}

bool
afl::io::InternalFileSystem::isPathSeparator(char c)
{
    return PosixFileNames().isPathSeparator(c);
}

afl::io::FileSystem::FileName_t
afl::io::InternalFileSystem::makePathName(FileName_t path, FileName_t name)
{
    return PosixFileNames().makePathName(path, name);
}

afl::io::FileSystem::FileName_t
afl::io::InternalFileSystem::getCanonicalPathName(FileName_t name)
{
    return PosixFileNames().getCanonicalPathName(name);
}

afl::io::FileSystem::FileName_t
afl::io::InternalFileSystem::getAbsolutePathName(FileName_t name)
{
    return getCanonicalPathName(makePathName(getWorkingDirectoryName(), name));
}

afl::io::FileSystem::FileName_t
afl::io::InternalFileSystem::getFileName(FileName_t name)
{
    return PosixFileNames().getFileName(name);
}

afl::io::FileSystem::FileName_t
afl::io::InternalFileSystem::getDirectoryName(FileName_t name)
{
    return PosixFileNames().getDirectoryName(name);
}

afl::io::FileSystem::FileName_t
afl::io::InternalFileSystem::getWorkingDirectoryName()
{
    return "/";
}
