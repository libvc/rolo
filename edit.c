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
 * $Id: edit.c,v 1.4 2003/02/20 08:58:46 ahsu Exp $
 */

#include "edit.h"
#include <stdlib.h>
#include <string.h>

#define HARD_CODED_HEADER_STR "q:Quit  h:Help"

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
  keypad(win, TRUE);            /* enable keypad for use of arrow keys */
  print_header();
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
print_footer(const char *fn)
{
  char *footer_str = NULL;
  int i = 0;

  footer_str = (char *)malloc(sizeof(char) * (COLS + 2));

  for (i = 0; i < COLS; i++) {
    footer_str[i] = '-';
  }

  /*
   * ("---[ editing: %s ]---\n", fn);
   */

  footer_str[COLS] = '\n';
  footer_str[COLS + 1] = '\0';

  wattron(win, A_REVERSE);
  mvwprintw(win, LINES - 2, 0, footer_str);
  wstandend(win);
  free(footer_str);
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
