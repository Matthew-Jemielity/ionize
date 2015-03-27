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

/**
 * \brief Forward declaration of the plasma structure.
 */
typedef struct plasma_struct plasma;
/**
 * \brief properties required from the memory buffer
 *
 * This structure has different meaning when used in allocation of memory
 * or locking a buffer for use.
 * When used in allocation, the service will try to allocate memory starting
 * from maximum given and, if that fails, move into smaller and smaller sizes
 * until minimum is reached. The step is defined by alignment. The allocated
 * buffer is aligned to size specified by the alignment field. If requested
 * alignment cannot be obtained, the allocation will fail.
 * When used for locking a buffer for use, the returned buffer (if possible)
 * will always have a size between minimum and maximum specified and alignment
 * as requested. If no such buffer is available, the server will return an
 * appropriate error.
 */
typedef struct
{
    size_t minimum; /** Minimum size of the buffer in bytes. */
    size_t maximum; /** Maximum size of the buffer in bytes. */
    size_t alignment; /** Alignment of the buffer, in bytes. */
}
plasma_properties;
/**
 * \brief Requests allocation of memory buffers with given sizes.
 * \param self Pointer to plasma object on which we'll operate.
 * \param properties Array of buffer properties.
 * \param length Length of sizes array, that many buffer will be allocated.
 * \return Zero on success, else error code.
 * \see plasma_properties
 *
 * Send a request for allocation to appropriate service. Allocated
 * space is added to the back of circular queue managed by the service.
 * This method blocks until service returns status of the allocation
 * to the client.
 */
typedef ionize_status ( * plasma_allocate_func )(
    plasma * const restrict self,
    plasma_properties const * const restrict properties,
    size_t const length
);
/**
 * \brief Representation of read-only memory buffer.
 */
typedef struct
{
    void const * data; /** Pointer to buffer memory */
    size_t size; /** Size of buffer memory */
}
plasma_read_only;
/**
 * \brief Representation of type returned by read lock method.
 * \see plasma_read_only
 */
typedef struct
{
    ionize_status status; /** Zero on success, else error code. */
    plasma_read_only buf; /** Read-only memory buffer */
}
plasma_read;
/**
 * \brief Locks first available buffer for reading and returns it.
 * \param self Pointer to plasma object on which we'll operate.
 * \param requested Properties of the buffer we want to acquire.
 * \return Structure containing error code and read-only buffer descriptor.
 * \warning Using the buffer after unlocking it is undefined.
 * \see plasma_read
 * \see plasma_properties
 *
 * Depending on the blocking behaviour this method will either block
 * until a buffer is available or return with status EAGAIN. Note that
 * the method will always block for the amount of time needed for backend
 * service to communicate with the client.
 */
typedef plasma_read ( * plasma_read_lock_func )(
    plasma * const self,
    plasma_properties const requested
);
/**
 * \brief Representation of writable memory buffer.
 */
typedef struct
{
    void * data; /** Pointer to buffer memory */
    size_t size; /** Size of buffer memory */
}
plasma_read_write;
/**
 * \brief Representation of type returned by write lock method.
 * \see plasma_read_write
 */
typedef struct
{
    ionize_status status; /** Zero on success, else error code. */
    plasma_read_write buf; /** Writable memory buffer */
}
plasma_write;
/**
 * \brief Locks first available buffer for writing and returns it.
 * \param self Pointer to plasma object on which we'll operate.
 * \param requested Properties of the buffer we want to acquire.
 * \return Structure containing error code and writable buffer descriptor.
 * \warning Using the buffer after unlocking it is undefined.
 * \see plasma_write
 * \see plasma_properties
 *
 * Depending on the blocking behaviour this method will either block
 * until a buffer is available or return with status EAGAIN. Note that
 * the method will always block for the amount of time needed for backend
 * service to communicate with the client.
 */
typedef plasma_write ( * plasma_write_lock_func )(
    plasma * const self,
    plasma_properties const requested
);
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
/**
 * \brief Representation of type returned by uid getter method.
 */
typedef struct
{
    ionize_status status; /** Zero on success, else error code. */
    uint32_t uid; /** Unique circular memory queue identifier. */
}
plasma_uid;
/**
 * \brief Gets unique identifier of the buffer circular queue.
 * \param self Pointer to plasma object on which we'll operate.
 * \return Structure with error code and 32-bit unique identifier.
 * \see plasma_uid
 *
 * If two plasma objects return the same uid, then their states are
 * synchronized, i.e. they point to and operate on same circular queue
 * of memory buffers. The state of buffer locks (blocking or not) is
 * local to the plasma object instance and not synchronized.
 * This method will not contact the backend service, it is local to
 * the client.
 */
typedef plasma_uid ( * plasma_uid_func )( plasma * const self );
/**
 * \brief Opaque type holding internal plasma state.
 */
typedef struct plasma_state_struct plasma_state;

/**
 * \brief Representation of the plasma object.
 * \see plasma_state
 * \see plasma_allocate_func
 * \see plasma_read_lock_func
 * \see plasma_write_lock_func
 * \see plasma_unlock_func
 * \see plasma_blocking_func
 * \see plasma_uid_func
 */
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

