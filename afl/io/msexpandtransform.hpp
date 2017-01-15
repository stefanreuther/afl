/**
  *  \file afl/io/msexpandtransform.hpp
  *  \brief Class afl::io::MsExpandTransform
  */
#ifndef AFL_AFL_IO_MSEXPANDTRANSFORM_HPP
#define AFL_AFL_IO_MSEXPANDTRANSFORM_HPP

#include "afl/io/transform.hpp"
#include "afl/base/memory.hpp"
#include "afl/base/types.hpp"

namespace afl { namespace io {

    /** Microsoft-compatible decompressor.
        This decompresses MS "compress.exe"/"expand.exe" ("SZDD") files.
        These files are compressed using a simple LZSS variant
        and typically have the last character of their file extension replaced by a "_".

        - 8 bytes fixed signature ("SZDD\x88\xF0\x27\x33")
        - 1 byte compression type ("A")
        - 1 byte last character of file extension (0=unknown/none)
        - 4 bytes original file size
        - compressed data stream, as a loop:
          - 1 byte control
          - 8x 1 byte literal, or 2 bytes backreference */
    class MsExpandTransform : public Transform {
     public:
        /** Default constructor. */
        MsExpandTransform();

        /** Destructor. */
        ~MsExpandTransform();

        // Transform:
        virtual void transform(afl::base::ConstBytes_t& in, afl::base::Bytes_t& out);
        virtual void flush();

     private:
        static const size_t WINDOW_SIZE = 4096;
        uint8_t m_window[WINDOW_SIZE];  ///< Backreference window.
        size_t m_windowPos;             ///< Current window position.

        enum State {                    ///< Decoder state.
            Header,                     ///< Expecting header
            Control,                    ///< Expecting control byte
            ReadData,                   ///< Expecting data (literal or backreference position)
            ReadLength,                 ///< Expecting data (backreference length)
            WriteLiteral,               ///< Writing literal data
            WriteBackreference          ///< Writing backreference
        };
        State m_state;                  ///< Decoder state.
        uint8_t m_header[14];           ///< [Header] Header
        uint8_t m_control;              ///< [ReadData,ReadLength,WriteLiteral,WriteBackreference] Current control byte
        uint8_t m_mask;                 ///< [ReadData,ReadLength,WriteLiteral,WriteBackreference] Mask for next iteration
        size_t m_count;                 ///< [Header] Number of bytes read, [Backreference] Number of bytes to write
        size_t m_pos;                   ///< [Backreference] Backreference position
        uint8_t m_data;                 ///< [WriteLiteral] Current literal

        static size_t next(size_t p);
    };

} }

#endif
