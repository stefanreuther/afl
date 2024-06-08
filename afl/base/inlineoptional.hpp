/**
  *  \file afl/base/inlineoptional.hpp
  *  \brief Template class afl::base::InlineOptional
  */
#ifndef AFL_AFL_BASE_INLINEOPTIONAL_HPP
#define AFL_AFL_BASE_INLINEOPTIONAL_HPP

#include "afl/base/nothingtype.hpp"

namespace afl { namespace base {

    /** Optional object with inline signalisation.
        This is a simple version of Optional<T>, intended for efficient storage of scalars.
        Instead of storing a separate validity flag, it uses a special value to signify invalidity.
        This can be used to pass optional/non-present values around.

        For example, a value that can be 0..1000 or unknown could be represented as InlineOptional<int,-1>.

        \param StorageType Type used for storage of values
        \param NullValue Value to use as null value
        \param UserType Type to use in the user API. Can be different from the storage type */
    template<class StorageType, StorageType NullValue, class UserType = StorageType>
    class InlineOptional {
     public:
        /** Default constructor.
            Makes an empty object. */
        InlineOptional();

        /** Default constructor.
            Makes an empty object.
            This constructor allows you to write "return Nothing" to return a default-constructed InlineOptional. */
        InlineOptional(NothingType);

        /** Value constructor.
            Makes an object containing %value.
            \param value Value */
        InlineOptional(const UserType& value);

        // Use default copy constructor.

        /** Copy constructor for related object.
            Makes a copy of a related object.
            \tparam OtherStorage,OtherNull,OtherUser Parameters for other type.
            \param other Object to copy from */
        template<typename OtherStorage, OtherStorage OtherNull, class OtherUser>
        InlineOptional(const InlineOptional<OtherStorage, OtherNull, OtherUser>& other);

        // Use default (=no) destructor.

        /** Assignment operator.
            Makes this object contain the same value as the other one.
            If both objects contain a value, the value is assigned internally;
            otherwise, constructor/destructor are used.
            \param other Object to copy from
            \return *this
            \see assign */
        InlineOptional& operator=(const InlineOptional& other);

        /** Assignment operator.
            Makes this object contain %value.
            If this object already contains a value, assigns it internally; otherwise, constructs a copy.
            \param value Object to copy from
            \return *this
            \see assign */
        InlineOptional& operator=(const UserType& value);

        /** Assign nothing.
            Makes this object contain no value.
            \param value Nothing
            \return *this
            \see clear */
        InlineOptional& operator=(NothingType value);

        /** Get contained object, copy-out version.
            If this object contains a value, copies it to the provided variable.
            \param result [out] result
            \retval true Object contains a value which has been copied to \c result
            \retval false Object does not contain a value, \c unchanged */
        bool get(UserType& result) const;

        /** Check validity.
            \retval true This object contains a value
            \retval false This object is empty */
        bool isValid() const;

        /** Clear content.
            Makes this object contain nothing. */
        void clear();

        /** Compare.
            This is not operator== because it does not implement true trivalued logic. */
        bool isSame(const InlineOptional& other) const;

        /** Get value or default.
            \param defaultValue default value
            \return contained value if any; otherwise, the given default */
        UserType orElse(const UserType defaultValue) const;

     private:
        StorageType m_value;
    };

} }

// Default constructor.
template<class StorageType, StorageType NullValue, class UserType>
inline
afl::base::InlineOptional<StorageType, NullValue, UserType>::InlineOptional()
    : m_value(NullValue)
{ }

// Default constructor.
template<class StorageType, StorageType NullValue, class UserType>
inline
afl::base::InlineOptional<StorageType, NullValue, UserType>::InlineOptional(NothingType)
    : m_value(NullValue)
{ }

// Value constructor.
template<class StorageType, StorageType NullValue, class UserType>
inline
afl::base::InlineOptional<StorageType, NullValue, UserType>::InlineOptional(const UserType& value)
    : m_value(StorageType(value))
{ }

// Copy constructor for related object.
template<class StorageType, StorageType NullValue, class UserType>
template<typename OtherStorage, OtherStorage OtherNull, class OtherUser>
inline
afl::base::InlineOptional<StorageType, NullValue, UserType>::InlineOptional(const InlineOptional<OtherStorage, OtherNull, OtherUser>& other)
{
    OtherUser tmp;
    if (other.get(tmp)) {
        m_value = static_cast<StorageType>(tmp);
    } else {
        m_value = NullValue;
    }
}

// Assignment operator.
template<class StorageType, StorageType NullValue, class UserType>
inline
afl::base::InlineOptional<StorageType, NullValue, UserType>&
afl::base::InlineOptional<StorageType, NullValue, UserType>::operator=(const InlineOptional& other)
{
    m_value = other.m_value;
    return *this;
}

// Assignment operator.
template<class StorageType, StorageType NullValue, class UserType>
inline
afl::base::InlineOptional<StorageType, NullValue, UserType>&
afl::base::InlineOptional<StorageType, NullValue, UserType>::operator=(const UserType& value)
{
    m_value = StorageType(value);
    return *this;
}

// Assign nothing.
template<class StorageType, StorageType NullValue, class UserType>
inline
afl::base::InlineOptional<StorageType, NullValue, UserType>&
afl::base::InlineOptional<StorageType, NullValue, UserType>::operator=(NothingType)
{
    m_value = NullValue;
    return *this;
}

// Get contained object, copy-out version.
template<class StorageType, StorageType NullValue, class UserType>
inline bool
afl::base::InlineOptional<StorageType, NullValue, UserType>::get(UserType& result) const
{
    if (isValid()) {
        result = static_cast<UserType>(m_value);
        return true;
    } else {
        return false;
    }
}

// Check validity.
template<class StorageType, StorageType NullValue, class UserType>
inline bool
afl::base::InlineOptional<StorageType, NullValue, UserType>::isValid() const
{
    return m_value != NullValue;
}

// Clear content.
template<class StorageType, StorageType NullValue, class UserType>
inline void
afl::base::InlineOptional<StorageType, NullValue, UserType>::clear()
{
    m_value = NullValue;
}

// Compare.
template<class StorageType, StorageType NullValue, class UserType>
inline bool
afl::base::InlineOptional<StorageType, NullValue, UserType>::isSame(const InlineOptional& other) const
{
    return m_value == other.m_value;
}

// Get value or default.
template<class StorageType, StorageType NullValue, class UserType>
inline UserType
afl::base::InlineOptional<StorageType, NullValue, UserType>::orElse(const UserType defaultValue) const
{
    return m_value == NullValue ? defaultValue : static_cast<UserType>(m_value);
}

#endif
