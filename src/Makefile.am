## Process this file with automake to produce Makefile.in
##
## $Id: Makefile.am,v 1.5 2003/03/24 07:44:15 ahsu Exp $
##

INCLUDES = -I../vcard

bin_PROGRAMS = rolo
rolo_SOURCES = main.c index.c index.h view.c view.h edit.c edit.h \
               help.c help.h delete.c delete.h add.c add.h entry.c entry.h
rolo_LDADD = ../vcard/libvcard.a
EXTRA_DIST = ChangeLog
