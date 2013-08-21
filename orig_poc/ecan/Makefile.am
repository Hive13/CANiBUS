AUTOMAKE_OPTIONS = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.h.in configure depcomp install-sh missing

INCLUDES = -I$(top_srcdir)

bin_PROGRAMS = ecanibus

ecanibus_SOURCES = main.c
ecanibus_LDADD = @ELEMENTARY_LIBS@
ecanibus_CFLAGS = @ELEMENTARY_CFLAGS@
