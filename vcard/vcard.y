/*
 * libvcard - vCard library
 * Copyright (C) 2003  Andrew Hsu
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: vcard.y,v 1.1 2003/02/16 05:02:15 ahsu Rel $
 */

%{

#include "vcard.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define YYSTYPE char*

extern FILE *yyin;

extern int yylex();
void yyerror( char *s );

static vcard *current_vcard;
static vcard_item *current_vi;

%}

/*-----------------------
   create a y.tab.h file
  -----------------------*/
%defines

%start vcard

%token TOK_BEGIN_VCARD
%token TOK_END_VCARD
%token TOK_GROUP
%token TOK_NAME
%token TOK_PARAM
%token TOK_VALUE

%%

vcard         : begin_line
                contentlines 
                end_line { YYACCEPT; }
              | error '\n' { yyerrok; }
              ;

begin_line    : filler first_line
              | first_line
              ;

filler        : filler '\n'
              | '\n'
              ;

first_line    : TOK_BEGIN_VCARD '\n' { current_vcard = create_vcard(); }
              | TOK_GROUP '.' TOK_BEGIN_VCARD '\n' {
                   current_vcard = create_vcard();
                   set_vcard_group(current_vcard, $1); }
              ;

end_line      : TOK_END_VCARD '\n'
              | TOK_GROUP '.' TOK_END_VCARD '\n'
              ;

contentlines  : contentlines group_contentline
              | group_contentline
              ;

group_contentline  : TOK_GROUP '.'
                     contentline { set_vcard_item_group(current_vi, $1); }
                   | contentline
                   ;

contentline   : name params ':' value '\n'
              | name ':' value '\n' 
              ;

name          : TOK_NAME { current_vi = insert_vcard_item(current_vcard, $1); }
              ;

params        : ';' param
              | params ';' param
              ;

param         : TOK_PARAM { insert_vcard_item_param(current_vi, $1); }
              ;

value         : TOK_VALUE { set_vcard_item_value(current_vi, $1); }
              ;

%%

void yyerror(char *s)
{
  fprintf(stderr, "** yyerror:%s\n", s);
}

vcard *parse_vcard_file(FILE *fp)
{
  vcard *v = NULL;

  yyin = fp;
  
  if(0 == yyparse())
    v = current_vcard;
  else
    v = NULL;

  return v;
}

