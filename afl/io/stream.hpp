/**
  *  \file afl/io/stream.hpp
  *  \brief Class afl::io::Stream
  */
#ifndef AFL_AFL_IO_STREAM_HPP
#define AFL_AFL_IO_STREAM_HPP

#include "afl/base/refcounted.hpp"
#include "afl/base/memory.hpp"
#include "afl/base/types.hpp"
#include "afl/string/string.hpp"
#include "afl/base/ptr.hpp"
#include "afl/io/datasink.hpp"

namespace afl { namespace io {

    class FileMapping;

    /** Data stream.
        An abstract interface for reading or writing data from a file or other data source. */
    class Stream : public afl::base::RefCounted, public DataSink /* includes Deletable */ {
     public:
        /*
         *  Capabilities
         */

        /** "Read" capability. If set in getCapabilities(), this stream claims to support read(). */
        static const uint32_t CanRead = 1;

        /** "Write" capability. If set in getCapabilities(), this stream claims to support write(). */
        static const uint32_t CanWrite = 2;

        /** "Seek" capability. If set in getCapabilities(), this stream claims to support getPos(), setPos(), and getSize(). */
        static const uint32_t CanSeek = 4;

        /*
         *  Types
         */

        /** Type of mutable memory (for read). */
        typedef afl::base::Bytes_t Bytes_t;

        /** Type of constant memory (for write). */
        typedef afl::base::ConstBytes_t ConstBytes_t;

        /** Type of a file position or size. */
        typedef uint64_t FileSize_t;

        /*
         *  Functions
         */

        /** Read data.
            \param m Memory descriptor
            \return Number of bytes read */
        virtual size_t read(Bytes_t m) = 0;

        /** Write data.
            \param m Memory descriptor
            \return Number of bytes written */
        virtual size_t write(ConstBytes_t m) = 0;

        /** Flush.
            Makes sure that outstanding unwritten data is given to the operating system
            and still-undetected errors are generated.

            This method is intended to make the observed state match the operating system state;
            it is not intended to force operating system buffers out to disk ("fsync"). */
        virtual void flush() = 0;

        /** Set file position.
            \param pos New position */
        virtual void setPos(FileSize_t pos) = 0;

        /** Get file position.
            \return File position */
        virtual FileSize_t getPos() = 0;

        /** Get file size.
            \return File size in bytes */
        virtual FileSize_t getSize() = 0;

        /** Get capabilities.
            Note that a capability reported here doesn't absolutely guarantee that
            the operation will work if attempted.
            \return Capabilities (CanRead, CanWrite, CanSeek). */
        virtual uint32_t getCapabilities() = 0;

        /** Get name.
            \return Name of this file. */
        virtual String_t getName() = 0;

        /** Create a separate handle to this stream.
            This will create a handle with a separate read/write position.
            Multiple children can be created and each will be isolated from the others.
            Use this if another component requires "exclusive" access to the stream.

            The following restrictions apply:
            - after creating and giving out a child, you shouldn't use the original (master) stream anymore;
              that would interfere with whatever the children do.
              Instead, create a child stream for yourself.
            - the master stream must still be kept open until the last child has stopped using it.
              The behaviour is undefined if you close (=delete) it too early. */
        virtual afl::base::Ptr<Stream> createChild() = 0;

        /** Create a file mapping.
            The file mapping consists of all bytes from the current file position (getPos()) and includes up to \c limit bytes.
            The file position is advanced by \c limit bytes, as if read() had been called for that size.
            See FileMapping for details.

            This function should try hard to create a real, operating-system supported mapping.
            On error, this function returns null.

            If an application does not care whether it receives a real mapping or an emulated one,
            it should use createVirtualMapping().

            \param limit Size limit. Map at most this many bytes.
            \return New file mapping if possible, null if mapping could not be created */
        virtual afl::base::Ptr<FileMapping> createFileMapping(FileSize_t limit = FileSize_t(-1)) = 0;


        /*
         *  Derived Functions
         */

        /** Full read.
            Attempts to read all of #m, retrying partial reads if necessary.
            If not enough data can be read, throws a FileTooShortException.
            \param m Memory to read. */
        void fullRead(Bytes_t m);

        /** Full write.
            Attempts to write all of #m, retrying partial writes if necessary.
            If not enough data can be read, throws a FileProblemException.
            \param m Memory to write. */
        void fullWrite(ConstBytes_t m);

        /** Check for capabilities.
            \param which Capabilities to check (one or more of CanXxx).
            \return true if this stream supports all the specified capabilities */
        bool hasCapabilities(uint32_t which);

        /** Copy from another stream.
            Copies as many bytes as possible, i.e. until #other ends.
            \param other Stream to copy from */
        // FIXME: progress indicator?
        void copyFrom(Stream& other);

        /** Copy bytes from another stream.
            Copies precisely #size bytes from another stream.
            Throws a FileTooShortException if the source stream can not supply enough data.
            \param other Stream to copy from
            \param size Number of bytes to copy */
        void copyFrom(Stream& other, FileSize_t size);

        /** Create a virtual file mapping.
            The file mapping consists of all bytes from the current file position (getPos()) and includes up to \c limit bytes.
            The file position is advanced by \c limit bytes, as if read() had been called for that size.

            Creates a real, operating-system supported mapping if possible, an InternalFileMapping otherwise.

            \param stream Input stream. Object needs to be valid for the duration of this call, but no longer.
            \param limit Size limit. Map at most this many bytes.
            \return New file mapping, never null */
        afl::base::Ptr<FileMapping> createVirtualMapping(FileSize_t limit = FileSize_t(-1));


        // DataSink:
        virtual bool handleData(const String_t& name, afl::base::ConstBytes_t& data);
    };

} }

#endif
