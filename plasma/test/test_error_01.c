/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Tests plasma_status_t.
 * \date        03/08/2015 06:09:49 AM
 * \file        test_error_01.c
 * \version     1.0
 *
 *
 **/
#include <plasma/error.h>
#include <plasma/macros.h>
#include <stdio.h>
#include <string.h>
plasma_status_t foo( void )
{
    return EINVAL;
}

int main( int argc, char ** args )
{
    UNUSED( argc );
    UNUSED( args );
    char const * string = strerror( foo() );
    printf( "Error string is: %s\n", string );
    return 0;
}

