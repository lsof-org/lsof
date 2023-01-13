AC_INIT([lsof],[4.96.6])
AM_INIT_AUTOMAKE([subdir-objects])

# Require C compiler
AC_PROG_CC

# Detect OS
AC_CANONICAL_HOST

LSOF_DIALECT=unknown
case "${host_os}" in
	linux*)
		LSOF_DIALECT=linux
		;;
	darwin*)
		LSOF_DIALECT=darwin/libproc
		LSOF_VSTR=$(uname -r)
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
				AC_MSG_NOTICE([Unknown Darwin release: `uname -r`])
				AC_MSG_NOTICE([Assuming Darwin 19.0])
				LSOF_VERS=1900
				;;
			*)
		esac

		# Add DARWINV to cflags
		CFLAGS="$CFLAGS -DDARWINV=$LSOF_VERS"

		;;
	*)
		AC_MSG_ERROR(["Host $host_os not supported"])
esac

# Dialect provides dnode.c/ddev.c
AM_CONDITIONAL([HAS_DNODE], [test x$LSOF_DIALECT != xdarwin/libproc])
AM_CONDITIONAL([HAS_DDEV], [test x$LSOF_DIALECT = xdarwin/libproc])

# Pass LSOF_DIALECT to Makefile.am
AC_SUBST([LSOF_DIALECT])

# Check rpc/rpc.h
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

# Generate Makefile from Makefile.in/am
AC_CONFIG_FILES([Makefile])

# Pass build configurations to version.h.in
AC_SUBST(host, $(uname -n))
AC_SUBST(logname, $LOGNAME)
AC_SUBST(user, $USER)
AC_SUBST(cc, $CC)
AC_SUBST(ccv, $($CC -v 2>&1 | sed -n 's/.*version \(.*\)/\1/p'))
AC_SUBST(ccflags, $CFLAGS)
AC_SUBST(ldflags, "$LDFLAGS$LIBS")
AC_SUBST(sysinfo, $(uname -a))

# Generate version.h/autotools.h
AC_CONFIG_FILES([autotools/version.h autotools/autotools.h])

# Generate config.h
AC_CONFIG_HEADERS([config.h])

# Finish
AC_OUTPUT
