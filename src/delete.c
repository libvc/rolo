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

#include "delete.h"
#include <limits.h>
#include <ncurses.h>
#include <vc.h>
#include <string.h>

/***************************************************************************
 */

int
delete_entry (const char *datafile, long pos)
{
  FILE *tfp = NULL;
  FILE *dfp = NULL;
  char tmp_datafile[PATH_MAX];
  vc_component *v = NULL;
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
    {
      return DELETE_ABORTED;
    }

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

  return DELETE_SUCCESSFUL;
}
