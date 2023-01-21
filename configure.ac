AC_INIT([lsof],[4.97.0])
AM_INIT_AUTOMAKE([subdir-objects])

# Require C compiler
AC_PROG_CC
# For legacy automake on e.g. CentOS 7
AM_PROG_CC_C_O

# Detect OS
AC_CANONICAL_HOST

# Pass OS version
LSOF_VSTR=$(uname -r)
LSOF_TMP=$(echo $LSOF_VSTR | sed 's/(/\\\\(/g' | sed 's/)/\\\\)/g')
CFLAGS="$CFLAGS -DLSOF_VSTR=\\\"$LSOF_TMP\\\""

LSOF_DIALECT=unknown
LSOF_DIALECT_DIR=unknown
LSOF_TGT=unknown
case "${host_os}" in
	linux*)
		LSOF_DIALECT=linux
		LSOF_DIALECT_DIR=linux
		LSOF_TGT=linux

		# Compute version
		LSOF_VERS=`echo $LSOF_VSTR | sed 's/\./ /g' | awk '{printf "%d%d%03d",\$1,\$2,\$3}'`

		# Always use large file
		CFLAGS="$CFLAGS -D_FILE_OFFSET_BITS=64"

		# Enable LTbigf test
		LSOF_TEST_CFLAGS="$LSOF_TEST_CFLAGS -DLT_BIGF -D_FILE_OFFSET_BITS=64"
		;;
	darwin*)
		LSOF_DIALECT=darwin
		LSOF_DIALECT_DIR=darwin/libproc
		LSOF_TGT=darwin

		case $LSOF_VSTR in
			7.*)			# Mac OS X 10.3 (Panther)
				LSOF_VERS=700
				;;
			8.*)			# Mac OS X 10.4 (Tiger)
				LSOF_VERS=800
				;;
			9.*)			# Mac OS X 10.5 (Leopard)
				LSOF_VERS=900
				;;
			10.*)			# Mac OS X 10.6 (SnowLeopard)
				LSOF_VERS=1000
				;;
			11.*)			# Mac OS X 10.7 (Lion)
				LSOF_VERS=1100
				;;
			12.*)			# Mac OS X 10.8 (Mountain Lion)
				LSOF_VERS=1200
				;;
			13.*)			# Mac OS X 10.9 (Mavericks)
				LSOF_VERS=1300
				;;
			14.*)			# Mac OS X 10.10 (Yosemite)
				LSOF_VERS=1400
				;;
			15.*)			# Mac OS X 10.11 (El Capitan)
				LSOF_VERS=1500
				;;
			16.*)			# macOS 10.12 (Sierra)
				LSOF_VERS=1600
				;;
			17.*)			# macOS 10.13 (High Sierra)
				LSOF_VERS=1700
				;;
			18.*)			# macOS 10.14 (Mojave)
				LSOF_VERS=1800
				;;
			19.*)			# macOS 10.15 (Catalina)
				LSOF_VERS=1900
				;;
			*)
				AC_MSG_NOTICE([Unknown Darwin release: $(uname -r)])
				AC_MSG_NOTICE([Assuming Darwin 19.0])
				LSOF_VERS=1900
				;;
		esac

		# Add DARWINV to cflags
		CFLAGS="$CFLAGS -DDARWINV=$LSOF_VERS"

		# Enable LTbigf test
		LSOF_TEST_CFLAGS="$LSOF_TEST_CFLAGS -DLT_BIGF"
		;;
	freebsd*)
		LSOF_DIALECT=freebsd
		LSOF_DIALECT_DIR=freebsd
		LSOF_TGT=freebsd

		case $LSOF_VSTR in
			10*)
				LSOF_VERS=10000
				;;
			11*)
				LSOF_VERS=11000
				;;
			12*)
				LSOF_VERS=12000
				;;
			13*)
				LSOF_VERS=13000
				;;
			14*)
				LSOF_VERS=14000
				;;
			*)
				AC_MSG_ERROR([Unknown FreeBSD release: $(uname -r)])
				;;
		esac

		# Add FREEBSDV to cflags
		CFLAGS="$CFLAGS -DFREEBSDV=$LSOF_VERS"

		# Always define HASPROCFS HAS_DUP2 HAS_CLOSEFROM
		CFLAGS="$CFLAGS -DHASPROCFS -DHAS_DUP2 -DHAS_CLOSEFROM"

		# Always link libutil
		LDFLAGS="$LDFLAGS -lutil"

		# Detect FreeBSD source code
		if test -d /usr/src/sys	# {
		then
			FREEBSD_SYS=/usr/src/sys
		else
			if test -d /sys	# {
			then
				FREEBSD_SYS=/sys
			else
				AC_MSG_WARN([No kernel sources in /usr/src/sys or /sys])
			fi	# }
		fi	# }

		# Enable LTbigf test
		LSOF_TEST_CFLAGS="$LSOF_TEST_CFLAGS -DLT_BIGF"
		;;
	*)
		AC_MSG_ERROR(["Host $host_os not supported"])
esac

# Pass dialect info to Makefile.am
AM_CONDITIONAL([LINUX], [test x$LSOF_DIALECT_DIR = xlinux])
AM_CONDITIONAL([DARWIN_LIBPROC], [test x$LSOF_DIALECT_DIR = xdarwin/libproc])
AM_CONDITIONAL([FREEBSD], [test x$LSOF_DIALECT_DIR = xfreebsd])

# Pass LSOF_DIALECT/LSOF_DIALECT_DIR to Makefile.am
AC_SUBST([LSOF_DIALECT])
AC_SUBST([LSOF_DIALECT_DIR])

# --enable-security to define HASSECURITY
AC_ARG_ENABLE(security, AS_HELP_STRING([--enable-security],
	[allow only the root user to list all open files @<:@default=no@:>@]), [], [enable_security=no])
AS_IF([test "x$enable_security" = xyes], [
	CFLAGS="$CFLAGS -DHASSECURITY"
])

# --enable-no-sock-security to define HASNOSOCKSECURITY
AC_ARG_ENABLE(no_sock_security, AS_HELP_STRING([--enable-no-sock-security],
	[combined with --enable-security, allow anyone to list anyone else's socket files @<:@default=no@:>@]), [], [enable_no_sock_security=no])
AS_IF([test "x$enable_no_sock_security" = xyes], [
	AS_IF([test "x$enable_security" = xyes], [
		CFLAGS="$CFLAGS -DHASNOSOCKSECURITY"
	], [
		AC_MSG_ERROR([--enable-no-sock-security must be used with --enable-security])
	])
])

# Check rpc/rpc.h or libtirpc
# with_libtirpc=yes/no/auto
AC_ARG_WITH(libtirpc, AS_HELP_STRING([--with-libtirpc],
	[build with libtirpc support @<:@default=auto@:>@]), [], [with_libtirpc=auto])
AS_IF([test "x$with_libtirpc" != xno], [
	PKG_CHECK_MODULES([LIBTIRPC], [libtirpc], [
		CFLAGS="$CFLAGS $LIBTIRPC_CFLAGS"
		LIBS="$LIBS $LIBTIRPC_LIBS"
		with_libtirpc=yes
	], [
		if test "x$with_libtirpc" = xyes
		then
			AC_MSG_ERROR([--with-libtirpc specified, but libtirpc could not be found])
		fi
		with_libtirpc=no
	])
])

AS_IF([test "x$with_libtirpc" = "xyes"],
        [AC_DEFINE(HAVE_LIBTIRPC, 1, [Enable libtirpc support])])

AC_CHECK_HEADERS([rpc/rpc.h])

# Detect glibc for GLIBCV
AC_CHECK_DECLS([__GLIBC__])

# Detect ipv6 headers and definitions for HASIPv6
AC_CHECK_DECLS([AF_INET6], [], [], [[#include <sys/socket.h>]])
AC_CHECK_HEADERS([netinet6/in6.h netinet/in6.h netinet/ip6.h])

# Detect tcp definitions for NEEDS_NETINET_TCPH
AC_CHECK_DECLS([TCP_ESTABLISHED], [], [], [[#include <netinet/tcp.h>]])

# Detect unix socket endpoint headers for HASUXSOCKEPT
AC_CHECK_HEADERS([linux/sock_diag.h linux/unix_diag.h])

# Detect pty endpoint definition for HASPTYEPT
AC_CHECK_DECLS([TTYAUX_MAJOR], [], [], [[#include <linux/major.h>]])

# Detect socket state definition for HASSOSTATE and HASSOOPT
AC_CHECK_DECLS([SS_CONNECTED], [], [], [[#include <linux/net.h>]])

# Define _FILE_OFFSET_BITS if necessary
AC_SYS_LARGEFILE

# Detect strftime function for HAS_STRFTIME
AC_CHECK_FUNCS([strftime])

# Detect selinux headers for HASSELINUX
AC_CHECK_HEADERS([selinux/selinux.h], [LIBS="$LIBS -lselinux"])

# Detect utmpx headers for HASUTMPX
AC_CHECK_HEADERS([utmpx.h])

# Detect vm_memattr_t definition for HAS_VM_MEMATTR_T
AC_CHECK_TYPES([vm_memattr_t], [], [], [[#include <sys/types.h>
		#include <vm/vm.h>]])

# Detect doadump function for NEEDS_BOOLEAN_T
AC_CHECK_DECLS([doadump], [], [], [[#define _KERNEL 1
		#include <sys/types.h>
		#include <sys/param.h>
		#include <sys/conf.h>]])

# Detect struct vnode definition for HAS_KVM_VNODE
AC_EGREP_CPP([struct vnode], [#define _KVM_VNODE 1
		#include <sys/vnode.h>], [AC_DEFINE([HAVE_KVM_VNODE], [1], ["Define to 1 if struct vnode exists"])
		LIBS="$LIBS -lkvm"])

# Detect SBS_CANTSENDMORE definition for HASSBSTATE
AC_CHECK_DECLS([SBS_CANTSENDMORE], [], [], [[#include <sys/types.h>
		#include <sys/socketvar.h>]])

# Detect lockf_entry definition for HAS_LOCKF_ENTRY
AC_CHECK_TYPES([struct lockf_entry], [
	# Generate lockf_owner.h from kern/kern_lockf.c
	rm -f ./lockf_owner.h
	if test -r ${FREEBSD_SYS}/kern/kern_lockf.c	# {
	then
		# Find the line number of struct lock_owner
		# Note extra quoting [[]] for M4
		LSOF_TMP1=$(grep -n "^struct lock_owner {" ${FREEBSD_SYS}/kern/kern_lockf.c | sed 's/\([[0-9]]*\):.*$/\1/')
		if test "X$LSOF_TMP1" != "X"	# {
		then
			LSOF_TMP2=0
			# Find the end of struct lock_owner
			for i in $(grep -n "};" ${FREEBSD_SYS}/kern/kern_lockf.c | sed 's/\([[0-9]]*\):.*/\1/') # {
			do
				if test $LSOF_TMP2 -eq 0 -a $i -gt $LSOF_TMP1	# {
				then
					LSOF_TMP2=$i
				fi	# }
				done	# }
			if test $LSOF_TMP2 -eq 0	# {
			then
				LSOF_TMP1=""
			else
				cat > ./lockf_owner.h << LOCKF_OWNER1
/*
 * lockf_owner.h -- created by lsof Configure script on
LOCKF_OWNER1
				printf " * " >> ./lockf_owner.h
				date >> ./lockf_owner.h
				cat >> ./lockf_owner.h << LOCKF_OWNER2
 */

#if	!defined(LOCKF_OWNER_H)
#define	LOCKF_OWNER_H

LOCKF_OWNER2
				ed -s ${FREEBSD_SYS}/kern/kern_lockf.c >> ./lockf_owner.h << LOCKF_OWNER3
${LSOF_TMP1},${LSOF_TMP2}p
LOCKF_OWNER3
				if test $? -ne 0	# {
				then
					LSOF_TMP1=""
				else
					cat >> ./lockf_owner.h << LOCKF_OWNER4

#endif	/* defined(LOCKF_OWNER_H) */
LOCKF_OWNER4
				fi	# }
			fi	# }
		fi	# }
	else
		echo "FATAL ERROR: can't read ${FREEBSD_SYS}/kern/kern_lockf.c"
	fi	# }

	if test "X$LSOF_TMP1" != "X" -a "X$LSOF_TMP2" != "X0" # {
	then
		AC_MSG_NOTICE([lockf_owner.h creation succeeded.])
		CFLAGS="$CFLAGS -DHAS_LOCKF_ENTRY"
	else
		AC_MSG_ERROR([lockf_owner.h creation failed (see 00FAQ)])
	fi	# }
], [], [#include <sys/types.h>
		#include <sys/lockf.h>])

# Detect ki_numthreads definition for HASTASKS
AC_CHECK_MEMBERS([struct kinfo_proc.ki_numthreads], [], [], [#include <sys/types.h>
		#include <sys/user.h>])

# Detect i_effnlink/i_ffs_effnlink for HASEFFNLINK
AC_CHECK_MEMBERS([struct inode.i_effnlink], [CFLAGS="$CFLAGS -DHASEFFNLINK=i_effnlink"], [], [#define __BSD_VISIBLE 1
		#include <sys/types.h>
		#include <ufs/ufs/quota.h>
		#include <ufs/ufs/inode.h>])

# Detect struct file.f_vnode for HASF_VNODE
AC_CHECK_MEMBERS([struct file.f_vnode], [], [], [#define __BSD_VISIBLE 1
		#define _KERNEL 1
		#define GENOFFSET
		#include <sys/types.h>
		#include <sys/file.h>])

# Detect struct filedescent for HAS_FILEDESCENT
AC_CHECK_TYPES([struct filedescent], [], [], [#define __BSD_VISIBLE 1
		#include <sys/types.h>
		#include <sys/filedesc.h>])

# Detect fs/tmpfs/tmpfs.h for HAS_TMPFS
if test -r ${FREEBSD_SYS}/fs/tmpfs/tmpfs.h	# {
then
	AC_DEFINE([HAS_TMPFS], [1], [Define if fs/tmpfs/tmpfs.h is found])
fi	#}

# Detect wctype.h for HASWCTYPE_H
AC_CHECK_HEADERS([wctype.h])

# Detect struct inode.i_din2 for HAS_UFS1_2
AC_CHECK_MEMBERS([struct inode.i_din2], [], [], [#define __BSD_VISIBLE 1
		#include <sys/types.h>
		#include <ufs/ufs/quota.h>
		#include <ufs/ufs/inode.h>])

# Detect struct vnode.v_lockf definition for HAS_V_LOCKF
AC_CHECK_MEMBERS([struct vnode.v_lockf], [], [], [#define _KVM_VNODE 1
		#define __BSD_VISIBLE 1
		#include <sys/types.h>
		#include <sys/vnode.h>])

# Detect fdescfs version for HASFDESCFS
AC_CHECK_HEADERS([fs/fdescfs/fdesc.h], [], [], [#define _KERNEL 1
		#define __BSD_VISIBLE 1
		#include <sys/types.h>
		#include <sys/mount.h>])

# Detect pseudofs for HASPSEUDOFS
if test -d ${FREEBSD_SYS}/fs/pseudofs	# {
then
	CFLAGS="$CFLAGS -DHASPSEUDOFS -I$FREEBSD_SYS"
fi	# }

# Detect nullfs for HASNULLFS
AC_CHECK_HEADERS([fs/nullfs/null.h], [], [], [])

# Detect struct xtcpcb.t_maxseg for HAS_XTCPCB_TMAXSEG
AC_CHECK_MEMBERS([struct xtcpcb.t_maxseg], [], [], [#include <sys/types.h>
	#include <sys/queue.h>
	#include <sys/socketvar.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netinet/in_pcb.h>
	#include <netinet/tcp_var.h>])

# Detect struct kinfo_file.kf_un.kf_sock.kf_sock_sendq for HAS_KF_SOCK_SENDQ
AC_CHECK_MEMBERS([struct kinfo_file.kf_un.kf_sock.kf_sock_sendq], [], [], [#include <sys/types.h>
	#include <sys/user.h>])

# Detect sizeof(dev_t) for LT_DEV64
AC_CHECK_SIZEOF([dev_t])
AS_IF([test "x$ac_cv_sizeof_dev_t" = x8], [
	LSOF_TEST_CFLAGS="$LSOF_TEST_CFLAGS -DLT_DEV64"
])

# Generate Makefile from Makefile.in/am
AC_CONFIG_FILES([Makefile])

# Pass build configurations to version.h.in
AC_SUBST(cc, $CC)
AC_SUBST(ccv, $($CC -v 2>&1 | sed -n 's/.*version \(.*\)/\1/p'))
AC_SUBST(ccflags, $CFLAGS)
AC_SUBST(ldflags, "$LDFLAGS$LIBS")
# Reproducible build
if test "X$SOURCE_DATE_EPOCH" = "X"	# {
then
	AC_SUBST(host, $(uname -n))
	AC_SUBST(logname, $LOGNAME)
	AC_SUBST(user, $USER)
	AC_SUBST(sysinfo, $(uname -a))
fi	# }

# Generate version.h/autotools.h
AC_CONFIG_FILES([autotools/version.h autotools/autotools.h])

# Generate version
AC_CONFIG_FILES([version])

# Generate config.h
AC_CONFIG_HEADERS([config.h])

# For VPATH building
AC_CONFIG_LINKS([tests/common.bash:tests/common.bash
	dialects/linux/tests/util-open-flags.bash:dialects/linux/tests/util-open-flags.bash
	Lsof.8:Lsof.8 00DIALECTS:00DIALECTS 00DIST:00DIST])

# Generate cflags for tests
LSOF_TEST_CFLAGS="$LSOF_TEST_CFLAGS -DLT_DIAL_$LSOF_TGT -DLT_VERS=$LSOF_VERS"
# Override default lsof path
LSOF_TEST_CFLAGS="$LSOF_TEST_CFLAGS -DLT_DEF_LSOF_PATH=\\\"$PWD/lsof\\\""
AC_SUBST([LSOF_TEST_CFLAGS])

# Finish
AC_OUTPUT
