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

/**
 * \brief Specifies that variable is not used in a compiler-friendly way.
 */
# define UNUSED( VAR ) (( void ) ( VAR ))

/**
 * \brief Describes port used in the client <-> server architecture.
 * \return Port number.
 */
uint16_t ionize_port( void );

#endif /* IONIZE_UNIVERSAL_H__ */

