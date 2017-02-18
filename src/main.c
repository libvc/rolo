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
 *  $Id$
 */

#include <vc.h>
#include "add.h"
#include "view.h"
#include "delete.h"
#include "edit.h"
#include "index.h"
#include "help.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <menu.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <locale.h>

#if HAVE_CONFIG_H
#include "config.h"
#else
#define PACKAGE_STRING "rolo"
#endif

#define DEFAULT_HOME_ROLO_DIR ".rolo"
#define DEFAULT_FILENAME "contacts.vcf"

/*** GLOBALS ***/

enum win_states
{ WINDOW_INDEX, WINDOW_VIEW, WINDOW_RAW_VIEW, WINDOW_EDIT, WINDOW_DELETE,
  WINDOW_ADD
};
char data_path[PATH_MAX];

/*** PROTOTYPES ***/

static void finish (int sig);
static void resize (int sig);
static void set_defaults ();
static void process_command_line_args (int argc, char *const *argv);
static void display_usage (const char *prog_name);
static void display_version ();
static void set_contacts_file ();
static void display_license ();
static char *get_env_editor ();

/***************************************************************************
    Gets the editor to invoke from the environment settings.  If
    none of the environment settings are set, then the default value
    of `vi' will be returned.  The user of this function must
    remember to free the returned string.
 */

static char *
get_env_editor ()
{
  char *editor = NULL;

  editor = getenv ("ROLO_EDITOR");

  if (NULL == editor)
    {
      editor = getenv ("EDITOR");
      if (NULL == editor)
        {
          editor = strdup ("/usr/bin/editor");
          return editor;
        }
    }

  return strdup (editor);
}

/***************************************************************************
    This is called upon when the window is resized.
 */

static void
resize (int sig)
{
  finish_index ();
  endwin ();
  refresh ();
  initscr ();

  keypad (stdscr, TRUE);        /* enable keypad for use of arrow keys */
  nonl ();                      /* tell curses not to do NL->CR/NL on output */
  cbreak ();                    /* take input chars immediately */
  noecho ();

  init_index (data_path);
  display_index ();
  refresh ();
}

/***************************************************************************
    This is called upon when the program is asked to finish.
 */

static void
finish (int sig)
{
  finish_index ();
  endwin ();
  exit (0);
}

/***************************************************************************
    Sets the default program startup values.
 */

static void
set_defaults ()
{
  char default_datafile[PATH_MAX];
  char *home = NULL;
  char *editor = NULL;

  if (strlen (data_path) == 0) {
    home = getenv ("HOME");
    if (NULL != home)
      {
        int result = 1;
        struct stat sb;

        strcpy (default_datafile, home);
        result = stat (default_datafile, &sb);
        if (-1 == result)
          {
            fprintf (stderr, "home directory unavailable\n");
            exit (1);
          }

        strncat (default_datafile, "/", 1);
        strncat (default_datafile, DEFAULT_HOME_ROLO_DIR,
                 strlen (DEFAULT_HOME_ROLO_DIR));
        result = stat (default_datafile, &sb);
        if (-1 == result)
          {
            if (ENOENT == errno)
              {
                mkdir (default_datafile, S_IRWXU);
              }
            else
              {
                exit (1);
              }
          }

        strncat (default_datafile, "/", 1);
        strncat (default_datafile, DEFAULT_FILENAME, strlen (DEFAULT_FILENAME));
        result = stat (default_datafile, &sb);
        if (-1 == result)
          {
            if (ENOENT == errno)
              {
                FILE *fp;

                fp = fopen (default_datafile, "w");
                if (NULL == fp) {
                  fprintf (stderr, "unable to open file %s\n", default_datafile);
                  exit (1);
                }
                fclose (fp);
              }
            else
              {
                exit (1);
              }
          }
      }
    else
      {
        fprintf (stderr, "unable to deterime home directory");
        exit (1);
      }

    strcpy (data_path, default_datafile);
  }

  editor = get_env_editor ();

  set_edit_editor (editor);
  set_add_editor (editor);
}

/***************************************************************************
    Ouputs how to use the program.
 */

static void
display_usage (const char *prog_name)
{
  printf ("usage: %s [-r] [-f <file>]\n", prog_name);
  printf ("       %s -v\n", prog_name);
  printf ("       %s -V\n", prog_name);
  printf ("       %s -h\n", prog_name);
  printf ("options:\n");
  printf ("  -r            open the contact file as read-only\n");
  printf ("  -f <file>     specify a contact file to use\n");
  printf ("  -v            display version\n");
  printf ("  -V            display copyright and license\n");
  printf ("  -h            this help message\n");
}

/***************************************************************************
    Outputs a one-line version statement.
 */

static void
display_version ()
{
  printf ("%s\n", PACKAGE_STRING);
}

/***************************************************************************
    Outputs the software license.
 */

static void
display_license ()
{
  printf ("rolo - contact management software\n");
  printf ("Copyright (C) 2003  Andrew Hsu\n");
  printf ("\n");
  printf ("This program is free software;");
  printf (" you can redistribute it and/or modify\n");
  printf ("it under the terms of the");
  printf (" GNU General Public License as published by\n");
  printf ("the Free Software Foundation;");
  printf (" either version 2 of the License, or\n");
  printf ("(at your option) any later version.\n");
  printf ("\n");
  printf ("This program is distributed");
  printf (" in the hope that it will be useful,\n");
  printf ("but WITHOUT ANY WARRANTY;");
  printf (" without even the implied warranty of\n");
  printf ("MERCHANTABILITY or FITNESS FOR A PARTICULAR");
  printf (" PURPOSE.  See the\n");
  printf ("GNU General Public License for more details.\n");
  printf ("\n");
  printf ("You should have received a copy of");
  printf (" the GNU General Public License\n");
  printf ("along with this program;");
  printf (" if not, write to the Free Software\n");
  printf ("Foundation, Inc., 59 Temple Place, Suite 330,");
  printf (" Boston, MA  02111-1307  USA\n");
}

/***************************************************************************
    Helper function for setting the contact file.
 */

static void
set_contacts_file ()
{
  strncpy (data_path, optarg, PATH_MAX);
  data_path[PATH_MAX - 1] = '\0';
}

/***************************************************************************
    Parses the command-line arguments.
 */

static void
process_command_line_args (int argc, char *const *argv)
{
  int ch = -1;

  data_path [0] = '\0';

  while (-1 != (ch = getopt (argc, argv, "rf:vVh")))
    {
      switch (ch)
        {
        case 'r':
          /* FIXME: implement read-only option */
          break;
        case 'f':
          set_contacts_file ();
          break;
        case 'v':
          display_version ();
          exit (0);
          break;
        case 'V':
          display_license ();
          exit (0);
          break;
        case 'h':
        case '?':
        default:
          display_usage (argv[0]);
          exit (0);
        }
    }
}

/***************************************************************************
    The main function.
 */

int
main (int argc, char *argv[])
{
  vc_component *v = NULL;
  fpos_t *fpos = NULL;
  long pos = 0;
  FILE *fp = NULL;
  ITEM *it = NULL;
  int entry_number = 0;

  bool done = FALSE;
  int win_state = WINDOW_INDEX;
  int command = 0;

  process_command_line_args (argc, argv);
  set_defaults ();
  /*
   * process_environment_variables(); 
   * process_configuration_file(); 
   */

  signal (SIGINT, finish);      /* catch interrupt for exiting */
  signal (SIGWINCH, resize);    /* catch interrupt for resizing */
  setlocale (LC_ALL, "");
  initscr ();

  keypad (stdscr, TRUE);        /* enable keypad for use of arrow keys */
  nonl ();                      /* tell curses not to do NL->CR/NL on output */
  cbreak ();                    /* take input chars immediately */
  noecho ();

  init_index (data_path);
  set_index_help_fcn (show_index_help);
  init_view ();
  set_view_help_fcn (show_view_help);
  init_edit ();
  set_edit_help_fcn (show_edit_help);
  init_help ();

  while (!done)
    {
      switch (win_state)
        {
        case WINDOW_INDEX:

      /*-------------------
         display the index
        -------------------*/

          display_index ();
          command = process_index_commands ();

          switch (command)
            {
            case INDEX_COMMAND_VIEW:
              win_state = WINDOW_VIEW;
              break;
            case INDEX_COMMAND_RAW_VIEW:
              win_state = WINDOW_RAW_VIEW;
              break;
            case INDEX_COMMAND_EDIT:
              win_state = WINDOW_EDIT;
              break;
            case INDEX_COMMAND_ADD:
              win_state = WINDOW_ADD;
              break;
            case INDEX_COMMAND_DELETE:
              win_state = WINDOW_DELETE;
              break;
            case INDEX_COMMAND_QUIT:
              done = TRUE;
              break;
            default:
              break;
            }

          break;

        case WINDOW_RAW_VIEW:

      /*-------------------------------------------------
         view the currently selected item with the pager
        -------------------------------------------------*/

          it = get_current_item ();

          /* only display if there is an item that is selected */
          if (NULL == it)
            {
              v = NULL;
            }
          else
            {
              fpos = (fpos_t *) item_userptr (it);

              fp = fopen (data_path, "r");
              fsetpos (fp, fpos);
              v = parse_vcard_file (fp);
              fclose (fp);
            }

          if (v != NULL)
            {
              raw_view (v);
              vc_delete_deep (v);
              v = NULL;
            }

          win_state = WINDOW_INDEX;

          break;

        case WINDOW_VIEW:

      /*----------------------------------
         view the currently selected item
        ----------------------------------*/

          it = get_current_item ();

          /* only display if there is an item that is selected */
          if (NULL == it)
            {
              v = NULL;
            }
          else
            {
              fpos = (fpos_t *) item_userptr (it);

              fp = fopen (data_path, "r");
              fsetpos (fp, fpos);
              v = parse_vcard_file (fp);
              fclose (fp);
            }

          if (v != NULL)
            {
              entry_number = get_entry_number (it);
              view_vcard (entry_number, v);
              command = process_view_commands ();

              switch (command)
                {
                case VIEW_COMMAND_EDIT:
                  win_state = WINDOW_EDIT;
                  break;
                case VIEW_COMMAND_INDEX:
                  win_state = WINDOW_INDEX;
                  break;
                case VIEW_COMMAND_PREVIOUS:
                  select_previous_item ();
                  win_state = WINDOW_VIEW;
                  break;
                case VIEW_COMMAND_NEXT:
                  select_next_item ();
                  win_state = WINDOW_VIEW;
                  break;
                default:
                  break;
                }
            }
          else
            {
              win_state = WINDOW_INDEX;
            }

          vc_delete_deep (v);
          v = NULL;

          break;

        case WINDOW_EDIT:

      /*--------------
         edit a vcard
        --------------*/

          it = get_current_item ();

          /* only display if there is an item that is selected */
          if (NULL != it)
            {
              fpos = (fpos_t *) item_userptr (it);

              fp = fopen (data_path, "r");
              fsetpos (fp, fpos);
              pos = ftell (fp);
              fclose (fp);
              fp = NULL;

              if (EDIT_SUCCESSFUL == edit_entry (data_path, pos))
                {
                  refresh_index ();
                }
            }

          win_state = WINDOW_INDEX;
          break;

        case WINDOW_ADD:
          if (ADD_SUCCESSFUL == add_entry (data_path))
            {
              refresh_index ();
            }

          win_state = WINDOW_INDEX;
          break;
        case WINDOW_DELETE:

          it = get_current_item ();

          /* only delete if there is an item that is selected */
          if (NULL != it)
            {
              fpos = (fpos_t *) item_userptr (it);

              fp = fopen (data_path, "r");
              fsetpos (fp, fpos);
              pos = ftell (fp);
              fclose (fp);
              fp = NULL;

              if (DELETE_SUCCESSFUL == delete_entry (data_path, pos))
                {
                  refresh_index ();
                }
            }

          win_state = WINDOW_INDEX;
          break;
        default:
          break;
        }
    }

  finish (0);
  exit (EXIT_SUCCESS);
  return (0);
}
