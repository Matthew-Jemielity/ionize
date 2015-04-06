/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Logging facilities.
 * \date        04/02/2015 12:48:19 PM
 * \file        log.h
 * \version     1.0
 *
 *
 **/

#ifndef IONIZE_LOG_H__
# define IONIZE_LOG_H__

# define __STDC_FORMAT_MACROS /* needed for PRIu64 */

# include <inttypes.h> /* PRIu64 */
# include <ionize/error.h> /* ionize_status */
# include <ionize/universal.h> /* THREADUNSAFE */
# include <stdarg.h> /* va_list */
# include <stdint.h> /* uint64_t */

/**
 * \brief Defines type of log level.
 */
typedef enum
{
    ERROR, /** Unrecoverable errors */
    WARNING, /** Problems where recovery is possible */
    INFO, /** Messages that should be logged during normal operation */
    DEBUG /** Debug-only messages */
}
ionize_log_level;

/**
 * \brief Returns current time, as measured from some arbitrary point.
 * \return Time in nanoseconds, with at least millisecond precision.
 *
 * The time reported here is equal to the wall clock time, thus it's affected
 * by jumps resulting from changing the wall clock time.
 */
INDIRECT uint64_t ionize_log_time( void );

/**
 * \brief Directs output of a log message to registered handlers.
 * \param level Log level.
 * \param format Formatting string, as in printf.
 * \param ... Arguments to output, according to format, as in printf.
 * \see ionize_log_level
 *
 * This method operates on static values. It's thread safe.
 */
INDIRECT void
ionize_log( ionize_log_level const level, char const * const format, ... );

/**
 * \defgroup IONIZE_LOGGERS Group of logging macros.
 * \see IONIZE_LOG_WRAPPERS
 *
 * The macros in this group are responsible for extended logging. They include
 * current time, file, function name and line from which they are called. The
 * macros defined in this group shouldn't be used directly, only through the
 * wrapper macros defined in group IONIZE_LOG_WRAPPERS.
 *
 * @{
 */
# define IONIZE_LOG____( LEVEL, FORMAT, ... ) \
    ionize_log( \
        LEVEL, \
        "[%"PRIu64"][%s:%s:%u] " FORMAT "%c", \
        ionize_log_time(), \
        __FILE__, \
        __func__, \
        __LINE__, \
        __VA_ARGS__ \
    )
# define IONIZE_LOG__( LEVEL, ... ) IONIZE_LOG____( LEVEL, __VA_ARGS__, '\n' )
/**@}*/

/**
 * \defgroup IONIZE_LOG_WRAPPERS Convenient wrappers for logging.
 * \warning The first argument must be a string literal.
 * \see IONIZE_LOGGERS
 *
 * The macros in this group wrap around logging macros. They provide convenient
 * and easy to use way of logging messages. They should be used in way similar
 * to printf, i.e. IONIZE_DEBUG( "This is a debug message: %s" , "argument" );
 *
 * @{
 */
# define IONIZE_ERROR( ... ) IONIZE_LOG__( ERROR, __VA_ARGS__ )
# define IONIZE_WARNING( ... ) IONIZE_LOG__( WARNING, __VA_ARGS__ )
# define IONIZE_INFO( ... ) IONIZE_LOG__( INFO, __VA_ARGS__ )
# define IONIZE_DEBUG( ... ) IONIZE_LOG__( DEBUG, __VA_ARGS__ )
/**@}*/

/**
 * \brief Forward declaration of ionize_log_obj.
 */
typedef struct ionize_log_obj_struct ionize_log_obj;

/**
 * \brief Definition of a log handler function.
 * \param level Log level.
 * \param format Formatting string, as in printf.
 * \param args Arguments for the format string, as in vprintf.
 * \warning Handler implementations must be thread-safe.
 */
typedef void ( * ionize_log_handler )(
    ionize_log_level const level,
    char const * const format,
    va_list args
);

/**
 * \brief Operates on logging framework with a handler for log messages.
 * \param self Log object on which we'll operate.
 * \param handler Handler for log messages.
 * \return Zero on success, else error code.
 *
 * By default the logging system doesn't have any handlers registered. This
 * means logging is a no-op. By adding a handler we add an output for log
 * messages. This method may add a specific handler or remove one from the
 * list of all handlers set in self object.
 * Possible error codes:
 * 1. common:
 * a) EINVAL - invalid ionize_log_obj given;
 * 2. adding handler:
 * a) ENOMEM - failed to allocate memory for handler holder structure;
 * b) EEXIST - handler already is on the list of handlers;
 * c) codes returned by ionize_mutex locking and unlocking methods;
 * d) codes returned by ionize_pointer_list add method;
 * 3. removing handler:
 * a) codes returned by ionize_mutex  locking and unlocking methods;
 * b) codes returned by ionize_pointer_list remove method.
 */
typedef ionize_status ( * ionize_log_handling_func )(
    ionize_log_obj * const self,
    ionize_log_handler const handler
);

typedef struct ionize_log_obj_state_struct ionize_log_obj_state;

/**
 * \brief Declaration of log object.
 */
struct ionize_log_obj_struct
{
    ionize_log_obj_state * const state; /** Log object state. */
    ionize_log_handling_func const add; /** Adds handler to log object. */
    ionize_log_handling_func const remove; /** Removes handler from log. */
};

/**
 * \brief Defines return type of ionize_log_setup.
 */
typedef struct
{
    ionize_status status; /** Zero on sucess, else error code. */
    ionize_log_obj * log; /** Log object and control. */
}
ionize_log_setup_result;

/**
 * \brief Initializes the logging framework.
 * \returns Structure with error code and log object.
 * \see ionize_log_register
 * \see ionize_log_setup_result
 * \see ionize_mutex_setup
 *
 * This method must be called first before any logging macros or methods. It
 * initializes the internal structures used when logging. This method works
 * on static objects, which allows for easy, hassle free usage of logging
 * macros. By default, no handlers are registered (therefore logging would
 * do nothing). This method is NOT thread-safe as it has to use static
 * variable. If EALREADY is returned as status, the returned log pointer will
 * point to initialized structure that can be used normally.
 * Possible error codes:
 * 1. EALREADY - logging framework already initialized;
 * 2. any error code returned by ionize_mutex_setup.
 */
THREADUNSAFE ionize_log_setup_result ionize_log_setup( void );

/**
 * \brief Deinitializes and frees resources used by the logging framework.
 * \param log Log object to deinitialize and free.
 * \return Zero on success, else error code.
 * \see ionize_mutex_cleanup
 * \see ionize_mutex_func
 * \see ionize_pointer_list_cleanup
 *
 * This method frees internal structures used by logging framework. It operates
 * on static values. It's NOT thread-safe as it has to use static variable.
 * Possible error codes:
 * 1. EALREADY - logging framework already uninitialized;
 * 2. any error codes returned by ionize_mutex lock and unlock methods;
 * 3. any error codee returned by ionize_pointer_list_cleanup;
 * 4. any error code returned by ionize_mutex_cleanup.
 */
THREADUNSAFE ionize_status ionize_log_cleanup( ionize_log_obj * const log );

#endif /* IONIZE_LOG_H__ */

