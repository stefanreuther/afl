/**
  *  \file afl/io/limitedstream.hpp
  *  \brief Class afl::io::LimitedStream
  */
#ifndef AFL_AFL_IO_LIMITEDSTREAM_HPP
#define AFL_AFL_IO_LIMITEDSTREAM_HPP

#include "afl/io/stream.hpp"
#include "afl/base/ref.hpp"

namespace afl { namespace io {

    /** Limited stream.
        Provides a limited view onto another stream.

        The view is defined by an origin position and length.
        Both are optional and can be given as \c nil to disable them.

        \b Origin: If set, specifies the starting position within the parent stream.
        If not set, the parent stream's position is not used, and seeking will not be supported.

        \b Length: If set, specifies the size of the accessible window for seekable streams,
        the maximum number of bytes to read for non-seekable streams.
        If not set, the underlying stream can be read until its end. */
    class LimitedStream : public Stream {
     public:
        /** File position to signify "no limit/not present". */
        static const FileSize_t nil = FileSize_t(-1);

        /** Constructor.
            \param parent Underlying stream. It makes sense to use another stream's child (createChild) here.
            \param origin Origin position if desired; nil if no origin should be set.
            \param length Length if desired; nil if no length should be set */
        LimitedStream(afl::base::Ref<Stream> parent, FileSize_t origin, FileSize_t length);

        /** Destructor. */
        ~LimitedStream();

        // Stream:
        virtual size_t read(Bytes_t m);
        virtual size_t write(ConstBytes_t m);
        virtual void flush();
        virtual void setPos(FileSize_t pos);
        virtual FileSize_t getPos();
        virtual FileSize_t getSize();
        virtual uint32_t getCapabilities();
        virtual String_t getName();
        virtual afl::base::Ref<Stream> createChild(uint32_t flags);
        virtual afl::base::Ptr<FileMapping> createFileMapping(FileSize_t limit = FileSize_t(-1));

     private:
        /** Underlying stream. */
        afl::base::Ref<Stream> m_parent;

        /** Origin (absolute position in m_parent). */
        FileSize_t m_origin;

        /** Length. */
        FileSize_t m_length;

        /** Position (relative to m_origin, <= m_length). */
        FileSize_t m_position;

        bool hasPosition() const;
        bool hasLength() const;
        void fixupPosition(size_t n);
    };

} }

#endif
