AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = canibusd
canibusd_SOURCES = main.cc server.cc sessionobject.cc client.cc \
  sessionproperty.cc hacksession.cc io.cc candevice.cc serial.cc \
  elmcandevice.cc canpacket.cc

noinst_HEADERS = main.h server.h sessionobject.h client.h sessionproperty.h \
  hacksession.h io.h candevice.h serial.h elmcandevice.h canpacket.h

canibusd_LDADD = -lcapsinetwork -lrt
