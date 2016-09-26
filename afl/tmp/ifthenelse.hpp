/**
  *  \file afl/tmp/ifthenelse.hpp
  *  \brief Template class afl::tmp::IfThenElse
  */
#ifndef AFL_AFL_TMP_IFTHENELSE_HPP
#define AFL_AFL_TMP_IFTHENELSE_HPP

namespace afl { namespace tmp {

    /** Compile-time "if-then-else" type switch.
        \param cond Condition
        \param T    Type to return if condition is true
        \param F    Type to return if condition is false */
    template<bool Cond, typename T, typename F>
    struct IfThenElse {
        /** Result type. Same as \c T if \c cond is true, otherwise same as \c F. */
        typedef T Type;
    };

    template<typename T, typename F>
    struct IfThenElse<false,T,F> {
        typedef F Type;
    };

} }

#endif
