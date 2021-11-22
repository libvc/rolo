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
 *  $Id: search.c,v 1.5 2003/05/22 05:04:54 ahsu Exp $
 */

#include "search.h"
#include <string.h>
#include <ctype.h>

static int strstr_nocase (const char *haystack, const char *needle);
static ITEM *search_menu_forwards (MENU * menu);
static ITEM *search_menu_backwards (MENU * menu);

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
 */

void
set_menu_search_string (MENU * menu, const char *search_string)
{
  void *userptr = NULL;

  userptr = menu_userptr (menu);

  if (NULL != userptr)
    {
      free (userptr);
    }

  if (NULL == search_string)
    {
      set_menu_userptr (menu, NULL);
    }
  else
    {
      set_menu_userptr (menu, strdup (search_string));
    }
}

/***************************************************************************
    Perform the search for an item using the stored search string
    in the user_ptr.
 */

ITEM *
search_menu (MENU * menu, int initial_direction)
{
  ITEM *result_item = NULL;

  switch (initial_direction)
    {
    case REQ_SCR_DPAGE:
      result_item = search_menu_forwards (menu);
      break;
    case REQ_SCR_UPAGE:
      result_item = search_menu_backwards (menu);
      break;
    default:
      break;
    }

  return result_item;
}

static ITEM *
search_menu_forwards (MENU * menu)
{
  char *search_string = NULL;
  ITEM *result_item = NULL;

  /* the search string is stored in the menu user pointer */
  search_string = (char *) menu_userptr (menu);
  if (NULL != search_string)
    {
      int i = -1;
      int current_index = -1;
      int count = -1;
      ITEM **items = NULL;
      int found = 0;
      bool done = FALSE;

      current_index = item_index (current_item (menu));
      count = item_count (menu);
      items = menu_items (menu);

      /* start search from the item immediately after the current item */
      for (i = current_index + 1; i < count && !found; i++)
        {
          found = strstr_nocase (item_description (items[i]), search_string);
        }

      if (!found)
        {
          /* start search from the beginning (i.e. wrap around) */
          for (i = 0; i <= current_index && !found; i++)
            {
              found =
                  strstr_nocase (item_description (items[i]), search_string);
            }
        }

      if (found)
        {
          result_item = items[i - 1];
        }

    }

  return result_item;
}

static ITEM *
search_menu_backwards (MENU * menu)
{
  char *search_string = NULL;
  ITEM *result_item = NULL;

  /* the search string is stored in the menu user pointer */
  search_string = (char *) menu_userptr (menu);

  if (NULL != search_string)
    {
      int i = -1;
      int current_index = -1;
      ITEM **items = NULL;
      int found = 0;
      bool done = FALSE;

      current_index = item_index (current_item (menu));
      items = menu_items (menu);

      /* start search from the item immediately before the current item */
      for (i = current_index - 1; i >= 0 && !found; i--)
        {
          found = strstr_nocase (item_description (items[i]), search_string);
        }

      if (!found)
        {
          int count = -1;
          count = item_count (menu);
          /* start search from the end (i.e. wrap around) */
          for (i = count - 1; i >= current_index && !found; i--)
            {
              found =
                  strstr_nocase (item_description (items[i]), search_string);
            }
        }

      if (found)
        {
          result_item = items[i + 1];
        }
    }

  return result_item;
}
