/**
  *  \file afl/async/receiveoperation.hpp
  *  \brief Class afl::async::ReceiveOperation
  */
#ifndef AFL_AFL_ASYNC_RECEIVEOPERATION_HPP
#define AFL_AFL_ASYNC_RECEIVEOPERATION_HPP

#include "afl/base/memory.hpp"
#include "afl/base/types.hpp"
#include "afl/async/operation.hpp"

namespace afl { namespace async {

    class SendOperation;

    /** Descriptor for a receive operation with data. */
    class ReceiveOperation : public Operation {
     public:
        /** Constructor.
            \param data Buffer for data to receive */
        ReceiveOperation(afl::base::Bytes_t data);

        /** Constructor.
            Does not define space for data to receive; use setData(). */
        ReceiveOperation();

        /** Set data buffer.
            Calling this resets the object back to the state as if it had been constructed giving #data as parameter.
            \param data Buffer for data to receive */
        void setData(afl::base::Bytes_t data);

        /** Get number of bytes received so far.
            \return Number of bytes received */
        size_t getNumReceivedBytes() const;

        /** Get bytes received so far.
            \return Buffer descriptor for all bytes received so far; a subrange of the buffer given to the constructor. */
        afl::base::ConstBytes_t getReceivedBytes() const;

        /** Check for completion.
            \retval true The complete buffer has been filled with data
            \retval false The buffer is not yet full */
        bool isCompleted() const;

        /** Copy data from a SendOperation.
            Copies as much as possible data from the SendOperation's send buffer into this object's receive buffer,
            considering the bytes sent on the send side, and received on this side.
            \param other [in/out] Send operation */
        void copyFrom(SendOperation& other);

        /** Get buffer space that has not been received yet.
            \return Descriptor to buffer space data that doesn't yet contain data */
        afl::base::Bytes_t getUnreceivedBytes() const;

        /** Mark some bytes received.
            Caller must have initialized these bytes already.
            \param n Number of bytes to consider received */
        void addReceivedBytes(size_t n);

     private:
        afl::base::Bytes_t m_data;
        size_t m_numReceivedBytes;
    };

} }

inline
afl::async::ReceiveOperation::ReceiveOperation(afl::base::Bytes_t data)
    : m_data(data),
      m_numReceivedBytes(0)
{ }

inline
afl::async::ReceiveOperation::ReceiveOperation()
    : m_data(),
      m_numReceivedBytes(0)
{ }

inline void
afl::async::ReceiveOperation::setData(afl::base::Bytes_t data)
{
    m_data = data;
    m_numReceivedBytes = 0;
}

inline size_t
afl::async::ReceiveOperation::getNumReceivedBytes() const
{
    return m_numReceivedBytes;
}

inline afl::base::ConstBytes_t
afl::async::ReceiveOperation::getReceivedBytes() const
{
    return m_data.subrange(0, m_numReceivedBytes);
}

inline bool
afl::async::ReceiveOperation::isCompleted() const
{
    return m_numReceivedBytes >= m_data.size();
}

inline afl::base::Bytes_t
afl::async::ReceiveOperation::getUnreceivedBytes() const
{
    return m_data.subrange(m_numReceivedBytes);
}

inline void
afl::async::ReceiveOperation::addReceivedBytes(size_t n)
{
    m_numReceivedBytes += n;
}

#endif
