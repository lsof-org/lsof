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

# Detect glibc
AC_CHECK_DECLS([__GLIBC__])

# Generate Makefile from Makefile.in/am
AC_CONFIG_FILES([Makefile])

# Generate config.h
AC_CONFIG_HEADERS([config.h])

# Finish
AC_OUTPUT