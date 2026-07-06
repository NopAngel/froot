AC_INIT([froot],, [angelnieto1402@gmail.com])

AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_INIT_AUTOMAKE([foreign -Wall -Werror subdir-objects])
AC_CONFIG_HEADERS([config.h])

PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.40])

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug], [Enable debug logging and symbols])],
  [enable_debug=$enableval],
  [enable_debug=no])


AS_IF([test "x$enable_debug" = "xyes"], [
  AC_DEFINE([DEBUG_MODE], [1], [Define if debug mode is enabled])
  CFLAGS="$CFLAGS -g -O0"
], [
  CFLAGS="$CFLAGS -O2"
])


AC_CONFIG_FILES([
  Makefile
  src/Makefile
])

AC_OUTPUT
