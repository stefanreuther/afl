/**
  *  \file arch/posix/posixfilemapping.hpp
  *  \brief Class arch::posix::PosixFileMapping
  */
#ifndef AFL_ARCH_POSIX_POSIXFILEMAPPING_HPP
#define AFL_ARCH_POSIX_POSIXFILEMAPPING_HPP

#include "afl/io/filemapping.hpp"
#include "afl/base/ptr.hpp"
#include "afl/base/uncopyable.hpp"
#include "afl/io/stream.hpp"

namespace arch { namespace posix {

    /** Implementation of FileMapping for POSIX. */
    class PosixFileMapping : public afl::io::FileMapping, private afl::base::Uncopyable {
     public:
        /** Constructor.
            Receives the result of the mmap() operation.
            \param address Mapped address
            \param bytesBefore Mapped bytes before user-requested data (in case of misaligned request)
            \param bytesMapped Total number of bytes mapped (including bytesBefore) */
        PosixFileMapping(void* address, size_t bytesBefore, size_t bytesMapped);

        virtual ~PosixFileMapping();

        virtual afl::base::ConstBytes_t get() const;

        /** Create file mapping. */
        static afl::base::Ptr<afl::io::FileMapping> createFileMapping(int fd, afl::io::Stream::FileSize_t limit);

     private:
        void* m_address;
        size_t m_bytesBefore;
        size_t m_bytesMapped;
    };

} }

#endif
