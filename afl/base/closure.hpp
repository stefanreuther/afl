/**
  *  \file afl/base/closure.hpp
  *  \brief Template class afl::base::Closure
  */
#ifndef AFL_AFL_BASE_CLOSURE_HPP
#define AFL_AFL_BASE_CLOSURE_HPP

#include "afl/base/deletable.hpp"

namespace afl { namespace base {

    template<typename Signature> class Closure;


    /*
     *  Nullary
     */

    /** Nullary closure.
        Provides an interface to store a static or bound nullary function. */
    template<typename T>
    class Closure<T()> : public Deletable {
     public:
        /** Invoke the referenced function. */
        virtual T call() = 0;

        /** Make a null closure.
            When invoked, it does nothing.
            \return the closure, never null */
        static Closure<T()>* makeNull();

        /** Make a static-function closure.
            When invoked, calls the function.
            \param func function
            \return the closure, never null */
        static Closure<T()>* makeStatic(T (*func)());

        /** Make a bound member function closure.
            When invoked, calls the function on the object.
            \param Obj object type
            \param Ptr pointer type (could be a smart pointer)
            \param ptr pointer (must be dereferencable, i.e. operator*)
            \param func function
            \return the closure, never null */
        template<typename Obj, typename Ptr>
        static Closure<T()>* makeBound(Ptr ptr, T (Obj::*func)());

        /** A function that does nothing.
            Used to implement makeNull(), exported for convenience. */
        static T nullFunction();

        class Static;
        template<typename Obj, typename Ptr = Obj*> class Bound;
    };

    // Static member function
    template<typename T>
    class Closure<T()>::Static : public Closure<T()> {
     public:
        Static(T (*func)());
        T call();
     private:
        T (*m_func)();
    };

    // Bound member function
    template<typename T>
    template<typename Obj, typename Ptr>
    class Closure<T()>::Bound : public Closure<T()> {
     public:
        Bound(Ptr ptr, T (Obj::*func)());
        T call();
     private:
        Ptr m_ptr;
        void (Obj::*m_func)();
    };


    /*
     *  n-ary
     */

#define AFL_BASE_CLOSURE_DEFINE()                                                                                                                      \
    template<typename T, AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TEMPLATE)>                                                                         \
    class Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))> : public Deletable {                                                            \
     public:                                                                                                                                           \
        typedef Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))> Self_t;                                                                   \
        virtual T call(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE)) = 0;                                                                          \
                                                                                                                                                       \
        static Self_t* makeNull();                                                                                                                     \
        static Self_t* makeStatic(T (*func)(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE)));                                                        \
        static Self_t* makeStatic(T (*func)());                                                                                                        \
                                                                                                                                                       \
        template<typename Obj, typename Ptr>                                                                                                           \
        static Self_t* makeBound(Ptr ptr, T (Obj::*)(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE)));                                               \
        template<typename Obj, typename Ptr>                                                                                                           \
        static Self_t* makeBound(Ptr ptr, T (Obj::*)());                                                                                               \
                                                                                                                                                       \
        class Static;                                                                                                                                  \
        template<typename Obj, typename Ptr = Obj*> class Bound;                                                                                       \
        class StaticNullary;                                                                                                                           \
        template<typename Obj, typename Ptr = Obj*> class BoundNullary;                                                                                \
    };                                                                                                                                                 \
                                                                                                                                                       \
    template<typename T, AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TEMPLATE)>                                                                         \
    class Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>::Static                                                                         \
        : public Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>                                                                          \
    {                                                                                                                                                  \
     public:                                                                                                                                           \
        Static(T (*func)(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE)))                                                                            \
            : m_func(func)                                                                                                                             \
            { }                                                                                                                                        \
        T call(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_DECL))                                                                                       \
            { return m_func(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_NAME)); }                                                                       \
     private:                                                                                                                                          \
        T (*m_func)(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE));                                                                                 \
    };                                                                                                                                                 \
                                                                                                                                                       \
    template<typename T, AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TEMPLATE)>                                                                         \
    template<typename Obj, typename Ptr>                                                                                                               \
    class Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>::Bound                                                                          \
        : public Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>                                                                          \
    {                                                                                                                                                  \
     public:                                                                                                                                           \
        Bound(Ptr ptr, T (Obj::*func)(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE)))                                                               \
            : m_ptr(ptr), m_func(func)                                                                                                                 \
            { }                                                                                                                                        \
        T call(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_DECL))                                                                                       \
            { return ((*m_ptr).*m_func)(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_NAME)); }                                                           \
     private:                                                                                                                                          \
        Ptr m_ptr;                                                                                                                                     \
        T (Obj::*m_func)(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE));                                                                            \
    };                                                                                                                                                 \
                                                                                                                                                       \
    template<typename T, AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TEMPLATE)>                                                                         \
    class Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>::StaticNullary                                                                  \
        : public Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>                                                                          \
    {                                                                                                                                                  \
     public:                                                                                                                                           \
        StaticNullary(T (*func)())                                                                                                                     \
            : m_func(func)                                                                                                                             \
            { }                                                                                                                                        \
        T call(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))                                                                                       \
            { return m_func(); }                                                                                                                       \
     private:                                                                                                                                          \
        T (*m_func)();                                                                                                                                 \
    };                                                                                                                                                 \
                                                                                                                                                       \
    template<typename T, AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TEMPLATE)>                                                                         \
    template<typename Obj, typename Ptr>                                                                                                               \
    class Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>::BoundNullary                                                                   \
        : public Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>                                                                          \
    {                                                                                                                                                  \
     public:                                                                                                                                           \
        BoundNullary(Ptr ptr, T (Obj::*func)())                                                                                                        \
            : m_ptr(ptr), m_func(func)                                                                                                                 \
            { }                                                                                                                                        \
        T call(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))                                                                                       \
            { return ((*m_ptr).*m_func)(); }                                                                                                           \
     private:                                                                                                                                          \
        Ptr m_ptr;                                                                                                                                     \
        T (Obj::*m_func)();                                                                                                                            \
    };                                                                                                                                                 \
                                                                                                                                                       \
    template<typename T, AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TEMPLATE)>                                                                         \
    Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>*                                                                                      \
    Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>::makeNull()                                                                           \
    {                                                                                                                                                  \
        return new StaticNullary(Closure<T()>::nullFunction);                                                                                                 \
    }                                                                                                                                                  \
                                                                                                                                                       \
    template<typename T, AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TEMPLATE)>                                                                         \
    Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>*                                                                                      \
    Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>::makeStatic(T (*func)(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE)))              \
    {                                                                                                                                                  \
        return new Static(func);                                                                                                                       \
    }                                                                                                                                                  \
                                                                                                                                                       \
    template<typename T, AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TEMPLATE)>                                                                         \
    Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>*                                                                                      \
    Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>::makeStatic(T (*func)())                                                              \
    {                                                                                                                                                  \
        return new StaticNullary(func);                                                                                                                \
    }                                                                                                                                                  \
                                                                                                                                                       \
    template<typename T, AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TEMPLATE)>                                                                         \
    template<typename Obj, typename Ptr>                                                                                                               \
    Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>*                                                                                      \
    Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>::makeBound(Ptr ptr, T (Obj::*func)(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))) \
    {                                                                                                                                                  \
        return new Bound<Obj,Ptr>(ptr, func);                                                                                                          \
    }                                                                                                                                                  \
                                                                                                                                                       \
    template<typename T, AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TEMPLATE)>                                                                         \
    template<typename Obj, typename Ptr>                                                                                                               \
    Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>*                                                                                      \
    Closure<T(AFL_BASE_CLOSURE_ARGS(AFL_BASE_CLOSURE_ARG_TYPE))>::makeBound(Ptr ptr, T (Obj::*func)())                                                 \
    {                                                                                                                                                  \
        return new BoundNullary<Obj,Ptr>(ptr, func);                                                                                                   \
    }

#define AFL_BASE_CLOSURE_ARG_TEMPLATE(a,b) typename a
#define AFL_BASE_CLOSURE_ARG_TYPE(a,b) a
#define AFL_BASE_CLOSURE_ARG_NAME(a,b) b
#define AFL_BASE_CLOSURE_ARG_DECL(a,b) a b

    /* Unary */
#define AFL_BASE_CLOSURE_ARGS(picker) picker(T1, a)
    AFL_BASE_CLOSURE_DEFINE()
#undef AFL_BASE_CLOSURE_ARGS

    /* Binary */
#define AFL_BASE_CLOSURE_ARGS(picker) picker(T1, a), picker(T2, b)
    AFL_BASE_CLOSURE_DEFINE()
#undef AFL_BASE_CLOSURE_ARGS

    /* Ternary */
#define AFL_BASE_CLOSURE_ARGS(picker) picker(T1, a), picker(T2, b), picker(T3, c)
    AFL_BASE_CLOSURE_DEFINE()
#undef AFL_BASE_CLOSURE_ARGS

    /* Quaterary */
#define AFL_BASE_CLOSURE_ARGS(picker) picker(T1, a), picker(T2, b), picker(T3, c), picker(T4, d)
    AFL_BASE_CLOSURE_DEFINE()
#undef AFL_BASE_CLOSURE_ARGS

    /* Quinary */
#define AFL_BASE_CLOSURE_ARGS(picker) picker(T1, a), picker(T2, b), picker(T3, c), picker(T4, d), picker(T5, e)
    AFL_BASE_CLOSURE_DEFINE()
#undef AFL_BASE_CLOSURE_ARGS

    /* Senary */
#define AFL_BASE_CLOSURE_ARGS(picker) picker(T1, a), picker(T2, b), picker(T3, c), picker(T4, d), picker(T5, e), picker(T6, f)
    AFL_BASE_CLOSURE_DEFINE()
#undef AFL_BASE_CLOSURE_ARGS


} }

template<typename T>
inline
afl::base::Closure<T()>::Static::Static(T (*func)())
    : Closure<T()>(),
      m_func(func)
{ }

template<typename T>
template<typename Obj, typename Ptr>
inline
afl::base::Closure<T()>::Bound<Obj,Ptr>::Bound(Ptr ptr, T (Obj::*func)())
    : Closure<T()>(),
      m_ptr(ptr),
      m_func(func)
{ }

template<typename T>
template<typename Obj, typename Ptr>
T
afl::base::Closure<T()>::Bound<Obj,Ptr>::call()
{
    ((*m_ptr).*m_func)();
}

template<typename T>
template<typename Obj, typename Ptr>
afl::base::Closure<T()>*
afl::base::Closure<T()>::makeBound(Ptr ptr, T (Obj::*func)())
{
    return new Bound<Obj,Ptr>(ptr, func);
}

template<typename T>
afl::base::Closure<T()>*
afl::base::Closure<T()>::makeNull()
{
    return new Static(nullFunction);
}

template<typename T>
afl::base::Closure<T()>*
afl::base::Closure<T()>::makeStatic(T (*func)())
{
    return new Static(func);
}

template<typename T>
T
afl::base::Closure<T()>::nullFunction()
{
    return T();
}

template<typename T>
T
afl::base::Closure<T()>::Static::call()
{
    return m_func();
}

#endif
