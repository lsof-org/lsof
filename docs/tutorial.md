# Tutorial

## Finding Uses of a Specific Open File

Often you're interested in knowing who is using a specific file.
You know the path to it and you want lsof to tell you the processes
that have open references to it.

Simple -- execute lsof and give it the path name of the file of interest --
e.g.,

```shell
$ lsof /etc/passwd
```

Caveat: this only works if lsof has permission to get the status (via stat(2))
of the file at the named path.  Unless the lsof process has enough authority  --
e.g., it is being run with a real User ID (UID) of root.

Further caveat: this use of lsof will fail if the stat(2) kernel syscall returns
different file parameters -- particularly device and inode numbers -- than lsof
finds in kernel node structures.  This condition is rare and is usually
documented in the FAQ.

```shell
$ lsof /etc/security/passwd
lsof: status error on /etc/security/passwd: Permission denied
```

## Finding Open Files Filling a File System

Oh! Oh!  /tmp is filling and ls doesn't show that any large files are being
created.  Can lsof help?

Maybe.  If there's a process that is writing to a file that has been unlinked,
lsof may be able to discover the process for you.  You ask it to list all open
files on the file system where /tmp is located.

Sometimes /tmp is a file system by itself.  In that case,

```shell
$ lsof /tmp
```

is the appropriate command.  If, however, /tmp is part of another file system,
typically /, then you may have to ask lsof to list all files open on the
containing file system and locate the offending file and its process by
inspection -- e.g.,

```shell
$ lsof / | more
# or
$ lsof / | grep ...
```

Caveat: there must be a file open to a for the lsof search to succeed.
Sometimes the kernel may cause a file reference to persist, even where there's
no file open to a process.  (Can you say kernel bug?  Maybe.)  In any event,
lsof won't be able to help in this case.

## Finding an Unlinked Open File

A pesky variant of a file that is filling a file system is an unlinked file to
which some process is still writing.  When a process opens a file and then
unlinks it, the file's resources remain in use by the process, but the file's
directory entries are removed.  Hence, even when you know the directory where
the file once resided, you can't detect it with ls.

This can be an administrative problem when the unlinked file is large, and the
process that holds it open continues to write to it.  Only when the process
closes the file will its resources, particularly disk space, be released.

Lsof can help you find unlinked files on local disks.  It has an option, +L,
that will list the link counts of open files.  That helps because an unlinked
file on a local disk has a zero link count.  Note: this is NOT true for NFS
files, accessed from a remote server.

You could use the option to list all files and look for a zero link count in the
NLINK column -- e.g.,

```shell
$ lsof +L
COMMAND   PID USER   FD  TYPE DEVICE SIZE/OFF NLINK  NODE NAME
...
less    25366  abe  txt  VREG    6,0    40960     1 76319 /usr/...
...
> less    25366  abe    3r VREG    6,0    17360     0 98768 / (/dev/sd0a)
```

Better yet, you can specify an upper bound to the +L option, and
lsof will select only files that have a link count less than the
upper bound.  For example:

```shell
$ lsof +L1
COMMAND   PID USER   FD   TYPE DEVICE SIZE/OFF NLINK  NODE NAME
less    25366  abe    3r  VREG    6,0    17360     0 98768 / (/dev/sd0a)
```

You can use lsof's -a (AND) option to narrow the link count search
to a particular file system.  For example, to look for zero link
counts on the /home file system, use:

```shell
$ lsof -a +L1 /home
```

CAUTION: lsof can't always report link counts for all file types
-- e.g., it may not report them for FIFOs, pipes, or sockets.
Remember also that link counts for NFS files on an NFS client
host don't behave as do link counts for files on local disks.

## Finding Processes Blocking Umount

When you need to unmount a file system with the umount command, you may find the
operation blocked by a process that has a file open on the file systems.  Lsof
may be able to help you find the process.  In response to:

```shell
$ lsof <file_system_name>
```

Lsof will display all open files on the named file system.  It will also set its
exit code zero when it finds some open files and non-zero when it doesn't,
making this type of lsof call useful in shell scripts.  (See section 16.)

Consult the output of the df command for file system names.

See the caveat in the preceding section about file references that persist in
the kernel without open file traces.  That situation may hamper lsof's ability
to help with umount, too.

## Finding Listening Sockets

Sooner or later you may wonder if someone has installed a network
server that you don't know about.  Lsof can list for you all the
network socket files open on your machine with:

```shell
$ lsof -i
```

The -i option without further qualification lists all open Internet socket
files.  You can add network names or addresses, protocol names, and service
names or port numbers to the -i option to refine the search.  (See the next
section.)

## Finding a Particular Network Connection

When you know the source or destination of a network connection whose open files
and process you'd like to identify, the -i option may help.

If, for example, you want to know what process has a connection open to or from
the Internet host named aaa.bbb.ccc, you can ask lsof to search for it with:

```shell
$ lsof -i@aaa.bbb.ccc
```

If you're interested in a particular protocol -- TCP or UDP --
and a specific port number or service name, you can add those
discriminators to the -i information:

```shell
$ lsof -iTCP@aaa.bbb.ccc:ftp-data
```

If you're interested in a particular IP version -- IPv4 or IPv6
-- and your UNIX dialect supports both (It does if "IPv[46]"
appears in the lsof -h output.), you can add the '4' or '6'
selector immediately after -i:

```shell
$ lsof -i4
$ lsof -i6
```

## Identifying a Netstat Connection

How do I identify the process that has a network connection
described in netstat output?  For example, if netstat says:

```
Proto Recv-Q Send-Q  Local Address          Foreign Address        (state)
tcp        0      0  vic.1023               ipscgate.login         ESTABLISHED
```

What process is connected to service name `login` on ipscgate?

Use lsof's -i option:

```
$ lsof -iTCP@ipscgate:login
COMMAND     PID     USER   FD   TYPE     DEVICE   SIZE/OFF  INODE NAME
rlogin    25023      abe    3u  inet 0x10144168      0t184    TCP lsof.itap.purdue.edu:1023->ipscgate.cc.purdue.edu:login
...
```

There's another way.  Notice the 0x10144168 in the DEVICE column
of the lsof output?  That's the protocol control block (PCB)
address.  Many netstat applications will display it when given
the -A option:

```shell
$ netstat -A
PCB      Proto Recv-Q Send-Q  Local Address      Foreign Address    (state)
10144168 tcp        0      0  vic.1023           ipscgate.login     ESTABLISHED
...
```

Using the PCB address, lsof, and grep, you can find the process this
way, too:

```shell
$ lsof -i | grep 10144168
rlogin    25023      abe    3u  inet 0x10144168      0t184    TCP lsof.itap.purdue.edu:1023->ipscgate.cc.purdue.edu:login
...
```

If the file is a UNIX socket and netstat reveals and address for it,
like this Solaris 11 example:

```shell
$ netstat -a -f unix
Active UNIX domain sockets
Address  Type          Vnode     Conn  Local Addr      Remote Addr
ffffff0084253b68 stream-ord 0000000 0000000
```

Using lsof's -U option and its output piped to a grep on the address
yields:

```shell
$ lsof -U | grep ffffff0084253b68
squid 1638 nobody 12u unix 18,98 0t10 9437188 /devices/pseudo/tl@0:ticots->0xffffff0084253b68 stream-ord
```

## Finding Files Open to a Named Command

When you want to look at the files open to a particular command,
you can look up the PID of the process running the command and
use lsof's -p option to specify it.

```shell
$ lsof -p <PID>
```

However, there's a quicker way, using lsof's -c option, provided
you don't mind seeing output for every process running the named
command.

```shell
$ lsof -c <first_characters_of_command_name_that_interest_you>
```

The lsof -c option is useful when you want to see how many instances
of a given command are executing and what their open files are.
One useful example is for the sendmail command.

```shell
$ lsof -c sendmail
```

## Deciphering the Remote Login Trail

If the network connection you're interested in tracing has been initiated
externally and is connected to an rlogind, sshd, or telnetd process, asking lsof
to identify that process might not give a wholly satisfying answer.  The report
may be that the connection exists, but to a process owned by root.

### The Fundamentals

How do you get from there to the login name really using the connection?  You
have to know a little about how real and pseudo ttys are paired in your system,
and then use several lsof probes to identify the login.

This example comes from a Solaris 2.4 system, named klaatu.cc.  I've logged on
to it via rlogin from lsof.itap.  The first lsof probe,

```shell
$ lsof -i@lsof.itap
```

yields (among other things):

```shell
COMMAND     PID     USER   FD   TYPE     DEVICE   SIZE/OFF  INODE NAME
in.rlogin  7362     root    0u  inet 0xfc0193b0      0t242    TCP klaatu.cc.purdue.edu:login->lsof.itap.purdue.edu:1023
...
```

This confirms that a connection exists.  A second lsof probe
shows:

```shell
$ lsof -p7362
COMMAND     PID     USER   FD   TYPE     DEVICE   SIZE/OFF  INODE NAME
...
in.rlogin  7362     root    0u  inet 0xfc0193b0      0t242    TCP klaatu.cc.purdue.edu:login->lsof.itap.purdue.edu:1023
...
in.rlogin  7362     root    3u  VCHR    23,   0       0t66  52928 /devices/pseudo/clone@0:ptmx->pckt->ptm
```

7362 is the Process ID (PID) of the in.rlogin process, discovered
in the first lsof probe.  (I've abbreviated the output to simplify
the example.)  Now comes a need to understand Solaris pseudo-ttys.
The key indicator is in the DEVICE column for FD 3, the major/minor
device number of 23,0.  This translates to /dev/pts/0, so a third
lsof probe,

```shell
$ lsof /dev/pts/0
COMMAND     PID     USER   FD   TYPE     DEVICE   SIZE/OFF  INODE NAME
ksh        7364      abe    0u  VCHR    24,   0     0t2410  53410 /dev/pts/../../devices/pseudo/pts@0:0
```

shows in part that login abe has a ksh process on /dev/pts/0.
(The NAME that lsof shows is not /dev/pts/0 but the full expansion
of the symbolic link that lsof finds at /dev/pts/0.)

Here's a second example, done on an HP-UX 9.01 host named ghg.ecn.
Again, I've logged on to it from lsof.itap, so I start with:

```shell
$ lsof -i@lsof.itap
COMMAND     PID     USER   FD   TYPE       DEVICE   SIZE/OFF  INODE NAME
rlogind   10214     root    0u  inet   0x041d5f00     0t1536    TCP ghg.ecn.purdue.edu:login->lsof.itap.purdue.edu:1023
...
```

Then,

```shell
$ lsof -p10214
COMMAND     PID     USER   FD   TYPE       DEVICE   SIZE/OFF  INODE NAME
...
rlogind   10214     root    0u  inet   0x041d5f00     0t2005    TCP ghg.ecn.purdue.edu:login->lsof.itap.purdue.edu:1023
...
rlogind   10214     root    3u  VCHR  16,0x000030     0t2037  24642 /dev/ptym/ptys0
```

Here the key is the NAME /dev/ptym/ptys0.  In HP-UX 9.01 tty and
pseudo tty devices are paired with the names like /dev/ptym/ptys0
and /dev/pty/ttys0, so the following lsof probe is the final step.

```shell
$ lsof /dev/pty/ttys0
COMMAND     PID     USER   FD   TYPE       DEVICE   SIZE/OFF  INODE NAME
ksh       10215      abe    0u  VCHR  17,0x000030     0t3399  22607 /dev/pty/ttys0
...
```

Here's a third example for an AIX 4.1.4 system.  I've used telnet
to connect to it from lsof.itap.purdue.edu.  I start with:

```shell
$ lsof -i@lsof.itap.purdue.edu
COMMAND     PID     USER   FD   TYPE     DEVICE   SIZE/OFF      INODE NAME
...
telnetd   15616     root    0u  inet 0x05a93400     0t5156        TCP cloud.cc.purdue.edu:telnet->lsof.itap.purdue.edu:3369
```

Then I look at the telnetd process:

```shell
$ lsof -p15616
COMMAND     PID     USER   FD   TYPE     DEVICE   SIZE/OFF      INODE NAME
...
telnetd   15616     root    0u  inet 0x05a93400     0t5641        TCP cloud.cc.purdue.edu:telnet->lsof.itap.purdue.edu:3369
...
telnetd   15616     root    3u  VCHR    25,   0     0t5493        103 /dev/ptc/0
```

Here the key is /dev/ptc/0.  In AIX it's paired with /dev/pts/0.
The last probe for that shows:

```shell
$ lsof /dev/pts/0
COMMAND     PID     USER   FD   TYPE     DEVICE   SIZE/OFF      INODE NAME
...
ksh       16642      abe    0u  VCHR    26,   0     0t6461        360 /dev/pts/0
```

### The idrlogin.perl[5] Scripts

There's another, perhaps easier way, to go about the job of
tracing a network connection.  The lsof distribution contains
two Perl scripts, idrlogin.perl (Perl 4) and idrlogin.perl5
(Perl 5), that use lsof field output to display values for
shells that are parented by rlogind, sshd, or telnetd, or
connected directly to TCP sockets.  The lsof test suite contains
a C library that can be adapted for use with C programs that
need to call lsof and process its field output.

The two Perl scripts use the lsof -R option; it causes the
paRent process ID (PPID) to be listed in the lsof output.  The
scripts identify all shell processes -- e.g., ones whose command
names end in ``sh'' -- and determine if: 1) the ultimate ancestor
process before a PID greater than 2 (e.g., init's PID is 1) is
rlogind, sshd, or telnetd; or 2) the shell process has open
TCP socket files.

Here's an example of output from idlogin.perl on a Solaris 2.4
system:

```shell
centurion: 1 = cd src/lsof4/scripts
centurion: 2 = ./idrlogin.perl
Login    Shell       PID Via           PID TTY        From
oboyle   ksh       12640 in.telnetd  12638 pts/5      opal.cc.purdue.edu
icdtest  ksh       15158 in.rlogind  15155 pts/6      localhost
sh       csh       18207 in.rlogind  18205 pts/1      babylon5.cc.purdue.edu
root     csh       18242 in.rlogind  18205 pts/1      babylon5.cc.purdue.edu
trouble  ksh       19208 in.rlogind  18205 pts/1      babylon5.cc.purdue.edu
abe      ksh       21334 in.rlogind  21332 pts/2      lsof.itap.purdue.edu
```

The scripts assume that its parent directory contains an
executable lsof.  If you decide to use one of the scripts, you
may want to customize it for your local lsof and perl paths.

Note that processes executing as remote shells are also
identified.

Here's another example from a UnixWare 7.1.0 system.

```shell
tweeker: 1 = cd src/lsof4/scripts
tweeker: 9 = ./idrlogin.perl
Login    Shell       PID Via           PID TTY        From
abe      ksh        9438 in.telnetd   9436 pts/3      lsof.itap.purdue.edu
```

## Watching an Ftp or Rcp Transfer

The nature of the Internet being one of unpredictable performance
at times, occasionally you want to know if a file transfer, being
done by ftp or rcp, is making any progress.

To use lsof for watching a file transfer, you need to know the
PID of the file transfer process.  You can use ps to find that.
Then use lsof,

```shell
$ lsof -p<PID>
```

to examine the files open to the transfer process.  Usually the
ftp files or interest are at file descriptors 9 and 10 or 10 and
11; for rcp, 3 and 4.  They describe the network socket file and
the local data file.

If you want to watch only those file descriptors as the file
transfer progresses, try these lsof forms (for ftp in the example):

```shell
$ lsof -p<PID> -ad9,10 -r
# or
$ lsof -p<PID> -ad10,11 -r
```

Some options need explaining:

- -p<PID>	specifies that lsof is to restrict its attention
	to the process whose ID is <PID>.  You can specify
	a set of PIDs by separating them with commas.

		$ lsof -p 1234,5678,9012

- -a		specifies that lsof is to AND its tests together.
	The two tests that are specified are tests on the
	PID and tests on file descriptions (`d9,10`).

- d9,10	specifies that lsof is to test only file descriptors
	9 and 10.  Note that the `-` is absent, since `-a`
	is a unary option and can be followed immediately
	by another lsof option.

- -r          tells lsof to list the requested open file information,
	sleep for a default 15 seconds, then list the open
	file information again.  You can specify a different
	time (in seconds) after -r and override the default.
	Lsof issues a short line of equal signs between
	each set of output to distinguish it.

For an rcp transfer, the above example becomes:

```shell
$ lsof -p<PID> -ad3,4 -r
```

## Listing Open NFS Files

Lsof will list all files open on remote file systems, supported
by an NFS server.  Just use:

```shell
$ lsof -N
```

Note, however, that when run on an NFS server, lsof will not list
files open to the server from one of its clients.  That's because
lsof can only examine the processes running on the machine where
it is called -- i.e., on the NFS server.

If you run lsof on the NFS client, using the -N option, it will
list files open by processes on the client that are on remote
NFS file systems.

## Listing Files Open by a Specific Login

If you're interested in knowing what files the processes owned
by a particular login name have open, lsof can help.

```shell
$ lsof -u<login>
# or
$ lsof -u<User ID number>
```

You can specify either the login name or the UID associated with
it.  You can specify multiple login names and UID numbers, mixed
together, by separating them with commas.

```shell
$ lsof -u548,abe
```

On the subject of login names and UIDs, it's worth noting that
lsof can be told to report either.  By default it reports login
names; the -l option switches reporting to UIDs.  You might want
to use -l if login name lookup is slow for some reason.

### Ignoring a Specific Login

The -u option can also be used to direct lsof to ignore a
specific login name or UID, or a list of them.  Simply prefix
the login names or UIDs with a `^` character, as you might do
in a regular expression.  The `^` prefix is useful, for example,
when you want to have lsof ignore the files open to system
processes, owned by the root (UID 0) login.  Try:

```shell
$ lsof -u ^root
# or
$ lsof -u ^0
```

## Listing Files Open to a Specific Process Group

There's a Unix collection of processes called a process group.
The name indicates that the processes of the group have a common
association and are grouped so that a signal sent to one (e.g.,
a keyboard kill stroke) is delivered to all.

This causes Unix to create a two element process group:

```shell
$ lsof | less
```

You can use lsof to look at the open files of all members of a
process group, if you know the process group ID number.  Assuming
that it is 12717 for the above example, this lsof command:

```shell
$ lsof -g12717 -adcwd
```

would produce on a Solaris 8 system:

```shell
$ lsof -g12717 -adcwd
COMMAND   PID  PGID USER  FD TYPE DEVICE SIZE/OFF    NODE NAME
sshd    11369 12717 root cwd VDIR    0,2      189 1449175 /tmp (swap)
sshd    12717 12717 root cwd VDIR  136,0     1024       2 /
```

The `-g12717'` option specifies the process group ID of interest;
the `-adcwd` option specifies that options are to be ANDed and
that lsof should limit file output to information about current
working directory (`cwd`) files.


## Output for Other Programs

The -F option allows you to specify that lsof should describe
open files with a special form of output, called field output,
that can be parsed easily by a subsequent program.  The lsof
distribution comes with sample AWK, Perl 4, and Perl 5 scripts
that post-process field output.  The lsof test suite has a C
library that could be adapted for use by C programs that want to
process lsof field output from an in-bound pipe.

The lsof manual page describes field output in detail in its
OUTPUT FOR OTHER PROGRAMS section.  A quick look at a sample
script in the scripts/ subdirectory of the lsof distribution will
also give you an idea how field output works.

The most important thing about field output is that it is relatively
homogeneous across Unix dialects.  Thus, if you write a script
to post-process field output for AIX, it probably will work for
HP-UX, Solaris, and Ultrix as well.

Support for other formats e.g. JSON is planned.

## The Lsof Exit Code and Shell Scripts

When lsof executes successfully, it returns an exit code based on
the result of its search for specified files.  (If no files were
specified, then the successful exit code is 0 (zero).)

If lsof was asked to search for specific files, including any
files on specified file systems, it returns an exit code of 0
(zero) if it found all the specified files and at least one file
on each specified file system.  Otherwise it returns a 1 (one) if
any part of the search failed.

This behavior can be modified by calling lsof with -Q, which will
tell it to provide a successful exit code of 0 (zero) even if any
part of the file or filesystem search failed.

If lsof detects a generic (non-search) error during its execution,
it returns an exit code of 1 (one).  The -Q option will not
affect this behavior.

You can use the exit code in a shell script to search for files
on a file system and take action based on the result -- e.g.,

```shell
#!/bin/sh
lsof <file_system_name> > /dev/null 2>&1
if test $? -eq 0
then
echo "<file_system_name> has some users."
else
echo "<file_system_name> may have no users."
fi
```

The -Q option can help in certain circumstances.  For example, if
you want to log filesystem users without caring if there are no
users:

```shell
#!/bin/sh
lsof -Q <file_system_name>  > fs_users.log
if test $? -ne 0
then
echo "Error: Something actually went wrong!" 1>&2
exit 1
fi
```

## Strange messages in the NAME column

When lsof encounters problems analyzing a particular file, it may
put a message in the file's NAME column.  Many of those messages
are explained in the 00FAQ file of the lsof distribution.

So consult 00FAQ first if you encounter a NAME column message you
don't understand.  (00FAQ is a possible source of information
about other unfamiliar things in lsof output, too.)

If you can't find help in 00FAQ, you can use grep to look in the
lsof source files for the message -- e.g.,

```shell
$ cd .../lsof_4.76_src
$ grep "can't identify protocol" *.[ch]
```

The code associated with the message will usually make clear the
reason for the message.

If you have an lsof source tree that has been processed by the
lsof Configure script, you need grep only there.  If, however,
your source tree hasn't been processed by Configure, you may
have to look in the top-level lsof source directory and in the
dialects sub-directory for the UNIX dialect you are using - e.g.,

```shell
$ cd .../lsof_4.76_src
$ grep "can't identify protocol" *.[ch]
$ cd dialects/Linux
$ grep "can't identify protocol" *.[ch]
```

In rare cases you may have to look in the lsof library, too --
e.g.,

```
$ cd .../lsof_4.76_src
$ grep "can't identify protocol" *.[ch]
$ cd dialects/Linux
$ grep "can't identify protocol" *.[ch]
$ cd ../../lib
$ grep "can't identify protocol" *.[ch]
```
