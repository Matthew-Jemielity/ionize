/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Log handler printing to syslog.
 * \date        04/06/2015 04:02:25 AM
 * \file        syslog.h
 * \version     1.0
 *
 *
 **/

#ifndef IONIZE_LOG_SYSLOG_H__
# define IONIZE_LOG_SYSLOG_H__

# include <ionize/log.h>
# include <stdarg.h> /* va_list */

void ionize_log_syslog(
    ionize_log_level const level,
    char const * const format,
    va_list args
);

#endif /* IONIZE_LOG_SYSLOG_H__ */

