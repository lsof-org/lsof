/*
 * ptti.c -- BSD style print_tcptpi() function for lsof library
 */

/*
 * Copyright 1997 Purdue Research Foundation, West Lafayette, Indiana
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

#define TCPSTATES /* activate tcpstates[] */
#include "common.h"
#include "machine.h"

#if defined(USE_LIB_PRINT_TCPTPI)

/*
 * build_IPstates() -- build the TCP and UDP state tables
 *
 * Note: this module does not support a UDP state table.
 */

void build_IPstates(struct lsof_context *ctx) {

    /*
     * Set the TcpNstates global variable.
     */
    TcpNstates = TCP_NSTATES;
    TcpSt = (char **)&tcpstates;
}
#endif /* defined(USE_LIB_PRINT_TCPTPI) */