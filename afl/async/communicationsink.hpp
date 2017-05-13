/**
  *  \file afl/async/communicationsink.hpp
  *  \brief Class afl::async::CommunicationSink
  */
#ifndef AFL_AFL_ASYNC_COMMUNICATIONSINK_HPP
#define AFL_AFL_ASYNC_COMMUNICATIONSINK_HPP

#include "afl/io/datasink.hpp"
#include "afl/base/ptr.hpp"
#include "afl/async/communicationobject.hpp"
#include "afl/sys/types.hpp"

namespace afl { namespace async {

    class Controller;

    /** Communication Sink.
        Adapts a CommunicationObject (possibly asynchronous send/receive using a Controller)
        to the Sink interface (synchronous sequentual write).
        handleData() calls translate into send() calls, possibly repeated to ensure full writes.

        A CommunicationSink references a Controller, therefore it can only be used from one thread.

        A CommunicationSink can be used with a timeout.
        The timeout is not enforced to the last syllable,
        but presence of a timeout means that a handleData() can time out and will not hang around forever. */
    class CommunicationSink : public afl::io::DataSink {
     public:
        /** Constructor.
            \param ctl Controller to associate with this instance
            \param obj Asynchronous communication object to use */
        CommunicationSink(Controller& ctl, afl::base::Ptr<CommunicationObject> obj);

        /** Destructor. */
        ~CommunicationSink();

        /** Define a timeout.
            \param t Timeout in milliseconds. Default is no timeout (infinity). */
        void setTimeout(afl::sys::Timeout_t t);

        // DataSink methods:
        virtual bool handleData(afl::base::ConstBytes_t& data);

     private:
        Controller& m_controller;
        afl::base::Ptr<CommunicationObject> m_pObj;
        afl::sys::Timeout_t m_timeout;
    };

} }

#endif
