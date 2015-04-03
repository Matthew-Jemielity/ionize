/**
 * \author      Mateusz Jemielity matthew.jemielity@gmail.com
 * \brief       Defines API for linked list of pointers.
 * \date        04/03/2015 04:39:54 AM
 * \file        pointer_list.h
 * \version     1.0
 *
 *
 **/

#ifndef IONIZE_POINTER_LIST_H__
# define IONIZE_POINTER_LIST_H__

# include <ionize/error.h> /* ionize_status */

/**
 * \brief Forward definition of ionize_pointer_list element.
 */
typedef struct ionize_pointer_list_element_struct ionize_pointer_list_element;

/**
 * \brief Definition of ionize_pointer_list element.
 */
struct ionize_pointer_list_element_struct
{
    void * pointer; /** Pointer held in the list. */
    ionize_pointer_list_element * next; /** Pointer to next list element. */
};

/*
 * \brief Forward declaration of ionize_pointer_list.
 */
typedef struct ionize_pointer_list_struct ionize_pointer_list;

/**
 * \brief Common list operation function type.
 * \param self ionize_pointer_list on which we'll operate.
 * \param pointer Pointer with which the operation will be done.
 *
 * Error codes possible:
 * 1. Common:
 * a) EINVAL - if invalid self pointer is given.
 * 2. When adding a pointer:
 * a) ENOMEM - memory allocation for list element failed;
 * b) EEXIST - given pointer already exists in the list.
 * 3. When removing a pointer:
 * a) ENODATA - given pointer doesn't exist in the list.
 */
typedef ionize_status ( * ionize_pointer_list_func )(
    ionize_pointer_list * const self,
    void * const pointer
);

/**
 * \brief Definition of ionize_pointer_list.
 *
 * The list holds unique pointers to arbitrary data. Adding a pointer will
 * first check whether such pointer exists.
 */
struct ionize_pointer_list_struct
{
    ionize_pointer_list_element * head; /** Pointer to first list element */
    ionize_pointer_list_func const add; /** Adds pointer to the list. */
    ionize_pointer_list_func const remove; /** Removes existing pointer. */
};

/**
 * \brief Sets up a ionize_pointer_list.
 * \return A ionize_pointer_list object.
 *
 * Head is initialized to NULL. This method will not fail.
 */
ionize_pointer_list ionize_pointer_list_setup( void );

/**
 * \brief Deinitializes and frees ionize_pointer_list.
 * \param list Pointer to ionize_pointer_list to cleanup.
 * \return Zero on success, else error code.
 *
 * The error codes returned can be:
 * 1. EINVAL if inbalid argument is given;
 * 2. the same as returned by list element removal method.
 * After the method successfully returns, the list head will be NULL.
 */
ionize_status ionize_pointer_list_cleanup( ionize_pointer_list * const list );


#endif /* IONIZE_POINTER_LIST_H__ */
