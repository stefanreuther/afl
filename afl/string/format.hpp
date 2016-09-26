/**
  *  \file afl/string/format.hpp
  *  \brief Class afl::string::Format
  */
#ifndef AFL_AFL_STRING_FORMAT_HPP
#define AFL_AFL_STRING_FORMAT_HPP

#include <cassert>
#include <iosfwd>
#include "afl/base/types.hpp"
#include "afl/base/uncopyable.hpp"
#include "afl/bits/smallset.hpp"
#include "afl/string/formattraits.hpp"
#include "afl/string/formatunion.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace string {

    /** Typesafe "sprintf" workalike.
        This implements a format-string based conversion of arbitrary data into text.

        It supports up to 10 (MAX_ARGS) arguments.
        To use, construct a Format object, passing it the format string and the arguments
        (alternatively, add the arguments using the << operator), and convert to String_t.
        Formatting happens in the conversion operator.

        This is a class, not a function, to allow code like
        <code>
           String_t result = Format("...") << x << y;
        </code>
        without having to copy around a format state.
        It heavily relies on being used as a temporary object only, and takes pointers to its arguments.
        If not used as a temporary, make sure that all values live long enough!

        The format string contains escapes like this:
        <pre>
           \%<em>index$</em> <em>flags</em> <em>width</em> <em>.prec</em> <em>code</em>
        </pre>
        All parts except code are optional.

        <pre>
        index$  set argument pointer to index, e.g., "%5$d" means format 6th argument with type 'd'
        flags   contains
                   "#"  -- alternate format
                   "0"  -- pad with zero, not space; overrides '-'
                   "-"  -- left justify (default: right justify)
                   " "  -- (space) prepend blank to positive numbers
                   "+"  -- show positive sign if nonnegative, overrides " "
                   "'"  -- use 1000-separator. Can't be used with '0'
                   "!"  -- do not actually output value (i.e. only set
                           condition codes)
        width   positive number, output gets at least that many places,
                padded with spaces/zeroes to the left/right if needed.
                E.g., "%-10d" left justify to 10 places
        .prec   precision. differs for each type.
        code    modifier. differs for each type.
        </pre>

        Formats for strings (String_t):
        - precision is maximum number of characters to copy from string (e.g. "%.3s" copies at most 3 characters)
        - any modifier does
        - modifier 'd' causes conditions to be set like for integers, depending on content of the string

        Formats for C strings (const char*):
        - precision works like for strings. String need not be 0-terminated when this is used.
        - modifier 'p' causes output in pointer format (void*), any other causes output as string.
        - modifier 'd' causes conditions to be set like for integers, depending on content of the string

        Formats for integers:
        - precision is minimum number of digits. Output is zero-padded when needed
        - modifier selects base: 'o' octal, 'x' hex (lower-case), 'X' hex (upper-case), others decimal.
        - flag '#' causes "0" or "0x" to be prepended to oct/hex numbers

        Formats for floating point (float/double/long double):
        - precision is number of decimals after period, defaults to 6.
        - modifier selects output format: 'e'/'E' always exponential format (with 'e'/'E' for the exponent),
          'f'/'F' always decimal format, 'g'/'G' auto-select format and omit period of possible
        - flag '#' causes 'g'/'G' not to remove trailing zeroes
        - flag ''' is currently not supported
        - currently, after a floating-point output, condition codes 0 and 1 are unspecified.

        Formats for pointers (void*)
        - non-null pointers are output as unsigned longs with the '#' flag set and type 'x',
          hence the other integer things work, too.

        Format strings can contain conditional constructs.
        - \%<n>{\<value-if-true>\%}
        - \%<n>{\<value-if-true>\%|\<value-if-false>\%}

        \<n> is the number of the condition we're testing, 0 to 31.
        If omitted, it is zero.
        Currently, there are the conditions 0 (true iff last output number was zero)
        and 1 (true iff last output number was 1).
        This allows for things like
        <pre>
          "%d %1{item%|items%}"
        </pre>
        Conditions can be nested up to 31 levels.
        Conditions can be negated using "\%!<n>{".
        To test a value without outputting it, use the '!' flag for it:
        <pre>
          "%!d%1{one item%|%0$d items%}"
        </pre>
        Note the use of "%0$d" to reset the argument pointer to the first argument.

        To extend format for your own types, see FormatTraits.

        \todo
        - convert from const char* to StringMemory_t.
        - support %0'd (zeropad with grouping)
        - support %'f (floating point with grouping)
        - support infinities/NaNs in floating point. Needs C99 or BSD.
        - should FP set condition codes? */
    class Format : public afl::base::Uncopyable {
     public:
        /** Maximum number of arguments.
            Having this fixed simplifies life a lot. */
        static const size_t MAX_ARGS = 10;

        /** Construct formatter object with no parameters.
            Use operator<< to add parameters.
            \param fmt format string */
        Format(const char* fmt)
            : m_formatString(fmt),
              m_numArgs(0)
            { }

        /** Construct formatter object with one parameter.
            \param fmt format string
            \param arg first argument */
        template<typename A>
        Format(const char* fmt, const A& arg)
            : m_formatString(fmt),
              m_numArgs(1)
            {
                FormatTraits<A>::store(m_args[0].m_data, arg);
                m_args[0].m_pFunction = &FormatTraits<A>::format;
            }

        /** Construct formatter object with two parameters.
            \param fmt format string
            \param a first argument
            \param b second argument */
        template<typename A, typename B>
        Format(const char* fmt, const A& a, const B& b)
            : m_formatString(fmt),
              m_numArgs(2)
            {
                FormatTraits<A>::store(m_args[0].m_data, a);
                m_args[0].m_pFunction = &FormatTraits<A>::format;
                FormatTraits<B>::store(m_args[1].m_data, b);
                m_args[1].m_pFunction = &FormatTraits<B>::format;
            }

        /** Construct formatter object with three parameters.
            \param fmt format string
            \param a first argument
            \param b second argument
            \param c third argument */
        template<typename A, typename B, typename C>
        Format(const char* fmt, const A& a, const B& b, const C& c)
            : m_formatString(fmt),
              m_numArgs(3)
            {
                FormatTraits<A>::store(m_args[0].m_data, a);
                m_args[0].m_pFunction = &FormatTraits<A>::format;
                FormatTraits<B>::store(m_args[1].m_data, b);
                m_args[1].m_pFunction = &FormatTraits<B>::format;
                FormatTraits<C>::store(m_args[2].m_data, c);
                m_args[2].m_pFunction = &FormatTraits<C>::format;
            }

        /** Construct formatter object with four parameters.
            \param fmt format string
            \param a first argument
            \param b second argument
            \param c third argument
            \param d fourth argument */
        template<typename A, typename B, typename C, typename D>
        Format(const char* fmt, const A& a, const B& b, const C& c, const D& d)
            : m_formatString(fmt),
              m_numArgs(4)
            {
                FormatTraits<A>::store(m_args[0].m_data, a);
                m_args[0].m_pFunction = &FormatTraits<A>::format;
                FormatTraits<B>::store(m_args[1].m_data, b);
                m_args[1].m_pFunction = &FormatTraits<B>::format;
                FormatTraits<C>::store(m_args[2].m_data, c);
                m_args[2].m_pFunction = &FormatTraits<C>::format;
                FormatTraits<D>::store(m_args[3].m_data, d);
                m_args[3].m_pFunction = &FormatTraits<D>::format;
            }

        /** Add one parameter.
            \param T type
            \param arg value
            \return *this */
        template<typename T>
        Format& operator<<(const T& arg) {
            assert(m_numArgs < MAX_ARGS);
            // FIXME: what if T is volatile?
            FormatTraits<T>::store(m_args[m_numArgs].m_data, arg);
            m_args[m_numArgs].m_pFunction = &FormatTraits<T>::format;
            ++m_numArgs;
            return *this;
        }

        /** Convert to string. This does the actual conversion.
            \return formatted text */
        operator String_t() const;

     private:
        /** Argument state. */
        struct Arg {
            FormatUnion m_data;
            String_t (*m_pFunction)(const FormatUnion&, FormatState&);
        };

        const char* m_formatString;
        Arg m_args[MAX_ARGS];
        size_t m_numArgs;
    };

} }

/** Output formatted.
    This outputs the result of the format operation on the specified output stream to the left. */
std::ostream& operator<<(std::ostream& os, const afl::string::Format& fmt);

#endif
