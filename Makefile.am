bin_PROGRAMS = lsof

# Documentation
EXTRA_DIST = 00.README.FIRST 00CREDITS 00DCACHE 00DIALECTS 00DIST 00FAQ 00LSOF-L 00MANIFEST 00PORTING 00QUICKSTART 00README 00TEST 00XCONFIG
# Testing
EXTRA_DIST += tests/00README tests/TestDB tests/CkTestDB tests/Makefile tests/LsofTest.h $(wildcard tests/*.bash tests/*.c) \
	check.bash $(wildcard dialects/*/tests/*.bash) $(wildcard dialects/*/tests/Makefile) \
	$(wildcard dialects/*/tests/*.c)

# Dialect neutral sources
lsof_SOURCES = arg.c main.c misc.c node.c print.c proc.c store.c usage.c util.c \
	lib/ptti.c lib/isfn.c lib/pdvn.c lib/lkud.c lib/rdev.c lib/dvch.c lib/ckkv.c lib/fino.c
lsof_SOURCES += lsof.h lsof_fields.h proto.h hash.h
DIALECT_ROOT = $(top_srcdir)/dialects
DIALECT_PATH = $(DIALECT_ROOT)/$(LSOF_DIALECT_DIR)

# Dialect specific sources
if LINUX
lsof_SOURCES += dialects/linux/dfile.c \
		dialects/linux/dmnt.c \
		dialects/linux/dnode.c \
		dialects/linux/dproc.c \
		dialects/linux/dsock.c \
		dialects/linux/dstore.c \
		dialects/linux/dlsof.h \
		dialects/linux/dproto.h \
		dialects/linux/machine.h
endif

if DARWIN_LIBPROC
lsof_SOURCES += dialects/darwin/libproc/ddev.c \
		dialects/darwin/libproc/dfile.c \
		dialects/darwin/libproc/dmnt.c \
		dialects/darwin/libproc/dproc.c \
		dialects/darwin/libproc/dsock.c \
		dialects/darwin/libproc/dstore.c \
		dialects/darwin/libproc/dlsof.h \
		dialects/darwin/libproc/dproto.h \
		dialects/darwin/libproc/machine.h
endif

if FREEBSD
lsof_SOURCES += dialects/freebsd/dmnt.c \
		dialects/freebsd/dnode.c \
		dialects/freebsd/dproc.c \
		dialects/freebsd/dsock.c \
		dialects/freebsd/dstore.c \
		dialects/freebsd/dlsof.h \
		dialects/freebsd/dproto.h \
		dialects/freebsd/machine.h
endif

lsof_CPPFLAGS = -I$(DIALECT_PATH) -Iautotools
lsof_CPPFLAGS += -DAUTOTOOLS

# Manpages
lsof.man: Lsof.8 version 00DIALECTS
	soelim < Lsof.8 > $@
man8_MANS = lsof.man
EXTRA_DIST += Lsof.8

# Testing scripts
AM_TESTS_ENVIRONMENT = export LSOF_DIALECT_DIR=$(LSOF_DIALECT_DIR); export LSOF_DIALECT=$(LSOF_DIALECT);
# Dialect neutral
TESTS = tests/case-00-hello.bash \
	tests/case-01-version.bash \
	tests/case-20-exit-status.bash \
	tests/case-20-fd-only-inclusion.bash \
	tests/case-20-handle-missing-files.bash \
	tests/case-20-repeat-count.bash \
	tests/case-21-exit-Q-status.bash

# Dialect specific
check_PROGRAMS =
if LINUX
check_PROGRAMS += dialects/linux/tests/epoll \
	dialects/linux/tests/eventfd \
	dialects/linux/tests/mmap \
	dialects/linux/tests/mq_fork \
	dialects/linux/tests/mq_open \
	dialects/linux/tests/open_with_flags \
	dialects/linux/tests/pidfd \
	dialects/linux/tests/pipe \
	dialects/linux/tests/pty \
	dialects/linux/tests/ux 
dialects_linux_tests_mq_fork_LDADD = -lrt
dialects_linux_tests_mq_open_LDADD = -lrt

TESTS += dialects/linux/tests/case-10-mqueue.bash \
	dialects/linux/tests/case-10-ux-socket-state.bash \
	dialects/linux/tests/case-20-epoll.bash \
	dialects/linux/tests/case-20-eventfd-endpoint.bash \
	dialects/linux/tests/case-20-inet6-ffffffff-handling.bash \
	dialects/linux/tests/case-20-inet6-socket-endpoint.bash \
	dialects/linux/tests/case-20-inet-socket-endpoint.bash \
	dialects/linux/tests/case-20-mmap.bash \
	dialects/linux/tests/case-20-mqueue-endpoint.bash \
	dialects/linux/tests/case-20-open-flags-cx.bash \
	dialects/linux/tests/case-20-open-flags-path.bash \
	dialects/linux/tests/case-20-open-flags-tmpf.bash \
	dialects/linux/tests/case-20-pidfd-pid.bash \
	dialects/linux/tests/case-20-pipe-endpoint.bash \
	dialects/linux/tests/case-20-pipe-no-close-endpoint.bash \
	dialects/linux/tests/case-20-pty-endpoint.bash \
	dialects/linux/tests/case-20-ux-socket-endpoint.bash \
	dialects/linux/tests/case-20-ux-socket-endpoint-unaccepted.bash
endif

# Testing programs
TESTS += tests/LTbasic tests/LTbigf tests/LTdnlc tests/LTlock tests/LTnfs tests/LTnlink tests/LTsock tests/LTszoff tests/LTunix

check_PROGRAMS += tests/LTbasic tests/LTbigf tests/LTdnlc tests/LTlock tests/LTnfs tests/LTnlink tests/LTsock tests/LTszoff tests/LTunix
tests_LTbasic_SOURCES = tests/LTbasic.c tests/LTlib.c
tests_LTbasic_CFLAGS = @LSOF_TEST_CFLAGS@
tests_LTbigf_SOURCES = tests/LTbigf.c tests/LTlib.c
tests_LTbigf_CFLAGS = @LSOF_TEST_CFLAGS@
tests_LTdnlc_SOURCES = tests/LTdnlc.c tests/LTlib.c
tests_LTdnlc_CFLAGS = @LSOF_TEST_CFLAGS@
tests_LTlock_SOURCES = tests/LTlock.c tests/LTlib.c
tests_LTlock_CFLAGS = @LSOF_TEST_CFLAGS@
tests_LTnfs_SOURCES = tests/LTnfs.c tests/LTlib.c
tests_LTnfs_CFLAGS = @LSOF_TEST_CFLAGS@
tests_LTnlink_SOURCES = tests/LTnlink.c tests/LTlib.c
tests_LTnlink_CFLAGS = @LSOF_TEST_CFLAGS@
tests_LTsock_SOURCES = tests/LTsock.c tests/LTlib.c
tests_LTsock_CFLAGS = @LSOF_TEST_CFLAGS@
tests_LTszoff_SOURCES = tests/LTszoff.c tests/LTlib.c
tests_LTszoff_CFLAGS = @LSOF_TEST_CFLAGS@
tests_LTunix_SOURCES = tests/LTunix.c tests/LTlib.c
tests_LTunix_CFLAGS = @LSOF_TEST_CFLAGS@
