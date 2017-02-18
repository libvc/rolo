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
 *  $Id: index.c,v 1.19 2003/05/22 08:37:34 ahsu Exp $
 */

#include "entry.h"
#include "index.h"
#include "search.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <vc.h>

#define MENU_BAR_STRING "q:Quit  v:View  e:Edit  a:Add  d:Delete  h:Help"
#ifndef CTRL
#define CTRL(x)  ((x) & 0x1f)
#endif

/*** STATIC PROTOTYPES ***/

static void print_footer (const char *filename, int entries);
static void print_header ();
static void index_search ();
static void index_search_next ();
static void index_search_prev ();

static ITEM **get_items (entry_node ** entries, int count);
static void filter_menu ();
static void unfilter_menu ();
static void sort_ascending (int sort_by);
static void sort_descending (int sort_by);
static int scroll_to_result (ITEM * found_item);
static void clear_message_bar (MENU * m);

/*** STATIC VARIABLES ***/

static void (*display_help) (void);
static MENU *menu = NULL;
static WINDOW *win = NULL;
static WINDOW *sub = NULL;
static const char *datafile = NULL;
static char *filter_string = NULL;
static ITEM **items = NULL;
static g_sort_by = -1;

/***************************************************************************
 */

void
finish_index ()
{
  unpost_menu (menu);
  free_menu (menu);
  menu = NULL;
  free_items (items);
  items = NULL;
}

/***************************************************************************
    Initializes the index window by openning the data file,
    populating the menu items, and setting up the header and footer.
 */

void
init_index (const char *filename)
{
  int count = 0;
  int deleted = 0;
  int rows = 0;
  int cols = 0;
  FILE *fp = NULL;
  entry_node *entries_ll = NULL;
  entry_node **entries_array = NULL;

  /* have to get the win and sub pointers setup early */
  win = stdscr;
  scale_menu (menu, &rows, &cols);      /* get the size required for menu */
  sub = derwin (win, rows, cols, 1, 0);

  /* remember: first node is a dummy node */
  entries_ll = new_entry_node ();

  datafile = filename;
  fp = fopen (datafile, "r");
  if (NULL == fp)
    {
      endwin ();                /* exit gracefully */
      fprintf (stderr, "unable to open file: %s\n", filename);
      exit (1);
    }

  count = count_vcards (fp);

  rewind (fp);
  /* this should get a linked list */
  count = get_entries (fp, entries_ll);
  fclose (fp);

  /* this should reduce the linked list if need be */
  deleted = filter_entries (entries_ll, filter_string);

  count -= deleted;

  /* index the linked list in an array */
  entries_array = entries_lltoa (entries_ll, count);
  free_entry_node (entries_ll);
  entries_ll = NULL;

  /* use the array to sort */
  if (-1 == g_sort_by)
    {
      g_sort_by = SORT_ENTRIES_BY_FAMILY_N;
    }
  sort_entries (entries_array, count, g_sort_by);

  /* construct the items array and cleanup */
  items = get_items (entries_array, count);
  free_entries_array_but_leave_data (entries_array, count);
  entries_array = NULL;

  menu = get_menu (items);

  /* set the menu hook to clear the message bar */
  set_item_init (menu, clear_message_bar);

  /* initialize the menu_userptr to NULL (to be used as search string) */
  set_menu_userptr (menu, NULL);

  print_header ();
  print_footer (filename, count);

  set_menu_win (menu, win);
  set_menu_sub (menu, sub);
  post_menu (menu);
}

/***************************************************************************
 */

static ITEM **
get_items (entry_node ** entries, int count)
{
  int i = 0;
  ITEM **items;
  char *line_number = NULL;

  items = (ITEM **) malloc (sizeof (ITEM *) * (count + 1));

  for (i = 0; i < count; i++)
    {
      line_number = (char *) malloc (sizeof (char) * 4);
      sprintf (line_number, "%3i", (i + 1) % 1000);
      items[i] = new_item (line_number, (entries[i])->description);
      set_item_userptr (items[i], (entries[i])->fpos);
    }

  items[i] = (ITEM *) NULL;

  return items;
}

/***************************************************************************
    Cleans up the two-dimensional array of items.  
 */

void
free_items (ITEM ** items)
{
  char *name = NULL;
  char *description = NULL;
  fpos_t *fpos = NULL;
  int i = 0;

  while ((ITEM *) NULL != items[i])
    {
      name = (char *) item_name (items[i]);
      free (name);

      description = (char *) item_description (items[i]);
      free (description);

      fpos = (fpos_t *) item_userptr (items[i]);
      free (fpos);

      free_item (items[i]);
      i++;
    }

  free (items);
}

/***************************************************************************
    Sets up a new menu.
 */

MENU *
get_menu (ITEM ** items)
{
  MENU *menu = NULL;
  int x = 0;
  int y = 0;

  getmaxyx (win, y, x);
  menu = new_menu (items);
  set_menu_format (menu, y - 3, 1);     /* LINE rows, 1 column */

  return menu;
}

/***************************************************************************
    Displays the header for the index window.
 */

static void
print_header ()
{
  char *header_str = NULL;
  int i = 0;
  int x = 0;
  int y = 0;

  getmaxyx (win, y, x);
  header_str = (char *) malloc (sizeof (char) * (x + 1));

  strncpy (header_str, MENU_BAR_STRING, x);

  for (i = strlen (MENU_BAR_STRING); i < x; i++)
    {
      header_str[i] = ' ';
    }

  header_str[x] = '\0';

  wattron (win, A_REVERSE);
  mvwprintw (win, 0, 0, header_str);
  wstandend (win);
  free (header_str);
}

/***************************************************************************
    Displays a footer that will adjust its contents based on the
    width of the screen.
 */

static void
print_footer (const char *filename, int entries)
{
  char *footer_str = NULL;
  char *rolo_block = NULL;
  int rolo_block_len = 0;
  char *entries_block = NULL;
  int entries_block_len = 0;
  int i = 0;
  int j = 0;
  int x = 0;
  int y = 0;

  getmaxyx (win, y, x);
  footer_str = (char *) malloc (sizeof (char) * (x + 1));

  /*
   * set a default footer of all dashes
   */

  for (i = 0; i < x; i++)
    {
      footer_str[i] = '-';
    }

  rolo_block_len = strlen (filename) + 16;
  entries_block_len = 22;

  /*
   * add the `rolo' block only if there is enough room
   */

  if (rolo_block_len + entries_block_len <= x)
    {
      rolo_block = (char *) malloc (sizeof (char) * (rolo_block_len + 1));
      sprintf (rolo_block, "---[ rolo: %s ]---", filename);

      for (i = 0; i < rolo_block_len; i++)
        {
          footer_str[i] = rolo_block[i];
        }

      free (rolo_block);
    }

  /*
   * add the `entries' block only if there is enough room
   */

  if (entries_block_len <= x)
    {
      entries_block =
          (char *) malloc (sizeof (char) * (entries_block_len + 1));
      sprintf (entries_block, "---[ entries: %i ]---", entries % 1000);

      entries_block_len = strlen (entries_block);
      for (i = x - entries_block_len, j = 0; j < entries_block_len; i++, j++)
        {
          footer_str[i] = entries_block[j];
        }

      free (entries_block);
    }

  footer_str[x] = '\0';

  wattron (win, A_REVERSE);
  mvwprintw (win, LINES - 2, 0, footer_str);
  wstandend (win);
  free (footer_str);
}

/***************************************************************************
    Display the index to the end-user.
 */

void
display_index ()
{
  redrawwin (win);
  wrefresh (win);
}

/***************************************************************************
    Handle input from the end-user.
 */

int
process_index_commands ()
{
  bool done = FALSE;
  int return_command = 0;
  int ch = 0;

  while (!done)
    {
      ch = wgetch (win);

      switch (ch)
        {
        case KEY_NPAGE:
        case ' ':
        case KEY_C3:
          menu_driver (menu, REQ_SCR_DPAGE);
          break;
        case '/':
          index_search ();
          break;
        case 'a':
          return_command = INDEX_COMMAND_ADD;
          clear_message_bar (menu);
          done = TRUE;
          break;
        case KEY_PPAGE:
        case 'b':
        case KEY_A3:
          menu_driver (menu, REQ_SCR_UPAGE);
          break;
        case 'd':
          return_command = INDEX_COMMAND_DELETE;
          clear_message_bar (menu);
          done = TRUE;
          break;
        case 'e':
          return_command = INDEX_COMMAND_EDIT;
          clear_message_bar (menu);
          done = TRUE;
          break;
        case 'f':
          filter_menu ();
          break;
        case 'F':
          unfilter_menu ();
          break;
        case KEY_HOME:
        case 'g':
        case KEY_A1:
          menu_driver (menu, REQ_FIRST_ITEM);
          break;
        case KEY_END:
        case 'G':
        case KEY_C1:
          menu_driver (menu, REQ_LAST_ITEM);
          break;
        case 'h':
          (*display_help) ();
          touchwin (win);
          wrefresh (win);
          wrefresh (sub);
          clear_message_bar (menu);
          break;
        case KEY_DOWN:
        case 'j':
          menu_driver (menu, REQ_NEXT_ITEM);
          break;
        case KEY_UP:
        case 'k':
          menu_driver (menu, REQ_PREV_ITEM);
          break;
        case 'n':
          index_search_next ();
          break;
        case 'N':
          index_search_prev ();
          break;
        case 'q':
          clear_message_bar (menu);
          return_command = INDEX_COMMAND_QUIT;
          done = TRUE;
          break;
        case 's':
          sort_ascending (SORT_ENTRIES_BY_FAMILY_N);
          break;
        case 'S':
          sort_descending (SORT_ENTRIES_BY_FAMILY_N);
          break;
        case 't':
          menu_driver (menu, REQ_TOGGLE_ITEM);
          clear_message_bar (menu);
          break;
        case KEY_ENTER:
        case 13:
        case 'v':
          return_command = INDEX_COMMAND_VIEW;
          clear_message_bar (menu);
          done = TRUE;
          break;
        case 'V':
          return_command = INDEX_COMMAND_RAW_VIEW;
          clear_message_bar (menu);
          done = TRUE;
          break;
        default:
          beep ();
          break;
        }
    }
  return return_command;
}

/***************************************************************************
 */

void
select_next_item ()
{
  menu_driver (menu, REQ_NEXT_ITEM);
}

/***************************************************************************
 */

void
select_previous_item ()
{
  menu_driver (menu, REQ_PREV_ITEM);
}

/***************************************************************************
 */

ITEM *
get_current_item ()
{
  return current_item (menu);
}

/***************************************************************************
 */

int
get_entry_number (const ITEM * item)
{
  return (1 + item_index (item));
}

/***************************************************************************
 */

void
set_index_help_fcn (void (*fcn) (void))
{
  display_help = fcn;
}

/***************************************************************************
 */

static void
sort_ascending (int sort_by)
{
  int ch = 0;
  wmove (win, LINES - 1, 0);
  wclrtoeol (win);
  wprintw (win,
           "Sort ascending by [F]amily Name, [G]iven Name, [E]mail, [T]el: ");
  ch = wgetch (win);

  wmove (win, LINES - 1, 0);
  wclrtoeol (win);

  switch (tolower (ch))
    {
    case 'f':
      g_sort_by = SORT_ENTRIES_BY_FAMILY_N;
      break;
    case 'g':
      g_sort_by = SORT_ENTRIES_BY_GIVEN_N;
      break;
    case 'e':
      g_sort_by = SORT_ENTRIES_BY_EMAIL;
      break;
    case 't':
      g_sort_by = SORT_ENTRIES_BY_TEL;
      break;
    default:
      return;
      break;
    }

  finish_index ();
  init_index (datafile);
  display_index ();
}

/***************************************************************************
 */

static void
sort_descending (int sort_by)
{
  int ch = 0;
  wmove (win, LINES - 1, 0);
  wclrtoeol (win);
  wprintw (win,
           "Sort descending by [F]amily Name, [G]iven Name, [E]mail, [T]el: ");
  ch = wgetch (win);

  wmove (win, LINES - 1, 0);
  wclrtoeol (win);

  switch (tolower (ch))
    {
    case 'f':
      g_sort_by = SORT_ENTRIES_BY_FAMILY_N;
      break;
    case 'g':
      g_sort_by = SORT_ENTRIES_BY_GIVEN_N;
      break;
    case 'e':
      g_sort_by = SORT_ENTRIES_BY_EMAIL;
      break;
    case 't':
      g_sort_by = SORT_ENTRIES_BY_TEL;
      break;
    default:
      return;
      break;
    }

  finish_index ();
  init_index (datafile);
  display_index ();
}

/***************************************************************************
 */

void
refresh_index ()
{
  int count = 0;
  int current_index = 0;
  ITEM *item = NULL;
  ITEM **items = NULL;

  item = current_item (menu);

  if (NULL == item)
    {
      finish_index ();
      init_index (datafile);
      display_index ();
      return;
    }

  current_index = item_index (item);

  finish_index ();
  init_index (datafile);
  display_index ();

  count = item_count (menu);

  items = menu_items (menu);
  current_index = current_index >= count ? count : current_index;
  if (items != NULL)
    set_current_item (menu, items[current_index]);
}

/***************************************************************************
 */

static void
unfilter_menu ()
{
  if (NULL != filter_string)
    {
      free (filter_string);
    }

  filter_string = NULL;

  finish_index ();
  init_index (datafile);
  display_index ();
}

/***************************************************************************
 */

static void
filter_menu ()
{
  char f_string[80];
  ITEM *found_item = NULL;

  wmove (win, LINES - 1, 0);
  wclrtoeol (win);
  wprintw (win, "Filter: ");
  nocbreak ();
  echo ();
  wscanw (win, "%s", f_string);

  cbreak ();
  noecho ();
  wmove (win, LINES - 1, 0);
  wclrtoeol (win);

  if (NULL != filter_string)
    {
      free (filter_string);
    }

  filter_string = strdup (f_string);

  finish_index ();
  init_index (datafile);
  display_index ();
}

/***************************************************************************
    Search for the entered search string in the index index screen.
    This will move the cursor and print a status message line at
    the bottom of the screen.
 */

static void
index_search ()
{
  char search_string[80];

  wmove (win, LINES - 1, 0);
  wclrtoeol (win);
  wprintw (win, "Search for: ");
  nocbreak ();
  echo ();
  wscanw (win, "%s", search_string);
  cbreak ();
  noecho ();

  set_menu_search_string (menu, search_string);
  index_search_next ();
}

/***************************************************************************
 */

static int
scroll_to_result (ITEM * found_item)
{
  int direction = 0;

  if (NULL != found_item)
    {
      int current_index = -1;
      int found_index = -1;

      current_index = item_index (current_item (menu));
      found_index = item_index (found_item);

      direction = found_index > current_index ? REQ_SCR_DPAGE : REQ_SCR_UPAGE;

      while (FALSE == item_visible (found_item))
        {
          menu_driver (menu, direction);
        }

      set_current_item (menu, found_item);
    }

  return direction;
}

/***************************************************************************
 */

static void
clear_message_bar (MENU * m)
{
  WINDOW *w = NULL;

  w = menu_win (m);

  wmove (w, LINES - 1, 0);
  wclrtoeol (w);
}

/***************************************************************************
 */

static void
index_search_next ()
{
  ITEM *found_item = NULL;
  int direction = 0;

  found_item = search_menu (menu, REQ_SCR_DPAGE);
  direction = scroll_to_result (found_item);
  switch (direction)
    {
    case 0:
      wmove (win, LINES - 1, 0);
      wclrtoeol (win);
      mvwprintw (win, LINES - 1, 0, "Not found.");
      beep ();
      break;

    case REQ_SCR_UPAGE:
      mvwprintw (win, LINES - 1, 0, "Search wrapped to top.");
      break;

    default:
      break;
    }
}

/***************************************************************************
 */

static void
index_search_prev ()
{
  ITEM *found_item = NULL;
  int direction = 0;

  found_item = search_menu (menu, REQ_SCR_UPAGE);
  direction = scroll_to_result (found_item);
  switch (direction)
    {
    case 0:
      wmove (win, LINES - 1, 0);
      wclrtoeol (win);
      mvwprintw (win, LINES - 1, 0, "Not found.");
      beep ();
      break;

    case REQ_SCR_DPAGE:
      mvwprintw (win, LINES - 1, 0, "Search wrapped to bottom.");
      break;

    default:
      break;
    }
}
