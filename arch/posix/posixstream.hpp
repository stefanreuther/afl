/**
  *  \file arch/posix/posixstream.hpp
  *  \brief Class arch::posix::PosixStream
  */
#ifndef AFL_ARCH_POSIX_POSIXSTREAM_HPP
#define AFL_ARCH_POSIX_POSIXSTREAM_HPP

#include "afl/io/stream.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/base/uncopyable.hpp"
#include "afl/io/multiplexablestream.hpp"

namespace arch { namespace posix {

    /** Implementation of Stream for POSIX. */
    class PosixStream : public afl::io::MultiplexableStream, public afl::base::Uncopyable {
     public:
        PosixStream(afl::io::FileSystem::FileName_t name, afl::io::FileSystem::OpenMode mode);
        PosixStream(afl::io::FileSystem::FileName_t name, int fd);
        ~PosixStream();
        virtual size_t read(Bytes_t m);
        virtual size_t write(ConstBytes_t m);
        virtual void flush();
        virtual void setPos(FileSize_t pos);
        virtual FileSize_t getPos();
        virtual FileSize_t getSize();
        virtual uint32_t getCapabilities();
        virtual String_t getName();
        virtual afl::base::Ptr<afl::io::FileMapping> createFileMapping(FileSize_t limit);

        int getFileDescriptor() const;

     private:
        int m_fd;
        String_t m_name;
        uint32_t m_capabilities;

        void init(afl::io::FileSystem::FileName_t name, afl::io::FileSystem::OpenMode mode);
        void init(int fd);
        void error();
    };

} }

inline int
arch::posix::PosixStream::getFileDescriptor() const
{
    return m_fd;
}

#endif
