#
# rolo - contact management software
# Copyright (C) 2003  Andrew Hsu
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#
# $Id: Makefile,v 1.1 2003/02/16 06:09:15 ahsu Exp $
#

SHELL = /bin/sh
CC = gcc
BISON = bison
FLEX = flex
CFLAGS = -g -ansi -Wall -Ilibvcard
YFLAGS = --yacc --debug
LDFLAGS = -lmenu -lncurses
INDENT = gindent
IFLAGS = -nut -i2

all: rolo

rolo: lib main index view edit help
	$(CC) main.o help.o index.o view.o edit.o \
        $(CFLAGS) $(LDFLAGS) -o rolo -lvcard -Llibvcard

lib:
	cd libvcard; make; cd ..

main: main.c
	$(CC) $(CFLAGS) main.c -c

index: index.c index.h
	$(CC) $(CFLAGS) index.c -c

view: view.c view.h
	$(CC) $(CFLAGS) view.c -c

edit: edit.c edit.h
	$(CC) $(CFLAGS) edit.c -c

help: help.c help.h
	$(CC) $(CFLAGS) help.c -c

pretty:
	$(INDENT) $(IFLAGS) main.c
	$(INDENT) $(IFLAGS) index.c
	$(INDENT) $(IFLAGS) index.h
	$(INDENT) $(IFLAGS) view.c
	$(INDENT) $(IFLAGS) view.h
	$(INDENT) $(IFLAGS) edit.c
	$(INDENT) $(IFLAGS) edit.h
	$(INDENT) $(IFLAGS) help.c
	$(INDENT) $(IFLAGS) help.h

clean:
	-rm -fv rolo *core *~ *.exe *.o *.stackdump

