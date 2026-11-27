/*
 * dnode.c - Solaris /proc node compatibility shims.
 *
 * The procfs backend fills lfile entries directly from /proc.  These
 * functions remain only for the mount-table and file-selection support
 * code; they never read kernel memory.
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

/*
 * Common stream/AFS code still references kread.  The procfs backend has no
 * kernel memory source, so those optional paths simply fail.
 */
int kread(struct lsof_context *ctx, KA_T addr, char *buf, READLEN_T len) {
    (void)ctx;
    (void)addr;
    (void)buf;
    (void)len;
    return (1);
}

/* Required by the lsof core; procfs needs no one-time kernel setup. */
void initialize(struct lsof_context *ctx) {
    (void)ctx;
}

/* Zone-name filtering is not available in the procfs backend. */
int enter_zone_arg(struct lsof_context *ctx, char *zn) {
    (void)ctx;
    (void)zn;
    return (1);
}

/*
 * build_IPstates() - build the TCP and UDP state tables
 */

void build_IPstates(struct lsof_context *ctx) {
    if (!TcpSt) {
        (void)enter_IPstate(ctx, "TCP", "CLOSED", TCPS_CLOSED);
        (void)enter_IPstate(ctx, "TCP", "IDLE", TCPS_IDLE);
        (void)enter_IPstate(ctx, "TCP", "BOUND", TCPS_BOUND);
        (void)enter_IPstate(ctx, "TCP", "LISTEN", TCPS_LISTEN);
        (void)enter_IPstate(ctx, "TCP", "SYN_SENT", TCPS_SYN_SENT);
        (void)enter_IPstate(ctx, "TCP", "SYN_RCVD", TCPS_SYN_RCVD);
        (void)enter_IPstate(ctx, "TCP", "ESTABLISHED", TCPS_ESTABLISHED);
        (void)enter_IPstate(ctx, "TCP", "CLOSE_WAIT", TCPS_CLOSE_WAIT);
        (void)enter_IPstate(ctx, "TCP", "FIN_WAIT_1", TCPS_FIN_WAIT_1);
        (void)enter_IPstate(ctx, "TCP", "CLOSING", TCPS_CLOSING);
        (void)enter_IPstate(ctx, "TCP", "LAST_ACK", TCPS_LAST_ACK);
        (void)enter_IPstate(ctx, "TCP", "FIN_WAIT_2", TCPS_FIN_WAIT_2);
        (void)enter_IPstate(ctx, "TCP", "TIME_WAIT", TCPS_TIME_WAIT);
        (void)enter_IPstate(ctx, "TCP", (char *)NULL, 0);
    }
    if (!UdpSt) {
        (void)enter_IPstate(ctx, "UDP", "Unbound", TS_UNBND);
        (void)enter_IPstate(ctx, "UDP", "Wait_BIND_REQ_Ack", TS_WACK_BREQ);
        (void)enter_IPstate(ctx, "UDP", "Wait_UNBIND_REQ_Ack", TS_WACK_UREQ);
        (void)enter_IPstate(ctx, "UDP", "Idle", TS_IDLE);
        (void)enter_IPstate(ctx, "UDP", "Wait_OPT_REQ_Ack", TS_WACK_OPTREQ);
        (void)enter_IPstate(ctx, "UDP", "Wait_CONN_REQ_Ack", TS_WACK_CREQ);
        (void)enter_IPstate(ctx, "UDP", "Wait_CONN_REQ_Confirm", TS_WCON_CREQ);
        (void)enter_IPstate(ctx, "UDP", "Wait_CONN_IND_Response", TS_WRES_CIND);
        (void)enter_IPstate(ctx, "UDP", "Wait_CONN_RES_Ack", TS_WACK_CRES);
        (void)enter_IPstate(ctx, "UDP", "Wait_Data_Xfr", TS_DATA_XFER);
        (void)enter_IPstate(ctx, "UDP", "Wait_Read_Release", TS_WIND_ORDREL);
        (void)enter_IPstate(ctx, "UDP", "Wait_Write_Release", TS_WREQ_ORDREL);
        (void)enter_IPstate(ctx, "UDP", "Wait_DISCON_REQ_Ack", TS_WACK_DREQ6);
        (void)enter_IPstate(ctx, "UDP", "Wait_DISCON_REQ_Ack", TS_WACK_DREQ7);
        (void)enter_IPstate(ctx, "UDP", "Wait_DISCON_REQ_Ack", TS_WACK_DREQ9);
        (void)enter_IPstate(ctx, "UDP", "Wait_DISCON_REQ_Ack", TS_WACK_DREQ10);
        (void)enter_IPstate(ctx, "UDP", "Wait_DISCON_REQ_Ack", TS_WACK_DREQ11);
        (void)enter_IPstate(ctx, "UDP", (char *)NULL, 0);
    }
}
