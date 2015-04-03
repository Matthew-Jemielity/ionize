/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Basic mutex API, wrapper for possible implementations.
 * \date        04/03/2015 09:55:56 AM
 * \file        mutex.h
 * \version     1.0
 *
 * Mutex will be implemented using C11 mtx_t or, if unavailable, POSIX
 * threads' pthread_mutex_t.
 **/

#ifndef IONIZE_MUTEX_H__
# define IONIZE_MUTEX_H__

# include <ionize/error.h> /* ionize_status */

/**
 * \brief Forward declaration of ionize_mutex object.
 */
typedef struct ionize_mutex_struct ionize_mutex;

/**
 * \brief Common operations on ionize_mutex object.
 * \param self The ionize_mutex object on which we'll operate
 * \return Zero on success, else error code.
 *
 * Possible error codes:
 * 1. EINVAL - invalide value of self given;
 * 2. EIO - operation failed.
 */
typedef ionize_status ( * ionize_mutex_func )( ionize_mutex const self );

/**
 * \brief Forward declaration of ionize_mutex state object.
 */
typedef struct ionize_mutex_state_struct ionize_mutex_state;

/**
 * \brief Declaration of mutex object.
 */
struct ionize_mutex_struct
{
    ionize_mutex_state * state; /** Object's state */
    ionize_mutex_func const lock; /** Mutex locking method. */
    ionize_mutex_func const unlock; /** Mutex unlocking method. */
};

/**
 * \brief Declaration of type returned by ionize_mutex_setup.
 */
typedef struct
{
    ionize_status status; /** Zero on success, else error code. */
    ionize_mutex mutex; /** Mutex object. */
}
ionize_mutex_setup_result;

/**
 * \brief Creates simple mutex.
 * \return Structure containing error code and mutex object.
 * \see ionize_mutex_setup_result
 *
 * Possible error codes:
 * 1. ENOMEM - coudln't allocate memory for mutex state;
 * 2. EIO - mutex initialization failed.
 */
ionize_mutex_setup_result ionize_mutex_setup( void );

/**
 * \brief Safely destroys a ionize_mutex object.
 * \param mutex Mutex to destroy.
 * \return Zero on success, else error code.
 *
 * Possible error codes:
 * 1. EINVAL - invalid mutex object given;
 * 2. EIO - failure destroying the mutex.
 */
ionize_status ionize_mutex_cleanup( ionize_mutex * const mutex );

#endif /* IONIZE_MUTEX_H__ */

