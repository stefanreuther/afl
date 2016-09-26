/**
  *  \file afl/base/countof.hpp
  *  \brief Macro countof
  */
#ifndef AFL_AFL_BASE_COUNTOF_HPP
#define AFL_AFL_BASE_COUNTOF_HPP

namespace afl { namespace base {

    /** Helper function for countof.
        This function is never implemented and never called.
        Do not call it.

        It is passed a reference to an array of arbitrary type,
        and returns a pointer to an array of char of the same number of elements. */
    template<typename T, int n> char (*countofHelperFunction(T (&)[n]))[n];

} }

/** Statically get number of array elements.
    This macro is suitable to use in array dimensioning, switch/case, and static_assert.
    \param x Name of an array variable
    \return Number of elements the variable is defined as */
#define countof(x) sizeof(*::afl::base::countofHelperFunction(x))


/*
 *  Classic version for reference:
 *   #define countof(x) sizeof(x) / sizeof(x[0])
 */

#endif
