# Process this file with autoconf to produce a configure script.
# $Id: configure.ac,v 1.18 2004/07/25 12:07:00 ahsu Exp $

AC_INIT([rolo],[020],[rafael@laboissiere.net])
AM_INIT_AUTOMAKE
AC_PREREQ([2.71])
AC_COPYRIGHT([Copyright (C) 2003  Andrew Hsu
Copyright (C) 2017, 2019, 2021, 2024  Rafael Laboissière])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(vc, vc_new)
AC_CHECK_LIB(ncursesw, newwin)
AC_CHECK_LIB(menuw, new_menu)
CFLAGS="$CFLAGS -I/usr/include/ncursesw/"

# Glib settings
PKG_CHECK_MODULES([GLIB],[glib-2.0])

# Libunac settings
PKG_CHECK_MODULES([UNAC],[unac])

# Set LIBS and CFLAGS
LIBS="$LIBS $GLIB_LIBS $UNAC_LIBS"
CFLAGS="$CFLAGS $GLIB_CFLAGS $UNAC_CFLAGS"

# Checks for header files.
AC_CHECK_INCLUDES_DEFAULT

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strdup strstr])

AC_CHECK_PROG(HAS_SHUNIT2,shunit2,yes)
if test x"$HAS_SHUNIT2" != x"yes" ; then
    AC_MSG_WARN([shunit2 not found. Unit tests will be skipped.])
fi

AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile test/Makefile])
AC_OUTPUT
