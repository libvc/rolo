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
 * $Id: help.c,v 1.1 2003/02/16 06:11:58 ahsu Exp $
 */

#include "help.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define HARD_CODED_HEADER_STR "Press any key to exit this screen"

/*** GLOBALS ***/

enum help_window { HELP_INDEX, HELP_VIEW, HELP_EDIT };

/*** PROTOTYPES ***/
static void print_header();
static void print_footer(const char *text);
static void process_help_commands();

static WINDOW *win = NULL;
static WINDOW *sub = NULL;

void
init_help()
{
  win = newwin(0, 0, 0, 0);
  sub = derwin(win, LINES - 3, COLS, 1, 0);
  print_header();
}

void
show_index_help()
{
  werase(sub);
  wprintw(sub, "q              quit the program\n");
  wprintw(sub, "<Down>         move to the next item\n");
  wprintw(sub, "<Up>           move to the previous item\n");
  wprintw(sub, "<Home>         move to the first item\n");
  wprintw(sub, "<End>          move to the last item\n");
  wprintw(sub, "j              move to the next item\n");
  wprintw(sub, "k              move to the previous item\n");
  wprintw(sub,
      "h              show help for key bindings (what you are viewing now)\n");
  wprintw(sub, "<Page Up>      scroll up a page\n");
  wprintw(sub, "<Page Down>    scroll down a page\n");
  wprintw(sub, "t              tag/un-tag an item\n");
  print_footer("Help for the index screen");
  touchwin(win);
  wrefresh(sub);
  wrefresh(win);
  process_help_commands();
}

void
show_view_help()
{
  werase(sub);
  wprintw(sub, "<Down>   move to the next item\n");
  wprintw(sub, "<Up>     move to the previous item\n");
  wprintw(sub, "<Home>   move to the first item\n");
  wprintw(sub, "<End>    move to the last item\n");
  wprintw(sub, "j        move to the next item\n");
  wprintw(sub, "k        move to the previous item\n");
  wprintw(sub,
      "h        show help for key bindings (what you are viewing now)\n");
  print_footer("Help for the view screen");
  touchwin(win);
  wrefresh(sub);
  wrefresh(win);
  process_help_commands();
}

void
show_edit_help()
{
  werase(sub);
  wprintw(sub, "This is help documentation for edit.");
  print_footer("Help for the edit screen");
  touchwin(win);
  wrefresh(sub);
  wrefresh(win);
  process_help_commands();
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
print_footer(const char *text)
{
  wattron(win, A_REVERSE);
  mvwprintw(win, LINES - 2, 0, "%s", text);
  wstandend(win);
}

static void
process_help_commands()
{
  int ch = 0;

  ch = wgetch(sub);
}