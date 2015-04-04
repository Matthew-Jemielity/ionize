/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Definitions of pointer list methods.
 * \date        04/03/2015 05:06:42 AM
 * \file        pointer_list.c
 * \version     1.0
 *
 *
 **/

#include <errno.h>
#include <ionize/error.h> /* ionize_status */
#include <ionize/pointer_list.h>
#include <ionize/universal.h>
#include <stdlib.h>

static ionize_status
add( ionize_pointer_list * const self, void * const pointer )
{
    if( NULL == self )
    {
        return EINVAL;
    }

    ionize_pointer_list_element ** current = &( self->head );

    for( ; NULL != *current; current = &(( *current )->next ))
    {
        if( pointer == ( *current )->pointer )
        {
            return EEXIST;
        }
    }

    *current = malloc( sizeof( ionize_pointer_list_element ));
    if( NULL == *current )
    {
        return ENOMEM;
    }
    ( *current )->pointer = pointer;
    ( *current )->next = NULL;
    return 0;
}

static ionize_status
remove( ionize_pointer_list * const self, void * const pointer )
{
    if( NULL == self )
    {
        return EINVAL;
    }

    for(
        ionize_pointer_list_element ** current = &( self->head );
        NULL != *current;
        current = &(( *current )->next )
    )
    {
        if( pointer != ( *current )->pointer )
        {
            continue;
        }

        ionize_pointer_list_element * const found = *current;
        *current = ( *current )->next;
        free( found );
        return 0;
    }

    return ENODATA;
}

static ionize_status foreach(
    ionize_pointer_list const self,
    ionize_pointer_list_foreach_callback const callback,
    void * const userdata
)
{
    if( NULL == callback )
    {
        return ENOEXEC;
    }

    ionize_status result = ENODATA;
    for(
        ionize_pointer_list_element * current = self.head;
        current != NULL;
        current = current->next
    )
    {
        result = callback( current->pointer, userdata );
        if( 0 != result )
        {
            break;
        }
    }
    return result;
}

ionize_pointer_list ionize_pointer_list_setup( void )
{
    return ( ionize_pointer_list )
    {
        .head = NULL,
        .add = add,
        .remove = remove,
        .foreach = foreach
    };
}

/*
 * removal of element known to be on the list will not fail
 * therefore we'll simplify the implementation to use that info
 */
ionize_status ionize_pointer_list_cleanup( ionize_pointer_list * const list )
{
    if( NULL == list )
    {
        return EINVAL;
    }

    while( NULL != list->head )
    {
        UNUSED( list->remove( list, list->head->pointer ));
    }
    return 0;
}

