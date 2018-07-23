/*
 *  ctflook - read CTF info from a kernel module to look up type member
offsets
 *
 *      cc [ -m64 ] ctflook.c -lctf -o ctflook
 */

static const char rcsid[] =
    "$Id: ctflook.c,v 1.4 2009/07/13 20:18:42 byrnes Exp $";

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

#include <libctf.h>

/* Exit status codes: */
#define E_SUCCESS   0		/* Everything worked! */
#define E_USAGE     1		/* Usage error. */
#define E_SYSTEM    2		/* System error. */

/* Program basename from the command line, for error messages: */

static char *prog;

/* Emit a complaint about a system error, and exit. */

static void
panic(const char *format, ...)
{
    int save_errno = errno;	/* errno might be clobbered by
				 * [v]fprintf. */

    va_list ap;

    va_start(ap, format);

    (void)  fprintf(stderr, "%s: ", prog);
    (void) vfprintf(stderr, format, ap);

    va_end(ap);

    if (save_errno)
	(void) fprintf(stderr, ": %s\n", strerror(save_errno));
    else
	(void) fprintf(stderr, "\n");

    exit(E_SYSTEM);
}

#pragma does_not_return(panic)

/*
 * Callback function for ctf_member_iter().
 * This is called for each member of a structure.
 */

static int
membercb(const char *name, ctf_id_t id, ulong_t offset, void *arg)
{
    (void) printf("\t%4lu\t%s\n", offset, name);

    return 0;
}

/* Main program. */
int
main(int argc, char **argv)
{
    const char *module;		/* Pathname for the kernel module. */
    const char *type;		/* Type name. */

    ctf_file_t *ctf_file;	/* Handle for looking up CTF info in file. */
    ctf_id_t ctf_id;		/* Handle for CTF type. */
    int ctf_err;		/* CTF error number. */

    /* Initialize the program basename, for error messages. */

    if ((prog = strrchr(*argv, '/')) != NULL)
	prog++;
    else
	prog = *argv;

    if (argc < 3) {
	(void) fprintf(stderr, "usage: %s module type ...\n", prog);
	exit(E_USAGE);
    }

    module = *++argv;

    /* Open the file, and read CTF info. */

    if ((ctf_file = ctf_open(module, &ctf_err)) == NULL)
	panic("ctf_open: %s: %s", module, ctf_errmsg(ctf_err));

    while ((type = *++argv) != NULL) {
	/* Look up the type. */
	if ((ctf_id = ctf_lookup_by_name(ctf_file, type)) == CTF_ERR)
	    panic("ctf_lookup_by_name: %s: %s: %s",
		  module, type, ctf_errmsg(ctf_errno(ctf_file)));

	(void) printf("\n%s: (defined in %s)\n\n", type, module);

	(void) printf("\tbits\tmember\n");
	(void) printf("\t----\t------\n");

	/* Iterate through the members, printing the offsets. */
	if (ctf_member_iter(ctf_file, ctf_id, membercb, NULL) == CTF_ERR)
	    panic("ctf_member_iter: %s: %s: %s",
		  module, type, ctf_errmsg(ctf_errno(ctf_file)));
    }

    ctf_close(ctf_file);

    exit(E_SUCCESS);
}
