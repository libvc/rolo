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

#include "edit.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MENU_BAR_STRING "q:Quit  h:Help"

/*** PROTOTYPES ***/
static void print_header ();
static void print_footer (const char *fn);
static char *basename (const char *path);
static void update_datafile (const char *datafile, long pos,
                             const char *new_entry_filename);

/*** STATIC VARIABLES ***/
static WINDOW *win = NULL;
static WINDOW *sub = NULL;
static void (*display_help) (void);
static const char *editor = NULL;
static const char *editor_basename = NULL;

/***************************************************************************
 */

static void
update_datafile (const char *datafile, long pos,
                 const char *new_entry_filename)
{
  FILE *tfp = NULL;
  FILE *dfp = NULL;
  FILE *efp = NULL;
  char tmp_datafile[PATH_MAX];
  int ch = 0;
  long i = 0;
  int rc = 0;

  strcpy (tmp_datafile, datafile);
  strcat (tmp_datafile, ".tmp");
  dfp = fopen (datafile, "r");
  tfp = fopen (tmp_datafile, "w");

  /* copy the datafile into the temp file but only upto where to change */
  i = 0;
  ch = getc (dfp);
  while (ch != EOF && i < pos)
    {
      putc (ch, tfp);
      ch = getc (dfp);
      i++;
    }

  efp = fopen (new_entry_filename, "r");

  /* copy the new entry into the temp file */
  putc ('\n', tfp);
  ch = getc (efp);
  while (ch != EOF)
    {
      putc (ch, tfp);
      ch = getc (efp);
    }

  fclose (efp);                 /* entry file no longer needed */

  /* move to just after the changed entry position */
  fseek (dfp, pos, SEEK_SET);
  parse_vcard_file (dfp);

  /* copy the rest of the datafile over to the temp file */
  ch = getc (dfp);
  while (ch != EOF)
    {
      putc (ch, tfp);
      ch = getc (dfp);
    }

  fclose (dfp);                 /* datafile no longer needed */
  fclose (tfp);                 /* temp file no longer needed */

  /* replace the datafile with the temp datafile */
  rename (tmp_datafile, datafile);

  if (-1 == rc)
    {
      fprintf (stderr, "error with rename...\n");
      perror ("rolo");
    }
}

/***************************************************************************
 */

void
init_edit ()
{
  editor_basename = basename (editor);
  win = newwin (0, 0, 0, 0);
  sub = derwin (win, LINES - 3, COLS, 1, 0);
  keypad (win, TRUE);           /* enable keypad for use of arrow keys */
  print_header ();
}

/***************************************************************************
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
 */

static void
print_footer (const char *fn)
{
  char *footer_str = NULL;
  int i = 0;

  footer_str = (char *) malloc (sizeof (char) * (COLS + 2));

  for (i = 0; i < COLS; i++)
    {
      footer_str[i] = '-';
    }

  /*
   * ("---[ editing: %s ]---\n", fn);
   */

  footer_str[COLS] = '\n';
  footer_str[COLS + 1] = '\0';

  wattron (win, A_REVERSE);
  mvwprintw (win, LINES - 2, 0, footer_str);
  wstandend (win);
  free (footer_str);
}

/***************************************************************************
 */

void
set_edit_editor (const char *str)
{
  editor = str;
}

/***************************************************************************
 */

static char *
basename (const char *path)
{
  char *beginning = NULL;
  char *retval = NULL;

  beginning = strrchr (path, '/');

  if (NULL == beginning)
    {
      retval = strdup (path);
    }
  else
    {
      retval = strdup (beginning);
    }

  return retval;
}

/***************************************************************************
 */

int
edit_entry (const char *datafile, long pos)
{
  FILE *fp = NULL;
  char filename[PATH_MAX];
  pid_t process_id = 0;
  int status = 0;
  struct stat sb;
  time_t modified_time = 0;
  vc_component *v = NULL;
  int ret_val = -1;

  /* retrieve the entry for editing */
  fp = fopen (datafile, "r");
  fseek (fp, pos, SEEK_SET);
  v = parse_vcard_file (fp);
  fclose (fp);
  fp = NULL;

  /* open a temp file for editing */
  tmpnam (filename);
  fp = fopen (filename, "w");

  /* dump the entry into the temp file for editing */
  fprintf_vcard (fp, v);
  fclose (fp);
  fp = NULL;

  /* record when the file has been modified */
  stat (filename, &sb);
  modified_time = sb.st_mtime;

  endwin ();

  process_id = fork ();
  if (process_id < 0)
    {
      /* could not fork... check errno */
    }
  else if (0 == process_id)
    {
      /* child is running */

      /* replace process image with the editor */
      execlp (editor, editor_basename, filename, NULL);
      _exit (2);                /* execlp failed */
    }
  else
    {
      /* parent is running */
      waitpid (process_id, &status, 0);
    }

  /* check if the temp file has been modified */
  stat (filename, &sb);
  if (modified_time != sb.st_mtime)
    {
      /* need to change the datafile */
      update_datafile (datafile, pos, filename);
      ret_val = EDIT_SUCCESSFUL;
    }
  else
    {
      ret_val = EDIT_ABORTED;
    }

  remove (filename);

  /* put everything back to normal */
  refresh ();
  initscr ();
  keypad (stdscr, TRUE);        /* enable keypad for use of arrow keys */
  nonl ();                      /* tell curses not to do NL->CR/NL on output */
  cbreak ();                    /* take input chars immediately */
  noecho ();
  return ret_val;
}

/***************************************************************************
 */

int
process_edit_commands ()
{
  bool done = FALSE;
  int ch = 0;
  int command = 0;

  while (!done)
    {
      ch = wgetch (win);

      switch (ch)
        {
        case 'h':
          (*display_help) ();
          touchwin (win);
          wrefresh (win);
          wrefresh (sub);
          break;
        case 'q':
          done = TRUE;
          break;
        default:
          beep ();
          break;
        }
    }
  return command;
}

/***************************************************************************
 */

void
set_edit_help_fcn (void (*fcn) (void))
{
  display_help = fcn;
}
