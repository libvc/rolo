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

#include <stdio.h>

/*** STRUCTS ***/

/* FIXME: move this struct to entry.c and abstract out the manipulation of
          the struct so that the user of this module does not have to know
          what is inside of the struct */
struct entry_node_tag
{
  char *description;
  fpos_t *fpos;
  struct entry_node_tag *next;
};

typedef struct entry_node_tag entry_node;

enum sort_entries_by
{ SORT_ENTRIES_BY_FAMILY_N, SORT_ENTRIES_BY_GIVEN_N,
  SORT_ENTRIES_BY_EMAIL, SORT_ENTRIES_BY_TEL
};

extern entry_node **entries_lltoa (entry_node * entries_ll, int count);
extern void sort_entries (entry_node ** entries, int count, int sort_by);
extern void free_entry_node (entry_node * e);
extern int get_entries (FILE * fp, entry_node * entries_ll);
extern entry_node *new_entry_node ();
extern int filter_entries (entry_node * entries_ll, const char *filter_string);
extern void free_entries_array_but_leave_data (entry_node ** entries_array,
                                               int count);
