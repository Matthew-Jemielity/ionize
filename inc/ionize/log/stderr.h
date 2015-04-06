/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Log handler printing to stderr.
 * \date        04/06/2015 04:02:25 AM
 * \file        stderr.h
 * \version     1.0
 *
 *
 **/

#ifndef IONIZE_LOG_STDERR_H__
# define IONIZE_LOG_STDERR_H__

# include <ionize/log.h>

void ionize_log_stderr(
    ionize_log_level const level,
    char const * const format,
    va_list args
);

#endif /* IONIZE_LOG_STDERR_H__ */

