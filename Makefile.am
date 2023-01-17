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
lsof_SOURCES += $(DIALECT_ROOT)/linux/dfile.c \
		$(DIALECT_ROOT)/linux/dmnt.c \
		$(DIALECT_ROOT)/linux/dnode.c \
		$(DIALECT_ROOT)/linux/dproc.c \
		$(DIALECT_ROOT)/linux/dsock.c \
		$(DIALECT_ROOT)/linux/dstore.c \
		$(DIALECT_ROOT)/linux/dlsof.h \
		$(DIALECT_ROOT)/linux/dproto.h \
		$(DIALECT_ROOT)/linux/machine.h
endif

if DARWIN_LIBPROC
lsof_SOURCES += $(DIALECT_ROOT)/darwin/libproc/ddev.c \
		$(DIALECT_ROOT)/darwin/libproc/dfile.c \
		$(DIALECT_ROOT)/darwin/libproc/dmnt.c \
		$(DIALECT_ROOT)/darwin/libproc/dproc.c \
		$(DIALECT_ROOT)/darwin/libproc/dsock.c \
		$(DIALECT_ROOT)/darwin/libproc/dstore.c \
		$(DIALECT_ROOT)/darwin/libproc/dlsof.h \
		$(DIALECT_ROOT)/darwin/libproc/dproto.h \
		$(DIALECT_ROOT)/darwin/libproc/machine.h
endif

if FREEBSD
lsof_SOURCES += $(DIALECT_ROOT)/freebsd/dmnt.c \
		$(DIALECT_ROOT)/freebsd/dnode.c \
		$(DIALECT_ROOT)/freebsd/dproc.c \
		$(DIALECT_ROOT)/freebsd/dsock.c \
		$(DIALECT_ROOT)/freebsd/dstore.c \
		$(DIALECT_ROOT)/freebsd/dlsof.h \
		$(DIALECT_ROOT)/freebsd/dproto.h \
		$(DIALECT_ROOT)/freebsd/machine.h
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
