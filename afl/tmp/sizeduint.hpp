/**
  *  \file afl/tmp/sizeduint.hpp
  *  \brief Template class afl::tmp::SizedUInt
  */
#ifndef AFL_AFL_TMP_SIZEDUINT_HPP
#define AFL_AFL_TMP_SIZEDUINT_HPP

#include "afl/tmp/ifthenelse.hpp"
#include "afl/base/types.hpp"

namespace afl { namespace tmp {

    /** Integer with a fixed byte size.
        Given an integer, returns one of uint8_t, uint16_t, uint32_t, or uint64_t,
        if the size matches the type. Otherwise, returns a default.
    
        \param N desired size.
        \param Default default value to return if none matches. */
    template<size_t N, typename Default = void>
    struct SizedUInt {
        typedef typename IfThenElse<N==sizeof(uint8_t), uint8_t,
            typename IfThenElse<N==sizeof(uint16_t), uint16_t,
                typename IfThenElse<N==sizeof(uint32_t), uint32_t,
                    typename IfThenElse<N==sizeof(uint64_t), uint64_t, Default>::Type
                >::Type
            >::Type
        >::Type Type;
    };

} }

#endif
