/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Test for P_NARG utility macro.
 * \date        07/03/2015 01:54:41 AM
 * \file        test_macros_pp_narg_01.c
 * \version     1.0
 *
 *
 **/
#include <assert.h>
#include <plasma/macros.h>

int main( int argc, char ** args )
{
    UNUSED( argc );
    UNUSED( args );

    assert( 0 == PP_NARG() );
    assert( 1 == PP_NARG( a ));
    assert( 2 == PP_NARG( a, b ));

    assert( 2 == PP_NARG( , ));
    assert( 2 == PP_NARG( a, ));

    return 0;
}

