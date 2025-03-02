/**
  *  \file afl/functional/binaryfunction.hpp
  *  \brief Template class afl::functional::BinaryFunction
  */
#ifndef AFL_AFL_FUNCTIONAL_BINARYFUNCTION_HPP
#define AFL_AFL_FUNCTIONAL_BINARYFUNCTION_HPP

#include "afl/functional/unaryfunction.hpp"

namespace afl { namespace functional {

    /** Binary function.
        Base class for functions that map two value to one another.

        \param A1,A2 input (parameter, domain) of the function
        \param B output (result, codomain) of the function */
    template<typename A1, typename A2, typename B>
    class BinaryFunction {
     public:
        /** Implementation of the function.
            \param a1,a2 input
            \return result */
        virtual B get(A1 a1, A2 a2) const = 0;

        /** Invoke with elements. */
        B operator()(A1 a1, A2 a2) const;

        template<typename C>
        class ComposedFunction;

        /** Compose functions.
            This maps/transforms the output of this function with another function.
            The result is another binary function.
            If <tt>c = a.map(b)</tt>, <tt>c(x,y) = b(a(x,y))</tt>.

            \tparam C final result type
            \param function transformation function */
        template<typename C>
        ComposedFunction<C> map(const UnaryFunction<B,C>& function) const;
    };

} }

template<typename A1, typename A2, typename B>
template<typename C>
class afl::functional::BinaryFunction<A1,A2,B>::ComposedFunction : public BinaryFunction<A1,A2,C> {
 public:
    ComposedFunction(const BinaryFunction<A1,A2,B>& inner, const UnaryFunction<B,C>& outer)
        : m_inner(inner),
          m_outer(outer)
        { }

    virtual C get(A1 a1, A2 a2) const
        { return m_outer(m_inner(a1, a2)); }

 private:
    const BinaryFunction<A1,A2,B>& m_inner;
    const UnaryFunction<B,C>& m_outer;
};

template<typename A1, typename A2, typename B>
template<typename C>
inline typename afl::functional::BinaryFunction<A1,A2,B>::template ComposedFunction<C>
afl::functional::BinaryFunction<A1,A2,B>::map(const UnaryFunction<B,C>& function) const
{
    return ComposedFunction<C>(*this, function);
}

template<typename A1, typename A2, typename B>
inline B
afl::functional::BinaryFunction<A1,A2,B>::operator()(A1 a1, A2 a2) const
{
    return this->get(a1, a2);
}

#endif
