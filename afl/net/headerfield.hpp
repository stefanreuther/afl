/**
  *  \file afl/net/headerfield.hpp
  *  \brief Class afl::net::HeaderField
  */
#ifndef AFL_AFL_NET_HEADERFIELD_HPP
#define AFL_AFL_NET_HEADERFIELD_HPP

#include "afl/string/string.hpp"

namespace afl { namespace net {

    class HeaderConsumer;

    /** Header field.
        Represents a field, consisting of name and value, of some sort of header.

        Functions to parse structured fields accept a %mode parameter.
        This parameter defaults to 0, but you can use a combination of %NoQuotes, %NoComments, and %NoPrimary to modify behaviour:
        - by default, quoted strings are processed and the quotes removed.
          NoQuotes removes that behaviour; quotes are no longer special.
        - by default, parenthesized comments are processed and removed.
          NoComments removes that behaviour; parens are no longer special.
        - by default, the first value of a field up to a ";" is the primary value, followed by named secondary values.
          This is the format used for headers such as Content-Type ("text/plain; charset=utf-8").
          NoPrimary defines the header field as having named values only that are retrieved by getSecondaryValue().
          This format is used for some headers ("Cookie: one=1; two=2").

        In particular, a ";" within quotes or parens will separate fields when quotes or parens are disabled. */
    class HeaderField {
     public:
        static const int NoComments = 1;
        static const int NoQuotes   = 2;
        static const int NoPrimary  = 4;

        /** Constructor. Makes a blank HeaderField. */
        HeaderField();

        /** Constructor.
            \param name Name of field
            \param value Value of field */
        HeaderField(const String_t& name, const String_t& value);

        /** Destructor. */
        ~HeaderField();

        /** Get name.
            \return name */
        const String_t& getName() const;

        /** Get value.
            \return complete value */
        const String_t& getValue() const;

        /** Set value.
            \return new value */
        void setValue(const String_t& value);

        /** Get primary value of this header field.
            If the header field has a complex value, such as "text/html; charset=UTF-8",
            returns the primary value, in this case, "text/html".
            Quoting/escaping using double-quotes and backslashes is interpreted,
            comments using parentheses are also interpreted.
            \param mode Parse mode (NoQuotes, NoComments; see class description. NoPrimary has no effect here.)
            \return primary value */
        String_t getPrimaryValue(int mode = 0) const;

        /** Get secondary value of this header field.
            If the header field has a complex value, such as "text/html; charset=UTF-8",
            returns the secondary value identified by the %name.
            If %name is "charset", this would return "UTF-8" for the above example.
            Name matching is case-insensitive.

            Quoting/escaping using double-quotes and backslashes is interpreted,
            comments using parentheses are also interpreted.
            \param name [in] Secondary field name
            \param value [out] Secondary field value
            \param mode Parse mode (NoQuotes, NoComments, NoPrimary; see class description)
            \retval true field has been found, %value has been filled in
            \retval false field has not been found, %value is unchanged */
        bool getSecondaryValue(const String_t& name, String_t& value, int mode = 0) const;

        /** Get address value of this header field.
            This is intended to parse "From" headers.
            It accepts "add@ress (Name)", "add@ress", "Name <add@ress>" formats.
            \param result [out] Result goes here
            \retval true Address parsed successfully
            \retval false No address found */
        bool getAddressValue(String_t& result) const;

        /** Enumerate secondary values.
            If the header field has a complex value, such as "text/html; charset=UTF-8",
            enumerates all secondary fields.
            In the example, this will call HeaderConsumer::handleHeader("charset", "UTF-8").
            \param consumer Header consumer to receive the information
            \param mode Parse mode (NoQuotes, NoComments, NoPrimary; see class description) */
        void enumerateSecondaryValues(HeaderConsumer& consumer, int mode = 0) const;

     private:
        String_t m_name;
        String_t m_value;
    };

} }

#endif
