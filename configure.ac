# Process this file with autoconf to produce a configure script.
# $Id: configure.ac,v 1.18 2004/07/25 12:07:00 ahsu Exp $

AC_INIT([rolo],[011],[ahsu@users.sf.net])
AM_INIT_AUTOMAKE
AC_PREREQ(2.59)
AC_COPYRIGHT([Copyright (C) 2003  Andrew Hsu
Copyright (C) 2017  Rafael Laboissiere])
AC_CONFIG_SRCDIR([src/main.c])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(vc, vc_new)
AC_CHECK_LIB(ncursesw, newwin)
AC_CHECK_LIB(menuw, new_menu)
CFLAGS="$CFLAGS -I/usr/include/ncursesw/"

# Glib settings
LIBS="$LIBS $(pkg-config glib-2.0 --libs)"
CFLAGS="$CFLAGS $(pkg-config glib-2.0 --cflags)"

# Libunac settings
LIBS="$LIBS $(pkg-config unac --libs)"
CFLAGS="$CFLAGS $(pkg-config unac --cflags)"

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
