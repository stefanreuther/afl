/**
  *  \file afl/base/optional.hpp
  *  \brief Class afl::base::Optional
  */
#ifndef AFL_AFL_BASE_OPTIONAL_HPP
#define AFL_AFL_BASE_OPTIONAL_HPP

#include "afl/base/types.hpp"
#include "afl/tmp/ifthenelse.hpp"
#include "afl/base/inlineoptional.hpp"
#include "afl/base/nothingtype.hpp"

namespace afl { namespace base {

    /** Optional object.
        Contains either an object of type T, or nothing.
        This can be used to pass optional/non-present values around.

        \param T contained type */
    template<typename T>
    class Optional {
     public:
        /** Default constructor.
            Makes an empty object. */
        Optional();

        /** Default constructor.
            Makes an empty object.
            This constructor allows you to write "return Nothing" to return a default-constructed Optional. */
        Optional(NothingType);

        /** Value constructor.
            Makes an object containing %value.
            \param value Value */
        Optional(const T& value);

        /** Copy constructor.
            Makes a copy: if the other object is empty, the new one will be empty;
            if the other object is nonempty, the new one will have a copy of its contained object.
            \param other Object to copy from */
        Optional(const Optional& other);

        /** Copy constructor.
            Makes a copy of a related object.
            \tparam Other Other type. Must be assignable to T.
            \param other Object to copy from */
        template<typename Other>
        Optional(const Optional<Other>& other);

        /** Copy constructor.
            Makes a copy of a related InlineOptional object.
            \tparam OtherStorage,OtherNull,OtherUser Parameters for other type.
            \param other Object to copy from */
        template<typename OtherStorage, OtherStorage OtherNull, class OtherUser>
        Optional(const InlineOptional<OtherStorage, OtherNull, OtherUser>& other);

        /** Destructor. */
        ~Optional();

        /** Assignment operator.
            Makes this object contain the same value as the other one.
            If both objects contain a value, the value is assigned internally;
            otherwise, constructor/destructor are used.
            \param other Object to copy from
            \return *this
            \see assign */
        Optional& operator=(const Optional& other);

        /** Assignment operator.
            Makes this object contain %value.
            If this object already contains a value, assigns it internally; otherwise, constructs a copy.
            \param value Object to copy from
            \return *this
            \see assign */
        Optional& operator=(const T& value);

        /** Assign nothing.
            Makes this object contain no value.
            \param value Nothing
            \return *this
            \see clear */
        Optional& operator=(NothingType value);

        /** Get contained object.
            If this object contains a value, returns a pointer to it.
            Otherwise, returns null.
            Use as "if (const T* p = x.get()) {...}".
            \return pointer */
        const T* get() const;

        /** Get contained object.
            If this object contains a value, returns a pointer to it.
            Otherwise, returns null.
            Use as "if (const T* p = x.get()) {...}".
            \return pointer */
        T* get();

        /** Get contained object, copy-out version.
            If this object contains a value, copies it to the provided variable.
            \param result [out] result
            \retval true Object contains a value which has been copied to \c result
            \retval false Object does not contain a value, \c unchanged */
        bool get(T& result) const;

        /** Check validity.
            \retval true This object contains a value
            \retval false This object is empty */
        bool isValid() const;

        /** Assign a value.
            Makes this object contain %value.
            If this object already contains a value, assigns it internally; otherwise, constructs a copy.
            This method allows a possibly mixed-type assignment,
            e.g. Optional<String_t>::assign("const char*").
            \tparam Other Related type
            \param value Object to copy from
            \return *this
            \see assign */
        template<typename Other>
        void assign(const Other& value);

        /** Clear content.
            Makes this object contain nothing. */
        void clear();

        /** Compare.
            This is not operator== because it does not implement true trivalued logic. */
        bool isSame(const Optional& other) const;

        /** Get value or default.
            \param defaultValue default value
            \return contained value if any; otherwise, the given default */
        const T& orElse(const T& defaultValue) const;

     private:
        // Find a type that has suitable alignment.
        // Since we're in C++03, we don't have alignment language support, so just take a guess on the sizes.
        typedef typename afl::tmp::IfThenElse<sizeof(uint32_t)<sizeof(T), uint64_t,
            typename afl::tmp::IfThenElse<sizeof(uint16_t)<sizeof(T), uint32_t,
                typename afl::tmp::IfThenElse<sizeof(uint8_t)<sizeof(T), uint16_t, uint8_t>::Type
            >::Type
        >::Type Word_t;

        // Space for contained object.
        union {
            struct {
                char m_bytes[sizeof(T)];
                bool m_valid;
            } m_storage;
            Word_t m_align;
        } m_buffer;
    };

} }

template<typename T>
inline
afl::base::Optional<T>::Optional()
{
    m_buffer.m_storage.m_valid = false;
}

template<typename T>
inline
afl::base::Optional<T>::Optional(NothingType)
{
    m_buffer.m_storage.m_valid = false;
}

template<typename T>
inline
afl::base::Optional<T>::Optional(const T& value)
{
    // Start initialized to invalid in case constructor throws
    m_buffer.m_storage.m_valid = false;

    // Construct contained object
    new (static_cast<void*>(&m_buffer.m_storage.m_bytes)) T(value);
    m_buffer.m_storage.m_valid = true;
}

template<typename T>
inline
afl::base::Optional<T>::Optional(const Optional& other)
{
    m_buffer.m_storage.m_valid = false;
    if (const T* p = other.get()) {
        assign(*p);
    }
}

template<typename T>
template<typename Other>
inline
afl::base::Optional<T>::Optional(const Optional<Other>& other)
{
    m_buffer.m_storage.m_valid = false;
    if (const Other* p = other.get()) {
        new (static_cast<void*>(&m_buffer.m_storage.m_bytes)) T(*p);
        m_buffer.m_storage.m_valid = true;
    }
}

template<typename T>
template<typename OtherStorage, OtherStorage OtherNull, class OtherUser>
inline
afl::base::Optional<T>::Optional(const InlineOptional<OtherStorage, OtherNull, OtherUser>& other)
{
    m_buffer.m_storage.m_valid = false;
    OtherUser value;
    if (other.get(value)) {
        new (static_cast<void*>(&m_buffer.m_storage.m_bytes)) T(value);
        m_buffer.m_storage.m_valid = true;
    }
}

template<typename T>
inline
afl::base::Optional<T>::~Optional()
{
    if (T* p = get()) {
        p->~T();
    }
}

template<typename T>
afl::base::Optional<T>&
afl::base::Optional<T>::operator=(const Optional& other)
{
    if (const T* p = other.get()) {
        assign(*p);
    } else {
        clear();
    }
    return *this;
}

template<typename T>
inline afl::base::Optional<T>&
afl::base::Optional<T>::operator=(const T& other)
{
    assign(other);
    return *this;
}

template<typename T>
inline afl::base::Optional<T>&
afl::base::Optional<T>::operator=(NothingType)
{
    clear();
    return *this;
}

template<typename T>
inline
const T*
afl::base::Optional<T>::get() const
{
    return isValid()
        ? static_cast<const T*>(static_cast<const void*>(&m_buffer.m_storage.m_bytes))
        : 0;
}

template<typename T>
inline
T*
afl::base::Optional<T>::get()
{
    return isValid()
        ? static_cast<T*>(static_cast<void*>(&m_buffer.m_storage.m_bytes))
        : 0;
}

template<typename T>
inline bool
afl::base::Optional<T>::get(T& result) const
{
    if (const T* p = this->get()) {
        result = *p;
        return true;
    } else {
        return false;
    }
}

template<typename T>
inline bool
afl::base::Optional<T>::isValid() const
{
    return m_buffer.m_storage.m_valid;
}

template<typename T>
template<typename Other>
inline void
afl::base::Optional<T>::assign(const Other& value)
{
    if (T* p = get()) {
        *p = value;
    } else {
        new (static_cast<void*>(&m_buffer.m_storage.m_bytes)) T(value);
        m_buffer.m_storage.m_valid = true;
    }
}

template<typename T>
inline void
afl::base::Optional<T>::clear()
{
    if (T* p = get()) {
        p->~T();
        m_buffer.m_storage.m_valid = false;
    }
}

template<typename T>
bool
afl::base::Optional<T>::isSame(const Optional& other) const
{
    const T* a = get();
    const T* b = other.get();
    if (a) {
        return b != 0 && *a == *b;
    } else {
        return b == 0;
    }
}

template<typename T>
inline const T&
afl::base::Optional<T>::orElse(const T& defaultValue) const
{
    const T* a = get();
    return a != 0 ? *a : defaultValue;
}

#endif
