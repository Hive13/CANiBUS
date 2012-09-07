AC_INIT([src/main.cc], [0.1], [og@opengarages.org],
             [CANiBUS], [http://opengarages.org/])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE(canibusd, 0.0.1)
AC_LANG(C++)
AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX

# Checks for header files.
AC_CHECK_HEADER([libcapsinetwork/listener.h], [], [AC_MSG_ERROR([Please install libCapsiNetwork >= 0.3.0 from http://unixcode.org/libcapsinetwork/])])
AC_CHECK_HEADER([libcapsinetwork/socket.h], [], [AC_MSG_ERROR([Please install libCapsiNetwork >= 0.3.0 from http://unixcode.org/libcapsinetwork/])])
AC_CHECK_HEADER([libcapsinetwork/socket.h], [], [AC_MSG_ERROR([Please install libCapsiNetwork >= 0.3.0 from http://unixcode.org/libcapsinetwork/])])
AC_CHECK_HEADER([tinyxml.h], [], [AC_MSG_ERROR([Please install libTinyXML])])
AC_CHECK_HEADER([ncurses.h], [], [AC_MSG_ERROR([Please install libncurses])])

AC_OUTPUT([
  Makefile
  src/Makefile
  client/Makefile
])

