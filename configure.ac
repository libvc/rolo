# Process this file with autoconf to produce a configure script.
# $Id: configure.ac,v 1.16 2003/05/14 12:36:20 ahsu Exp $

AC_INIT(rolo, 011, ahsu@users.sf.net)
AM_INIT_AUTOMAKE
AC_PREREQ(2.57)
AC_COPYRIGHT([Copyright (C) 2003  Andrew Hsu])
AC_REVISION($Revision: 1.16 $)
AC_CONFIG_SRCDIR([src/main.c])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(vc, vc_new)
AC_CHECK_LIB(ncurses, newwin)
AC_CHECK_LIB(menu, new_menu)
AC_CHECK_LIB(form, new_form)

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
