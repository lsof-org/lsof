/*
 * dsock.c - NetBSD and OpenBSD socket processing functions for lsof
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

#ifndef lint
static char copyright[] =
"@(#) Copyright 1994 Purdue Research Foundation.\nAll rights reserved.\n";
#endif

#include "lsof.h"

/*
 * process_socket() - process socket
 */
void
process_socket(struct kinfo_file *file)
{
	char *type_name = NULL;
	char *proto = NULL;
	char *type = NULL;
	char buf[64];
	int flag;

	/* Alloc Lf and set fd */
	alloc_lfile(NULL, file->fd_fd);

	/* Type name */
	switch (file->so_family) {
	case AF_INET:
	     type_name = "IPv4";
	     break;
	case AF_INET6:
	     type_name = "IPv6";
	     break;
	case AF_UNIX:
	     type_name = "unix";
	     break;
	case AF_ROUTE:
	     type_name = "rte";
	     break;
	}
	if (type_name)
	    (void) snpf(Lf->type, sizeof(Lf->type), "%s", type_name);

	/*
	 * Construct access code.
	 */
	if (file->fd_fd >= 0) {
	    if ((flag = (file->f_flag & (FREAD | FWRITE))) == FREAD)
	        Lf->access = 'r';
	    else if (flag == FWRITE)
	        Lf->access = 'w';
	    else if (flag == (FREAD | FWRITE))
	        Lf->access = 'u';
	}

	/* Fill iproto */
	switch (file->so_type) {
	case SOCK_STREAM:
	    proto = "TCP";
	    break;
	case SOCK_DGRAM:
	    proto = "UDP";
	    break;
	case SOCK_RAW:
	    proto = "RAW";
	    break;
	}
	if (proto) {
	    (void) snpf(Lf->iproto, sizeof(Lf->iproto), "%s", proto);
	    Lf->inp_ty = 2;
	}

	/* Fill offset, always zero */
	Lf->off = 0;
	Lf->off_def = 1;

	if (file->so_family == AF_INET || file->so_family == AF_INET6) {
	    /* Show this entry if -i */
	    if (Fnet) {
		/* Handle v4/v6 only */
		if (FnetTy == 4 && file->so_family == AF_INET) {
		    Lf->sf |= SELNET;
		} else if (FnetTy == 6 && file->so_family == AF_INET6) {
		    Lf->sf |= SELNET;
		} else if (FnetTy == 0) {
		    Lf->sf |= SELNET;
		}
	    }

	    /* Fill internet address info */
	    if (file->inp_fport) {
	        ent_inaddr((unsigned char *)file->inp_laddru, file->inp_lport,
	            (unsigned char *)file->inp_faddru, file->inp_fport, file->so_family);
	    } else {
		/* No foreign address on LISTEN sockets */
	        ent_inaddr((unsigned char *)file->inp_laddru, file->inp_lport,
	            NULL, 0, file->so_family);
	    }

	    /* Fill TCP state */
	    if (file->so_type == SOCK_STREAM) {
	        Lf->lts.type = 0;
	        Lf->lts.state.i = file->t_state;
	    }

	    /* Fill dev with pcb if available */
	    if (file->inp_ppcb) {
	        (void) snpf(buf, sizeof(buf), "0x%llx", file->inp_ppcb);
	        enter_dev_ch(buf);
	    }
	} else if (file->so_family == AF_UNIX) {
	    /* Show this entry if requested */
	    /* Via -U */
	    if (Funix)
		Lf->sf |= SELUNX;
	    /* Name matches */
	    if (is_file_named(file->unp_path, 0)) {
		Lf->sf |= SELNM;
	    }

	    /* Fill name */
	    switch (file->so_type) {
	    case SOCK_STREAM:
	        type = "STREAM";
	        break;
	    case SOCK_DGRAM:
	        type = "DGRAM";
	        break;
	    default:
	        type = "UNKNOWN";
	        break;
	    }

	    (void) snpf(Namech, Namechl, "%s%stype=%s",
		file->unp_path[0] ? file->unp_path : "",
		file->unp_path[0] ? " " : "",
		type);
	    (void) enter_nm(Namech);

	    /* Fill TCP state */
	    if (file->so_type == SOCK_STREAM) {
	        Lf->lts.type = 0;
	        Lf->lts.state.i = file->t_state;
	    }

	    /* Fill dev with so_pcb if available */
	    if (file->so_pcb && file->so_pcb != (uint64_t)(-1)) {
	        (void) snpf(buf, sizeof(buf), "0x%llx", file->so_pcb);
	        enter_dev_ch(buf);
	    }
	} else if (file->so_family == AF_ROUTE) {
	    /* Fill dev with f_data if available */
	    if (file->f_data) {
	        (void) snpf(buf, sizeof(buf), "0x%llx", file->f_data);
	        enter_dev_ch(buf);
	    }
	}

	/* Finish */
	if (Lf->sf)
		link_lfile();
}
