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

#include "view.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MENU_BAR_STRING "i:Index  h:Help  1:Ident  2:Geo  3:Tel  4:Org  5:Misc"
#define HEADING_IDENT_STRING "IDENTIFICATION"
#define HEADING_GEO_STRING "GEOGRAPHIC POSITION"
#define HEADING_TEL_STRING "TELECOMMUNICATION"
#define HEADING_ORG_STRING "ORGANIZATION"
#define HEADING_MISC_STRING "MISCELLANEOUS"

/*** PROTOTYPES ***/
static void print_header ();
static void print_footer (int entry_number, const char *fn);
static void view_ident ();
static void view_geo ();
static void view_tel ();
static void view_org ();
static void view_misc ();
static char *basename (const char *path);
static void view_left ();
static void view_right ();

enum view_modes
{ VIEW_IDENT = 1, VIEW_GEO, VIEW_TEL, VIEW_ORG, VIEW_MISC };

/*** STATIC VARIABLES ***/
static WINDOW *win = NULL;
static WINDOW *sub = NULL;
static void (*display_help) (void);
static vc_component *g_v = NULL;
static int g_mode = 1;

/***************************************************************************
 */

static void
view_left ()
{
  switch (g_mode)
    {
    case VIEW_IDENT:
      view_misc ();
      break;
    case VIEW_GEO:
      view_ident ();
      break;
    case VIEW_TEL:
      view_geo ();
      break;
    case VIEW_ORG:
      view_tel ();
      break;
    case VIEW_MISC:
      view_org ();
      break;
    default:
      break;
    }
}

/***************************************************************************
 */

static void
view_right ()
{
  switch (g_mode)
    {
    case VIEW_IDENT:
      view_geo ();
      break;
    case VIEW_GEO:
      view_tel ();
      break;
    case VIEW_TEL:
      view_org ();
      break;
    case VIEW_ORG:
      view_misc ();
      break;
    case VIEW_MISC:
      view_ident ();
      break;
    default:
      break;
    }
}

/***************************************************************************
    Initialize the view window without displaying it to the
    end-user.
 */

void
init_view ()
{
  win = newwin (0, 0, 0, 0);
  sub = derwin (win, LINES - 3, COLS, 1, 0);
  print_header ();
  keypad (win, TRUE);           /* enable keypad for use of arrow keys */
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
view_ident ()
{
  vc_component *vc = NULL;
  char *val = NULL;
  int x = 0;
  int y = 0;

  g_mode = VIEW_IDENT;

  werase (sub);

  x = (COLS - strlen (HEADING_IDENT_STRING)) / 2;
  y = 1;

  wattron (sub, A_UNDERLINE);
  mvwprintw (sub, y, x, HEADING_IDENT_STRING);
  wstandend (sub);
  wmove (sub, 3, 0);

  vc = vc_get_next_by_name (g_v, VC_FORMATTED_NAME);
  val = vc_get_value (vc);
  wprintw (sub, "Name         : %s\n", val ? val : "");

  vc = vc_get_next_by_name (g_v, VC_NICKNAME);
  val = vc_get_value (vc);
  wprintw (sub, "Nickname     : %s\n", val ? val : "");

  vc = vc_get_next_by_name (g_v, VC_BIRTHDAY);
  val = vc_get_value (vc);
  wprintw (sub, "Birthday     : %s\n", val ? val : "");

  wprintw (sub, "\n");

  vc = vc_get_next_by_name (g_v, VC_URL);
  val = vc_get_value (vc);
  wprintw (sub, "URL    : %s\n", val ? val : "");

  vc = vc_get_next_by_name (g_v, VC_PHOTO);
  val = vc_get_value (vc);
  wprintw (sub, "Photo  : %s\n", val ? val : "");

  vc = vc_get_next_by_name (g_v, VC_SOUND);
  val = vc_get_value (vc);
  wprintw (sub, "Sound  : %s\n", val ? val : "");

  wprintw (sub, "\n");

  vc = vc_get_next_by_name (g_v, VC_CATEGORIES);
  val = vc_get_value (vc);
  wprintw (sub, "Categories  : %s\n", val ? val : "");

  vc = vc_get_next_by_name (g_v, VC_REVISION);
  val = vc_get_value (vc);
  wprintw (sub, "Revision    : %s\n", val ? val : "");

  vc = vc_get_next_by_name (g_v, VC_NOTE);
  val = vc_get_value (vc);
  wprintw (sub, "Note        : %s\n", val ? val : "");

  touchwin (win);
  wrefresh (sub);
  wrefresh (win);
}

/***************************************************************************
 */

static void
view_geo ()
{
  vc_component *vc = NULL;
  char *str = NULL;
  char *val = NULL;
  int x = 0;
  int y = 0;

  g_mode = VIEW_GEO;

  werase (sub);

  x = (COLS - strlen (HEADING_GEO_STRING)) / 2;
  y = 1;

  wattron (sub, A_UNDERLINE);
  mvwprintw (sub, y, x, HEADING_GEO_STRING);
  wstandend (sub);
  wmove (sub, 3, 0);

  vc = vc_get_next_by_name (g_v, VC_GEOGRAPHIC_POSITION);
  val = vc_get_value (vc);

  str = get_val_struct_part (val, GEO_LATITUDE);
  wprintw (sub, "Latitude   : %s\n", str ? str : "");
  free (str);

  str = get_val_struct_part (val, GEO_LONGITUDE);
  wprintw (sub, "Longitude  : %s\n", str ? str : "");
  free (str);

  wprintw (sub, "\n");
  vc = vc_get_next_by_name (g_v, VC_TIME_ZONE);
  val = vc_get_value (vc);
  wprintw (sub, "Time Zone  : %s\n", val ? val : "");

  wprintw (sub, "\n");

  vc = vc_get_next_by_name (g_v, VC_ADDRESS);
  val = vc_get_value (vc);

  str = get_val_struct_part (val, ADR_PO_BOX);
  wprintw (sub, "Post Office Box   : %s\n", str ? str : "");
  free (str);

  str = get_val_struct_part (val, ADR_EXT_ADDRESS);
  wprintw (sub, "Extended Address  : %s\n", str ? str : "");
  free (str);

  str = get_val_struct_part (val, ADR_STREET);
  wprintw (sub, "Street Address    : %s\n", str ? str : "");
  free (str);

  str = get_val_struct_part (val, ADR_LOCALITY);
  wprintw (sub, "Locality          : %s\n", str ? str : "");
  free (str);

  str = get_val_struct_part (val, ADR_REGION);
  wprintw (sub, "Region            : %s\n", str ? str : "");
  free (str);

  str = get_val_struct_part (val, ADR_POSTAL_CODE);
  wprintw (sub, "Postal Code       : %s\n", str ? str : "");
  free (str);

  str = get_val_struct_part (val, ADR_COUNTRY);
  wprintw (sub, "Country           : %s\n", str ? str : "");
  free (str);

  wprintw (sub, "\n");
  touchwin (win);
  wrefresh (sub);
  wrefresh (win);
}

/***************************************************************************
 */

static void
view_tel ()
{
  vc_component *vc = NULL;
  char *val = NULL;
  int x = 0;
  int y = 0;
  int i = 0;

  g_mode = VIEW_TEL;

  werase (sub);

  x = (COLS - strlen (HEADING_TEL_STRING)) / 2;
  y = 1;

  wattron (sub, A_UNDERLINE);
  mvwprintw (sub, y, x, HEADING_TEL_STRING);
  wstandend (sub);
  wmove (sub, 3, 0);

  for (i = 1, vc = g_v; i <= 5; i++)
    {
      vc = vc_get_next_by_name (vc, VC_TELEPHONE);
      val = vc_get_value (vc);
      wprintw (sub, "Telephone #%i  : %s\n", i, val ? val : "");
    }

  wprintw (sub, "\n");

  for (i = 1, vc = g_v; i <= 5; i++)
    {
      vc = vc_get_next_by_name (vc, VC_EMAIL);
      val = vc_get_value (vc);
      wprintw (sub, "Email Address #%i  : %s\n", i, val ? val : "");
    }

  wprintw (sub, "\n");

  vc = vc_get_next_by_name (g_v, VC_MAILER);
  val = vc_get_value (vc);
  wprintw (sub, "Email Mailer      : %s\n", val ? val : "");

  touchwin (win);
  wrefresh (sub);
  wrefresh (win);
}

/***************************************************************************
 */

static void
view_org ()
{
  vc_component *vc = NULL;
  char *str = NULL;
  char *val = NULL;
  int x = 0;
  int y = 0;

  g_mode = VIEW_ORG;

  werase (sub);

  x = (COLS - strlen (HEADING_ORG_STRING)) / 2;
  y = 1;

  wattron (sub, A_UNDERLINE);
  mvwprintw (sub, y, x, HEADING_ORG_STRING);
  wstandend (sub);
  wmove (sub, 3, 0);

  vc = vc_get_next_by_name (g_v, VC_TITLE);
  val = vc_get_value (vc);
  wprintw (sub, "Title  : %s\n", val ? val : "");

  vc = vc_get_next_by_name (g_v, VC_ROLE);
  val = vc_get_value (vc);
  wprintw (sub, "Role   : %s\n", val ? val : "");

  wprintw (sub, "\n");

  vc = vc_get_next_by_name (g_v, VC_ORGANIZATION);
  val = vc_get_value (vc);

  str = get_val_struct_part (val, ORG_NAME);
  wprintw (sub, "Organization Name       : %s\n", str ? str : "");
  free (str);

  str = get_val_struct_part (val, ORG_UNIT_1);
  wprintw (sub, "Organizational Unit #1  : %s\n", str ? str : "");
  free (str);

  str = get_val_struct_part (val, ORG_UNIT_2);
  wprintw (sub, "Organizational Unit #2  : %s\n", str ? str : "");
  free (str);

  str = get_val_struct_part (val, ORG_UNIT_3);
  wprintw (sub, "Organizational Unit #3  : %s\n", str ? str : "");
  free (str);

  str = get_val_struct_part (val, ORG_UNIT_4);
  wprintw (sub, "Organizational Unit #4  : %s\n", str ? str : "");
  free (str);

  wprintw (sub, "\n");

  vc = vc_get_next_by_name (g_v, VC_LOGO);
  val = vc_get_value (vc);
  wprintw (sub, "Logo  : %s\n", val ? val : "");

  vc = vc_get_next_by_name (g_v, VC_AGENT);
  val = vc_get_value (vc);
  wprintw (sub, "Agent  : %s\n", val ? val : "");

  touchwin (win);
  wrefresh (sub);
  wrefresh (win);
}

/***************************************************************************
 */

static void
view_misc ()
{
  vc_component *vc = NULL;
  char *str = NULL;
  char *val = NULL;
  int x = 0;
  int y = 0;

  g_mode = VIEW_MISC;

  werase (sub);

  x = (COLS - strlen (HEADING_MISC_STRING)) / 2;
  y = 1;

  wattron (sub, A_UNDERLINE);
  mvwprintw (sub, y, x, HEADING_MISC_STRING);
  wstandend (sub);
  wmove (sub, 3, 0);

  vc = vc_get_next_by_name (g_v, VC_SORT_STRING);
  val = vc_get_value (vc);
  wprintw (sub, "Sort String  : %s\n", val ? val : "");

  vc = vc_get_next_by_name (g_v, VC_CLASS);
  val = vc_get_value (vc);
  wprintw (sub, "Class       : %s\n", val ? val : "");

  vc = vc_get_next_by_name (g_v, VC_KEY);
  val = vc_get_value (vc);
  wprintw (sub, "Public Key  : %s\n", val ? val : "");

  touchwin (win);
  wrefresh (sub);
  wrefresh (win);
}

/***************************************************************************
    Display the given vCard to the end-user.
 */

void
view_vcard (int entry_number, vc_component * v)
{
  vc_component *vc = NULL;
  char *val = NULL;

  g_v = v;

  vc = vc_get_next_by_name (g_v, VC_FORMATTED_NAME);
  val = vc_get_value (vc);
  print_footer (entry_number, val ? val : "");

  switch (g_mode)
    {
    case VIEW_IDENT:
      view_ident ();
      break;
    case VIEW_GEO:
      view_geo ();
      break;
    case VIEW_TEL:
      view_tel ();
      break;
    case VIEW_ORG:
      view_org ();
      break;
    case VIEW_MISC:
      view_misc ();
      break;
    default:
      break;
    }

}

/***************************************************************************
    Prints the header to the window buffer, but does not display it
    to the end-user.
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
    Prints the footer to the window buffer, but does not display it
    to the end-user.  The contents of the footer depends on the
    width of the screen.
 */

static void
print_footer (int entry_number, const char *fn)
{
  char *footer_str = NULL;
  char *entry_block = NULL;
  int entry_block_len = 0;
  int i = 0;

  footer_str = (char *) malloc (sizeof (char) * (COLS + 2));

  /****************************************
    initialize the footer string to dashes
   ****************************************/

  for (i = 0; i < COLS; i++)
    {
      footer_str[i] = '-';
    }

  if (NULL != fn)
    {
      entry_block_len = strlen (fn) + 18 + 3;   /* 3 for entry_number */
    }
  else
    {
      entry_block_len = 17 + 3; /* 3 for entry_number */
    }

  /****************************************************
    add the `entry' block only if there is enough room
   ****************************************************/

  if (entry_block_len <= COLS)
    {
      entry_block = (char *) malloc (sizeof (char) * (entry_block_len + 1));

      if (NULL != fn)
        {
          sprintf (entry_block, "---[ entry %i: %s ]---", entry_number % 1000,
                   fn);
        }
      else
        {
          sprintf (entry_block, "---[ entry %i ]---", entry_number % 1000);
        }

      entry_block_len = strlen (entry_block);
      for (i = 0; i < entry_block_len; i++)
        {
          footer_str[i] = entry_block[i];
        }

      free (entry_block);
    }

  footer_str[COLS] = '\n';
  footer_str[COLS + 1] = '\0';

  wattron (win, A_REVERSE);
  mvwprintw (win, LINES - 2, 0, footer_str);
  wstandend (win);
  free (footer_str);
}

/***************************************************************************
    Handle input from the end-user.
 */

int
process_view_commands ()
{
  bool done = FALSE;
  int ch = 0;
  int command = 0;

  while (!done)
    {
      ch = wgetch (win);

      switch (ch)
        {
        case '1':
          view_ident ();
          break;
        case '2':
          view_geo ();
          break;
        case '3':
          view_tel ();
          break;
        case '4':
          view_org ();
          break;
        case '5':
          view_misc ();
          break;
        case KEY_DOWN:
        case 'j':
          command = VIEW_COMMAND_NEXT;
          done = TRUE;
          break;
        case KEY_UP:
        case 'k':
          command = VIEW_COMMAND_PREVIOUS;
          done = TRUE;
          break;
        case 'e':
          command = VIEW_COMMAND_EDIT;
          done = TRUE;
          break;
        case 'h':
          (*display_help) ();
          touchwin (win);
          wrefresh (win);
          wrefresh (sub);
          break;
        case 'i':
        case 'q':
          command = VIEW_COMMAND_INDEX;
          g_mode = VIEW_IDENT;
          done = TRUE;
          break;
        case 'V':
          raw_view (g_v);
          break;
        case KEY_LEFT:
          view_left ();
          break;
        case KEY_RIGHT:
          view_right ();
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
set_view_help_fcn (void (*fcn) (void))
{
  display_help = fcn;
}

/***************************************************************************
 */

void
raw_view (const vc_component * v)
{
  /* FIXME: have a simple scrolling raw display of the vcard */
}
