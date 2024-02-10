/**
  *  \file afl/functional/mapping.hpp
  *  \brief Template class afl::functional::Mapping
  */
#ifndef AFL_AFL_FUNCTIONAL_MAPPING_HPP
#define AFL_AFL_FUNCTIONAL_MAPPING_HPP

#include "afl/functional/unaryfunction.hpp"
#include "afl/functional/binaryfunction.hpp"

namespace afl { namespace functional {

    /** Mapping.
        This extends UnaryFunction - functions that map one value to another - by functions to enumerate the domain (input) set.
        Common uses are iteration over an array or map; inputs are therefore also called <em>keys</em>.

        To iterate the result set, do
        <pre>
                A key;
                for (bool v = getFirstKey(key); v; v = getNextKey(key)) { ... }
        </pre>

        \param A input (parameter, domain) of the function
        \param B output (result, codomain) of the function */
    template<typename A, typename B>
    class Mapping : public UnaryFunction<A, B> {
     public:
        /** Get first key.
            \param a [out] Key
            \retval true The input set is nonempty, a key has been produced in \c a.
            \retval false The input set is empty, \c a has an unspecified value */
        virtual bool getFirstKey(A& a) const = 0;

        /** Get next key.
            \param a [in/out] Key.
            \retval true Another key has been produced in \c a.
            \retval false No more keys available, \c a has an unspecified value */
        virtual bool getNextKey(A& a) const = 0;

        /** Fold values.
            This iterates through all keys and combines them using a function, producing a single output.
            For example, if the mapping \c a contains values 1..5,
            <tt>a.fold(b, z)</tt> computes <tt>b(a(5), b(a(4), b(a(3), b(a(2), b(a(1), z)))))</tt>. */
        template<typename Acc>
        Acc fold(const BinaryFunction<Acc, B, Acc>& function, Acc value) const;

        template<typename C>
        class ComposedMapping;

        /** Compose mappings.
            This maps/transforms the output of this mapping with another function.
            The result is another mapping.
            If <tt>c = a.map(b)</tt>, <tt>c(x) = b(a(x))</tt>.

            This function is reimplemented from UnaryFunction to pass on the getFirstKey()/getNextKey() functions.

            \param C final result type
            \param function transformation function
            \return composed mapping */
        template<typename C>
        ComposedMapping<C> map(const UnaryFunction<B,C>& function) const;
    };

} }

template<typename A, typename B>
template<typename C>
class afl::functional::Mapping<A,B>::ComposedMapping : public Mapping<A,C> {
 public:
    ComposedMapping(const Mapping<A,B>& inner, const UnaryFunction<B,C>& outer)
        : m_inner(inner),
          m_outer(outer)
        { }

    virtual C get(A a) const
        { return m_outer(m_inner(a)); }

    virtual bool getFirstKey(A& a) const
        { return m_inner.getFirstKey(a); }
    virtual bool getNextKey(A& a) const
        { return m_inner.getNextKey(a); }

 private:
    const Mapping<A,B>& m_inner;
    const UnaryFunction<B,C>& m_outer;
};

template<typename A, typename B>
template<typename Acc>
Acc
afl::functional::Mapping<A,B>::fold(const BinaryFunction<Acc, B, Acc>& function, Acc value) const
{
    A key;
    for (bool v = getFirstKey(key); v; v = getNextKey(key)) {
        value = function(value, (*this)(key));
    }
    return value;
}

template<typename A, typename B>
template<typename C>
inline afl::functional::Mapping<A,B>::ComposedMapping<C>
afl::functional::Mapping<A,B>::map(const UnaryFunction<B,C>& function) const
{
    return ComposedMapping<C>(*this, function);
}

#endif
