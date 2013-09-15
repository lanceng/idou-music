#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(iDouMusic, 0.1)
AM_INIT_AUTOMAKE(iDouMusic, 0.1)
LT_INIT
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Checks for libraries.
PKG_CHECK_MODULES([GTK3], [gtk+-3.0])
PKG_CHECK_MODULES([GST], [gstreamer-0.10])
PKG_CHECK_MODULES([WEBKIT], [webkitgtk-3.0])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 resource/Makefile
                 resource/image/Makefile
                 resource/image/button/Makefile
                 resource/image/skin/Makefile
                 resource/image/icon/Makefile
                 resource/image/playmode/Makefile
                 resource/image/combobox/Makefile
                 resource/html/Makefile
                 resource/html/js/Makefile
                 resource/html/css/Makefile
                 src/Makefile
                 src/tools/Makefile
                 src/itk/Makefile
                 src/app/Makefile
                 src/gui/Makefile])
AC_OUTPUT
