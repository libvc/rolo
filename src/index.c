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
 * $Id: index.c,v 1.3 2003/03/02 19:32:52 ahsu Exp $
 */

#include "index.h"
#include <vcard.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define HARD_CODED_HEADER_STR "q:Quit  v:View  h:Help"
#define MENU_PRINT_FORMAT_SIZE 38
#ifndef CTRL
#define CTRL(x)         ((x) & 0x1f)
#endif

/*** STRUCTS ***/

struct entry_node_tag
{
  char *description;
  fpos_t *fpos;
  struct entry_node_tag *next;
};

typedef struct entry_node_tag entry_node;

/*** STATIC PROTOTYPES ***/

static char *construct_menu_name (const char *family_name,
                                  const char *given_name, const char *email,
                                  const char *tel);
static void print_footer (const char *filename, int entries);
static void print_header ();
static void search_menu ();
static ITEM *search_items (const char *search_string);
static void set_menu_print_format (char *menu_print_format, int width);

static int filter_entries (entry_node * entries_ll, const char *filter_string);
static void free_entry_node (entry_node * e);
static int cmp_desc_by_family_n (const void *a, const void *b);
static void sort_entries (entry_node ** entries, int count, int sort_by);
static entry_node **entries_lltoa (entry_node * entries_ll, int count);
static ITEM **get_items (entry_node ** entries, int count);
static int get_entries (FILE * fp, entry_node * entries_ll);
static entry_node *new_entry_node ();
static void filter_menu ();
static void unfilter_menu ();
static void sort_ascending (int sort_by);
static void sort_descending (int sort_by);
static void free_entries_array_but_leave_data (entry_node ** entries_array,
                                               int count);

/*** STATIC VARIABLES ***/

static void (*display_help) (void);
static MENU *menu = NULL;
static WINDOW *win = NULL;
static WINDOW *sub = NULL;
static const char *datafile = NULL;
static char *filter_string = NULL;
static ITEM **items = NULL;

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
 */

static int
cmp_desc_by_family_n (const void *a, const void *b)
{
  int ret_val = 0;
  entry_node **ena = NULL;
  entry_node **enb = NULL;

  ena = (entry_node **) a;
  enb = (entry_node **) b;

  ret_val = strcmp ((*ena)->description, (*enb)->description);

  return ret_val;
}

/***************************************************************************
    FIXME: Implement the rest of the sort_by types.
 */

static void
sort_entries (entry_node ** entries, int count, int sort_by)
{
  switch (sort_by)
    {
    case SORT_ENTRIES_BY_FAMILY_N:
      qsort (entries, count, sizeof (entry_node *), cmp_desc_by_family_n);
      break;
    case SORT_ENTRIES_BY_GIVEN_N:
      break;
    case SORT_ENTRIES_BY_EMAIL:
      break;
    case SORT_ENTRIES_BY_TEL:
      break;
    default:
      break;
    }
}

/***************************************************************************
    Filters the given linked list of entries by keeping only those
    nodes that match the filter_string.  Returns the number of nodes
    that are left out.
 */

static int
filter_entries (entry_node * entries_ll, const char *filter_string)
{
  int count = 0;
  entry_node *e = NULL;
  entry_node *prev = NULL;

  /* first check if there is a filter string */
  if (NULL != filter_string)
    {
      /* remember: the first node is a dummy node */
      prev = entries_ll;
      e = entries_ll->next;

      while (e != NULL)
        {
          if (NULL == strstr (e->description, filter_string))
            {
              /* must delete node */
              prev->next = e->next;
              free_entry_node (e);
              e = prev->next;
              count++;
            }
          else
            {
              /* skip node, move on */
              prev = e;
              e = e->next;
            }
        }
    }

  /* the number of deleted nodes */
  return count;
}

/***************************************************************************
 */

static entry_node **
entries_lltoa (entry_node * entries_ll, int count)
{
  entry_node **entries_a = NULL;
  entry_node *e = NULL;
  int i = 0;

  entries_a = (entry_node **) malloc (sizeof (entry_node *) * count);

  for (i = 0, e = entries_ll->next; i < count; i++)
    {
      entries_a[i] = e;
      e = e->next;
    }

  return entries_a;
}

/***************************************************************************
 */

static void
free_entries_array_but_leave_data (entry_node ** entries_array, int count)
{
  int i = 0;
  for (i = 0; i < count; i++)
    {
      free (entries_array[i]);
    }

  free (entries_array);
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
  sort_entries (entries_array, count, SORT_ENTRIES_BY_FAMILY_N);

  /* construct the items array and cleanup */
  items = get_items (entries_array, count);
  free_entries_array_but_leave_data (entries_array, count);
  entries_array = NULL;

  menu = get_menu (items);

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
    Frees an entry node.  The user of this function must also
    remember to set the pointer to NULL after calling this function
    for cleanliness.
 */

static void
free_entry_node (entry_node * e)
{
  if (NULL != e)
    {
      free (e->description);
      free (e->fpos);
      free (e);
    }
}

/***************************************************************************
    Allocates memory for an entry node and initialises the pointers
    in the structure to NULL.
 */

static entry_node *
new_entry_node ()
{
  entry_node *e = NULL;

  e = (entry_node *) malloc (sizeof (entry_node));

  if (NULL == e)
    {
      endwin ();
      fprintf (stderr, "unable to malloc an entry_node\n");
      exit (1);
    }
  else
    {
      e->description = NULL;
      e->fpos = NULL;
      e->next = NULL;
    }

  return e;
}

/***************************************************************************
    Parses the data file for entries one at a time and stuffs them
    into a linked list.
 */

static int
get_entries (FILE * fp, entry_node * entries_ll)
{
  int i = 0;
  vcard *v = NULL;
  vcard_item *vi = NULL;
  char *email = NULL;
  char *tel = NULL;
  char *family_name = NULL;
  char *given_name = NULL;
  char *desc = NULL;
  int count = 0;
  entry_node *entry = NULL;
  fpos_t *fpos = NULL;

  fpos = (fpos_t *) malloc (sizeof (fpos_t));
  fgetpos (fp, fpos);
  v = parse_vcard_file (fp);

  while (NULL != v)
    {
      count++;
      entry = new_entry_node ();

      entry->next = entries_ll->next;
      entries_ll->next = entry;

      entry->fpos = fpos;
      fpos = NULL;

      /*
       * retrive the name value
       */

      vi = get_vcard_item_by_name (v, VC_NAME);
      family_name = get_val_struct_part (get_vcard_item_value (vi), N_FAMILY);
      given_name = get_val_struct_part (get_vcard_item_value (vi), N_GIVEN);

      /*
       * retrive the email value
       */

      vi = get_vcard_item_by_name (v, VC_EMAIL);
      email = get_vcard_item_value (vi);

      /*
       * retrive the telephone value
       */

      vi = get_vcard_item_by_name (v, VC_TELEPHONE);
      tel = get_vcard_item_value (vi);

      /*
       * put them together
       */

      desc = construct_menu_name (family_name, given_name, email, tel);
      entry->description = desc;

      /*
       * cleanup
       */

      free (family_name);
      free (given_name);
      family_name = NULL;
      given_name = NULL;
      delete_vcard (v);
      v = NULL;

      fpos = (fpos_t *) malloc (sizeof (fpos_t));
      fgetpos (fp, fpos);
      v = parse_vcard_file (fp);
    }

  return count;
}

/***************************************************************************
    Constructs a string containing the description part of a
    menu_item.  The user of this function must remember to free the
    returned string at some point in the future.
 */

static char *
construct_menu_name (const char *family_name, const char *given_name,
                     const char *email, const char *tel)
{
  char *menu_name = NULL;
  char menu_print_format[MENU_PRINT_FORMAT_SIZE];
  int x = 0;
  int y = 0;

  getmaxyx (win, y, x);
  menu_name = (char *) malloc (sizeof (char) * (x - 5 + 1));

  set_menu_print_format (menu_print_format, x);
  sprintf (menu_name, menu_print_format,
           family_name ? family_name : "",
           given_name ? given_name : "", email ? email : "", tel ? tel : "");

  return menu_name;
}

/***************************************************************************
    Sets the print format string given a width.
 */

static void
set_menu_print_format (char *menu_print_format, int width)
{
  /* 75 characters long -- fits into 80 character wide screen */
  strcpy (menu_print_format, "%-12.12s %-12.12s %-30.30s %-18.18s");
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

  getmaxyx(win, y, x);
  menu = new_menu (items);
  set_menu_format (menu, y - 3, 1); /* LINE rows, 1 column */

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
  header_str = (char *) malloc (sizeof (char) * (x + 2));

  strncpy (header_str, HARD_CODED_HEADER_STR, x);

  for (i = strlen (HARD_CODED_HEADER_STR); i < x; i++)
    {
      header_str[i] = ' ';
    }

  header_str[x] = '\n';
  header_str[x + 1] = '\0';

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
  footer_str = (char *) malloc (sizeof (char) * (x + 2));

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

  footer_str[x] = '\n';
  footer_str[x + 1] = '\0';

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
          search_menu ();
          break;
        case KEY_PPAGE:
        case 'b':
        case KEY_A3:
          menu_driver (menu, REQ_SCR_UPAGE);
          break;
        case 'e':
          return_command = INDEX_COMMAND_EDIT;
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
          menu_driver (menu, REQ_NEXT_MATCH);
          break;
        case 'N':
          menu_driver (menu, REQ_PREV_MATCH);
          break;
        case 'q':
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
          break;
        case KEY_ENTER:
        case 13:
        case 'v':
          return_command = INDEX_COMMAND_VIEW;
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
  wmove (win, LINES - 1, 0);
  wclrtoeol (win);
  wprintw (win,
           "Sort ascending by [F]amily Name, [G]iven Name, [E]mail, [T]el: ");
  getch ();

  wmove (win, LINES - 1, 0);
  wclrtoeol (win);
}

/***************************************************************************
 */

static void
sort_descending (int sort_by)
{
  wmove (win, LINES - 1, 0);
  wclrtoeol (win);
  wprintw (win,
           "Sort descending by [F]amily Name, [G]iven Name, [E]mail, [T]el: ");
  getch ();

  wmove (win, LINES - 1, 0);
  wclrtoeol (win);
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
search_menu ()
{
  char search_string[80];
  ITEM *found_item = NULL;

  wmove (win, LINES - 1, 0);
  wclrtoeol (win);
  wprintw (win, "Search for: ");
  nocbreak ();
  echo ();
  wscanw (win, "%s", search_string);

  found_item = search_items (search_string);
  if (found_item != NULL)
    {
      set_current_item (menu, found_item);
    }

  cbreak ();
  noecho ();
  wmove (win, LINES - 1, 0);
  wclrtoeol (win);
}

/***************************************************************************
    Perform the search for an item with a given search string.
 */

static ITEM *
search_items (const char *search_string)
{
  ITEM *result_entry = NULL;
  int i = 0;
  ITEM **items = NULL;
  char *found_string = NULL;
  bool done = FALSE;

  i = item_index (current_item (menu));
  items = menu_items (menu);

  while (!done && (items[i] != NULL))
    {
      found_string = strstr (item_description (items[i]), search_string);

      if (found_string != NULL)
        {
          result_entry = items[i];
          done = TRUE;
        }

      i++;
    }

  return result_entry;
}
