/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Definition of log handler priniting to Android's logcat.
 * \date        04/06/2015 04:06:04 AM
 * \file        logcat.c
 * \version     1.0
 *
 *
 **/

#include <android/log.h>
#include <ionize/log.h> /* ionize_log_level */
#include <ionize/log/logcat.h>
#include <ionize/universal.h> /* UNUSED */
#include <stdarg.h> /* va_list */

static android_LogPriority translate( ionize_log_level const level )
{
    switch( level )
    {
        case ERROR: return ANDROID_LOG_ERROR;
        case WARNING: return ANDROID_LOG_WARN;
        case INFO: return ANDROID_LOG_INFO;
        case DEBUG: return ANDROID_LOG_DEBUG;
        default: return ANDROID_LOG_DEFAULT;
    }
}

void ionize_log_logcat(
    ionize_log_level const level,
    char const * const format,
    va_list args
)
{
    /* vprint is relatively new in NDK */
    UNUSED( __android_log_vprint(
        translate( level ),
        "ionize",
        format,
        args
    ));
}

