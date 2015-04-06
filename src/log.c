/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Definitions of logging framework methods.
 * \date        04/02/2015 07:53:01 PM
 * \file        log.c
 * \version     1.0
 *
 *
 **/

#include <assert.h>
#include <errno.h> /* EALREADY, EINVAL, etc. */
#include <ionize/error.h> /* ionize_status */
#include <ionize/log.h>
#include <ionize/mutex.h> /* ionize_mutex */
#include <ionize/pointer_list.h> /* ionize_pointer_list */
#include <ionize/universal.h> /* THREADLOCAL, THREADUNSAFE, UNUSED */
#include <stdbool.h> /* bool */
#include <stddef.h> /* NULL */
#include <stdlib.h> /* malloc, free */
#include <string.h> /* memcpy */

struct ionize_log_obj_state_struct
{
    bool initialized;
    ionize_pointer_list handlers;
    ionize_mutex mutex;
};

typedef struct
{
    ionize_log_handler handler;
}
pointer_list_element;

static ionize_status
add( ionize_log_obj * const self, ionize_log_handler const handler )
{
    if(( NULL == self ) || ( NULL == self->state ))
    {
        return EINVAL;
    }

    pointer_list_element * const element =
        malloc( sizeof( pointer_list_element ));
    if( NULL == element )
    {
        return ENOMEM;
    }

    element->handler = handler;

    ionize_status result = self->state->mutex.lock( self->state->mutex );
    if( 0 == result )
    {
        result =
            self->state->handlers.add( &( self->state->handlers ), element );
        UNUSED( self->state->mutex.unlock( self->state->mutex ));
    }
    return result;
}

/*
 * handler is what we search for
 * pointer will be list element containing handler, returned from foreach
 */
typedef struct
{
    ionize_log_handler handler;
    void * pointer;
}
removal_userdata;

static ionize_status
removal_callback( void * const pointer, void * const userdata )
{
    pointer_list_element const * const element = pointer;
    removal_userdata * const data = userdata;

    /* exactly one element will be equal */
    if( element->handler == data->handler )
    {
        data->pointer = pointer;
        free( pointer );
    }
    return 0;
}

static ionize_status
remove( ionize_log_obj * const self, ionize_log_handler const handler )
{
    if(( NULL == self ) || ( NULL == self->state ))
    {
        return EINVAL;
    }

    removal_userdata data =
    {
        .handler = handler,
        .pointer = NULL
    };

    ionize_status result = self->state->mutex.lock( self->state->mutex );
    if( 0 != result )
    {
        return result;
    }
    result = self->state->handlers.foreach(
        self->state->handlers,
        removal_callback,
        &data
    );
    if( 0 == result )
    {
        /* data.pointer now contains pointer to remove from list */
        result = self->state->handlers.remove(
            &( self->state->handlers ),
            data.pointer
        );
    }
    UNUSED( self->state->mutex.unlock( self->state->mutex ));
    return result;
}

static THREADLOCAL ionize_log_obj_state state =
{
    .initialized = false
};

static THREADLOCAL ionize_log_obj log =
{
    .state = &state,
    .add = add,
    .remove = remove
};

typedef struct
{
    ionize_log_level level;
    char const * format;
    va_list args;
}
callback_userdata;

/*
 * since we control what gets added to the pointer list,
 * we can omit most of the error checks
 */
static ionize_status
handler_callback( void * const pointer, void * const userdata )
{
    pointer_list_element const * const element = pointer;
    callback_userdata * data = userdata;

    element->handler( data->level, data->format, data->args );
    return 0;
}

/* uses static variable log, won't modify it, except for using mutex */
void
ionize_log( ionize_log_level const level, char const * const format, ... )
{
    if( false == log.state->initialized )
    {
        return;
    }

    va_list args;
    va_start( args, format );

    callback_userdata data =
    {
        .level = level,
        .format = format,
    };
    memcpy( &( data.args ), &args, sizeof( va_list ));

    ionize_status result = log.state->mutex.lock( log.state->mutex );
    if( 0 == result )
    {
        result = log.state->handlers.foreach(
            log.state->handlers,
            handler_callback,
            &data
        );
        assert(( 0 == result ) || ( ENODATA == result ));
        UNUSED( log.state->mutex.unlock( log.state->mutex ));
    }

    va_end( args );
}

THREADUNSAFE ionize_log_setup_result ionize_log_setup( void )
{
    if( true == log.state->initialized )
    {
        return ( ionize_log_setup_result )
        {
            .status = EALREADY,
            .log = &log
        };
    }

    ionize_mutex_setup_result const mutex = ionize_mutex_setup();
    if( 0 != mutex.status )
    {
        return ( ionize_log_setup_result )
        {
            .status = mutex.status,
            .log = NULL
        };
    }
    ionize_pointer_list const handlers = ionize_pointer_list_setup();

    state.initialized = true;
    state.mutex = mutex.mutex;
    state.handlers = handlers;
    return ( ionize_log_setup_result )
    {
        .status = 0,
        .log = &log
    };
}

static ionize_status
free_callback( void * const pointer, void * const userdata )
{
    UNUSED( userdata );
    free( pointer );
    return 0;
}

THREADUNSAFE ionize_status ionize_log_cleanup( ionize_log_obj * const log )
{
    if( false == log->state->initialized )
    {
        return EALREADY;
    }

    ionize_status result = log->state->mutex.lock( log->state->mutex );
    if( 0 != result )
    {
        return result;
    }
    result = log->state->handlers.foreach(
        log->state->handlers,
        free_callback,
        NULL
    );
    assert(( 0 == result ) || ( ENODATA == result));
    result = ionize_pointer_list_cleanup( &( log->state->handlers ));
    UNUSED( log->state->mutex.unlock( log->state->mutex ));
    if(( 0 == result ) || ( ENODATA == result ))
    {
        result = ionize_mutex_cleanup( &( log->state->mutex ));
    }
    log->state->initialized = !( 0 == result );
    return result;
}

