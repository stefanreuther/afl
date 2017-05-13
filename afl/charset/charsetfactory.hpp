/**
  *  \file afl/charset/charsetfactory.hpp
  *  \brief Interface afl::charset::CharsetFactory
  */
#ifndef AFL_AFL_CHARSET_CHARSETFACTORY_HPP
#define AFL_AFL_CHARSET_CHARSETFACTORY_HPP

#include "afl/base/deletable.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace charset {

    class Charset;

    /** Character set factory.
        This interface allows a user to create Charset instances according to application-defined rules. */
    class CharsetFactory : public afl::base::Deletable {
     public:
        /** Create Charset instance.
            \param name Name of character set to create
            \return Newly-allocated charset instance if any, null if no matching charset known. */
        virtual Charset* createCharset(String_t name) = 0;
    };

} }

#endif
