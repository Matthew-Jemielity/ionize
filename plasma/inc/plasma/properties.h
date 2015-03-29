/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Defines properties required from the memory buffer.
 * \date        03/29/2015 05:37:38 AM
 * \file        plasma_properties.h
 * \version     1.0
 *
 * The plasma_properties object is used when allocating a memory buffer or
 * when trying to lock it for read or write operation.
 */

#ifndef PLASMA_PROPERTIES_H__
# define PLASMA_PROPERTIES_H__

#include <ionize/error.h> /* ionize_status */
# include <stddef.h> /* size_t */

/**
 * \brief Properties required from the memory buffer.
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
 * Supported alignments are powers of two no greater that alignof(max_align_t).
 **/
typedef struct
{
    size_t minimum; /** Minimum size of the buffer in bytes. */
    size_t maximum; /** Maximum size of the buffer in bytes. */
    size_t alignment; /** Alignment of the buffer, in bytes. */
}
plasma_properties;

/*
 * \brief Checks whether plasma_properties structure doesn't contain errors.
 * \param property Plasma properties structure to validate.
 * \return Zero if property is valid, error code otherwise.
 *
 * Error codes that can be returned:
 * 1. E2BIG - alignment is greater than maximum supported;
 * 2. ENOTSUP - alignment value not supported;
 * 3. ERANGE - minimum > maximum;
 * 4. EINVAL - alignment, minimum or maximum have improper values, i.e. == 0;
 */
ionize_status plasma_properties_validator( plasma_properties const property );

#endif /* PLASMA_PROPERTIES_H__ */

