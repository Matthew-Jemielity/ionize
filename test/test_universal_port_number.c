/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Test for ionize_port().
 * \date        03/15/2015 05:04:06 AM
 * \file        test_universal_port_number.c
 * \version     1.0
 *
 *
 **/

#define __STDC_FORMAT_MACROS

#include <assert.h>
#include <inttypes.h>
#include <ionize/universal.h>
#include <stdint.h>
#include <stdio.h>

int main( int argc, char ** args )
{
    UNUSED( argc );
    UNUSED( args );

    uint16_t const port = ionize_port();
    assert( 1023U < port );
    assert( port < UINT16_MAX );
    assert( port != UINT16_MAX );
    printf( "port is %"PRIu16"\n", port );
    if(( 1023U < port ) && ( port < 49152U ))
    {
        puts( "port in registered ports range" );
    }
    else if( 49151U < port )
    {
        puts( "port in dynamic/private port range" );
    }
    return 0;
}

