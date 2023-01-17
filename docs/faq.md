# Frequently Asked Questions

## General Concepts

### Lsof -- what is it?

Lsof is a UNIX-specific tool.  Its name stands for LiSt
Open Files, and it does just that.  It lists information
about files that are open by the processes running on a
UNIX system.

See the lsof [man page](./manpage.md), the 00DIST file, the
[tutorial](./tutorial.md) and the README.md file of the lsof distribution for
more information.

### Where do I get lsof?

Lsof is available at [GitHub lsof-org/lsof](https://github.com/lsof-org/lsof).

You can find the latest releases from [GitHub Releases](https://github.com/lsof-org/lsof/releases)

### Are lsof executable available?

You can install lsof from package managers.

If you must use a binary file, please be conscious of the
security and configuration implications in using an executable.

Three additional cautions apply to executables:

1.  Don't try to use an lsof executable, compiled for one
	version of a UNIX dialect, on another.  Patches can
	make the dialect version different.

2.  If you want to use an lsof binary on multiple systems,
	they must be running the same dialect OS version and
	have the same patches and feature support.

### Where can I get more lsof documentation?

A significant set of documentation may be found in the lsof distribution (See
"Where can I get lsof?).  There is a [manual page](./manpage.md), and a copy of
this FAQ in the file docs/faq.md (perhaps slightly less recent than this file if
you're reading it via a web browser.)

Two URLs provide some documentation that appears in the
lsof distribution:

FAQ: [https://lsof.readthedocs.io/en/latest/faq](https://lsof.readthedocs.io/en/latest/faq)

man page: [https://lsof.readthedocs.io/en/latest/manpage](https://lsof.readthedocs.io/en/latest/manpage)

### How do I report an lsof bug?

If you believe you have discovered a bug in lsof, you can report it to
https://github.com/lsof-org/lsof.  Do NOT report lsof bugs to the UNIX
dialect vendor.

Before you send a bug report, please read the "Bug Reports" section of
the 00README file of the lsof distribution.  It lists the steps you
should take before and when reporting a suspected bug.

### Where can I get the lsof FAQ?

This lsof FAQ is available in the file 00FAQ in the lsof
distribution and at the URL:

[https://lsof.readthedocs.io/en/latest/faq/](https://lsof.readthedocs.io/en/latest/faq/)

### How timely is the on-line FAQ?

The on-line FAQ is sometimes too timely.  :-)

I update it as soon as new information is available.   That may include
information about support that won't appear in the lsof source distribution
until the next revision.  If you encounter something like that, please file a
new issue at GitHub. 

### Is there a test suite?

Yes, as of lsof revision 4.63 there's an automated lsof
test suite in the tests/ sub-directory of the lsof top-level
directory.

More information on using the test suite, what it does,
how to use it and how to configure it may be found in the
00TEST file of the lsof distribution.  That file also
explains where the test suite has been tested.

Frequently asked questions about the test suite will be
asked and answered here in the FAQ.  (See "Test Suite
Problems.")

After lsof has been configured with the Configure script,
lsof can be made and tested with:

	$ make
	$ cd tests
	$ make

Under normal conditions -- i.e., unless the lsof tree has
been cleaned or purged severely -- all tests or individual
tests may be run by:

	$ cd test
	$ make
	# or
	$ <run a single test>	(See 00TEST.)

### Is lsof vulnerable to the standard I/O descriptor attack?

Lsof revisions 4.63 and above are not vulnerable.

Lsof revisions 4.62 and below are vulnerable, but no damage
scenarios have so far been demonstrated.

The standard I/O descriptor attack is a local programmed
assault on setuid and setgid programs that tricks them into
opening a sensitive file with write access on a standard
descriptor, usually stderr (2), and writing error messages
to stderr.  If the attacker can control the content of the
error message, the attacker may gain elevated privileges.

The attack was first described in Pine Internet Advisory
PINE-CERT-20020401, available at:

	http://www.pine.nl/advisories/pine-cert-20020401.txt

If you are using an lsof revision below 4.63, you should
remove any setuid or setgid permissions you might have
given its executable.  Then you should upgrade to lsof
revision 4.63.

### Can I alter lsof's make(1) behavior?

Yes.  There are at least two ways to do that.

You can put replacements for lsof Makefile strings in your
environment.  If you specify the -e make option, make will
give environment variable values precedence over strings
from the Makefile.  For example, to change the compiler
string CC from the environment, you might do this with the
Bourne shell:

	$ CC=foobar; export CC
	$ make -e

You can also replace lsof Makefile strings in the make
command invocation.  Here's the previous example done that
way:

	$ make CC=foobar

Changing the CFGF, CFGL, and DEBUG strings used in lsof
Makefiles, either from the environment or from the make
invocation, can significantly alter lsof make(1) behavior.
I commonly use DEBUG to change the -O option to -g so I
can build an lsof executable for debugging -- e.g.,

	$ make DEBUG=-g

(Look for DEBUG in this FAQ for other examples of its use.)

Consult the Makefiles to see what CFGL, CFGL, and other
lsof Makefile strings contain, and to see what influence
their alteration might have on lsof make(1) behavior.

### Is there an lsof license?

No.

The only restriction on the use or redistribution of lsof
is contained in this copyright statement, found in every
lsof source file.  (The copyright year in or format of the
notice may vary slightly.)

	/*
	 * Copyright 2002 Purdue Research Foundation, West Lafayette,
	 * Indiana 47907.  All rights reserved.
	 *
	 * Written by Victor A. Abell
	 *
	 * This software is not subject to any license of the American
	 * Telephone and Telegraph Company or the Regents of the
	 * University of California.
	 *
	 * Permission is granted to anyone to use this software for
	 * any purpose on any computer system, and to alter it and
	 * redistribute it freely, subject to the following
	 * restrictions:
	 *
	 * 1. Neither the authors nor Purdue University are responsible
	 *    for any consequences of the use of this software.
	 *
	 * 2. The origin of this software must not be misrepresented,
	 *    either by explicit claim or by omission.  Credit to the
	 *    authors and Purdue University must appear in documentation
	 *    and sources.
	 *
	 * 3. Altered versions must be plainly marked as such, and must
	 *    not be misrepresented as being the original software.
	 *
	 * 4. This notice may not be removed or altered.
	 */

### Language locale support

#### Does lsof support language locales?  How do I use the support?

Most UNIX dialect versions of lsof support 8 bit language
locale characters -- e.g., the ability to print 8 bit
characters that have accents and other marks over them.

See the answer to the "Does lsof support wide characters in
language locales?" question for information on when lsof's
language locale support covers characters wider than 8 bits.

To see if lsof supports language locales for your dialect, look
in the dialect's machine.h header file for the HASSETLOCALE
definition.  If it is present and not disabled, then lsof has
language locale support for the dialect.

To enable lsof's language locale support, you must specify in a
locale environment variable (e.g., LANG) a language locale
known to your system that supports the printing of marked
characters -- e.g, en_US.  (On some dialects locale(1) may be
used to list the known language locales.)

Note that LANG=C and LANG=POSIX are NOT language locales that
support the printing of marked characters.

If the language locale doesn't support the printing of marked
characters, lsof's OUTPUT of them follows the rules for
non-printable characters described in the OUTPUT section of
lsof(8).

Consult your dialect's setlocale(3) man page for the names of
environment variables other than LANG  -- e.g., LC_ALL,
LC_TYPE, etc. -- which may be used to define language locales.

#### Does lsof support wide characters in language locales?

When lsof's language locale support is enabled with the
HASSETLOCALE definition, for selected dialects lsof will also
print wide characters (e.g., from UTF-8) when iswprint(3)
reports them to be printable.

Wide character support is available when HASWIDECHAR is defined
in a dialect's machine.h header file.  As of this writing on
July 22, 2004, the following dialect versions have wide character
support:

	AIX >= 4.3.2
	Apple Darwin >= 7.3.0
	FreeBSD >= 5.2
	HP-UX >= 11.00
	/proc-based Linux
	NetBSD >= 1.6
	SCO OpenServer >= 5.0.6
	Solaris >= 2.6
	Tru64 UNIX 5.1

### Are any files in the lsof distribution copyrighted?

Yes.  Most files carry the copyright of the Purdue Research
Foundation and may be redistributed under the terms that
accompany the copyright notice.  Those terms may also be found
in the answer to the question, "Is there an lsof license?")

A few files carry other copyright notices.  Some are BSD
notices and they explain the terms under which they are
included in the lsof distribution.

Those that carry vendor copyright notices have been reproduced
in their original or modified forms with permission from the
copyright owners.  That permission is indicated in the README
files that accompany the files.

### Are there other lsof-related resources?

There are other resources available, connected to lsof.  Among
them are FreeBSD and Linux packages whose products use lsof and
two particularly interesting resources.

The two interesting resources are a Gnome Tool Kit (GTK) GUI
for lsof and a Perl wrapper module.

The GTK GUI is called Glsof and was developed by Gnele.  It can
be found at:

	http://www.sourceforge.net

The Perl wrapper module by Marc Beyer can be found at:

	http://search.cpan.org/dist/Unix-Lsof/

### What does the "WARNING: unsupported dialect or version" mean?

The lsof configure script issues that message for UNIX dialects
or their versions where I have been unable to test the current
revision of lsof.  The message doesn't mean that lsof won't
work, just that I have no direct evidence that it will.

If the Configure script succeeds, except for the warning, try
compiling) lsof.  If that succeeds, try the lsof test suite.
