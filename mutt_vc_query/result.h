/*
 * mutt_vc_query - vCard address query utility for mutt
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

#include <stdio.h>

typedef struct query_result_tag query_result;

enum sort_results_by
{ SORT_RESULTS_BY_NAME, SORT_RESULTS_BY_EMAIL, SORT_RESULTS_BY_MISC };

/*** PROTOTYPES ***/
query_result *create_query_result ();
void get_results (FILE * fp, const char *query_string, int *searched,
                  query_result * results, int *rc);
void print_results (const query_result * results);
void sort_results (query_result * results, int n, int sort_by);
query_result *last_result (query_result * results);
