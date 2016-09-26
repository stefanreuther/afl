/**
  *  \file afl/base/staticassert.hpp
  *  \brief Macro static_assert
  */
#ifndef AFL_AFL_BASE_STATICASSERT_HPP
#define AFL_AFL_BASE_STATICASSERT_HPP

/** \def static_assert
    \brief Compile-Time Condition Check.

    Evaluates the specified expression, \c expr, during compile time
    and fails the compilation if the expression is not true.
    The expression must be a constant expression as far as the compiler is concerned,
    which allows for some more tests (in particular, using sizeof()) than the preprocessor.

    The message is intended to aid the programmer in finding out what's wrong;
    a C++11 compiler will usually display it with the error message.
    Older compilers will produce a message about an array size being negative.

    static_assert is allowed
    - in global scope (C/C++)
    - in functions (C/C++)
    - in class definitions (C++)

    \param expr Expression to test
    \param msg Message; a string constant. */

#if !__cplusplus
/* C version: 'extern' is allowed in global scope and in functions, and allows redefinitions. */
# define static_assert(expr, msg) extern int static_assert_##__LINE__[(expr) ? 2 : -2]
#elsif __cplusplus > 199711L
/* C++11 version: Nothing to do, compiler has this built in */
#else
/* C++98 version: 'typedef' is allowed in global scope, functions, and class definitions,
   and allows redefinitions.
   FIXME: some compilers support this despite reporting C++98;
   those will now get the macro instead of their builtin implementation. */
# ifdef __GNUC__
#  define __static_assert_unused __attribute__((unused))
# else
#  define __static_assert_unused
# endif
# define static_assert(expr, msg) typedef __static_assert_unused int static_assert_##__LINE__[(expr) ? 2 : -2]
#endif

#endif
