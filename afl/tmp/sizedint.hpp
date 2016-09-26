/**
  *  \file afl/tmp/sizedint.hpp
  *  \brief Template class afl::tmp::SizedInt
  */
#ifndef AFL_AFL_TMP_SIZEDINT_HPP
#define AFL_AFL_TMP_SIZEDINT_HPP

#include "afl/tmp/ifthenelse.hpp"
#include "afl/base/types.hpp"

namespace afl { namespace tmp {

    /** Integer with a fixed byte size.
        Given an integer, returns one of int8_t, int16_t, int32_t, or int64_t,
        if the size matches the type. Otherwise, returns a default.
    
        \param N desired size.
        \param Default default value to return if none matches. */
    template<size_t N, typename Default = void>
    struct SizedInt {
        /** Result type. */
        typedef typename IfThenElse<N==sizeof(int8_t), int8_t,
            typename IfThenElse<N==sizeof(int16_t), int16_t,
                typename IfThenElse<N==sizeof(int32_t), int32_t,
                    typename IfThenElse<N==sizeof(int64_t), int64_t, Default>::Type
                >::Type
            >::Type
        >::Type Type;
    };

} }

#endif
