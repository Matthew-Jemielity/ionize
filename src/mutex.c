/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       C version-specific mplementation of simple mutex.
 * \date        04/03/2015 10:16:23 AM
 * \file        mutex.c
 * \version     1.0
 *
 *
 **/

#include <errno.h>
#include <ionize/mutex.h>
#include <stdlib.h>
#if __STDC_NO_THREADS__
# include <pthread.h>
#else /* !__STDC_NO_THREADS__ */
# include <threads.h>
#endif /* __STDC_NO_THREADS__ */

typedef
#if __STDC_NO_THREADS__
    pthread_mutex_t
#else /* !__STDC_NO_THREADS__ */
    mtx_t
#endif /* __STDC_NO_THREADS__ */
internal_mutex;

struct ionize_mutex_state_struct
{
    internal_mutex mutex;
};

typedef int ( * internal_mutex_op )( internal_mutex * mutex );
static internal_mutex_op const lock_op =
#if __STDC_NO_THREADS__
    pthread_mutex_lock
#else /* !__STDC_NO_THREADS__ */
    mtx_lock
#endif /* __STDC_NO_THREADS__ */
;
static internal_mutex_op const unlock_op =
#if __STDC_NO_THREADS__
    pthread_mutex_unlock
#else /* !__STDC_NO_THREADS__ */
    mtx_unlock
#endif /* __STDC_NO_THREADS__ */
;

static ionize_status
op( ionize_mutex const self, internal_mutex_op const op )
{
    if( NULL == self.state )
    {
        return EINVAL;
    }

    switch( op( &( self.state->mutex )))
    {
#if __STDC_NO_THREADS__
        case 0:
#else /* !__STDC_NO_THREADS__ */
        case thrd_success:
#endif /* __STDC_NO_THREADS__ */
            return 0;
        default:
            return EIO;
    }
}

static ionize_status lock( ionize_mutex const self )
{
    return op( self, lock_op );
}

static ionize_status unlock( ionize_mutex const self )
{
    return op( self, unlock_op );
}

ionize_mutex_setup_result ionize_mutex_setup( void )
{
    ionize_mutex self =
    {
        .state = NULL,
        .lock = lock,
        .unlock = unlock
    };

    self.state = malloc( sizeof( ionize_mutex_state ));
    if( NULL == self.state )
    {
        return ( ionize_mutex_setup_result )
        {
            .status = ENOMEM,
            .mutex = self
        };
    }

#if __STDC_NO_THREADS__
    int const result = pthread_mutex_init( &( self.state->mutex ), NULL );
#else /* !__STDC_NO_THREADS__ */
    int const result = mtx_init( &( self.state->mutex ), mtx_plain );
#endif /* __STDC_NO_THREADS__ */
    switch( result )
    {
#if __STDC_NO_THREADS__
        case 0:
#else /* !__STDC_NO_THREADS__ */
        case thrd_success:
#endif /* __STDC_NO_THREADS__ */
            return ( ionize_mutex_setup_result )
            {
                .status = 0,
                .mutex = self
            };
        default:
            return ( ionize_mutex_setup_result )
            {
                .status = EIO,
                .mutex = self
            };
    }
}

ionize_status ionize_mutex_cleanup( ionize_mutex * const self )
{
    if(( NULL == self ) || ( NULL == self->state ))
    {
        return EINVAL;
    }

    /*
     * in case of pthreads (and possibly C11 threads, since they seem to use
     * pthreads) the implementation should allow safe destruction of mutex
     * right after it has been unlocked
     */
    self->lock( *self );
    self->unlock( *self );
#if __STDC_NO_THREADS__
    if( 0 != pthread_mutex_destroy( &( self->state->mutex )))
    {
        return EIO;
    }
#else /* !__STDC_NO_THREADS__ */
    mtx_destroy( &( self->state->mutex ));
#endif /* __STDC_NO_THREADS__ */
    free( self->state );
    self->state = NULL;
    return 0;
}

