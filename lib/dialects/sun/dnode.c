/*
 * dnode.c - Solaris /proc node compatibility shims.
 *
 * The procfs backend fills lfile entries directly from /proc.  These
 * functions remain only for the mount-table and file-selection support
 * code; they never read kernel memory.
 */

#include "common.h"

void process_node(struct lsof_context *ctx, KA_T f) {
    (void)ctx;
    (void)f;
}

int vop2ty(struct lsof_context *ctx, struct vnode *vp, int fx) {
    (void)ctx;
    (void)vp;
    (void)fx;
    return N_REGLR;
}

void open_kvm(struct lsof_context *ctx) {
    (void)ctx;
}

void close_kvm(struct lsof_context *ctx) {
    (void)ctx;
}

int kread(struct lsof_context *ctx, KA_T addr, char *buf, READLEN_T len) {
    (void)ctx;
    (void)addr;
    (void)buf;
    (void)len;
    return (1);
}

void initialize(struct lsof_context *ctx) {
    (void)ctx;
}

int enter_zone_arg(struct lsof_context *ctx, char *zn) {
    (void)ctx;
    (void)zn;
    return (0);
}

void build_IPstates(struct lsof_context *ctx) {
    (void)ctx;
}

void read_v_path(struct lsof_context *ctx, KA_T ka, char *rb, size_t rbl) {
    (void)ctx;
    (void)ka;
    if (rb && rbl)
        rb[0] = '\0';
}
