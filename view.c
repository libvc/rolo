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
 * $Id$
 */

#include "view.h"
#include <assert.h>

/*** GLOBALS ***/

/*** PROTOTYPES ***/
static void print_header();
static void print_footer(const char *fn);

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
view_vcard(vcard * v)
{
  vcard_item *vi = NULL;
  char *str = NULL;

  werase(sub);

  vi = get_vcard_item_by_name(v, VC_FORMATTED_NAME);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Name       : %s\n", str ? str : "");
  print_footer(str ? str : "");

  vi = get_vcard_item_by_name(v, VC_NICKNAME);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Nickname   : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_TELEPHONE);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Telephone  : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_TIME_ZONE);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Time Zone  : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_EMAIL);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Email      : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_MAILER);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Mailer     : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_BIRTHDAY);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Birthday   : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_REVISION);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Revision   : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_NOTE);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Note       : %s\n", str ? str : "");

  touchwin(win);
  wrefresh(sub);
  wrefresh(win);
}

static void
print_header()
{
  wattron(win, A_REVERSE);
  wprintw(win,
      "q:Quit  e:Edit  j:Next  k:Prev  h:Help                                       \n");
  wstandend(win);
}

static void
print_footer(const char *fn)
{
  wattron(win, A_REVERSE);
  mvwprintw(win, LINES - 2, 0, "-[ %s ]-\n", fn);
  wstandend(win);
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
