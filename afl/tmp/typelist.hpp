/**
  *  \file afl/tmp/typelist.hpp
  *  \brief Template Class afl::tmp::TypeList
  */
#ifndef AFL_AFL_TMP_TYPELIST_HPP
#define AFL_AFL_TMP_TYPELIST_HPP

namespace afl { namespace tmp {

    /** Type list.
        This can be used to construct a compile-time meta data structure.
        To represent a list of three types, use
        <code>
          TypeList<A, TypeList<B, TypeList<C, void> > >
        </code>.

        \param Car element type
        \param Cdr list tail; another TypeList or void. */
    template<typename Car, typename Cdr>
    struct TypeList { };


    /** Type list element test.
        Checks whether the type list contains the specified type.
        \param Type type to look for
        \param List a TypeList */
    template<typename Type, typename List>
    struct TypeListContains {
        /** Result of search. */
        static const bool result = false;
    };

    template<typename Type, typename Car, typename Cdr>
    struct TypeListContains<Type, TypeList<Car,Cdr> > {
        static const bool result = TypeListContains<Type,Cdr>::result;
    };

    template<typename Type, typename Cdr>
    struct TypeListContains<Type, TypeList<Type,Cdr> > {
        static const bool result = true;
    };

} }

#endif
