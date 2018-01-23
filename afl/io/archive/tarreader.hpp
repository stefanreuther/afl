/**
  *  \file afl/io/archive/tarreader.hpp
  *  \brief Class afl::io::archive::TarReader
  */
#ifndef AFL_AFL_IO_ARCHIVE_TARREADER_HPP
#define AFL_AFL_IO_ARCHIVE_TARREADER_HPP

#include "afl/io/directory.hpp"
#include "afl/base/ref.hpp"
#include "afl/container/ptrvector.hpp"
#include "afl/io/stream.hpp"

namespace afl { namespace io { namespace archive {

    /** TAR file read access.
        This provides simple, read-only access to a ZIP file.
        It builds an in-memory index of the member files on-the fly.

        Limitations:
        - "ustar" file format only.
        - directory structure is removed unless you provide the KeepPaths option.
        - it supports reading .tar.gz (or any other unseekable/transform-based stream) and its content directly,
          but only if you request each file's content immediately after receiving the DirectoryEntry.
          Seekable (uncompressed) files are not restricted. */
    class TarReader : public Directory {
     public:
        /** Constructor.
            Open a file a TAR file for reading.
            This method ensures that all TarReader instances are created dynamically.
            \param file TAR file
            \param options Options for interpreting the file
            \return TarReader */
        static afl::base::Ref<TarReader> open(afl::base::Ref<Stream> file, int options);

        virtual ~TarReader();

        enum {
            /** Keep paths.
                If specified, reports path names "as-is".
                If not specified (default), reports path names restricted to their final component,
                thus "flattening" the file. */
            KeepPaths = 1
        };

        // Directory:
        virtual afl::base::Ref<DirectoryEntry> getDirectoryEntryByName(String_t name);
        virtual afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<DirectoryEntry> > > getDirectoryEntries();
        virtual afl::base::Ptr<Directory> getParentDirectory();
        virtual String_t getDirectoryName();
        virtual String_t getTitle();

     private:
        /** Underlying file. */
        afl::base::Ref<Stream> m_file;

        /** Options. */
        int m_options;

        /** Position of next file header. */
        afl::io::Stream::FileSize_t m_nextEntryPosition;

        /** true if end has been reached and no more headers ought to be read. */
        bool m_atEnd;

        /*
         *  Index
         *
         *  We remember all (supported) entries in the Index.
         */

        struct IndexEntry;

        /** Index. Contains IndexEntry for each element encountered so far. */
        afl::container::PtrVector<IndexEntry> m_index;


        /*
         *  Directory interface
         */

        class TarDirEntry;
        class TarDirEnum;
        class TarStoredMember;
        friend class TarDirEntry;
        friend class TarDirEnum;
        friend class TarStoredMember;

        /** Private constructor.
            \param file TAR file
            \param options Options for interpreting the file */
        TarReader(afl::base::Ref<Stream> file, int options);

        /** Read next entry.
            Add it to the index if applicable.
            \retval true success
            \retval false file end reached */
        bool readNextEntry();

        /** Find entry, given a name.
            This searches a portion of the index, and is the basic primitive we build file-by-name access from.
            \param name Name to find
            \param start Starting index [0,end]
            \param end Ending index [0,m_index.size()) */
        const IndexEntry* findEntryByName(const String_t& name, size_t start, size_t end) const;

        /** Normalize a file name according to the options given.
            \param name [in/out] File name */
        void normalizeName(String_t& name) const;
    };

} } }

#endif
