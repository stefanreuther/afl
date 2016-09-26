/**
  *  \file afl/string/formatunion.hpp
  *  \brief Union afl::string::FormatUnion
  */
#ifndef AFL_AFL_STRING_FORMATUNION_HPP
#define AFL_AFL_STRING_FORMATUNION_HPP

#include "afl/base/types.hpp"

namespace afl { namespace string {

    /** Temporary storage for afl::string::Format.
        Provides storage for data of a varying type.

        This union is not used in user code;
        it is used through FormatTraits.

        \internal */
    union FormatUnion {
        /** Pointer value. */
        const void* m_ptr;

        /** Signed integer value. */
        intmax_t m_signed;

        /** Unsigned integer value. */
        uintmax_t m_unsigned;
    };

} }

#endif
