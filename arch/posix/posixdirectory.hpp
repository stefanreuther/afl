/**
  *  \file arch/posix/posixdirectory.hpp
  *  \brief Class arch::posix::PosixDirectory
  */
#ifndef AFL_ARCH_POSIX_POSIXDIRECTORY_HPP
#define AFL_ARCH_POSIX_POSIXDIRECTORY_HPP

#include "afl/io/directory.hpp"

namespace arch { namespace posix {

    /** Directory implementation for POSIX. */
    class PosixDirectory : public afl::io::Directory {
     public:
        class Entry;
        class Enum;

        PosixDirectory(String_t dirName);
        virtual ~PosixDirectory();
        virtual afl::base::Ref<afl::io::DirectoryEntry> getDirectoryEntryByName(String_t name);
        virtual afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > > getDirectoryEntries();
        virtual afl::base::Ptr<afl::io::Directory> getParentDirectory();
        virtual String_t getDirectoryName();
        virtual String_t getTitle();
        virtual void flush();

     private:
        String_t m_dirName;
    };

} }

#endif
