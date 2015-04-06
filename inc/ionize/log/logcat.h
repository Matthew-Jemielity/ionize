/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Log handler printing to Android's logcat.
 * \date        04/06/2015 04:02:25 AM
 * \file        logcat.h
 * \version     1.0
 *
 *
 **/

#ifndef IONIZE_LOG_LOGCAT_H__
# define IONIZE_LOG_LOGCAT_H__

# include <ionize/log.h>
# include <stdarg.h> /* va_list */

void ionize_log_logcat(
    ionize_log_level const level,
    char const * const format,
    va_list args
);

#endif /* IONIZE_LOG_LOGCAT_H__ */

