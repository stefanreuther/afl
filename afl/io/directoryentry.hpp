/**
  *  \file afl/io/directoryentry.hpp
  *  \brief Base Class afl::io::DirectoryEntry
  */
#ifndef AFL_AFL_IO_DIRECTORYENTRY_HPP
#define AFL_AFL_IO_DIRECTORYENTRY_HPP

#include "afl/base/deletable.hpp"
#include "afl/base/ptr.hpp"
#include "afl/base/refcounted.hpp"
#include "afl/base/types.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/io/stream.hpp"
#include "afl/sys/time.hpp"
#include "afl/bits/smallset.hpp"

namespace afl { namespace io {

    /** Directory entry.
        A directory entry refers to a file within a Directory.
        It allows accessing the actual file, or manipulating it as a whole (rename, erase).
        The actual implementation is related to the system-specific implementation of Directory.

        A DirectoryEntry also allows access to a file's properties.
        These are read on demand as needed. */
    class DirectoryEntry : public afl::base::Deletable, public afl::base::RefCounted {
     public:
        /** File size. */
        typedef Stream::FileSize_t FileSize_t;

        /** File type. */
        enum FileType {
            /** We don't know what it is. Maybe it doesn't exist at all or we don't have permissions.
                A dead link will have the "Link" flag and status tUnknown. */
            tUnknown,
            /** A regular file. It can be opened as a file. */
            tFile,
            /** A directory. It can be opened as a directory. */
            tDirectory,
            /** An archive. It can be opened as a file or directory. */
            tArchive,
            /** A root directory. It can be opened as a directory, but has no parent. */
            tRoot,
            /** A device. It can be opened, but will probably behave weird. */
            tDevice,
            /** Something else. We don't know what it is. */
            tOther
        };

        /** Flags.
            Directory entries can have an arbitrary combination of these flags. */
        enum FileFlag {
            /** This is a hidden file. */
            Hidden,
            /** This is a symbolic link. */
            Link
        };
        typedef afl::bits::SmallSet<FileFlag> FileFlags_t;

        /** Constructor. */
        DirectoryEntry();

        /** Destructor. */
        virtual ~DirectoryEntry();

        /** Get file type.
            \return file type. tUnknown if not known. */
        FileType getFileType();

        /** Get file size.
            \return file size in bytes. 0 if not known. */
        FileSize_t getFileSize();

        /** Get link text.
            \return link text, empty string if not known. */
        String_t getLinkText();

        /** Get modification time.
            \return modification time, invalid if not known. */
        afl::sys::Time getModificationTime();

        /** Get flags.
            \return flags, empty if not known. */
        FileFlags_t getFlags();

        /** Rename directory entry.
            The DirectoryEntry keeps refering to the original name.
            \param newName New file name. Should be just a file name, not a path name (in a different directory).
            \throw FileProblemException if there is a problem */
        void renameTo(String_t newName);

        /** Rename directory entry.
            The DirectoryEntry keeps refering to the original name.

            This is a wrapper for erase; if there is a FileProblemException, this function just returns false.
            Other exceptions propagate normally.

            \param newName New file name. Should be just a file name, not a path name (in a different directory).
            \return true on success */
        bool renameToNT(String_t newName);

        /** Erase directory entry.
            The DirectoryEntry keeps refering to the original name.
            For example, it can be created anew using openFile().
            \throw FileProblemException if there is a problem */
        void erase();

        /** Erase directory entry, but don't throw FileProblemException's.
            The DirectoryEntry keeps refering to the original name.
            For example, it can be created anew using openFile().

            This is a wrapper for erase; if there is a FileProblemException, this function just returns false.
            Other exceptions propagate normally.
            \return true on success */
        bool eraseNT();

        /** Create as directory.
            The created directory can be opened using openDirectory().
            \throw FileProblemException if there is a problem */
        void createAsDirectory();

        /** Get title of this directory entry.
            This is the title to be shown to users.
            For actual disk files or directories, it should be the basename of the file or directory.
            For virtual files, it is the user-friendly name.
            \return Title, UTF-8 encoded */
        virtual String_t getTitle() = 0;

        /** Get path name of this directory.
            If this is a real disk file or directory, this is a name compatible with
            FileSystem::openFile or FileSystem::openDirectory.
            For virtual files, it is an empty string.
            \return Path name, UTF-8 encoded */
        virtual String_t getPathName() = 0;

        /** Open this entry as a file.
            \param mode Open mode
            \return Stream, never null
            \throw FileProblemException if there is a problem */
        virtual afl::base::Ptr<Stream> openFile(FileSystem::OpenMode mode) = 0;

        /** Open this entry as a directory.
            \return Directory, never null
            \throw FileProblemException if there is a problem */
        virtual afl::base::Ptr<Directory> openDirectory() = 0;

        /** Open this entry's parent.
            \return Directory, never null. */
        virtual afl::base::Ptr<Directory> openContainingDirectory() = 0;

     protected:
        static const uint32_t InfoType = 1;
        static const uint32_t InfoSize = 2;
        static const uint32_t InfoLinkText = 4;
        static const uint32_t InfoModificationTime = 8;
        static const uint32_t InfoFlags = 16;

        /** Update file information.
            Called by the information-retrieval functions.
            Must call setFileType(), setFileSize() as needed.

            This function can provide more information than required, saving further calls.
            It can also provide less information than required, causing the user request to return a default.

            \param requested Bitfield of required information.
            This function must provide at least the required information.
            If that information is not provided, we assume it does not exist and return defaults. */
        virtual void updateInfo(uint32_t requested) = 0;

        /** Rename, implementation.
            \param newName New file name
            \throw FileProblemException if there is a problem */
        virtual void doRename(String_t newName) = 0;

        /** Erase, implementation.
            \throw FileProblemException if there is a problem */
        virtual void doErase() = 0;

        /** Create as directory, implementation.
            \throw FileProblemException if there is a problem */
        virtual void doCreateAsDirectory() = 0;

        /** Set file type.
            updateInfo() should call this to set the file type.
            \param type File type */
        void setFileType(FileType type);

        /** Set file size.
            updateInfo() should call this to set the file size.
            \param size File size */
        void setFileSize(FileSize_t size);

        /** Set link text.
            updateInfo() should call this to set the link text.
            \param text Link text */
        void setLinkText(String_t text);

        /** Set modification time.
            updateInfo() should call this to set the modification time.
            \param modificationTime Modification time */
        void setModificationTime(afl::sys::Time modificationTime);

        /** Set file flags.
            updateInfo() should call this to set the flags.
            \param flags flags */
        void setFlags(FileFlags_t flags);

        /** Clear information.
            Forget all stored information.
            Future calls to information retrieval functions will call updateInfo() again.
            You can call this if you know that the information now is stale. */
        void clearInfo();

     private:
        /** Bitfield of available information. */
        uint32_t m_availableInfo;

        /** Bitfield of requested information.
            If a bit is set here but not in m_availableInfo,
            the information is known to be not available. */
        uint32_t m_requestedInfo;

        /** File size. */
        FileSize_t m_fileSize;

        /** File type. */
        FileType m_fileType;

        /** Link text. */
        String_t m_linkText;

        /** Modification time. */
        afl::sys::Time m_modificationTime;

        /** Flags. */
        FileFlags_t m_flags;

        /** Fetch information, common condition.
            Tries to make the requested information available.
            \param requested Requested information
            \return true iff the information is available. */
        bool fetchInfo(uint32_t requested);
    };

} }

#endif
