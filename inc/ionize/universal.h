/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Global macros, methods, etc.
 * \date        03/15/2015 04:29:59 AM
 * \file        universal.h
 * \version     1.0
 *
 *
 **/

#ifndef IONIZE_UNIVERSAL_H__
# define IONIZE_UNIVERSAL_H__

# include <stdint.h>
# if !__STDC_NO_THREADS__
#  include <threads.h> /* thread_local */
# endif /* !__STDC_NO_THREADS__ */

/**
 * \brief Specifies that a method shouldn't be used directly.
 *
 * This macro is currently a no-op. It signifies that a method wasn't designed
 * to be used directly and that there are legitimate wrappers that should be
 * used instead.
 */
# define INDIRECT

/**
 * \brief Specifies that a local copy of variable should be created per-thread.
 *
 * This macro provides a wrapper for optional C11 thread_local functionality.
 */
# if __STDC_NO_THREADS__
#  define THREADLOCAL
# else /* !__STDC_NO_THREADS__ */
#  define THREADLOCAL thread_local
# endif /* __STDC_NO_THREADS__ */

/**
 * \brief Specifies that a method is not thread-safe.
 *
 * Currently a no-op. Signifies that method wasn't designed to be thread-safe
 * (for example it uses static variables).
 */
# define THREADUNSAFE

/**
 * \brief Specifies that variable is not used in a compiler-friendly way.
 *
 * We want our code to compile cleanly even with gcc's -pedantic flag. This
 * macro allows to mark a variable that is not used at the moment. Provides
 * a single keyworkd to grep for and even allows for additional functionality,
 * like logging, if the macro is expanded.
 */
# define UNUSED( VAR ) (( void ) ( VAR ))

/**
 * \brief Describes port used in the client <-> server architecture.
 * \return Port number.
 *
 * Port number will be greater than 1024, so no special permissions are needed
 * to use the ionize framework. Ideally the port number will not be anything
 * known to be used by other programs.
 */
uint16_t ionize_port( void );

#endif /* IONIZE_UNIVERSAL_H__ */

