/**
  *  \file afl/data/access.hpp
  *  \brief Class afl::data::Access
  */
#ifndef AFL_AFL_DATA_ACCESS_HPP
#define AFL_AFL_DATA_ACCESS_HPP

#include <memory>
#include "afl/string/string.hpp"
#include "afl/base/types.hpp"
#include "afl/data/integerlist.hpp"
#include "afl/data/stringlist.hpp"

namespace afl { namespace data {

    class Value;

    /** Value access and conversion.
        This implements convenience functions to convert Values to actual C++ values.
        It includes some fuzziness for cases where values have been transmitted
        over a protocol that does not preserve all type equivalences/details.
        For example, asking for a number when a string is given attempts to parse the string.
        Details are given in the respective functions.

        Types are resolved using Visitor, so actual type inheritance does not affect this.

        Any attempt to access an Error value (other than by isError(), isNull()) will cause a RemoteErrorException being thrown.

        Multithreading: This is a utility class that implements no locking of its own.

        Lifetime: Access objects are intended to be short-lived and do not take ownership of the structure they parse. */
    class Access {
     public:
        /** Constructor.
            Makes an access object for the given value.
            \param p [optional] Value (may be null, default) */
        Access(Value* p = 0);

        /** Constructor.
            Makes an access object for the given value.
            Simplifies usage of Access with auto_ptr values.
            Note that this does not take ownership!
            \param p Value. Passed by reference to avoid accidentally being given a temporary. */
        Access(std::auto_ptr<Value>& p);

        /** Convert to integer.
            - null value: returns 0
            - integer value: returns the value
            - float value: returns the truncated value
            - boolean value: returns 0 or 1
            - string value: if it is empty, returns 0; if it is parseable as a decimal integer (strToInteger),
              returns the value, otherwise throws an exception
            - errors: throw afl::except::RemoteErrorException
            - others: throw afl::except::InvalidDataException
            \return integer value
            \throw afl::except::InvalidDataException */
        int32_t toInteger() const;

        /** Convert to string.
            - null value: returns empty string
            - integer or float value: formats as decimal number
            - boolean value: returns "true" or "false"
            - vector value: returns all values, separated with comma
            - errors: throw afl::except::RemoteErrorException
            - others: returns a dummy string
            \return string value */
        String_t toString() const;

        /** Get size of array.
            If this is an array value, returns the size;
            If this is an error, throw a afl::except::RemoteErrorException.
            Otherwise, returns 0 (as an indication that calling operator[] is pointless).
            \return array size */
        size_t getArraySize() const;

        /** Get list of hash keys.
            If this is a hash, appends all hash keys to \c list.
            If this is an array, interprets it as a list of alternating key/value pairs lists the keys (first values).
            If this is an error, throw a afl::except::RemoteErrorException.
            Otherwise, does nothing.
            \param list [out] Keys are listed here */
        void getHashKeys(StringList_t& list) const;

        /** Get array element.
            If this is an array, and \c index is a valid index, returns an Access to the corresponding element.
            If this is an error, throw a afl::except::RemoteErrorException.
            Otherwise, returns a null access.
            \param index Array index
            \return access to element */
        Access operator[](size_t index) const;

        /** Get hash element.
            If this is a hash, returns an Access to the given value.
            If this is an array, interprets it as a list of alternating key/value pairs and returns the respective value, if any.
            If this is an error, throw a afl::except::RemoteErrorException.
            Otherwise, returns a null access.
            \param key Key
            \return access to element */
        Access operator()(String_t key) const;

        /** Get hash element.
            If this is a hash, returns an Access to the given value.
            If this is an array, interprets it as a list of alternating key/value pairs and returns the respective value, if any.
            If this is an error, throw a afl::except::RemoteErrorException.
            Otherwise, returns a null access.
            \param key Key
            \return access to element */
        Access operator()(const char* key) const;

        /** Get embedded value.
            \return embedded value */
        Value* getValue() const;

        /** Convert to integer list.
            If this is an array, converts all values as-if by calling toInteger(), and appends them to the list.
            If this is an error, throw a afl::except::RemoteErrorException.
            Otherwise, does nothing.
            \param list [in/out] data will be appended here */
        void toIntegerList(IntegerList_t& list) const;

        /** Convert to string list.
            If this is an array, converts all values as-if by calling toString(), and appends them to the list.
            If this is an error, throw a afl::except::RemoteErrorException.
            Otherwise, does nothing.
            \param list [in/out] data will be appended here  */
        void toStringList(StringList_t& list) const;

        /** Check for null.
            Note that error values are treated as non-null.
            \retval true embedded value is null
            \retval false embedded value is not null */
        bool isNull() const;

        /** Check for error.
            \param source [out] error source
            \param str [out] error text
            \retval true embedded value is an error
            \retval false embedded value is not an error */
        bool isError(String_t& source, String_t& str) const;

     private:
        Value* m_value;
    };

} }

#endif
