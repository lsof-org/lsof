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

API_EXPORT
enum lsof_error lsof_select_process(struct lsof_context *ctx, char *command,
                                    int exclude) {
    char *cp; /* command copy */
    MALLOC_S len;
    struct str_lst *lpt, *str;
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    /*
     * Check for command inclusion/exclusion conflicts.
     */
    for (str = Cmdl; str; str = str->next) {
        if (str->x != exclude) {
            if (!strcmp(str->str, command)) {
                if (ctx->err) {
                    (void)fprintf(ctx->err, "%s: -c^%s and -c%s conflict.\n",
                                  Pn, str->str, command);
                }
                return LSOF_ERROR_INVALID_ARGUMENT;
            }
        }
    }

    if (!(cp = mkstrcpy(command, &len))) {
        if (ctx->err) {
            (void)fprintf(ctx->err, "%s: no string copy space: ", Pn);
            safestrprt(command, ctx->err, 1);
        }
        return LSOF_ERROR_NO_MEMORY;
    }

#if defined(MAXSYSCMDL)
    if (len > MAXSYSCMDL) {
        /* Impossible to match */
        if (ctx->err) {
            (void)fprintf(ctx->err, "%s: \"-c ", Pn);
            (void)safestrprt(command, ctx->err, 2);
            (void)fprintf(ctx->err, "\" length (%zu) > what system", len);
            (void)fprintf(ctx->err, " provides (%d)\n", MAXSYSCMDL);
        }
        CLEAN(cp);
        return LSOF_ERROR_INVALID_ARGUMENT;
    }
#endif

    if ((lpt = (struct str_lst *)malloc(sizeof(struct str_lst))) == NULL) {
        if (ctx->err) {
            safestrprt(command, ctx->err, 1);
            (void)fprintf(ctx->err, "%s: no list space: ", Pn);
            safestrprt(command, ctx->err, 1);
        }
        CLEAN(cp);
        return LSOF_ERROR_NO_MEMORY;
    }

    /* Insert into list */
    lpt->f = 0;
    lpt->str = cp;
    lpt->len = (int)len;
    lpt->x = exclude;
    if (exclude) {
        Cmdnx++;
    } else {
        Cmdni++;
        /* Update selection flag for inclusions */
        Selflags |= SELCMD;
    }
    lpt->next = Cmdl;
    Cmdl = lpt;

    return LSOF_SUCCESS;
}

API_EXPORT
enum lsof_error lsof_set_output_stream(struct lsof_context *ctx, FILE *fp,
                                       char *program_name, int warn) {
    if (!ctx) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }
    ctx->err = fp;
    ctx->program_name = mkstrcpy(program_name, NULL);
    ctx->warn = warn;
    return LSOF_SUCCESS;
}

API_EXPORT
enum lsof_error lsof_freeze(struct lsof_context *ctx) {
    enum lsof_error ret = LSOF_SUCCESS;
    if (ctx->frozen) {
        ret = LSOF_ERROR_INVALID_ARGUMENT;
    } else {
        if (Selflags == 0) {
            Selflags = SelAll;
        } else {
            if ((Selflags & (SELNA | SELNET)) != 0 &&
                (Selflags & ~(SELNA | SELNET)) == 0)
                Selinet = 1;
            AllProc = 0;
        }

        initialize(ctx);
        hashSfile(ctx);
        ctx->frozen = 1;
    }
    return ret;
}