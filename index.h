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
 * $Id$
 */

#include <menu.h>

enum index_commands { INDEX_COMMAND_VIEW, INDEX_COMMAND_EDIT,
  INDEX_COMMAND_QUIT
};

/*** PROTOTYPES ***/

extern void init_index(const char *dirpath);
extern ITEM **get_items(FILE * fp, int count);
extern MENU *get_menu(ITEM ** items);
extern void display_index();
extern int process_index_commands();
extern void free_items(ITEM ** items);
extern void select_next_item();
extern void select_previous_item();
extern ITEM *get_current_item();
extern void set_index_help_fcn(void (*fcn) (void));
