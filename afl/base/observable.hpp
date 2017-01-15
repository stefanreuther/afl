/**
  *  \file afl/base/observable.hpp
  *  \brief Template class afl::base::Observable
  */
#ifndef AFL_AFL_BASE_OBSERVABLE_HPP
#define AFL_AFL_BASE_OBSERVABLE_HPP

#include "afl/base/signal.hpp"
#include "afl/base/uncopyable.hpp"

namespace afl { namespace base {

    /** Observable object.
        This provides a simple signal-if-changed mechanism.
        The Observable contains an object of type T,
        and raises sig_change whenever the value changes.
        Assignments of the current value, as in o.set(o.get()), do not raise the signal.

        \tparam T value type */
    template<typename T>
    class Observable : Uncopyable {
     public:
        /** Default constructor.
            Makes a new Observable, default-constructing the value */
        Observable();

        /** Constructor.
            Makes a new Observable, initializing it to the given value
            \param value Value */
        Observable(const T& value);

        /** Destructor. */
        ~Observable();

        /** Set value.
            If this is a value change, raises sig_change.
            \param newValue New value */
        void set(const T& newValue);

        /** Get value.
            \return value */
        const T& get() const;

        /** Signal: change.
            Raised whenever the value changes */
        Signal<void()> sig_change;

     private:
        T m_value;
    };

} }

template<typename T>
inline
afl::base::Observable<T>::Observable()
    : sig_change(),
      m_value()
{ }

template<typename T>
inline
afl::base::Observable<T>::Observable(const T& value)
    : sig_change(),
      m_value(value)
{ }

template<typename T>
inline
afl::base::Observable<T>::~Observable()
{ }

template<typename T>
void
afl::base::Observable<T>::set(const T& newValue)
{
    if (newValue != m_value) {
        m_value = newValue;
        sig_change.raise();
    }
}

template<typename T>
inline const T&
afl::base::Observable<T>::get() const
{
    return m_value;
}

#endif
