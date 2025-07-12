/**
  *  \file arch/posix/posixroot.hpp
  *  \brief Class arch::posix::PosixRoot
  */
#ifndef AFL_ARCH_POSIX_POSIXROOT_HPP
#define AFL_ARCH_POSIX_POSIXROOT_HPP

#include "afl/io/directory.hpp"

namespace arch { namespace posix {

    /** Root directory implementation for POSIX. */
    class PosixRoot : public afl::io::Directory {
     public:
        class Entry;
        class Enum;

        PosixRoot();
        virtual ~PosixRoot();
        virtual afl::base::Ref<afl::io::DirectoryEntry> getDirectoryEntryByName(String_t name);
        virtual afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > > getDirectoryEntries();
        virtual afl::base::Ptr<afl::io::Directory> getParentDirectory();
        virtual String_t getDirectoryName();
        virtual String_t getTitle();
        virtual void flush();
    };

} }

#endif
