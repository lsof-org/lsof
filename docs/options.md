# Options

The document describes the lsof options in detail.

## Selection Options

Lsof has a rich set of options for selecting the files to be
displayed.  These include:

- -a	tells lsof to AND the set of selection options that
	are specified.  Normally lsof ORs them.

	For example, if you specify the -p<PID> and -u<UID>
	options, lsof will display all files for the
	specified PID or for the specified UID.

	By adding -a, you specify that the listed files
	should be limited to PIDs owned by the specified
	UIDs -- i.e., they match the PIDs *and* the UIDs.

		$ lsof -p1234 -au 5678

- -c	specifies that lsof should list files belonging
	to processes having the associated command name.

	Hint: if you want to select files based on more than
	one command name, use multiple -c<name> specifications.

		$ lsof -clsof -cksh

- -d      tells lsof to select by the associated file descriptor
	(FD) set.  An FD set is a comma-separated list of
	numbers and the names lsof normally displays in
	its FD column:  cwd, Lnn, ltx, <number>, etc.  See
	the OUTPUT section of the lsof man page for the
	complete list of possible file descriptors.  Example:

		$ lsof -dcwd,0,1,2

- -g      tells lsof to select by the associated process
	group ID (PGID) set.  The PGID set is a comma-separated
	list of PGID numbers.  When -g is specified, it also
	enables the display of PGID numbers.

	Note: when -g isn't followed by a PGID set, it
	simply selects the listing of PGID for all processes.
	Examples:

		$ lsof -g
		$ lsof -g1234,5678

- -i	tells lsof to display Internet socket files.  If no
	protocol/address/port specification follows -i,
	lsof lists all Internet socket files.

	If a specification follows -i, lsof lists only the
	socket files whose Internet addresses match the
	specification.

	Hint: multiple addresses may be specified with
	multiple -i options.  Examples:

		$ lsof -iTCP
		$ lsof -i@lsof.itap.purdue.edu:sendmail

- -N	selects the listing of files mounted on NFS devices.

- -U	selects the listing of socket files in the Unix
	domain.


## Output Options

Lsof has these options to control its output format:

- -F	produce output that can be parsed by a subsequent
	program.

- -g	print process group (PGID) IDs.

- -l	list UID numbers instead of login names.

- -n	list network numbers instead of host names.

- -o	always list file offset.

- -P	list port numbers instead of port service names.

- -s	always list file size.

## Precautionary Options

Lsof uses system functions that can block or take a long time,
depending on the health of the Unix dialect supporting it.  These
include:

- -b	directs lsof to avoid system functions -- e.g.,
	lstat(2), readlink(2), stat(2) -- that might block
	in the kernel.  See the BLOCKS AND TIMEOUTS
	section of the lsof man page.

	You might want to use this option when you have
	a mount from an NFS server that is not responding.

- -C	tells lsof to ignore the kernel's name cache.  As
	a precaution this option will have little effect on
	lsof performance, but might be useful if the kernel's
	name cache is scrambled.  (I've never seen that
	happen.)

- -D	might be used to direct lsof to ignore an existing
	device cache file and generate a new one from /dev
	(and /devices).  This might be useful if you have
	doubts about the integrity of an existing device
	cache file.

- -l      tells lsof to list UID numbers instead of login
	names -- this is useful when UID to login name
	conversion is slow or inoperative.

- -n	tells lsof to avoid converting Internet addresses
	to host numbers.  This might be useful when your
	host name lookup (e.g., DNS) is inoperative.

- -O      tells lsof to avoid its strategy of forking to
	perform potentially blocking kernel operations.
	While the forking allows lsof to detect that a
	block has occurred (and possibly break it), the
	fork operation is a costly one.  Use the -O option
	with care, lest your lsof be blocked.

- -P      directs lsof to list port numbers instead of trying
	to convert them to port service names.  This might
	be useful if port to service name lookups (e.g.,
	via NIS) are slow or failing.

- -S      can be used to change the lstat/readlink/stat
	timeout interval that governs how long lsof waits
	for response from the kernel.  This might be useful
	when an NFS server is slow or unresponsive.  When
	lsof times out of a kernel function, it may have
	less information to display.  Example:

		$ lsof -S2

- -w	tells lsof to avoid issuing warning messages, if
	they are enabled by default, or enable them if they
	are disabled by default.  Check the -h (help) output
	to determine their status.  If it says `-w enable warnings`, then warning messages are disabled by
	default; `-w disable warnings`, they are enabled
	by default.

	This may be a useful option, for example, when you
	specify -b, if warning messages are enabled, because
	it will suppress the warning messages lsof issues
	about avoiding functions that might block in the
	kernel.

## Miscellaneous Lsof Options

There are some lsof options that are hard to classify, including:

- -?, -h	these options select help output.

- -F      selects field output.  Field output is a mode where
	lsof produces output that can be parsed easily by
	subsequent programs -- e.g., AWK or Perl scripts.
	See ``15. Output for Other Programs'' for more
	information.

- -k	specifies an alternate kernel symbol file -- i.e.,
	where nlist() will get its information.  Example:

		$ lsof -k/usr/crash/vmunix.1

- -m	specifies an alternate kernel memory file from
	which lsof will read kernel structures in place
	of /dev/kmem or kvm_read().  Example:

		$ lsof -m/usr/crash/vmcore.n

- -r	tells lsof to repeat its scan every 15 seconds (the
	default when no associated value is specified).  A
	repeat time, different from the default, can follow
	-r.  Example:

		$ lsof -r30

- -Q	tells lsof not to consider it an error if it was
	given search terms and any part of the search came
	up empty. This will silence any reports of missing
	files to stderr. Additionally, lsof will exit with
	a non-error code despite any missing files or
	filesystems with no open files.

- -v	displays information about the building of the
	lsof executable.

- --      The double minus sign option may be used to
	signal the end of options.  It's particularly useful
	when arguments to the last option are optional and
	you want to supply a file path that could be confused
	for arguments to the last option.  Example:

		$ lsof -g -- 1

	Where `1' is a file path, not PGID ID 1.
