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

#define BUFSIZE 10

static uint8_t buf[ BUFSIZE ]; /* inited to zeores */
static pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct plasma_state_struct
{
    bool blocking;
};

static ionize_status allocate(
    plasma * const restrict self,
    size_t const * const restrict sizes,
    size_t const length
)
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
        || ( NULL == sizes )
        || ( length != 1 )
        || ( BUFSIZE != sizes[ 0 ] )
    )
    {
        return EINVAL;
    }
    return 0;
}

static ionize_status blocking( plasma * const self, bool const state )
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
    )
    {
        return EINVAL;
    }
    self->state->blocking = state;
    return 0;
}

static ionize_status dummy_unlock( plasma * const self )
{
    UNUSED( self );
    return EINVAL;
}

static plasma_read rlock( plasma * const self );
static plasma_write wlock( plasma * const self );
static plasma_read tryrlock( plasma * const self );
static plasma_write trywlock( plasma * const self );
static ionize_status unlock( plasma * const self )
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
    )
    {
        return EINVAL;
    }
    UNUSED( pthread_mutex_lock( &mutex ));
    int const result = pthread_rwlock_unlock( &rwlock );
    if( 0 == result )
    {
        self->unlock = dummy_unlock;
        self->read_lock = ( self->state->blocking ) ? rlock : tryrlock;
        self->write_lock = ( self->state->blocking ) ? wlock : trywlock;
    }
    UNUSED( pthread_mutex_unlock( &mutex ));
    return result;
}

static plasma_read dummy_rlock( plasma * const self )
{
    UNUSED( self );
    return ( plasma_read )
    {
        EINVAL,
        { NULL, 0 }
    };
}

static plasma_write dummy_wlock( plasma * const self )
{
    UNUSED( self );
    return ( plasma_write )
    {
        EINVAL,
        { NULL, 0 }
    };
}

static plasma_read tryrlock( plasma * const self )
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
    )
    {
        return ( plasma_read )
        {
            EINVAL,
            { NULL, 0 }
        };
    }
    UNUSED( pthread_mutex_lock( &mutex ));
    int const result = pthread_rwlock_tryrdlock( &rwlock );
    plasma_read const res = ( plasma_read )
        {
            result,
            { NULL, 0 }
        };
    if( 0 != result )
    {
        UNUSED( pthread_mutex_unlock( &mutex ));
        return res;
    }
    self->read_lock = dummy_rlock;
    self->write_lock = dummy_wlock;
    self->unlock = unlock;
    UNUSED( pthread_mutex_unlock( &mutex ));
    return ( plasma_read ) { 0, { buf, BUFSIZE } };
}

static plasma_read rlock( plasma * const self )
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
    )
    {
        return ( plasma_read )
        {
            EINVAL,
            { NULL, 0 }
        };
    }
    UNUSED( pthread_mutex_lock( &mutex ));
    int const result = pthread_rwlock_rdlock( &rwlock );
    plasma_read const res = ( plasma_read )
        {
            result,
            { NULL, 0 }
        };
    if( 0 != result )
    {
        UNUSED( pthread_mutex_unlock( &mutex ));
        return res;
    }
    self->read_lock = dummy_rlock;
    self->write_lock = dummy_wlock;
    self->unlock = unlock;
    UNUSED( pthread_mutex_unlock( &mutex ));
    return ( plasma_read ) { 0, { buf, BUFSIZE } };
}

static plasma_write trywlock( plasma * const self )
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
    )
    {
        return ( plasma_write )
        {
            EINVAL,
            { NULL, 0 }
        };
    }
    UNUSED( pthread_mutex_lock( &mutex ));
    int const result = pthread_rwlock_trywrlock( &rwlock );
    plasma_write const res = ( plasma_write )
        {
            result,
            { NULL, 0 }
        };
    if( 0 != result )
    {
        UNUSED( pthread_mutex_unlock( &mutex ));
        return res;
    }
    self->read_lock = dummy_rlock;
    self->write_lock = dummy_wlock;
    self->unlock = unlock;
    UNUSED( pthread_mutex_unlock( &mutex ));
    return ( plasma_write ) { 0, { buf, BUFSIZE } };
}

static plasma_write wlock( plasma * const self )
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
    )
    {
        return ( plasma_write )
        {
            EINVAL,
            { NULL, 0 }
        };
    }
    UNUSED( pthread_mutex_lock( &mutex ));
    int const result = pthread_rwlock_wrlock( &rwlock );
    plasma_write const res = ( plasma_write )
        {
            result,
            { NULL, 0 }
        };
    if( 0 != result )
    {
        UNUSED( pthread_mutex_unlock( &mutex ));
        return res;
    }
    self->read_lock = dummy_rlock;
    self->write_lock = dummy_wlock;
    self->unlock = unlock;
    UNUSED( pthread_mutex_unlock( &mutex ));
    return ( plasma_write ) { 0, { buf, BUFSIZE } };
}

static plasma_uid uid( plasma * const self )
{
    UNUSED( self );
    return ( plasma_uid )
    {
        0,
        0
    };
}

typedef ionize_status ( * auto_func )( void );

static plasma * pp;

ionize_status autotest_allocate( void )
{
    switch( rand() % 2 )
    {
        case 0:
        {
            size_t const a = rand();
            size_t const b = rand();
            ionize_status const result =
                pp->allocate( pp, ( size_t const [] ) { a }, b );
            printf(
                "[%s false (%lu, %lu)] status = %d\n",
                __func__,
                a,
                b,
                result
            );
            return result;
        }
        case 1:
        {
            ionize_status const result = 
                pp->allocate( pp, ( size_t[] ) { BUFSIZE }, 1U );
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
    plasma_read const result = pp->read_lock( pp );
    printf(
        "[%s] status = %d, data = %p, size = %lu\n",
        __func__,
        result.status,
        result.buf.data,
        result.buf.size
    );
    return result.status;
}

ionize_status autotest_write( void )
{
    plasma_write const result = pp->write_lock( pp );
    printf(
        "[%s] status = %d, data = %p, size = %lu\n",
        __func__,
        result.status,
        result.buf.data,
        result.buf.size
    );
    return result.status;
}

ionize_status autotest_unlock( void )
{
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
    autotest_allocate,
    autotest_read,
    autotest_write,
    autotest_unlock,
    autotest_blocking,
    autotest_uid
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
    uint32_t others = 0;
    for( uint32_t i = 0U; i < 10000000U; ++i )
    {
        size_t test = rand() % testsize;
        ionize_status const result = ( tests[ test ] )();
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
        "results:\n\tsuccesses = %"PRIu32"\n\teinvals = %"PRIu32"\n"
        "\tothers (investigate) = %"PRIu32"\n",
        zeroes,
        einvals,
        others
    );

    return 0;
}
