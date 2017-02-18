# Process this file with autoconf to produce a configure script.
# $Id: configure.ac,v 1.18 2004/07/25 12:07:00 ahsu Exp $

AC_INIT([rolo],[011],[ahsu@users.sf.net])
AM_INIT_AUTOMAKE
AC_PREREQ(2.59)
AC_COPYRIGHT([Copyright (C) 2003  Andrew Hsu])
AC_REVISION($Revision: 1.18 $)
AC_CONFIG_SRCDIR([src/main.c])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(vc, vc_new)
AC_CHECK_LIB(ncursesw, newwin)
AC_CHECK_LIB(menuw, new_menu)
AC_CHECK_LIB(formw, new_form)
CFLAGS="$CFLAGS -I/usr/include/ncursesw/"

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([strdup strstr])

AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile])
AC_OUTPUT
