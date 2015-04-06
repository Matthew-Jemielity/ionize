/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Tests basic logging functionality.
 * \date        04/06/2015 04:10:54 AM
 * \file        test_log_01.c
 * \version     1.0
 *
 *
 **/

#include <assert.h>
#include <ionize/log.h>
#include <ionize/log/stderr.h>
#include <ionize/universal.h>

int main( int argc, char * args[] )
{
    UNUSED( argc );
    UNUSED( args );

    ionize_log_setup_result log = ionize_log_setup();
    assert( 0 == log.status );
    assert( 0 == log.log->add( log.log, ionize_log_stderr ));
    IONIZE_ERROR( "test" );
    assert( 0 == ionize_log_cleanup( log.log ));
    return 0;
}

