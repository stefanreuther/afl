/**
  *  \file afl/async/sendoperation.hpp
  *  \brief Class afl::async::SendOperation
  */
#ifndef AFL_AFL_ASYNC_SENDOPERATION_HPP
#define AFL_AFL_ASYNC_SENDOPERATION_HPP

#include "afl/base/memory.hpp"
#include "afl/base/types.hpp"
#include "afl/async/operation.hpp"

namespace afl { namespace async {

    /** Descriptor for a send operation with data. */
    class SendOperation : public Operation {
     public:
        /** Constructor.
            Makes a SendOperation for the given data.
            \param data Data to send */
        SendOperation(afl::base::ConstBytes_t data);

        /** Constructor.
            Makes a SendOperation that sends no data. */
        SendOperation();

        /** Set data.
            Calling this resets the object back to the state as if it had been constructed giving \c data as parameter.
            \param data Data to send */
        void setData(afl::base::ConstBytes_t data);

        /** Get number of bytes that have already been sent.
            \return Number of bytes that have been sent */
        size_t getNumSentBytes() const;

        /** Check for completion.
            \retval true All data has been sent
            \retval false There is some unsent data */
        bool isCompleted() const;

        /** Get data that has not been sent yet.
            \return Descriptor to data that has not yet been sent */
        afl::base::ConstBytes_t getUnsentBytes() const;

        /** Mark some bytes sent.
            For use by data transfer implementations.
            \param n Number of additional bytes to consider sent */
        void addSentBytes(size_t n);

     private:
        afl::base::ConstBytes_t m_data;
        size_t m_numSentBytes;
    };

} }

/***************************** Implementation ****************************/

inline
afl::async::SendOperation::SendOperation(afl::base::ConstBytes_t data)
    : m_data(data),
      m_numSentBytes(0)
{ }

inline
afl::async::SendOperation::SendOperation()
    : m_data(),
      m_numSentBytes(0)
{ }

inline void
afl::async::SendOperation::setData(afl::base::ConstBytes_t data)
{
    m_data = data;
    m_numSentBytes = 0;
}

inline size_t
afl::async::SendOperation::getNumSentBytes() const
{
    return m_numSentBytes;
}

inline bool
afl::async::SendOperation::isCompleted() const
{
    return m_numSentBytes >= m_data.size();
}

inline afl::base::ConstBytes_t
afl::async::SendOperation::getUnsentBytes() const
{
    return m_data.subrange(m_numSentBytes);
}

inline void
afl::async::SendOperation::addSentBytes(size_t n)
{
    m_numSentBytes += n;
}

#endif
