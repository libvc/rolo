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

#include "vcard.h"
#include "view.h"
#include "edit.h"
#include "index.h"
#include "help.h"
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <menu.h>
#include <assert.h>

/*** GLOBALS ***/

enum win_states { WINDOW_INDEX, WINDOW_VIEW, WINDOW_EDIT };

/*** PROTOTYPES ***/

static void finish(int sig);
static void resize(int sig);

static void
resize(int sig)
{
  endwin();
  exit(0);
}

static void
finish(int sig)
{
  endwin();
  exit(0);
}

int
main(int argc, char *argv[])
{
  vcard *v = NULL;
  char filename[80];
  fpos_t *fpos = NULL;
  FILE *fp = NULL;
  ITEM *it = NULL;

  bool done = FALSE;
  int win_state = WINDOW_INDEX;
  int command = 0;

  /*
   * process_command_line_arguments(); 
   * process_environment_variables(); 
   * process_configuration_file(); 
   */

  signal(SIGINT, finish);       /* catch interrupt for exiting */
  signal(SIGWINCH, resize);     /* catch interrupt for resizing */
  initscr();

  keypad(stdscr, TRUE);         /* enable keypad for use of arrow keys */
  nonl();                       /* tell curses not to do NL->CR/NL on output */
  cbreak();                     /* take input chars immediately */
  noecho();

  strcpy(filename, "data.vcf");
  init_index(filename);
  set_index_help_fcn(show_index_help);
  init_view();
  set_view_help_fcn(show_view_help);
  init_edit();
  set_edit_help_fcn(show_edit_help);
  init_help();

  while (!done) {
    switch (win_state) {
    case WINDOW_INDEX:

      /*-------------------
         display the index
        -------------------*/

      display_index();
      command = process_index_commands();

      switch (command) {
      case INDEX_COMMAND_VIEW:
        win_state = WINDOW_VIEW;
        break;
      case INDEX_COMMAND_EDIT:
        win_state = WINDOW_EDIT;
        break;
      case INDEX_COMMAND_QUIT:
        done = TRUE;
        break;
      default:
        break;
      }

      break;

    case WINDOW_VIEW:

      /*----------------------------------
         view the currently selected item
        ----------------------------------*/

      it = get_current_item();
      fpos = (fpos_t *) item_userptr(it);

      fp = fopen(filename, "r");
      fsetpos(fp, fpos);
      v = parse_vcard_file(fp);
      fclose(fp);

      if (v != NULL) {
        view_vcard(v);
        command = process_view_commands();

        switch (command) {
        case VIEW_COMMAND_EDIT:
          win_state = WINDOW_EDIT;
          break;
        case VIEW_COMMAND_INDEX:
          win_state = WINDOW_INDEX;
          break;
        case VIEW_COMMAND_PREVIOUS:
          select_previous_item();
          win_state = WINDOW_VIEW;
          break;
        case VIEW_COMMAND_NEXT:
          select_next_item();
          win_state = WINDOW_VIEW;
          break;
        default:
          break;
        }
      } else {
        win_state = WINDOW_INDEX;
      }

      delete_vcard(v);
      v = NULL;

      break;

    case WINDOW_EDIT:

      /*--------------
         edit a vcard
        --------------*/

      edit_vcard(v);
      command = process_edit_commands();

      switch (command) {
      case EDIT_COMMAND_INDEX:
        win_state = WINDOW_INDEX;
        break;
      default:
        break;
      }

      break;

    default:
      break;

    }
  }

  finish(0);
  exit(EXIT_SUCCESS);
  return(0);
}
