# When Lsof Seems to Hang

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