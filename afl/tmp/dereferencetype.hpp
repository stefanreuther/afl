/**
  *  \file afl/tmp/dereferencetype.hpp
  *  \brief Template class afl::tmp::DereferenceType
  */
#ifndef AFL_AFL_TMP_DEREFERENCETYPE_HPP
#define AFL_AFL_TMP_DEREFERENCETYPE_HPP

namespace afl { namespace tmp {

    /** Dereference a pointer type.
        Given a pointer type, returns its base.
        That is, <tt>DereferenceType<T*>::Type = T</tt>. */
    template<typename T>
    struct DereferenceType { };

    template<typename T>
    struct DereferenceType<T*> {
        typedef T Type;
    };

    template<typename T>
    struct DereferenceType<T* const> {
        typedef T Type;
    };

} }

#endif
