/**
  *  \file afl/string/formattraits.hpp
  *  \brief Template class afl::string::FormatTraits
  */
#ifndef AFL_AFL_STRING_FORMATTRAITS_HPP
#define AFL_AFL_STRING_FORMATTRAITS_HPP

#include "afl/tmp/typelist.hpp"
#include "afl/tmp/ifthenelse.hpp"
#include "afl/string/string.hpp"
#include "afl/string/formatunion.hpp"

namespace afl { namespace string {

    class FormatState;

    /*
     *  Generic
     */

    String_t formatToString(const String_t& str, FormatState& state);
    String_t formatToString(const void* p, FormatState& state);
    String_t formatToString(const char* p, FormatState& state);
    String_t formatToString(const float& val, FormatState& state);
    String_t formatToString(const double& val, FormatState& state);
    String_t formatToString(const long double& val, FormatState& state);

    /** Generic format traits.
        Defines how afl::string::Format formats an object of type T.
        It calls a function <tt>afl::string::formatToString(T, FormatState&)</tt>
        to do the actual formatting.

        To implement formatting for your own type, implement your own formatToString function,
        OR implement your own FormatTraits specialization.

        This class is never used directly, but can be specialized by the user.

        \param T Type to format */
    template<typename T>
    struct FormatTraits {
        /** Store value in a FormatUnion.
            Since Format lives mostly in temporaries, this just stores a pointer.
            Specializations can optimize by storing in the other variants of the FormatUnion.

            \param u [out] Target
            \param value [in] Value to store. */
        static void store(FormatUnion& u, const T& value)
            {
                u.m_ptr = &value;
            }

        /** Format value stored in FormatUnion.
            This function should evaluate all applicable format options.
            It need not handle FormatState::LeftJustFlag and FormatState::getWidth(),
            those are handled by Format if needed.

            \param u [in] Stored value
            \param state [in/out] Format options and states
            \return formatted value */
        static String_t format(const FormatUnion& u, FormatState& state)
            {
                return formatToString(*static_cast<const T*>(u.m_ptr), state);
            }
    };

    /** FormatTraits specialization for pointers.
        Defines how afl::string::Format formats a pointer type.
        It calls a function <tt>afl::string::formatToString(const T*, FormatState&)</tt>
        to do the actual formatting.
        It is a little more efficient than the generic version because it optimizes
        storage of the pointer in the FormatUnion. */
    template<typename T>
    struct FormatTraits<T*> {
        static void store(FormatUnion& u, const T* p)
            {
                u.m_ptr = p;
            }
        static String_t format(const FormatUnion& u, FormatState& state)
            {
                return formatToString(static_cast<const T*>(u.m_ptr), state);
            }
    };


    /*
     *  Integer Utilities
     *
     *  We must overload FormatTraits for integers on all known integer types.
     *  The problem is that we know about a few series of integers:
     *  - the standard types (short, int, etc.)
     *  - the standard generic typedefs (size_t, intmax_t, etc.)
     *  - the standard fixed-width typedefs (int8_t, etc.)
     *  but we do not know their relations, i.e. we do not know whether, say,
     *  uint32_t is the same as unsigned int, or size_t.
     *
     *  We therefore check for each type whether it is already contained in the
     *  list of types we already specialized for. If it is, we don't specialize
     *  on the typedef ("Real"), but on an incomplete-and-never-completed struct
     *  ("Faux"), which the user cannot generate, so that this template is never
     *  selected.
     */

    namespace detail {
        using afl::tmp::TypeList;

        /** Select type to specialize on.
            \internal
            \param List TypeList of types already specialized for
            \param Faux Incomplete-and-never-completed struct to return if type is already specialized for
            \param Real Real type, to check for in list, and to return if it is not already specialized for */
        template<typename List, typename Faux, typename Real>
        struct FormatSelect {
            /** Return value. Faux or Real, see class description. */
            typedef typename afl::tmp::IfThenElse<afl::tmp::TypeListContains<Real, List>::result, Faux, Real>::Type Type;
        };
    }


    /*
     *  Unsigned Integers
     */

    namespace detail {
        /** FormatTraits for unsigned integers. */
        struct FormatUnsigned {
            static void store(FormatUnion& u, uintmax_t value)
                {
                    u.m_unsigned = value;
                }
            static String_t format(const FormatUnion& u, FormatState& state);
        };

        typedef TypeList<unsigned char,
                    TypeList<unsigned short,
                        TypeList<unsigned int,
                            TypeList<unsigned long,
                                TypeList<unsigned long long, void> > > > > UnsignedTypes1;
        typedef TypeList<size_t, UnsignedTypes1> UnsignedTypes2;
        typedef TypeList<uintptr_t, UnsignedTypes2> UnsignedTypes3;
        typedef TypeList<uintmax_t, UnsignedTypes3> UnsignedTypes4;
    }

    template<> struct FormatTraits<unsigned char>  : public detail::FormatUnsigned { };
    template<> struct FormatTraits<unsigned short> : public detail::FormatUnsigned { };
    template<> struct FormatTraits<unsigned int>   : public detail::FormatUnsigned { };
    template<> struct FormatTraits<unsigned long>  : public detail::FormatUnsigned { };
    template<> struct FormatTraits<unsigned long long> : public detail::FormatUnsigned { };
    template<> struct FormatTraits<bool>           : public detail::FormatUnsigned { };
    template<> struct FormatTraits<detail::FormatSelect<detail::UnsignedTypes1, struct FormatSizet,    size_t>::Type>    : public detail::FormatUnsigned { };
    template<> struct FormatTraits<detail::FormatSelect<detail::UnsignedTypes2, struct FormatUintptrt, uintptr_t>::Type> : public detail::FormatUnsigned { };
    template<> struct FormatTraits<detail::FormatSelect<detail::UnsignedTypes3, struct FormatUintmaxt, uintmax_t>::Type> : public detail::FormatUnsigned { };
    template<> struct FormatTraits<detail::FormatSelect<detail::UnsignedTypes4, struct FormatU8,       uint8_t >::Type>  : public detail::FormatUnsigned { };
    template<> struct FormatTraits<detail::FormatSelect<detail::UnsignedTypes4, struct FormatU16,      uint16_t>::Type>  : public detail::FormatUnsigned { };
    template<> struct FormatTraits<detail::FormatSelect<detail::UnsignedTypes4, struct FormatU32,      uint32_t>::Type>  : public detail::FormatUnsigned { };
    template<> struct FormatTraits<detail::FormatSelect<detail::UnsignedTypes4, struct FormatU64,      uint64_t>::Type>  : public detail::FormatUnsigned { };


    /*
     *  Signed Integers
     */

    namespace detail {
        /** FormatTraits for signed integers. */
        struct FormatSigned {
            static void store(FormatUnion& u, intmax_t value)
                {
                    u.m_signed = value;
                }
            static String_t format(const FormatUnion& u, FormatState& state);
        };

        typedef TypeList<signed char,
                    TypeList<char,
                        TypeList<signed short,
                            TypeList<signed int,
                                TypeList<signed long,
                                    TypeList<signed long long, void> > > > > > SignedTypes1;
        typedef TypeList<ptrdiff_t, SignedTypes1> SignedTypes2;
        typedef TypeList<intptr_t, SignedTypes2> SignedTypes3;
        typedef TypeList<intmax_t, SignedTypes3> SignedTypes4;
    }

    template<> struct FormatTraits<char>         : public detail::FormatSigned { };
    template<> struct FormatTraits<signed char>  : public detail::FormatSigned { };
    template<> struct FormatTraits<signed short> : public detail::FormatSigned { };
    template<> struct FormatTraits<signed int>   : public detail::FormatSigned { };
    template<> struct FormatTraits<signed long>  : public detail::FormatSigned { };
    template<> struct FormatTraits<signed long long> : public detail::FormatSigned { };
    template<> struct FormatTraits<detail::FormatSelect<detail::SignedTypes1, struct FormatPtrdifft, ptrdiff_t>::Type> : public detail::FormatSigned { };
    template<> struct FormatTraits<detail::FormatSelect<detail::SignedTypes2, struct FormatIntptrt,  intptr_t>::Type>  : public detail::FormatSigned { };
    template<> struct FormatTraits<detail::FormatSelect<detail::SignedTypes3, struct FormatIntmaxt,  intmax_t>::Type>  : public detail::FormatSigned { };
    template<> struct FormatTraits<detail::FormatSelect<detail::SignedTypes4, struct FormatI8,       int8_t >::Type>   : public detail::FormatSigned { };
    template<> struct FormatTraits<detail::FormatSelect<detail::SignedTypes4, struct FormatI16,      int16_t>::Type>   : public detail::FormatSigned { };
    template<> struct FormatTraits<detail::FormatSelect<detail::SignedTypes4, struct FormatI32,      int32_t>::Type>   : public detail::FormatSigned { };
    template<> struct FormatTraits<detail::FormatSelect<detail::SignedTypes4, struct FormatI64,      int64_t>::Type>   : public detail::FormatSigned { };

} }

#endif
