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

#include "vcard.h"
#include <ncurses.h>

/*** GLOBALS ***/

enum view_commands { VIEW_COMMAND_EDIT, VIEW_COMMAND_INDEX,
  VIEW_COMMAND_PREVIOUS, VIEW_COMMAND_NEXT
};

/*** PROTOTYPES ***/

extern void init_view();
extern void view_vcard(vcard * v);
extern int process_view_commands();
extern void set_view_help_fcn(void (*fcn) (void));
