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

lsof_CPPFLAGS = -I$(DIALECT_PATH) -I$(top_srcdir)/autotools
lsof_CPPFLAGS += -DAUTOTOOLS

# manpages
lsof.man: Lsof.8 version 00DIALECTS
	soelim < Lsof.8 > $@
man8_MANS = lsof.man
EXTRA_DIST += Lsof.8

# testing
check:
	bash check.bash $(LSOF_DIALECT)
