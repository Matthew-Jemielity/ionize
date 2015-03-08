/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Definition of plasma error status.
 * \date        03/08/2015 05:58:38 AM
 * \file        error.h
 * \version     1.0
 *
 * Error status is errno-compatible, which means zero is success and
 * non-zero positive number is errno code, like ENOMEM, EINVAL, etc.
 * Non-zero negative will not occur, but we use signed type to be
 * compatible with errno type. Compatibility means functions like
 * strerror() can be used on error codes returned by plasma library.
 **/
#ifndef PLASMA_ERROR_H__
# define PLASMA_ERROR_H__
# include <errno.h>

typedef int plasma_status_t;

#endif /* PLASMA_ERROR_H__ */

