/**
  *  \file afl/io/multiplexablestream.hpp
  *  \brief Class afl::io::MultiplexableStream
  */
#ifndef AFL_AFL_IO_MULTIPLEXABLESTREAM_HPP
#define AFL_AFL_IO_MULTIPLEXABLESTREAM_HPP

#include "afl/io/stream.hpp"
#include "afl/base/ptr.hpp"

namespace afl { namespace io {

    /** Base class for streams without own createChild() method.
        This implements a createChild() method for streams that do not implement that themselves.
        It does so by keeping track of created children and switching back and forth between them.
        Accesses are serialized using a Mutex.

        If the master stream dies before the children,
        the children go into a zombie state where they do not react on any request (but do not crash). */
    class MultiplexableStream : public Stream {
     public:
        MultiplexableStream();

        ~MultiplexableStream();

        afl::base::Ref<Stream> createChild(uint32_t flags);

     private:
        class ControlNode;
        class Child;

        afl::base::Ref<ControlNode> m_controlNode;
    };

} }

#endif
