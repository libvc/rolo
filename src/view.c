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
 * $Id: view.c,v 1.2 2003/02/28 03:03:31 ahsu Rel $
 */

#include "view.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define HARD_CODED_HEADER_STR "q:Quit  e:Edit  h:Help"

/*** PROTOTYPES ***/
static void print_header();
static void print_footer(int entry_number, const char *fn);

/*** STATIC VARIABLES ***/
static WINDOW *win = NULL;
static WINDOW *sub = NULL;
static void (*display_help) (void);

/* ------------------------------------------------------------------
    Initialize the view window without displaying it to the
    end-user.
 */

void
init_view()
{
  win = newwin(0, 0, 0, 0);
  sub = derwin(win, LINES - 3, COLS, 1, 0);
  print_header();
  keypad(win, TRUE);            /* enable keypad for use of arrow keys */
}

/* ------------------------------------------------------------------
    Display the given vCard to the end-user.
 */

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
  wprintw(sub, "Name            : %s\n", str ? str : "");
  print_footer(entry_number, str ? str : "");

  vi = get_vcard_item_by_name(v, VC_NICKNAME);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Nickname        : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_BIRTHDAY);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Birthday        : %s\n", str ? str : "");

  /*********************
    Delivery Addressing
   *********************/

  vi = get_vcard_item_by_name(v, VC_ADDRESS);
  str = get_val_struct_part(get_vcard_item_value(vi), ADR_STREET);
  wprintw(sub, "Street Address  : %s\n", str ? str : "");
  free(str);

  vi = get_vcard_item_by_name(v, VC_ADDRESS);
  str = get_val_struct_part(get_vcard_item_value(vi), ADR_LOCALITY);
  wprintw(sub, "City            : %s\n", str ? str : "");
  free(str);

  vi = get_vcard_item_by_name(v, VC_ADDRESS);
  str = get_val_struct_part(get_vcard_item_value(vi), ADR_COUNTRY);
  wprintw(sub, "Country         : %s\n", str ? str : "");
  free(str);

  /*******************************
    Telecommunications Addressing
   *******************************/

  vi = get_vcard_item_by_name(v, VC_TELEPHONE);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Telephone       : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_EMAIL);
  str = get_vcard_item_value(vi);
  wprintw(sub, "E-Mail          : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_MAILER);
  str = get_vcard_item_value(vi);
  wprintw(sub, "E-Mailer        : %s\n", str ? str : "");

  /**************
    Geographical
   **************/

  vi = get_vcard_item_by_name(v, VC_TIME_ZONE);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Time Zone       : %s\n", str ? str : "");

  /****************
    Organizational
   ****************/

  vi = get_vcard_item_by_name(v, VC_TITLE);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Title           : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_ORGANIZATION);
  str = get_val_struct_part(get_vcard_item_value(vi), ORG_NAME);
  wprintw(sub, "Organization    : %s\n", str ? str : "");
  free(str);

  /*************
    Explanatory
   *************/

  vi = get_vcard_item_by_name(v, VC_CATEGORIES);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Categories      : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_NOTE);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Note            : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_REVISION);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Revision        : %s\n", str ? str : "");

  vi = get_vcard_item_by_name(v, VC_URL);
  str = get_vcard_item_value(vi);
  wprintw(sub, "URL             : %s\n", str ? str : "");

  /**********
    Security
   **********/

  vi = get_vcard_item_by_name(v, VC_CLASS);
  str = get_vcard_item_value(vi);
  wprintw(sub, "Class           : %s\n", str ? str : "");

  touchwin(win);
  wrefresh(sub);
  wrefresh(win);
}

/* ------------------------------------------------------------------
    Prints the header to the window buffer, but does not display it
    to the end-user.
 */

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

/* ------------------------------------------------------------------
    Prints the footer to the window buffer, but does not display it
    to the end-user.  The contents of the footer depends on the
    width of the screen.
 */

static void
print_footer(int entry_number, const char *fn)
{
  char *footer_str = NULL;
  char *entry_block = NULL;
  int entry_block_len = 0;
  int i = 0;

  footer_str = (char *)malloc(sizeof(char) * (COLS + 2));

  /****************************************
    initialize the footer string to dashes
   ****************************************/

  for (i = 0; i < COLS; i++) {
    footer_str[i] = '-';
  }

  if (NULL != fn) {
    entry_block_len = strlen(fn) + 18 + 3;      /* 3 for entry_number */
  } else {
    entry_block_len = 17 + 3;   /* 3 for entry_number */
  }

  /****************************************************
    add the `entry' block only if there is enough room
   ****************************************************/

  if (entry_block_len <= COLS) {
    entry_block = (char *)malloc(sizeof(char) * (entry_block_len + 1));

    if (NULL != fn) {
      sprintf(entry_block, "---[ entry %i: %s ]---", entry_number % 1000, fn);
    } else {
      sprintf(entry_block, "---[ entry %i ]---", entry_number % 1000);
    }

    entry_block_len = strlen(entry_block);
    for (i = 0; i < entry_block_len; i++) {
      footer_str[i] = entry_block[i];
    }

    free(entry_block);
  }

  footer_str[COLS] = '\n';
  footer_str[COLS + 1] = '\0';

  wattron(win, A_REVERSE);
  mvwprintw(win, LINES - 2, 0, footer_str);
  wstandend(win);
  free(footer_str);
}

/* ------------------------------------------------------------------
    Handle input from the end-user.
 */

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

/* ------------------------------------------------------------------
 */

void
set_view_help_fcn(void (*fcn) (void))
{
  display_help = fcn;
}
