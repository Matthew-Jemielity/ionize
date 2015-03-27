/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Mock implementation of plasma structure functions.
 * \date        03/08/2015 07:26:10 AM
 * \file        test_plasma_01.c
 * \version     1.0
 *
 *
 **/
#define __STDC_FORMAT_MACROS
#define _XOPEN_SOURCE 500

#include <assert.h>
#include <inttypes.h>
#include <ionize/error.h>
#include <ionize/universal.h>
#include <plasma/macros.h>
#include <plasma/plasma.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> /* sync */

#define BUFSIZE 10
#define EDUMMY 0xC0FFEEEE

static uint8_t buf[ BUFSIZE ]; /* inited to zeroes */
static pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct plasma_state_struct
{
    bool blocking;
};

static ionize_status allocate(
    plasma * const restrict self,
    plasma_properties const * const restrict properties,
    size_t const length
)
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
        || ( NULL == properties )
        || ( length != 1U )
        || ( BUFSIZE != ( properties[ 0 ] ).minimum )
        || ( BUFSIZE != ( properties[ 0 ] ).maximum )
        || ( 1U != ( properties[ 0 ] ).alignment )
    )
    {
        return EINVAL;
    }
    return 0;
}

static plasma_read rlock(
    plasma * const self,
    plasma_properties const requested
);
static plasma_write wlock(
    plasma * const self,
    plasma_properties const requested
);
static plasma_read tryrlock(
    plasma * const self,
    plasma_properties const requested
);
static plasma_write trywlock(
    plasma * const self,
    plasma_properties const requested
);

static ionize_status blocking( plasma * const self, bool const state )
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
    )
    {
        return EINVAL;
    }
    ( void ) pthread_mutex_lock( &mutex );
    self->state->blocking = state;
    self->read_lock = ( self->state->blocking ) ? rlock : tryrlock;
    self->write_lock = ( self->state->blocking ) ? wlock : trywlock;
    ( void ) pthread_mutex_unlock( &mutex );
    return 0;
}

static ionize_status dummy_unlock( plasma * const self )
{
    UNUSED( self );
    return EINVAL;
}

static ionize_status unlock( plasma * const self )
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
    )
    {
        return EINVAL;
    }
    ( void ) pthread_mutex_lock( &mutex );
    int const result = pthread_rwlock_unlock( &rwlock );
    if( 0 == result )
    {
        self->unlock = dummy_unlock;
        self->read_lock = ( self->state->blocking ) ? rlock : tryrlock;
        self->write_lock = ( self->state->blocking ) ? wlock : trywlock;
    }
    ( void ) pthread_mutex_unlock( &mutex );
    return result;
}

/*
 * Normally we wouldn't change locking operations to dummies, however we
 * may want to test situations where locking should block, perhaps in an
 * automated manner. Therefore we intruduce dummy operations. An operation
 * returning EDUMMY value should block. EDUMMY value and dummy operations
 * are to be used only in testing.
 */
static plasma_read dummy_rlock(
    plasma * const self,
    plasma_properties const required
)
{
    UNUSED( self );
    UNUSED( required );
    return ( plasma_read )
    {
        EDUMMY,
        { NULL, 0 }
    };
}

static plasma_write dummy_wlock(
    plasma * const self,
    plasma_properties const required
)
{
    UNUSED( self );
    UNUSED( required );
    return ( plasma_write )
    {
        EDUMMY,
        { NULL, 0 }
    };
}

static plasma_read tryrlock(
    plasma * const self,
    plasma_properties const requested
)
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
        || ( BUFSIZE != requested.minimum )
        || ( BUFSIZE != requested.maximum )
        || ( 1U != requested.alignment )
    )
    {
        return ( plasma_read )
        {
            EINVAL,
            { NULL, 0 }
        };
    }
    ( void ) pthread_mutex_lock( &mutex );
    int const result = pthread_rwlock_tryrdlock( &rwlock );
    plasma_read const res = ( plasma_read )
        {
            result,
            { NULL, 0 }
        };
    if( 0 != result )
    {
        ( void ) pthread_mutex_unlock( &mutex );
        return res;
    }
    self->unlock = unlock;
    if( self->state->blocking )
    {
        self->read_lock = dummy_rlock;
        self->write_lock = dummy_wlock;
    }
    ( void ) pthread_mutex_unlock( &mutex );
    return ( plasma_read ) { 0, { buf, BUFSIZE } };
}

static plasma_read rlock(
    plasma * const self,
    plasma_properties const requested
)
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
        || ( BUFSIZE != requested.minimum )
        || ( BUFSIZE != requested.maximum )
        || ( 1U != requested.alignment )
    )
    {
        return ( plasma_read )
        {
            EINVAL,
            { NULL, 0 }
        };
    }
    ( void ) pthread_mutex_lock( &mutex );
    int const result = pthread_rwlock_rdlock( &rwlock );
    plasma_read const res = ( plasma_read )
        {
            result,
            { NULL, 0 }
        };
    if( 0 != result )
    {
        ( void ) pthread_mutex_unlock( &mutex );
        return res;
    }
    self->unlock = unlock;
    if( self->state->blocking )
    {
        self->read_lock = dummy_rlock;
        self->write_lock = dummy_wlock;
    }
    ( void ) pthread_mutex_unlock( &mutex );
    return ( plasma_read ) { 0, { buf, BUFSIZE } };
}

static plasma_write trywlock(
    plasma * const self,
    plasma_properties const requested
)
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
        || ( BUFSIZE != requested.minimum )
        || ( BUFSIZE != requested.maximum )
        || ( 1U != requested.alignment )
    )
    {
        return ( plasma_write )
        {
            EINVAL,
            { NULL, 0 }
        };
    }
    ( void ) pthread_mutex_lock( &mutex );
    int const result = pthread_rwlock_trywrlock( &rwlock );
    plasma_write const res = ( plasma_write )
        {
            result,
            { NULL, 0 }
        };
    if( 0 != result )
    {
        ( void ) pthread_mutex_unlock( &mutex );
        return res;
    }
    self->unlock = unlock;
    if( self->state->blocking )
    {
        self->read_lock = dummy_rlock;
        self->write_lock = dummy_wlock;
    }
    ( void ) pthread_mutex_unlock( &mutex );
    return ( plasma_write ) { 0, { buf, BUFSIZE } };
}

static plasma_write wlock(
    plasma * const self,
    plasma_properties const requested
)
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
        || ( BUFSIZE != requested.minimum )
        || ( BUFSIZE != requested.maximum )
        || ( 1U != requested.alignment )
    )
    {
        return ( plasma_write )
        {
            EINVAL,
            { NULL, 0 }
        };
    }
    ( void ) pthread_mutex_lock( &mutex );
    int const result = pthread_rwlock_wrlock( &rwlock );
    plasma_write const res = ( plasma_write )
        {
            result,
            { NULL, 0 }
        };
    if( 0 != result )
    {
        ( void ) pthread_mutex_unlock( &mutex );
        return res;
    }
    self->unlock = unlock;
    if( self->state->blocking )
    {
        self->read_lock = dummy_rlock;
        self->write_lock = dummy_wlock;
    }
    ( void ) pthread_mutex_unlock( &mutex );
    return ( plasma_write ) { 0, { buf, BUFSIZE } };
}

static plasma_uid uid( plasma * const self )
{
    UNUSED( self );
    return ( plasma_uid )
    {
        0,
        0U
    };
}

typedef ionize_status ( * auto_func )( void );

static plasma * pp;

ionize_status autotest_allocate( void )
{
    if( pp->state->blocking )
    {
        assert(
            ( dummy_rlock == pp->read_lock )
            || ( rlock == pp->read_lock )
        );
        assert(
            ( dummy_wlock == pp->write_lock )
            || ( wlock == pp->write_lock )
        );
    }
    else
    {
        assert( tryrlock == pp->read_lock );
        assert( trywlock == pp->write_lock );
    }

    switch( rand() % 2 )
    {
        case 0:
        {
            size_t const a = rand();
            size_t const b = rand();
            size_t const c = rand();
            size_t const d = rand();
            plasma_properties const random = { a, b, c };
            ionize_status const result =
                pp->allocate( pp,
                        ( plasma_properties const [] ) { random }, d );
            printf(
                "[%s false (%lu, %lu, %lu, %lu)] status = %d\n",
                __func__,
                a,
                b,
                c,
                d,
                result
            );
            return result;
        }
        case 1:
        {
            plasma_properties const ok = { BUFSIZE, BUFSIZE, 1U };
            ionize_status const result = 
                pp->allocate( pp, ( plasma_properties const[] ) { ok }, 1U );
            printf(
                "[%s true] status = %d\n",
                __func__,
                result
            );
            return result;
        }
        default:
        {
            printf( "[%s ???]\n", __func__ );
            return EINVAL;
        }
    }
}

ionize_status autotest_read( void )
{
    if( pp->state->blocking )
    {
        assert(
            ( dummy_rlock == pp->read_lock )
            || ( rlock == pp->read_lock )
        );
        assert(
            ( dummy_wlock == pp->write_lock )
            || ( wlock == pp->write_lock )
        );
    }
    else
    {
        assert( tryrlock == pp->read_lock );
        assert( trywlock == pp->write_lock );
    }

    switch( rand() % 2 )
    {
        case 0:
        {
            size_t const a = rand();
            size_t const b = rand();
            size_t const c = rand();
            plasma_properties const random = { a, b, c };
            plasma_read const result = pp->read_lock( pp, random );
            printf(
                "[%s false (%lu, %lu, %lu), %sblocking] "
                "status = %d, data = %p, size = %lu\n",
                __func__,
                a,
                b,
                c,
                pp->state->blocking ? "" : "non",
                result.status,
                result.buf.data,
                result.buf.size
            );
            return result.status;
        }
        case 1:
        {
            plasma_properties const ok = { BUFSIZE, BUFSIZE, 1U };
            plasma_read const result = pp->read_lock( pp, ok );
            printf(
                "[%s true, %sblocking] status = %d, data = %p, size = %lu\n",
                __func__,
                pp->state->blocking ? "" : "non",
                result.status,
                result.buf.data,
                result.buf.size
            );
            return result.status;
        }
        default:
        {
            printf( "[%s ???]\n", __func__ );
            return EINVAL;
        }
    }
}

ionize_status autotest_write( void )
{
    if( pp->state->blocking )
    {
        assert(
            ( dummy_rlock == pp->read_lock )
            || ( rlock == pp->read_lock )
        );
        assert(
            ( dummy_wlock == pp->write_lock )
            || ( wlock == pp->write_lock )
        );
    }
    else
    {
        assert( tryrlock == pp->read_lock );
        assert( trywlock == pp->write_lock );
    }

    switch( rand() % 2 )
    {
        case 0:
        {
            size_t const a = rand();
            size_t const b = rand();
            size_t const c = rand();
            plasma_properties const random = { a, b, c };
            plasma_write const result = pp->write_lock( pp, random );
            printf(
                "[%s false (%lu, %lu, %lu), %sblocking] "
                "status = %d, data = %p, size = %lu\n",
                __func__,
                a,
                b,
                c,
                pp->state->blocking ? "" : "non",
                result.status,
                result.buf.data,
                result.buf.size
            );
            return result.status;
        }
        case 1:
        {
            plasma_properties const ok = { BUFSIZE, BUFSIZE, 1U };
            plasma_write const result = pp->write_lock( pp, ok );
            printf(
                "[%s true, %sblocking] status = %d, data = %p, size = %lu\n",
                __func__,
                pp->state->blocking ? "" : "non",
                result.status,
                result.buf.data,
                result.buf.size
            );
            return result.status;
        }
        default:
        {
            printf( "[%s ???]\n", __func__ );
            return EINVAL;
        }
    }
}

ionize_status autotest_unlock( void )
{
    if( pp->state->blocking )
    {
        assert(
            ( dummy_rlock == pp->read_lock )
            || ( rlock == pp->read_lock )
        );
        assert(
            ( dummy_wlock == pp->write_lock )
            || ( wlock == pp->write_lock )
        );
    }
    else
    {
        assert( tryrlock == pp->read_lock );
        assert( trywlock == pp->write_lock );
    }

    ionize_status const result = pp->unlock( pp );
    printf(
        "[%s] status = %d\n",
        __func__,
        result
    );
    return result;
}

ionize_status autotest_blocking( void )
{
    if( pp->state->blocking )
    {
        assert(
            ( dummy_rlock == pp->read_lock )
            || ( rlock == pp->read_lock )
        );
        assert(
            ( dummy_wlock == pp->write_lock )
            || ( wlock == pp->write_lock )
        );
    }
    else
    {
        assert( tryrlock == pp->read_lock );
        assert( trywlock == pp->write_lock );
    }

    switch( rand() % 2 )
    {
        case 0:
        {
            ionize_status const result = pp->blocking( pp, false );
            printf(
                "[%s false] status = %d\n",
                __func__,
                result
            );
            return result;
        }
        case 1:
        {
            ionize_status const result = pp->blocking( pp, true );
            printf(
                "[%s true] status = %d\n",
                __func__,
                result
            );
            return result;
        }
        default:
        {
            printf( "[%s ???]\n", __func__ );
            return EINVAL;
        }
    }
}

ionize_status autotest_uid( void )
{
    if( pp->state->blocking )
    {
        assert(
            ( dummy_rlock == pp->read_lock )
            || ( rlock == pp->read_lock )
        );
        assert(
            ( dummy_wlock == pp->write_lock )
            || ( wlock == pp->write_lock )
        );
    }
    else
    {
        assert( tryrlock == pp->read_lock );
        assert( trywlock == pp->write_lock );
    }

    plasma_uid const result = pp->uid( pp );
    printf(
        "[%s]: status = %d, uid = %"PRIu32"\n",
        __func__,
        result.status,
        result.uid
    );
    return result.status;
}

static auto_func const tests[] = {
    autotest_read,
    autotest_write,
    autotest_unlock,
};
static size_t const testsize = sizeof( tests ) / sizeof( auto_func );

int main( int argc, char ** args )
{
    UNUSED( argc );
    UNUSED( args );

    plasma_state state = { true };
    plasma p = { &state, allocate, rlock, wlock, dummy_unlock, blocking, uid };

    pp = &p;

    srand( time( NULL ));
    uint32_t zeroes = 0;
    uint32_t einvals = 0;
    uint32_t ebusys = 0;
    uint32_t edummies = 0;
    uint32_t others = 0;

    assert( 0 == p.blocking( &p, false ));

    for( uint32_t i = 0U; i < 10000000U; ++i )
    {
        size_t test = rand() % testsize;
        ionize_status const result = ( tests[ test ] )();
        sync();
        switch( result )
        {
            case 0:
            {
                ++zeroes;
                break;
            }
            case EINVAL:
            {
                ++einvals;
                break;
            }
            case EBUSY:
            {
                ++ebusys;
                break;
            }
            case EDUMMY:
            {
                ++edummies;
                break;
            }
            default:
            {
                ++others;
                break;
            }
        }
    }

    pp = NULL;

    fprintf(
        stderr,
        "results:\n\tsuccesses = %"PRIu32"\n\teinvals = %"PRIu32
        "\n\tebusys = %"PRIu32"\n\tedummies = %"PRIu32
        "\n\tothers (investigate) = %"PRIu32"\n",
        zeroes,
        einvals,
        ebusys,
        edummies,
        others
    );

    return 0;
}

