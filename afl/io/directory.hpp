/**
  *  \file afl/io/directory.hpp
  *  \brief Base Class afl::io::Directory
  */
#ifndef AFL_AFL_IO_DIRECTORY_HPP
#define AFL_AFL_IO_DIRECTORY_HPP

#include "afl/base/deletable.hpp"
#include "afl/base/ptr.hpp"
#include "afl/base/ref.hpp"
#include "afl/string/string.hpp"
#include "afl/base/refcounted.hpp"
#include "afl/base/enumerator.hpp"
#include "afl/io/filesystem.hpp"

namespace afl { namespace io {

    class Stream;
    class DirectoryEntry;

    /** A directory of files.
        Represents a physical or virtual directory.
        - Physical directories have a string representation of the directory name,
          which can later be used with FileSystem::openDirectory, for example.
          They can also be used to construct file names, or persisted for use in a later run.
        - Virtual directories do not have a string representation.
          Their content can only be accessed by following links from the Directory object. */
    class Directory : public afl::base::Deletable, public afl::base::RefCounted {
     public:
        /** Virtual destructor. */
        virtual ~Directory();

        /** Get directory entry for a name.
            The item need not exist.
            \param name Name of item
            \return directory entry. Never null.
            \throw FileProblemException if a DirectoryEntry cannot be created (only for immutable directories). */
        virtual afl::base::Ref<DirectoryEntry> getDirectoryEntryByName(String_t name) = 0;

        /** Get directory entry enumerator.
            \return enumerator. Never null.
            \throw FileProblemException if there is a problem creating the enumerator (e.g. opendir() fails). */
        virtual afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<DirectoryEntry> > > getDirectoryEntries() = 0;

        /** Get parent directory.
            \return parent directory. Can be null if there is no parent. */
        virtual afl::base::Ptr<Directory> getParentDirectory() = 0;

        /** Get directory name.
            For physical directories in the file system, this is a name compatible with FileSystem::openDirectory.
            For virtual directories, this is an empty string.
            \return directory name, UTF-8 encoded */
        virtual String_t getDirectoryName() = 0;

        /** Get directory title.
            This is the title to be shown to users.
            For physical or directories, it should be the basename of the directory.
            For virtual directories, it is the user-friendly name.
            \return Title, UTF-8 encoded */
        virtual String_t getTitle() = 0;

        /** Write back changes.
            If this Directory object represents content managed in a copy-out-copy-in fashion (e.g. an archive file),
            this will synchronize the content with the underlying storage.
            For objects representing actual file system directories, this is a no-op.
            \throw FileProblemException on error */
        virtual void flush() = 0;

        /*
         *  Convenience Functions
         */

        /** Open a subdirectory.
            This function is short for obtaining a directory entry and opening that.
            \param name Name of subdirectory
            \return directory, never null
            \throw FileProblemException if there is a problem */
        afl::base::Ref<Directory> openDirectory(String_t name);

        /** Open a file.
            This function is short for obtaining a directory entry and opening that.
            \param name Name of file
            \param mode Open mode
            \return file, never null
            \throw FileProblemException if there is a problem */
        afl::base::Ref<Stream> openFile(String_t name, FileSystem::OpenMode mode);

        /** Open a file, but don't throw FileProblemException's.
            This function is short for obtaining a directory entry and opening that.
            If there is a FileProblemException, it just returns null.
            This is for implementing "if (file exists) { read it }" patterns.

            Note that other exceptions (e.g. std::bad_alloc) will still propagate.
            \param name Name of file
            \param mode Open mode
            \return file, null if cannot be opened */
        afl::base::Ptr<Stream> openFileNT(String_t name, FileSystem::OpenMode mode);

        /** Erase a file.
            This function is short for obtaining a directory entry and erasing that.
            \param name Name
            \throw FileProblemException if there is a problem */
        void erase(String_t name);

        /** Erase a file, but don't throw FileProblemException's.
            This function is short for obtaining a directory entry and erasing that.
            If there is a FileProblemException, it just returns false.

            Note that other exceptions (e.g. std::bad_alloc) will still propagate.
            \param name Name of file
            \return true if file was erased successfully, false on error */
        bool eraseNT(String_t name);

        /** Erase all content, recursively.
            Obtains all directory entries and removes them; recursively if they are directories.
            \throw FileProblemException if there is a problem */
        void eraseContentRecursively();
    };

} }

#endif
