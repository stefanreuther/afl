/**
  *  \file afl/io/multidirectory.hpp
  *  \brief Class afl::io::MultiDirectory
  */
#ifndef AFL_AFL_IO_MULTIDIRECTORY_HPP
#define AFL_AFL_IO_MULTIDIRECTORY_HPP

#include <vector>
#include "afl/io/directory.hpp"

namespace afl { namespace io {

    /** Directory Multiplexer.
        MultiDirectory provides a directory that behaves as the union of multiple other directories.
        Files are looked up in all directories, taking the first matching one.
        This can be used to implement search paths.

        Entries cannot be created or modified in a MultiDirectory. */
    class MultiDirectory : public Directory {
     public:
        /*
         *  Construction
         */

        /** Create a directory.
            Use this function to create instances.
            \return New instance */
        static afl::base::Ptr<MultiDirectory> create();

        // Directory:
        virtual ~MultiDirectory();
        virtual afl::base::Ptr<DirectoryEntry> getDirectoryEntryByName(String_t name);
        virtual afl::base::Ptr<afl::base::Enumerator<afl::base::Ptr<DirectoryEntry> > > getDirectoryEntries();
        virtual afl::base::Ptr<Directory> getParentDirectory();
        virtual String_t getDirectoryName();
        virtual String_t getTitle();

        // MultiDirectory:

        /** Add new directory.
            Entries will be looked up in this directory if they are not found in the previous ones.
            \param dir New directory */
        void addDirectory(afl::base::Ptr<Directory> dir);

     private:
        MultiDirectory();

        typedef std::vector<afl::base::Ptr<Directory> > Vector_t;

        Vector_t m_directories;

        class Enum;
        friend class Enum;
    };

} }

#endif
