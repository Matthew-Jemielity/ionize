/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Tests ionize_mutex.
 * \date        04/03/2015 06:12:46 PM
 * \file        test_mutex_01.c
 * \version     1.0
 *
 * Uses pthreads.
 **/

#include <assert.h>
#include <ionize/mutex.h>
#include <ionize/universal.h>
#include <pthread.h>
#include <stddef.h>

#define ITERATIONS 100000000U
#define THREADS 4U

typedef struct
{
    ionize_mutex mutex;
    unsigned int data;
}
test_struct;

void * func( void * ptr )
{
    test_struct * t = ptr;
    for( unsigned int i = 0; i < ITERATIONS; ++i )
    {
        assert( 0 == t->mutex.lock( t->mutex ));
        ++( t->data );
        assert( 0 == t->mutex.unlock( t->mutex ));
    }
    return NULL;
}

int main( int argc, char * args[] )
{
    UNUSED( argc );
    UNUSED( args );

    pthread_t threads[ THREADS ];

    ionize_mutex_setup_result setup = ionize_mutex_setup();
    assert( 0 == setup.status );

    test_struct t = { .mutex = setup.mutex, .data = 0U };
    for( unsigned int i = 0; i < THREADS; ++i )
    {
        assert( 0 == pthread_create( &( threads[ i ] ), NULL, func, &t ));
    }
    for( unsigned int i = 0; i < THREADS; ++i )
    {
        assert( 0 == pthread_join( threads[ i ], NULL ));
    }
    assert(( THREADS * ITERATIONS ) == t.data );
    assert( 0 == ionize_mutex_cleanup( &( setup.mutex )));

    return 0;
}

