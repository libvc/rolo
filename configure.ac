# Process this file with autoconf to produce a configure script.
#
# $Id$
#
AC_INIT(rolo, 001, ahsu@users.sf.net)
AM_INIT_AUTOMAKE(rolo, 001)
AC_CONFIG_SRCDIR([src/main.c])
# AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_YACC
AM_PROG_LEX

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([strdup strstr])

AC_CONFIG_FILES([Makefile vcard/Makefile src/Makefile])
AC_OUTPUT
