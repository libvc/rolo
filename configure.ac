# Process this file with autoconf to produce a configure script.
#
# $Id: configure.ac,v 1.1 2003/02/27 07:29:11 ahsu Exp $
#
AC_INIT(rolo, 001, ahsu@users.sf.net)
AM_INIT_AUTOMAKE(rolo, 001)
AC_COPYRIGHT([Copyright (C) 2003  Andrew Hsu])
AC_REVISION($Revision$)
AC_CONFIG_SRCDIR([src/main.c])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB(ncurses, newwin)
AC_CHECK_LIB(menu, new_menu)

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([strdup strstr])

AC_CONFIG_FILES([Makefile vcard/Makefile src/Makefile])
AC_OUTPUT
