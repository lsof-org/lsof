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

#include "lsof.h"
#ifdef AUTOTOOLS
#    include "config.h"
#endif
#include "common.h"
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
        // Initialization
        memset(ctx, 0, sizeof(struct lsof_context));
        if (!(Namech = (char *)malloc(MAXPATHLEN + 1))) {
            free(ctx);
            return NULL;
        }
        Namechl = (size_t)(MAXPATHLEN + 1);
        /* all processes are selected (default) */
        AllProc = 1;
        /* default command name limit */
        ctx->cmd_name_limit = CMDL;
        /* Readlink() and stat() timeout (seconds) */
        TmLimit = TMLIMIT;
        /* Fdl[] type: -1 == none */
        FdlTy = -1;
        /* get pid */
        Mypid = getpid();
        /* Initialize selection flags */
        SelAll = SELALL;
        SelProc = SELPROC;
        /* device cache path index: -1 = path not defined */
        ctx->dev_cache_path_index = -1;
        ctx->dev_cache_fd = -1;
        /*	device cache state:
         *  3 = update; read and rebuild if
         *	    necessary (-Du[path]) */
        ctx->dev_cache_state = 3;
        /* enable name cache */
        ctx->name_cache_enable = 1;
    }
    return ctx;
}

API_EXPORT
enum lsof_error lsof_freeze(struct lsof_context *ctx) {
    enum lsof_error ret = LSOF_SUCCESS;
    if (ctx->frozen) {
        ret = LSOF_ERROR_INVALID_ARGUMENT;
    } else {
        initialize(ctx);
        hashSfile(ctx);
        ctx->frozen = 1;
    }
    return ret;
}

API_EXPORT
enum lsof_error lsof_gather(struct lsof_context *ctx,
                            struct lsof_result **result) {
    enum lsof_error ret = LSOF_SUCCESS;
    int pi = 0; /* proc index */
    struct lsof_process *p;
    struct lproc *lp;
    int fi = 0; /* file index */
    size_t num_files;
    struct lsof_file *f;
    struct lfile *lf;
    struct lfile *lf_next;

    if (!result) {
        ret = LSOF_ERROR_INVALID_ARGUMENT;
        return ret;
    } else if (!ctx->frozen) {
        ret = LSOF_ERROR_INVALID_ARGUMENT;
        return ret;
    }
    gather_proc_info(ctx);

    /* Cleanup orphaned cur_file, if any*/
    if (ctx->cur_file) {
        clean_lfile(ctx, ctx->cur_file);
        CLEAN(ctx->cur_file);
    }

    /* Fill result */
    struct lsof_result *res =
        (struct lsof_result *)malloc(sizeof(struct lsof_result));
    struct lsof_process *user_procs = (struct lsof_process *)malloc(
        sizeof(struct lsof_process) * ctx->procs_size);
    memset(user_procs, 0, sizeof(struct lsof_process) * ctx->procs_size);

    for (pi = 0; pi < ctx->procs_size; pi++) {
        /* Copy fields from lproc */
        p = &user_procs[pi];
        lp = &ctx->procs[pi];

        p->command = lp->cmd;
        lp->cmd = NULL;
        p->pid = lp->pid;

#if defined(HASTASKS)
        p->tid = lp->tid;
        p->task_cmd = lp->tcmd;
        lp->tcmd = NULL;
#endif
        p->pgid = lp->pgid;
        p->ppid = lp->ppid;
        p->uid = lp->uid;

#if defined(HASSELINUX)
        CLEAN(lp->cntx);
#endif /* defined(HASSELINUX) */

        /* Compute number of files in the linked list */
        num_files = 0;
        for (lf = lp->file; lf; lf = lf->next) {
            num_files++;
        }

        p->files =
            (struct lsof_file *)malloc(sizeof(struct lsof_file) * num_files);
        memset(p->files, 0, sizeof(struct lsof_file) * num_files);
        p->num_files = num_files;
        for (fi = 0, lf = lp->file; fi < num_files; fi++, lf = lf_next) {
            /* Copy fields from lfile */
            f = &p->files[fi];
            f->flags = 0;

            /* FD column */
            f->fd_type = lf->fd_type;
            f->fd_num = lf->fd_num;
            f->access = lf->access;
            f->lock = lf->lock;

            /* TYPE column */
            f->file_type = lf->type;
            f->unknown_file_type_number = lf->unknown_file_type_number;

            /* DEVICE column */
            f->dev = lf->dev;
            if (lf->dev_def) {
                f->flags |= LSOF_FLAG_DEV_VALID;
            }
            f->rdev = lf->rdev;
            if (lf->rdev_def) {
                f->flags |= LSOF_FLAG_RDEV_VALID;
            }

            /* SIZE, SIZE/OFF, OFFSET column */
            f->size = lf->sz;
            if (lf->sz_def) {
                f->flags |= LSOF_FLAG_SIZE_VALID;
            }
            f->offset = lf->off;
            if (lf->off_def) {
                f->flags |= LSOF_FLAG_OFFSET_VALID;
            }

            /* NLINK column */
            f->num_links = lf->nlink;
            if (lf->nlink_def) {
                f->flags |= LSOF_FLAG_NUM_LINKS_VALID;
            }

            /* NODE column */
            f->inode = lf->inode;
            if (lf->inode_def) {
                f->flags |= LSOF_FLAG_INODE_VALID;
            }
            f->protocol = lf->iproto;
            f->unknown_proto_number = lf->unknown_proto_number;

            /* NAME column */
            f->name = lf->nm;
            lf->nm = NULL;

            /* free lf */
            lf_next = lf->next;
            CLEAN(lf->nma);
            CLEAN(lf->dev_ch);
            free(lf);
        }
        lp->file = NULL;
    }

    /* Cleanup */
    CLEAN(ctx->procs);
    ctx->cur_proc = NULL;

    res->processes = user_procs;
    res->num_processes = ctx->procs_size;

    ctx->procs_size = ctx->procs_cap = 0;
    ctx->cur_file = ctx->prev_file = NULL;

    /* Params */
    *result = res;

    return ret;
}

API_EXPORT
enum lsof_error lsof_output_stream(struct lsof_context *ctx, FILE *fp,
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
void lsof_destroy(struct lsof_context *ctx) {
    struct str_lst *str_lst, *str_lst_next;
    struct int_lst *int_lst, *int_lst_next;
    struct mounts *mnt, *mnt_next;
    if (!ctx) {
        return;
    }
    /* Free parameters */
    for (str_lst = Cmdl; str_lst; str_lst = str_lst_next) {
        str_lst_next = str_lst->next;
        CLEAN(str_lst->str);
        CLEAN(str_lst);
    }
    CLEAN(Spid);
    CLEAN(Spgid);

    /* Free temporary */
    CLEAN(Namech);

    /* Free local mount info */
    if (Lmist) {
        for (mnt = Lmi; mnt; mnt = mnt_next) {
            mnt_next = mnt->next;
            CLEAN(mnt->dir);
            CLEAN(mnt->fsname);
            CLEAN(mnt->fsnmres);
#if defined(HASFSTYPE)
            CLEAN(mnt->fstype);
#endif
            CLEAN(mnt);
        }
        Lmi = NULL;
        Lmist = 0;
    }

    /* dialect specific free */
    deinitialize(ctx);
    CLEAN(ctx);
}

API_EXPORT
void lsof_free_result(struct lsof_result *result) {
    int pi, fi;
    struct lsof_process *p;
    struct lsof_file *f;
    for (pi = 0; pi < result->num_processes; pi++) {
        p = &result->processes[pi];
        /* Free files */
        for (fi = 0; fi < p->num_files; fi++) {
            f = &p->files[fi];
            if (f->name) {
                free(f->name);
            }
        }
        CLEAN(p->files);

        /* Free process fields */
        CLEAN(p->command);
    }
    CLEAN(result->processes);
    CLEAN(result);
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
enum lsof_error lsof_select_process_regex(struct lsof_context *ctx, char *x) {
    int bmod = 0;
    int bxmod = 0;
    int i, re;
    int imod = 0;
    int xmod = 0;
    int co = REG_NOSUB | REG_EXTENDED;
    char reb[256], *xb, *xe, *xm;
    MALLOC_S xl;
    char *xp = (char *)NULL;

    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    /*
     * Make sure the supplied string starts a regular expression.
     */
    if (!*x || (*x != '/')) {
        if (ctx->err) {
            (void)fprintf(ctx->err, "%s: regexp doesn't begin with '/': ", Pn);
            if (x)
                safestrprt(x, ctx->err, 1);
        }
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    /*
     * Skip to the end ('/') of the regular expression.
     */
    xb = x + 1;
    for (xe = xb; *xe; xe++) {
        if (*xe == '/')
            break;
    }
    if (*xe != '/') {
        if (ctx->err) {
            (void)fprintf(ctx->err, "%s: regexp doesn't end with '/': ", Pn);
            safestrprt(x, ctx->err, 1);
        }
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    /*
     * Decode any regular expression modifiers.
     */
    for (i = 0, xm = xe + 1; *xm; xm++) {
        switch (*xm) {
        case 'b': /* This is a basic expression. */
            if (++bmod > 1) {
                if (bmod == 2 && ctx->err) {
                    (void)fprintf(ctx->err,
                                  "%s: b regexp modifier already used: ", Pn);
                    safestrprt(x, ctx->err, 1);
                }
                return LSOF_ERROR_INVALID_ARGUMENT;
            } else if (xmod) {
                if (++bxmod == 1 && ctx->err) {
                    (void)fprintf(
                        ctx->err,
                        "%s: b and x regexp modifiers conflict: ", Pn);
                    safestrprt(x, ctx->err, 1);
                }
                return LSOF_ERROR_INVALID_ARGUMENT;
            } else
                co &= ~REG_EXTENDED;
            break;
        case 'i': /* Ignore case. */
            if (++imod > 1) {
                if (imod == 2 && ctx->err) {
                    (void)fprintf(ctx->err,
                                  "%s: i regexp modifier already used: ", Pn);
                    safestrprt(x, ctx->err, 1);
                }
                i = 1;
            } else
                co |= REG_ICASE;
            break;
        case 'x': /* This is an extended expression. */
            if (++xmod > 1) {
                if (xmod == 2 && ctx->err) {
                    (void)fprintf(ctx->err,
                                  "%s: x regexp modifier already used: ", Pn);
                    safestrprt(x, ctx->err, 1);
                }
                i = 1;
            } else if (bmod) {
                if (++bxmod == 1 && ctx->err) {
                    (void)fprintf(
                        ctx->err,
                        "%s: b and x regexp modifiers conflict: ", Pn);
                    safestrprt(x, ctx->err, 1);
                }
                i = 1;
            } else
                co |= REG_EXTENDED;
            break;
        default:
            if (ctx->err)
                (void)fprintf(ctx->err, "%s: invalid regexp modifier: %c\n", Pn,
                              (int)*xm);
            i = 1;
        }
    }
    if (i) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    /*
     * Allocate space to hold expression and copy it there.
     */
    xl = (MALLOC_S)(xe - xb);
    if (!(xp = (char *)malloc(xl + 1))) {
        if (ctx->err) {
            (void)fprintf(ctx->err, "%s: no regexp space for: ", Pn);
            safestrprt(x, ctx->err, 1);
        }
        Error(ctx);
        return LSOF_ERROR_NO_MEMORY;
    }
    (void)strncpy(xp, xb, xl);
    xp[(int)xl] = '\0';
    /*
     * Assign a new CmdRx[] slot for this expression.
     */
    if (ctx->cmd_regex_cap <= NCmdRxU) {
        /*
         * More CmdRx[] space must be assigned.
         */
        ctx->cmd_regex_cap += 32;
        xl = (MALLOC_S)(ctx->cmd_regex_cap * sizeof(lsof_rx_t));
        if (CmdRx)
            CmdRx = (lsof_rx_t *)realloc((MALLOC_P *)CmdRx, xl);
        else
            CmdRx = (lsof_rx_t *)malloc(xl);
        if (!CmdRx) {
            if (ctx->err) {
                (void)fprintf(ctx->err, "%s: no space for regexp: ", Pn);
                safestrprt(x, ctx->err, 1);
            }
            Error(ctx);
            return LSOF_ERROR_NO_MEMORY;
        }
    }
    i = NCmdRxU;
    CmdRx[i].exp = xp;
    /*
     * Compile the expression.
     */
    if ((re = regcomp(&CmdRx[i].cx, xp, co))) {
        if (ctx->err) {
            (void)fprintf(ctx->err, "%s: regexp error: ", Pn);
            safestrprt(x, ctx->err, 0);
            (void)regerror(re, &CmdRx[i].cx, &reb[0], sizeof(reb));
            (void)fprintf(ctx->err, ": %s\n", reb);
        }
        if (xp) {
            (void)free((FREE_P *)xp);
            xp = (char *)NULL;
        }
        return (1);
    }
    /*
     * Complete the CmdRx[] table entry.
     */
    CmdRx[i].mc = 0;
    CmdRx[i].exp = xp;
    NCmdRxU++;

    /** Update selection flags for inclusion */
    if (CmdRx)
        Selflags |= SELCMD;
    return LSOF_SUCCESS;
}

/* Internal helper for lsof_select_uid/lsof_select_login*/
enum lsof_error lsof_select_uid_login(struct lsof_context *ctx, uint32_t uid,
                                      char *login, int exclude) {
    int i, j;
    MALLOC_S len;
    char *lp;

    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    /*
     * Avoid entering duplicates.
     */
    for (i = j = 0; i < Nuid; i++) {
        if (uid != Suid[i].uid)
            continue;
        /* Duplicate */
        if (Suid[i].excl == exclude) {
            return LSOF_SUCCESS;
        }

        /* Conflict */
        if (ctx->err) {
            (void)fprintf(ctx->err,
                          "%s: UID %d has been included and excluded.\n", Pn,
                          (int)uid);
        }
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    /*
     * Allocate space for User IDentifier.
     */
    if (Nuid >= Mxuid) {
        Mxuid += 10;
        len = (MALLOC_S)(Mxuid * sizeof(struct seluid));
        if (!Suid)
            Suid = (struct seluid *)malloc(len);
        else
            Suid = (struct seluid *)realloc((MALLOC_P *)Suid, len);
        if (!Suid) {
            if (ctx->err) {
                (void)fprintf(ctx->err, "%s: no space for UIDs", Pn);
            }
            Error(ctx);
            return LSOF_ERROR_NO_MEMORY;
        }
    }
    if (login) {
        if (!(lp = mkstrcpy(login, (MALLOC_S *)NULL))) {
            if (ctx->err) {
                (void)fprintf(ctx->err, "%s: no space for login: ", Pn);
                safestrprt(login, ctx->err, 1);
            }
            Error(ctx);
            return LSOF_ERROR_NO_MEMORY;
        }
        Suid[Nuid].lnm = lp;
    } else
        Suid[Nuid].lnm = (char *)NULL;
    Suid[Nuid].f = 0;
    Suid[Nuid].uid = uid;
    Suid[Nuid++].excl = exclude;
    if (exclude)
        Nuidexcl++;
    else {
        Nuidincl++;
        Selflags |= SELUID;
    }
    return LSOF_SUCCESS;
}

API_EXPORT
enum lsof_error lsof_select_uid(struct lsof_context *ctx, uint32_t uid,
                                int exclude) {
    return lsof_select_uid_login(ctx, uid, NULL, exclude);
}

API_EXPORT
enum lsof_error lsof_select_login(struct lsof_context *ctx, char *login,
                                  int exclude) {
    struct passwd *pw;
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    /* Convert login to uid, then call lsof_select_uid_login */
    if ((pw = getpwnam(login)) == NULL) {
        if (ctx->err) {
            (void)fprintf(ctx->err, "%s: can't get UID for ", Pn);
            safestrprt(login, ctx->err, 1);
        }
        return LSOF_ERROR_INVALID_ARGUMENT;
    }
    return lsof_select_uid_login(ctx, pw->pw_uid, login, exclude);
}

API_EXPORT
enum lsof_error lsof_avoid_blocking(struct lsof_context *ctx) {
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }
    Fblock = 1;
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
enum lsof_error lsof_select_fd(struct lsof_context *ctx,
                               enum lsof_fd_type fd_type, int fd_num_lo,
                               int fd_num_hi, int exclude) {
    char buf[256];
    char fd[FDLEN];
    struct fd_lst *f, *ft;

    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    /*
     * Don't allow a mixture of exclusions and inclusions.
     */
    if (FdlTy >= 0) {
        if (FdlTy != exclude) {
            if (ctx->err) {
                /*
                 * Report a mixture.
                 */
                if (fd_type != LSOF_FD_NUMERIC) {
                    print_fd(fd_type, 0, fd);
                    (void)snpf(buf, sizeof(buf) - 1, "%s%s", exclude ? "^" : "",
                               fd);
                } else {
                    if (fd_num_lo != fd_num_hi) {
                        (void)snpf(buf, sizeof(buf) - 1, "%s%d-%d",
                                   exclude ? "^" : "", fd_num_lo, fd_num_hi);
                    } else {
                        (void)snpf(buf, sizeof(buf) - 1, "%s%d",
                                   exclude ? "^" : "", fd_num_lo);
                    }
                }
                buf[sizeof(buf) - 1] = '\0';
                (void)fprintf(ctx->err, "%s: %s in an %s -d list: %s\n", Pn,
                              exclude ? "exclude" : "include",
                              FdlTy ? "exclude" : "include", buf);
            }
            return LSOF_ERROR_INVALID_ARGUMENT;
        }
    }

    /*
     * Allocate an fd_lst entry.
     */
    if (!(f = (struct fd_lst *)malloc((MALLOC_S)sizeof(struct fd_lst)))) {
        if (ctx->err)
            (void)fprintf(ctx->err, "%s: no space for FD list entry\n", Pn);
        Error(ctx);
        return LSOF_ERROR_NO_MEMORY;
    }

    /*
     * Skip duplicates.
     */
    for (ft = Fdl; ft; ft = ft->next) {
        if (ft->fd_type != f->fd_type)
            continue;
        if (ft->fd_type == LSOF_FD_NUMERIC) {
            if ((fd_num_lo != ft->lo) || (fd_num_hi != ft->hi))
                continue;
        }
        (void)free((FREE_P *)f);
        return LSOF_SUCCESS;
    }

    /*
     * Complete the fd_lst entry and link it to the head of the chain.
     */
    f->fd_type = fd_type;
    f->hi = fd_num_hi;
    f->lo = fd_num_lo;
    f->next = Fdl;
    Fdl = f;
    FdlTy = exclude;

    /* Update selection flag */
    Selflags |= SELFD;
    return LSOF_SUCCESS;
}

/* Internel helper for lsof_select_pid/pgid */
enum lsof_error lsof_select_pid_pgid(struct lsof_context *ctx, int32_t id,
                                     struct int_lst **sel, int *cap, int *size,
                                     int *incl_num, int *excl_num, int exclude,
                                     int is_pid) {
    int i, j;
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    /*
     * Avoid entering duplicates and conflicts.
     */
    for (i = j = 0; i < *size; i++) {
        if (id == (*sel)[i].i) {
            if (exclude == (*sel)[i].x) {
                return LSOF_SUCCESS;
            }
            if (ctx->err) {
                (void)fprintf(ctx->err,
                              "%s: P%sID %d has been included and excluded.\n",
                              Pn, is_pid ? "" : "G", id);
            }
            return LSOF_ERROR_INVALID_ARGUMENT;
        }
    }

    /*
     * Allocate table table space.
     */
    if (*size >= *cap) {
        *cap += 10;
        if (!(*sel))
            *sel = (struct int_lst *)malloc(
                (MALLOC_S)(sizeof(struct int_lst) * (*cap)));
        else
            *sel = (struct int_lst *)realloc(
                (MALLOC_P *)(*sel),
                (MALLOC_S)(sizeof(struct int_lst) * (*cap)));
        if (!(*sel)) {
            if (ctx->err) {
                (void)fprintf(ctx->err, "%s: no space for %d process%s IDs", Pn,
                              *cap, is_pid ? "" : " group");
            }
            Error(ctx);
            return LSOF_ERROR_NO_MEMORY;
        }
    }

    /* Insert selection into sel_pid/sel_pgid*/
    (*sel)[*size].f = 0;
    (*sel)[*size].i = id;
    (*sel)[(*size)++].x = exclude;
    if (exclude)
        (*excl_num)++;
    else {
        (*incl_num)++;
        /* Update selection flags */
        Selflags |= is_pid ? SELPID : SELPGID;
    }
    return LSOF_SUCCESS;
}

API_EXPORT
enum lsof_error lsof_select_pid(struct lsof_context *ctx, uint32_t pid,
                                int exclude) {
    enum lsof_error res = lsof_select_pid_pgid(ctx, pid, &Spid, &Mxpid, &Npid,
                                               &Npidi, &Npidx, exclude, 1);
    /* Record number of unselected PIDs for optimization */
    ctx->num_unsel_pid = Npid;
    return res;
}

API_EXPORT
enum lsof_error lsof_select_pgid(struct lsof_context *ctx, uint32_t pgid,
                                 int exclude) {
    return lsof_select_pid_pgid(ctx, pgid, &Spgid, &Mxpgid, &Npgid, &Npgidi,
                                &Npgidx, exclude, 0);
}

API_EXPORT
enum lsof_error lsof_select_inet(struct lsof_context *ctx, int inet_proto) {
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }
    if (inet_proto != 0 && inet_proto != 4 && inet_proto != 6) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    /*
     * Sequential -i, -i4, and -i6 specifications interact logically -- e.g.,
     * -i[46] followed by -i[64] is the same as -i. Specifying  -i4, or -i6
     * after -i is the same as specifying -i4 or -i6 by itself.
     */
    if (!Fnet) {
        Fnet = 1;
        FnetTy = inet_proto;
        /* Selection flags */
        Selflags |= SELNET;
    } else {
        if (FnetTy) {
            if (FnetTy != inet_proto)
                FnetTy = 0;
        } else
            FnetTy = inet_proto;
    }
    return LSOF_SUCCESS;
}

API_EXPORT
enum lsof_error lsof_select_unix_socket(struct lsof_context *ctx) {
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    Funix = 1;
    /* Selection flags */
    Selflags |= SELUNX;
    return LSOF_SUCCESS;
}

API_EXPORT
enum lsof_error lsof_select_task(struct lsof_context *ctx, int show) {
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    if (show) {
        Ftask = 1;
        IgnTasks = 0;
        Selflags |= SELTASK;
    } else {
        Ftask = 0;
        IgnTasks = 1;
        Selflags &= ~SELTASK;
    }

    /*
     * If IgnTasks is set, remove SELTASK from SelAll and
     * SelProc.
     */
    SelAll = IgnTasks ? (SELALL & ~SELTASK) : SELALL;
    SelProc = IgnTasks ? (SELPROC & ~SELTASK) : SELPROC;

    return LSOF_SUCCESS;
}

API_EXPORT
enum lsof_error lsof_select_proto_state(struct lsof_context *ctx, int tcp,
                                        char *state, int exclude) {
    int num_states;
    char **states;
    unsigned char **state_incl;
    int *state_incl_num;
    unsigned char **state_excl;
    int *state_excl_num;
    char *ty;
    int found;
    int i;

    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }
    (void)build_IPstates(ctx);

    if (tcp) {
        ty = "TCP";
        num_states = TcpNstates;
        states = TcpSt;
        state_incl = &TcpStI;
        state_incl_num = &TcpStIn;
        state_excl = &TcpStX;
        state_excl_num = &TcpStXn;
    } else {
        ty = "UDP";
        num_states = UdpNstates;
        states = UdpSt;
        state_incl = &UdpStI;
        state_incl_num = &UdpStIn;
        state_excl = &UdpStX;
        state_excl_num = &UdpStXn;
    }

    /* Allocate incl/excl array */
    if (num_states) {
        if (!*state_incl) {
            if (!(*state_incl = (unsigned char *)calloc(
                      (MALLOC_S)num_states, sizeof(unsigned char)))) {
                if (ctx->err) {
                    (void)fprintf(ctx->err,
                                  "%s: no %s state inclusion table space\n", Pn,
                                  ty);
                }
                Error(ctx);
                return LSOF_ERROR_NO_MEMORY;
            }
        }
        if (!*state_excl) {
            if (!(*state_excl = (unsigned char *)calloc(
                      (MALLOC_S)num_states, sizeof(unsigned char)))) {
                if (ctx->err) {
                    (void)fprintf(ctx->err,
                                  "%s: no %s state exclusion table space\n", Pn,
                                  ty);
                }
                Error(ctx);
                return LSOF_ERROR_NO_MEMORY;
            }
        }
    } else {
        if (ctx->err) {
            (void)fprintf(ctx->err, "%s: no %s state names available: %s\n", Pn,
                          ty, state);
        }
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    found = 0;
    /* Find match */
    if (num_states) {
        for (i = 0; i < num_states; i++) {
            if (!strcasecmp(state, states[i])) {
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        if (ctx->err) {
            (void)fprintf(ctx->err, "%s: unknown %s state name: %s\n", Pn, ty,
                          state);
        }
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    /* Find duplicate and conflict */
    if (exclude) {
        if ((*state_incl)[i]) {
            if (ctx->err) {
                (void)fprintf(ctx->err,
                              "%s: can't include and exclude %s state: %s\n",
                              Pn, ty, states[i]);
            }
            return LSOF_ERROR_INVALID_ARGUMENT;
        } else if (!(*state_excl)[i]) {
            (*state_excl)[i] = 1;
            (*state_excl_num)++;
        }
    } else {
        if ((*state_excl)[i]) {
            if (ctx->err) {
                (void)fprintf(ctx->err,
                              "%s: can't include and exclude %s state: %s\n",
                              Pn, ty, states[i]);
            }
            return LSOF_ERROR_INVALID_ARGUMENT;
        } else if (!(*state_incl)[i]) {
            (*state_incl)[i] = 1;
            (*state_incl_num)++;
        }
    }

    return LSOF_SUCCESS;
}

API_EXPORT
enum lsof_error lsof_select_nfs(struct lsof_context *ctx) {
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }
    Fnfs = 1;
    /* Selection flags */
    Selflags |= SELNFS;
    return LSOF_SUCCESS;
}

API_EXPORT
enum lsof_error lsof_select_num_links(struct lsof_context *ctx, int threshold) {
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }
    Nlink = threshold;
    /* Selection flags */
    if (Nlink) {
        Selflags |= SELNLINK;
    }
    return LSOF_SUCCESS;
}

API_EXPORT
enum lsof_error lsof_exempt_fs(struct lsof_context *ctx, char *orig_path,
                               int avoid_readlink) {
    char *ec;         /* pointer to copy of path */
    efsys_list_t *ep; /* file system path list pointer */
    int i;            /* temporary index */
    char *path;       /* Readlink() of file system path */

    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    if (!orig_path || (*orig_path != '/')) {
        if (ctx->err && ctx->warn) {
            (void)fprintf(ctx->err,
                          "%s: -e not followed by a file system path: \"%s\"\n",
                          Pn, orig_path);
        }
        return LSOF_ERROR_INVALID_ARGUMENT;
    }
    if (!(ec = mkstrcpy(orig_path, (MALLOC_S *)NULL))) {
        if (ctx->err) {
            (void)fprintf(ctx->err, "%s: no space for -e string: ", Pn);
            safestrprt(orig_path, ctx->err, 1);
        }
        Error(ctx);
        return LSOF_ERROR_NO_MEMORY;
    }
    if (avoid_readlink)
        path = ec;
    else {
        if (!(path = Readlink(ctx, ec)))
            return LSOF_ERROR_INVALID_ARGUMENT;
    }

    /*
     * Remove terminating `/' characters from paths longer than one.
     */
    for (i = (int)strlen(path); (i > 1) && (path[i - 1] == '/'); i--) {
        path[i - 1] = '\0';
    }
    /*
     * Enter file system path on list, avoiding duplicates.
     */
    for (ep = Efsysl; ep; ep = ep->next) {
        if (!strcmp(ep->path, path)) {
            (void)free((FREE_P *)path);
            return LSOF_SUCCESS;
        }
    }

    if (!(ep = (efsys_list_t *)malloc((MALLOC_S)(sizeof(efsys_list_t))))) {
        if (ctx->err) {
            (void)fprintf(ctx->err, "%s: no space for \"-e %s\" entry\n", Pn,
                          orig_path);
        }
        Error(ctx);
        return LSOF_ERROR_NO_MEMORY;
    }
    ep->path = path;
    ep->pathl = i;
    ep->rdlnk = avoid_readlink;
    ep->mp = (struct mounts *)NULL;
    ep->next = Efsysl;
    Efsysl = ep;
    return LSOF_SUCCESS;
}

API_EXPORT
enum lsof_error lsof_avoid_forking(struct lsof_context *ctx, int avoid) {
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }
    Fovhd = avoid;
    return LSOF_SUCCESS;
}

enum lsof_error lsof_exit_on_fatal(struct lsof_context *ctx, int exit) {
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }
    ctx->exit_on_fatal = exit;
    return LSOF_SUCCESS;
}