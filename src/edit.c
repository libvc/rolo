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
 * $Id: edit.c,v 1.3 2003/03/22 11:54:46 ahsu Rel $
 */

#include "edit.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define HARD_CODED_HEADER_STR "q:Quit  h:Help"

/*** PROTOTYPES ***/
static void print_header ();
static void print_footer (const char *fn);
static char *basename (const char *path);
static void append_to_datafile (const char *datafile,
                                const char *new_entry_filename);
static void update_datafile (const char *datafile, long pos,
                             const char *new_entry_filename);

/*** STATIC VARIABLES ***/
static WINDOW *win = NULL;
static WINDOW *sub = NULL;
static void (*display_help) (void);

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
  vcard *v = NULL;

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

void
delete_entry (const char *datafile, long pos)
{
  FILE *tfp = NULL;
  FILE *dfp = NULL;
  char tmp_datafile[PATH_MAX];
  vcard *v = NULL;
  int ch = 0;
  long i = 0;
  int rc = 0;

  wmove (stdscr, LINES - 1, 0);
  wclrtoeol (stdscr);
  wprintw (stdscr, "Delete selected item? [Y]es [N]o: ");
  ch = wgetch (stdscr);

  wmove (stdscr, LINES - 1, 0);
  wclrtoeol (stdscr);

  if ('y' != tolower (ch))
    return;

  /* open a temp file for editing */
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

  fseek (dfp, pos, SEEK_SET);
  v = parse_vcard_file (dfp);

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
add_vcard (const char *datafile)
{
  FILE *fp = NULL;
  char filename[PATH_MAX];
  char *editor = NULL;
  char *editor_basename = NULL;
  pid_t process_id = 0;
  int status = 0;
  struct stat sb;
  time_t modified_time = 0;
  vcard *v = NULL;

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

  /*-------------------------
     setup the editor to use
    -------------------------*/
  editor = getenv ("EDITOR");
  if (NULL == editor)
    {
      editor = strdup ("vi");
      editor_basename = strdup ("vi");
    }
  else
    {
      editor_basename = basename (editor);
    }

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
    }

  remove (filename);

  /* put everything back to normal */
  refresh ();
  initscr ();
  keypad (stdscr, TRUE);        /* enable keypad for use of arrow keys */
  nonl ();                      /* tell curses not to do NL->CR/NL on output */
  cbreak ();                    /* take input chars immediately */
  noecho ();
}

/***************************************************************************
 */

void
init_edit ()
{
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

  strncpy (header_str, HARD_CODED_HEADER_STR, COLS);

  for (i = strlen (HARD_CODED_HEADER_STR); i < COLS; i++)
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
edit_vcard (const char *datafile, long pos)
{
  FILE *fp = NULL;
  char filename[PATH_MAX];
  char *editor = NULL;
  char *editor_basename = NULL;
  pid_t process_id = 0;
  int status = 0;
  struct stat sb;
  time_t modified_time = 0;
  vcard *v = NULL;

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

  /*-------------------------
     setup the editor to use
    -------------------------*/
  editor = getenv ("EDITOR");
  if (NULL == editor)
    {
      editor = strdup ("vi");
      editor_basename = strdup ("vi");
    }
  else
    {
      editor_basename = basename (editor);
    }

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
    }

  remove (filename);

  /* put everything back to normal */
  refresh ();
  initscr ();
  keypad (stdscr, TRUE);        /* enable keypad for use of arrow keys */
  nonl ();                      /* tell curses not to do NL->CR/NL on output */
  cbreak ();                    /* take input chars immediately */
  noecho ();
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
