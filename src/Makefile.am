## Process this file with automake to produce Makefile.in
##
## $Id: Makefile.am,v 1.1 2003/02/26 04:36:44 ahsu Exp $
##

INCLUDES = -I../vcard

bin_PROGRAMS = rolo
rolo_SOURCES = main.c index.c view.c edit.c help.c
LDFLAGS = -lmenu -lncurses
rolo_LDADD = ../vcard/libvcard.a
