/*
 *  rolo - contact management software
 *  Copyright (C) 2003  Andrew Hsu
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307 USA
 *
 *  $Id: help.c,v 1.13 2003/05/20 01:08:52 ahsu Exp $
 */

#include "help.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MENU_BAR_STRING "Press any key to exit this screen"

enum help_window
{ HELP_INDEX, HELP_VIEW, HELP_EDIT };

/*** PROTOTYPES ***/
static void print_header ();
static void print_footer (const char *text);
static void process_help_commands ();

/*** STATIC VARIABLES ***/
static WINDOW *win = NULL;
static WINDOW *sub = NULL;

/***************************************************************************
 */

void
init_help ()
{
  win = newwin (0, 0, 0, 0);
  sub = derwin (win, LINES - 3, COLS, 1, 0);
  /* scrollok (sub, TRUE); */
  print_header ();
}

/***************************************************************************
    The help screen for the index.
 */

void
show_index_help ()
{
  werase (sub);
  wprintw (sub, "q              quit the program\n");
/*
  wprintw (sub, "<Home>         move to the first entry\n");
  wprintw (sub, "<Down>         move to the next entry\n");
  wprintw (sub, "<Up>           move to the previous entry\n");
  wprintw (sub, "<Page Up>      scroll up a page\n");
  wprintw (sub, "<Page Down>    scroll down a page\n");
  wprintw (sub, "<End>          move to the last entry\n");
*/
  wprintw (sub, "<Enter>        view the details of an entry\n");
  wprintw (sub, "g              move to the first entry\n");
  wprintw (sub, "j              move to the next entry\n");
  wprintw (sub, "k              move to the previous entry\n");
  wprintw (sub, "b              scroll up a page\n");
  wprintw (sub, "<Space>        scroll down a page\n");
  wprintw (sub, "G              move to the last entry\n");
  wprintw (sub, "v              view the details of an entry\n");
/* wprintw (sub, "V              view the vCard format of an entry\n"); */
  wprintw (sub, "s              sort the entries in ascending order\n");
/* wprintw (sub, "S              sort the entries in descending order\n"); */
  wprintw (sub, "f              filter the entries\n");
  wprintw (sub, "F              remove the filter\n");
  wprintw (sub, "/              search the index screen with a given string\n");
  wprintw (sub, "n              search forwards for the next search result\n");
  wprintw (sub, "N              search backwards for the next search result\n");
  wprintw (sub, "a              add a new entry using a text editor\n");
/* wprintw (sub, "a              add a new entry using a form\n"); */
  wprintw (sub, "e              edit an entry\n");
  wprintw (sub, "d              delete an entry\n");
/* wprintw(sub, "t              tag/un-tag an entry\n"); */
/* wprintw(sub, "x              export an entry\n"); */
  wprintw (sub,
           "h              show help for key bindings (what you are viewing now)\n");
  print_footer ("Help for the index screen");
  touchwin (win);
  wrefresh (sub);
  wrefresh (win);
  process_help_commands ();
}

/***************************************************************************
    The help screen for viewing entries.
 */

void
show_view_help ()
{
  werase (sub);
  wprintw (sub, "q        exit to index screen\n");
  wprintw (sub, "i        exit to index screen\n");
  wprintw (sub, "<Down>   move to the next entry\n");
  wprintw (sub, "<Up>     move to the previous entry\n");
  wprintw (sub, "<Left>   cycle backwards through the details screen\n");
  wprintw (sub, "<Right>  cycle forwards through the details screen\n");
  wprintw (sub, "j        move to the next entry\n");
  wprintw (sub, "k        move to the previous entry\n");
  wprintw (sub, "e        edit the entry\n");
  wprintw (sub, "1        view the identification details\n");
  wprintw (sub, "2        view the geographic position details\n");
  wprintw (sub, "3        view the telecommunication details\n");
  wprintw (sub, "4        view the organization details\n");
  wprintw (sub, "5        view the miscellaneous details\n");
  wprintw (sub,
           "h        show help for key bindings (what you are viewing now)\n");
  print_footer ("Help for the view screen");
  touchwin (win);
  wrefresh (sub);
  wrefresh (win);
  process_help_commands ();
}

/***************************************************************************
    The help screen for editing entries.
 */

void
show_edit_help ()
{
  werase (sub);
  wprintw (sub, "This is help documentation for edit.");
  print_footer ("Help for the edit screen");
  touchwin (win);
  wrefresh (sub);
  wrefresh (win);
  process_help_commands ();
}

/***************************************************************************
    Prints the header for the help screen.
 */

static void
print_header ()
{
  char *header_str = NULL;
  int i = 0;

  header_str = (char *) malloc (sizeof (char) * (COLS + 2));

  strncpy (header_str, MENU_BAR_STRING, COLS);

  for (i = strlen (MENU_BAR_STRING); i < COLS; i++)
    {
      header_str[i] = ' ';
    }

  header_str[COLS] = '\n';
  header_str[COLS + 1] = '\0';

  wattron (win, A_REVERSE);
  wprintw (win, header_str);
  wstandend (win);
  free (header_str);
}

/***************************************************************************
    Prints the footer for the help screen.
 */

static void
print_footer (const char *text)
{
  char *footer_str = NULL;
  char *help_block = NULL;
  int help_block_len = 0;
  int i = 0;

  footer_str = (char *) malloc (sizeof (char) * (COLS + 2));

  for (i = 0; i < COLS; i++)
    {
      footer_str[i] = '-';
    }

  if (NULL != text)
    {
      help_block_len = strlen (text) + 10;
      help_block = (char *) malloc (sizeof (char) * (help_block_len + 1));

      sprintf (help_block, "---[ %s ]---", text);

      for (i = 0; i < help_block_len; i++)
        {
          footer_str[i] = help_block[i];
        }

      free (help_block);
    }

  footer_str[COLS] = '\n';
  footer_str[COLS + 1] = '\0';

  wattron (win, A_REVERSE);
  mvwprintw (win, LINES - 2, 0, footer_str);
  wstandend (win);
  free (footer_str);
}

/***************************************************************************
    Processes the help screen input commands.
 */

static void
process_help_commands ()
{
  bool done = FALSE;
  int ch = 0;

  while (!done)
    {
      ch = wgetch (sub);

      switch (ch)
        {
        case 'j':
          /* wscrl (sub, 1); */
          wrefresh (sub);
          break;
        case 'k':
          /* wscrl (sub, -1); */
          wrefresh (sub);
          break;
        case 'q':
          done = TRUE;
          break;
        default:
          done = TRUE;
          break;
        }
    }
}
