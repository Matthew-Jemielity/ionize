/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Definition of filament connection structure.
 * \date        03/22/2015 03:17:13 PM
 * \file        filament.h
 * \version     1.0
 *
 * Describes everything a client needs to know about filament.
 **/
#ifndef FILAMENT_FILAMENT_H__
# define FILAMENT_FILAMENT_H__

# include <ionize/error.h> /* ionize_status */
# include <stddef.h> /* size_t */
# include <stdint.h> /* uint8_t */

typedef struct filament_struct filament;

/**
 * \brief Type representing received buffer.
 * \remark The data buffer is allocated and must be freed.
 */
typedef struct
{
    uint8_t * data; /** Allocated byte buffer. */
    size_t size; /* Size of the returned buffer. */
}
rx_buf;
/**
 * \brief Type representing result of received transmission.
 * \see rx_buf
 */
typedef struct
{
    ionize_status status; /** Zero on success, else code. */
    rx_buf buf; /** Received buffer. */
}
filament_rx;
/**
 * \brief Initiates listening for transmission.
 * \param self Pointer to filament object on which we operate.
 * \return Structure containing status and received buffer.
 * \warning This method blocks until transmission is received.
 * \see filament_rx
 */
typedef filament_rx ( * filament_rx_func )(
    filament const * const self
);

/**
 * \brief Type representing sent buffer.
 */
typedef struct
{
    uint8_t const * data; /* Byte buffer. */
    size_t size; /* Size of the sent buffer. */
}
tx_buf;
/*
 * \brief Initiates sending transmission.
 * \param self Pointer to filament object on which we operate.
 * \param buf Structure representing sent buffer.
 * \return Zero on success, else error code.
 * \warning This method blocks until whole transmission is sent.
 * \see tx_buf
 */
typedef ionize_status ( * filament_tx_func )(
    filament const * const self,
    tx_buf const buf
);

/*
 * \brief Opaque type holding internal filament state.
 */
typedef struct filament_state_struct filament_state;

struct filament_struct
{
    filament_state * state;
    filament_rx_func rx;
    filament_tx_func tx;
};

/**
 * \brief Type representing result of newly created filament.
 * \remark The filament object is allocated, must be freed bu destroy_filament.
 * \see filament
 */
typedef struct
{
    ionize_status status; /** Zero on success, else code. */
    filament * filament; /* Pointer to allocated filament structure. */
}
new_filament;

/**
 * \brief Creates new filament in the client context.
 * \return Structure with error code and pointer to allocated filament object.
 * \see new_filament
 *
 * The filament created by this method can communicate with server.
 * Server must be created separately.
 */
new_filament get_filament( void );
/**
 * \brief Cleans up a filament object.
 * \param filament Pointer ot filament object, which will be destroyed.
 * \return Zero on success, else error code.
 * \warning Using the filament object after destruction is undefined.
 * \see filament
 */
ionize_status destroy_filament( filament * filament );

#endif /* FILAMENT_FILAMENT_H__ */

