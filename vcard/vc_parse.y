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
 * $Id: vc_parse.y,v 1.3 2003/04/19 14:58:10 ahsu Rel $
 */

%{

#include "vcard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YYSTYPE char*

extern FILE *yyin;

extern int yylex ();
void yyerror (char *s);

static vcard_component *current_vcard;
static vcard_component *current_vc;

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

first_line    : TOK_BEGIN_VCARD '\n' { current_vcard = vc_new (); }
              | TOK_GROUP '.' TOK_BEGIN_VCARD '\n' {
                  current_vcard = vc_new ();
                  vc_set_group (current_vcard, $1); }
              ;

end_line      : TOK_END_VCARD '\n'
              | TOK_GROUP '.' TOK_END_VCARD '\n'
              ;

contentlines  : contentlines group_contentline
              | group_contentline
              ;

group_contentline  : TOK_GROUP '.'
                     contentline { vc_set_group (current_vc, $1); }
                   | contentline
                   ;

contentline   : name params ':' value '\n'
              | name ':' value '\n' 
              ;

name          : TOK_NAME {
                  current_vc = vc_append_with_name (current_vcard, $1); }

              ;

params        : ';' param
              | params ';' param
              ;

param         : TOK_PARAM {
                  vcard_component_param *tmp_vc_param = NULL;
                  char str[80];
                  char *param_name = NULL;
                  char *param_value = NULL;
                  
                  tmp_vc_param = vc_param_new ();
                  vc_param_set_str (tmp_vc_param, $1);
                  
                  /* TODO: clean up parsing of parameters
                   */ 
                  strncpy (str, $1, 79);
                  str[79] = '\0';
                  param_name = strtok (str, "=");
                  param_value = param_name + strlen(param_name) + 1;
                  vc_param_set_name (tmp_vc_param, param_name);
                  vc_param_set_value (tmp_vc_param, param_value);
                  
                  vc_add_param (current_vc, tmp_vc_param); }

              ;

value         : TOK_VALUE { vc_set_value (current_vc, $1); }
              ;

%%

void
yyerror (char *s)
{
}

vcard_component *
parse_vcard_file (FILE * fp)
{
  vcard_component *v = NULL;

  yyin = fp;

  if (0 == yyparse ())
    {
      v = current_vcard;
    }

  return v;
}
