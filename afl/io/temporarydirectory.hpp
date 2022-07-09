/**
  *  \file afl/io/temporarydirectory.hpp
  *  \brief Class afl::io::TemporaryDirectory
  */
#ifndef AFL_AFL_IO_TEMPORARYDIRECTORY_HPP
#define AFL_AFL_IO_TEMPORARYDIRECTORY_HPP

#include "afl/io/directory.hpp"
#include "afl/io/directoryentry.hpp"

namespace afl { namespace io {

    /** Temporary directory.
        Creates and cleans up a temporary directory in RAII fashion.
        The directory name is chosen internally to avoid conflicts with existing directories.

        Use this when you need directory that has an actual file system representation,
        e.g. for communication with other programs,
        or for very large files.

        For temporary storage, consider using InternalDirectory, InternalFileSystem. */
    class TemporaryDirectory {
     public:
        /** Constructor.
            @param parentDirectory Parent directory. The temporary directory is created as a subdirectory of this directory. */
        explicit TemporaryDirectory(afl::base::Ref<Directory> parentDirectory);

        /** Destructor.
            Will remove the temporary directory and all its content. */
        ~TemporaryDirectory();

        /** Get directory.
            @return Directory object for the temporary directory */
        afl::base::Ref<Directory> get();

     private:
        afl::base::Ref<DirectoryEntry> m_entry;

        static afl::base::Ref<DirectoryEntry> create(Directory& parentDirectory);
    };

} }

#endif
