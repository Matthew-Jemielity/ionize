/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Definition of log handler priniting to syslog.
 * \date        04/06/2015 04:06:04 AM
 * \file        syslog.c
 * \version     1.0
 *
 *
 **/

#define _POSIX_C_SOURCE 200112L /* for vsnprintf */

#include <ionize/log.h> /* ionize_log_level */
#include <ionize/log/syslog.h>
#include <ionize/universal.h> /* UNUSED */
#include <stdarg.h> /* va_list */
#include <stdio.h> /* vsnprintf */
#include <syslog.h>

#define STRING_LIMIT 1024U

static int translate( ionize_log_level const level )
{
    switch( level )
    {
        case ERROR: return LOG_ERR;
        case WARNING: return LOG_WARNING;
        case INFO: return LOG_INFO;
        case DEBUG: return LOG_DEBUG;
        default: return LOG_NOTICE;
    }
}

void ionize_log_syslog(
    ionize_log_level const level,
    char const * const format,
    va_list args
)
{
    openlog( "ionize", LOG_PID, LOG_USER );

    char buffer[ STRING_LIMIT + 1 ] = { 0, };
    UNUSED( vsnprintf( buffer, STRING_LIMIT, format, args ));
    syslog( translate( level ), "%s", buffer );

    closelog();
}

