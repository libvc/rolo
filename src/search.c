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
 *  $Id: search.c,v 1.2 2003/05/21 00:01:00 ahsu Exp $
 */

#include "search.h"
#include <string.h>

static int strstr_nocase (const char *haystack, const char *needle);

/***************************************************************************
    Searches for the needle in the haystack without worrying about
    the case.  Returns 1 if found, 0 otherwise.
 */

static int
strstr_nocase (const char *haystack, const char *needle)
{
  int result = 0;

  if (NULL != haystack && NULL != needle)
    {
      char tmp_haystack[80];
      char tmp_needle[80];
      int len = 0;
      int i = 0;

      strncpy (tmp_haystack, haystack, sizeof (tmp_haystack) - 1);
      tmp_haystack[sizeof (tmp_haystack) - 1] = '\0';
      strncpy (tmp_needle, needle, sizeof (tmp_needle) - 1);
      tmp_needle[sizeof (tmp_haystack) - 1] = '\0';

      len = strlen (tmp_haystack);
      for (i = 0; i < len; i++)
        {
          tmp_haystack[i] = tolower (tmp_haystack[i]);
        }

      len = strlen (tmp_needle);
      for (i = 0; i < len; i++)
        {
          tmp_needle[i] = tolower (tmp_needle[i]);
        }

      if (NULL != strstr (tmp_haystack, tmp_needle))
        {
          result = 1;
        }
    }

  return result;
}

/***************************************************************************
    Perform the search for an item with a given search string.
 */

ITEM *
search_menu (MENU * menu, const char *search_string)
{
  ITEM *result_entry = NULL;
  int i = 0;
  ITEM **items = NULL;
  int found_string = 0;
  bool done = FALSE;

  if (NULL != search_string)
    {
      i = item_index (current_item (menu));
      items = menu_items (menu);

      while (!done && (NULL != items[i]))
        {
          found_string =
              strstr_nocase (item_description (items[i]), search_string);

          if (1 == found_string)
            {
              result_entry = items[i];
              done = TRUE;
            }

          i++;
        }
    }

  return result_entry;
}
