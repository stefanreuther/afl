/**
  *  \file afl/async/communicationstream.hpp
  *  \brief Class afl::async::CommunicationStream
  */
#ifndef AFL_AFL_ASYNC_COMMUNICATIONSTREAM_HPP
#define AFL_AFL_ASYNC_COMMUNICATIONSTREAM_HPP

#include "afl/async/communicationobject.hpp"
#include "afl/base/ptr.hpp"
#include "afl/io/stream.hpp"
#include "afl/sys/types.hpp"

namespace afl { namespace async {

    class Controller;

    /** Communication Stream.
        Adapts a CommunicationObject (possibly asynchronous send/receive using a Controller)
        to the Stream interface (synchronous write/read, standalone).
        Sequences of read() or write() calls translate into sequences of receive() and send(), respectively,
        where partial transfers are correctly re-assembled.

        A CommunicationStream references a Controller, therefore it can only be used from one thread.
        It is usually an error to even try to use a CommunicationStream from multiple threads,
        but if that is required, each thread must have its own instance.
        For the same reason, createChild() is not supported.

        A CommunicationStream can be used with a timeout.
        The timeout is not enforced to the last syllable,
        but presence of a timeout means that a read() or write() can time out and will not hang around forever. */
    class CommunicationStream : public afl::io::Stream {
     public:
        /** Constructor.
            \param ctl Controller to associate with this instance
            \param obj Asynchronous communication object to use */
        // FIXME: Ref
        CommunicationStream(Controller& ctl, afl::base::Ptr<CommunicationObject> obj);

        /** Destructor. */
        ~CommunicationStream();

        /** Define a timeout.
            \param t Timeout in milliseconds. Default is no timeout (infinity). */
        void setTimeout(afl::sys::Timeout_t t);

        // Stream methods:
        virtual size_t read(Bytes_t m);
        virtual size_t write(ConstBytes_t m);
        virtual void flush();
        virtual void setPos(FileSize_t pos);
        virtual FileSize_t getPos();
        virtual FileSize_t getSize();
        virtual uint32_t getCapabilities();
        virtual String_t getName();
        virtual afl::base::Ref<afl::io::Stream> createChild(uint32_t flags);
        virtual afl::base::Ptr<afl::io::FileMapping> createFileMapping(FileSize_t limit);

     private:
        Controller& m_controller;
        afl::base::Ptr<CommunicationObject> m_pObj;
        afl::sys::Timeout_t m_timeout;
    };

} }

#endif
