/**
  *  \file afl/io/archive/zipreader.hpp
  *  \brief Class afl::io::archive::ZipReader
  */
#ifndef AFL_AFL_IO_ARCHIVE_ZIPREADER_HPP
#define AFL_AFL_IO_ARCHIVE_ZIPREADER_HPP

#include "afl/io/directory.hpp"
#include "afl/base/ref.hpp"
#include "afl/io/stream.hpp"
#include "afl/container/ptrvector.hpp"

namespace afl { namespace io { namespace archive {

    /** ZIP file read access.
        This provides simple, read-only access to a ZIP file.
        It builds an in-memory index of the member files on-the fly.

        Limitations:
        - we support only "classic" PKZIP 2.0 compression (deflate), no Deflate64, LZMA, etc.
        - the ZIP file is "flattened" i.e. all directory structure removed.
          Use the KeepPaths option to get paths reported anyway.
        - compressed members do not allow seeking.
        - unlike many zippers, this one reads the local headers, not the central headers.
        - unless specifically marked as Unicode, file names are treated as codepage 437 (western DOS).
        - unsupported entries are ignored. In particular, if a file is not a zip file at all,
          it is treated as empty (with a large unsupported entry). */
    class ZipReader : public Directory {
     public:
        /** Constructor.
            Open a file a ZIP file for reading.
            \param file ZIP file
            \param options Options for interpreting the file
            \return ZipReader */
        static afl::base::Ref<ZipReader> open(afl::base::Ref<Stream> file, int options);

        /** Virtual destructor. */
        virtual ~ZipReader();

        enum {
            /** Option: keep paths.
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
        virtual void flush();

     private:
        afl::base::Ref<Stream> m_file;      /**< Underlying file. */
        afl::base::Ref<Stream> m_view;      /**< child stream, Isolation against other users. */

        int m_options;

        // Index
        struct IndexEntry;
        afl::container::PtrVector<IndexEntry> m_index;
        bool m_indexerReachedEnd;
        bool m_hadUnsupportedFeature;

        // Directory interface
        class ZipDirEntry;
        class ZipDirEnum;
        class ZipStoredMember;
        class ZipDeflatedMember;
        friend class ZipDirEntry;
        friend class ZipDirEnum;
        friend class ZipStoredMember;
        friend class ZipDeflatedMember;

        ZipReader(afl::base::Ref<Stream> file, int options);

        bool readNextEntry();
        IndexEntry* findEntryByName(const String_t& name, size_t start, size_t end);
    };

} } }

#endif
