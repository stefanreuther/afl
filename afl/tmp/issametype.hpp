/**
  *  \file afl/tmp/issametype.hpp
  *  \brief Template class afl::tmp::IsSameType
  */
#ifndef AFL_AFL_TMP_ISSAMETYPE_HPP
#define AFL_AFL_TMP_ISSAMETYPE_HPP

namespace afl { namespace tmp {

    /** Compare types.
        \param A First type
        \param B Second type */
    template<typename A, typename B>
    struct IsSameType {
        /** Result.
            true iff types A and B are the same. */
        static const bool result = false;
    };

    template<typename A>
    struct IsSameType<A,A> {
        static const bool result = true;
    };

} }

#endif
