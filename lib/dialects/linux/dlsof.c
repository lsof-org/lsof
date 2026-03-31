/*
 * dlsof.c - Linux dialect-specific implementations
 */

#include "common.h"
#include "dlsof.h"

/*
 * lsof_dialect_destroy() - clean up dialect-specific resources
 */

void lsof_dialect_destroy(struct lsof_context *ctx) {
    int i;
    struct llock *lp, *np;
#if defined(HASEPTOPTS)
    pxinfo_t *pp, *pnp;
#endif

    /* Free lock hash table */
    if (LckH) {
        for (i = 0; i < PIDBUCKS; i++) {
            for (lp = LckH[i]; lp; lp = np) {
                np = lp->next;
                (void)free((FREE_P *)lp);
            }
            LckH[i] = (struct llock *)NULL;
        }
        CLEAN(LckH);
    }

#if defined(HASEPTOPTS)
    /* Free pipe endpoint hash */
    if (Pinfo) {
        for (i = 0; i < PINFOBUCKS; i++) {
            for (pp = Pinfo[i]; pp; pp = pnp) {
                pnp = pp->next;
                (void)free((FREE_P *)pp);
            }
            Pinfo[i] = (pxinfo_t *)NULL;
        }
        CLEAN(Pinfo);
    }

#    if defined(HASPTYEPT)
    /* Free PTY endpoint hash */
    if (PtyInfo) {
        for (i = 0; i < PINFOBUCKS; i++) {
            for (pp = PtyInfo[i]; pp; pp = pnp) {
                pnp = pp->next;
                (void)free((FREE_P *)pp);
            }
            PtyInfo[i] = (pxinfo_t *)NULL;
        }
        CLEAN(PtyInfo);
    }
#    endif

    /* Free message queue endpoint hash */
    if (PSXMQinfo) {
        for (i = 0; i < PINFOBUCKS; i++) {
            for (pp = PSXMQinfo[i]; pp; pp = pnp) {
                pnp = pp->next;
                (void)free((FREE_P *)pp);
            }
            PSXMQinfo[i] = (pxinfo_t *)NULL;
        }
        CLEAN(PSXMQinfo);
    }

    /* Free eventfd endpoint hash */
    if (EvtFDinfo) {
        for (i = 0; i < PINFOBUCKS; i++) {
            for (pp = EvtFDinfo[i]; pp; pp = pnp) {
                pnp = pp->next;
                (void)free((FREE_P *)pp);
            }
            EvtFDinfo[i] = (pxinfo_t *)NULL;
        }
        CLEAN(EvtFDinfo);
    }
#endif /* defined(HASEPTOPTS) */

    /* Free field pointers */
    if (Fp) {
        CLEAN(Fp);
        Fpa = 0;
    }

    /* Free lock stream buffer */
    if (Vbuf) {
        CLEAN(Vbuf);
        Vsz = 0;
    }
}
