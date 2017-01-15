/**
  *  \file afl/io/deflatetransform.cpp
  *  \brief Class afl::io::DeflateTransform
  */

#include <cassert>
#include "afl/io/deflatetransform.hpp"
#include "afl/base/staticassert.hpp"
#include "afl/config.h"
#include "afl/checksums/crc32.hpp"

#ifdef HAVE_ZLIB
/*
 *  ZLIB is available - real version
 */
# include <zlib.h>
# include "afl/except/invaliddataexception.hpp"
# include "afl/string/messages.hpp"
# include "afl/bits/value.hpp"
# include "afl/bits/uint32le.hpp"
namespace {
    static const uint8_t DEFAULT_GZIP_HEADER[] = {
        0x1F, 0x8B, // magic
        8,          // compression method
        0,          // flags
        0, 0, 0, 0, // mtime
        0,          // extra flags
        255         // OS
    };

    inline unsigned int convertSize(size_t sz)
    {
        static const unsigned int maxValue = ~0U;
        if (sz > maxValue) {
            return maxValue;
        } else {
            return static_cast<unsigned int>(sz);
        }
    }
}

class afl::io::DeflateTransform::Impl {
    struct GzipTrailer {
        afl::bits::Value<afl::bits::UInt32LE> crc32;
        afl::bits::Value<afl::bits::UInt32LE> bytes;
    };
 public:
    Impl(Personality pers);
    void transform(afl::base::ConstBytes_t& in, afl::base::Bytes_t& out);
    void flush();
    static bool isAvailable();

 private:
    enum State {
        PrepareHeader,
        WriteHeader,
        PrepareCompression,
        Compressing,
        PrepareTrailer,
        WriteTrailer,
        EndReached
    };
    const Personality m_personality;
    State m_state;
    bool m_flushing;

    afl::base::ConstBytes_t m_pending;
    GzipTrailer m_trailer;

    uint32_t m_gzipSize;
    uint32_t m_gzipCRC;

    z_stream m_zStream;

    void reportError(int code);
};

inline
afl::io::DeflateTransform::Impl::Impl(Personality pers)
    : m_personality(pers),
      m_state(PrepareHeader),
      m_flushing(false),
      m_pending(),
      m_trailer(),
      m_gzipSize(0),
      m_gzipCRC(0),
      m_zStream()
{ }

inline void
afl::io::DeflateTransform::Impl::transform(afl::base::ConstBytes_t& in, afl::base::Bytes_t& out)
{
    size_t outIndex = 0;

    if (m_state == PrepareHeader) {
        m_pending.reset();
        switch (m_personality) {
         case Gzip:
            m_pending = DEFAULT_GZIP_HEADER;
            break;

         case Raw:
         case Zlib:
            break;
        }
        m_state = WriteHeader;
    }

    if (m_state == WriteHeader) {
        size_t did = out.subrange(outIndex).copyFrom(m_pending).size();
        m_pending.split(did);
        outIndex += did;
        if (m_pending.empty()) {
            m_state = PrepareCompression;
        }
    }

    if (m_state == PrepareCompression) {
        // Final state for gzip header, initial state for raw deflate: initialize zlib
        int windowBits = (m_personality == Zlib ? 15 : -15);
        int result = deflateInit2(&m_zStream,
                                  /* level: */ Z_DEFAULT_COMPRESSION,
                                  /* method: */ Z_DEFLATED,
                                  /* windowBits (negative to turn off zlib header): */ windowBits,
                                  /* memLevel: */ 8,
                                  /* strategy: */ Z_DEFAULT_STRATEGY);
        m_state = Compressing;
        if (result != Z_OK) {
            reportError(result);
        }
    }

    while (m_state == Compressing) {
        // Feed all data into zlib.
        m_zStream.next_in  = const_cast<Bytef*>(in.unsafeData());
        m_zStream.avail_in = convertSize(in.size());

        afl::base::Bytes_t remainingOutput = out.subrange(outIndex);
        m_zStream.next_out  = remainingOutput.unsafeData();
        m_zStream.avail_out = convertSize(remainingOutput.size());

        int result = deflate(&m_zStream, (m_flushing ? Z_FINISH : Z_NO_FLUSH));

        // Update buffer pointers
        size_t numBytesWritten = remainingOutput.size() - m_zStream.avail_out;
        size_t numBytesRead    = in.size()              - m_zStream.avail_in;

        outIndex += numBytesWritten;
        afl::base::ConstBytes_t bytesRead = in.split(numBytesRead);

        // Update gzip variables (m_gzipSize is defined as being mod 2**32, overflow is ok)
        if (m_personality == Gzip) {
            m_gzipSize += uint32_t(numBytesRead);
            m_gzipCRC = afl::checksums::CRC32::getDefaultInstance().add(bytesRead, m_gzipCRC);
        }

        // Process zlib return code
        if (result == Z_STREAM_END) {
            // end of stream signalled by stream
            m_state = PrepareTrailer;
        } else if (result == Z_BUF_ERROR || outIndex >= out.size()) {
            // buffer error, means: our input buffer got empty
            break;
        } else if (result == Z_OK) {
            // OK
        } else {
            // Actual error
            m_state = EndReached;
            reportError(result);
        }
    }

    if (m_state == PrepareTrailer) {
        m_pending.reset();
        switch (m_personality) {
         case Gzip:
            m_trailer.bytes = m_gzipSize;
            m_trailer.crc32 = m_gzipCRC;
            m_pending = afl::base::fromObject(m_trailer);
         case Raw:
         case Zlib:
            break;
        }
        m_state = WriteTrailer;
    }

    if (m_state == WriteTrailer) {
        size_t did = out.subrange(outIndex).copyFrom(m_pending).size();
        m_pending.split(did);
        outIndex += did;
        if (m_pending.empty()) {
            m_state = EndReached;
        }
    }

    out.trim(outIndex);
}

inline void
afl::io::DeflateTransform::Impl::flush()
{
    m_flushing = true;
}

inline bool
afl::io::DeflateTransform::Impl::isAvailable()
{
    return true;
}

void
afl::io::DeflateTransform::Impl::reportError(int code)
{
    String_t message = afl::string::Messages::unexpectedError();
    message += ": ";
    message += zError(code);
    if (m_zStream.msg != 0) {
        message += ", ";
        message += m_zStream.msg;
    }
    throw afl::except::InvalidDataException(message);
}
#else
/*
 *  No ZLIB available - dummy version
 */
# include "afl/except/unsupportedexception.hpp"
class afl::io::DeflateTransform::Impl {
 public:
    Impl(Personality /*pers*/)
        { throw afl::except::UnsupportedException("deflate"); }

    void transform(afl::base::ConstBytes_t& /*in*/, afl::base::Bytes_t& out)
        { out.reset(); }

    void flush()
        { }

    static bool isAvailable()
        { return false; }
};
#endif

afl::io::DeflateTransform::DeflateTransform(Personality personality)
    : m_pImpl(new Impl(personality))
{ }

afl::io::DeflateTransform::~DeflateTransform()
{ }

void
afl::io::DeflateTransform::transform(afl::base::ConstBytes_t& in, afl::base::Bytes_t& out)
{
    return m_pImpl->transform(in, out);
}

void
afl::io::DeflateTransform::flush()
{
    m_pImpl->flush();
}

bool
afl::io::DeflateTransform::isAvailable()
{
    return Impl::isAvailable();
}
