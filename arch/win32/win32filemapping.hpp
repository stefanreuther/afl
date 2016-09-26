/**
  *  \file arch/win32/win32filemapping.hpp
  *  \brief Class arch::win32::Win32FileMapping
  */
#ifndef AFL_ARCH_WIN32_WIN32FILEMAPPING_HPP
#define AFL_ARCH_WIN32_WIN32FILEMAPPING_HPP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "afl/io/filemapping.hpp"
#include "afl/base/ptr.hpp"
#include "afl/base/uncopyable.hpp"
#include "afl/io/stream.hpp"

namespace arch { namespace win32 {

    /** Implementation of FileMapping for Win32. */
    class Win32FileMapping : public afl::io::FileMapping, private afl::base::Uncopyable {
     public:
        Win32FileMapping(void* address, size_t bytesBefore, size_t bytesMapped);

        virtual ~Win32FileMapping();

        virtual afl::base::ConstBytes_t get() const;

        /** Create file mapping. */
        static afl::base::Ptr<afl::io::FileMapping> createFileMapping(HANDLE hFile, afl::io::Stream::FileSize_t limit);

     private:
	void* m_address;
	size_t m_bytesBefore;
	size_t m_bytesMapped;
    };

} }

#endif
