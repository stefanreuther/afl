/**
  *  \file afl/string/formatstate.hpp
  *  \brief Class afl::string::FormatState
  */
#ifndef AFL_AFL_STRING_FORMATSTATE_HPP
#define AFL_AFL_STRING_FORMATSTATE_HPP

#include "afl/bits/smallset.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace string {

    /** Intermediate state for Format.
        This class contains all information extracted from a format string, needed to format a single argument.
        It consists of two parts:
        - per-argument state: this is the result of parsing the format place-holder.
          It is provided by #Format, and read by the #FormatTraits implementation.
          It is reset after each placeholder format.
        - persistent state: this is the result of formatting placeholders, controlling conditionals.
          It is set by the #FormatTraits implementation, and evaluated by #Format.
          It is persistent over multiple format operations. */
    class FormatState {
     public:
        /** Format flags. These are specified in the format string and are part of the per-argument state. */
        enum Flag {
            AlternateFlag,        /**< Alternate format "#". */
            ZeroPadFlag,          /**< Zero-pad "0" (overrides "-"). */
            LeftJustFlag,         /**< Left-justify "-". */
            BlankFlag,            /**< Prepend blank if positive " ". */
            SignFlag,             /**< Always include sign "+" (overrides " "). */
            GroupFlag,            /**< Use locale-specific grouping "'". */
            PrecisionFlag         /**< A precision has been specified. */
        };

        /** Condition flags. These are produced by formatted values and are part of the persistent state. */
        enum Condition {
            ZeroCondition,        /**< Result was 0. */
            OneCondition          /**< Result was 1. */
        };

        /** Constructor. */
        FormatState();

        /** Clear per-argument state.
            Keeps the persistent state (conditions). */
        void clear();

        /** Get format code (per-argument state).
            \return code set with setCode. */
        char getCode() const;

        /** Set format code (per-argument state).
            \param code format code */
        void setCode(char code);

        /** Get field width (per-argument state).
            \return width set with setWidth. */
        size_t getWidth() const;

        /** Set field width (per-argument state).
            \param width field width */
        void setWidth(size_t width);

        /** Get field precision (per-argument state).
            \return precision set with setPrecision. */
        size_t getPrecision() const;

        /** Set field precision (per-argument state).
            \param prec precision */
        void setPrecision(size_t prec);

        /** Check format flag (per-argument state).
            \param which flag to check
            \return true iff flag was set */
        bool hasFlag(Flag which) const;

        /** Set format flag (per-argument state).
            \param which flag to set */
        void setFlag(Flag which);

        /** Clear format flags (per-argument state). */
        void clearFlags();

        /** Check condition code (persistent state).
            \param which condition to check
            \return true iff condition is active */
        bool hasCondition(Condition which) const;

        /** Set condition code (persistent state).
            \param which condition to set
            \param value true to activate, false to deactivate condition */
        void setCondition(Condition which, bool value);

        /** Set number states for a string interpreted as a number (persistent state).
            Assumes that the #data refers to a number stringified with possibly fancy options
            (such as added unit before/after, grouping, whitespace, etc.),
            and sets the ZeroCondition and OneCondition accordingly.

            The idea is to make it possible for #Format to handle numeric conditions
            even if the input numbers are formatted by an external formatter.

            \param data [in] String */
        void setNumberConditions(ConstStringMemory_t data);

     private:
        char m_code;
        size_t m_width;
        size_t m_precision;
        afl::bits::SmallSet<Flag> m_flags;
        afl::bits::SmallSet<Condition> m_conditions;
    };

} }

#endif
