/**
  *  \file afl/io/internalfilesystem.hpp
  *  \brief Class afl::io::InternalFileSystem
  */
#ifndef AFL_AFL_IO_INTERNALFILESYSTEM_HPP
#define AFL_AFL_IO_INTERNALFILESYSTEM_HPP

#include "afl/base/ref.hpp"
#include "afl/container/ptrmultilist.hpp"
#include "afl/io/directory.hpp"
#include "afl/io/filesystem.hpp"

namespace afl { namespace io {

    /** Internal (in-memory) file system implementation.

        Implements a minimal version of FileSystem, primarily for testing, but possibly useful elsewhere as well.

        File names follow POSIX syntax, i.e. using slashes as separator.
        The current directory is always the root, so name "dir/file" and "/dir/file" are equivalent.
        You can create files and directories.
        File names are case-sensitive, all characters permitted.

        File flags, timestamps and symlinks are not supported.

        Special cases:
        - The "dot-dot" syntax as in "a/../b" is not supported in file names, but `getCanonicalPathName()` will clean it up.
        - The "dot" notation is supported because it appears as `getDirectoryName("file")`.
        - Empty path name components/multiple slashes as in "a//b" are not supported.
          These typically only appear when code uses string concatenation instead of `makePathName()`.
          As an exception, a trailing slash on a directory name such as "a/b/" is accepted.
        - Operations on a `DirectoryEntry` do not support names containing slashes.
        - openRootDirectory() will return a single root, "/".

        Lifetime: objects are linked using Ptr/Ref.
        A file stream can still be used after having been erased, or even after the InternalFileSystem object has been destroyed.
        Likewise, a Directory or DirectoryEntry can still be used after having been deleted. */
    class InternalFileSystem : public FileSystem {
     public:
        /** Constructor.
            Make an empty file system instance. */
        InternalFileSystem();

        /** Destructor. */
        ~InternalFileSystem();

        /** Create a directory.
            This is a shortcut for DirectoryEntry::createAsDirectory().
            @param dirName directory name
            @throw afl::except::FileProblemException on error (parent does not exist, name already exists) */
        void createDirectory(FileName_t dirName);

        // FileSystem:
        virtual afl::base::Ref<Stream> openFile(FileName_t fileName, OpenMode mode);
        virtual afl::base::Ref<Directory> openDirectory(FileName_t dirName);
        virtual afl::base::Ref<Directory> openRootDirectory();
        virtual bool isAbsolutePathName(FileName_t path);
        virtual bool isPathSeparator(char c);
        virtual FileName_t makePathName(FileName_t path, FileName_t name);
        virtual FileName_t getCanonicalPathName(FileName_t name);
        virtual FileName_t getAbsolutePathName(FileName_t name);
        virtual FileName_t getFileName(FileName_t name);
        virtual FileName_t getDirectoryName(FileName_t name);
        virtual FileName_t getWorkingDirectoryName();

     private:
        struct FileNode;
        struct DirectoryNode;
        class Dir;
        class Entry;
        class Enum;
        class RootDir;
        class RootEntry;
        class RootEnum;

        struct Node : public afl::base::Deletable {
            String_t name;
            Node(const String_t& name)
                : name(name)
                { }
        };

        struct NodeList : public afl::base::RefCounted {
            afl::container::PtrMultiList<Node> nodes;

            afl::base::Ref<Dir> findDirectory(afl::string::ConstStringMemory_t& pathName, const String_t& fullName);
            Node* findNode(afl::string::ConstStringMemory_t componentName);
            afl::container::PtrMultiList<Node>::iterator findIterator(afl::string::ConstStringMemory_t componentName);
        };

        afl::base::Ref<NodeList> m_root;
    };

} }

#endif
