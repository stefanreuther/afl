/**
  *  \file afl/functional/unaryfunction.hpp
  *  \brief Template class afl::functional::UnaryFunction
  */
#ifndef AFL_AFL_FUNCTIONAL_UNARYFUNCTION_HPP
#define AFL_AFL_FUNCTIONAL_UNARYFUNCTION_HPP

#include "afl/base/optional.hpp"
#include "afl/base/deletable.hpp"

namespace afl { namespace functional {

    /** Unary function.
        Base class for functions that map one value to another.

        \param A input (parameter, domain) of the function
        \param B output (result, codomain) of the function */
    template<typename A, typename B>
    class UnaryFunction : public afl::base::Deletable {
     public:
        /** Implementation of the function.
            \param a input
            \return result */
        virtual B get(A a) const = 0;

        /** Invoke with element. */
        B operator()(A a) const;

        /** Invoke with optional element. */
        afl::base::Optional<B> operator()(afl::base::Optional<A> a) const;

        template<typename C>
        class ComposedFunction;

        /** Compose functions.
            This maps/transforms the output of this function with another function.
            The result is another unary function.
            If <tt>c = a.map(b)</tt>, <tt>c(x) = b(a(x))</tt>.

            \tparam C final result type
            \param function transformation function
            \return composed function */
        template<typename C>
        ComposedFunction<C> map(const UnaryFunction<B,C>& function) const;
    };

} }

template<typename A, typename B>
template<typename C>
class afl::functional::UnaryFunction<A,B>::ComposedFunction : public afl::functional::UnaryFunction<A,C> {
 public:
    ComposedFunction(const UnaryFunction<A,B>& inner, const UnaryFunction<B,C>& outer)
        : m_inner(inner),
          m_outer(outer)
        { }

    virtual C get(A a) const
        { return m_outer(m_inner(a)); }

 private:
    const UnaryFunction<A,B>& m_inner;
    const UnaryFunction<B,C>& m_outer;
};

template<typename A, typename B>
template<typename C>
inline afl::functional::UnaryFunction<A,B>::ComposedFunction<C>
afl::functional::UnaryFunction<A,B>::map(const UnaryFunction<B,C>& function) const
{
    return ComposedFunction<C>(*this, function);
}

// Invoke with element.
template<typename A, typename B>
inline B
afl::functional::UnaryFunction<A,B>::operator()(A a) const
{
    return this->get(a);
}

// Invoke with optional element.
template<typename A, typename B>
inline afl::base::Optional<B>
afl::functional::UnaryFunction<A,B>::operator()(afl::base::Optional<A> a) const
{
    if (const A* p = a.get()) {
        return this->get(*p);
    } else {
        return afl::base::Optional<B>();
    }
}

#endif
