/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Defines plasma_t control object.
 * \date        03/08/2015 05:00:23 AM
 * \file        plasma.h
 * \version     1.0
 *
 * The object controls operations of circular queue of memory buffers.
 * Each memory buffer can be read from or written to. Many reads from
 * the same buffer are possible, only one write to a buffer can happen
 * at the same time, reads and writes are mutually exclusive.
 * Circular queue is used. This means that if, for example, the last
 * element in the queue is locked for writing, a request for another
 * write will try searching writable buffers from the first element.
 **/
#ifndef PLASMA_PLASMA_H__
# define PLASMA_PLASMA_H__

# include <plasma/error.h> /* plasma_status_t */
# include <stdbool.h> /* bool */
# include <stddef.h> /* size_t */
# include <stdint.h> /* uint32_t */

typedef struct plasma plasma_t;
/**
 * \brief Requests allocation of memory buffers with given sizes.
 * \param self Pointer to plasma_t object on which we'll operate.
 * \param sizes Array of buffer sizes.
 * \param length Length of sizes array.
 * \return Zero on success, else error code.
 */
typedef plasma_status_t ( * plasma_allocate_func_t )(
    plasma_t * const restrict self,
    size_t const * const restrict sizes,
    size_t const length
);
typedef struct
{
    void const * buf; /** Pointer to buffer memory */
    size_t size; /** Size of buffer memory */
}
plasma_read_buf_t;
typedef struct
{
    plasma_status_t status;
    plasma_read_buf_t buf;
}
plasma_read_result_t;
/**
 * \brief Locks first available buffer for reading and returns it.
 * \param self Pointer to plasma_t object on which we'll operate.
 * \return Structure containing error code and read-only buffer descriptor.
 * \warning Using the buffer after unlocking it is undefined.
 */
typedef plasma_read_result_t
( * plasma_read_lock_func_t )( plasma_t * const self );
typedef struct
{
    void * buf; /** Pointer to buffer memory */
    size_t size; /** Size of buffer memory */
}
plasma_write_buf_t;
typedef struct
{
    plasma_status_t status;
    plasma_write_buf_t buf;
}
plasma_write_result_t;
/**
 * \brief Locks first available buffer for writing and returns it.
 * \param self Pointer to plasma_t object on which we'll operate.
 * \return Structure containing error code and writable buffer descriptor.
 * \warning Using the buffer after unlocking it is undefined.
 */
typedef plasma_write_result_t
( * plasma_write_lock_func_t )( plasma_t * const self );
/**
 * \brief Unlocks previously locked buffer.
 * \param self Pointer to plasma_t object on which we'll operate.
 * \return Zero on success, else error code.
 */
typedef plasma_status_t ( * plasma_unlock_func_t )( plasma_t * const self );
/**
 * \brief Sets mode of operation for locks.
 * \param self Pointer to plasma_t object on which we'll operate.
 * \param state New setting for mode of operation for locks.
 * \return Zero on success, erro code otherwise.
 * \warning Accessing buffer returned by read_lock or write_lock after unlock
 *          has been called is an undefined behavior.
 *
 * Buffer locks can operate in two modes:
 * - blocking - if there is lock contention and no other buffer is available
 *              then the lock operation will wait;
 * - non-blocking - in above case the operation will return with error.
 * Changing the mode will only work for new lock requests.
 */
typedef plasma_status_t ( * plasma_blocking_func_t )(
    plasma_t * const self,
    bool const state
);
typedef struct
{
    plasma_status_t status;
    uint32_t uid;
}
plasma_uid_t;
/**
 * \brief Gets unique identifier of the buffer circular queue.
 * \param self Pointer to plasma_t object on which we'll operate.
 * \return Structure with error code and 32-bit unique identifier.
 *
 * If two plasma_t objects return the same uid, then their states are
 * synchronized. They point to and operate on the same circular queue
 * of memory buffers.
 */
typedef plasma_uid_t ( * plasma_uid_func_t )( plasma_t * const self );
/**
 * \brief Opaque type holding internal plasma_t state.
 */
typedef struct plasma_state plasma_state_t;

struct plasma
{
    plasma_state_t * state;
    plasma_allocate_func_t allocate;
    plasma_read_lock_func_t read_lock;
    plasma_write_lock_func_t write_lock;
    plasma_unlock_func_t unlock;
    plasma_blocking_func_t blocking;
    plasma_uid_func_t uid;
};

#endif /* PLASMA_PLASMA_H__ */

