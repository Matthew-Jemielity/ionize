/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Mock implementation of plasma_t structure functions.
 * \date        03/08/2015 07:26:10 AM
 * \file        test_plasma_t_01.c
 * \version     1.0
 *
 *
 **/
#define _XOPEN_SOURCE 500

#include <assert.h>
#include <plasma/macros.h>
#include <plasma/plasma.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

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

int main( int argc, char ** args )
{
    UNUSED( argc );
    UNUSED( args );

    plasma_state_t state = { true };
    plasma_t p = { &state, allocate, rlock, wlock, dummy_unlock, blocking, uid };

    assert( 0 != p.allocate( &p, ( size_t[] ) { 2, 3 }, 2 ));
    assert( 0 != p.allocate( NULL, NULL, 0 ));
    assert( 0 != p.allocate( &p, ( size_t[] ) { 1 }, 1 ));
    assert( 0 == p.allocate( &p, ( size_t[] ) { BUFSIZE }, 1 ));

    assert( 0 != p.unlock( &p ));
    assert( dummy_unlock == p.unlock );

    plasma_write_result_t wr;
    assert( 0 != p.write_lock( NULL ).status );
    assert( 0 == ( wr = p.write_lock( &p )).status );
    assert( BUFSIZE == wr.buf.size );
    (( char * )( wr.buf.buf ))[ 1 ] = 42;
    assert( unlock == p.unlock );
    assert( 0 != p.unlock( NULL ));
    assert( 0 == p.unlock( &p ));
    assert( dummy_unlock == p.unlock );

    plasma_read_result_t rr;
    assert( 0 != p.read_lock( NULL ).status );
    assert( 0 == ( rr = p.read_lock( &p )).status );
    assert( dummy_rlock == p.read_lock );
    assert( dummy_wlock == p.write_lock );
    assert( BUFSIZE == rr.buf.size );
    assert( 42 == (( char * )( rr.buf.buf ))[ 1 ] );
    assert( 0 == p.unlock( &p ));
    assert( 0 == p.blocking( &p, false ));
    assert( false == state.blocking );

    return 0;
}
