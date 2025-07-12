/**
  *  \file afl/io/internaldirectory.hpp
  *  \brief Class afl::io::InternalDirectory
  */
#ifndef AFL_AFL_IO_INTERNALDIRECTORY_HPP
#define AFL_AFL_IO_INTERNALDIRECTORY_HPP

#include "afl/io/directory.hpp"
#include "afl/container/ptrmultilist.hpp"
#include "afl/io/stream.hpp"

namespace afl { namespace io {

    /** Internal (In-Memory) Directory.
        Implements a virtual directory that provides access to a number of streams.
        Virtual files can be created within the InternalDirectory, but directories cannot be created.
        Files newly-created through the regular Directory interface will be of type InternalStream,
        but users can add files of other types (say, ConstMemoryStream, or even actual disk files)
        using the specific addStream() interface.

        Instances of InternalDirectory must be created dynamically;
        therefore, this class does not publish a constructor, but a factory function create().

        The moveTo() operation cannot move between different InternalDirectory instances.
        If you need that, use directory instances derived from InternalFileSystem. */
    class InternalDirectory : public Directory {
     public:
        /*
         *  Construction
         */

        /** Create a directory.
            Use this function to create instances.
            \param name Name of instance (for reporting in getTitle()).
            \return New instance */
        static afl::base::Ref<InternalDirectory> create(String_t name);

        /*
         *  Directory:
         */
        virtual ~InternalDirectory();
        virtual afl::base::Ref<DirectoryEntry> getDirectoryEntryByName(String_t name);
        virtual afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<DirectoryEntry> > > getDirectoryEntries();
        virtual afl::base::Ptr<Directory> getParentDirectory();
        virtual String_t getDirectoryName();
        virtual String_t getTitle();
        virtual void flush();

        /*
         *  InternalDirectory:
         */

        /** Add a stream (virtual file) to this directory.
            If one with the given name already exists, it is replaced.
            \param name Name of stream (virtual file)
            \param stream Stream object */
        void addStream(String_t name, afl::base::Ref<Stream> stream);

        /** Get stream (virtual file).
            This is a possibly more convenient interface than the regular Directory interface.
            \param name Name of stream
            \return Stream object; null if none */
        afl::base::Ptr<Stream> getStream(String_t name);

        /** Delete a stream (virtual file).
            \param name Name of stream
            \retval true Stream was removed
            \retval false Stream not found */
        bool eraseStream(String_t name);

        /** Rename a stream (virtual file).
            If a stream already exists under the newName, that one will be deleted.
            \param oldName Old name
            \param newName New name
            \retval true Renaming succeeded
            \retval false oldName not found */
        bool renameStream(String_t oldName, String_t newName);

     private:
        InternalDirectory(String_t name);

        class Enum;
        class Entry;
        friend class Enum;
        friend class Entry;

        struct Node {
            String_t m_name;
            afl::base::Ref<Stream> m_stream;

            Node(String_t name, afl::base::Ref<Stream> stream);
        };
        typedef afl::container::PtrMultiList<Node> NodeList_t;

        String_t m_name;
        NodeList_t m_content;

        NodeList_t::iterator findName(const String_t& name) const;
    };

} }

#endif
