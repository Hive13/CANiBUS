AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = canibus
canibus_SOURCES = main.cc socket.cc handler.cc screen.cc canibusmsg.cc \
  state.cc logger.cc clients.cc canbusdevice.cc

noinst_HEADERS = main.h socket.h handler.h screen.h canibusmsg.h \
  state.h logger.h clients.h canbusdevice.h

canibus_LDADD = -ltinyxml -lncurses
