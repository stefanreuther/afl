/**
  *  \file afl/tmp/stripcv.hpp
  *  \brief Template Class afl::tmp::StripCV
  */
#ifndef AFL_AFL_TMP_STRIPCV_HPP
#define AFL_AFL_TMP_STRIPCV_HPP

namespace afl { namespace tmp {

    /** Strip const/volatile Qualifiers.
        For example, <tt>StripCV<const int>::Type</tt> will be <tt>int</tt>.
        \param T Type to strip qualifiers from */
    template<typename T>
    struct StripCV {
        /** Type without qualifiers. */
        typedef T Type;
    };

    template<typename T>
    struct StripCV<const T> {
        typedef T Type;
    };

    template<typename T>
    struct StripCV<volatile T> {
        typedef T Type;
    };

    template<typename T>
    struct StripCV<volatile const T> {
        typedef T Type;
    };

} }

#endif
