/**
  *  \file afl/io/archive/arreader.hpp
  *  \brief Class afl::io::archive::ArReader
  */
#ifndef AFL_AFL_IO_ARCHIVE_ARREADER_HPP
#define AFL_AFL_IO_ARCHIVE_ARREADER_HPP

#include "afl/io/directory.hpp"
#include "afl/base/growablememory.hpp"
#include "afl/base/ref.hpp"
#include "afl/container/ptrvector.hpp"
#include "afl/io/stream.hpp"

namespace afl { namespace io { namespace archive {

    /** AR file read access.
        This provides simple, read-only access to a AR (Unix static library) file.
        It builds an in-memory index of the member files on-the fly.

        Features:
        - supports long file names
        - skips armap (symbol table)
        - supports reading unseekable streams if you request each file's content
          immediately after receiving the DirectoryEntry, random access to all members
          for random-accessible files */
    class ArReader : public Directory {
     public:
        /** Constructor.
            Open an archive file file for reading.
            This method ensures that all ArReader instances are created dynamically.
            \param file File
            \return ArReader */
        static afl::base::Ref<ArReader> open(afl::base::Ref<Stream> file);

        virtual ~ArReader();

        // Directory:
        virtual afl::base::Ref<DirectoryEntry> getDirectoryEntryByName(String_t name);
        virtual afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<DirectoryEntry> > > getDirectoryEntries();
        virtual afl::base::Ptr<Directory> getParentDirectory();
        virtual String_t getDirectoryName();
        virtual String_t getTitle();
        virtual void flush();

     private:
        /** Underlying file. */
        afl::base::Ref<Stream> m_file;

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

        /** Long name table.
            In-memory copy of the long-name-table, if any. */
        afl::base::GrowableBytes_t m_longNameTable;


        /*
         *  Directory interface
         */

        class ArDirEntry;
        class ArDirEnum;
        class ArMember;
        friend class ArDirEntry;
        friend class ArDirEnum;
        friend class ArMember;

        /** Private constructor.
            \param file file */
        explicit ArReader(afl::base::Ref<Stream> file);

        /** Read header.
            Throw on file format error. */
        void readHeader();

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
    };

} } }

#endif
