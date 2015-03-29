/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Definitions of helper methods for plasma_properties.
 * \date        03/29/2015 06:13:03 AM
 * \file        properties.c
 * \version     1.0
 *
 *
 **/

#include <errno.h> /* E2BIG, EINVAL, ENOTSUP, ERANGE */
#include <ionize/error.h> /* ionize_status */
#include <plasma/properties.h> /* plasma_properties */
#include <stdalign.h> /* alignof */
#include <stddef.h> /* size_t, max_align_t */

ionize_status plasma_properties_validator( plasma_properties const property )
{
    /* alignment must be > 0 */
    if( 0U == property.alignment )
    {
        return EINVAL;
    }
    /* check if alignment is not bigger than maximum possible */
    if( alignof(max_align_t) < property.alignment )
    {
        return E2BIG;
    }
    /* alignment is 0 < x < max, but must also be power of 2 */
    if( 0U != ( property.alignment & ( property.alignment - 1U )))
    {
        return ENOTSUP;
    }
    /* minimum and maximum must be > 0 */
    if(( 0 == property.minimum ) || ( 0 == property.maximum ))
    {
        return EINVAL;
    }
    /* minimum and maximum must be proper range */
    if( property.minimum > property.maximum )
    {
        return ERANGE;
    }
    /* property is valid */
    return 0;
}

