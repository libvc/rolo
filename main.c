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
 * $Id: main.c,v 1.3 2003/02/17 08:15:35 ahsu Exp $
 */

#include "vcard.h"
#include "view.h"
#include "edit.h"
#include "index.h"
#include "help.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <menu.h>
#include <assert.h>

#define ROLO_VERSION_STRING "001"

/*** GLOBALS ***/

enum win_states { WINDOW_INDEX, WINDOW_VIEW, WINDOW_EDIT };
char data_path[PATH_MAX];

/*** PROTOTYPES ***/

static void finish(int sig);
static void resize(int sig);
static void set_defaults();
static void process_command_line_args(int argc, char *const *argv);
static void display_usage(const char *prog_name);
static void display_version();
static void set_contacts_file();
static void display_license();

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

static void
set_defaults()
{
  strcpy(data_path, "data.vcf");
}

static void
display_usage(const char *prog_name)
{
  printf("usage: %s [-r] [-f <file>]\n", prog_name);
  printf("       %s -v\n", prog_name);
  printf("       %s -V\n", prog_name);
  printf("       %s -h\n", prog_name);
  printf("options:\n");
  /*
   * do not show options that are not implemented: 
   */
  /*
   * printf("  -r            open the contact file as read-only\n"); 
   */
  printf("  -f <file>     specify a contact file to use\n");
  printf("  -v            display rolo version\n");
  printf("  -V            display copyright and license\n");
  printf("  -h            this help message\n");
}

static void
display_version()
{
  printf("rolo version %s\n", ROLO_VERSION_STRING);
}

static void
display_license()
{
  printf("rolo - contact management software\n");
  printf("Copyright (C) 2003  Andrew Hsu\n");
  printf("\n");
  printf("This program is free software;");
  printf(" you can redistribute it and/or modify\n");
  printf("it under the terms of the");
  printf(" GNU General Public License as published by\n");
  printf("the Free Software Foundation;");
  printf(" either version 2 of the License, or\n");
  printf("(at your option) any later version.\n");
  printf("\n");
  printf("This program is distributed");
  printf(" in the hope that it will be useful,\n");
  printf("but WITHOUT ANY WARRANTY;");
  printf(" without even the implied warranty of\n");
  printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR");
  printf(" PURPOSE.  See the\n");
  printf("GNU General Public License for more details.\n");
  printf("\n");
  printf("You should have received a copy of");
  printf(" the GNU General Public License\n");
  printf("along with this program;");
  printf(" if not, write to the Free Software\n");
  printf("Foundation, Inc., 59 Temple Place, Suite 330,");
  printf(" Boston, MA  02111-1307  USA\n");
}

static void
set_contacts_file()
{
  strncpy(data_path, optarg, PATH_MAX);
  data_path[PATH_MAX - 1] = '\0';
}

static void
process_command_line_args(int argc, char *const *argv)
{
  int ch = -1;

  while (-1 != (ch = getopt(argc, argv, "rf:vVh"))) {
    switch (ch) {
    case 'r':
      /*
       * todo: implement read-only option 
       */
      break;
    case 'f':
      set_contacts_file();
      break;
    case 'v':
      display_version();
      exit(0);
      break;
    case 'V':
      display_license();
      exit(0);
      break;
    case 'h':
    case '?':
    default:
      display_usage(argv[0]);
      exit(0);
    }
  }
}

int
main(int argc, char *argv[])
{
  vcard *v = NULL;
  fpos_t *fpos = NULL;
  FILE *fp = NULL;
  ITEM *it = NULL;

  bool done = FALSE;
  int win_state = WINDOW_INDEX;
  int command = 0;

  set_defaults();
  process_command_line_args(argc, argv);
  /*
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

  init_index(data_path);
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

      fp = fopen(data_path, "r");
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
  return (0);
}
