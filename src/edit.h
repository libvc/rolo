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
 * $Id: edit.h,v 1.6 2003/04/03 14:51:01 ahsu Rel $
 */

#include <vc.h>
#include <ncurses.h>

enum edit_commands
{ EDIT_COMMAND_INDEX };

enum edit_entry_results
{ EDIT_SUCCESSFUL, EDIT_ABORTED };

extern void init_edit ();
extern int edit_entry (const char *datafile, long pos);
extern int process_edit_commands ();
extern void set_edit_help_fcn (void (*fcn) (void));
extern void set_edit_editor (const char *str);
