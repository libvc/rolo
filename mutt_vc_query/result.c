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
 * $Id: result.c,v 1.2 2003/05/04 12:37:54 ahsu Rel $
 */

#include "result.h"
#include <stdlib.h>
#include <string.h>
#include <vcard.h>

struct query_result_tag
{
  char *email;
  char *name;
  char *misc;
  query_result *next;
};

/*** LOCAL PROTOTYPES ***/
static query_result **qr_lltoa (query_result * results, int n);
static query_result *qr_atoll (query_result ** results_a, int n);
static int cmp_results_by_name (const void *a, const void *b);
static int cmp_results_by_email (const void *a, const void *b);
static int cmp_results_by_misc (const void *a, const void *b);
static void delete_query_result (query_result * qr);

/***************************************************************************
 */
query_result *
create_query_result ()
{
  query_result *ret_val = NULL;

  ret_val = (query_result *) malloc (sizeof (query_result));
  if (NULL == ret_val)
    {
      fprintf (stderr, "Unable to malloc query_result.\n");
      exit (1);
    }
  ret_val->name = NULL;
  ret_val->email = NULL;
  ret_val->misc = NULL;
  ret_val->next = NULL;
}

/***************************************************************************
 */

void
get_results (FILE * fp, const char *query_string, int *searched,
             query_result * results, int *rc)
{
  vcard_component *v = NULL;
  char *s_result = NULL;
  query_result *r = NULL;
  char *email = NULL;
  char *name = NULL;
  char *misc = NULL;
  vcard_component *fn = NULL;
  vcard_component *adr = NULL;

  r = results;
  *rc = 0;
  *searched = 0;
  for (v = parse_vcard_file (fp); NULL != v; v = parse_vcard_file (fp))
    {
      (*searched)++;
      fn = vc_get_next_by_name (v, VC_FORMATTED_NAME);
      name = vc_get_value (fn);
      email = vc_get_preferred_email (v);

      if (NULL != name && NULL != email)
        {
          adr = vc_get_next_by_name (v, VC_ADDRESS);
          misc = get_val_struct_part (vc_get_value (adr), ADR_LOCALITY);
          misc = misc ? misc : " ";

          if (strstr (name, query_string) || strstr (email, query_string)
              || strstr (misc, query_string))
            {
              r->next = create_query_result ();
              r = r->next;
              r->name = strdup (name);
              r->email = strdup (email);
              r->misc = strdup (misc);
              (*rc)++;
            }
        }

      vc_delete_deep (v);
      v = NULL;
    }
}

/***************************************************************************
 */

void
print_results (const query_result * results)
{
  query_result *r = NULL;

  for (r = results->next; NULL != r; r = r->next)
    {
      fprintf (stdout, "%s\t%s\t%s\n", r->email, r->name, r->misc);
    }
}

/***************************************************************************
 */

static int
cmp_results_by_name (const void *a, const void *b)
{
  int ret_val = 0;
  query_result **qra = NULL;
  query_result **qrb = NULL;

  qra = (query_result **) a;
  qrb = (query_result **) b;

  ret_val = strcmp ((*qra)->name, (*qrb)->name);

  if (0 == ret_val)
    {
      ret_val = strcmp ((*qra)->email, (*qrb)->email);
      if (0 == ret_val)
        {
          ret_val = strcmp ((*qra)->misc, (*qrb)->misc);
        }
    }

  return ret_val;
}

/***************************************************************************
 */

static int
cmp_results_by_email (const void *a, const void *b)
{
  int ret_val = 0;
  query_result **qra = NULL;
  query_result **qrb = NULL;

  qra = (query_result **) a;
  qrb = (query_result **) b;

  ret_val = strcmp ((*qra)->email, (*qrb)->email);

  if (0 == ret_val)
    {
      ret_val = strcmp ((*qra)->name, (*qrb)->name);
      if (0 == ret_val)
        {
          ret_val = strcmp ((*qra)->misc, (*qrb)->misc);
        }
    }

  return ret_val;
}

/***************************************************************************
 */

static int
cmp_results_by_misc (const void *a, const void *b)
{
  int ret_val = 0;
  query_result **qra = NULL;
  query_result **qrb = NULL;

  qra = (query_result **) a;
  qrb = (query_result **) b;

  ret_val = strcmp ((*qra)->misc, (*qrb)->misc);

  if (0 == ret_val)
    {
      ret_val = strcmp ((*qra)->name, (*qrb)->name);
      if (0 == ret_val)
        {
          ret_val = strcmp ((*qra)->email, (*qrb)->email);
        }
    }

  return ret_val;
}

/***************************************************************************
 */

void
sort_results (query_result * results, int n, int sort_by)
{
  query_result **results_a = NULL;
  int i = 0;

  results_a = qr_lltoa (results, n);

  switch (sort_by)
    {
    case SORT_RESULTS_BY_NAME:
      qsort (results_a, n, sizeof (query_result *), cmp_results_by_name);
      break;
    case SORT_RESULTS_BY_EMAIL:
      qsort (results_a, n, sizeof (query_result *), cmp_results_by_email);
      break;
    case SORT_RESULTS_BY_MISC:
      qsort (results_a, n, sizeof (query_result *), cmp_results_by_misc);
      break;
    default:
      break;
    }

  results->next = qr_atoll (results_a, n);
}

/***************************************************************************
 */

static query_result **
qr_lltoa (query_result * results, int n)
{
  query_result **results_a = NULL;
  query_result *r = NULL;
  int i = 0;

  results_a = (query_result **) malloc (sizeof (query_result *) * n);

  r = results->next;
  for (i = 0; i < n; i++)
    {
      results_a[i] = r;
      r = r->next;
    }

  return results_a;
}

/***************************************************************************
 */

static void
delete_query_result (query_result * qr)
{
  free (qr->name);
  qr->name = NULL;
  free (qr->email);
  qr->email = NULL;
  free (qr->misc);
  qr->misc = NULL;

  free (qr);
}

/***************************************************************************
 */

static query_result *
qr_atoll (query_result ** results_a, int n)
{
  query_result *r = NULL;
  query_result *returning_r = NULL;
  query_result *tmp = NULL;
  int i = 0;

  r = create_query_result ();
  returning_r = r;
  for (i = 0; i < n; i++)
    {
      r->next = results_a[i];
      r = r->next;
      r->next = NULL;
    }

  tmp = returning_r;
  returning_r = returning_r->next;
  delete_query_result (tmp);
  return returning_r;
}
