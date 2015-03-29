/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Interface used by filament server.
 * \date        03/23/2015 03:27:25 PM
 * \file        filament_server.h
 * \version     1.0
 *
 *
 **/

#ifndef FILAMENT_FILAMENT_SERVER_H__
# define FILAMENT_FILAMENT_SERVER_H__

#include <filament/filament.h>

/**
 * \brief Creates new filament in the server context.
 * \return Structure with error code and pointer to allocated filament object.
 * \see new_filament
 *
 * The filament created by this method can communicate with client filaments.
 * Clients are created separately.
 */
new_filament get_filament_server( void );

#endif /* FILAMENT_FILAMENT_SERVER_H__ */

