/**
  *  \file afl/io/inflatetransform.cpp
  *  \brief Class afl::io::InflateTransform
  *
  *  Format of gzip stream:
  *    BYTE     magic 1, 0x1F
  *    BYTE     magic 2, 0x8B
  *    BYTE     compression method, 8
  *    BYTE     flag
  *              bit 0    ASCII (not binary)
  *              bit 1    header CRC present
  *              bit 2    extra data present
  *              bit 3    file name present
  *              bit 4    comment present
  *    DWORD    mtime
  *    BYTE     flag
  *    BYTE     OS
  *    --- if flag bit 2 set ---
  *    WORD     length
  *    BYTE[]   data
  *    --- if flag bit 3 set ---
  *    BYTE[]   filename, 0-terminated
  *    --- if flag bit 4 set ---
  *    BYTE[]   comment, 0-terminated
  *    --- if flag bit 1 set ---
  *    WORD     CRC
  *    --- always ---
  *    BYTE[]   compressed stream
  *    DWORD    CRC32
  *    DWORD    size
  *  CRC and size are NOT verified.
  */

#include <cassert>
#include "afl/io/inflatetransform.hpp"
#include "afl/base/staticassert.hpp"
#include "afl/config.h"

#ifdef HAVE_ZLIB
/*
 *  ZLIB is available - real version
 */
# include <zlib.h>
# include "afl/except/invaliddataexception.hpp"
# include "afl/string/messages.hpp"
namespace {
    const uint8_t flASCII     = 1;
    const uint8_t flHeaderCRC = 2;
    const uint8_t flExtraData = 4;
    const uint8_t flFileName  = 8;
    const uint8_t flComment   = 16;
    const uint8_t flAll       = flASCII + flHeaderCRC + flExtraData + flFileName + flComment;


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

class afl::io::InflateTransform::Impl {
 public:
    Impl(Personality pers);
    ~Impl();
    void transform(afl::base::ConstBytes_t& in, afl::base::Bytes_t& out);
    static bool isAvailable();

 private:
    enum State {
        CheckHeader,
        CheckExtraDataLength,
        CheckExtraData,
        CheckFileName,
        CheckComment,
        CheckHeaderCRC,
        FinishedHeader,
        Decompressing,
        CheckingTrailer,
        EndReached
    };
    const Personality m_personality;
    State m_state;

    uint8_t m_buffer[16];
    size_t  m_size;

    uint8_t  m_gzHeaderFlags;
    uint16_t m_gzExtraDataLength;

    z_stream m_zStream;

    void reportError(int code);
};

inline
afl::io::InflateTransform::Impl::Impl(Personality pers)
    : m_personality(pers),
      m_state(pers == Gzip ? CheckHeader : FinishedHeader),
      m_size(0),
      m_gzHeaderFlags(0),
      m_gzExtraDataLength(0),
      m_zStream()
{ }

afl::io::InflateTransform::Impl::~Impl()
{
    switch (m_state) {
     case CheckHeader:
     case CheckExtraDataLength:
     case CheckExtraData:
     case CheckFileName:
     case CheckComment:
     case CheckHeaderCRC:
     case FinishedHeader:
        break;
     case Decompressing:
     case CheckingTrailer:
     case EndReached:
        inflateEnd(&m_zStream);
        break;
    }
}

inline void
afl::io::InflateTransform::Impl::transform(afl::base::ConstBytes_t& in, afl::base::Bytes_t& out)
{
    int result;
    size_t outIndex = 0;
    while (m_state != EndReached && !in.empty() && outIndex < out.size()) {
        switch (m_state) {
         case CheckHeader:
            // Check to read 10 bytes
            static_assert(sizeof(m_buffer) >= 10, "header buffer");
            while (m_size < 10 && !in.empty()) {
                m_buffer[m_size++] = *in.eat();
            }

            // Try to proceed to next state
            if (m_size >= 10) {
                if (m_buffer[0] != 0x1F
                    || m_buffer[1] != 0x8B
                    || m_buffer[2] != 8
                    || (m_buffer[3] & ~flAll) != 0)
                {
                    throw afl::except::InvalidDataException(afl::string::Messages::invalidFileHeader());
                }
                m_gzHeaderFlags = m_buffer[3];
                m_state = CheckExtraDataLength;
                m_size = 0;
            }
            break;

         case CheckExtraDataLength:
            // Do we need this?
            if ((m_gzHeaderFlags & flExtraData) != 0) {
                // Must have two bytes
                static_assert(sizeof(m_buffer) >= 2, "data length buffer");
                while (m_size < 2 && !in.empty()) {
                    m_buffer[m_size++] = *in.eat();
                }

                if (m_size >= 2) {
                    m_gzExtraDataLength = uint16_t(256*m_buffer[1] + m_buffer[0]);
                    m_state = CheckExtraData;
                    m_size = 0;
                }
            } else {
                // No extra data
                m_state = CheckFileName;
            }
            break;

         case CheckExtraData:
            // Skip extra data
            in.splitUpdate(m_gzExtraDataLength);
            if (m_gzExtraDataLength == 0) {
                m_state = CheckFileName;
            }
            break;

         case CheckFileName:
            // Do we need this?
            if ((m_gzHeaderFlags & flFileName) != 0) {
                // Yes, file name present; skip until null byte
                while (const uint8_t* pc = in.eat()) {
                    if (*pc == '\0') {
                        m_state = CheckComment;
                        break;
                    }
                }
            } else {
                // No file name
                m_state = CheckComment;
            }
            break;

         case CheckComment:
            // Do we need this?
            if ((m_gzHeaderFlags & flComment) != 0) {
                // Yes, comment present; skip until null byte
                while (const uint8_t* pc = in.eat()) {
                    if (*pc == '\0') {
                        m_state = CheckHeaderCRC;
                        break;
                    }
                }
            } else {
                // No file name
                m_state = CheckHeaderCRC;
            }
            break;

         case CheckHeaderCRC:
            // Do we need this?
            if ((m_gzHeaderFlags & flHeaderCRC) != 0) {
                // Must have two bytes
                m_size += in.split(2 - m_size).size();
                if (m_size >= 2) {
                    m_size = 0;
                    m_state = FinishedHeader;
                }
            } else {
                m_state = FinishedHeader;
            }
            break;

         case FinishedHeader:
            // Final state for gzip header parsing, initial state for raw inflate:
            // initialize zlib
            result = inflateInit2(&m_zStream, m_personality == Zlib ? 15 : -15);
            m_state = Decompressing;
            if (result != Z_OK) {
                reportError(result);
            }
            break;

         case Decompressing:
         {
            // Feed all data into zlib.
            m_zStream.next_in = const_cast<Bytef*>(in.unsafeData());
            m_zStream.avail_in = convertSize(in.size());

            afl::base::Bytes_t remainingOutput = out.subrange(outIndex);
            m_zStream.next_out = remainingOutput.unsafeData();
            m_zStream.avail_out = convertSize(remainingOutput.size());

            result = inflate(&m_zStream, 0);

            // Update buffer pointers
            outIndex += remainingOutput.size() - m_zStream.avail_out;
            in.split(in.size() - m_zStream.avail_in);

            // Process zlib return code
            if (result == Z_STREAM_END) {
                // end of stream signalled by stream
                m_state = CheckingTrailer;
            } else if (result == Z_BUF_ERROR || m_zStream.avail_in == 0) {
                // buffer error, means: our input buffer got empty
            } else if (result == Z_OK) {
                // OK
            } else {
                // Actual error
                m_state = EndReached;
                reportError(result);
            }
            break;
         }

         case CheckingTrailer:
            if (m_personality == Gzip) {
                // Ends with 8 bytes (CRC, Length)
                m_size += in.split(8 - m_size).size();
                if (m_size >= 8) {
                    m_size = 0;
                    m_state = EndReached;
                }
            } else {
                m_state = EndReached;
            }
            break;

         case EndReached:
            break;
        }
    }
    out.trim(outIndex);
}

inline bool
afl::io::InflateTransform::Impl::isAvailable()
{
    return true;
}

void
afl::io::InflateTransform::Impl::reportError(int code)
{
    String_t message = afl::string::Messages::invalidCompressedData();
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
class afl::io::InflateTransform::Impl {
 public:
    Impl(Personality /*pers*/)
        { throw afl::except::UnsupportedException("inflate"); }

    void transform(afl::base::ConstBytes_t& /*in*/, afl::base::Bytes_t& out)
        { out.reset(); }

    static bool isAvailable()
        { return false; }
};
#endif

afl::io::InflateTransform::InflateTransform(Personality personality)
    : m_pImpl(new Impl(personality))
{ }

afl::io::InflateTransform::~InflateTransform()
{ }

void
afl::io::InflateTransform::transform(afl::base::ConstBytes_t& in, afl::base::Bytes_t& out)
{
    return m_pImpl->transform(in, out);
}

void
afl::io::InflateTransform::flush()
{ }

bool
afl::io::InflateTransform::isAvailable()
{
    return Impl::isAvailable();
}
