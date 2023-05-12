/*
 * lsof.c -- implement lsof_* functions() for liblsof
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

#ifdef AUTOTOOLS
#    include "config.h"
#endif
#include "common.h"
#include "lsof.h"
#include <unistd.h>

#ifndef API_EXPORT
#    define API_EXPORT
#endif

API_EXPORT
int lsof_get_api_version() { return LSOF_API_VERSION; }

#ifdef AUTOTOOLS
API_EXPORT
char *lsof_get_library_version() { return PACKAGE_VERSION; }
#endif

API_EXPORT
struct lsof_context *lsof_new() {
    struct lsof_context *ctx =
        (struct lsof_context *)malloc(sizeof(struct lsof_context));
    if (ctx) {
        /* Initialization */
        memset(ctx, 0, sizeof(struct lsof_context));

#if defined(WARNINGSTATE)
        /* suppress warnings */
        Fwarn = 1;
#else  /* !defined(WARNINGSTATE) */
        /* display warnings */
        Fwarn = 0;
#endif /* defined(WARNINGSTATE) */

#if defined(HASXOPT_VALUE)
        /* -X option status */
        Fxopt = HASXOPT_VALUE;
#endif /* defined(HASXOPT_VALUE) */

        /* -1 == none */
        FdlTy = -1;

        /* Readlink() and stat() timeout (seconds) */
        TmLimit = TMLIMIT;

        /* default */
        AllProc = 1;

        /* -1 == none */
        FdlTy = -1;

	/* device cache file descriptor */
	DCfd = -1;

	/* device cache path index: -1 = path not defined */
	DCpathX = -1;

	/* device cache state: 3 = update; read and rebuild if necessary */
	DCstate = 3;
    }
    return ctx;
}

API_EXPORT
enum lsof_error lsof_avoid_blocking(struct lsof_context *ctx, int avoid) {
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }
    Fblock = avoid;
    return LSOF_EXIT_SUCCESS;
}

API_EXPORT
enum lsof_error lsof_avoid_forking(struct lsof_context *ctx, int avoid) {
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }
    Fovhd = avoid;
    return LSOF_SUCCESS;
}

API_EXPORT
enum lsof_error lsof_logic_and(struct lsof_context *ctx) {
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }
    Fand = 1;
    return LSOF_SUCCESS;
}