# Frequently Asked Questions

Caveat: most of the FAQ comes from the original lsof, which may be outdated.

## When Lsof Seems to Hang

On occasion when you run lsof it seems to hang and produce no
output.  This may result from system conditions beyond the control
of lsof.  Lsof has a number of options that may allow you to
bypass the blockage.

### Kernel lstat(), readlink(), and stat() Blockages

Lsof uses the kernel (system) calls lstat(), readlink(), and
stat() to locate mounted file system information.  When a file
system has been mounted from an NFS server and that server is
temporarily unavailable, the calls lsof uses may block in the
kernel.

Lsof will announce that it is being blocked with warning messages
(unless they have been suppressed by the lsof builder), but
only after a default waiting period of fifteen seconds has
expired for each file system whose server is unavailable.  If
you have a number of such file systems, the total wait may be
unacceptably long.

You can do two things to shorten your suffering: 1) reduce the
wait time with the -S option; or 2) tell lsof to avoid the
kernel calls that might block by specifying the -b option.

```shell
$ lsof -S 5
# or
$ lsof -b
```

Avoiding the kernel calls that might block may result in the
lack of some information that lsof needs to know about mounted
file systems.  Thus, when you use -b, lsof warns that it might
lack important information.

The warnings that result from using -b (unless suppressed by
the lsof builder) can themselves be annoying.  You can suppress
them by adding the -w option.  (Of course, if you do, you won't
know what warning messages lsof might have issued.)

```shell
$ lsof -bw
```

Note: if the lsof builder suppressed warning message issuance,
you don't need to use -w to suppress them.  You can tell what
the default state of message warning issuance is by looking at
the -h (help) output.  If it says `-w enable warnings` then
warnings are disabled by default; `-w disable warnings`, they
are enabled by default.

### Problems with /dev or /devices

Lsof scans the /dev or /devices branch of your file system to
obtain information about your system's devices.  (The scan isn't
necessary when a device cache file exists.)

Sometimes that scan can take a very long time, especially if
you have a large number of devices, and if your kernel is
relatively slow to process the stat() system call on device
nodes.  You can't do anything about the stat() system call
speed.

However, you can make sure that lsof is allowed to use its
device cache file feature.  When lsof can use a device cache
file, it retains information it gleans via the stat() calls
on /dev or /devices in a separate file for later, faster
access.

The device cache file feature is described in the lsof man
page.  See the DEVICE CACHE FILE, LSOF PERMISSIONS THAT AFFECT
DEVICE CACHE FILE ACCESS, DEVICE CACHE FILE PATH FROM THE -D
OPTION, DEVICE CACHE PATH FROM AN ENVIRONMENT VARIABLE,
SYSTEM-WIDE DEVICE CACHE PATH, PERSONAL DEVICE CACHE PATH
(DEFAULT), and MODIFIED PERSONAL DEVICE CACHE PATH sections.

There is also a separate file in the lsof distribution, named
00DCACHE, that describes the device cache file in detail,
including information about possible security problems.

One final observation: don't overlook the possibility that your
/dev or /devices tree might be damaged.  See if

```shell
$ ls -R /dev
# or
$ ls -R /devices
```

completes or hangs.  If it hangs, then lsof will probably hang,
too, and you should try to discover why ls hangs.

### Host and Service Name Lookup Hangs

Lsof can hang up when it tries to convert an Internet dot-form
address to a host name, or a port number to a service name.  Both
hangs are caused by the lookup functions of your system.

An independent check for both types of hangs can be made with
the netstat program.  Run it without arguments.  If it hangs,
then it is probably having lookup difficulties.  When you run
it with -n it shouldn't hang and should report network and port
numbers instead of names.

Lsof has two options that serve the same purpose as netstat's
-n option.  The lsof -n option tells it to avoid host name
lookups; and -P, service name lookups.  Try those options when
you suspect lsof may be hanging because of lookup problems.

```shell
$ lsof -n
# or
$ lsof -P
# or
$ lsof -nP
```

### UID to Login Name Conversion Delays

By default lsof converts User IDentification (UID) numbers to
login names when it produces output.  That conversion process
may sometimes hang because of system problems or interlocks.

You can tell lsof to skip the lookup with the -l option; it
will then report UIDs in the USER column.

```shell
$ lsof -l
```

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

## Lsof Ports

### What ports exist?

The pub/lsof.README file carries the latest port information:

	AIX 5.[23] and 5.3
	FreeBSD 4.9 and 6.4 for x86-based systems
	FreeBSD 8.[234], 9.0, 10.0 and 11.0 for AMD64-based systems
	Darwin(macOS) for AMD64/ARM64-based systemd
	Linux 2.1.72 and above for x86-based systems
	Solaris 9, 10 and 11

In the above list the only UNIX dialects present are ones for
which I test the current lsof revision.  Lsof may still support
unlisted dialect versions -- e.g., HP-UX 10.20, Solaris 7, etc.
-- but I don't have access to systems where I could test lsof
on them, so I can't claim lsof works on them. If your dialect
isn't in the list, you should try building lsof on it anyway.

Lsof version 4 predecessors, versions 2 and 3, may support older
version of some dialects.  Contact me via e-mail at <abe@purdue.edu>
if you're interested in their distributions.  Make sure "lsof"
appears in the "Subject:" line so my e-mail filter won't classify
your letter as Spam.

### What about a new port?

The 00PORTING file in the distribution gives hints on doing
a port.  I will consider doing a port in exchange for
permanent access to a test host.  I require permanent access
so I can test new lsof revisions, because I will not offer
distributions of dialect ports I cannot upgrade and test.

#### User-contributed Ports

Sometimes I receive contributions of ports of lsof to
systems where I can't test future revisions of lsof.  Hence,
I don't incorporate these contributions into my lsof
distribution.

However, I do make descriptions of these contributions
available.  You can find them in the 00INDEX and README
files at:

	ftp://lsof.itap.purdue.edu/pub/tools/unix/lsof/contrib

Consult the 00INDEX file in the contrib/ directory for a
list of the available contributions and consult README
there for information on how to obtain them.

### Why isn't there an AT&T SVR4 port?

I haven't produced an AT&T SVR4 port because I haven't seen
a UNIX dialect that is strictly limited to the AT&T System
V, Release 4 source code.  Every one I have seen is a
derivative with vendor additions.

The vendor additions are significant to lsof because they
affect the internal kernel structures with which lsof does
business.  While some vendor derivatives of SVR4 are similar,
each one I have encountered so far has been different enough
from its siblings to require special source code.

If you're interested in an SVR4 version of lsof, here are
some existing ports you might consider:

	DC/OSx (This obsolete port is only available upon
		special request.)
	Reliant UNIX (This obsolete port is only available
			upon special request.)
	SCO|Caldera UnixWare (This is the most likely choice.)
	Solaris

### Why isn't there an SGI IRIX port?

Lsof support for IRIX was terminated at lsof revision 4.36,
because it had become increasingly difficult for me to
obtain information on the IRIX kernel structures lsof needs
to access.

At IRIX 6.5 I decided the obstacles were too large for me
to overcome, and I stopped supporting lsof on IRIX.  I have
sources to the last revision of lsof (4.36) for IRIX, but
that version of lsof does not work on IRIX 6.5 and is
vulnerable to the standard I/O descriptor attack.  (See
the "Is lsof vulnerable to the standard I/O descriptor
attack?" Q&A for more information.) Contact me to discuss
obtaining those sources.

If you wish to pursue the issue, don't contact me, contact
SGI.  This case was opened with SGI on the subject:

	Case ID:	0982584
	Category: Unix
	Priority: 30-Moderate Impact

	Problem Summary:
	kernel structure header files needed for continued lsof
	support

	Problem Description:
	Email In  07/17/98 19:09:23

### Why does lsof's Configure script report "WARNING: unsupported
dialect or version"?

Lsof's Configure script issues this message when it encounters
a dialect or its version that lsof once supported, but no
longer does.  Usually I drop support for a dialect or version
when I can no longer test lsof on it.

However, it's worth trying to compile and use lsof.  Be sure to
run the test suite.  (See the answer to the "Is there a test
suite?  question for information on the test suite.)

If you have problems with an unsupported dialect or version,
file a new GitHub issue and I may be able to help.


## Lsof Problems

### Configuration Problems

#### Why can't Configure determine the UNIX dialect version?

The lsof Configure script uses UNIX shell commands, often in a
command pipeline, to determine the UNIX dialect version.
(Consult the dialect stanza in Configure to determine which
commands are used.)  If Configure can't determine the dialect
version, probably one of the commands is not behaving as
Configure expects.

Symptoms of the failure include Configure warning messages and
incorrect version definitions in the Makefile CFLAGS.

If you suspect that the lsof Configure script is failing to
determine the dialect version correctly, try running the
commands from Configure stanza one at a time.  That will
usually reveal the source of the problem.  Be particularly
mindful that the PATH environment variable can cause commands
to be executed from non-standard directories.

If you can't determine the source of the problem, there is a
work-around.  You can supply the UNIX dialect version in the
LSOF_VSTR environment variable.  Use Configure as a guide to
forming what it expects in LSOF_VSTR.  There is also some
information on  LSOF_VSTR in the 00XCONFIG documentation file
of the lsof distribution.

### Compilation Problems

#### Why does the compiler complain about missing header files?

When you use make to build lsof, the compiler may complain
that it can't find header files -- e.g.,

	$ make
	(cd lib; make DEBUG="-O" CFGF="-DAIXA=0 -DAIXV=4330 \
	-DLSOF_VSTR=\"4.3.3.0\"")
	gcc  -DAIXA=0 -DAIXV=4330 -DLSOF_VSTR="4.3.3.0" -O \
	-c ckkv.c
	In file included from ckkv.c:33: ../machine.h:70: \
	sys/types.h: A file or directory in the path name \
	does not exist. \

That type of complaint doesn't represent an lsof problem.
It represents a problem with a missing system header file
that probably should be found in /usr/include or in the
system source tree.

As a first step try using find(1) to locate the problem
header file.  If it's a system header file and can't be
found, here are some possible causes:

1. The file set, RPM or package containing the header files
	has not been installed.  Instructions for doing that
	are specific to the UNIX dialect and beyond the scope
	of this document.

2. If the compiler is gcc, the private gcc header files:

	* May not have been installed;

	* May have been installed incorrectly;

	* May not have been updated properly after the last
	compiler or system update;

	* Ones from a previous installation may not have been
	removed.

	A path leading to the gcc private header files can be
	found with `gcc -v`.  Consult the gcc documentation for
	instructions on proper installation of the private gcc
	header files.

3. On some dialects -- e.g., FreeBSD, NetBSD, OpenBSD --
	lsof may need to use header files that are located in
	the system source tree -- /sys or /usr/src/sys, for
	example.  Make sure the system source tree has been
	installed.

#### Why does gcc complain about the contents of header files
distributed by the system's vendor?

When you use make to build lsof and gcc to compile it, gcc
may complain that it finds errors in system header files
-- e.g.,

	$ make
	(cd lib; make DEBUG="-O" CFGF="-Dsolaris=80000 \
	-DHASPR_GWINDOWS -m64 -DHASIPv6 -DHAS_VSOCK \
	-DLSOF_VSTR=\"5.8\"")
	gcc -Dsolaris=80000  -DHASPR_GWINDOWS -m64 -DHASIPv6 \
	-DHAS_VSOCK -DLSOF_VSTR="5.8"  -O  -c  dvch.c
	In file included from /usr/include/sys/proc.h:31, \
	from /homes/abe/gnu/gcc-3.2.1/lib/gcc-lib/sparcv9-sun-solaris2/ \
	3.2.1/include/sys/user.h:267, from /usr/include/kvm.h:13, \
	from ../dlsof.h:53, from ../lsof.h:172, from dvch.c:43: \
	/homes/abe/gnu/gcc-3.2.1/lib/gcc-lib/sparcv9-sun-solaris2/\
	3.2.1/include/sys/task.h:59: parse error before "uint_t"

Errors like the above are most likely not problems in the
system's header files, but in the private copies of them
that were created when gcc was made or installed.  Note
the presence of
".../gcc-3.2.1/lib/gcc-lib/sparcv9-sun-solaris2/3.2.1/include/..."
in the paths for user.h and task.h.  It indicates both
header files are gcc-specific.

To solve errors like this requires comparing the header
files in the vendor's /usr/include tree to the gcc-specific
ones in gcc's private gcc-lib/.../include tree.  It may be
necessary to regenerate gcc-specific header files, correct
them or remove them.  See the gcc distribution for the
appropriate tools.

A possible temporary work-around is to direct gcc to use
the vendor's header files instead of its temporary ones by
declaring -I/usr/include in the compilation flags.

#### Other header file problems

Don't overlook any vendor tools that might validate the
vendor header files installed on the system  -- e.g., the
Solaris pkgchk tool can be used to check the header files
that were installed from the SUNWhea package.

For other header file problems contact me at <abe@purdue.edu>.
Please follow the reporting guidelines in the "How do I
report an lsof bug?" section of this FAQ.

### Why doesn't lsof report full path names?

Lsof reports the full path name when it is specified as a
search argument for open files that match the argument.
However, if the argument is a file system mounted-on
directory, and lsof finds additional path name components
from the kernel name cache, it will report them.

Lsof reports path name for file system types that have path
name lookup features -- e.g., some versions of AdvFS for
Digital and Tru64 UNIX.  The Linux /proc-based lsof reports
full path names, because the Linux /proc file system provides
them.  Lsof on recent builds of Solaris 10 also report full
path names, because those Solaris kernels record the full path
name in the vnode structure.

Otherwise, lsof uses the kernel name cache, where it exists
and can be accessed, and reports some or all path name
components (e.g., the sys and proc.h components of
/usr/include/sys/proc.h) for these dialects:

	Apple Darwin
	DC/OSx
	FreeBSD
	HP-UX, /dev/kmem and PSTAT based
	Linux, /dev/kmem-based
	NetBSD
	OpenBSD
	Reliant UNIX
	SCO OpenServer
	SCO|Caldera UnixWare
	Solaris 2.x, 7, 8 and 9 (except for some VxFS versions;
					see the "Why doesn't Solaris
					lsof report VxFS path name
					components?" section for more
					information)
	Solaris 10 (early builds) Tru64 UNIX

As far as I can determine, AFS path lookups don't share in
kernel name cache operations, so lsof can't identify open AFS
path name components.  Apparently Solaris VxFS versions 4 and
above don't share in kernel name cache operations, either, so
lsof can't display path name components for those open files.

Since the size of the kernel name cache is limited and the
cache is in constant flux, it does not always contain the names
of all components in an open file's path; sometimes it contains
none of them.

Lsof reports the file system directory name and whatever
components of the file's path it finds in the cache, starting
with the last component and working backwards through the
directories that contain it.  If lsof finds no path
components, lsof reports the file system device name instead.

When lsof does report some path components in the NAME
column, it prefixes them with the file system directory
name, followed by " -- ", followed by the components --
e.g., /usr -- sys/path.h for /usr/include/sys/path.h.  The
" -- " is omitted when lsof finds all the path name components
of a file's name.

The PSTAT-based HP-UX lsof relies on kernel name cache
contents, too, even though its information comes to lsof
via pstat() function calls.  Consequently, PSTAT-based
HP-UX lsof won't always report full paths, but may use the
" -- " partial path name notation, or may occasionally
report no path name at all but just the file system mounted-on
directory and device names.

Lsof can't obtain path name components from the kernel name
caches of the following dialects:

	AIX

Only the Linux kernel records full path names in the
structures it maintains about open files; instead, most
kernels convert path names to device and node number doublets
and use them for subsequent file references once files have
been opened.

To convert the device and node number doublet into a
complete path name, lsof would have to start at the root
node (root directory) of the file system on which the node
resides, and search every branch for the node, building
possible path names along the way.  That would be a time
consuming operation and require access to the raw disk
device (usually implying setuid-root permission).

If the prospect of all that local disk activity doesn't
concern you, think about the cost when the device is
NFS-mounted.

Try using the file system mount point and node number lsof
reports as parameters to find -- e.g.,

	$ find <mount_point> -inum <node_number> -print

and you may get an appreciation of what a file system
directory tree search would cost.

#### Why do lsof -r reports show different path names?

When you run lsof with its repeat (`-r`) option, you may
notice that the extent to which it reports path names for
the same files may vary from cycle to cycle.  That happens
because other processes are making kernel calls affecting
the cache and causing entries to be removed from and added
to it.

#### Why does lsof report the wrong path names?

Under some circumstances lsof may report an incorrect path
name component, especially for files in a rapidly changing
directory like /tmp.

In a rapidly changing directory, like /tmp, if the kernel
doesn't clear the cache entry when it removes a file, a
new file may be given the same keys and lead lsof to believe
that the old cache entry with the same keys belongs to the
new file.

Lsof tries to avoid this error by purging duplicate entries
from its copy of the kernel name cache when they have the
same device and inode number, but different names.

This error is less likely to occur in UNIX dialects where the
keys to the name cache are node address and possibly a
capability ID.  The Apple Darwin, Digital UNIX, FreeBSD, HP-UX,
NEXTSTEP, OPENSTEP, Solaris, Tru64 UNIX, and UnixWare dialects
use node address.  Apple Darwin, FreeBSD, NetBSD, OpenBSD,
Tru64 UNIX, and also use a capability ID to further identify
name cache entries.

#### Why doesn't lsof report path names for unlinked (rm'd) files?

When lsof gets path name components from the kernel's name
cache, it does not report the path names of a file that has
been unlinked from its parent directory -- e.g., deleted via
rm, or the unlink() system call -- even when some process may
still hold the file open; lsof reports only the file system's
mounted-on directory and device.  That's because path name
components are removed from the kernel name cache when the file
is unlinked.

Unlinked open files are sometimes used by applications for
temporary, but invisible storage (i.e., ls won't show them,
and no other process can open them.)  However, they may
occasionally consume disk space to excess and cause concern
for a system administrator, who will be unable to locate
them with find, ls, du, or other tools that rely on finding
files by examining the directory tree.

By using lsof's +L option you can see the link count of
open files -- in the NLINK column.  An unlinked file will
have an NLINK value of zero.  By using the option +L1 you
can tell lsof to display only files whose link count is
less than one (i.e., zero).

There are some UNIX dialect-specific exceptions to lsof's
inability to report unlinked path names.  They are described in
the answer to the "When will lsof report path names for deleted
files?" question.

#### Why doesn't lsof report the "correct" hard linked file path
name?

When lsof reports a rightmost path name component for a
file with hard links, the component may come from the
kernel's name cache.  Since the key which connects an open
file to the kernel name cache may be the same for each
differently named hard link, lsof may report only one name
for all open hard-linked files.   Sometimes that will be
"correct" in the eye of the beholder; sometimes it will
not.  Remember, the file identification keys significant
to the kernel are the device and node numbers, and they're
the same for all the hard linked names.

#### When will lsof report path names for deleted files?

Lsof will report path names for deleted files for two
dialects:  Linux and later builds of Solaris 10.

Deleted Linux path names are reported by default and have
"(deleted)" at their ends.

The display of Solaris 10 deleted path names may be selected
with the -X option.  When selected they are also reported with
"(deleted)" at their ends.

### Why is lsof so slow?

Lsof may appear to be slow if network address to host name
resolution is slow.  This can happen, for example, when the
name server is unreachable, or when a Solaris PPP cache daemon
is malfunctioning.

To see if name lookup is causing lsof to be slow, turn it off
with the `-n` option.

Port service name lookup or portmap registration lookup may
also be causes of slow-down.  To suppress port service name
lookup, specify the `-P` option.

Lsof doesn't usually make direct portmap calls -- only when +M
is specified, or when HASPMAPENABLED is defined during lsof
construction.  (The lsof help panel, produced with `lsof -h`
will display the default portmap registration reporting
state.)  The quickest first step in checking if lsof is slow
because of the portmapper is to use lsof's `-M` option.

Lsof may be slow if UID to login name lookups are slow.
Suppress them with `-l`.

On dialects where lsof uses the kernel name cache, try
disabling its use with `-C`.  (You can tell if lsof uses the
kernel name cache by looking for `-C` in lsof's `-h`
output.)  Of course, disabling kernel name cache use will mean
that lsof won't report full or partial path names, just file
system and character device names.

If you're just interested in the open files of one process, try
using the `-p <Process-ID>` option to limit lsof to that
process.  (The `-p` option may also be followed with a list
of Process-IDs.)

If you're interested in including or excluding certain
commands, try lsof's "-c[^]cmd" option.

If you're interested in certain Internet TCP and UDP states
(e.g., ESTABLISHED) or in excluding some (e.g., CLOSE_WAIT),
try lsof's "-s p:s" option, available where shown on the lsof
help output, obtained with -h or -?.  More information on it
may be found in the answer to the "How are protocol state name
exclusion and inclusion used?" question.

Your UNIX dialect may not support "-s p:s" and its associated
performance improvements to Internet-only file processing.  You
can find more information on those topics in the answer to the
"Why doesn't my dialect support state name exclusion and
inclusion?" question.

Older AIX lsof may be slow to start because of its oslevel
identity comparison.  (Newer AIX lsof uses uname(2).)  See the
"Why does AIX lsof start so slowly?" and "Why does lsof warn
"compiled for x ... y; this is z.?" sections for more
information.

### Why doesn't lsof's setgid or setuid permission work?

If you install lsof on an NFS file system that has been
mounted with the nosuid option, lsof may not be able to
use the setgid or setuid permission you give it, complaining
it can't open the kernel memory device -- e.g., /dev/kmem.

The only solution is to install lsof on a file system that
doesn't inhibit setgid or setuid permission.

### Does lsof have security problems?

I don't think so.  However, lsof does usually start with
setgid permission, and sometimes with setuid-root permission.
Any program that has setgid or setuid-root permission,
should always be regarded with suspicion.

Lsof drops setgid power, holding it only while it opens
access to kernel memory devices (e.g., /dev/kmem, /dev/mem,
/dev/swap).  That allows lsof to bypass the weaker security
of access(2) in favor of the stronger checks the kernel
makes when it examines the right of the lsof process to
open files declared with -k and -m.  Lsof also restricts
some device cache file naming options when it senses the
process has setuid-root power.

On a few dialects lsof requires setuid-root permission
during its full execution in order to access files in the
/proc file system.  These dialects include:

	DC/OSx 1.1 for Pyramid systems
	Reliant UNIX 5.4[34] for Pyramid systems

When lsof runs with setuid-root permission it severely
restricts all file accesses it might be asked to make with
its options.

The device cache file (typically .lsof_hostname in the home
directory of the real user ID that executes lsof) has 0600
modes.  (The suffix, hostname, is the first component of
the host's name returned by gethostname(2).)  However, even
when lsof runs setuid-root, it makes sure the file's
ownerships are changed to that of the real user and group.
In addition, lsof checks the file carefully before using
it (See the question "How do I disable the device cache
file feature or alter its behavior?" for a description of
the checks.); discards the file if it fails the scrutiny;
complains about the condition of the file; then rebuilds
the file.

See the 00DCACHE file of the lsof distribution for more
information about device cache file handling and the risks
associated with the file.

### Will lsof show remote hosts using files via NFS?

No.  Remember, lsof displays open files for the processes
of the host on which it runs.  If the host on which lsof
is running is an NFS server, the remote NFS client processes
that are accessing files on the server leave no process
records on the server for lsof to examine.

### Why doesn't lsof report locks held on NFS files?

Generally lock information held by local processes on remote
NFS files is not recorded by the UNIX dialect kernel.  Hence,
lsof can't report it.

One exception is some patch levels of Solaris 2.3, and all
versions of Solaris 2.4 and above.  Lsof for those dialects
does report on locks held by local processes on remotely
mounted NFS files.

#### Why does lsof report a one byte lock on byte zero as a full
file lock?

When a process has a lock of length one, starting at byte
zero, lsof can't distinguish it from a full file lock.
That's because most UNIX dialects represent both locks the
same way in their file lock (flock or eflock) structures.

### Why does lsof report different values for open files on the same file system (the automounter phenomenon)?

On UNIX dialects where file systems may be mounted by an
automounter with the `direct` type, lsof may sometimes
report difference DEVICE, SIZE/OFF, INODE and NAME values
when asked to report files open on the file system.

This happens because some files open on the file system --
e.g., the current directory of a shell that changed its
directory to the file system as the file system's first
reference -- may be characterized in the kernel with
temporary automounter node information.  The cd doesn't
cause the file system to be mounted.

A subsequent reference to the file system -- e.g., an ls
of any place in it -- will cause the file system to be
mounted.  Processes with files open to the mounted file
system are characterized in the kernel with data that
reflects the mounted file system's parameters.

Unfortunately some kernels (e.g., some versions of Solaris
2.x) don't revisit the process that did only a change-directory
for the purpose of updating the data associated with the
open directory file.  The file continues to be characterized
with temporary automounter information until it does another
directory change, even a trivial `cd .`.

Lsof will report on both reference types, when supplied
the file system name as an argument, but the data lsof
reports will reflect what it finds in the kernel.  For the
different types lsof will display different data, including
different major and minor device numbers in the DEVICE
column, different lengths in the SIZE/OFF column, different
node numbers in the INODE column, and slightly different
file system names in the NAME column.

In contrast, fuser, where available, can only report on
one reference type when supplied the file system name as
an argument.  Usually it will report on the one that is
associated with the mounted file system information.  If
the only reference type is the temporary automounter one,
fuser will often be silent about it.

### Why don't lsof and netstat output match?

Lsof and netstat output don't match because lsof reports
the network information it finds in open file system objects
-- e.g., socket files -- while netstat often gets its
information from separate kernel tables.

The information available to netstat may describe network
activities never or no longer associated with open files,
but necessary for proper network state machine operation.

For example, a TCP connection in the FIN_WAIT_[12] state
may no longer have an associated open file, because the
connection has been closed at the application layer and is
now being closed at the TCP/IP protocol layer.

#### Why can't lsof find accesses to some TCP and UDP ports?

Lsof stands for LiSt Open Files.  If there is no open file
connected to a TCP or UDP port, lsof won't find it.  That's
the most common reason why lsof doesn't find a port netstat
might report open.

One reason I've found on some UNIX dialects is that their
kernels set aside TCP and UDP ports for communicating with
support activities, running in application layer servers
-- the automounter daemons, and the NFS biod and nfsd
daemons are examples.  Netstat may report the ports are in
use, but lsof doesn't.

Another reason is that netstat may also be able to report
a port is open on a particular dialect, because it uses a
source of data different from what lsof uses -- e.g.,
netstat might examine kernel tables or use streams messages
to MIB2, while lsof relies on the information it finds in
open file structures and their descendants.

Sometimes it's possible to search the data netstat and lsof
use.  For example, on Linux /proc/tcp and /proc/udp can be
examined.  There might an entry there for a particular
protocol and port, but if the line on which the port appears
doesn't have an inode number that matches an inode number
of an open file, lsof won't be able to identify the process
using the port.

This is a tough question to which there is no easy answer.

### Why does lsof update the device cache file?

At the end of the lsof output you may see the message:

	lsof: WARNING: /Homes/abe/.lsof_vic was updated.

In this message /Homes/abe/.lsof_vic is the path to the
private device cache file for login abe.  (See 00DCACHE.)

Lsof issues this message when it finds it necessary to
recheck the system device directory (e.g., /dev or /devices)
and rebuild the device cache file during the open file
scan.  Lsof may need to do these things it finds that a
device directory node has changed, or if it cannot find a
device in the cache.

### Why doesn't lsof report state for UDP socket files?

Lsof reports UDP TPI connection state -- TS_IDLE (Idle),
TS_BOUND (Bound), etc. -- for some, but not all dialects.
TPI state is stream-based TCP/IP information that isn't
available in many dialects.

A fairly weak general rule is if netstat(1) reports UDP
TPI state, lsof may be able to report it, too.  But don't
be surprised if lsof fails to report UDP TPI state for your
dialect.  Other factors influence lsof's ability to report
UDP TPI state, including the availability of state number
data in kernel structures, and state number to state name
conversion data.

### I am editing a file with vi; why doesn't lsof find the file?

Classic implementations of vi usually don't keep open the file
being edited.  (Newer ones may do so in order to maintain an
advisory lock.)  Instead classic vi opens the file, makes a
temporary copy (usually in /tmp or /usr/tmp), and does its work
in that file.  When you save the file being edited from a
classic vi implementation, it reopens and rewrites the file.

During a classic vi session, except for the brief periods when
vi is reading or rewriting the file, lsof won't find an open
reference to the file from the vi process, because there is
none.

### Why doesn't lsof report TCP/TPI window and queue sizes for my
dialect?

Lsof only reports TCP/TPI window sizes for Solaris, because
only its netstat reports them.  The intent of providing
TCP/TPI information in lsof NAME column output is to make
it easier to match netstat output to lsof output.

In general lsof only reports queue sizes for both TCP and
UDP (TPI) connections on BSD-derived UNIX dialects, where
both sets of values appear in kernel socket queue structures.
SYSV-derived UNIX dialects whose TCP/IP implementations
are based on streams generally provide only TCP queue sizes,
not UDP (TPI) ones.

While you may find that netstat on some SYSV-derived UNIX
dialects with streams TCP/IP may report UDP (TPI) queue
sizes, you will probably also find that the sizes are always
zero -- netstat supplies a constant zero for UDP (TPI)
queue sizes to make its headers align the same for TCP and
UDP (TPI) connections.  Solaris seems to get it right --
i.e., its netstat does not report UDP (TPI) queue sizes.

When in doubt, I chose to avoid reporting UDP (TPI) queue
sizes for UNIX dialects whose netstat-reported values I
knew to be a constant zero or whose origin I couldn't
determine.  OSR is a dialect in this category.

#### Why doesn't lsof report socket options, socket states, and TCP
flags and values for my dialect?

The lsof -T argument, 'f', that selects the reporting of socket
options, socket states and TCP flags was implemented at lsof
revision 4.71 for the following UNIX dialects, providing the
indicated information:

	AIX 4.3.2 and 5.1 and above
	All socket options and values, socket states, and TCP
	flags and values described in lsof(8) are reported.
	Apple Darwin 7.2 and above
	All socket options and values, socket states, and TCP
	flags and values described in lsof(8) are reported.
	FreeBSD 4.9 and above
	All socket options and values, socket states, and TCP
	flags and values described in lsof(8) are reported.
	HP-UX 11.00 (/dev/kmem-based lsof)
	All socket options and values are reported.  No socket
	states are reported.  Only the TF_NODELAY TCP flag and
	the TF_MSS value are reported.
	HP-UX 11.11 and iiiv2 (PSTAT-based lsof)
	All socket options and values, and socket states are
	reported.  No TCP flags or values are reported.
	Linux
	No socket options and values, socket states, or TCP
	flags and values are reported.  The support for "-Tf"
	could not be added to Linux, because socket options,
	socket states, and TCP flags and values are not
	available via the /proc file system.
	NetBSD 1.6G and above
	All socket options and values, socket states, and TCP
	flags and values described in lsof(8) are reported.
	OpenBSD 3.4 and above
	All socket options and values, socket states, and TCP
	flags and values described in lsof(8) are reported.
	OpenUNIX 8
	All socket options and values, socket states, and TCP
	flags and values described in lsof(8) are reported.
	SCO OpenServer Release 5.0.6
	All socket options and values, socket states, and TCP
	flags and values described in lsof(8) are reported.
	Solaris 2.6, 8 and above
	The socket option display is limited to BROADCAST,
	DEBUG, DGRAM_ERRIND, DONTROUTE and OOBINLINE.  Socket
	values are limited to KEEPALIVE and LINGER.  No socket
	states are reported.  The TCP DELACK, NODELAY and
	SENTFIN flags are reported.  The TCP MSS value is
	reported.
	UnixWare 7.1.[134]
	All socket options and values, socket states, and TCP
	flags and values described in lsof(8) are reported.

#### Why doesn't lsof report the partial listen queue connection
count for my dialect?

The reporting of partial listen queue connections was added to
-Tf processing at lsof revision 4.76.  Currently it is reported
for these dialects:

	AIX 4.3.2
	This dialect is no longer supported, so no attempt
	was made to add partial listen queue length support
	for it.
	AIX 5.1 and above
	Partial listen queue information is available.
	Apple Darwin 7.2 and above
	Partial listen queue information is available.
	FreeBSD 4.9 and above
	Partial listen queue information is available.
	HP-UX 11.00 (/dev/kmem-based lsof)
	No partial listen queue information is available.
	HP-UX 11.11 and iiiv2 (PSTAT-based lsof)
	No partial listen queue information is available.
	Linux
	No partial listen queue information is available.
	NetBSD 1.6G and above
	Partial listen queue information is available.
	OpenBSD 3.4 and above
	Partial listen queue information is available.
	OpenUNIX 8
	This dialect is no longer supported, so no attempt
	was made to add partial listen queue length support
	for it.
	SCO OpenServer Release 5.0.6
	No partial listen queue information is available.
	Solaris 2.6, 8 and above
	Partial listen queue information is available.
	Tru64 UNIX 5.0
	This dialect is no longer supported, so no attempt
	was made to add partial listen queue length support
	for it.
	Tru64 UNIX 5.1
	Partial listen queue information is available.
	UnixWare 7.1.[134]
	Partial listen queue information is available.


### What does "no more information" in the NAME column mean?

When lsof can find no successor structures -- a gnode,
inode, socket, or vnode -- connected to the file structure
of an open descriptor of a process, it reports "no more
information" in the NAME column.  The TYPE, DEVICE, SIZE/OFF,
and INODE columns will be blank.

Because the file structure is supposed to contain a pointer
to the next structure of a file's processing support, if
the pointer is NUL, lsof can go no further.

Some UNIX dialects have file structures for system processes
-- e.g., the sched process -- that have no successor
structure pointers.  The "no more information" NAME will
commonly appear for these processes in lsof output.

It may also be the case that lsof has read the file structure
while it is being assembled and before a successor structure
pointer value has been set.  The "no more information" NAME
will again result.

Unless lsof output is filled with "no more information"
NAME column messages, the appearance of a few should be no
cause for alarm.

### Why doesn't lsof find a process that ps finds?

If lsof fails to display open files for a process that ps
indicates exists, there may be several reasons for the
difference.

The process may be a "zombie" for which ps displays the
"(defunct)" state.  In that case, the process has exited
and has no open file information lsof can display.  It does
still have a process structure, sufficient for the needs
of ps.

Another possible explanation is that kernel tables and
structures may have been changing when lsof looked for the
process, making lsof unable to find all relevant process
structures.  Try repeating the lsof request.

### Why doesn't -V report a search failure?

The usual reason that -V won't report a search failure is
that lsof located the search item, but was prevented from
listing it by an option that doesn't participate in search
failure reporting.

For example, this lsof invocation:

	$ lsof -V -i TCP@foobar -a -d 999

won't report it can't find the Internet address TCP@foobar,
even if there is an open file connected to that address,
unless the open file also has a file descriptor number of
999 (the `-a -d 999` options).

Compile-time options can also affect -V results in much the
same way.  For example, if HASSECURITY and HASNOSOCKSECURITY
are defined at compile time, this lsof invocation, run by a
non-root user:

	$ lsof -V -c inetd

won't report that it can't find the inetd command, even if
there is a process running the inetd command, because the
HASSECURITY and HASNOSOCKSECURITY options prevent the
listing of all but the socket files of another user, and
no socket file selector (e.g., "-i") was specified.


### Portmap problems

#### Why isn't a name displayed for the portmap registration?

When portmap registration reporting is enabled, any time
there is a registration for a local TCP or UDP port, lsof
displays it in square brackets, following the port number
or service name -- e.g., `:1234[name]` or `:name[100083]`.

The TCP or UDP port number or service number (what follows
the `:') is displayed under the control of the lsof -P
option.  The registration identity is held by the portmapper
and may be a name or a number, depending on how the
registration's owner declared it.  Lsof reports what the
port map holds and cannot derive a registration name from
a registration number.

Lsof can be compiled with registration reporting enabled
or disabled by default, under the control of the HASPMAPENABLED
#define (usually in machine.h).  The lsof help panel (`lsof
-h`) will show the default.  Lsof is distributed with
reporting disabled by default.

#### How can I display only portmap registrations?

Lsof doesn't have an option that will display only TCP or
UDP ports with portmap registrations.  The +M option only
enables the reporting of registration information when
Internet socket files are displayed; +M doesn't select
the displaying of Internet socket files -- the -i option
does that.

This simple lsof pipe to grep will do the job:

	$ lsof -i +M | grep "\["

This works because -i selects Internet socket files, +M
enables portmap registration reporting, and only output
lines with opening square brackets will have registrations.

When portmap registration reporting is enabled by default,
because the lsof builder constructed it that way, +M is
not necessary.  (The lsof help panel, produced with `lsof
-h` will display the default portmapper registration
reporting state.)  However, specifying +M when reporting
is already enabled is acceptable, as is specifying -M when
reporting is already disabled.

Digression: lsof will accept `+' or `-' as a prefix to most
options.  (That isn't documented in the man page or help
panel to reduce confusion and complexity.)  The -i option
is as acceptable as +i, so the above example could be
written a little more tersely as:

	$ lsof +Mi | grep "\["

But be careful to use the `Mi` ordering, since `iM`
implies M is an address argument to `i`.

#### Why doesn't lsof report portmap registrations for some ports?

Lsof reports portmap registrations for local TCP and UDP
ports only.  It identifies local ports this way:

*  The port appears in the local address section of the
	kernel structure that contains it.

*  The port appears in the foreign address section of a
	kernel structure whose local and foreign Internet
	addresses are the same.

*  The port appears in the foreign address section of a
	kernel address structure whose Internet address is
	INADDR_LOOPBACK (127.0.0.1).

Following these rules, lsof ignores foreign portmapped
ports.  That's done for reasons of efficiency and possible
security prohibitions.  Contacting all remote portmappers
could take a long time and be blocked by network difficulties
(i.e., be inefficient).  Many firewalls block portmapper
access for security reasons.

Lsof may occasionally ignore portmap registration information
for a legitimate local port by virtue of its local port
rules.  This can happen when a port appears in the foreign
part of its kernel structure and the local and foreign
Internet addresses don't match (perhaps because they're on
different interfaces), and the foreign Internet address
isn't INADDR_LOOPBACK (127.0.0.1).

#### Why doesn't lsof report portmap registrations for some Solaris
versions?

In some versions of Solaris -- 9 and 10 are known to exhibit
this problem -- lsof is unable to display portmap registrations.

This portmap registration reporting failure occurs when the
Solaris netconfig field (in /etc or etc/inet) has its first two
non-comment lines enabling tcp6 and udp6.  When netconfig is
configured in that fashion, lsof's attempt to read the portmap
via an RPC function fails.

I don't have an explanation for the failure, but this comment
in the netconfig(4) man page appears to have some bearing on
the problem:

	# The following two entries starting with udp6 and tcp6 are
	# meant to be used for IPv6. If you have Ipv6 enabled on your
	# machine then you can uncomment these two lines to enable
	# RPC and NFS to use the Ipv6 stack.
	...
	#udp6  tpi_clts      v  inet6  udp  /dev/udp6  -
	#tcp6  tpi_cots_ord  v  inet6  tcp  /dev/tcp6  - "

My interpretation of that comment is that there is a different
RPC interface to the portmap when IPv6 is enabled.  However, I
can't find any documentation on it in the RPC man pages.  If
anyone has information on it, please send it to me at
<abe@purdue> and put "lsof Solaris portmap" in the subject
line.

A work-around may be to move the ucp6 and tcp6 lines after the
udp and tcxp lines in netconfig.  I don't know if that change
has any unacceptable consequences, but it works for me on my
Solaris 9 test system, and I have a report that it also works
on Solaris 10.


### Why is `lsof | wc` bigger than my system's open file limit?

There is a strong temptation to count open files by piping
lsof output to wc.  If your purpose is to compare the number
you get to some Unix system parameter that defines the
number of open files your system can have, resist the
temptation.

One reason is that lsof reports a number of "files" that
don't occupy Unix file table space -- current working
directories, root directories, jail directories, text files,
library files, memory mapped files are some.  Another reason
is that lsof can report a file shared by more than one
process that itself occupies only one file table slot.

If you want to know the number of open files that occupy
file table slots, use the +ff option and process the lsof
output's FILE_ADDR column information with standard Unix
tools like cut, grep, sed, and sort.

You might also consider using use lsof's field output with
+ff, selecting the file struct address with -FF, and
processing the output with an AWK or Perl script.  See the
list_fields.awk, list_fields.perl, and shared.perl5 scripts
in the scripts/ subdirectory of the lsof distribution for
hints on file struct post-processing filters.

### Why doesn't lsof report file offset (position)?

Lsof won't report a file offset (position) value if the -s
option (without parameters) has been specified, or if the
dialect doesn't support the displaying of file offset
(position).  (Note that on selected dialects the help output,
obtained with -h or -?, may show that the -s option can also be
supplied the "p:s" parameters; for more information on that
addition, see the answer to the "How are protocol state name
exclusion and inclusion used?" question.)

That lsof is reporting only file size is indicated by the
fact that the appropriate column header says SIZE instead
of SIZE/OFF.

If lsof doesn't support the displaying of file offset
(position) -- e.g., for Linux /proc-based lsof -- the -h
or -? output panel won't list the -o option.

Sometimes the availability of file offset information
depends on the dialect's kernel.  This is particularly true
for socket file offsets.

Maintenance of offsets for pseudo-terminal devices varies
by UNIX dialect and is related to how the dialect kernel
implements pseudo-terminal support.  Kernels like AIX, for
example, that short-circuit the transfer of data between
socket and pseudo devices to reduce TCP/IP daemon interrupt
rates won't advance offsets in the TCP/IP daemon socket
files.  Instead they will advance offsets in the open
standard I/O files of the shell child precess where the
pseudo-terminal devices are used.

When in doubt about the behavior of lsof in reporting file
offset information, do some carefully measured experiments,
consult the lsof sources, or contact me at <abe@purdue.edu>
to discuss the matter.  Please follow the reporting guidelines
in the "How do I report an lsof bug?" section of this FAQ.

#### What does lsof report for size when the file doesn't really have one?

When a file has no true size -- e.g., it's a socket, a
FIFO, or a pipe -- lsof tries to report the information it
finds in the kernel that describes the contents of associated
kernel buffers.

Thus, for example, size for most TCP/IP files is socket
buffer size.  The size of the socket read buffer is reported
for read-only files; the size of the write buffer for
write-only files; and the sum of the buffers sizes for
read-write files.

### Problems with path name arguments

#### How do I ask lsof to search a file system?

You can ask lsof to search for all open files on a file
system by specifying its mounted path name as an lsof
argument -- e.g.,

	$ lsof /

Output of the mount command will show file system mounted
path names.  It will also show the mounted-on device path
for the file system.

If the mounted-on device is a block device (the permission
field in output of `ls -l <device>` starts with a `b/),
you can specify its name, too -- e.g.,

	$ lsof /dev/sd0a

If the mounted-on device isn't a block device -- for example,
some UNIX dialects call a CD-ROM device a character device
(ls output starts with a `c') -- you can force lsof to
assume that the specified device names a file system with
the +f option -- e.g.,

	$ lsof +f -- /dev/sd0a

(Note: you must use ``--'' after +f or -f if a file name
follows immediately, because  +f and -f can be followed by
characters that specify flag output selections.)

When you use +f and lsof can't match the device to a file
system, lsof will issue a complaint.

The +f option may be used in some dialects to ask lsof to
search for an NFS file system by its server name and server
mount point.  If the mount application reports an NFS file
system mounted-on value that way, then this sample lsof
request should work.

	$ lsof +f -- fleet:/home/fleet/u5

Finally, you can use -f if you don't want a mounted file
system path name to be considered a request to report all
open files on the file system.  This is useful when you
want to know if anyone is using the file system's mounted
path name.  This example directs lsof to report on open
access to the `/' directory, including when it's being used
as a current working or root directory.

	$ lsof -f -- /

The lsof -f option performs the same function as -f does
in some fuser implementations.  However, since the lsof -c
option was chosen for another purpose before the `f' option
was added to lsof, +f was selected as the analogue to the
fuser -c option.  (Sorry for the potential confusion.)

#### Why doesn't lsof find all the open files in a file system?

Lsof may not find all the open files in a file system for
several reasons.

First, some processes with files open on the file system
may have been changing status when lsof examined the process
table, and lsof "missed" them.  Remember, the kernel changes
much faster than lsof can respond to the changes.

Second, be sure you have specified the file system correctly.
Perhaps you specified a file instead.  You can use lsof's
-V option to have lsof report in detail on what it couldn't
find.  Make sure the report for the file system you specified
says "file system."  Here's some -V output:

	$ /lsof -V /tmp ./lsof.h ./lsof
	COMMAND  PID USER   FD   TYPE DEVICE SIZE/OFF  INODE NAME
	lsof    2688  abe  txt   VREG 18,1,7  1428583 226641 ./lsof
	lsof    2689  abe  txt   VREG 18,1,7  1428583 226641 ./lsof
	lsof: no file use located: ./lsof.h

You can also use lsof's +f option to force it to consider
a path name as a file system.  If lsof can't find a file
system by the specified name, it will issue a complaint --
e.g.,

	$ lsof +f -- /usr
	lsof: not a file system: /usr

(/usr is a directory in the / file system.)

#### Why does the lsof exit code report it didn't find open files
when some files were listed?

Sometimes lsof will list some open files, yet return a
non-zero exit code, suggesting it hasn't found all the
specified files.

The first thing you should when you suspect lsof is incorrect
is to repeat the request, adding the -V option.  In the
resulting report you may find that your file system
specification really wasn't a file system specification,
just a file specification.

Finally, if you specify two files or two file systems twice,
lsof will credit all matches to the first of the two and
believe that there were no matches for the second.  It's
possible to specify a single file system twice with different
path names by using both its mounted directory path name
and mounted-one device name.

	$ lsof +f -V spcuna:/sysprog /sysprog
	COMMAND   PID USER   FD   TYPE DEVICE SIZE/OFF  INODE NAME
	ksh     11092  abe  cwd   VDIR 39,0,1     1536 226562 /sysprog
	(spcuna:/sysprog)
	...
	lsof: no file system use located: spcuna:/sysprog

All matches were credited to /sysprog; none to spcuna:/sysprog.

#### Why won't lsof find all the open files in a directory?

When you give lsof a simple directory path name argument
(not a file system mounted-on name), you are asking it to
search for processes that have the directory open as a
file, or as a process-specific directory -- e.g., root or
current working directory.

If you want to list instances of open files inside the
directory, you need to either specify the individual path names
of those files, their common mount point, or use the lsof +D 
and +d options.

For example, if you wanted to check the files open in your /lsof 
folder then the following might be the quickest way to get
the answer you are looking for:

lsof / | grep /lsof | grep -v DIR

See the answer to the question "Why are the +D and +d
options so slow?" before you use +D or +d casually.

See the answer to the question "Why do the +D and +d options
produce warning messages?" for an explanation of some
process authority limitations of +D and +d.

#### Why are the +D and +d options so slow?

The +D and +d options cause lsof to build a path name search
list for a specified directory.  +D causes lsof to descend
the directory to its furthest subdirectory, while +d
restricts it to the top level.  In both cases, the specified
directory itself is included in the search list.  In both
symbolic links are ignored.

Building such a search list can take considerable time,
especially when the specified directory contains many files
and subdirectories -- lsof must call the system readlink()
and stat() functions for each file and directory.  Storing
the search list can cause lsof to use more than its normal
amount of dynamic memory -- each file recorded in the search
list consumes dynamic memory for its path name, characteristics,
and search linkages.  Using the list means lsof must search
it for every open file in the system.

Building the search list for a directory specified on some
file systems can be slow -- e.g., for an NFS directory with
many files.  Some file systems have special logging features
that can introduce additional delays to the building of
the search list -- e.g., NFS logging, or logging on a
Solaris UFS file system.  The bottom line is that slow
search list construction may not be so much an lsof problem
as a file system problem.  (Hint: if you're using Solaris
UFS logging, consider specifying the "logging,noatime"
option pair to reduce the number of atime writes to the
UFS logging queue and disk.)

A somewhat risky way to speed up lsof's building of the
search list is to use lsof's ``-O'' option.  It forces lsof
to do all system calls needed to build the search list
directly, rather than in a child process.  While direct
system calls are much faster, they can block in the kernel
-- e.g., when an NFS server stops responding -- stopping
lsof until the kernel operation unblocks.

As an example of the load +D can impose, consider that an
`lsof +D /` on a lightly loaded NeXT '040 cube with a 1GB
root file system disk took 4+ minutes of real time.  It
also generated several hundred error messages about files
and directories the lsof process didn't have permission to
access with stat(2).

The bottom line is that +D and +d should be used cautiously.
+D is more costly than +d for deeply nested directory trees,
because of the full directory descent it causes.  So use
+d where possible.  And you might need to consider the
performance of the file system that holds the directory
you name with +d or +D.

In view of these warnings, when is it appropriate to use
+D or +d?  Probably the most appropriate time is when you
would specify the directory's contents to lsof with a shell
globbing construct -- e.g., `lsof *`.  If that's what you
need to do, `lsof +d .` is probably more efficient than
having the shell produce a directory list, form it into an
argument vector, and pass the vector to lsof for it to
unravel.

See the answer to the question "Why do the +D and +d options
produce warning messages?" for an explanation of some
process authority limitations of +D and +d.

#### Why do the +D and +d options produce warning messages?

+D and +d option processing is limited by the authority of
the lsof process -- i.e., lsof can only examine (with
lstat(2) and stat(2)) files the owner of the process can
access.

If the ownership, group membership, or permissions of the
specified directory, file within it, or directory within
it prevents the owner of the lsof process from using lstat(2)
or stat(2) on it, lsof will issue a warning message, naming
the path and giving the system's (lstat(2's or stat(2)'s)
reason (errno explanation text) for refusing access.

As an example, assume user abc has a subdirectory in /tmp,
owned by abc and readable, writable and searchable by only
its owner.  If user def asks lsof to search for all /tmp
references with +D or +d, lsof will be unable to lstat(2)
or stat(2) anything in abc's private subdirectory, and will
issue an appropriate warning.

Lsof warnings can usually be suppressed with the -w option.
However, using -w with +D or +d means that there will be
no indication why lsof couldn't find an open reference to
a restricted directory or something contained in it.

Hint: if you need to use +D or +d and avoid authority
warnings, and if you have super-user power, su and use lsof
with +D or +d as root.

### Why can't my C compiler find the rpcent structure definition?

When you try to compile lsof your compiler may complain
that the rpcent structure is undefined.  The complaints
may look like this:

	>print.c: In function `fill_portmap':
	>print.c:213: dereferencing pointer to incomplete type
	>...

The most likely cause is that someone has allowed a BIND
installation to update /usr/include/netdb.h (or perhaps
/usr/include/rpc/netdb.h), removing the rpcent structure
definition that lsof expects to find there.

Only Solaris has an automatic work-around.  (See dlsof.h
in dialects/sun.).  The Solaris work-around succeeds because
there is another header file, <rpc/rpcent.h>, with the rpcent
structure definition, and there is a Solaris C pre-processor
test that can tell when the BIND <netdb.h> is in place and
hence <rpc/rpcent.h> must be included.

Doubtlessly there are similar work-arounds possible in
other UNIX dialects whose header files have been "touched"
by BIND, but in general I recommend restoration of the
vendor's <netdb.h> and any other header files BIND might
have replaced.  (I think BIND replaces <resolv.h>,
<sys/bitypes.h>, <sys/cdefs.h> -- and maybe others.)

### Why doesn't lsof report fully on file "foo" on UNIX dialect "bar?"

Lsof sometimes won't report much information on a given
file, or may even report an error message in its NAME
column.  That's usually because the file is of a special
type -- e.g., in a file system specific to the UNIX dialect
-- and I haven't used a system where the file appeared
during my testing.

If you encounter such a situation, send me e-mail at
<abe@purdue.edu> and we may be able to devise an addition to
lsof that will report on the file in question.  Please follow
the reporting guidelines in the "How do I report an lsof bug?"
section of this FAQ.  Make sure "lsof" appears in the
"Subject:" line so my e-mail filter won't classify your letter
as Spam.

### Why do I get a complaint when I execute lsof that some library file can't be found?

On systems where the LIBPATH (or the equivalent) environment
variable is used to record the library search path in
executable files when they are built, an incorrect value
may make it impossible for the system to find the shared
libraries needed to load lsof for execution.

This may be particularly true on systems like AIX >= 4.1.4,
where the lsof Makefile takes the precautionary step of
using the -bnolibpath loader flag to insure that the path
to the private static lsof library is not recorded in the
lsof binary.  Should LIBPATH be invalid when lsof is built,
it will be recorded in the lsof binary as the default
library path search order and lead to an inability to find
libraries when lsof is executed.

So, if you get missing library complaints when you try to
execute lsof, check LIBPATH, or whatever environment variable
is used on your system to define library search order in
executable files.  Use the tools at your disposal to look
at the library paths recorded in the lsof binary -- e.g.,
chatr on HP-UX, dump on AIX, ldd on Solaris.

Make sure, too, that when the correct library search path
has been recorded in the executable file, the required
library files exist at one or more of the search paths.


### Why does lsof complain it can't open files?

When lsof begins execution, unless it has been asked to
report only help or version information, typically it will
attempt to access kernel memory and symbol files -- e.g.,
/unix, /dev/kmem.  Even though lsof needs only permission
to open these files for reading, read access to them might
be restricted by ownerships and permission modes.

So the first step to diagnosing lsof problems with opening
files is to use ls(1) to examine the ownerships and permission
modes of the files that lsof wants to open.  You may find
that lsof needs to be installed with some type of special
ownership or permission modes to enable it to open the
necessary files for reading.  See the "Installing Lsof"
section of 00README for more information.

### Why does lsof warn "compiled for x ... y; this is z."?

Unless warnings are suppressed (with -w) or the kernel
identity check symbol (HASKERNIDCK) definition has been
deleted, all but one lsof dialect version (exception:
/proc-based Linux lsof) compare the identity of the running
kernel to that of the one for which lsof was constructed.
If the identities don't match, lsof issues a warning like
this:

	lsof: WARNING: compiled for Solaris release 5.7; this is 5.6.

Two kernel identity differences can generate this warning
-- the version number and the release number.

Build and running identity differences are usually significant,
because they usually indicate kernels whose structures are
different -- kernel structures commonly change at dialect
version releases.  Since lsof reads data from the kernel
in the form of structures, it is sensitive to changes in
them.  The general rule is that an lsof compiled for one
UNIX dialect version will not work correctly when run on
a different version.

There are three work-arounds: 1) use -w to suppress the
warning -- and risk missing other warnings; 2) permanently
disable the identity check by deleting the definition of
HASKERNIDCK in the dialect's machine.h header file -- with
the same risk; or 3) rebuild lsof on the system where it
is to be run.  (Deleting HASKERNIDCK can be done with the
Customize script or by editing machine.h.)

Generally checking kernel identity is a quick operation
for lsof.  However, it is potentially slow under AIX, where
lsof must run /usr/bin/oslevel.  To speed up lsof, use -w
to suppress the /usr/bin/oslevel test.  See "Why does AIX
lsof start so slowly?" for more information.

### How can I disable the kernel identity check?

The kernel identity check is controlled by the HASKERNIDCK
definition.  When it is defined, most dialects (exclusion:
/proc-based Linux lsof) will compare the build-time kernel
identity with the run-time one.

To disable the kernel identity check, disable the HASKERNIDCK
definition in the dialect's machine.h header file.  The
Customize script can be used to do that in its section
about the kernel identity check.

Caution: while disabling the kernel identity check may
result in smaller lsof startup overhead, it comes with the
risk of executing an lsof that may produce warning messages,
error messages, incorrect output, or no output at all.

### Why don't ps(1) and lsof agree on the owner of a process?

Generally the user ID lsof reports in its USER column is
the process effective user ID, as found in the process
structure.  Sometimes that may not agree with what ps(1)
reports for the same process.

There are sundry reasons for the difference.  Sometimes
ps(1) uses a different source for process information,
e.g., the /proc file system or the psinfo structure.
Sometimes the kernel is lax or confused (e.g., Solaris
2.5.1) about what ID to report as the effective user ID.
Sometimes the system carries only one user ID in its process
structure (some BSD derivatives), leaving lsof no choice.

The differences between lsof and ps(1) user identifications
should be small and normally it will be apparent that the
confusion is over a process whose application has changed
to an effective user ID different from the real one.

### Why doesn't lsof find an open socket file whose connection
state is past CLOSE_WAIT?

TCP/IP connections in states past CLOSE_WAIT -- e.g.,
FIN_WAIT_1, CLOSING, LAST_ACK, FIN_WAIT_2, and TIME_WAIT
-- don't always have open files associated with them.  When
they don't, lsof can't identify them.  When the connection
state advances from CLOSE_WAIT, sometimes the open file
associated with the connection is deleted.

### Why don't machine.h definitions work when the surrounding
comments are removed?

The machine.h header files in dialect subdirectories have
some commented-out definitions like:

	/* #define HASSYSDC "/your/choice/of/path */

You can't simply remove the comments and expect the definition
to work.  That's intended to make you think about what
value you are assigning to the symbol.  The assigned value
might have a system-specific convention.  HASSYSDC, for
example, might be /var/db/lsof.dc for FreeBSD, but it might
be /var/adm/lsof.dc for Solaris.

Symbols defined in the lsof documentation are described in
00PORTING, other machine.h comments, and other lsof
documentation files.  HASSYSDC, for example, is discussed
in 00DCACHE.  When comments and documentation don't suffice,
consult the source code for hints on how the symbol is
used.

### What do "can't read inpcb at 0x...", "no protocol control block", "no PCB, CANTSENDMORE, CANTRCVMORE", etc. mean?

Sometimes lsof will report "can't read inpcb at 0x00000000",
"no protocol control block", "no PCB, CANTSENDMORE,
CANTRCVMORE" or a similar message in the NAME column for
open TCP socket files.  These messages mean the file's socket
structure lacks a pointer to the INternet Protocol Control
Block (inpcb) where lsof expects to find connection addresses
-- local and foreign ports, local and foreign IP addresses.
The socket file has probably been submitted to the shutdown(2)
function for processing.

In some implementations lsof issues the "no PCB, CANTSENDMORE,
CANTRCVMORE" message, which tries to explain the absence
of a protocol control block by showing the socket state
settings that have been made by the shutdown(2) function.

If a non-zero address follows the "0x" in the "can't read
inpcb" message, it means lsof couldn't read inpcb contents
from the indicated address in kernel memory.

### What do the "unknown file system type" warnings mean?

Lsof may report a message similar to"

	unknown file system type, v_op: 0x10472f10

in the NAME column for some files.

This means that lsof has encountered a vnode for the file
whose operation switch address (from v_op) references a
file system type for which there is no support in lsof.
After lsof identifies the file system type, it uses
pre-compiled code to locate the file system specific node
for the file where lsof finds information like file size,
device number, node number, etc.

To get some idea of what the file system type might be,
use nm on your kernel symbol file to locate the symbol name
that corresponds to the v_op address -- e.g., on Solaris
do:

	$ nm -x /dev/ksyms | grep 0x10472f10
	0x10472f10 ... |file_system_name_vnodeops

Where "file_system_name" is the clue to the unsupported
file system.

Lsof doesn't use the v_op address to identify file system
types on all dialects.  Sometimes it uses an index number
it finds in the vnode.  It will translate that symbol to
a short name in the warning message -- e.g., "nfs3" -- if
possible.

### Installation

#### How do I install lsof?

There is no "standard" way to install lsof.  Too much
depends on local conditions for me to be able to provide
working install rules in the lsof make files.  (The skeleton
install rules you will find just give "hints.")  See the
"Installing Lsof" section of 00README for a fuller explanation.

To install lsof you will need to consider these questions:

*  Who should be able to use lsof?  (See HASSECURITY and
	HASNOSOCKSECURITY in the "Security" section of 00README.)

*  Where should lsof be installed?  This is a decision
	mostly dictated by local conditions.  Somewhere in
	/usr/local -- etc/ or sbin/ -- is a common choice.

*  What permissions should I give the lsof executable?
	The answer to this varies by dialect.  The make files
	have install rules that give hints.  The "Installing
	Lsof" section of 00README gives information, too.

*  What if I want to install lsof in a shared file system
	for machines that require different lsof configurations?
	See the next question and answer, "How do I install a
	common lsof when I have machines that need differently
	constructed lsof binaries?"

#### How do I install a common lsof when I have machines that need differently constructed lsof binaries?

A dilemma that faces some system administrators when they
install lsof in a shared file system -- e.g., NFS -- is
that they must have different lsof executables for different
systems.

The answer is to build an lsof wrapper script that is
executed in place of lsof.  The script can use system
commands to determine which lsof binary should be executed.

Consider this example.  You have HP-UX machines with 32
and 64 bit kernels that share the /usr/local/sbin directory
where you want to install lsof.  Consequently, on each
system you must use a different lsof executable, built for
the system's bit size.  (That's because lsof reads kernel
structures, sized by the kernel's bit size.)

One answer is to install three things in /usr/local/sbin:
1) a 32 bit lsof as lsof32; 2) a 64 bit lsof as lsof64;
and 3) an lsof script.  The script might look like this
one, based on work by Amir J. Katz:

	#!/bin/sh
	x=`/usr/bin/getconf KERNEL_BITS`  # returns 32 or 64
	if /usr/bin/test "X$x" = "X32"
	then
	lsof32 $*
	else
	if /usr/bin/test "X$x" = "X64"
	then
	lsof64 $*
	else
	echo "Can't determine which lsof executable to use;"
	echo "getconf KERNEL_BITS says: $x"
	exit 1
	fi
	fi

Solaris users should consult "How do I install lsof for
Solaris 7, 8 or 9?" for information on a similar trick
using the Solaris isaexec command.

Users of other dialects might be able to use a command like
uname(1) that can identify a distinguishing feature of the
system to be incorporated in pre-installed lsof executable
names.  For example, use `uname -r` and install binaries
with suffixes that match `uname -r` output.

### Why do lsof 4.53 and above reject device cache files built by earlier lsof revisions?

When lsof revisions 4.53 run and encounter a device cache
file built by an earlier revision, it will reject the file
and build a new one.  The rejection will be advertised with
these messages:

	lsof: WARNING: no /dev device in <name>: 2 sections
	...
	lsof: WARNING: created device cache file: <name>

This happens because the header line of the device cache
file was changed at revision 4.53 to contain the number of
the device on which the device directory resides.  The old
device cache file header line -- the "2 sections" line in
the above warning message, node reads "2 sections, dev=600".

This is not a serious problem, since lsof automatically
rebuilds the device cache file with the correct header
line.

### What do "like block special" and "like character special" mean in the NAME column?

When lsof comes across an open block or character file
whose device, raw device and inode place it somewhere other
than /dev (or /devices), lsof doesn't report the /dev (or
/devices) name in the NAME column.  Instead lsof reports
the file system name and device or path name in the NAME
column and parenthetically adds "like block special <path>"
or "like character special <path>".

The value for <path> will point to a block or character
device in /dev (or /devices) whose raw device number matches
that of the open file being reported, but whose device
number or node number (or both) don't match.

Such an open file is connected to a device node that has
been created in a directory other than /dev (or /devices.)
See mknod(8) for information on how such nodes are created.
(Generally one needs root power to create device nodes with
mknod.)

### Why does an lsof make fail because of undefined symbols?

When lsof is compiled via the `make` step and the final
load step fails because of missing symbols, the problem
may not be lsof.  The problem may be that ld, called by
the compiler as part of the `make` step, can't find some
library that lsof needs.

First check the last compiler line of the make operation
-- e.g., the last line with cc or gcc in it before the
undefined symbol report -- for loader arguments, i.e.,
ones beginning with "-l".  Except for "-llsof" the rest
name system libraries.  ("-L./lib" precedes "-llsof" to
tell the loader its location.)

Check that all the named system libraries exist.  Look in
/lib and /usr/lib as a start, but that may not be the only
place system libraries live.  Consult your dialect's
documentation, e.g., the compiler and loader man pages,
for other possible locations.

If some system library doesn't exist, that may mean it was
never installed or was removed.  You'll have to re-install
the missing library.

You may find that all the system libraries lsof uses exist.
Your next step might be to use nm and grep to see if any
of them contain the undefined symbols.

	$ nm library | grep symbol

If the undefined symbol exists in some library named by
the lsof make step, then you might have a problem with some
environment variable that controls the load step.  The most
common is LD_LIBRARY_PATH.  It may have a setting that
causes ld to ignore a directory containing a library lsof
names.  If this is the case, try unsetting LD_LIBRARY_PATH
in the environment of the ld process -- e.g., do:

	$ unset LD_LIBRARY_PATH
or
	% unsetenv LD_LIBRARY_PATH

Consult your ld man page for other environment variables
that might affect library searching -- e.g., LIBPATH, LPATH,
SHLIB_PATH, etc.

If the undefined function doesn't exist in any libraries
lsof names, check other libraries.  See if the function
has a man page that names its library.  If the latter is
true, please let me know, because that is an lsof problem
I need to fix.

If none of these solutions work for you, send me some
documentation via e-mail at <abe@purdue.edu>.  Include `uname
-a` output, the output of the lsof `Configure ...` and `make`
steps, and the contents of the environment in force when the
`make` step was executed -- e.g., `env` or `printenv` output.
If you've located the libraries lsof names, send me that
information, too.  Make sure "lsof" appears in the "Subject:"
line so my e-mail filter won't classify your letter as Spam.

### Command Regular Expressions (REs)

#### What are basic and extended regular expressions?

Lsof's ``-c'' option allows the specification of regular
expressions (REs), enclosed in two slash ('/') characters and
followed by these modifiers:

	b	the RE is a basic RE.
	i	ignore case.
	x	the RE is an extended RE (the default).

Note: the characters of the regular expression may need to
be quoted to prevent their expansion by the shell.

Example: this RE is an extended RE that matches exactly
four characters, whose third may be an upper ('O') or lower
case ('o') oh:

	-c /^..o.$/i

For simplicity's sake, an RE that is acceptable to egrep(1)
is usually called an extended RE.

REs suitable for the old line editor, ed(1), are often
called basic REs (and sometimes also called obsolete).

These are some ways basic REs usually differ from extended
REs.  (There are other differences.)

*  `|', `+', `?', '{', and '}' are ordinary characters.

*  `^' is an ordinary character except at the beginning of
	the RE.

*  `$' is an ordinary character except at the end of the
	RE.

*  `*' is an ordinary character if it appears at the
	beginning of the RE.

For more information on REs and the distinction between
basic and extended REs, consult your dialect's man pages
for ed(1), egrep(1), sed(1), and possibly regex(5) or
regex(7).

#### Why can't I put a slash in a command regular expression?

Since a UNIX command name is the last part of a path to
the command's executable, the lsof command regular expression
(RE) syntax uses slash ('/') to mark the beginning and end
of an RE.  Slash may not appear in the RE and the `\'
back-slash escape is ineffective for "hiding" it.

More likely than not, if you try to put a slash in an lsof
command RE, you'll get this response:

	$ lsof -s/.\// ...
	lsof: invalid regexp modifier: /

Lsof is complaining the first character it found after
the second slash isn't an lsof command RE modifier -- 'b',
'i', or 'x'.

#### Why does lsof say my command regular expression wasn't found?

When you use both forms of lsof's -c option --
``-c <command>'' and ``-c /RE/[m]'' -- and ask that lsof
do a verbose search (``-V''), you may be surprised that
lsof will say that the regular expression wasn't found.

This can happen if the ``-c <command>'' form matches first,
because then the ``-c/RE/[m]'' test will never have been
applied.  For example:

	$ ./lsof -clsof -c/^..o.$/ -V -adcwd
	COMMAND  PID USER   FD   TYPE DEVICE SIZE/OFF  NODE NAME
	lsof    7850  abe  cwd   VDIR    6,0     2048 96442 / (/dev/sd0a)
	lsof: no command found for regex: ^..o.$

The ``-clsof'' option matched first, so the ``-c/^..o.$/
option wasn't tested.

### Why doesn't lsof report on shared memory segments?

Lsof reports on shared memory segments only if they're
associated with an open file.  That's consistent with lsof's
mission -- to LiSt Open Files.  Shared memory segments with
no file associations aren't open files.

That's not to say that a report on shared memory segments
and their associated processes wouldn't be useful.  But it
calls for a new tool, not more baggage for lsof.

### Why does lsof report two instances of itself?

When you ask lsof to report all open files and it has
permission to do so, you may see two lsof processes in the
output.  The processes are connected via pipes -- e.g.,
here's an HP-UX 11 example.

	COMMAND     PID USER   FD   TYPE     DEVICE ...
	...
	lsof      29450  abe    7w  PIPE 0x48732408 ...
	lsof      29450  abe    8r  PIPE 0x48970808 ...
	...
	lsof      29451  abe    6r  PIPE 0x48732408 ...
	lsof      29451  abe    9w  PIPE 0x48970808 ...

The first process will usually be the lsof you initiated;
the second, an lsof child process that is used to isolate
its parent process from kernel functions that can block --
e.g., readlink() or stat().

Information to and from the kernel functions is exchanged
via the two pipes.  When the parent process detects that
the child process has become blocked, it attempts to kill
the child.  Depending on the UNIX dialect that may succeed
or fail, but the parent won't be blocked in any event.

See the "BLOCKS AND TIMEOUTS" and "AVOIDING KERNEL BLOCKS"
sections of the lsof man page for more information on why
the child process is used and how you can specify lsof
options to avoid it.  (Caution: that may be risky.)

### Why does lsof report '\n' in device cache file error messages?

Lsof revisions prior to 4.58 may report '\n' in error
messages it delivers about problems in the device cache
file -- e.g.,

	lsof: WARNING: no ...: 4 sections\n

That's deliberately done to show the exact contents of the
device cache file line about which lsof is complaining,
including its terminating NL (New Line) '\n' character.
In the above example the line in the device cache file
causing the lsof complaint contains "4 sections" and ends
with a '\n'.

At revision 4.58 and above, device cache error messages
like the one in the above example have been changed to
read:

	lsof: WARNING: no ...: line "4 sections"

The terminal '\n' is no longer reported, the line contents
are enclosed in double quote marks ('"'), and the word
"line" has been added as a prefix to denote that what
follows is a line from the device cache file.

### Kernel Symbol and Address Problems

#### What does "lsof: WARNING: name cache hash size length error: 0" mean?

When run on some systems, lsof may issue this warning:

	lsof: WARNING: name cache hash size length error: 0

That is an example from a FreeBSD system where lsof reads
the kernel's _nchash variable and finds its value is zero.

Similar warnings include:

	WARNING: kernel name cache size:
	WARNING: can't read kernel's name cache:
	WARNING: no name cache address
	WARNING: name cache hash size length error:
	WARNING: unusable name cache size:

These warnings are issued when lsof is attempting to read
the kernel's name cache information.  They are usually the
result of a mis-match between the addresses for kernel
symbols lsof gets via nlist(2) and the addresses in use by
the kernel.

Lsof usually gets kernel symbol addresses from what it
believes to be the kernel boot file.  In FreeBSD, for
example, that's the path returned by getbootfile(3), usually
/kernel.  The boot file can have other names in other UNIX
dialects -- /unix, /vmunix, /bsd, /netbsd, /mach, /stand/vmunix,
etc.

Lsof will get incorrect (mismatched) addresses from the
boot file if it has been replaced by a newer one which
hasn't yet been booted -- e.g., if this is done in FreeBSD:

	# mv /kernel /kernel.OLD
	# mv /kernel.NEW /kernel

Until the FreeBSD system is rebooted, the booted kernel is
/kernel.OLD, but getbootfile() says it is /kernel.  If
symbol addresses important to lsof in /kernel.OLD and
/kernel don't match, the lsof WARNING messages result.

#### Why does lsof produce "garbage" output?

Kernel name cache warnings may not be the only sign that
lsof is using incorrect symbol addresses to read kernel
values.  If there's no reasonable test lsof can make on
what it reads from the kernel, it may issue other warnings
or even report nonsensical results.

The warnings may appear on STDERR, such as:

	lsof: can't read proc table info

Or the warnings may appear in the NAME column as messages
saying lsof can't read or interpret some kernel structure --
e.g.,

	... NAME
	... can't read file struct from 0x12345

One possible work-around is to point lsof's kernel symbol
address gathering at the proper boot file.  That can be
done with lsof's -k option -- e.g.,

	$ lsof -k /kernel.OLD

The best work-around is to make sure the standard boot file
is properly sited -- e.g., if you've moved a new /kernel
in place, boot it.

### Why does lsof report open files when run as super user that it doesn't report when run with lesser privileges?

The most likely cause is that the HASSECURITY option was
selected when the lsof executable was built.

If HASSECURITY is defined when lsof is built, and lsof is
run with the privileges of a non-ROOT user, it will only
list open files belonging to the user.  The same lsof
executable, when run with root user privileges, will list
all open files.

However, if HASSECURITY and HASNOSOCKSECURITY are both
defined when lsof is built, lsof will list open files
belonging to the user and will also list anyone else's open
socket files, provided their listing is selected with the
"-i" option.

So first ask yourself if the process whose open files lsof
won't list belong to a user other than the one under which
you're running lsof, and are not open socket files.  If
either is true, use lsof's help (-h or -?) option and look
for a line near the bottom of the help panel that says:

	"... can list all files..."

If the leading "..." says "Only root" then HASSECURITY was
defined when lsof was built.  If the trailing "..." says
", but anyone can list socket files" then HASNOSOCKSECURITY
was also defined.

Should you want an lsof not built with HASSECURITY defined,
rerun the lsof Configure script.  If you let Configure do
customization, make sure you answer 'n' when it asks if
you want to enable HASSECURITY and HASNOSOCKSECURITY.  If
you don't need to do customization, you can rebuild lsof
with the "-n" option to Configure.  Here's an example of
such a rebuild sequence:

	$ Configure -clean
	$ Configure -n <dialect-abbreviation>
	$ make

More information on the HASSECURITY and HASNOSOCKSECURITY
options may be found in the "Security" section of the
00README file of the lsof distribution.

### Test Suite Problems

#### Errors all tests can report:

##### Why do tests complain "ERROR!!!  can't execute ../lsof"?

All tests in the test suite expect an executable lsof file
to exist in the tests parent directory, ../lsof.

If there's none there, the tests/Makefile has a rule to
make it, but there are probably circumstances where that
rule may fail.

The work-around is to re-Configure and re-make lsof, then
run the test suite.

##### Why do tests complain "ERROR!!! can't find ..." a file?

Many tests create (or use from a supplied environment
variable path) a test file and use lsof to find it.  When
lsof can't file the file, the tests report the error with
messages of the form:

	ERROR!!!  can't find ... : <some file path>
	or
	ERROR!!!  lsof couldn't find ...

These type of error messages mean that the lsof field output
delivered to the test didn't contain a file that the test
could identify as the one it intended lsof to find.  It
might also mean that the process information -- command
name, PID or parent PID -- didn't match what the test
expected.

This could imply a bug in the test or a bug in lsof.  Try
using lsof to find a known file that is open.  For example,
while in the tests sub-directory, do this:

	$ sleep 30 < Makefile
	$ ../lsof Makefile

If lsof doesn't report that Makefile is open, then the
fault may be with lsof.  If lsof reports the file is open,
search further in the test code for the failure cause.

##### Why do some tests fail to compile?

If a test suite program fails to compile, it may be because
I've never had an opportunity to compile the test on the
particular UNIX version you are using.

See Appendix B in 00TEST for a list of the UNIX dialects
where the test suite has been validate.

##### Why do some tests always fail?

There are several tests in the optional group that have
conflicting or special requirements:

	LTbigf      needs a dialect and file system that support
		large files.

	LTlock      won't work if the tests/ sub-directory is
		on an NFS file system.

	LTnfs       won't work if the tests/ sub-directory is
		not on an NFS file system.

So for two tests in particular, LTlock and LTnfs, one will
generally fail.

Some failing tests can be run successfully by supplying to
them a path to the appropriate type of file system with
the -p option.

##### Why does the test suite say it hasn't been validated on my dialect?

When you use the default rule of the test suite's Makefile,
it may issue this complaint:

	$ cd tests
	$ make
	!!!WARNING!!!

	This dialect or its particular version may not have
	been validated with the lsof test suite.  Consequently
	some tests may fail or may not even compile.

	!!!WARNING!!!

You are then given the opportunity to answer 'y' to have
the test suite operation continue.

This message means that the tests/TestDB file in the tests
sub-directory doesn't show that the test suite has been
run with the combination of compiler flags found in
tests/config.cflags.  The tests might nor run; they may
encounter compiler failures.

See 00TEST for more information on the UNIX dialects where
the test suite has been validated and on the workings of
TestDB and its supporting scripts.

When the tests/Makefile "auto" rule is used, the message
is more terse and the condition is fatal.

	This suite has not been validated on:

	<dialect_description>

No opportunity to continue is offered.

The tests/Makefile "silent" rule will skip checking for
the validation footprint.

##### Why do the tests complain they can't stat() or open() /dev/mem or /dev/kmem?

When the tests detect that lsof for the dialect reads its
information from kernel memory (i.e., the LT_KMEM definition
is present in tests/config.cflags), and when the lsof
executable path is ../lsof, the tests make sure they can
stat() and open() for read access the relevant kernel memory
devices, /dev/kmem and possibly /dev/mem.

If those stat() or open() operations fail, the tests issue
an error message and quit.  The message explains why the
system rejected the operation in terms of system "errno"
symbols and messages.  More often than not the explanation
will be that the process lacks permission to access the
indicated device node.

One work-around is to give the lsof executable being tested
the necessary permission -- e.g., via chgrp, chmod, etc.
-- and set its path in the LT_LSOF_PATH environment variable.
(See 00TEST.)

Another work-around is to make sure the process that runs
the tests has the necessary permissions -- e.g., run it as
root, or enable the process login to access the resources.
For example, I can run the tests on my personal work-station
because /dev/kmem and /dev/mem are readable by the "kmem"
group and my login is in that group.


#### LTbigf test issues

##### Why does the LTbigf test say that the dialect doesn't support large files?

Large file support is defined dialect by dialect in the
lsof source files and Configure script.  If large file
support isn't defined there, it isn't defined in the LTbigf
test.

If you think that's wrong for a particular dialect, contact me
via e-mail at <abe@purdue.edu>.  Make sure "lsof" appears in the
"Subject:" line so my e-mail filter won't classify your letter
as Spam.

##### Why does LTbigf complain about operations on its config.LTbigf*
	file?

The LTbigf must be able to write a large file test (size
> 32 bits) and seek within it and the process file ulimit
size must permit the operation.  If the default location
for the test file, tests/, isn't on a file system enabled
for large file operations or if the process ulimit file
block size is too small, lsof will get file operation
errors, particularly when seeking

There may be a work-around.  Specify the path to a file
LTbigf can write in a file system enabled for large file
operations a the -poption.  Make sure that the ulimit file
block size permits writing a large file.  For example,
presuming /scratch23 is large-file-enabled, and presuming
you have permission to raise the ulimit file block size,
this shell commands will allow the LTbigf test to run on
AIX:

	$ ./LTbigf -p /scratch23/abe/bigfile

(Note: syntax for the ulimit command varies by dialect and
by shell.  Discovering the proper variant is left to the
reader.)

More information on this subject can be found in the LTbigf
description in the 00TEST file.  If course, the LTbigf.c
source file in tests/ is the ultimate source of information,

##### Why does LTbigf warn that lsof doesn't return file offsets?

On some dialects (e.g., Linux) lsof can't report file
offsets, because the data access method underlying lsof
doesn't provide them.  If LTbigf knows that lsof can't
report file offsets for the dialect, it issues this warning:

	LTbigf ... WARNING!!!  lsof can't return file offsets
		for this dialect, so offset tests have
		been disabled.

LTbigf then performs the size test and skips the offset
tests.

For more information see 00TEST and the "Why doesn't
/proc-based lsof report file offsets (positions)?" Q&A of
this file.

#### Why does the LTbasic test complain "ERROR!!! lsof this ..." and "ERROR!!!  lsof that ..."?

The LTbasic test program uses lsof to examine a running
lsof process.  It looks for the lsof current working
directory, executable (if possible), and kernel memory file
(if applicable).

Failures to find those things result in the LTbasic error
messages.  More information on how LTbasic produces the error
messages may be found in the LTbasic.c source file.

On HP-UX 11.11 and higher, for example, if the test's current
working directory is on a loopback (LOFS) file system, LTbasic
won't be able to find the current working directory of the lsof
process because of a bug in the HP-UX kernel.

The solution for that HP-UX problem is to install an HP-UX
patch.  See the answer to the "Why doesn't PSTAT-based lsof
report a CWD that is on a loopback (LOFS) file system?"
question for more information on the patch.

#### NFS test issues

##### Why does the LTnfs test complain "couldn't find NFS file ..."?

The LTnfs test must work with an NFS test file.  After it
opens the file it asks lsof to find it on an NFS file system.
If the file isn't on an NFS file system, lsof won't find it,
and the NFS test script complains and fails.

The work-around is to use -p option to supply a path to a
regular NFS file (not a directory)  that is on an NFS file
system that LTnfs can read.  Presuming /share/bin/file is
such a file and can be opened for reading by the LTnfs
test, this sample shell command could be used to run the
LTnfs test successfully:

	$ ./LTnfs -p /share/bin/file

(If the NFS file system is enabled for large files, the
NFS test will produce the error message described in the
following Q&A.)

#### LTnlink test issues

##### Why does the LTnlink test complain that its test file is on an NFS file system?

The LTnlink test may complain:

	LTnlink ... WARNING!!!  test file <path> is NFS mounted.

and then issue an explanation and a hint about using the
-p option.

The LTnlist test does this because of the way NFS file
links are managed when an NFS file is unlinked and the
unlinking process still has the file open.  Unlike with
files on a local file system, when an NFS file that is
still open is unlinked, its link count is not reduced.

The file name is changed to a name of the form .nfsxxxx
and the link count is left unchanged until the process
holding the file open closes it.  That's done by NFS so it
can keep proper track of the file on NFS clients and servers.

Since the link count isn't reduced when the LTnlink test
program closes the NFS test file it still has open, lsof
won't find it for LTnlink with a link count of zero.
Consequently, LTnlink disables that test section and issues
its warning.

The warning suggests that the unlink test section can be
run by giving LTnlink a path to a test file with the -p
option.  That path must name a file LTnlink can write and
unlink.  Presuming /scratch23/abe/nlinkfile is on a local
file system and the LTnlink test can write to it and unlink
it, this sample shell command can be used to run the complete
LTnlink test successfully:

	$ LTnlink -p /scratch23/abe/nlinkfile

##### Why does LTnlink delay and report "waiting for link count update: ..."?

On some UNIX dialects and file system combinations the
updating of link count after a file has been unlinked can
be delayed.  Consequently, lsof won't be able to report
the updated link count to LTnlink for a while.

When lsof doesn't report the proper link count to LTnlink,
it sleeps and repeats the lsof call, using the "waiting
for link count update: ..." message as a signal that it is
waiting for the expected lsof response.  The wait cycle
duration is limited to approximately one minute.

##### Why does LTnlink fail because of an unlink error?

LTnlink may fail with an error similar to:

	LTnlink ... ERROR!! unlink(<name>) failed: (Permission denied).

That message will be followed by a short explanation.

The error means that the kernel support for the file system on
which the file <name> resides does not allow a process to
unlink a file while it has the file open.  (When LTnlink is run
without the "-p path" option, it creates a <name> that begins
with "./config.LTnlink" and ends with the LTnlink process ID
number.)

An unlink failure of this type runs counter to original UNIX
file system behavior, but it has been observed on some file
system types, especially on the ZFS file system.

The work-around is to run LTnlink on a file system that allows
a process to unlink a file it has open.  Usually /tmp has that
support.  So, try running LTnlink this way:

	$ ./LTnlink -p /tmp/<name>

where <name> is a unique name in /tmp of your choosing.  To
be safe, create a subdirectory in /tmp, named by your login:

	$ rm -f /tmp/<login>
	$ mkdir /tmp/<login>
	$ ./LTnlink -p /tmp/<login>/<name>

#### LTdnlc test issues

##### Why won't the LTdnlc test run?

Lsof is unable to access the DNLC cache on AIX, because the
kernel symbols for the DNLC aren't exported.  Contact IBM
to learn why that decision was made.

The LTdnlc test won't work on Apple Darwin because lsof
can't obtain reliable DNLC information.

The LTdnlc test may fail on other dialects.  Failure causes
include: a busy system with a DNLC that is changing rapidly;
path name components too large for the DNLC; a file system
-- e.g., NFS, /tmp, loopback -- which doesn't fully
participate in the DNLC; or DNLC limitations (Many DNLC
implementations will only store path name components if
they are 31 characters or less.)

If you suspect the file system doesn't fully participate
in kernel DNLC processing, as a work-around rebuild and
test lsof on one that does.

##### What does the LTdnlc test mean by "... <path> found: 100.00%"?

Even when it succeeds the LTdnlc test will report:

	LTdnlc ... /export/home/abe/src/lsof4/tests found: 100.00%

This message means that the LTdnlc test asked lsof to find
the file at the indicated path five times and lsof found
the full path name in the indicated percentage of calls.
The LTdnlc test considers it a failure if the percentage
falls below 50.0%

##### Why does the DNLC test fail?

The DNLC test may fail when some component of the lsof
tests/ sub-directory can't be cached by the kernel DNLC.
Some kernels have a limit on the length of individual
components (typically) 32.

#### Why hasn't the test suite been qualified for 64 bit HP-UX 11 when lsof is compiled with gcc?

When I attempted to qualify lsof for HP-UX 11, compiled
with gcc 3.0, the LTsock test failed.  I traced the failure
to a gcc compilation error.  Because LTsock is an important
test, I didn't feel that the test suite was qualified if
it failed.

LTsock compiles and runs correctly on 64 bit HP-UX 11 when
compiled with HP's ANSI-C.

#### LTszoff test issues

##### Why does LTszoff warn that lsof doesn't return file offsets?

On some dialects (e.g., Linux) lsof can't report file
offsets, because the data access method underlying lsof
doesn't provide them.  If LTszoff knows that lsof can't
report file offsets for the dialect, it issues this warning:

	LTszoff ... WARNING!!!  lsof can't return file offsets
			for this dialect, so offset tests have
			been disabled.

LTszoff then performs the size test and skips the offset
tests.

For more information see 00TEST and the "Why doesn't
/proc-based lsof report file offsets (positions)?" Q&A of
this file.

#### LTlock test issues

### File descriptor list (the ``-d'' option) problems

#### Why does lsof reject a ``-d'' FD list?

Lsof rejects ``-d'' FD lists that contain both exclusions
and inclusions with messages like:

	lsof: exclude in an include list: ^1
	lsof: include in an exclude list: 2

That's because ``-d'' FD lists are processed as ORed lists,
so it makes no sense for them to contain both exclusions
and inclusions.

I.e.,, if a ``-d'' FD list were to contain ``^cwd,1'', the
``^cwd'' member is useless, because the ``1'' member
dominates by saying "include only FD 1".  That effectively
excludes ``cwd'' FD.

Note that lists may have multiple members of the same type,
exclude or include.  They are processed as an ORed set.
If an FD isn't excluded by any member of an exclude list,
it is selected.  If an FD is included by any member of an
include list, it is selected.

#### Why are file descriptors other than those in my FD list reported?

The FD list that follows ``-d'' excludes or includes file
descriptors, but unless the ``-a'' (AND) option is specified,
the FD list selections are ORed to the other selections.

For example, the following lsof command will cause all file
descriptors to be listed for the lsof command, and all but
the cwd descriptor for all other commands, probably not
what was intended.

	$ lsof -clsof -d^cwd

Hint: use ``-a'' -- e.g.,

	$ lsof -clsof -a -d^cwd

### How can I supply device numbers for inaccessible NFS file systems?

When lsof can't get device numbers for inaccessible NFS file
systems via stat(2) or lstat(2), it attempts to get them from
the mount table's dev=xxx options.  Successes are reported with
a warning message that indicates the source of the device
number and that output might be incomplete as a consequence of
the warnings.

Some system mount tables -- e.g., Linux /proc/mounts -- don't
have a dev=xxx option.  In that case, and provided lsof for the
dialect supports them, you can use the +m option to create a
mount table supplement file and the "+m m" option to use it.

First check the lsof -h (help) output to see if the +m and
"+m m" options are supported.  If they are, use +m to create a
mount table supplement file when all mounted file systems are
accessible.  Use "+m m" later to make the supplement available
when some mounted file systems might not be available.

Here's an example that creates a mount supplement file in
$HOME/mnt-sup and later makes it available to lsof.

	$ rm -f $HOME/mnt-sup
	$ lsof +m > $HOME/mnt-sup
	...
	$ lsof +m $HOME/mnt-sup <other lsof options>

If lsof has to get the device number from the supplement, it
will issue an informative warning message.  The warning can be
suppressed with lsof's -w option.

Caution!  Since the mount table supplement file is static, it
is its supplier's responsibility to update it as file system
mounts change.

For more information, consult the lsof man page.  The
"ALTERNATE DEVICE NUMBERS" section has useful information on
how lsof acquires device numbers when stat(2) or lstat(2)
fail.

### Why won't lsof find open files on over-mounted file systems?

When a file system, /xyz for example, is mounted on the same
mount point as another file system, /abc for example, running
lsof with an argument of the path of the first file system's
mount point -- the over-mounted one, /abc -- probably will not
reveal any files open on /abc.

That's because lsof looks for open files on a file system by
looking for files with the file system's device number.  The
two file systems usually have different device numbers and lsof
determines the device number search key from the supplied name
of the second file system.

A general work-around exists only for Linux.  On that UNIX
dialect, when you know the over-mounted file system's mount
point path, you can ask lsof to report on all open files and
grep that output for the path of the over-mounted file system
mount point.

### What can be done when lsof reports no more space?

Many lsof methods cache information in memory, using the
dialects malloc() library function.  When malloc() can't
allocate the requested amount of memory, lsof exits with
warning messages similar to this AIX message:

	lsof: no more dev-ch space at pid 2257750: 0x82a8e600

Lsof then exits immediately and produces no more output.

A possible work-around is to increase the memory foot print
of the shell that runs lsof.  That is often done with the
ulimit(1) shell command.

### What if the lsof build encounters ar and ld problems?

The lsof main and library Makefiles use the library archiver,
ar, and the system loader, ld, applications.  Improperly
located, installed or configured versions of them may cause the
lsof build to encounter errors with them.

The application producing the error should identify itself in
its error messages.

The first thing to check the path of the application that is
being used.  Try `which ar` or `which ld` to see if perhaps the
PATH used during the build might be causing the wrong archiver
or loader to be used.

If the problem is with the use of the wrong archiver, and it's
not possible to correct the PATH to it, try using the LSOF_AR
environment variable to specify the path to and arguments for
the correct archiver.  See 00XCONFIG for more information and
note that LSOF_AR must specify the path to the archive
application and the arguments for it, less the terminating
library and module name arguments.

If the problem is with the loader, there is no lsof work-
around.  That's because lsof calls the loader via the C
compiler, so the problem must be fixed at the compiler (system)
level.

### Why does lsof -i report an open socket file for a process, but
lsof -p on that process' ID report nothing?

The lsof in use was probably built with the HASSECURITY and
HASNOSOCKSECURITY options and the process in question does not
belong to the user of lsof.

The HASSECURITY option limits lsof output to processes owned
by the user invoking lsof and the HASNOSOCKSECURITY option
weakens that slightly to allow output of open socket file
information for all processes.

For example, if process PID 12345 is owned by some user other
than the one invoking lsof, and lsof has been compiled with the
HASSECURITY and HASNOSOCKSECURITY options, the following lsof
command will display the open socket files of process 12345:

	$ lsof -p 12345 -a -i

This security restriction is described in the lsof(8) manual
page.
