/**
  *  \file arch/win32/win32directory.hpp
  *  \brief Class arch::win32::Win32Directory
  */
#ifndef AFL_ARCH_WIN32_WIN32DIRECTORY_HPP
#define AFL_ARCH_WIN32_WIN32DIRECTORY_HPP

#include "afl/io/directory.hpp"

namespace arch { namespace win32 {

    /** Directory implementation for Win32. */
    class Win32Directory : public afl::io::Directory {
     public:
        class Entry;
        class EnumW;
        class EnumA;

        Win32Directory(String_t dirName);
        virtual ~Win32Directory();
        virtual afl::base::Ref<afl::io::DirectoryEntry> getDirectoryEntryByName(String_t name);
        virtual afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > > getDirectoryEntries();
        virtual afl::base::Ptr<afl::io::Directory> getParentDirectory();
        virtual String_t getDirectoryName();
        virtual String_t getTitle();

     private:
        String_t m_dirName;
    };

} }

#endif
