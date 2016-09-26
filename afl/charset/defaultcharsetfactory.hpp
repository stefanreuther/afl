/**
  *  \file afl/charset/defaultcharsetfactory.hpp
  *  \brief Class afl::charset::DefaultCharsetFactory
  */
#ifndef AFL_AFL_CHARSET_DEFAULTCHARSETFACTORY_HPP
#define AFL_AFL_CHARSET_DEFAULTCHARSETFACTORY_HPP

#include "afl/charset/charsetfactory.hpp"

namespace afl { namespace charset {

    /** Default character set factory.
        This implements CharsetFactory with support for all character sets supported by AFL. */
    class DefaultCharsetFactory : public CharsetFactory {
     public:
        /** Constructor. */
        DefaultCharsetFactory();

        /** Destructor. */
        ~DefaultCharsetFactory();

        // CharsetFactory:
        virtual Charset* createCharset(afl::base::Deleter& deleter, String_t name);
    };

} }

#endif
