/**
  *  \file arch/win32/win32stream.hpp
  *  \brief Class arch::win32::Win32Stream
  */
#ifndef AFL_ARCH_WIN32_WIN32STREAM_HPP
#define AFL_ARCH_WIN32_WIN32STREAM_HPP

#ifndef NOMINMAX
# define NOMINMAX
#endif
#include <windows.h>
#include "afl/io/multiplexablestream.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/base/uncopyable.hpp"

namespace arch { namespace win32 {

    /** Stream implementation for Win32.
        Implements all I/O operations for Windows using Kernel32 calls (unbuffered, untranslated I/O). */
    class Win32Stream : public afl::io::MultiplexableStream, public afl::base::Uncopyable {
     public:
        Win32Stream(afl::io::FileSystem::FileName_t name, afl::io::FileSystem::OpenMode mode);
        Win32Stream(afl::io::FileSystem::FileName_t name, HANDLE h);
        ~Win32Stream();
        virtual size_t read(Bytes_t m);
        virtual size_t write(ConstBytes_t m);
        virtual void flush();
        virtual void setPos(FileSize_t pos);
        virtual FileSize_t getPos();
        virtual FileSize_t getSize();
        virtual uint32_t getCapabilities();
        virtual String_t getName();
        virtual afl::base::Ptr<afl::io::FileMapping> createFileMapping(FileSize_t limit);

     private:
        HANDLE m_handle;
        String_t m_name;
        uint32_t m_capabilities;

        void init(afl::io::FileSystem::FileName_t name, afl::io::FileSystem::OpenMode mode);
        void init(HANDLE h);
        void error();
    };

} }

#endif
