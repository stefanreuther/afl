/**
  *  \file arch/win32/win32root.hpp
  *  \brief Class arch::win32::Win32Root
  */
#ifndef AFL_ARCH_WIN32_WIN32ROOT_HPP
#define AFL_ARCH_WIN32_WIN32ROOT_HPP

#include "afl/io/directory.hpp"

namespace arch { namespace win32 {

    /** Root directory implementation for Win32. */
    class Win32Root : public afl::io::Directory {
     public:
        class Entry;
        class Enum;

        Win32Root();
        virtual ~Win32Root();
        virtual afl::base::Ref<afl::io::DirectoryEntry> getDirectoryEntryByName(String_t name);
        virtual afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > > getDirectoryEntries();
        virtual afl::base::Ptr<afl::io::Directory> getParentDirectory();
        virtual String_t getDirectoryName();
        virtual String_t getTitle();
    };

} }

#endif
