/*
 * rolo - contact management software
 * Copyright (C) 2003  Andrew Hsu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * $Id: view.c,v 1.4 2003/02/20 07:27:14 ahsu Exp $
 */

#include "view.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define HARD_CODED_HEADER_STR "q:Quit  j:Next  k:Prev  h:Help"

/*** GLOBALS ***/

/*** PROTOTYPES ***/
static void print_header();
static void print_footer(int entry_number, const char *fn);

static WINDOW *win = NULL;
static WINDOW *sub = NULL;

static void (*display_help) (void);

void
init_view()
{
  win = newwin(0, 0, 0, 0);
  sub = derwin(win, LINES - 3, COLS, 1, 0);
  print_header();
}

void
view_vcard(int entry_number, vcard * v)
{
  vcard_item *vi = NULL;
  char *str = NULL;

  werase(sub);

  /****************
    Identification
   ****************/

  vi = get_vcard_item_by_name(v, VC_FORMATTED_NAME);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Name          : %s\n", str ? str : "");
  print_footer(entry_number, str ? str : "");

  vi = get_vcard_item_by_name(v, VC_NICKNAME);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Nickname      : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_BIRTHDAY);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Birthday      : %s\n", str ? str : "");

  /*********************
    Delivery Addressing
   *********************/

  vi = get_vcard_item_by_name(v, VC_DELIVERY_LABEL);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Address       : %s\n", str ? str : "");

  /*******************************
    Telecommunications Addressing
   *******************************/

  vi = get_vcard_item_by_name(v, VC_TELEPHONE);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Telephone     : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_EMAIL);
  str = get_vcard_item_value(vi);
  wprintw(sub, "E-Mail        : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_MAILER);
  str = get_vcard_item_value(vi);
  wprintw(sub, "E-Mailer      : %s\n", str ? str : "");

  /**************
    Geographical
   **************/

  vi = get_vcard_item_by_name(v, VC_TIME_ZONE);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Time Zone     : %s\n", str ? str : "");

  /****************
    Organizational
   ****************/

  vi = get_vcard_item_by_name(v, VC_TITLE);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Title         : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_ORGANIZATION);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Organization  : %s\n", str ? str : "");

  /*************
    Explanatory
   *************/

  vi = get_vcard_item_by_name(v, VC_CATEGORIES);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Categories    : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_NOTE);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Note          : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_REVISION);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Revision      : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_URL);
  str = get_vcard_item_value(vi);
  wprintw(sub, "URL           : %s\n", str ? str : "");

  /**********
    Security
   **********/

  vi = get_vcard_item_by_name(v, VC_CLASS);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Class         : %s\n", str ? str : "");

  touchwin(win);
  wrefresh(sub);
  wrefresh(win);
}

static void
print_header()
{
  char *header_str = NULL;
  int i = 0;

  header_str = (char *)malloc(sizeof(char) * (COLS + 2));

  strncpy(header_str, HARD_CODED_HEADER_STR, COLS);

  for (i = strlen(HARD_CODED_HEADER_STR); i < COLS; i++) {
    header_str[i] = ' ';
  }

  header_str[COLS] = '\n';
  header_str[COLS + 1] = '\0';

  wattron(win, A_REVERSE);
  wprintw(win, header_str);
  wstandend(win);
  free(header_str);
}

static void
print_footer(int entry_number, const char *fn)
{
  char *footer_str = NULL;
  int i = 0;

  footer_str = (char *)malloc(sizeof(char) * (COLS + 2));

  for (i = 0; i < COLS; i++)
  {
    footer_str[i] = '-';
  }

  /*
     ("---[ entry %i: %s ]---\n", entry_number, fn);
   */

  footer_str[COLS] = '\n';
  footer_str[COLS + 1] = '\0';

  wattron(win, A_REVERSE);
  mvwprintw(win, LINES - 2, 0, footer_str);
  wstandend(win);
  free(footer_str);
}

int
process_view_commands()
{
  bool done = FALSE;
  int ch = 0;
  int command = 0;

  while (!done) {
    ch = wgetch(win);

    switch (ch) {
    case KEY_DOWN:
    case 'j':
      command = VIEW_COMMAND_NEXT;
      done = TRUE;
      break;
    case KEY_UP:
    case 'k':
      command = VIEW_COMMAND_PREVIOUS;
      done = TRUE;
      break;
    case 'e':
      command = VIEW_COMMAND_EDIT;
      done = TRUE;
      break;
    case 'h':
      (*display_help) ();
      touchwin(win);
      wrefresh(win);
      wrefresh(sub);
      break;
    case 'q':
      command = VIEW_COMMAND_INDEX;
      done = TRUE;
      break;
    default:
      beep();
      break;
    }
  }
  return command;
}

void
set_view_help_fcn(void (*fcn) (void))
{
  display_help = fcn;
}
