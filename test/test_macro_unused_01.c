/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Tests UNUSED utility macro
 * \date        03/06/2015 05:04:08 PM
 * \file        test_macros_unused_01.c
 * \version     1.0
 *
 * Test should be compiled with -std=c11 -Wall -Wextra -pedantic -Werror
 **/

#include <ionize/universal.h>

int main( int argc, char ** args )
{
    UNUSED( argc );
    UNUSED( args );
    return 0;
}

