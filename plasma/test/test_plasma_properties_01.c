/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Tests plasma_properties_validator.
 * \date        03/29/2015 06:36:16 AM
 * \file        test_plasma_properties_01.c
 * \version     1.0
 *
 *
 **/

#include <assert.h>
#include <errno.h>
#include <ionize/error.h>
#include <ionize/universal.h>
#include <plasma/properties.h>

int main( int argc, char * args[] )
{
    UNUSED( argc );
    UNUSED( args );

    assert( EINVAL == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 0U } ));
    assert( 0 == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 1U } ));
    assert( 0 == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 2U } ));
    assert( ENOTSUP == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 3U } ));
    assert( 0 == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 4U } ));
    assert( ENOTSUP == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 5U } ));
    assert( ENOTSUP == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 6U } ));
    assert( ENOTSUP == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 7U } ));
    assert( 0 == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 8U } ));
    assert( ENOTSUP == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 9U } ));
    assert( ENOTSUP == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 10U } ));
    assert( ENOTSUP == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 11U } ));
    assert( ENOTSUP == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 12U } ));
    assert( ENOTSUP == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 13U } ));
    assert( ENOTSUP == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 14U } ));
    assert( ENOTSUP == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 15U } ));
    assert( 0 == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 16U } ));
    assert( E2BIG == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 17U } ));
    assert( ERANGE == plasma_properties_validator(
                ( plasma_properties ) { 2U, 1U, 1U } ));
    assert( EINVAL == plasma_properties_validator(
                ( plasma_properties ) { 0U, 1U, 1U } ));
    assert( EINVAL == plasma_properties_validator(
                ( plasma_properties ) { 1U, 0U, 1U } ));
    assert( EINVAL == plasma_properties_validator(
                ( plasma_properties ) { 0U, 0U, 1U } ));
    assert( 0 == plasma_properties_validator(
                ( plasma_properties ) { 1U, 1U, 1U } ));

    return 0;
}

