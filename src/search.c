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

#include "search.h"
#include <string.h>

/***************************************************************************
    Perform the search for an item with a given search string.
 */

ITEM *
search_menu (MENU *menu, const char *search_string)
{
  ITEM *result_entry = NULL;
  int i = 0;
  ITEM **items = NULL;
  char *found_string = NULL;
  bool done = FALSE;

  if (NULL != search_string)
    {
      i = item_index (current_item (menu));
      items = menu_items (menu);

      while (!done && (items[i] != NULL))
        {
          found_string = strstr (item_description (items[i]), search_string);

          if (found_string != NULL)
            {
              result_entry = items[i];
              done = TRUE;
            }

          i++;
        }
    }

  return result_entry;
}

