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
 * $Id: index.c,v 1.2 2003/02/18 10:42:33 ahsu Exp $
 */

#include "index.h"
#include "vcard.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define HARD_CODED_HEADER_STR "q:Quit  v:View  h:Help"

/*** PROTOTYPES ***/
static char *construct_menu_name(const char *family_name,
    const char *given_name, const char *email, const char *tel);
static void print_footer(const char *filename, int entries);
static void print_header();
static void search_menu();
static ITEM *search_items(const char *search_string);

static void (*display_help) (void);
static MENU *menu = NULL;
static WINDOW *win = NULL;
static WINDOW *sub = NULL;

void
init_index(const char *filename)
{
  ITEM **items = NULL;
  int count = 0;
  int rows;
  int cols;
  FILE *fp = NULL;


  fp = fopen(filename, "r");
  if (NULL == fp) {
    fprintf(stderr, "unable to open file: %s\n", filename);
    endwin();                   /* exit gracefully */
    exit(1);
  }

  count = count_vcards(fp);
  rewind(fp);
  items = get_items(fp, count);
  fclose(fp);
  menu = get_menu(items);

  win = stdscr;
  scale_menu(menu, &rows, &cols);       /* get the size required for menu */
  sub = derwin(win, rows, cols, 1, 0);

  print_header();
  print_footer(filename, count);

  set_menu_win(menu, win);
  set_menu_sub(menu, sub);
  post_menu(menu);
}

void
free_items(ITEM ** items)
{
  char *filename = NULL;
  char *menuline = NULL;
  int i = 0;

  while ((ITEM *) NULL != items[i]) {
    filename = (char *)item_userptr(items[i]);
    free(filename);

    menuline = (char *)item_name(items[i]);
    free(menuline);

    free_item(items[i]);
    i++;
  }

  free(items);
}

ITEM **
get_items(FILE * fp, int count)
{
  int i = 0;
  vcard *v = NULL;
  vcard_item *vi = NULL;
  char *email = NULL;
  char *tel = NULL;
  char *family_name = NULL;
  char *given_name = NULL;
  char *menuline = NULL;
  char *line_number = NULL;
  ITEM **items = NULL;
  fpos_t *fpos = NULL;

  items = (ITEM **) malloc(sizeof(ITEM *) * (count + 1));

  for (i = 0; i < count; i++) {
    fpos = (fpos_t *) malloc(sizeof(fpos_t));
    fgetpos(fp, fpos);
    v = parse_vcard_file(fp);

      /*------------------------
         retrive the name value
        ------------------------*/

    vi = get_vcard_item_by_name(v, VC_NAME);
    family_name = get_val_struct_part(get_vcard_item_value(vi), N_FAMILY);
    given_name = get_val_struct_part(get_vcard_item_value(vi), N_GIVEN);

      /*-------------------------
         retrive the email value
        -------------------------*/

    vi = get_vcard_item_by_name(v, VC_EMAIL);
    email = get_vcard_item_value(vi);

      /*-----------------------------
         retrive the telephone value
        -----------------------------*/

    vi = get_vcard_item_by_name(v, VC_TELEPHONE);
    tel = get_vcard_item_value(vi);

      /*-------------------
         put them together
        -------------------*/

    line_number = (char *)malloc(sizeof(char) * 4);
    sprintf(line_number, "%3i", i + 1);
    menuline = construct_menu_name(family_name, given_name, email, tel);
    items[i] = new_item(line_number, menuline);

    set_item_userptr(items[i], fpos);

      /*---------
         cleanup
        ---------*/

    free(family_name);
    free(given_name);
    family_name = NULL;
    given_name = NULL;
    delete_vcard(v);
    v = NULL;
  }

  items[i] = (ITEM *) NULL;

  return items;
}

static char *
construct_menu_name(const char *family_name, const char *given_name,
    const char *email, const char *tel)
{
  char *menu_name;

  menu_name = (char *)malloc(sizeof(char) * 79);

  sprintf(menu_name, "%-10.10s %-10.10s %-30.30s %-17.17s",
      family_name ? family_name : "",
      given_name ? given_name : "", email ? email : "", tel ? tel : "");

  return menu_name;
}

MENU *
get_menu(ITEM ** items)
{
  MENU *menu;

  menu = new_menu(items);
  set_menu_format(menu, LINES - 3, 1);  /* LINE rows, 1 column */

  return menu;
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
print_footer(const char *filename, int entries)
{
  char *footer_str = NULL;
  int i = 0;

  footer_str = (char *)malloc(sizeof(char) * (COLS + 2));

  for (i = 0; i < COLS; i++)
  {
    footer_str[i] = '-';
  }

  /*
     ("---[ rolo: %s ]-----[ entries: %i ]---\n", filename, entries);
   */

  footer_str[COLS] = '\n';
  footer_str[COLS + 1] = '\0';

  wattron(win, A_REVERSE);
  mvwprintw(win, LINES - 2, 0, footer_str);
  wstandend(win);
  free(footer_str);
}

void
display_index()
{
  redrawwin(win);
  wrefresh(win);
}

int
process_index_commands()
{
  bool done = FALSE;
  int return_command = 0;
  int ch = 0;

  while (!done) {
    ch = getch();

    switch (ch) {
    case KEY_HOME:
      menu_driver(menu, REQ_FIRST_ITEM);
      break;
    case KEY_END:
      menu_driver(menu, REQ_LAST_ITEM);
      break;
    case KEY_NPAGE:
    case ' ':
      menu_driver(menu, REQ_SCR_UPAGE);
      break;
    case KEY_PPAGE:
    case 'b':
      menu_driver(menu, REQ_SCR_DPAGE);
      break;
    case KEY_DOWN:
    case 'j':
      menu_driver(menu, REQ_NEXT_ITEM);
      break;
    case KEY_UP:
    case 'k':
      menu_driver(menu, REQ_PREV_ITEM);
      break;
    case 't':
      menu_driver(menu, REQ_TOGGLE_ITEM);
      break;
    case KEY_ENTER:
    case 13:
    case 'v':
      return_command = INDEX_COMMAND_VIEW;
      done = TRUE;
      break;
    case 'e':
      return_command = INDEX_COMMAND_EDIT;
      done = TRUE;
      break;
    case '/':
      search_menu();
      break;
    case 'n':
      menu_driver(menu, REQ_NEXT_MATCH);
      break;
    case 'N':
      menu_driver(menu, REQ_PREV_MATCH);
      break;
    case 'h':
      (*display_help) ();
      touchwin(win);
      wrefresh(win);
      wrefresh(sub);
      break;
    case 'q':
      return_command = INDEX_COMMAND_QUIT;
      done = TRUE;
      break;
    default:
      beep();
      break;
    }
  }
  return return_command;
}

void
select_next_item()
{
  menu_driver(menu, REQ_NEXT_ITEM);
}

void
select_previous_item()
{
  menu_driver(menu, REQ_PREV_ITEM);
}

ITEM *
get_current_item()
{
  return current_item(menu);
}

int
get_entry_number(const ITEM *item)
{
  return (1 + item_index(item));
}

void
set_index_help_fcn(void (*fcn) (void))
{
  display_help = fcn;
}

/* ---------------------------------------------------------
    search for the entered search string in the index
    index screen.  this will move the cursor and print a
    status message line at the bottom of the screen.
 */

static void
search_menu()
{
  char search_string[80];
  ITEM *found_item = NULL;

  wmove(win, LINES - 1, 0);
  wclrtoeol(win);
  wprintw(win, "Search for: ");
  nocbreak();
  echo();
  wscanw(win, "%s", search_string);

  found_item = search_items(search_string);
  if (found_item != NULL) {
    set_current_item(menu, found_item);
  }

  cbreak();
  noecho();
  wmove(win, LINES - 1, 0);
  wclrtoeol(win);
}

static ITEM *
search_items(const char *search_string)
{
  ITEM *result_entry = NULL;
  int i = 0;
  ITEM **items = NULL;
  char *found_string = NULL;
  bool done = FALSE;

  i = item_index(current_item(menu));
  items = menu_items(menu);

  while (!done && (items[i] != NULL)) {
    found_string = strstr(item_description(items[i]), search_string);

    if (found_string != NULL) {
      result_entry = items[i];
      done = TRUE;
    }

    i++;
  }

  return result_entry;
}
