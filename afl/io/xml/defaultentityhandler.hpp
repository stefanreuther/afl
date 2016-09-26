/**
  *  \file afl/io/xml/defaultentityhandler.hpp
  *  \brief Class afl::io::xml::DefaultEntityHandler
  */
#ifndef AFL_AFL_IO_XML_DEFAULTENTITYHANDLER_HPP
#define AFL_AFL_IO_XML_DEFAULTENTITYHANDLER_HPP

#include "afl/io/xml/entityhandler.hpp"

namespace afl { namespace io { namespace xml {

    /** Default XML entity handler.
        Provides a default implementation of EntityHandler, including a default instance.

        Multithreading: this class has no internal state and can thus be used by many users in parallel.
        In particular, many users can use the default instance (getInstance()) without restrictions. */
    class DefaultEntityHandler : public EntityHandler {
     public:
        // EntityHandler method:
        virtual String_t expandEntityReference(String_t name);

        /** Get instance.
            \return statically allocated instance of DefaultEntityHandler. */
        static DefaultEntityHandler& getInstance();
    };

} } }

#endif
