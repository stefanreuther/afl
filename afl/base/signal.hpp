/**
  *  \file afl/base/signal.hpp
  *  \brief Template class afl::base::Signal
  *
  *  \todo new SignalHandlerClosure(Closure_t::makeXxx()) is not exception-safe
  */
#ifndef AFL_AFL_BASE_SIGNAL_HPP
#define AFL_AFL_BASE_SIGNAL_HPP

#include "afl/base/uncopyable.hpp"
#include "afl/base/signalhandler.hpp"
#include "afl/base/closure.hpp"
#include "afl/base/clonableref.hpp"
#include "afl/base/ptr.hpp"

namespace afl { namespace base {

    namespace detail {
        /** Base class for signal.
            This class is an implementation detail to implement the constructor and add() methods.
            Users use the derived Signal<> class.
            \param Closure closure type for signal handlers */
        template<typename Closure>
        class SignalBase : public Uncopyable {
         public:
            typedef Closure Closure_t;

            /** Constructor. Makes a signal that has no handlers connected. */
            SignalBase()
                : m_list(0)
                { }

            /** Destructor. Disconnects all handler. */
            ~SignalBase();

            /** Add static function.
                \param Fun function type
                \param f Function as appropriate for the derived signal
                \return SignalHandler (for use in a SignalConnection) */
            template<typename Fun>
            SignalHandler* add(Fun f)
                { return addNewClosure(Closure_t::makeStatic(f)); }

            /** Add bound function.
                \param Obj object pointer type
                \param Fun function type
                \param obj Object pointer
                \param f member function as appropriate for the derived signal
                \return SignalHandler (for use in a SignalConnection) */
            template<typename Obj, typename Fun>
            SignalHandler* add(Obj obj, Fun f)
                { return addNewClosure(Closure_t::makeBound(obj, f)); }

            /** Add bound function, with lifetime control.
                \param Obj object pointer type
                \param Fun function type
                \param obj Object pointer. Ownership is taken by signal.
                \param f member function as appropriate for the derived signal
                \return SignalHandler (for use in a SignalConnection) */
            template<typename Obj, typename Fun>
            SignalHandler* addNew(Obj* obj, Fun f)
                { return addNewClosure(Closure_t::makeBound(afl::base::Ptr<Obj>(obj), f)); }

            /** Add closure.
                This function can be used to create custom Closure implementations.
                \param closure closure. Ownership is taken by signal.
                \return SignalHandler (for use in a SignalConnection) */
            SignalHandler* addNewClosure(Closure_t* closure);

         protected:
            SignalHandler* m_list;

            struct SignalHandlerClosure : public SignalHandler {
                SignalHandlerClosure(SignalHandler** pList, Closure_t* closure)
                    : SignalHandler(pList),
                      m_closure(closure)
                    { }
                ~SignalHandlerClosure()
                    { delete m_closure; }
                Closure_t* m_closure;

             private:
                SignalHandlerClosure(const SignalHandlerClosure&);
                SignalHandlerClosure& operator=(const SignalHandlerClosure&);
            };
        };
    }

    template<typename Signature> class Signal;

    /** Nullary signal.
        Accepts handlers that can be called with 0 arguments.
        \param T return value type */
    template<typename T>
    class Signal<T()> : public detail::SignalBase<Closure<T()> > {
     public:
        /** Raise signal. Calls all handlers.
            \return Return value of last called (=oldest) signal handler */
        T raise();
    };

    /** Unary signal.
        Accepts handlers that can be called with 0 or 1 argument.
        \param T return value type
        \param T1 first argument type */
    template<typename T, typename T1>
    class Signal<T(T1)> : public detail::SignalBase<Closure<T(T1)> > {
     public:
        /** Raise signal. Calls all handlers.
            \param a argument
            \return Return value of last called (=oldest) signal handler */
        T raise(T1 a);
    };

    /** Binary signal.
        Accepts handlers that can be called with 0 or 2 arguments.
        \param T return value type
        \param T1 first argument type
        \param T2 second argument type */
    template<typename T, typename T1, typename T2>
    class Signal<T(T1,T2)> : public detail::SignalBase<Closure<T(T1,T2)> > {
     public:
        /** Raise signal. Calls all handlers.
            \param a first argument
            \param b second argument
            \return Return value of last called (=oldest) signal handler */
        T raise(T1 a, T2 b);
    };

    /** Ternary signal.
        Accepts handlers that can be called with 0 or 3 arguments.
        \param T return value type
        \param T1 first argument type
        \param T2 second argument type
        \param T3 third argument type */
    template<typename T, typename T1, typename T2, typename T3>
    class Signal<T(T1,T2,T3)> : public detail::SignalBase<Closure<T(T1,T2,T3)> > {
     public:
        /** Raise signal. Calls all handlers.
            \param a first argument
            \param b second argument
            \param c third argument
            \return Return value of last called (=oldest) signal handler */
        T raise(T1 a, T2 b, T3 c);
    };

    /** Quaterary signal.
        Accepts handlers that can be called with 0 or 4 arguments.
        \param T return value type
        \param T1 first argument type
        \param T2 second argument type
        \param T3 third argument type
        \param T4 fourth argument type */
    template<typename T, typename T1, typename T2, typename T3, typename T4>
    class Signal<T(T1,T2,T3,T4)> : public detail::SignalBase<Closure<T(T1,T2,T3,T4)> > {
     public:
        /** Raise signal. Calls all handlers.
            \param a first argument
            \param b second argument
            \param c third argument
            \param d fourth argument
            \return Return value of last called (=oldest) signal handler */
        T raise(T1 a, T2 b, T3 c, T4 d);
    };

    /** Quinary signal.
        Accepts handlers that can be called with 0 or 5 arguments.
        \param T return value type
        \param T1 first argument type
        \param T2 second argument type
        \param T3 third argument type
        \param T4 fourth argument type
        \param T5 fifth argument type */
    template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
    class Signal<T(T1,T2,T3,T4,T5)> : public detail::SignalBase<Closure<T(T1,T2,T3,T4,T5)> > {
     public:
        /** Raise signal. Calls all handlers.
            \param a first argument
            \param b second argument
            \param c third argument
            \param d fourth argument
            \param e fifth argument
            \return Return value of last called (=oldest) signal handler */
        T raise(T1 a, T2 b, T3 c, T4 d, T5 e);
    };

    /** Senary signal.
        Accepts handlers that can be called with 0 or 6 arguments.
        \param T return value type
        \param T1 first argument type
        \param T2 second argument type
        \param T3 third argument type
        \param T4 fourth argument type
        \param T5 fifth argument type
        \param T6 sixth argument type */
    template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    class Signal<T(T1,T2,T3,T4,T5,T6)> : public detail::SignalBase<Closure<T(T1,T2,T3,T4,T5,T6)> > {
     public:
        /** Raise signal. Calls all handlers.
            \param a first argument
            \param b second argument
            \param c third argument
            \param d fourth argument
            \param e fifth argument
            \param f sixth argument
            \return Return value of last called (=oldest) signal handler */
        T raise(T1 a, T2 b, T3 c, T4 d, T5 e, T6 f);
    };

} }

/***************************** Implementation ****************************/

template<typename Closure>
afl::base::SignalHandler*
afl::base::detail::SignalBase<Closure>::addNewClosure(Closure_t* closure)
{
    m_list = new SignalHandlerClosure(&m_list, closure);
    return m_list;
}

/*
 *  SignalBase
 */

template<typename Closure>
afl::base::detail::SignalBase<Closure>::~SignalBase()
{
    // FIXME: this needn't actually be a template!
    while (m_list) {
        delete m_list;
    }
}

/*
 *  Signal0
 */

template<typename T>
T
afl::base::Signal<T()>::raise()
{
    typedef typename detail::SignalBase<Closure<T()> >::SignalHandlerClosure SignalHandlerClosure;
    SignalHandler* q = this->m_list;
    if (q == 0) {
        return T();
    } else {
        while (q->next() != 0) {
            SignalHandlerClosure* here = static_cast<SignalHandlerClosure*>(q);
            q = q->next();
            here->m_closure->call();
        }
        return static_cast<SignalHandlerClosure*>(q)->m_closure->call();
    }
}

/*
 *  Signal1
 */

template<typename T, typename T1>
T
afl::base::Signal<T(T1)>::raise(T1 a)
{
    typedef typename detail::SignalBase<Closure<T(T1)> >::SignalHandlerClosure SignalHandlerClosure;
    SignalHandler* q = this->m_list;
    if (q == 0) {
        return T();
    } else {
        while (q->next() != 0) {
            SignalHandlerClosure* here = static_cast<SignalHandlerClosure*>(q);
            q = q->next();
            here->m_closure->call(a);
        }
        return static_cast<SignalHandlerClosure*>(q)->m_closure->call(a);
    }
}

/*
 *  Signal2
 */

template<typename T, typename T1, typename T2>
T
afl::base::Signal<T(T1,T2)>::raise(T1 a, T2 b)
{
    typedef typename detail::SignalBase<Closure<T(T1,T2)> >::SignalHandlerClosure SignalHandlerClosure;
    SignalHandler* q = this->m_list;
    if (q == 0) {
        return T();
    } else {
        while (q->next() != 0) {
            SignalHandlerClosure* here = static_cast<SignalHandlerClosure*>(q);
            q = q->next();
            here->m_closure->call(a, b);
        }
        return static_cast<SignalHandlerClosure*>(q)->m_closure->call(a, b);
    }
}

/*
 *  Signal3
 */

template<typename T, typename T1, typename T2, typename T3>
T
afl::base::Signal<T(T1,T2,T3)>::raise(T1 a, T2 b, T3 c)
{
    typedef typename detail::SignalBase<Closure<T(T1,T2,T3)> >::SignalHandlerClosure SignalHandlerClosure;
    SignalHandler* q = this->m_list;
    if (q == 0) {
        return T();
    } else {
        while (q->next() != 0) {
            SignalHandlerClosure* here = static_cast<SignalHandlerClosure*>(q);
            q = q->next();
            here->m_closure->call(a, b, c);
        }
        return static_cast<SignalHandlerClosure*>(q)->m_closure->call(a, b, c);
    }
}

/*
 *  Signal4
 */

template<typename T, typename T1, typename T2, typename T3, typename T4>
T
afl::base::Signal<T(T1,T2,T3,T4)>::raise(T1 a, T2 b, T3 c, T4 d)
{
    typedef typename detail::SignalBase<Closure<T(T1,T2,T3,T4)> >::SignalHandlerClosure SignalHandlerClosure;
    SignalHandler* q = this->m_list;
    if (q == 0) {
        return T();
    } else {
        while (q->next() != 0) {
            SignalHandlerClosure* here = static_cast<SignalHandlerClosure*>(q);
            q = q->next();
            here->m_closure->call(a, b, c, d);
        }
        return static_cast<SignalHandlerClosure*>(q)->m_closure->call(a, b, c, d);
    }
}

/*
 *  Signal5
 */

template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
T
afl::base::Signal<T(T1,T2,T3,T4,T5)>::raise(T1 a, T2 b, T3 c, T4 d, T5 e)
{
    typedef typename detail::SignalBase<Closure<T(T1,T2,T3,T4,T5)> >::SignalHandlerClosure SignalHandlerClosure;
    SignalHandler* q = this->m_list;
    if (q == 0) {
        return T();
    } else {
        while (q->next() != 0) {
            SignalHandlerClosure* here = static_cast<SignalHandlerClosure*>(q);
            q = q->next();
            here->m_closure->call(a, b, c, d, e);
        }
        return static_cast<SignalHandlerClosure*>(q)->m_closure->call(a, b, c, d, e);
    }
}

/*
 *  Signal6
 */

template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
T
afl::base::Signal<T(T1,T2,T3,T4,T5,T6)>::raise(T1 a, T2 b, T3 c, T4 d, T5 e, T6 f)
{
    typedef typename detail::SignalBase<Closure<T(T1,T2,T3,T4,T5,T6)> >::SignalHandlerClosure SignalHandlerClosure;
    SignalHandler* q = this->m_list;
    if (q == 0) {
        return T();
    } else {
        while (q->next() != 0) {
            SignalHandlerClosure* here = static_cast<SignalHandlerClosure*>(q);
            q = q->next();
            here->m_closure->call(a, b, c, d, e, f);
        }
        return static_cast<SignalHandlerClosure*>(q)->m_closure->call(a, b, c, d, e, f);
    }
}

#endif
