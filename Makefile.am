bin_PROGRAMS = lsof
lsof_SOURCES = arg.c main.c misc.c node.c print.c proc.c store.c usage.c util.c \
	lib/ptti.c lib/isfn.c lib/pdvn.c lib/lkud.c lib/rdev.c lib/dvch.c lib/ckkv.c lib/fino.c
DIALECT_PATH = $(top_srcdir)/dialects/$(LSOF_DIALECT)

lsof_SOURCES += $(DIALECT_PATH)/dmnt.c \
		$(DIALECT_PATH)/dproc.c \
		$(DIALECT_PATH)/dsock.c \
		$(DIALECT_PATH)/dstore.c

if HAS_DFILE
lsof_SOURCES += $(DIALECT_PATH)/dfile.c
endif

if HAS_DNODE
lsof_SOURCES += $(DIALECT_PATH)/dnode.c
endif

if HAS_DDEV
lsof_SOURCES += $(DIALECT_PATH)/ddev.c
endif

lsof_CPPFLAGS = -I$(DIALECT_PATH) -I$(top_srcdir)/autotools
lsof_CPPFLAGS += -DAUTOTOOLS
