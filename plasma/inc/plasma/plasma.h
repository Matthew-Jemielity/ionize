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

# include <ionize/error.h> /* ionize_status */
# include <stdbool.h> /* bool */
# include <stddef.h> /* size_t */
# include <stdint.h> /* uint32_t */

typedef struct plasma_struct plasma;
/**
 * \brief Requests allocation of memory buffers with given sizes.
 * \param self Pointer to plasma object on which we'll operate.
 * \param sizes Array of buffer sizes.
 * \param length Length of sizes array.
 * \return Zero on success, else error code.
 *
 * Send a request for allocation to appropriate service. Allocated
 * space is added to the back of circular queue managed by the service.
 * This method blocks until service returns status of the allocation
 * to the client.
 */
typedef ionize_status ( * plasma_allocate_func )(
    plasma * const restrict self,
    size_t const * const restrict sizes,
    size_t const length
);
typedef struct
{
    void const * data; /** Pointer to buffer memory */
    size_t size; /** Size of buffer memory */
}
plasma_read_only;
typedef struct
{
    ionize_status status;
    plasma_read_only buf;
}
plasma_read;
/**
 * \brief Locks first available buffer for reading and returns it.
 * \param self Pointer to plasma object on which we'll operate.
 * \return Structure containing error code and read-only buffer descriptor.
 * \warning Using the buffer after unlocking it is undefined.
 *
 * Depending on the blocking behaviour this method will either block
 * until a buffer is available or return with status EAGAIN. Note that
 * the method will always block for the amount of time needed for backend
 * service to communicate with the client.
 */
typedef plasma_read
( * plasma_read_lock_func )( plasma * const self );
typedef struct
{
    void * data; /** Pointer to buffer memory */
    size_t size; /** Size of buffer memory */
}
plasma_read_write;
typedef struct
{
    ionize_status status;
    plasma_read_write buf;
}
plasma_write;
/**
 * \brief Locks first available buffer for writing and returns it.
 * \param self Pointer to plasma object on which we'll operate.
 * \return Structure containing error code and writable buffer descriptor.
 * \warning Using the buffer after unlocking it is undefined.
 *
 * Depending on the blocking behaviour this method will either block
 * until a buffer is available or return with status EAGAIN. Note that
 * the method will always block for the amount of time needed for backend
 * service to communicate with the client.
 */
typedef plasma_write
( * plasma_write_lock_func )( plasma * const self );
/**
 * \brief Unlocks previously locked buffer.
 * \param self Pointer to plasma object on which we'll operate.
 * \return Zero on success, else error code.
 *
 * This method will block for amount of time needed for backend service to
 * communicate with the client.
 */
typedef ionize_status ( * plasma_unlock_func )( plasma * const self );
/**
 * \brief Sets mode of operation for locks.
 * \param self Pointer to plasma object on which we'll operate.
 * \param state New setting for mode of operation for locks.
 * \return Zero on success, error code otherwise.
 * \warning Accessing buffer returned by read_lock or write_lock after unlock
 *          has been called is an undefined behavior.
 *
 * Buffer locks can operate in two modes:
 * - blocking - if there is lock contention and no other buffer is available
 *              then the lock operation will wait;
 * - non-blocking - in above case the operation will return with error.
 * Changing the mode will only work for new lock requests.
 * This is a method local to the client, it doesn't communicate with backend
 * service.
 */
typedef ionize_status ( * plasma_blocking_func )(
    plasma * const self,
    bool const state
);
typedef struct
{
    ionize_status status;
    uint32_t uid;
}
plasma_uid;
/**
 * \brief Gets unique identifier of the buffer circular queue.
 * \param self Pointer to plasma object on which we'll operate.
 * \return Structure with error code and 32-bit unique identifier.
 *
 * If two plasma objects return the same uid, then their states are
 * synchronized. They point to and operate on the same circular queue
 * of memory buffers.
 * This method will block for the amount of time needed for backend service
 * to communicate with the client.
 */
typedef plasma_uid ( * plasma_uid_func )( plasma * const self );
/**
 * \brief Opaque type holding internal plasma state.
 */
typedef struct plasma_state_struct plasma_state;

struct plasma_struct
{
    plasma_state * state;
    plasma_allocate_func allocate;
    plasma_read_lock_func read_lock;
    plasma_write_lock_func write_lock;
    plasma_unlock_func unlock;
    plasma_blocking_func blocking;
    plasma_uid_func uid;
};

#endif /* PLASMA_PLASMA_H__ */

