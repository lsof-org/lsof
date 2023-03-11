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
    int se1, se2, ss = 0, ad;
    struct stat sb;
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
        /* enable name cache by default */
        Fncache = 1;

#if defined(WARNINGSTATE)
        Fwarn = 1; /* +|-w option status */
#else              /* !defined(WARNINGSTATE) */
        Fwarn = 0; /* +|-w option status */
#endif             /* defined(WARNINGSTATE) */

        /*
         * Get the device for DEVDEV_PATH.
         */
        if (stat(DEVDEV_PATH, &sb)) {
            se1 = errno;
            if ((ad = strcmp(DEVDEV_PATH, "/dev"))) {
                if ((ss = stat("/dev", &sb)))
                    se2 = errno;
                else {
                    DevDev = sb.st_dev;
                    se2 = 0;
                }
            } else {
                se2 = 0;
                ss = 1;
            }
            if (ss) {
                if (ctx->err)
                    (void)fprintf(ctx->err, "%s: can't stat(%s): %s\n", Pn,
                                  DEVDEV_PATH, strerror(se1));
                if (ad && ctx->err) {
                    (void)fprintf(ctx->err, "%s: can't stat(/dev): %s\n", Pn,
                                  strerror(se2));
                }
                Error(ctx);
            }
        } else {
            DevDev = sb.st_dev;
        }
    }
    return ctx;
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

static struct sockaddr_storage fill_sockaddr(struct linaddr li) {
    struct sockaddr_storage ret;
    struct sockaddr_in *in = (struct sockaddr_in *)&ret;
    struct sockaddr_in6 *in6 = (struct sockaddr_in6 *)&ret;

    memset(&ret, 0, sizeof(ret));
    if (li.af == AF_INET) {
        in->sin_family = AF_INET;
        in->sin_port = htons(li.p);
        in->sin_addr = li.ia.a4;
    } else if (li.af == AF_INET6) {
        in6->sin6_family = AF_INET6;
        in6->sin6_port = htons(li.p);
        in6->sin6_addr = li.ia.a6;
    }
    return ret;
}

API_EXPORT
enum lsof_error lsof_gather(struct lsof_context *ctx,
                            struct lsof_result **result) {
    enum lsof_error ret = LSOF_SUCCESS;
    int pi = 0;  /* proc index */
    int upi = 0; /* user proc index */
    struct lsof_process *p;
    struct lproc *lp;
    int fi = 0; /* file index */
    size_t num_files;
    struct lsof_file *f;
    struct lfile *lf;
    struct lfile *lf_next;
    size_t sel_procs = 0;
    char *cp;
    char buf[64];
    int s;
    struct str_lst *str;
    struct sfile *sfp;
    struct nwad *np, *npn;
#if defined(HASPROCFS)
    struct procfsid *pfi;
#endif /* defined(HASPROCFS) */
#if defined(HASZONES)
    znhash_t *zp;
#endif /* defined(HASZONES) */
#if defined(HASSELINUX)
    cntxlist_t *cntxp;
#endif /* defined(HASSELINUX) */
    int pass;
    int i;
    struct lsof_selection *selections;
    size_t num_selections = 0;

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

    /* Count selected procs */
    for (pi = 0; pi < ctx->procs_size; pi++) {
        lp = &ctx->procs[pi];
        if (lp->pss) {
            sel_procs++;
        }
    }

    /* Fill result */
    struct lsof_result *res =
        (struct lsof_result *)malloc(sizeof(struct lsof_result));
    struct lsof_process *user_procs =
        (struct lsof_process *)malloc(sizeof(struct lsof_process) * sel_procs);
    memset(user_procs, 0, sizeof(struct lsof_process) * sel_procs);

    for (pi = 0, upi = 0; pi < ctx->procs_size; pi++) {
        /* Copy fields from lproc */
        lp = &ctx->procs[pi];
        if (lp->pss) {
            /* selected process */
            p = &user_procs[upi++];

            p->command = lp->cmd;
            lp->cmd = NULL;
            p->pid = lp->pid;

#if defined(HASTASKS)
            p->tid = lp->tid;
            p->task_cmd = lp->tcmd;
            lp->tcmd = NULL;
#endif
#if defined(HASZONES)
            p->solaris_zone = lp->zn;
            lp->zn = NULL;
#endif
#if defined(HASSELINUX)
            p->selinux_context = lp->cntx;
            lp->cntx = NULL;
#endif

            p->pgid = lp->pgid;
            p->ppid = lp->ppid;
            p->uid = lp->uid;

            /* Compute number of files in the linked list */
            num_files = 0;
            for (lf = lp->file; lf; lf = lf->next) {
                if (!is_file_sel(ctx, lp, lf))
                    continue;
                num_files++;
            }

            p->files = (struct lsof_file *)malloc(sizeof(struct lsof_file) *
                                                  num_files);
            memset(p->files, 0, sizeof(struct lsof_file) * num_files);
            p->num_files = num_files;
            for (fi = 0, lf = lp->file; lf; lf = lf_next) {
                if (is_file_sel(ctx, lp, lf)) {
                    /* Copy fields from lfile */
                    f = &p->files[fi++];
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
                        f->flags |= LSOF_FILE_FLAG_DEV_VALID;
                    }
                    f->rdev = lf->rdev;
                    if (lf->rdev_def) {
                        f->flags |= LSOF_FILE_FLAG_RDEV_VALID;
                    }

                    /* SIZE, SIZE/OFF, OFFSET column */
                    f->size = lf->sz;
                    if (lf->sz_def) {
                        f->flags |= LSOF_FILE_FLAG_SIZE_VALID;
                    }
                    f->offset = lf->off;
                    if (lf->off_def) {
                        f->flags |= LSOF_FILE_FLAG_OFFSET_VALID;
                    }

                    /* NLINK column */
                    f->num_links = lf->nlink;
                    if (lf->nlink_def) {
                        f->flags |= LSOF_FILE_FLAG_NUM_LINKS_VALID;
                    }

                    /* NODE column */
                    f->inode = lf->inode;
                    if (lf->inode_def) {
                        f->flags |= LSOF_FILE_FLAG_INODE_VALID;
                    }
                    f->protocol = lf->iproto;
                    f->unknown_proto_number = lf->unknown_proto_number;

                    /* NAME column */
                    f->name = lf->nm;
                    lf->nm = NULL;

                    /* network address */
                    f->net_local = fill_sockaddr(lf->li[0]);
                    f->net_foreign = fill_sockaddr(lf->li[1]);

                    /* tcp/tpi state */
                    if (lf->lts.type != -1) {
                        f->flags |= LSOF_FILE_FLAG_TCP_TPI_VALID;
                        if (lf->lts.type == 0) {
                            /* TCP */
                            if (!TcpSt)
                                (void)build_IPstates(ctx);
                            if ((s = lf->lts.state.i + TcpStOff) < 0 ||
                                s >= TcpNstates) {
                                (void)snpf(buf, sizeof(buf),
                                           "UNKNOWN_TCP_STATE_%d",
                                           lf->lts.state.i);
                                cp = buf;
                            } else
                                cp = TcpSt[s];
                        } else {
                            /* TPI */
                            if (!UdpSt)
                                (void)build_IPstates(ctx);
                            if ((s = lf->lts.state.i + UdpStOff) < 0 ||
                                s >= UdpNstates) {
                                (void)snpf(buf, sizeof(buf),
                                           "UNKNOWN_TPI_STATE_%d",
                                           lf->lts.state.i);
                                cp = buf;
                            } else
                                cp = UdpSt[s];
                        }
                        f->tcp_tpi.state = mkstrcpy(cp, NULL);

#if defined(HASTCPTPIQ)
                        if (lf->lts.rqs) {
                            f->tcp_tpi.recv_queue_len = lf->lts.rq;
                            f->tcp_tpi.flags |=
                                LSOF_TCP_TPI_FLAG_RECV_QUEUE_LEN_VALID;
                        }
                        if (lf->lts.sqs) {
                            f->tcp_tpi.send_queue_len = lf->lts.sq;
                            f->tcp_tpi.flags |=
                                LSOF_TCP_TPI_FLAG_SEND_QUEUE_LEN_VALID;
                        }
#endif /* defined(HASTCPTPIQ) */
                    }
                }
                lf_next = lf->next;
            }
        }

        for (lf = lp->file; lf; lf = lf_next) {
            /* free lf */
            lf_next = lf->next;
            CLEAN(lf->nma);
            CLEAN(lf->dev_ch);
#if defined(CLRLFILEADD)
            CLRLFILEADD(lf)
#endif /* defined(CLRLFILEADD) */
            CLEAN(lf);
        }
        lp->file = NULL;

        /* skip and free */
        CLEAN(lp->cmd);
#if defined(HASTASKS)
        CLEAN(lp->tcmd);
#endif
#if defined(HASSELINUX)
        CLEAN(lp->cntx);
#endif /* defined(HASSELINUX) */
        continue;
    }

    /* Cleanup */
    CLEAN(ctx->procs);
    ctx->cur_proc = NULL;

    res->processes = user_procs;
    res->num_processes = sel_procs;

    ctx->procs_size = ctx->procs_cap = 0;
    ctx->cur_file = ctx->prev_file = NULL;

    /* Collect selection result */
    for (pass = 0; pass < 2; pass++) {
        num_selections = 0;

        /* command */
        for (str = Cmdl; str; str = str->next) {
            if (pass) {
                selections[num_selections].type = LSOF_SELECTION_COMMAND;
                selections[num_selections].found = str->f;
                selections[num_selections].string = str->str;
            }
            num_selections++;
        }

        /* command regex */
        for (i = 0; i < NCmdRxU; i++) {
            if (pass) {
                selections[num_selections].type = LSOF_SELECTION_COMMAND_REGEX;
                selections[num_selections].found = CmdRx[i].mc > 0;
                selections[num_selections].string = CmdRx[i].exp;
            }
            num_selections++;
        }

        /* select file or file system */
        for (sfp = Sfile; sfp; sfp = sfp->next) {
            if (pass) {
                selections[num_selections].type =
                    sfp->type ? LSOF_SELECTION_PATH
                              : LSOF_SELECTION_FILE_SYSTEM;
                selections[num_selections].found = sfp->f;
                selections[num_selections].string = sfp->aname;
            }
            num_selections++;
        }

#if defined(HASPROCFS)
        /* procfs */
        if (Procsrch) {
            if (pass) {
                selections[num_selections].type = LSOF_SELECTION_FILE_SYSTEM;
                selections[num_selections].found = Procfind;
                selections[num_selections].string =
                    Mtprocfs ? Mtprocfs->dir : HASPROCFS;
            }
            num_selections++;
        }

        for (pfi = Procfsid; pfi; pfi = pfi->next) {
            if (pass) {
                selections[num_selections].type = LSOF_SELECTION_PATH;
                selections[num_selections].found = pfi->f;
                selections[num_selections].string = pfi->nm;
            }
            num_selections++;
        }
#endif /* defined(HASPROCFS) */

        /* network address */
        for (np = Nwad; np;) {
            int found = np->f;
            if (!(cp = np->arg)) {
                np = np->next;
                continue;
            }
            for (npn = np->next; npn; npn = npn->next) {
                if (!npn->arg)
                    continue;
                if (!strcmp(cp, npn->arg)) {
                    /* Found duplicate specification */
                    found |= npn->f;
                } else {
                    break;
                }
            }

            if (pass) {
                selections[num_selections].type =
                    LSOF_SELECTION_NETWORK_ADDRESS;
                selections[num_selections].found = found;
                selections[num_selections].string = np->arg;
            }
            num_selections++;
            np = npn;
        }

        /* ip protocol */
        if (Fnet) {
            if (pass) {
                selections[num_selections].type = LSOF_SELECTION_INTERNET;
                selections[num_selections].found = Fnet == 2;
            }
            num_selections++;
        }

#if defined(HASTCPUDPSTATE)
        /* tcp/tpi protocol state */
        if (TcpStIn) {
            for (i = 0; i < TcpNstates; i++) {
                if (TcpStI[i]) {
                    if (pass) {
                        selections[num_selections].type =
                            LSOF_SELECTION_PROTOCOL_STATE;
                        selections[num_selections].found = TcpStI[i] == 2;
                        selections[num_selections].string = TcpSt[i];
                    }
                    num_selections++;
                }
            }
        }
        if (UdpStIn) {
            for (i = 0; i < UdpNstates; i++) {
                if (UdpStI[i]) {
                    if (pass) {
                        selections[num_selections].type =
                            LSOF_SELECTION_PROTOCOL_STATE;
                        selections[num_selections].found = UdpStI[i] == 2;
                        selections[num_selections].string = UdpSt[i];
                    }
                    num_selections++;
                }
            }
        }
#endif /* defined(HASTCPUDPSTATE) */

        /* nfs */
        if (Fnfs) {
            if (pass) {
                selections[num_selections].type = LSOF_SELECTION_NFS;
                selections[num_selections].found = Fnfs == 2;
            }
            num_selections++;
        }

        /* pid */
        for (i = 0; i < Npid; i++) {
            if (Spid[i].x)
                continue;
            if (pass) {
                selections[num_selections].type = LSOF_SELECTION_PID;
                selections[num_selections].found = Spid[i].f;
                selections[num_selections].integer = Spid[i].i;
            }
            num_selections++;
        }

        /* pgid */
        for (i = 0; i < Npgid; i++) {
            if (Spgid[i].x)
                continue;
            if (pass) {
                selections[num_selections].type = LSOF_SELECTION_PGID;
                selections[num_selections].found = Spgid[i].f;
                selections[num_selections].integer = Spgid[i].i;
            }
            num_selections++;
        }

        /* uid */
        for (i = 0; i < Nuid; i++) {
            if (Suid[i].excl)
                continue;
            if (pass) {
                selections[num_selections].type = LSOF_SELECTION_UID;
                selections[num_selections].found = Suid[i].f;
                selections[num_selections].string = Suid[i].lnm;
                selections[num_selections].integer = Suid[i].uid;
            }
            num_selections++;
        }

#if defined(HASTASKS)
        /* tasks */
        if (Ftask) {
            if (pass) {
                selections[num_selections].type = LSOF_SELECTION_TASK;
                selections[num_selections].found = Ftask == 2;
            }
            num_selections++;
        }
#endif /* defined(HASTASKS) */

#if defined(HASZONES)
        /* solaris zones */
        if (ZoneArg) {
            for (i = 0; i < HASHZONE; i++) {
                for (zp = ZoneArg[i]; zp; zp = zp->next) {
                    if (pass) {
                        selections[num_selections].type =
                            LSOF_SELECTION_SOLARIS_ZONE;
                        selections[num_selections].found = zp->f;
                        selections[num_selections].string = zp->zn;
                    }
                    num_selections++;
                }
            }
        }
#endif /* defined(HASZONES) */

#if defined(HASSELINUX)
        /* SELinux context */
        if (CntxArg) {
            for (cntxp = CntxArg; cntxp; cntxp = cntxp->next) {
                if (pass) {
                    selections[num_selections].type =
                        LSOF_SELECTION_SELINUX_CONTEXT;
                    selections[num_selections].found = cntxp->f;
                    selections[num_selections].string = cntxp->cntx;
                }
                num_selections++;
            }
        }
#endif /* defined(HASSELINUX) */

        /* allocation selections array */
        if (pass == 0) {
            selections = (struct lsof_selection *)malloc(
                sizeof(struct lsof_selection) * num_selections);
            memset(selections, 0,
                   sizeof(struct lsof_selection) * num_selections);
            res->selections = selections;
            res->num_selections = num_selections;
        }
    }

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
    int i;
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
    for (i = 0; i < Nuid; i++) {
        CLEAN(Suid[i].lnm);
    }
    CLEAN(Suid);
    CLEAN(Nmlst);

    /* Free temporary */
    CLEAN(Namech);
#if defined(HASNLIST)
    CLEAN(Nl);
    Nll = 0;
#endif /* defined(HASNLIST) */

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

    /* state table */
#if !defined(USE_LIB_PRINT_TCPTPI)
    for (i = 0; i < TcpNstates; i++) {
        CLEAN(TcpSt[i]);
    }
    CLEAN(TcpSt);
#endif /* !defined(USE_LIB_PRINT_TCPTPI) */
    for (i = 0; i < UdpNstates; i++) {
        CLEAN(UdpSt[i]);
    }
    CLEAN(UdpSt);
    CLEAN(Pn);

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
            CLEAN(f->name);
            CLEAN(f->tcp_tpi.state);
        }
        CLEAN(p->files);

        /* Free process fields */
        CLEAN(p->command);
        CLEAN(p->task_cmd);
        CLEAN(p->solaris_zone);
        CLEAN(p->selinux_context);
    }
    CLEAN(result->processes);
    CLEAN(result->selections);
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
    enum lsof_error ret = LSOF_SUCCESS;

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
        ret = LSOF_ERROR_INVALID_ARGUMENT;
        goto cleanup;
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
        ret = LSOF_ERROR_INVALID_ARGUMENT;
        goto cleanup;
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
        ret = LSOF_ERROR_INVALID_ARGUMENT;
        goto cleanup;
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
        ret = LSOF_ERROR_NO_MEMORY;
        goto cleanup;
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
            ret = LSOF_ERROR_NO_MEMORY;
            goto cleanup;
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
        ret = LSOF_ERROR_INVALID_ARGUMENT;
        goto cleanup;
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
    ret = LSOF_SUCCESS;
cleanup:
    CLEAN(xp);
    return ret;
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
    for (i = 0; i < Nuid; i++) {
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
enum lsof_error lsof_avoid_blocking(struct lsof_context *ctx, int avoid) {
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }
    Fblock = avoid;
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
        if (ft->fd_type != fd_type)
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
    for (i = 0; i < *size; i++) {
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
    Npuns = Npid;
    return res;
}

API_EXPORT
enum lsof_error lsof_select_pgid(struct lsof_context *ctx, uint32_t pgid,
                                 int exclude) {
    return lsof_select_pid_pgid(ctx, pgid, &Spgid, &Mxpgid, &Npgid, &Npgidi,
                                &Npgidx, exclude, 0);
}

API_EXPORT
enum lsof_error lsof_select_ip(struct lsof_context *ctx, int af) {
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }
    if (af != AF_UNSPEC && af != AF_INET && af != AF_INET6) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    /*
     * Sequential -i, -i4, and -i6 specifications interact logically -- e.g.,
     * -i[46] followed by -i[64] is the same as -i. Specifying -i4, or -i6
     * after -i is the same as specifying -i4 or -i6 by itself.
     */
    if (!Fnet) {
        Fnet = 1;
        FnetTy = af;
        /* Selection flags */
        Selflags |= SELNET;
    } else {
        if (FnetTy != AF_UNSPEC) {
            if (FnetTy != af)
                FnetTy = AF_UNSPEC;
        } else
            FnetTy = af;
    }
    return LSOF_SUCCESS;
}

enum lsof_error lsof_select_inet_internal(struct lsof_context *ctx, char *arg,
                                          enum lsof_protocol proto, int af,
                                          size_t addr_len, void *addr,
                                          int port_lo, int port_hi) {
    int ac;
    unsigned char *ap;
    struct nwad nc;
    struct nwad *np;
    struct nwad *n = NULL;
    enum lsof_error ret = LSOF_SUCCESS;

    /*
     * Test address specification -- it must contain at least one of:
     * protocol, Internet address or port.  If correct, link into search
     * list.
     */
    if (proto == LSOF_PROTOCOL_INVALID && !addr && port_lo == -1 &&
        port_hi == -1) {
        if (ctx->err) {
            (void)fprintf(ctx->err,
                          "%s: incomplete Internet address specification: -i ",
                          Pn);
            safestrprt(arg, ctx->err, 1);
        }
        ret = LSOF_ERROR_INVALID_ARGUMENT;
        goto cleanup;
    }

    if (!(n = (struct nwad *)malloc(sizeof(struct nwad)))) {
        if (ctx->err) {
            (void)fprintf(ctx->err,
                          "%s: no space for network address from: -i ", Pn);
            safestrprt(arg, ctx->err, 1);
        }
        Error(ctx);
        ret = LSOF_ERROR_NO_MEMORY;
        goto cleanup;
    }

    /*
     * Construct and link the address specification.
     */
    n->proto = proto;
    n->af = af;
    n->sport = port_lo;
    n->eport = port_hi;
    n->f = 0;
    n->next = Nwad;

    if (af == AF_INET6 && addr) {
        /*
         * Copy an AF_INET6 address.
         */
        if (addr_len != sizeof(struct in6_addr)) {
            ret = LSOF_ERROR_INVALID_ARGUMENT;
            goto cleanup;
        }
        (void)memcpy((void *)&n->a[0], (void *)addr, sizeof(struct in6_addr));
    } else if (af == AF_INET && addr) {
        /*
         * Copy an AF_INET address.
         */
        if (addr_len != sizeof(struct in_addr)) {
            ret = LSOF_ERROR_INVALID_ARGUMENT;
            goto cleanup;
        }
        (void)memcpy((void *)&n->a[0], (void *)addr, sizeof(struct in_addr));
    }

    /*
     * Allocate space for the argument specification.
     */
    if (arg) {
        if (!(n->arg = mkstrcpy(arg, (MALLOC_S *)NULL))) {
            if (ctx->err) {
                (void)fprintf(ctx->err,
                              "%s: no space for Internet argument: -i ", Pn);
                safestrprt(arg, ctx->err, 1);
            }
            Error(ctx);
            ret = LSOF_ERROR_NO_MEMORY;
            goto cleanup;
        }
    } else
        n->arg = (char *)NULL;

    Nwad = n;
    n = NULL;

    /* Update selection flags */
    Selflags |= SELNA;

cleanup:
    CLEAN(n);
    return ret;
}

API_EXPORT
enum lsof_error lsof_select_inet(struct lsof_context *ctx, int af,
                                 enum lsof_protocol proto, size_t addr_len,
                                 void *addr, int port_lo, int port_hi) {
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    return lsof_select_inet_internal(ctx, "lsof_select_inet()", proto, af,
                                     addr_len, addr, port_lo, port_hi);
}

/*
 * isIPv4addr() - is host name an IPv4 address
 */
static char *isIPv4addr(char *hn,         /* host name */
                        unsigned char *a, /* address receptor */
                        int al)           /* address receptor length */
{
    int dc = 0;          /* dot count */
    int i;               /* temorary index */
    int ov[MIN_AF_ADDR]; /* octet values */
    int ovx = 0;         /* ov[] index */
    /*
     * The host name must begin with a number and the return octet value
     * arguments must be acceptable.
     */
    if ((*hn < '0') || (*hn > '9'))
        return ((char *)NULL);
    if (!a || (al < MIN_AF_ADDR))
        return ((char *)NULL);
    /*
     * Start the first octet assembly, then parse tge remainder of the host
     * name for four octets, separated by dots.
     */
    ov[0] = (int)(*hn++ - '0');
    while (*hn && (*hn != ':')) {
        if (*hn == '.') {

            /*
             * Count a dot.  Make sure a preceding octet value has been
             * assembled.  Don't assemble more than MIN_AF_ADDR octets.
             */
            dc++;
            if ((ov[ovx] < 0) || (ov[ovx] > 255))
                return ((char *)NULL);
            if (++ovx > (MIN_AF_ADDR - 1))
                return ((char *)NULL);
            ov[ovx] = -1;
        } else if ((*hn >= '0') && (*hn <= '9')) {

            /*
             * Assemble an octet.
             */
            if (ov[ovx] < 0)
                ov[ovx] = (int)(*hn - '0');
            else
                ov[ovx] = (ov[ovx] * 10) + (int)(*hn - '0');
        } else {

            /*
             * A non-address character has been detected.
             */
            return ((char *)NULL);
        }
        hn++;
    }
    /*
     * Make sure there were three dots and four non-null octets.
     */
    if ((dc != 3) || (ovx != (MIN_AF_ADDR - 1)) || (ov[ovx] < 0) ||
        (ov[ovx] > 255))
        return ((char *)NULL);
    /*
     * Copy the octets as unsigned characters and return the ending host name
     * character position.
     */
    for (i = 0; i < MIN_AF_ADDR; i++) {
        a[i] = (unsigned char)ov[i];
    }
    return (hn);
}

/*
 * lkup_hostnm() - look up host name
 */
static struct addrinfo *
lkup_hostnm(char *hn,       /* host name */
            struct nwad *n) /* network address destination */
{
    unsigned char *ap;
    struct addrinfo *he = NULL;
    struct addrinfo hint;
    int ln;
    int res;

    /*
     * Get hostname structure pointer.  Return NULL if there is none.
     */
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = n->af;

    res = getaddrinfo(hn, NULL, &hint, &he);

    if (!he || !he->ai_addr)
        return (he);

        /*
         * Copy first hostname structure address to destination structure.
         */

#if defined(HASIPv6)
    if (n->af != he->ai_family)
        return ((struct addrinfo *)NULL);
    if (n->af == AF_INET6) {
        /*
         * Copy an AF_INET6 address.
         */
        if (he->ai_addrlen != sizeof(struct sockaddr_in6))
            return ((struct addrinfo *)NULL);
        struct sockaddr_in6 *in6 = (struct sockaddr_in6 *)he->ai_addr;
        (void)memcpy((void *)&n->a[0], (void *)&in6->sin6_addr, MAX_AF_ADDR);
        return (he);
    }
#endif /* defined(HASIPv6) */

    /*
     * Copy an AF_INET address.
     */
    if (he->ai_addrlen != sizeof(struct sockaddr_in))
        return ((struct addrinfo *)NULL);
    struct sockaddr_in *in = (struct sockaddr_in *)he->ai_addr;
    ap = (unsigned char *)&in->sin_addr;
    n->a[0] = *ap++;
    n->a[1] = *ap++;
    n->a[2] = *ap++;
    n->a[3] = *ap;
    if ((ln = MAX_AF_ADDR - 4) > 0)
        zeromem((char *)&n->a[4], ln);
    return (he);
}

API_EXPORT
enum lsof_error lsof_select_inet_string(struct lsof_context *ctx, char *na) {
    int ae, i, pr;
    int ft = 0;
    struct addrinfo *he = NULL;
    struct addrinfo hint;
    char *hn = (char *)NULL;
    MALLOC_S l;
    char *p, *wa;
    int pt = 0;
    int pu = 0;
    struct servent *se, *se1;
    char *sn = (char *)NULL;
    MALLOC_S snl = 0;
    char *str_proto = NULL;
    enum lsof_error ret = LSOF_SUCCESS;

    size_t addr_len = 0;
    void *addr = NULL;

    /* arguments of lsof_select_inet_internal */
    int af = AF_UNSPEC;
    enum lsof_protocol proto = LSOF_PROTOCOL_INVALID;
    unsigned char a[MAX_AF_ADDR];
    int sp = -1;
    int ep = -1;

#if defined(HASIPv6)
    char *cp;
#endif /* defined(HASIPv6) */

    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    if (!na) {
        if (ctx->err)
            (void)fprintf(ctx->err, "%s: no network address specified\n", Pn);
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    wa = na;

    /*
     * Process an IP version type specification, IPv4 or IPv6, optionally
     * followed by a '@' and a host name or Internet address, or a ':' and a
     * service name or port number.
     */
    if ((*wa == '4') || (*wa == '6')) {
        if (*wa == '4')
            af = AF_INET;
        else if (*wa == '6') {

#if defined(HASIPv6)
            af = AF_INET6;
#else  /* !defined(HASIPv6) */
            if (ctx->err) {
                (void)fprintf(ctx->err, "%s: IPv6 not supported: -i ", Pn);
                safestrprt(na, ctx->err, 1);
            }
            ret = LSOF_ERROR_UNSUPPORTED;
            goto cleanup;
#endif /* defined(HASIPv6) */
        }
        wa++;
        if (!*wa) {

            /*
             * If nothing follows 4 or 6, then all network files of the
             * specified IP version are selected.
             */
            return lsof_select_ip(ctx, af);
        }
    }

    /*
     * Process protocol name, optionally followed by a '@' and a host name or
     * Internet address, or a ':' and a service name or port number.
     */
    if (*wa && *wa != '@' && *wa != ':') {
        for (p = wa; *wa && *wa != '@' && *wa != ':'; wa++)
            ;
        if ((l = wa - p)) {
            if (!(str_proto = mkstrcat(p, l, (char *)NULL, -1, (char *)NULL, -1,
                                       (MALLOC_S *)NULL))) {
                if (ctx->err) {
                    (void)fprintf(ctx->err,
                                  "%s: no space for protocol name from: -i ",
                                  Pn);
                    safestrprt(na, ctx->err, 1);
                }
                ret = LSOF_ERROR_NO_MEMORY;
                goto cleanup;
            }
            /*
             * The protocol name should be "tcp", "udp" or "udplite".
             */
            if (strcasecmp(str_proto, "tcp") == 0) {
                proto = LSOF_PROTOCOL_TCP;
            } else if (strcasecmp(str_proto, "udp") == 0) {
                proto = LSOF_PROTOCOL_UDP;
            } else if (strcasecmp(str_proto, "udplite") == 0) {
                proto = LSOF_PROTOCOL_UDPLITE;
            } else {
                if (ctx->err) {
                    (void)fprintf(ctx->err,
                                  "%s: unknown protocol name (%s) in: -i ", Pn,
                                  str_proto);
                    safestrprt(na, ctx->err, 1);
                }
                ret = LSOF_ERROR_INVALID_ARGUMENT;
                goto cleanup;
            }
            /*
             * Convert protocol name to lower case.
             */
            for (p = str_proto; *p; p++) {
                if (*p >= 'A' && *p <= 'Z')
                    *p = *p - 'A' + 'a';
            }
        }
    }

    /*
     * Process an IPv4 address (1.2.3.4), IPv6 address ([1:2:3:4:5:6:7:8]),
     * or host name, preceded by a '@' and optionally followed by a colon
     * and a service name or port number.
     */
    if (*wa == '@') {
        wa++;
        if (!*wa || *wa == ':') {

#if defined(HASIPv6)
        unacc_address:
#endif /* defined(HASIPv6) */
            if (ctx->err) {
                (void)fprintf(ctx->err,
                              "%s: unacceptable Internet address in: -i ", Pn);
                safestrprt(na, ctx->err, 1);
            }
            ret = LSOF_ERROR_INVALID_ARGUMENT;
            goto cleanup;
        }

        if ((p = isIPv4addr(wa, a, sizeof(a)))) {

            /*
             * Process IPv4 address.
             */
            if (af == AF_INET6) {
                if (ctx->err) {
                    (void)fprintf(ctx->err,
                                  "%s: IPv4 addresses are prohibited: -i ", Pn);
                    safestrprt(na, ctx->err, 1);
                }
                ret = LSOF_ERROR_INVALID_ARGUMENT;
                goto cleanup;
            }
            wa = p;
            af = AF_INET;
        } else if (*wa == '[') {

#if defined(HASIPv6)
            /*
             * Make sure IPv6 addresses are permitted.  If they are, assemble
             * one.
             */
            if (af == AF_INET) {
                if (ctx->err) {
                    (void)fprintf(ctx->err,
                                  "%s: IPv6 addresses are prohibited: -i ", Pn);
                    safestrprt(na, ctx->err, 1);
                }
                ret = LSOF_ERROR_INVALID_ARGUMENT;
                goto cleanup;
            }
            if (!(cp = strrchr(++wa, ']')))
                goto unacc_address;
            *cp = '\0';
            i = inet_pton(AF_INET6, wa, (void *)&a);
            *cp = ']';
            if (i != 1)
                goto unacc_address;
            for (ae = i = 0; i < MAX_AF_ADDR; i++) {
                if ((ae |= a[i]))
                    break;
            }
            if (!ae)
                goto unacc_address;
            if (IN6_IS_ADDR_V4MAPPED((struct in6_addr *)&a[0])) {
                if (af == AF_INET6) {
                    if (ctx->err) {

                        (void)fprintf(ctx->err,
                                      "%s: IPv4 addresses are prohibited: -i ",
                                      Pn);
                        safestrprt(na, ctx->err, 1);
                    }
                    ret = LSOF_ERROR_INVALID_ARGUMENT;
                    goto cleanup;
                }
                for (i = 0; i < 4; i++) {
                    a[i] = a[i + 12];
                }
                af = AF_INET;
            } else
                af = AF_INET6;
            wa = cp + 1;
#else  /* !defined(HASIPv6) */
            if (ctx->err) {
                (void)fprintf(ctx->err, "%s: unsupported IPv6 address in: -i ",
                              Pn);
                safestrprt(na, ctx->err, 1);
            }
            ret = LSOF_ERROR_UNSUPPORTED;
            goto cleanup;
#endif /* defined(HASIPv6) */

        } else {

            /*
             * Assemble host name.
             */
            for (p = wa; *p && *p != ':'; p++)
                ;
            if ((l = p - wa)) {
                if (!(hn = mkstrcat(wa, l, (char *)NULL, -1, (char *)NULL, -1,
                                    (MALLOC_S *)NULL))) {
                    if (ctx->err) {
                        (void)fprintf(ctx->err,
                                      "%s: no space for host name: -i ", Pn);
                        safestrprt(na, ctx->err, 1);
                    }
                    ret = LSOF_ERROR_NO_MEMORY;
                    goto cleanup;
                }

                /* Resolve hostnames */
                memset(&hint, 0, sizeof(hint));
                hint.ai_family = af;

                if (getaddrinfo(hn, NULL, &hint, &he)) {
                    if (ctx->err) {
                        fprintf(ctx->err, "%s: unknown host name (%s) in: -i ",
                                Pn, hn);
                        safestrprt(na, ctx->err, 1);
                    }
                    ret = LSOF_ERROR_INVALID_ARGUMENT;
                    goto cleanup;
                }
            }
            wa = p;
        }
    }
    /*
     * If there is no port number, enter the address.
     */
    if (!*wa)
        goto nwad_enter;
    /*
     * Process a service name or port number list, preceded by a colon.
     *
     * Entries of the list are separated with commas; elements of a numeric
     * range are specified with a separating minus sign (`-'); all service names
     * must belong to the same protocol; embedded spaces are not allowed.  An
     * embedded minus sign in a name is taken to be part of the name, the
     * starting entry of a range can't be a service name.
     */
    if (*wa != ':' || *(wa + 1) == '\0') {

    unacc_port:
        if (ctx->err) {

            (void)fprintf(ctx->err,
                          "%s: unacceptable port specification in: -i ", Pn);
            safestrprt(na, ctx->err, 1);
        }
        ret = LSOF_ERROR_INVALID_ARGUMENT;
        goto cleanup;
    }
    for (++wa; wa && *wa; wa++) {
        for (ep = pr = sp = 0; *wa; wa++) {
            if (*wa < '0' || *wa > '9') {

                /*
                 * Convert service name to port number, using already-specified
                 * protocol name.  A '-' is taken to be part of the name; hence
                 * the starting entry of a range can't be a service name.
                 */
                for (p = wa; *wa && *wa != ','; wa++)
                    ;
                if (!(l = wa - p)) {
                    if (ctx->err) {
                        (void)fprintf(ctx->err, "%s: invalid service name: -i ",
                                      Pn);
                        safestrprt(na, ctx->err, 1);
                    }
                    ret = LSOF_ERROR_INVALID_ARGUMENT;
                    goto cleanup;
                }
                if (sn) {
                    if (l > snl) {
                        sn = (char *)realloc((MALLOC_P *)sn, l + 1);
                        snl = l;
                    }
                } else {
                    sn = (char *)malloc(l + 1);
                    snl = l;
                }
                if (!sn) {
                    if (ctx->err) {
                        (void)fprintf(ctx->err,
                                      "%s: no space for service name: -i ", Pn);
                        safestrprt(na, ctx->err, 1);
                    }
                    ret = LSOF_ERROR_INVALID_ARGUMENT;
                    goto cleanup;
                }
                (void)strncpy(sn, p, l);
                *(sn + l) = '\0';
                if (proto != LSOF_PROTOCOL_INVALID) {
                    /*
                     * If the protocol has been specified, look up the port
                     * number for the service name for the specified protocol.
                     */
                    if (!(se = getservbyname(sn, str_proto))) {
                        if (ctx->err) {
                            (void)fprintf(
                                ctx->err,
                                "%s: unknown service %s for %s in: -i ", Pn, sn,
                                str_proto);
                            safestrprt(na, ctx->err, 1);
                        }
                        ret = LSOF_ERROR_INVALID_ARGUMENT;
                        goto cleanup;
                    }
                    pt = (int)ntohs(se->s_port);
                } else {

                    /*
                     * If no protocol has been specified, look up the port
                     * numbers for the service name for both TCP and UDP.
                     */
                    if ((se = getservbyname(sn, "tcp")))
                        pt = (int)ntohs(se->s_port);
                    if ((se1 = getservbyname(sn, "udp")))
                        pu = (int)ntohs(se1->s_port);
                    if (!se && !se1) {
                        if (ctx->err) {
                            (void)fprintf(ctx->err,
                                          "%s: unknown service %s in: -i ", Pn,
                                          sn);
                            safestrprt(na, ctx->err, 1);
                        }
                        ret = LSOF_ERROR_INVALID_ARGUMENT;
                        goto cleanup;
                    }
                    if (se && se1 && pt != pu) {
                        if (ctx->err) {
                            (void)fprintf(
                                ctx->err,
                                "%s: TCP=%d and UDP=%d %s ports conflict;\n",
                                Pn, pt, pu, sn);
                            (void)fprintf(
                                ctx->err,
                                "      specify \"tcp:%s\" or \"udp:%s\": -i ",
                                sn, sn);
                            safestrprt(na, ctx->err, 1);
                        }
                        ret = LSOF_ERROR_INVALID_ARGUMENT;
                        goto cleanup;
                    }
                    if (!se && se1)
                        pt = pu;
                }
                if (pr)
                    ep = pt;
                else {
                    sp = pt;
                    if (*wa == '-')
                        pr++;
                }
            } else {

                /*
                 * Assemble port number.
                 */
                for (; *wa && *wa != ','; wa++) {
                    if (*wa == '-') {
                        if (pr)
                            goto unacc_port;
                        pr++;
                        break;
                    }
                    if (*wa < '0' || *wa > '9')
                        goto unacc_port;
                    if (pr)
                        ep = (ep * 10) + *wa - '0';
                    else
                        sp = (sp * 10) + *wa - '0';
                }
            }
            if (!*wa || *wa == ',')
                break;
            if (pr)
                continue;
            goto unacc_port;
        }
        if (!pr)
            ep = sp;
        if (ep < sp)
            goto unacc_port;
        /*
         * Enter completed port or port range specification.
         */

    nwad_enter:

        for (; he; he = he->ai_next) {
            if (he->ai_family == AF_INET &&
                he->ai_addrlen == sizeof(struct sockaddr_in) && he->ai_addr) {
                struct sockaddr_in *in = (struct sockaddr_in *)he->ai_addr;
                addr_len = sizeof(struct in_addr);
                addr = &in->sin_addr;
            } else if (he->ai_family == AF_INET6 &&
                       he->ai_addrlen == sizeof(struct sockaddr_in6) &&
                       he->ai_addr) {
                struct sockaddr_in6 *in6 = (struct sockaddr_in6 *)he->ai_addr;
                addr_len = sizeof(struct in6_addr);
                addr = &in6->sin6_addr;
            } else {
                continue;
            }
            if ((ret = lsof_select_inet_internal(ctx, na, proto, he->ai_family,
                                                 addr_len, addr, sp, ep))) {
                goto cleanup;
            }
        }
        if (!*wa)
            break;
    }

cleanup:
    if (he) {
        freeaddrinfo(he);
    }
    CLEAN(str_proto);
    CLEAN(hn);
    CLEAN(sn);
    return ret;
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
    char *ec;                       /* pointer to copy of path */
    efsys_list_t *ep;               /* file system path list pointer */
    int i;                          /* temporary index */
    char *path;                     /* Readlink() of file system path */
    struct mounts *mp = NULL, *mpw; /* local mount table pointers */

    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    if (!orig_path || (*orig_path != '/')) {
        if (ctx->err && !Fwarn) {
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

    /*
     * If there are file systems specified by -e options, check them.
     */

    if ((mpw = readmnt(ctx))) {
        for (; mpw; mpw = mpw->next) {
            if (!strcmp(mpw->dir, path)) {
                mp = mpw;
                break;
            }
        }
        if (!mp) {
            if (ctx->err) {
                (void)fprintf(ctx->err,
                              "%s: \"-e %s\" is not a mounted file system.\n",
                              Pn, path);
                return LSOF_ERROR_INVALID_ARGUMENT;
            }
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
    ep->mp = mp;
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

/*
 * hash_zn() - hash zone name
 */
int hash_zn(char *zn) /* zone name */
{
    int i, h;
    size_t l;

    if (!(l = strlen(zn)))
        return (0);
    if (l == 1)
        return ((int)*zn & (HASHZONE - 1));
    for (i = h = 0; i < (int)(l - 1); i++) {
        h ^= ((int)zn[i] * (int)zn[i + 1]) << ((i * 3) % 13);
    }
    return (h & (HASHZONE - 1));
}

enum lsof_error lsof_select_solaris_zone(struct lsof_context *ctx, char *zn) {
#if defined(HASZONES)
    int zh;
    znhash_t *zp, *zpn;
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    /*
     * Allocate zone argument hash space, as required.
     */
    if (!ZoneArg) {
        if (!(ZoneArg = (znhash_t **)calloc(HASHZONE, sizeof(znhash_t *)))) {
            if (ctx->err)
                (void)fprintf(ctx->err, "%s: no space for zone arg hash\n", Pn);
            Error(ctx);
            return LSOF_ERROR_NO_MEMORY;
        }
    }
    /*
     * Hash the zone name and search the argument hash.
     */
    zh = hash_zn(zn);
    for (zp = ZoneArg[zh]; zp; zp = zp->next) {
        if (!strcmp(zp->zn, zn))
            break;
    }
    if (zp) {
        /*
         * Process a duplicate.
         */
        if (ctx->err && !Fwarn)
            (void)fprintf(ctx->err, "%s: duplicate zone name: %s\n", Pn, zn);
        return LSOF_SUCCESS;
    }
    /*
     * Create a new hash entry and link it to its bucket.
     */
    if (!(zpn = (znhash_t *)malloc((MALLOC_S)sizeof(znhash_t)))) {
        (void)fprintf(ctx->err, "%s no hash space for zone: %s\n", Pn, zn);
        Error(ctx);
        return LSOF_ERROR_NO_MEMORY;
    }
    zpn->f = 0;
    zpn->zn = zn;
    zpn->next = ZoneArg[zh];
    ZoneArg[zh] = zpn;

    /* Update selection flags */
    Selflags |= SELZONE;

    return LSOF_SUCCESS;
#else
    return LSOF_ERROR_UNSUPPORTED;
#endif /* defined(HASZONES) */
}

#if defined(HASSELINUX)
_PROTOTYPE(extern int cmp_cntx_eq, (char *pcntx, char *ucntx));

#    include <fnmatch.h>

/*
 * cmp_cntx_eq -- compare program and user security contexts
 */
int cmp_cntx_eq(char *pcntx, /* program context */
                char *ucntx) /* user supplied context */
{
    return !fnmatch(ucntx, pcntx, 0);
}

#endif /* defined(HASSELINUX) */

enum lsof_error lsof_select_selinux_context(struct lsof_context *ctx,
                                            char *cntx) {
#if defined(HASSELINUX)
    cntxlist_t *cntxp;
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    /*
     * Search the argument list for a duplicate.
     */
    for (cntxp = CntxArg; cntxp; cntxp = cntxp->next) {
        if (!strcmp(cntxp->cntx, cntx)) {
            if (ctx->err && !Fwarn) {
                (void)fprintf(ctx->err, "%s: duplicate context: %s\n", Pn,
                              cntx);
            }
            return LSOF_SUCCESS;
        }
    }

    /*
     * Create and link a new context argument list entry.
     */
    if (!(cntxp = (cntxlist_t *)malloc((MALLOC_S)sizeof(cntxlist_t)))) {
        if (ctx->err) {
            (void)fprintf(ctx->err, "%s: no space for context: %s\n", Pn, cntx);
        }
        Error(ctx);
        return LSOF_ERROR_NO_MEMORY;
    }
    cntxp->f = 0;
    cntxp->cntx = cntx;
    cntxp->next = CntxArg;
    CntxArg = cntxp;

    /* Update selection flags */
    Selflags |= SELCNTX;
    return LSOF_SUCCESS;
#else
    return LSOF_ERROR_UNSUPPORTED;
#endif /* defined(HASSELINUX) */
}

/*
 * ck_file_arg() - check file arguments
 */
enum lsof_error
ck_file_arg(struct lsof_context *ctx, char *arg, /* argument */
            int fv,           /* Ffilesys value (real or temporary)
                               *    0 = paths may be file systems
                               *    1 = paths are just files
                               *    2 = paths must be file systems
                               */
            int rs,           /* Readlink() status if argument count == 1:
                               *	0 = undone, 1 = done */
            struct stat *sbp, /* if non-NULL, pointer to stat(2) buffer
                               * when argument count == 1 */
            int accept_deleted_file) /* if non-zero, don't report an error
                                      * even when the file doesn't exist. */
{
    char *ap, *fnm, *fsnm, *path;
    enum lsof_error err = LSOF_SUCCESS;
    int fsm, ftype, j, k;
    MALLOC_S l;
    struct mounts *mp;
    struct mounts **mmp = (struct mounts **)NULL;
    int mx, nm;
    static int nma = 0;
    struct stat sb;
    struct sfile *sfp;
    short ss = 0;
    int err_stat = 0;

#if defined(HASPROCFS)
    unsigned char ad, an;
    int pfsnl = -1;
    pid_t pid;
    struct procfsid *pfi;
#endif /* defined(HASPROCFS) */

    if (rs)
        path = arg;
    else {
        if (!(path = Readlink(ctx, arg))) {
            err_stat = 1;
            err = LSOF_ERROR_INVALID_ARGUMENT;
            goto cleanup;
        }
    }
    /*
     * Remove terminating `/' characters from paths longer than one.
     */
    j = k = strlen(path);
    while ((k > 1) && (path[k - 1] == '/')) {
        k--;
    }
    if (k < j) {
        if (path != arg)
            path[k] = '\0';
        else {
            if (!(ap = (char *)malloc((MALLOC_S)(k + 1)))) {
                if (ctx->err)
                    (void)fprintf(ctx->err, "%s: no space for copy of %s\n", Pn,
                                  path);
                Error(ctx);
                err = LSOF_ERROR_NO_MEMORY;
                goto cleanup;
            }
            (void)strncpy(ap, path, k);
            ap[k] = '\0';
            path = ap;
            ap = NULL;
        }
    }
    /*
     * Check for file system argument.
     */
    for (ftype = 1, mp = readmnt(ctx), nm = 0; (fv != 1) && mp; mp = mp->next) {
        fsm = 0;
        if (strcmp(mp->dir, path) == 0)
            fsm++;
        else if (fv == 2 || (mp->fs_mode & S_IFMT) == S_IFBLK) {
            if (mp->fsnmres && strcmp(mp->fsnmres, path) == 0)
                fsm++;
        }
        if (!fsm)
            continue;
        ftype = 0;
        /*
         * Skip duplicates.
         */
        for (mx = 0; mx < nm; mx++) {
            if (strcmp(mp->dir, mmp[mx]->dir) == 0 && mp->dev == mmp[mx]->dev &&
                mp->rdev == mmp[mx]->rdev && mp->inode == mmp[mx]->inode)
                break;
        }
        if (mx < nm)
            continue;
        /*
         * Allocate space for and save another mount point match and
         * the type of match -- directory name (mounted) or file system
         * name (mounted-on).
         */
        if (nm >= nma) {
            nma += 5;
            l = (MALLOC_S)(nma * sizeof(struct mounts *));
            if (mmp)
                mmp = (struct mounts **)realloc((MALLOC_P *)mmp, l);
            else
                mmp = (struct mounts **)malloc(l);
            if (!mmp) {
                if (ctx->err)
                    (void)fprintf(ctx->err, "%s: no space for mount pointers\n",
                                  Pn);
                Error(ctx);
                err = LSOF_ERROR_NO_MEMORY;
                goto cleanup;
            }
        }
        mmp[nm++] = mp;
    }
    if (fv == 2 && nm == 0) {
        if (!accept_deleted_file) {
            if (ctx->err) {
                (void)fprintf(ctx->err, "%s: not a file system: ", Pn);
                safestrprt(arg, ctx->err, 1);
            }
        }
        err_stat = 1;
        err = LSOF_ERROR_INVALID_ARGUMENT;
        goto cleanup;
    }
    /*
     * Loop through the file system matches.  If there were none, make one
     * pass through the loop, using simply the path name.
     */
    mx = 0;
    do {

        /*
         * Allocate an sfile structure and fill in the type and link.
         */
        if (!(sfp = (struct sfile *)malloc(sizeof(struct sfile)))) {
            if (ctx->err)
                (void)fprintf(ctx->err, "%s: no space for files\n", Pn);
            Error(ctx);
            err = LSOF_ERROR_NO_MEMORY;
            goto cleanup;
        }
        sfp->next = Sfile;
        Sfile = sfp;
        sfp->f = 0;
        if ((sfp->type = ftype)) {

            /*
             * For a non-file system path, use the path as the file name
             * and set a NULL file system name.
             */
            fnm = path;
            fsnm = (char *)NULL;
            /*
             * Stat the path to obtain its characteristics.
             */
            if (sbp)
                sb = *sbp;
            else {
                if (statsafely(ctx, fnm, &sb) != 0) {
                    int en = errno;
                    if (!accept_deleted_file && ctx->err) {
                        (void)fprintf(ctx->err, "%s: status error on ", Pn);
                        safestrprt(fnm, ctx->err, 0);
                        (void)fprintf(ctx->err, ": %s\n", strerror(en));
                    }
                    Sfile = sfp->next;
                    err_stat = 1;
                    CLEAN(sfp);
                    CLEAN(path);
                    continue;
                }

#if defined(HASSPECDEVD)
                (void)HASSPECDEVD(ctx, fnm, &sb);
#endif /* defined(HASSPECDEVD) */
            }
            sfp->i = (INODETYPE)sb.st_ino;
            sfp->mode = sb.st_mode & S_IFMT;

            sfp->dev = sb.st_dev;
            sfp->rdev = sb.st_rdev;

#if defined(CKFA_MPXCHAN)
            /*
             * Save a (possible) multiplexed channel number.  (This is an
             * AIX artifact.)
             */
            sfp->ch = getchan(path);
#endif /* defined(CKFA_MPXCHAN) */

        } else {

#if defined(SAVE_MP_IN_SFILE)
            sfp->mp = mp = mmp[mx++];
#else  /* !defined(SAVE_MP_IN_SFILE) */
            mp = mmp[mx++];
#endif /* defined(SAVE_MP_IN_SFILE) */

            ss++;

#if defined(HASPROCFS)
            /*
             * If this is a /proc file system, set the search flag and
             * abandon the sfile entry.
             */
            if (mp == Mtprocfs) {
                Sfile = sfp->next;
                CLEAN(sfp);
                Procsrch = 1;
                continue;
            }
#endif /* defined(HASPROCFS) */

            /*
             * Derive file name and file system name for a mount point.
             *
             * Save the device numbers, inode number, and modes.
             */
            fnm = mp->dir;
            fsnm = mp->fsname;
            sfp->dev = mp->dev;
            sfp->rdev = mp->rdev;
            sfp->i = mp->inode;
            sfp->mode = mp->mode & S_IFMT;
        }
        ss = 1; /* indicate a "safe" stat() */
        /*
         * Store the file name and file system name pointers in the sfile
         * structure, allocating space as necessary.
         */
        if (!fnm || fnm == path) {
            sfp->name = fnm;

#if defined(HASPROCFS)
            an = 0;
#endif /* defined(HASPROCFS) */

        } else {
            if (!(sfp->name = mkstrcpy(fnm, (MALLOC_S *)NULL))) {
                if (ctx->err) {
                    (void)fprintf(ctx->err, "%s: no space for file name: ", Pn);
                    safestrprt(fnm, ctx->err, 1);
                }
                Error(ctx);
                err = LSOF_ERROR_NO_MEMORY;
                goto cleanup;
            }

#if defined(HASPROCFS)
            an = 1;
#endif /* defined(HASPROCFS) */
        }
        if (!fsnm || fsnm == path) {
            sfp->devnm = fsnm;

#if defined(HASPROCFS)
            ad = 0;
#endif /* defined(HASPROCFS) */

        } else {
            if (!(sfp->devnm = mkstrcpy(fsnm, (MALLOC_S *)NULL))) {
                if (ctx->err) {
                    (void)fprintf(ctx->err,
                                  "%s: no space for file system name: ", Pn);
                    safestrprt(fsnm, ctx->err, 1);
                }
                Error(ctx);
                err = LSOF_ERROR_NO_MEMORY;
                goto cleanup;
            }

#if defined(HASPROCFS)
            ad = 1;
#endif /* defined(HASPROCFS) */
        }
        if (!(sfp->aname = mkstrcpy(arg, (MALLOC_S *)NULL))) {
            if (ctx->err) {
                (void)fprintf(ctx->err,
                              "%s: no space for argument file name: ", Pn);
                safestrprt(arg, ctx->err, 1);
            }
            Error(ctx);
            err = LSOF_ERROR_NO_MEMORY;
            goto cleanup;
        }

#if defined(HASPROCFS)
        /*
         * See if this is an individual member of a proc file system.
         */
        if (!Mtprocfs || Procsrch)
            continue;

#    if defined(HASFSTYPE) && HASFSTYPE == 1
        if (strcmp(sb.st_fstype, HASPROCFS) != 0)
            continue;
#    endif /* defined(HASFSTYPE) && HASFSTYPE==1 */

        if (pfsnl == -1)
            pfsnl = strlen(Mtprocfs->dir);
        if (!pfsnl)
            continue;
        if (strncmp(Mtprocfs->dir, path, pfsnl) != 0)
            continue;
        if (path[pfsnl] != '/')

#    if defined(HASPINODEN)
            pid = 0;
#    else  /* !defined(HASPINODEN) */
            continue;
#    endif /* defined(HASPINODEN) */

        else {
            for (j = pfsnl + 1; path[j]; j++) {
                if (!isdigit((unsigned char)path[j]))
                    break;
            }
            if (path[j] || (j - pfsnl - 1) < 1 ||
                (sfp->mode & S_IFMT) != S_IFREG)

#    if defined(HASPINODEN)
                pid = 0;
#    else  /* !defined(HASPINODEN) */
                continue;
#    endif /* defined(HASPINODEN) */

            else
                pid = atoi(&path[pfsnl + 1]);
        }
        if (!(pfi = (struct procfsid *)malloc(
                  (MALLOC_S)sizeof(struct procfsid)))) {
            if (ctx->err) {
                (void)fprintf(ctx->err, "%s: no space for %s ID: ", Pn,
                              Mtprocfs->dir);
                safestrprt(path, ctx->err, 1);
            }
            Error(ctx);
            err = LSOF_ERROR_NO_MEMORY;
            goto cleanup;
        }
        pfi->pid = pid;
        pfi->f = 0;
        pfi->nm = sfp->aname;
        pfi->next = Procfsid;
        Procfsid = pfi;

#    if defined(HASPINODEN)
        pfi->inode = (INODETYPE)sfp->i;
#    endif /* defined(HASPINODEN) */

        /*
         * Abandon the Sfile entry, lest it be used in is_file_named().
         */
        Sfile = sfp->next;
        if (ad)
            (void)free((FREE_P *)sfp->devnm);
        if (an)
            (void)free((FREE_P *)sfp->name);
        CLEAN(sfp);
#endif /* defined(HASPROCFS) */

    } while (mx < nm);

cleanup:
    CLEAN(path);
    CLEAN(mmp);
    if (err == LSOF_SUCCESS) {
        if (accept_deleted_file) {
            if (!ss && err_stat == 0)
                err = LSOF_ERROR_INVALID_ARGUMENT;
        } else if (!ss) {
            err = LSOF_ERROR_INVALID_ARGUMENT;
        }
    }

    /* Update selection flags */
    if (err == LSOF_SUCCESS)
        Selflags |= SELNM;
    return err;
}

enum lsof_error lsof_select_file(struct lsof_context *ctx, char *path,
                                 int flags) {

    int fv = 0;
    int accept_deleted_file = 0;
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    if (flags &
        (LSOF_SELECT_FILE_ONLY_FILES | LSOF_SELECT_FILE_ONLY_FILE_SYSTEMS)) {
        /* conflcit */
        return LSOF_ERROR_INVALID_ARGUMENT;
    } else if (flags & LSOF_SELECT_FILE_ONLY_FILES) {
        /* paths are files */
        fv = 1;
    } else if (flags & LSOF_SELECT_FILE_ONLY_FILE_SYSTEMS) {
        /* paths are file systems */
        fv = 2;
    }

    if (flags & LSOF_SELECT_FILE_ACCEPT_DELETED) {
        accept_deleted_file = 1;
    }

    return ck_file_arg(ctx, path, fv, 0, NULL, accept_deleted_file);
}

enum lsof_error lsof_use_name_cache(struct lsof_context *ctx, int enable) {
    if (!ctx || ctx->frozen) {
        return LSOF_ERROR_INVALID_ARGUMENT;
    }

    Fncache = enable;

    return LSOF_SUCCESS;
}