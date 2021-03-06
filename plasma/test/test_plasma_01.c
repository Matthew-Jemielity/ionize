/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Mock implementation of plasma structure functions.
 * \date        03/08/2015 07:26:10 AM
 * \file        test_plasma_01.c
 * \version     1.0
 *
 *
 **/
#define _XOPEN_SOURCE 500

#include <assert.h>
#include <ionize/universal.h>
#include <plasma/macros.h>
#include <plasma/plasma.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define BUFSIZE 10
#define EDUMMY (( int ) 0xC0FFEEEE)

static uint8_t buf[ BUFSIZE ]; /* inited to zeroes */
static pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef enum
{
    NONE,
    READER,
    WRITER
}
owner;

struct plasma_state_struct
{
    bool blocking;
    uint32_t inside_critical_section;
    owner current;
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

static ionize_status dummy_unlock( plasma * const self )
{
    UNUSED( self );
    return EINVAL;
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

static ionize_status blocking( plasma * const self, bool const state )
{
    if(
        ( NULL == self )
        || ( NULL == self->state )
    )
    {
        return EINVAL;
    }
    UNUSED( pthread_mutex_lock( &mutex ));
    self->state->blocking = state;
    switch( self->state->current )
    {
        case NONE:
        {
            self->read_lock = ( self->state->blocking ) ? rlock : tryrlock;
            self->write_lock = ( self->state->blocking ) ? wlock : trywlock;
            break;
        }
        case READER:
        {
            self->read_lock = ( self->state->blocking ) ? rlock : tryrlock;
            self->write_lock = dummy_wlock;
            break;
        }
        case WRITER:
        {
            self->read_lock = dummy_rlock;
            self->write_lock = ( self->state->blocking ) ? wlock : trywlock;
            break;
        }
        default:
        {
            UNUSED( pthread_mutex_unlock( &mutex ));
            return EINVAL;
        }
    }
    UNUSED( pthread_mutex_unlock( &mutex ));
    return 0;
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
    UNUSED( pthread_mutex_lock( &mutex ));
    int const result = pthread_rwlock_unlock( &rwlock );
    if( 0 == result )
    {
        --( self->state->inside_critical_section );
        if( 0U == self->state->inside_critical_section )
        {
            self->state->current = NONE;
            self->unlock = dummy_unlock;
            if( dummy_rlock == self->read_lock )
            {
                /* writer quit, can read */
                self->read_lock = ( self->state->blocking ) ? rlock : tryrlock;
            }
            if( dummy_wlock == self->write_lock )
            {
                /* all readers quit, can write */
                self->write_lock = ( self->state->blocking ) ? wlock : trywlock;
            }
        }
    }
    UNUSED( pthread_mutex_unlock( &mutex ));
    return result;
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
    self->state->current = READER;
    ++( self->state->inside_critical_section );
    self->unlock = unlock;
    if( self->state->blocking )
    {
        self->write_lock = dummy_wlock;
    }
    UNUSED( pthread_mutex_unlock( &mutex ));
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
    self->state->current = READER;
    ++( self->state->inside_critical_section );
    self->unlock = unlock;
    if( self->state->blocking )
    {
        self->write_lock = dummy_wlock;
    }
    UNUSED( pthread_mutex_unlock( &mutex ));
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
    self->state->current = WRITER;
    ++( self->state->inside_critical_section );
    self->unlock = unlock;
    if( self->state->blocking )
    {
        self->read_lock = dummy_rlock;
        self->write_lock = dummy_wlock;
    }
    UNUSED( pthread_mutex_unlock( &mutex ));
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
    self->state->current = WRITER;
    ++( self->state->inside_critical_section );
    self->unlock = unlock;
    if( self->state->blocking )
    {
        self->read_lock = dummy_rlock;
        self->write_lock = dummy_wlock;
    }
    UNUSED( pthread_mutex_unlock( &mutex ));
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

int main( int argc, char ** args )
{
    UNUSED( argc );
    UNUSED( args );

    plasma_state state = { true, 0, NONE };
    plasma p = { &state, allocate, rlock, wlock, dummy_unlock, blocking, uid };

    plasma_properties const invalid = { 0U, 0U, 0U };
    plasma_properties const valid = { BUFSIZE, BUFSIZE, 1U };

    assert( 0 == p.blocking( &p, false ));
    assert( 0 == p.write_lock( &p, valid ).status );
    assert( EINVAL == p.read_lock( &p, invalid ).status );
    assert( 0 == p.unlock( &p ));
    assert( 0 == p.write_lock( &p, valid ).status );
    assert( 0 == p.unlock( &p ));
    assert( EINVAL == p.read_lock( &p, invalid ).status );
    assert( EINVAL == p.unlock( &p ));
    assert( 0 == p.write_lock( &p, valid ).status );
    assert( EBUSY == p.write_lock( &p, valid ).status );
    assert( 0 == p.unlock( &p ));

    assert( EINVAL == p.allocate( &p, ( plasma_properties[] ) {
                { 2U, 3U, 4U },
                { BUFSIZE, BUFSIZE - 1, 0U }
    }, 2U ));
    assert( EINVAL == p.allocate( NULL, NULL, 0U ));
    assert( EINVAL == p.allocate( &p,
                ( plasma_properties[] ) { { 1U, 1U, 1U } }, 1U ));
    assert( 0 == p.allocate( &p,
                ( plasma_properties[] ) { { BUFSIZE, BUFSIZE, 1U } }, 1U ));

    assert( EINVAL == p.unlock( &p ));
    assert( dummy_unlock == p.unlock );

    plasma_write wr;
    assert( EINVAL == p.write_lock( NULL, valid ).status );
    assert( 0 == ( wr = p.write_lock( &p, valid )).status );
    assert( BUFSIZE == wr.buf.size );
    (( char * )( wr.buf.data ))[ 1 ] = 42;
    assert( unlock == p.unlock );
    assert( EINVAL == p.unlock( NULL ));

    assert( 0 == p.blocking( &p, false ));
    assert( dummy_rlock == p.read_lock );
    assert( EDUMMY == p.read_lock( &p, valid ).status );
    assert( 0 == p.blocking( &p, true ));

    assert( 0 == p.unlock( &p ));
    assert( dummy_unlock == p.unlock );

    plasma_read rr;
    assert( EINVAL == p.read_lock( NULL, valid ).status );
    assert( EINVAL == ( rr = p.read_lock( &p, invalid )).status );
    assert( 0 == ( rr = p.read_lock( &p, valid )).status );
    assert( BUFSIZE == rr.buf.size );
    assert( 42 == (( char * )( rr.buf.data ))[ 1 ] );
    assert( 0 == p.unlock( &p ));

    assert( 0 == p.blocking( &p, false ));
    assert( false == state.blocking );
    assert( 0 == p.read_lock( &p, valid ).status );
    assert( trywlock == p.write_lock );
    assert( EBUSY == p.write_lock( &p, valid ).status );
    assert( 0 == p.unlock( &p ));

    assert( 0 == p.blocking( &p, false ));
    assert( false == state.blocking );
    assert( 0 == p.write_lock( &p, valid ).status );
    assert( trywlock == p.write_lock );
    assert( EBUSY == p.write_lock( &p, valid ).status );
    assert( 0 == p.unlock( &p ));

    assert( 0 == p.blocking( &p, false ));
    assert( false == state.blocking );
    assert( 0 == p.write_lock( &p, valid ).status );
    assert( tryrlock == p.read_lock );
    assert( EBUSY == p.read_lock( &p, valid ).status );
    assert( 0 == p.unlock( &p ));

    assert( 0 == p.blocking( &p, false ));
    assert( false == state.blocking );
    assert( 0 == p.read_lock( &p, valid ).status );
    assert( tryrlock == p.read_lock );
    /* multiple readers are allowed */
    assert( 0 == p.read_lock( &p, valid ).status );
    assert( 0 == p.unlock( &p ));
    assert( unlock == p.unlock ); /* still have to unlock */
    assert( 0 == p.unlock( &p ));

    assert( 0 == p.blocking( &p, true ));
    assert( 0 == p.read_lock( &p, valid ).status );
    assert( 0 == p.read_lock( &p, valid ).status );
    assert( 0 == p.unlock( &p ));
    assert( 0 == p.unlock( &p ));

    assert( EINVAL == p.unlock( &p ));
    assert( 0 == p.blocking( &p, false ));
    assert( 0 == p.read_lock( &p, valid ).status );
    assert( trywlock == p.write_lock );
    assert( 0 == p.read_lock( &p, valid ).status );
    assert( 0 == p.unlock( &p ));
    assert( trywlock == p.write_lock );
    assert( 0 == p.blocking( &p, true ));
    assert( dummy_wlock == p.write_lock );
    assert( EDUMMY == p.write_lock( &p, valid ).status );
    assert( 0 == p.unlock( &p ));

    assert( dummy_unlock == p.unlock );

    return 0;
}

