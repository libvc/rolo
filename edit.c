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

#include "edit.h"

static void print_header();
static void print_footer(const char *fn);

static WINDOW *win = NULL;
static WINDOW *sub = NULL;
static void (*display_help) (void);

void
init_edit()
{
  win = newwin(0, 0, 0, 0);
  sub = derwin(win, LINES - 3, COLS, 1, 0);
  print_header();
}

static void
print_header()
{
  wattron(win, A_REVERSE);
  wprintw(win,
      "q:Quit  h:Help                                                                 \n");
  wstandend(win);
}

static void
print_footer(const char *fn)
{
  wattron(win, A_REVERSE);
  mvwprintw(win, LINES - 2, 0, "-[ %s ]-\n", fn);
  wstandend(win);
}

void
edit_vcard(vcard * v)
{
  werase(sub);
  wprintw(sub, "edit vcard goes here.");
  print_footer("footer");

  touchwin(win);
  wrefresh(sub);
  wrefresh(win);
}

int
process_edit_commands()
{
  bool done = FALSE;
  int ch = 0;
  int command = 0;

  while (!done) {
    ch = wgetch(win);

    switch (ch) {
    case 'h':
      (*display_help) ();
      touchwin(win);
      wrefresh(win);
      wrefresh(sub);
      break;
    case 'q':
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
set_edit_help_fcn(void (*fcn) (void))
{
  display_help = fcn;
}
