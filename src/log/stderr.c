/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Definition of log handler priniting to stderr.h
 * \date        04/06/2015 04:06:04 AM
 * \file        stderr.c
 * \version     1.0
 *
 *
 **/

#include <ionize/log.h>
#include <ionize/log/stderr.h>
#include <ionize/universal.h>
#include <stdio.h>

void ionize_log_stderr(
    ionize_log_level const level,
    char const * const format,
    va_list args
)
{
    UNUSED( level );
    UNUSED( vfprintf( stderr, format, args ));
}

