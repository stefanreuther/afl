/**
  *  \file afl/net/acceptoperation.hpp
  *  \brief Class afl::net::AcceptOperation
  */
#ifndef AFL_AFL_NET_ACCEPTOPERATION_HPP
#define AFL_AFL_NET_ACCEPTOPERATION_HPP

#include "afl/async/operation.hpp"
#include "afl/base/ptr.hpp"

namespace afl { namespace net {

    class Socket;

    /** Descriptor for an asynchronous Accept operation.
        This class is used with afl::net::Listener::acceptAsync(). */
    class AcceptOperation : public afl::async::Operation {
     public:
        /** Constructor. Makes a null / idle object. */
        AcceptOperation();

        /** Get result.
            \return socket for accepted connection (what would be the result in a synchronous accept), not null. */
        const afl::base::Ptr<Socket>& getResult() const;

        /** Set result. This method is used by the acceptAsync() implementation.
            \param result newly-allocated socket; should not be null. */
        void setResult(afl::base::Ptr<Socket> result);

     private:
        /** Result. */
        afl::base::Ptr<Socket> m_result;
    };

} }

inline
afl::net::AcceptOperation::AcceptOperation()
    : afl::async::Operation(),
      m_result()
{ }

inline const afl::base::Ptr<afl::net::Socket>&
afl::net::AcceptOperation::getResult() const
{
    return m_result;
}

inline void
afl::net::AcceptOperation::setResult(afl::base::Ptr<Socket> result)
{
    m_result = result;
}

#endif
