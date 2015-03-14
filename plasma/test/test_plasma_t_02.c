/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Mock implementation of plasma_t structure functions.
 * \date        03/08/2015 07:26:10 AM
 * \file        test_plasma_t_01.c
 * \version     1.0
 *
 *
 **/
#define __STDC_FORMAT_MACROS
#define _XOPEN_SOURCE 500

#include <assert.h>
#include <inttypes.h>
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

struct plasma_state
{
    bool blocking;
};

static plasma_status_t allocate(
    plasma_t * const restrict self,
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

static plasma_status_t blocking( plasma_t * const self, bool const state )
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

static plasma_status_t dummy_unlock( plasma_t * const self )
{
    UNUSED( self );
    return EINVAL;
}

static plasma_read_result_t rlock( plasma_t * const self );
static plasma_write_result_t wlock( plasma_t * const self );
static plasma_read_result_t tryrlock( plasma_t * const self );
static plasma_write_result_t trywlock( plasma_t * const self );
static plasma_status_t unlock( plasma_t * const self )
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

static plasma_read_result_t dummy_rlock( plasma_t * const self )
{
    UNUSED( self );
    return ( plasma_read_result_t )
    {
        EINVAL,
        { NULL, 0 }
    };
}

static plasma_write_result_t dummy_wlock( plasma_t * const self )
{
    UNUSED( self );
    return ( plasma_write_result_t )
    {
        EINVAL,
        { NULL, 0 }
    };
}

static plasma_read_result_t tryrlock( plasma_t * const self )
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
    )
    {
        return ( plasma_read_result_t )
        {
            EINVAL,
            { NULL, 0 }
        };
    }
    ( void ) pthread_mutex_lock( &mutex );
    int const result = pthread_rwlock_tryrdlock( &rwlock );
    plasma_read_result_t const res = ( plasma_read_result_t )
        {
            result,
            { NULL, 0 }
        };
    if( 0 != result )
    {
        ( void ) pthread_mutex_unlock( &mutex );
        return res;
    }
    self->read_lock = dummy_rlock;
    self->write_lock = dummy_wlock;
    self->unlock = unlock;
    ( void ) pthread_mutex_unlock( &mutex );
    return ( plasma_read_result_t ) { 0, { buf, BUFSIZE } };
}

static plasma_read_result_t rlock( plasma_t * const self )
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
    )
    {
        return ( plasma_read_result_t )
        {
            EINVAL,
            { NULL, 0 }
        };
    }
    ( void ) pthread_mutex_lock( &mutex );
    int const result = pthread_rwlock_rdlock( &rwlock );
    plasma_read_result_t const res = ( plasma_read_result_t )
        {
            result,
            { NULL, 0 }
        };
    if( 0 != result )
    {
        ( void ) pthread_mutex_unlock( &mutex );
        return res;
    }
    self->read_lock = dummy_rlock;
    self->write_lock = dummy_wlock;
    self->unlock = unlock;
    ( void ) pthread_mutex_unlock( &mutex );
    return ( plasma_read_result_t ) { 0, { buf, BUFSIZE } };
}

static plasma_write_result_t trywlock( plasma_t * const self )
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
    )
    {
        return ( plasma_write_result_t )
        {
            EINVAL,
            { NULL, 0 }
        };
    }
    ( void ) pthread_mutex_lock( &mutex );
    int const result = pthread_rwlock_trywrlock( &rwlock );
    plasma_write_result_t const res = ( plasma_write_result_t )
        {
            result,
            { NULL, 0 }
        };
    if( 0 != result )
    {
        ( void ) pthread_mutex_unlock( &mutex );
        return res;
    }
    self->read_lock = dummy_rlock;
    self->write_lock = dummy_wlock;
    self->unlock = unlock;
    ( void ) pthread_mutex_unlock( &mutex );
    return ( plasma_write_result_t ) { 0, { buf, BUFSIZE } };
}

static plasma_write_result_t wlock( plasma_t * const self )
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
    )
    {
        return ( plasma_write_result_t )
        {
            EINVAL,
            { NULL, 0 }
        };
    }
    ( void ) pthread_mutex_lock( &mutex );
    int const result = pthread_rwlock_wrlock( &rwlock );
    plasma_write_result_t const res = ( plasma_write_result_t )
        {
            result,
            { NULL, 0 }
        };
    if( 0 != result )
    {
        ( void ) pthread_mutex_unlock( &mutex );
        return res;
    }
    self->read_lock = dummy_rlock;
    self->write_lock = dummy_wlock;
    self->unlock = unlock;
    ( void ) pthread_mutex_unlock( &mutex );
    return ( plasma_write_result_t ) { 0, { buf, BUFSIZE } };
}

static plasma_uid_t uid( plasma_t * const self )
{
    UNUSED( self );
    return ( plasma_uid_t )
    {
        0,
        0
    };
}

typedef void ( * auto_func_t )( void );

static plasma_t * pp;

void autotest_allocate( void )
{
    switch( rand() % 2 )
    {
        case 0:
        {
            size_t const a = rand();
            size_t const b = rand();
            printf(
                "[%s false (%lu, %lu)] status = %d\n",
                __func__,
                a,
                b,
                pp->allocate( pp, ( size_t const [] ) { a }, b )
            );
            break;
        }
        case 1:
        {
            printf(
                "[%s true] status = %d\n",
                __func__,
                pp->allocate( pp, ( size_t[] ) { BUFSIZE }, 1U )
            );
            break;
        }
        default:
        {
            printf( "[%s ???]\n", __func__ );
            break;
        }
    }
}

void autotest_read( void )
{
    plasma_read_result_t result = pp->read_lock( pp );
    printf(
        "[%s] status = %d, buf = %p, size = %lu\n",
        __func__,
        result.status,
        result.buf.buf,
        result.buf.size
    );
}

void autotest_write( void )
{
    plasma_write_result_t result = pp->write_lock( pp );
    printf(
        "[%s] status = %d, buf = %p, size = %lu\n",
        __func__,
        result.status,
        result.buf.buf,
        result.buf.size
    );
}

void autotest_unlock( void )
{
    printf(
        "[%s] status = %d\n",
        __func__,
        pp->unlock( pp )
    );
}

void autotest_blocking( void )
{
    switch( rand() % 2 )
    {
        case 0:
        {
            printf(
                "[%s false] status = %d\n",
                __func__,
                pp->blocking( pp, false )
            );
            break;
        }
        case 1:
        {
            printf(
                "[%s true] status = %d\n",
                __func__,
                pp->blocking( pp, true )
            );
            break;
        }
        default:
        {
            printf( "[%s ???]\n", __func__ );
            break;
        }
    }
}

void autotest_uid( void )
{
    plasma_uid_t result = pp->uid( pp );
    printf(
        "[%s]: status = %d, uid = %"PRIu32"\n",
        __func__,
        result.status,
        result.uid
    );
}

static auto_func_t const tests[] = {
    autotest_allocate,
    autotest_read,
    autotest_write,
    autotest_unlock,
    autotest_blocking,
    autotest_uid
};
static size_t const testsize = sizeof( tests ) / sizeof( auto_func_t );

int main( int argc, char ** args )
{
    UNUSED( argc );
    UNUSED( args );

    plasma_state_t state = { true };
    plasma_t p = { &state, allocate, rlock, wlock, dummy_unlock, blocking, uid };

    pp = &p;

    srand( time( NULL ));
    for( uint32_t i = 0U; i < 10000000U; ++i )
    {
        size_t test = rand() % testsize;
        ( tests[ test ] )();
    }

    pp = NULL;
    return 0;
}
