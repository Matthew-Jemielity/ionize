/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Tests for ionize_pointer_list
 * \date        04/03/2015 07:34:45 AM
 * \file        test_pointer_list_01.c
 * \version     1.0
 *
 *
 **/

#include <assert.h>
#include <errno.h>
#include <ionize/error.h>
#include <ionize/pointer_list.h>
#include <ionize/universal.h>
#include <stddef.h>
#include <stdio.h>

ionize_status callback( void * const pointer, void * const userdata )
{
    assert( NULL == userdata );
    printf( "%p\n", pointer );
    return 0;
}

int main( int argc, char * args[] )
{
    UNUSED( argc );
    UNUSED( args );

    int data = 23;
    ionize_pointer_list list = ionize_pointer_list_setup();

    assert( NULL == list.head );
    assert( ENODATA == list.remove( &list, NULL ));
    assert( EINVAL == ionize_pointer_list_cleanup( NULL ));
    assert( 0 == ionize_pointer_list_cleanup( &list ));
    assert( 0 == list.add( &list, &data ));
    assert( NULL != list.head );
    assert( data == *(( int * ) list.head->pointer ));
    assert( ENODATA == list.remove( &list, NULL ));
    assert( EEXIST == list.add( &list, &data ));
    assert( 0 == list.add( &list, NULL ));
    assert( 0 == list.remove( &list, NULL ));
    assert( 0 == list.add( &list, NULL ));
    assert( 0 == list.foreach( &list, callback, NULL ));
    assert( 0 == list.remove( &list, &data ));
    assert( NULL == list.head->pointer );
    assert( 0 == list.add( &list, &data ));
    assert( data = *(( int* ) list.head->next->pointer ));
    assert( 0 == ionize_pointer_list_cleanup( &list ));
    assert( NULL == list.head );
    return 0;
}

