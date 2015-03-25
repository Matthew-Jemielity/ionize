/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Utility macros
 * \date        07/03/2015 01:21:59 AM
 * \file        macros.h
 * \version     1.0
 *
 * Contains various utility macros used through the plasma project.
 **/
#ifndef PLASMA_MACROS_H__
# define PLASMA_MACROS_H__

/**
 * \defgroup PP_NARG Group of macros for getting number of arguments
 *
 * Those macros get the number of arguments passed to a variadic macro.
 * Basic usage is like: PP_NARG( a, b, c ), which will return (in this case) 3.
 * PP_NARG() will return 0. Macro can be used for more preprocessor magic.
 * Implementation gleaned from the wizards at Stack Overflow:
 * stackoverflow.com/questions/11317474/macro-to-count-number-of-arguments
 */
/**@{*/
# define PP_ARG_N( _1, _2, _3, N, ... ) N
# define PP_RSEQ_N 3, 2, 1, 0
# define PP_NARG_( ... ) PP_ARG_N( __VA_ARGS__ )
# define PP_COMMA_SEQ_N 1, 1, 0, 0
# define PP_HAS_COMMA( ... ) PP_NARG_( __VA_ARGS__, PP_COMMA_SEQ_N )
# define PP_COMMA() , /* the () is crucial */
# define PP_IS_ZERO_HELPER_0 PP_COMMA
# define PP_IS_ZERO( ARG ) PP_HAS_COMMA( PP_IS_ZERO_HELPER_ ## ARG )
# define PP_NARG_HELPER__00( N ) 1
# define PP_NARG_HELPER__01( N ) 0
# define PP_NARG_HELPER__11( N ) N
# define PP_NARG_HELPER_( A, B, N ) PP_NARG_HELPER__ ## A ## B( N )
# define PP_NARG_HELPER( A, B, N ) PP_NARG_HELPER_( A, B, N )
# define PP_NARG( ... ) \
    PP_NARG_HELPER( \
        PP_HAS_COMMA( __VA_ARGS__ ), \
        PP_HAS_COMMA( PP_COMMA __VA_ARGS__ () ), \
        PP_NARG_( __VA_ARGS__, PP_RSEQ_N ) \
    )
/**@}*/

#endif /* PLASMA_MACROS_H__ */

