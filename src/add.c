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

#include "add.h"
#include <limits.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vc.h>

static void append_to_datafile (const char *datafile,
                                const char *new_entry_filename);
static char *basename (const char *path);

static const char *editor = NULL;
static const char *editor_basename = NULL;

/***************************************************************************
 */

static void
append_to_datafile (const char *datafile, const char *new_entry_filename)
{
  FILE *tfp = NULL;
  FILE *dfp = NULL;
  FILE *efp = NULL;
  char tmp_datafile[PATH_MAX];
  int ch = 0;
  int rc = 0;
  vc_component *v = NULL;

  strcpy (tmp_datafile, datafile);
  strcat (tmp_datafile, ".tmp");
  dfp = fopen (datafile, "r");
  tfp = fopen (tmp_datafile, "w");

  efp = fopen (new_entry_filename, "r");
  v = parse_vcard_file (efp);
  fclose (efp);                 /* entry file no longer needed */

  /* copy the datafile into the temp datafile */
  ch = getc (dfp);
  while (ch != EOF)
    {
      putc (ch, tfp);
      ch = getc (dfp);
    }

  fclose (dfp);                 /* datafile no longer needed */

  /* copy the new entry into the temp file */
  putc ('\n', tfp);
  fprintf_vcard (tfp, v);

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

int
add_entry (const char *datafile)
{
  FILE *fp = NULL;
  char filename[PATH_MAX];
  pid_t process_id = 0;
  int status = 0;
  struct stat sb;
  time_t modified_time = 0;
  int ret_val = -1;

  /* open a temp file for editing */
  tmpnam (filename);
  fp = fopen (filename, "w");
  fprintf (fp, "BEGIN:VCARD\n");
  fprintf (fp, "VERSION:3.0\n");
  fprintf (fp, "FN:\n");
  fprintf (fp, "N:;\n");
  fprintf (fp, "END:VCARD\n");
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
      append_to_datafile (datafile, filename);
      ret_val = ADD_SUCCESSFUL;
    }
  else
    {
      ret_val = ADD_ABORTED;
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

void
set_add_editor (const char *str)
{
  editor = str;
  editor_basename = basename (editor);
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
