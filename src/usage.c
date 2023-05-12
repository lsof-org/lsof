/*
 * usage.c - usage functions for lsof
 */

/*
 * Copyright 1998 Purdue Research Foundation, West Lafayette, Indiana
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

static char copyright[] =
    "Copyright 1998 Purdue Research Foundation. All rights reserved.";

#include "common.h"
#include "cli.h"
#include "version.h"

/*
 * Local function prototypes
 */

static char *isnullstr(char *s);
static int print_in_col(int col, char *cp);
static void report_HASDCACHE(struct lsof_context *ctx, int type, char *ttl,
                             char *det);
static void report_HASKERNIDCK(char *pfx, char *verb);
static void report_SECURITY(char *pfx, char *punct);
static void report_WARNDEVACCESS(char *pfx, char *verb, char *punct);

/*
 * isnullstr() - is it a null string?
 */

static char *isnullstr(char *s) /* string pointer */
{
    if (!s)
        return ((char *)NULL);
    while (*s) {
        if (*s != ' ')
            return (s);
        s++;
    }
    return ((char *)NULL);
}

/*
 * print_in_col() -- print character string in help column
 */

static int print_in_col(int col,  /* column number */
                        char *cp) /* what to print */
{
    if (cp && *cp) {
        switch (col) {
        case 1:
            (void)fprintf(stderr, "  %-23.23s", cp);
            break;
        case 2:
            (void)fprintf(stderr, "  %-25.25s", cp);
            break;
        default:
            (void)fprintf(stderr, "  %s\n", cp);
            col = 0;
        }
        col++;
    }
    return (col);
}

/*
 * report_HASDCACHE() -- report device cache file state
 */

static void report_HASDCACHE(struct lsof_context *ctx, /* context */
                             int type,  /* type: 0 == read path report
                                         *       1 == full report */
                             char *ttl, /* title lines prefix
                                         * (NULL if none) */
                             char *det) /* detail lines prefix
                                         * (NULL if none) */
{

#if defined(HASDCACHE)
    char *cp;
    int dx;

#    if defined(WILLDROPGID)
    int saved_Setgid = Setgid;

    Setgid = 0;
#    endif /* defined(WILLDROPGID) */

    if (type) {

        /*
         * Report full device cache information.
         */
        (void)fprintf(stderr, "%sDevice cache file read-only paths:\n",
                      ttl ? ttl : "");
        if ((dx = dcpath(ctx, 1, 0)) < 0)
            (void)fprintf(stderr, "%snone\n", det ? det : "");
        else {
            (void)fprintf(stderr, "%sNamed via -D: %s\n", det ? det : "",
                          DCpath[0] ? DCpath[0] : "none");

#    if defined(HASENVDC)
            (void)fprintf(stderr, "%sNamed in environment variable %s: %s\n",
                          det ? det : "", HASENVDC,
                          DCpath[1] ? DCpath[1] : "none");
#    endif /* defined(HASENVDC) */

#    if defined(HASSYSDC)
            if (DCpath[2])
                (void)fprintf(stderr, "%sSystem-wide device cache: %s\n",
                              det ? det : "", DCpath[2]);
#    endif /* defined(HASSYSDC) */

#    if defined(HASPERSDC)
            (void)fprintf(stderr,
                          "%sPersonal path format (HASPERSDC): \"%s\"\n",
                          det ? det : "", HASPERSDC);
#        if defined(HASPERSDCPATH)
            (void)fprintf(stderr,
                          "%sModified personal path environment variable: %s\n",
                          det ? det : "", HASPERSDCPATH);
            cp = getenv(HASPERSDCPATH);
            (void)fprintf(stderr, "%s%s value: %s\n", det ? det : "",
                          HASPERSDCPATH, cp ? cp : "none");
#        endif /* defined(HASPERSDCPATH) */
            (void)fprintf(stderr, "%sPersonal path: %s\n", det ? det : "",
                          DCpath[3] ? DCpath[3] : "none");
#    endif /* defined(HASPERSDC) */
        }
        (void)fprintf(stderr, "%sDevice cache file write paths:\n",
                      ttl ? ttl : "");
        if ((dx = dcpath(ctx, 2, 0)) < 0)
            (void)fprintf(stderr, "%snone\n", det ? det : "");
        else {
            (void)fprintf(stderr, "%sNamed via -D: %s\n", det ? det : "",
                          DCstate == 2 ? "none"
                          : DCpath[0]  ? DCpath[0]
                                       : "none");

#    if defined(HASENVDC)
            (void)fprintf(stderr, "%sNamed in environment variable %s: %s\n",
                          det ? det : "", HASENVDC,
                          DCpath[1] ? DCpath[1] : "none");
#    endif /* defined(HASENVDC) */

#    if defined(HASPERSDC)
            (void)fprintf(stderr,
                          "%sPersonal path format (HASPERSDC): \"%s\"\n",
                          det ? det : "", HASPERSDC);
#        if defined(HASPERSDCPATH)
            (void)fprintf(stderr,
                          "%sModified personal path environment variable: %s\n",
                          det ? det : "", HASPERSDCPATH);
            cp = getenv(HASPERSDCPATH);
            (void)fprintf(stderr, "%s%s value: %s\n", det ? det : "",
                          HASPERSDCPATH, cp ? cp : "none");
#        endif /* defined(HASPERSDCPATH) */
            (void)fprintf(stderr, "%sPersonal path: %s\n", det ? det : "",
                          DCpath[3] ? DCpath[3] : "none");
#    endif /* defined(HASPERSDC) */
        }
    } else {

        /*
         * Report device cache read file path.
         */

#    if defined(HASENVDC) || defined(HASPERSDC) || defined(HASSYSDC)
        cp = NULL;
#        if defined(HASENVDC)
        if ((dx = dcpath(ctx, 1, 0)) >= 0)
            cp = DCpath[1];
#        endif /* defined(HASENVDC) */
#        if defined(HASSYSDC)
        if (!cp)
            cp = HASSYSDC;
#        endif /* defined(HASSYSDC) */
#        if defined(HASPERSDC)
        if (!cp && dx != -1 && (dx = dcpath(ctx, 1, 0)) >= 0)
            cp = DCpath[3];
#        endif /* defined(HASPERSDC) */
        if (cp)
            (void)fprintf(stderr,
                          "%s%s is the default device cache file read path.\n",
                          ttl ? ttl : "", cp);
#    endif /* defined(HASENVDC) || defined(HASPERSDC) || defined(HASSYSDC) */
    }

#    if defined(WILLDROPGID)
    Setgid = saved_Setgid;
#    endif /* defined(WILLDROPGID) */

#endif /* defined(HASDCACHE) */
}

/*
 * report_HASKERNIDCK() -- report HASKERNIDCK state
 */

static void report_HASKERNIDCK(pfx, verb) char *pfx; /* prefix (NULL if none) */
char *verb;                                          /* verb (NULL if none) */
{
    (void)fprintf(stderr, "%sernel ID check %s%s%s.\n", pfx ? pfx : "",
                  verb ? verb : "", verb ? " " : "",

#if defined(HASKERNIDCK)
                  "enabled"
#else  /* !defined(HASKERNIDCK) */
                  "disabled"
#endif /* defined(HASKERNIDCK) */

    );
}

/*
 * report_SECURITY() -- report *SECURITY states
 */

static void report_SECURITY(pfx, punct) char *pfx; /* prefix (NULL if none) */
char *punct;                                       /* short foem punctuation
                                                    * (NULL if none) */
{
    fprintf(stderr, "%s%s can list all files%s", pfx ? pfx : "",

#if defined(HASSECURITY)
            "Only root",
#    if defined(HASNOSOCKSECURITY)
            ", but anyone can list socket files.\n"
#    else  /* !defined(HASNOSOCKSECURITY) */
            punct ? punct : ""
#    endif /* defined(HASNOSOCKSECURITY) */
#else      /* !defined(HASSECURITY) */
            "Anyone", punct ? punct : ""
#endif     /* defined(HASSECURITY) */

    );
}

/*
 * report_WARNDEVACCESS() -- report WEARNDEVACCESS state
 */

static void report_WARNDEVACCESS(pfx, verb,
                                 punct) char *pfx; /* prefix (NULL if none) */
char *verb;                                        /* verb (NULL if none) */
char *punct;                                       /* punctuation */
{
    (void)fprintf(stderr, "%s/dev warnings %s%s%s%s", pfx ? pfx : "",
                  verb ? verb : "", verb ? " " : "",

#if defined(WARNDEVACCESS)
                  "enabled",
#else  /* !defined(WARNDEVACCESS) */
                  "disabled",
#endif /* defined(WARNDEVACCESS) */

                  punct);
}

/*
 * usage() - display usage and exit
 */

void usage(struct lsof_context *ctx, /* context */
           int err,     /* it is called as part of error handlng? */
           int fh,      /* ``-F ?'' status */
           int version) /* ``-v'' status */
{
    char buf[MAXPATHLEN + 1], *cp, *cp1, *cp2;
    int col, i;

    if (Fhelp || err) {
        (void)fprintf(stderr, "%s %s\n latest revision: %s\n", Pn, LSOF_VERSION,
                      LSOF_REPO_URL);
        (void)fprintf(stderr, " latest FAQ: %s\n", LSOF_FAQ_URL);
        (void)fprintf(stderr, " latest (non-formatted) man page: %s\n",
                      LSOF_MAN_URL);
        (void)fprintf(stderr, " usage: [-?ab%shH%slnNoOP%s%stUvV%s]",

#if defined(HASNCACHE)
                      "C",
#else  /* !defined(HASNCACHE) */
                      "",
#endif /* defined(HASNCACHE) */

#if defined(HASTASKS)
                      "K",
#else  /* !defined(HASTASKS) */
                      "",
#endif /* defined(HASTASKS) */

#if defined(HASPPID)
                      "R",
#else  /* !defined(HASPPID) */
                      "",
#endif /* defined(HASPPID) */

#if defined(HASTCPUDPSTATE)
                      "",
#else  /* !defined(HASTCPUDPSTATE) */
                      "s",
#endif /* defined(HASTCPUDPSTATE) */

#if defined(HASXOPT)
#    if defined(HASXOPT_ROOT)
                      (Myuid == 0) ? "X" : ""
#    else  /* !defined(HASXOPT_ROOT) */
                      "X"
#    endif /* defined(HASXOPT_ROOT) */
#else      /* !defined(HASXOPT) */
                      ""
#endif     /* defined(HASXOPT) */

        );

#if defined(HAS_AFS) && defined(HASAOPT)
        (void)fprintf(stderr, " [-A A]");
#endif /* defined(HAS_AFS) && defined(HASAOPT) */

        (void)fprintf(stderr, " [+|-c c] [+|-d s] [+%sD D]%s",
#if defined(HASDCACHE)
                      "|-",
#else  /* !defined(HASDCACHE) */
                      "",
#endif /* defined(HASDCACHE) */

#if defined(HASEPTOPTS)
                      " [+|-E]"
#else  /* !defined(HASEPTOPTS) */
                      ""
#endif /* defined(HASEPTOPTS) */

        );

        (void)fprintf(stderr,
                      " %s[+|-f%s%s%s%s%s%s]\n [-F [f]] [-g [s]] [-i [i]]",

#if defined(HASEOPT)
                      "[+|-e s] ",
#else  /* !defined(HASEOPT) */
                      "",
#endif /* defined(HASEOPT) */

#if defined(HASFSTRUCT)
                      "[",

#    if defined(HASNOFSCOUNT)
                      "",
#    else  /* !defined(HASNOFSCOUNT) */
                      "c",
#    endif /* defined(HASNOFSCOUNT) */

#    if defined(HASNOFSADDR)
                      "",
#    else  /* !defined(HASNOFSADDR) */
                      "f",
#    endif /* defined(HASNOFSADDR) */

#    if defined(HASNOFSFLAGS)
                      "",
#    else  /* !defined(HASNOFSFLAGS) */
                      "gG",
#    endif /* defined(HASNOFSFLAGS) */

#    if defined(HASNOFSNADDR)
                      "",
#    else  /* !defined(HASNOFSNADDR) */
                      "n",
#    endif /* defined(HASNOFSNADDR) */

                      "]"
#else  /* !defined(HASFSTRUCT) */
                      "", "", "", "", "", ""
#endif /* defined(HASFSTRUCT) */

        );

#if defined(HASKOPT)
        (void)fprintf(stderr, " [-k k]");
#endif /* defined(HASKOPT) */

        (void)fprintf(stderr, " [+|-L [l]]");

#if defined(HASMOPT) || defined(HASMNTSUP)
        (void)fprintf(stderr,
#    if defined(HASMOPT)
#        if defined(HASMNTSUP)
                      " [+|-m [m]]"
#        else  /* !defined(HASMNTSUP) */
                      " [-m m]"
#        endif /* defined(HASMNTSUP) */
#    else      /* !defined(HASMOPT) */
                      " [+m [m]]"
#    endif     /* defined(HASMOPT) */
        );
#endif /* defined(HASMOPT) || defined(HASMNTSUP) */

#if !defined(HASNORPC_H)
        (void)fprintf(stderr, " [+|-M]");
#endif /* !defined(HASNORPC_H) */

        (void)fprintf(stderr,
                      " [-o [o]] [-p s]\n [+|-r [t]]%s [-S [t]] [-T [t]]",

#if defined(HASTCPUDPSTATE)
                      " [-s [p:s]]"
#else  /* !defined(HASTCPUDPSTATE) */
                      ""
#endif /* defined(HASTCPUDPSTATE) */

        );
        (void)fprintf(stderr, " [-u s] [+|-w] [-x [fl]]");

#if defined(HASZONES)
        (void)fprintf(stderr, " [-z [z]]");
#else /* !defined(HASZONES) */
#    if defined(HASSELINUX)
        if (CntxStatus)
            (void)fprintf(stderr, " [-Z [Z]]");
#    endif /* defined(HASSELINUX) */
#endif     /* defined(HASZONES) */

        (void)fprintf(stderr, " [--] [names]\n");
    }
    if (err && !Fhelp) {
        (void)fprintf(stderr,
                      "Use the ``-h'' option to get more help information.\n");
        if (!fh)
            Exit(ctx, err ? LSOF_EXIT_ERROR : LSOF_EXIT_SUCCESS);
    }
    if (Fhelp) {
        (void)fprintf(
            stderr, "Defaults in parentheses; comma-separated set (s) items;");
        (void)fprintf(stderr, " dash-separated ranges.\n");
        col = print_in_col(1, "-?|-h list help");
        col = print_in_col(col, "-a AND selections (OR)");
        col = print_in_col(col, "-b avoid kernel blocks");
        col = print_in_col(col, "-c c  cmd c ^c /c/[bix]");
        (void)snpf(buf, sizeof(buf), "+c w  COMMAND width (%d)", CMDL);
        col = print_in_col(col, buf);

#if defined(HASNCACHE)
        col = print_in_col(col, "-C no kernel name cache");
#endif /* defined(HASNCACHE) */

        col = print_in_col(col, "+d s  dir s files");
        col = print_in_col(col, "-d s  select by FD set");
        col = print_in_col(col, "+D D  dir D tree *SLOW?*");

#if defined(HASDCACHE)
        if (Setuidroot)
            cp = "?|i|r";

#    if !defined(WILLDROPGID)
        else if (Myuid)
            cp = "?|i|r<path>";
#    endif /* !defined(WILLDROPGID) */

        else
            cp = "?|i|b|r|u[path]";
        (void)snpf(buf, sizeof(buf), "-D D  %s", cp);
#else  /* !defined(HASDCACHE) */
        buf[0] = '\0';
#endif /* defined(HASDCACHE) */

        col = print_in_col(col, buf);

#if defined(HASEOPT)
        col = print_in_col(col, "+|-e s  exempt s *RISKY*");
#endif /* defined(HASEOPT) */

        (void)snpf(buf, sizeof(buf), "-i select IPv%s files",

#if defined(HASIPv6)
                   "[46]"
#else  /* !defined(HASIPv6) */
                   "4"
#endif /* defined(HASIPv6) */

        );
        col = print_in_col(col, buf);

#if defined(HASTASKS)
        /* DEBUG	    col = print_in_col(col, "-K list tasKs (threads)");
         */
        col = print_in_col(col, "-K [i] list|(i)gn tasKs");
#endif /* defined(HASTASKS) */

        col = print_in_col(col, "-l list UID numbers");
        col = print_in_col(col, "-n no host names");
        col = print_in_col(col, "-N select NFS files");
        col = print_in_col(col, "-o list file offset");
        col = print_in_col(col, "-O no overhead *RISKY*");
        col = print_in_col(col, "-P no port names");
        col = print_in_col(col, "-Q allow failed search");

#if defined(HASPPID)
        col = print_in_col(col, "-R list paRent PID");
#endif /* defined(HASPPID) */

        col = print_in_col(col, "-s list file size");
        col = print_in_col(col, "-t terse listing");
        col = print_in_col(col, "-T disable TCP/TPI info");
        col = print_in_col(col, "-U select Unix socket");
        col = print_in_col(col, "-v list version info");
        col = print_in_col(col, "-V verbose search");
        (void)snpf(buf, sizeof(buf), "+|-w  Warnings (%s)",

#if defined(WARNINGSTATE)
                   "-"
#else  /* !defined(WARNINGSTATE) */
                   "+"
#endif /* defined(WARNINGSTATE) */
        );

        col = print_in_col(col, buf);

#if defined(HASXOPT)
#    if defined(HASXOPT_ROOT)
        if (Myuid == 0)
            (void)snpf(buf, sizeof(buf), "-X %s", HASXOPT);
        else
            buf[0] = '\0';
#    else  /* !defined(HASXOPT_ROOT) */
        (void)snpf(buf, sizeof(buf), "-X %s", HASXOPT);
#    endif /* defined(HASXOPT_ROOT) */
#else      /* !defined(HASXOPT) */
        buf[0] = '\0';
#endif     /* defined(HASXOPT) */

        col = print_in_col(col, buf);

#if defined(HASZONES)
        col = print_in_col(col, "-z z  zone [z]");
#endif /* defined(HASZONES) */

#if defined(HASSELINUX)
        col = print_in_col(col, "-Z Z  context [Z]");
#endif /* defined(HASSELINUX) */

        col = print_in_col(col, "-H human readable size");

        col = print_in_col(col, "-- end option scan");
        if (col != 1)
            (void)fprintf(stderr, "\n");

#if defined(HASEPTOPTS)
        (void)fprintf(stderr, "  %-36.36s  %s\n", "-E display endpoint info",
                      "+E display endpoint info and files");
#endif /* defined(HASEPTOPTS) */

        (void)fprintf(stderr, "  %-36.36s",
                      "+f|-f  +filesystem or -file names");

#if defined(HASFSTRUCT)
        (void)fprintf(stderr, "  +|-f[%s%s%s%s]%s%s%s%s %s%s%s%s%s%s%s\n",

#    if defined(HASNOFSCOUNT)
                      "",
#    else  /* !defined(HASNOFSCOUNT) */
                      "c",
#    endif /* defined(HASNOFSCOUNT) */

#    if defined(HASNOFSADDR)
                      "",
#    else  /* !defined(HASNOFSADDR) */
                      "f",
#    endif /* defined(HASNOFSADDR) */

#    if defined(HASNOFSFLAGS)
                      "",
#    else  /* !defined(HASNOFSFLAGS) */
                      "gG",
#    endif /* defined(HASNOFSFLAGS) */

#    if defined(HASNOFSNADDR)
                      "",
#    else  /* !defined(HASNOFSNADDR) */
                      "n",
#    endif /* defined(HASNOFSNADDR) */

#    if defined(HASNOFSCOUNT)
                      "",
#    else  /* !defined(HASNOFSCOUNT) */
                      " Ct",
#    endif /* defined(HASNOFSCOUNT) */

#    if defined(HASNOFSADDR)
                      "",
#    else  /* !defined(HASNOFSADDR) */
                      " Fstr",
#    endif /* defined(HASNOFSADDR) */

#    if defined(HASNOFSFLAGS)
                      "",
#    else  /* !defined(HASNOFSFLAGS) */
                      " flaGs",
#    endif /* defined(HASNOFSFLAGS) */

#    if defined(HASNOFSNADDR)
                      "",
#    else  /* !defined(HASNOFSNADDR) */
                      " Node",
#    endif /* defined(HASNOFSNADDR) */

                      Fsv ? "(" : "", (Fsv & FSV_CT) ? "C" : "",
                      (Fsv & FSV_FA) ? "F" : "",
                      ((Fsv & FSV_FG) && FsvFlagX) ? "g" : "",
                      ((Fsv & FSV_FG) && !FsvFlagX) ? "G" : "",
                      (Fsv & FSV_NI) ? "N" : "", Fsv ? ")" : "");
#else  /* !defined(HASFSTRUCT) */
        putc('\n', stderr);
#endif /* defined(HASFSTRUCT) */

        (void)fprintf(stderr, "  %-36.36s",
                      "-F [f] select fields; -F? for help");

#if defined(HASKOPT)
        (void)fprintf(stderr, "  -k k   kernel symbols (%s)\n",
                      Nmlst ? Nmlst
#    if defined(N_UNIX)
                            : N_UNIX
#    else  /* !defined(N_UNIX) */
                      : (Nmlst = get_nlist_path(ctx, 1)) ? Nmlst
                                                         : "none found"
#    endif /* defined(N_UNIX) */

        );
#else  /* !defined(HASKOPT) */
        putc('\n', stderr);
#endif /* defined(HASKOPT) */

        (void)fprintf(stderr,
                      "  +|-L [l] list (+) suppress (-) link counts < l (0 "
                      "= all; default = 0)\n");

#if defined(HASMOPT) || defined(HASMNTSUP)
#    if defined(HASMOPT)
        (void)snpf(buf, sizeof(buf), "-m m   kernel memory (%s)", KMEM);
#    else  /* !defined(HASMOPT) */
        buf[0] = '\0';
#    endif /* defined(HASMOPT) */

        (void)fprintf(stderr, "  %-36.36s", buf);

#    if defined(HASMNTSUP)
        (void)fprintf(stderr, "  +m [m] use|create mount supplement\n");
#    else  /* !defined(HASMNTSUP) */
        (void)fprintf(stderr, "\n");
#    endif /* defined(HASMNTSUP) */
#endif     /* defined(HASMOPT) || defined(HASMNTSUP) */

#if !defined(HASNORPC_H)
        (void)snpf(buf, sizeof(buf), "+|-M   portMap registration (%s)",

#    if defined(HASPMAPENABLED)
                   "+"
#    else  /* !defined(HASPMAPENABLED) */
                   "-"
#    endif /* defined(HASPMAPENABLED) */

        );
#else  /* defined(HASNORPC_H) */
        buf[0] = '\0';
#endif /* !defined(HASNORPC_H) */

        (void)fprintf(stderr, "  %-36.36s", buf);
        (void)snpf(buf, sizeof(buf), "-o o   o 0t offset digits (%d)",
                   OFFDECDIG);
        (void)fprintf(stderr, "  %s\n", buf);
        (void)fprintf(stderr, "  %-36.36s", "-p s   exclude(^)|select PIDs");
        (void)fprintf(stderr, "  -S [t] t second stat timeout (%d)\n", TMLIMIT);
        (void)snpf(buf, sizeof(buf), "-T %s%ss%s TCP/TPI %s%sSt%s (s) info",

#if defined(HASSOOPT) || defined(HASSOSTATE) || defined(HASTCPOPT)
                   "f",
#else  /* !defined(HASSOOPT) && !defined(HASSOSTATE) && !defined(HASTCPOPT)*/
                   "",
#endif /* defined(HASSOOPT) || defined(HASSOSTATE) || defined(HASTCPOPT)*/

#if defined(HASTCPTPIQ)
                   "q",
#else  /* !defined(HASTCPTPIQ) */
                   " ",
#endif /* defined(HASTCPTPIQ) */

#if defined(HASTCPTPIW)
                   "w",
#else  /* !defined(HASTCPTPIW) */
                   "",
#endif /* defined(HASTCPTPIW) */

#if defined(HASSOOPT) || defined(HASSOSTATE) || defined(HASTCPOPT)
                   "Fl,",
#else  /* !defined(HASSOOPT) && !defined(HASSOSTATE) && !defined(HASTCPOPT)*/
                   "",
#endif /* defined(HASSOOPT) || defined(HASSOSTATE) || defined(HASTCPOPT)*/

#if defined(HASTCPTPIQ)
                   "Q,",
#else  /* !defined(HASTCPTPIQ) */
                   "",
#endif /* defined(HASTCPTPIQ) */

#if defined(HASTCPTPIW)
                   ",Win"
#else  /* !defined(HASTCPTPIW) */
                   ""
#endif /* defined(HASTCPTPIW) */

        );
        (void)fprintf(stderr, "  %s\n", buf);

#if defined(HAS_AFS) && defined(HASAOPT)
        (void)fprintf(stderr, "  -A A   AFS name list file (%s)\n",
                      AFSAPATHDEF);
#endif /* defined(HAS_AFS) && defined(HASAOPT) */

        (void)fprintf(
            stderr, "  -g [s] exclude(^)|select and print process group IDs\n");
        (void)fprintf(stderr, "  -i i   select by IPv%s address:",

#if defined(HASIPv6)
                      "[46]"
#else  /* !defined(HASIPv6) */
                      "4"
#endif /* defined(HASIPv6) */

        );
        (void)fprintf(stderr, " [%s][proto][@host|addr][:svc_list|port_list]\n",

#if defined(HASIPv6)
                      "46"
#else  /* !defined(HASIPv6) */
                      "4"
#endif /* defined(HASIPv6) */

        );

        (void)fprintf(stderr, "  +|-r [%s] repeat every t seconds (%d); %s",

#if defined(HAS_STRFTIME)
                      "t[m<fmt>]",
#else  /* !defined(has_STRFTIME) */
                      "t",
#endif /* defined(HAS_STRFTIME) */

                      RPTTM, " + until no files, - forever.\n");

#if defined(HAS_STRFTIME)
        (void)fprintf(
            stderr,
            "       An optional suffix to t is m<fmt>; m must separate %s",
            "t from <fmt> and\n");
        (void)fprintf(stderr, "      <fmt> is an strftime(3) format %s",
                      "for the marker line.\n");
#endif /* defined(HAS_STRFTIME) */

#if defined(HASTCPUDPSTATE)
        (void)fprintf(
            stderr,
            "  -s p:s  exclude(^)|select protocol (p = TCP|UDP) states");
        (void)fprintf(stderr, " by name(s).\n");
#endif /* defined(HASTCPUDPSTATE) */

        (void)fprintf(stderr, "  -u s   exclude(^)|select login|UID set s\n");
        (void)fprintf(
            stderr,
            "  -x [fl] cross over +d|+D File systems or symbolic Links\n");
        (void)fprintf(
            stderr,
            "  names  select named files or files on named file systems\n");
        (void)report_SECURITY(NULL, "; ");
        (void)report_WARNDEVACCESS(NULL, NULL, ";");
        (void)report_HASKERNIDCK(" k", NULL);
        (void)report_HASDCACHE(ctx, 0, NULL, NULL);

#if defined(DIALECT_WARNING)
        (void)fprintf(stderr, "WARNING: %s\n", DIALECT_WARNING);
#endif /* defined(DIALECT_WARNING) */
    }
    if (fh) {
        (void)fprintf(stderr, "%s:\tID    field description\n", Pn);
        for (i = 0; FieldSel[i].nm; i++) {

#if !defined(HASPPID)
            if (FieldSel[i].id == LSOF_FID_PPID)
                continue;
#endif /* !defined(HASPPID) */

#if !defined(HASFSTRUCT)
            if (FieldSel[i].id == LSOF_FID_FA ||
                FieldSel[i].id == LSOF_FID_CT ||
                FieldSel[i].id == LSOF_FID_FG || FieldSel[i].id == LSOF_FID_NI)
                continue;
#else /* defined(HASFSTRUCT) */
#    if defined(HASNOFSADDR)
            if (FieldSel[i].id == LSOF_FID_FA)
                continue;
#    endif /* defined(HASNOFSADDR) */

#    if defined(HASNOFSCOUNT)
            if (FieldSel[i].id == LSOF_FID_CT)
                continue;
#    endif /* !defined(HASNOFSCOUNT) */

#    if defined(HASNOFSFLAGS)
            if (FieldSel[i].id == LSOF_FID_FG)
                continue;
#    endif /* defined(HASNOFSFLAGS) */

#    if defined(HASNOFSNADDR)
            if (FieldSel[i].id == LSOF_FID_NI)
                continue;
#    endif /* defined(HASNOFSNADDR) */
#endif     /* !defined(HASFSTRUCT) */

#if !defined(HASZONES)
            if (FieldSel[i].id == LSOF_FID_ZONE)
                continue;
#endif /* !defined(HASZONES) */

#if defined(HASSELINUX)
            if ((FieldSel[i].id == LSOF_FID_CNTX) && !CntxStatus)
                continue;
#else  /* !defined(HASSELINUX) */
            if (FieldSel[i].id == LSOF_FID_CNTX)
                continue;
#endif /* !defined(HASSELINUX) */

            (void)fprintf(stderr, "\t %c    %s\n", FieldSel[i].id,
                          FieldSel[i].nm);
        }
    }

#if defined(HASDCACHE)
    if (DChelp)
        report_HASDCACHE(ctx, 1, NULL, "    ");
#endif /* defined(HASDCACHE) */

    if (version) {

        /*
         * Display version information in reponse to ``-v''.
         */
        (void)fprintf(stderr, "%s version information:\n", Pn);
        (void)fprintf(stderr, "    revision: %s\n", LSOF_VERSION);
        (void)fprintf(stderr, "    copyright notice: %s\n", copyright);
        (void)fprintf(stderr, "    latest revision: %s\n", LSOF_REPO_URL);
        (void)fprintf(stderr, "    latest FAQ: %s\n", LSOF_FAQ_URL);
        (void)fprintf(stderr, "    latest (non-formatted) man page: %s\n",
                      LSOF_MAN_URL);

#if defined(LSOF_CINFO)
        if ((cp = isnullstr(LSOF_CINFO)))
            (void)fprintf(stderr, "    configuration info: %s\n", cp);
#endif /* defined(LSOF_CINFO) */

        cp = isnullstr(LSOF_HOST);
        if (!(cp1 = isnullstr(LSOF_LOGNAME)))
            cp1 = isnullstr(LSOF_USER);
        if (cp || cp1) {
            if (cp && cp1)
                cp2 = "by and on";
            else if (cp)
                cp2 = "on";
            else
                cp2 = "by";
            (void)fprintf(stderr, "    constructed %s: %s%s%s\n", cp2,
                          cp1 ? cp1 : "", (cp && cp1) ? "@" : "", cp ? cp : "");
        }

#if defined(LSOF_BLDCMT)
        if ((cp = isnullstr(LSOF_BLDCMT)))
            (void)fprintf(stderr, "    builder's comment: %s\n", cp);
#endif /* defined(LSOF_BLDCMT) */

        if ((cp = isnullstr(LSOF_CC)))
            (void)fprintf(stderr, "    compiler: %s\n", cp);
        if ((cp = isnullstr(LSOF_CCV)))
            (void)fprintf(stderr, "    compiler version: %s\n", cp);
        if ((cp = isnullstr(LSOF_CCFLAGS)))
            (void)fprintf(stderr, "    compiler flags: %s\n", cp);
        if ((cp = isnullstr(LSOF_LDFLAGS)))
            (void)fprintf(stderr, "    loader flags: %s\n", cp);
        if ((cp = isnullstr(LSOF_SYSINFO)))
            (void)fprintf(stderr, "    system info: %s\n", cp);
        // display configurations that might affect output
        char *features[] = {
#if defined(HASEFFNLINK)
            "effnlink",
#endif
#if defined(HASFDESCFS)
            "fdescfs",
#endif
#if defined(HASF_VNODE)
            "f_vnode",
#endif
#if defined(HASIPv6)
            "ipv6",
#endif
#if defined(HAS_KF_SOCK_SENDQ)
            "kf_sock_sendq",
#endif
#if defined(HAS_KF_FILE_NLINK)
            "kf_file_nlink",
#endif
#if defined(HASNULLFS)
            "nullfs",
#endif
#if defined(HAS_SYS_PIPEH)
            "pipe",
#endif
#if defined(HASPROCFS)
            "procfs",
#endif
#if defined(HASPSEUDOFS)
            "pseudofs",
#endif
#if defined(HASPTYEPT)
            "ptyept",
#endif
#if defined(HASPTYFS)
            "ptyfs",
#endif
#if !defined(HASNORPC_H)
            "rpc",
#endif
#if defined(HASSBSTATE)
            "sbstate",
#endif
#if defined(HASSELINUX)
            "selinux",
#endif
#if defined(HASSOOPT)
            "soopt",
#endif
#if defined(HASSOSTATE)
            "sostate",
#endif
#if defined(HASTASKS)
            "tasks",
#endif
#if defined(HAS_TMPFS) || defined(HASTMPFS)
            "tmpfs",
#endif
#if defined(HAS_XTCPCB_TMAXSEG)
            "xtcpcb_tmaxseg",
#endif
#if defined(HASUXSOCKEPT)
            "uxsockept",
#endif
#if defined(HAS_V_LOCKF)
            "v_lockf",
#endif
        };
        (void)fprintf(stderr, "    features enabled:");
        for (i = 0; i < sizeof(features) / sizeof(features[0]); i++) {
            (void)fprintf(stderr, " %s", features[i]);
        }
        (void)fprintf(stderr, "\n");
        (void)report_SECURITY("    ", ".\n");
        (void)report_WARNDEVACCESS("    ", "are", ".\n");
        (void)report_HASKERNIDCK("    K", "is");

#if defined(DIALECT_WARNING)
        (void)fprintf(stderr, "    WARNING: %s\n", DIALECT_WARNING);
#endif /* defined(DIALECT_WARNING) */

        (void)report_HASDCACHE(ctx, 1, "    ", "\t");
    }
    Exit(ctx, err ? LSOF_EXIT_ERROR : LSOF_EXIT_SUCCESS);
}
