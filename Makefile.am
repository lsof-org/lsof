bin_PROGRAMS = lsof
lsof_SOURCES = arg.c main.c misc.c node.c print.c proc.c store.c usage.c util.c
DIALECT_PATH = $(top_srcdir)/dialects/$(LSOF_DIALECT)
lsof_SOURCES += $(DIALECT_PATH)/dfile.c \
		$(DIALECT_PATH)/dmnt.c \
		$(DIALECT_PATH)/dnode.c \
		$(DIALECT_PATH)/dproc.c \
		$(DIALECT_PATH)/dsock.c \
		$(DIALECT_PATH)/dstore.c
lsof_CPPFLAGS = -I$(DIALECT_PATH) -I$(top_srcdir)/autotools
lsof_CPPFLAGS += -DHASNORPC_H -DAUTOTOOLS
