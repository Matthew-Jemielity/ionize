/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Tests ionize_status.
 * \date        03/08/2015 06:09:49 AM
 * \file        test_error_01.c
 * \version     1.0
 *
 *
 **/

#include <assert.h>
#include <ionize/error.h>
#include <ionize/universal.h>
#include <stdio.h>
#include <string.h>

static inline ionize_status foo( void )
{
    return EINVAL;
}

int main( int argc, char ** args )
{
    UNUSED( argc );
    UNUSED( args );
    assert( EINVAL == foo() );
    char const * string = strerror( foo() );
    printf( "Error string is: %s\n", string );
    return 0;
}

