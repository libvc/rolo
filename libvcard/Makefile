#
# libvcard - vCard library
# Copyright (C) 2003  Andrew Hsu
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# $Id$
#

SHELL = /bin/sh
CC = gcc
BISON = bison
FLEX = flex -i
CFLAGS = -g -ansi -Wall
YFLAGS = --yacc --debug
LDFLAGS = 
INDENT = gindent
IFLAGS = -nut -i2

all: lib

lib: lex vcard
	ar rcs libvcard.a y.tab.o lex.yy.o vcard.o

lex: parser
	$(FLEX) vcard.l
	$(CC) lex.yy.c -c

parser:
	$(BISON) $(YFLAGS) vcard.y
	$(CC) $(CFLAGS) y.tab.c -c

test: lib
	$(CC) $(CFLAGS) test.c -o test -l vcard -L "."

vcard:
	$(CC) $(CFLAGS) vcard.c -c

pretty:
	$(INDENT) $(IFLAGS) test.c
	$(INDENT) $(IFLAGS) vcard.c
	$(INDENT) $(IFLAGS) vcard.h

clean:
	-rm -fv lex.yy.c y.tab.c y.tab.h test libvcard.a *core *~ *.exe *.o *.stackdump

