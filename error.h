/**
 * @file
 *
 * @brief Set of functions for error handling.
 *
 * @author Varnike
 *
 * @date 21 Sep, 1:40 UTC+3.
 */


#ifndef ERROR_H
#define ERROR_H

#include <cstdio>
/**
 * @brief Stores most recent error code.
 *
 * Heavily lightweight errno analog.
 */

extern int ERRNUM;

const int ERRS_CNT = 23;

enum errtype {
	NO_ERR	        = 0,
	UNKNOWN_ERR     = 1,
	CALLOC_ERR      = 2,
	READ_ERR        = 3,
	WRITE_ERR       = 4,
	STRUCT_PTR_ERR  = 5,
	BUFF_PTR_ERR    = 6,
	FILE_SIZE_ERR   = 7,
	FCLOSE_ERR      = 8,
	FOPEN_ERR       = 9,
	REALL_ERR       = 10,
	UNDERFLOW_ERR   = 11,
	POISONED_STACK  = 12,
	NULLPTR_STACK   = 13,
	UNINIT_DATA     = 14,
	STACK_OVERFLOW  = 15,
	INVALID_STACK   = 16,
	INVALID_LCANARY = 17,
	INVALID_RCANARY = 18,
	INVALID_CANARIES= 19,
	INVALID_DATA_RCANARY   = 20,
       	INVALID_DATA_LCANARY   = 21,
	INVALID_HASH	       = 22
};
/**
 * Returns a pointer to the textual description of the code errtype.
 *
 * Uses error enumeration to determine error string.
 * @param errt error code.
 *
 * @return Pointer to a null-terminated byte string corresponding to the errtype code.
 */

const char *errmsg(int errt);

#endif //ERROR_H
