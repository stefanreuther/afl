/**
  *  \file afl/charset/charsetfactory.hpp
  *  \brief Interface afl::charset::CharsetFactory
  */
#ifndef AFL_AFL_CHARSET_CHARSETFACTORY_HPP
#define AFL_AFL_CHARSET_CHARSETFACTORY_HPP

#include "afl/base/deletable.hpp"
#include "afl/base/deleter.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace charset {

    class Charset;

    /** Character set factory.
        This interface allows a user to create Charset instances according to application-defined rules. */
    class CharsetFactory : public afl::base::Deletable {
     public:
        /** Create Charset instance.
            \param deleter Deleter to control lifetime of created instances
            \param name Name of character set to create
            \return Charset instance if any, null if no matching charset known.
            The lifetime of the created Charset instance must be at least equal to the lifetime of the Deleter
            (i.e. it must be allocated in the Deleter, or statically allocated). */
        virtual Charset* createCharset(afl::base::Deleter& deleter, String_t name) = 0;
    };

} }

#endif
