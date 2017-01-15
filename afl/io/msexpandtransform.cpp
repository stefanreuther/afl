/**
  *  \file afl/io/msexpandtransform.cpp
  *  \brief Class afl::io::MsExpandTransform
  */

#include "afl/io/msexpandtransform.hpp"
#include "afl/base/staticassert.hpp"
#include "afl/except/invaliddataexception.hpp"
#include "afl/string/messages.hpp"

namespace {
    const uint8_t HEADER[] = {
        'S','Z','D','D',        // well-known signature
        0x88,0xF0,0x27,0x33,    // not-so-well-known signature
        'A'                     // compression mode, only 'A' allowed
    };
}

/** Advance a window pointer.
    \param p Pointer */
inline size_t
afl::io::MsExpandTransform::next(size_t p)
{
    return (p + 1U) % WINDOW_SIZE;
}


// Default constructor.
afl::io::MsExpandTransform::MsExpandTransform()
    : Transform(),
      m_windowPos(WINDOW_SIZE - 16),
      m_state(Header),
      m_control(0),
      m_mask(0),
      m_count(0),
      m_pos(0),
      m_data(0)
{
    afl::base::Bytes_t(m_window).fill(' ');
}

// Destructor.
afl::io::MsExpandTransform::~MsExpandTransform()
{ }

// Transformation.
void
afl::io::MsExpandTransform::transform(afl::base::ConstBytes_t& in, afl::base::Bytes_t& out)
{
    /*
     *  In each state, we either advance the input pointer, or the output pointer.
     *  If we cannot advance the pointer, we stop the loop.
     */
    const uint8_t* p;
    uint8_t* q;
    bool running = true;
    while (running) {
        switch (m_state) {
         case Header:
            // Copy header
            while (m_count < sizeof(m_header) && (p = in.eat()) != 0) {
                m_header[m_count++] = *p;
            }

            // If header got completed, check it
            if (m_count >= sizeof(m_header)) {
                static_assert(sizeof(m_header) >= sizeof(HEADER), "header size");
                if (std::memcmp(m_header, HEADER, sizeof(HEADER)) != 0) {
                    throw afl::except::InvalidDataException(afl::string::Messages::invalidFileHeader());
                }

                // Next step
                m_state = Control;
            } else {
                // Unable to complete this step which means input is exhausted. Break.
                running = false;
            }
            break;

         case Control:
            if ((p = in.eat()) != 0) {
                // OK
                m_control = *p;
                m_mask = 1;
                m_state = ReadData;
            } else {
                // Unable to complete this step which means input is exhausted. Break.
                running = false;
            }
            break;

         case ReadData:
            if (m_mask == 0) {
                // Control byte completely processed. Get another.
                m_state = Control;
            } else if ((p = in.eat()) != 0) {
                // OK, got a byte.
                m_data = *p;
                if ((m_control & m_mask) != 0) {
                    // Literal
                    m_state = WriteLiteral;
                } else {
                    // Backreference. Need to get another byte.
                    m_state = ReadLength;
                }
                m_mask = static_cast<uint8_t>(m_mask << 1);
            } else {
                // Unable to complete this step which means input is exhausted. Break.
                running = false;
            }
            break;

         case ReadLength:
            if ((p = in.eat()) != 0) {
                // Got a length byte. Prepare for reading a backreference.
                m_count = (*p & 15) + 3;
                m_pos = m_data | ((*p & 0xF0) << 4);
                m_state = WriteBackreference;
            } else {
                // Unable to complete this step which means input is exhausted. Break.
                running = false;
            }
            break;

         case WriteLiteral:
            if ((q = out.eat()) != 0) {
                // Got an output byte. Write value and proceed with next step.
                *q = m_window[m_windowPos] = m_data;
                m_windowPos = next(m_windowPos);
                m_state = ReadData;
            } else {
                // Unable to complete this step which means input is exhausted. Break.
                running = false;
            }
            break;

         case WriteBackreference:
            // Write output.
            while (m_count > 0 && (q = out.eat()) != 0) {
                *q = m_window[m_windowPos] = m_window[m_pos];
                m_windowPos = next(m_windowPos);
                m_pos = next(m_pos);
                --m_count;
            }
            if (m_count == 0) {
                // Processed whole backreference.
                m_state = ReadData;
            } else {
                // Unable to complete this step which means input is exhausted. Break.
                running = false;
            }
            break;
        }
    }
}

// Nothing to flush in this algorithm.
void
afl::io::MsExpandTransform::flush()
{ }
