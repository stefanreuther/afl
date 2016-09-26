/**
  *  \file afl/io/multidirectory.cpp
  *  \brief Class afl::io::MultiDirectory
  */

#include <set>
#include "afl/io/multidirectory.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directoryentry.hpp"
#include "afl/string/messages.hpp"

/************************** MultiDirectory::Enum *************************/

class afl::io::MultiDirectory::Enum : public afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > {
 public:
    Enum(afl::base::Ptr<MultiDirectory> dir);
    virtual ~Enum();
    virtual bool getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result);

 private:
    afl::base::Ptr<MultiDirectory> m_parent;

    std::set<String_t> m_seenFileNames;

    afl::base::Ptr<DirectoryEntry> m_currentEntry;
    afl::base::Ptr<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > > m_currentIterator;
    Vector_t::size_type m_currentIndex;

    void next();
};

inline
afl::io::MultiDirectory::Enum::Enum(afl::base::Ptr<MultiDirectory> dir)
    : m_parent(dir),
      m_seenFileNames(),
      m_currentEntry(),
      m_currentIterator(),
      m_currentIndex(0)
{
    next();
}

inline
afl::io::MultiDirectory::Enum::~Enum()
{ }

bool
afl::io::MultiDirectory::Enum::getNextElement(afl::base::Ptr<afl::io::DirectoryEntry>& result)
{
    result = m_currentEntry;
    next();
    return result.get() != 0;
}

void
afl::io::MultiDirectory::Enum::next()
{
    bool found = false;
    while (!found) {
        if (m_currentIterator.get() != 0) {
            // I have an iterator. Check for an element.
            if (m_currentIterator->getNextElement(m_currentEntry)) {
                // Yay! Return it if we didn't already one named identically.
                // FIXME: the returned element will report the original directory as its parent.
                // FIXME: null check
                found = m_seenFileNames.insert(m_currentEntry->getTitle()).second;
            } else {
                // End. Discard this iterator.
                m_currentIterator = 0;
            }
        } else if (m_currentIndex < m_parent->m_directories.size()) {
            // Start iterating a directory
            m_currentIterator = m_parent->m_directories[m_currentIndex++]->getDirectoryEntries();
        } else {
            // No more directories.
            m_currentEntry = 0;
            found = true;
        }
    }
}



/***************************** MultiDirectory ****************************/

afl::base::Ptr<afl::io::MultiDirectory>
afl::io::MultiDirectory::create()
{
    return new MultiDirectory();
}

inline
afl::io::MultiDirectory::MultiDirectory()
    : Directory(),
      m_directories()
{ }

afl::io::MultiDirectory::~MultiDirectory()
{ }

// Directory:
afl::base::Ptr<afl::io::DirectoryEntry>
afl::io::MultiDirectory::getDirectoryEntryByName(String_t name)
{
    for (Vector_t::const_iterator i = m_directories.begin(), e = m_directories.end(); i != e; ++i) {
        try {
            afl::base::Ptr<DirectoryEntry> entry = (*i)->getDirectoryEntryByName(name);
            if (entry->getFileType() != DirectoryEntry::tUnknown) {
                return entry;
            }
        }
        catch (afl::except::FileProblemException&) { }
    }
    throw afl::except::FileProblemException(name, afl::string::Messages::cannotAccessFiles());
}

afl::base::Ptr<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > >
afl::io::MultiDirectory::getDirectoryEntries()
{
    return new Enum(this);
}

afl::base::Ptr<afl::io::Directory>
afl::io::MultiDirectory::getParentDirectory()
{
    // We don't have a parent; report 0.
    return 0;
}

String_t
afl::io::MultiDirectory::getDirectoryName()
{
    // This is a virtual directory; report blank.
    return String_t();
}

String_t
afl::io::MultiDirectory::getTitle()
{
    String_t result;
    for (Vector_t::const_iterator i = m_directories.begin(), e = m_directories.end(); i != e; ++i) {
        if (!result.empty()) {
            result += " + ";
        }
        result += (*i)->getTitle();
    }
    return result;
}

// MultiDirectory:
void
afl::io::MultiDirectory::addDirectory(afl::base::Ptr<Directory> dir)
{
    m_directories.push_back(dir);
}
