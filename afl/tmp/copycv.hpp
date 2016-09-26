/**
  *  \file afl/tmp/copycv.hpp
  *  \brief Template class afl::tmp::CopyCV
  */
#ifndef AFL_AFL_TMP_COPYCV_HPP
#define AFL_AFL_TMP_COPYCV_HPP

namespace afl { namespace tmp {

    /** Copy const/volatile Qualifiers.
        Copies the c/v qualifiers of one type to another.
        For example, <tt>CopyCV<const int, long>::Type</tt> will be <tt>const long</tt>.
        \param T Type to copy qualifiers from
        \param U Type to copy qualifiers to */
    template<typename T, typename U>
    struct CopyCV {
        /** Type with copied qualifiers. */
        typedef U Type;
    };

    template<typename T, typename U>
    struct CopyCV<const T, U> {
        typedef const U Type;
    };

    template<typename T, typename U>
    struct CopyCV<volatile T, U> {
        typedef volatile U Type;
    };

    template<typename T, typename U>
    struct CopyCV<volatile const T, U> {
        typedef volatile const U Type;
    };

} }

#endif
