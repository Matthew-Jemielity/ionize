/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Definition of log handler priniting to stderr.
 * \date        04/06/2015 04:06:04 AM
 * \file        stderr.c
 * \version     1.0
 *
 *
 **/

#include <ionize/log.h> /* ionize_log_level */
#include <ionize/log/stderr.h>
#include <ionize/universal.h> /* UNUSED */
#include <stdarg.h> /* va_list */
#include <stdio.h> /* fflush, vfprintf */

void ionize_log_stderr(
    ionize_log_level const level,
    char const * const format,
    va_list args
)
{
    UNUSED( level );
    UNUSED( vfprintf( stderr, format, args ));
    UNUSED( fflush( stderr ));
}

