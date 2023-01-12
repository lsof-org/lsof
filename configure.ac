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
	*)
		AC_MSG_ERROR(["Host $host_os not supported"])
esac

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

# Generate Makefile from Makefile.in/am
AC_CONFIG_FILES([Makefile])

# Pass build configurations to version.h.in
AC_SUBST(host, $(uname -n))
AC_SUBST(logname, $LOGNAME)
AC_SUBST(user, $USER)
AC_SUBST(cc, $CC)
AC_SUBST(ccv, $($CC -v 2>&1 | sed -n 's/.*version \(.*\)/\1/p'))
AC_SUBST(ccflags, $CFLAGS)
AC_SUBST(ldflags, $LDFLAGS)
AC_SUBST(sysinfo, $(uname -a))

# Generate version.h from version.h.in
AC_CONFIG_FILES([autotools/version.h])

# Generate config.h
AC_CONFIG_HEADERS([config.h])

# Finish
AC_OUTPUT