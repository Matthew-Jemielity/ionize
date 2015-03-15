/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Definitions of global methods.
 * \date        03/15/2015 04:31:43 AM
 * \file        universal.c
 * \version     1.0
 *
 *
 **/

#include <ionize/universal.h>
#include <stdint.h>

/*
 * I wanted to choose a port with number chosen according to "ionic"
 * nomenclature used throught the project. Having only two-byte value
 * greater than 1023 and also not some well-known port makes this a
 * little bit difficult. I ended up having to explain the number:
 * 'I' == 0x49
 * 'O' == 79U
 * 'N' == 'O' - 1
 * I'm open to interesting ideas. : )
 */
uint16_t ionize_port( void )
{
    return 49791U;
}

