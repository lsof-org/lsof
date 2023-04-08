/*
 * print.c - common print support functions for lsof
 */

/*
 * Copyright 1994 Purdue Research Foundation, West Lafayette, Indiana
 * 47907.  All rights reserved.
 *
 * Written by Victor A. Abell
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company or the Regents of the University of California.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. Neither the authors nor Purdue University are responsible for any
 *    consequences of the use of this software.
 *
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Credit to the authors and Purdue
 *    University must appear in documentation and sources.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 4. This notice may not be removed or altered.
 */

#include "common.h"
#include "lsof.h"

/*
 * Local definitions, structures and function prototypes
 */

/*
 * access_to_char() - convert enum lsof_file_access_mode to char
 */
char access_to_char(enum lsof_file_access_mode access) {
    switch (access) {
    default:
    case LSOF_FILE_ACCESS_NONE:
        return ' ';
    case LSOF_FILE_ACCESS_READ:
        return 'r';
    case LSOF_FILE_ACCESS_WRITE:
        return 'w';
    case LSOF_FILE_ACCESS_READ_WRITE:
        return 'u';
    }
}