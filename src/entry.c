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

#include "entry.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <vc.h>
#include <glib.h>
#include <unac.h>
#include <ctype.h>

#define MENU_PRINT_FORMAT_SIZE 80

static int cmp_tel (const char *desc_a, const char *desc_b);
static int cmp_email (const char *desc_a, const char *desc_b);
static int cmp_given_n (const char *desc_a, const char *desc_b);
static int cmp_desc_by_tel (const void *a, const void *b);
static int cmp_desc_by_email (const void *a, const void *b);
static int cmp_desc_by_given_n (const void *a, const void *b);
static int cmp_desc_by_family_n (const void *a, const void *b);
static char *construct_menu_name (const char *family_name,
                                  const char *given_name, const char *email,
                                  const char *tel);
static void set_menu_print_format (char *menu_print_format,
				   const char* fn, const char* gn,
				   const char* em);

/***************************************************************************
 */

static int
cmp_tel (const char *desc_a, const char *desc_b)
{
  int i = 0;
  int n = 0;
  int ret_val = 0;

  for (i = 57, n = 74; i < n; i++)
    {
      if (tolower (desc_a[i]) > tolower (desc_b[i]))
        {
          ret_val = 1;
          break;
        }
      else if (tolower (desc_a[i]) < tolower (desc_b[i]))
        {
          ret_val = -1;
          break;
        }
    }
  return ret_val;
}

/***************************************************************************
 */

static int
cmp_email (const char *desc_a, const char *desc_b)
{
  int i = 0;
  int n = 0;
  int ret_val = 0;

  for (i = 26, n = 56; i < n; i++)
    {
      if (tolower (desc_a[i]) > tolower (desc_b[i]))
        {
          ret_val = 1;
          break;
        }
      else if (tolower (desc_a[i]) < tolower (desc_b[i]))
        {
          ret_val = -1;
          break;
        }
    }
  return ret_val;
}

/***************************************************************************
 */

static int
cmp_given_n (const char *desc_a, const char *desc_b)
{
  int i = 0;
  int n = 0;
  int ret_val = 0;

  for (i = 13, n = 25; i < n; i++)
    {
      if (tolower (desc_a[i]) > tolower (desc_b[i]))
        {
          ret_val = 1;
          break;
        }
      else if (tolower (desc_a[i]) < tolower (desc_b[i]))
        {
          ret_val = -1;
          break;
        }
    }
  return ret_val;
}

/***************************************************************************
 */

static int
cmp_desc_by_tel (const void *a, const void *b)
{
  int ret_val = 0;
  entry_node **ena = NULL;
  entry_node **enb = NULL;

  ena = (entry_node **) a;
  enb = (entry_node **) b;

  ret_val = cmp_tel ((*ena)->description, (*enb)->description);

  if (0 == ret_val)
    {
      ret_val = strcmp ((*ena)->description, (*enb)->description);
    }

  return ret_val;
}

/***************************************************************************
 */

static int
cmp_desc_by_email (const void *a, const void *b)
{
  int ret_val = 0;
  entry_node **ena = NULL;
  entry_node **enb = NULL;

  ena = (entry_node **) a;
  enb = (entry_node **) b;

  ret_val = cmp_email ((*ena)->description, (*enb)->description);
  if (0 == ret_val)
    {
      ret_val = strcmp ((*ena)->description, (*enb)->description);
    }

  return ret_val;
}

/***************************************************************************
 */

static int
cmp_desc_by_given_n (const void *a, const void *b)
{
  int ret_val = 0;
  entry_node **ena = NULL;
  entry_node **enb = NULL;

  char* ena_desc = NULL;
  char* enb_desc = NULL;
  char* pure_a = NULL;
  char* pure_b = NULL;
  size_t length[1] = {0};
  int a_alloc = 1;
  int b_alloc = 1;

  ena = (entry_node **) a;
  enb = (entry_node **) b;

  ena_desc = (*ena)->description;
  enb_desc = (*enb)->description;

  /* Try to convert the strings from UTF-8 and fall back to the original
     strings if it fails */
  if (unac_string ("UTF-8", ena_desc, strlen (ena_desc), &pure_a, length)) {
    pure_a = ena_desc;
    a_alloc = 0;
  }

  if (unac_string ("UTF-8", enb_desc, strlen (enb_desc), &pure_b, length)) {
    pure_b = enb_desc;
    b_alloc = 0;
  }

  ret_val = cmp_given_n (pure_a, pure_b);
  if (0 == ret_val)
    {
      ret_val = strcmp (pure_a, pure_b);
    }

  if (a_alloc)
    free (pure_a);
  if (b_alloc)
    free (pure_b);

  return ret_val;
}

/***************************************************************************
 */

static int
cmp_desc_by_family_n (const void *a, const void *b)
{
  int ret_val = 0;
  entry_node **ena = NULL;
  entry_node **enb = NULL;

  char* ena_desc = NULL;
  char* enb_desc = NULL;
  char* pure_a = NULL;
  char* pure_b = NULL;
  size_t length[1] = {0};
  int a_alloc = 1;
  int b_alloc = 1;

  ena = (entry_node **) a;
  enb = (entry_node **) b;

  ena_desc = (*ena)->description;
  enb_desc = (*enb)->description;

  /* Try to convert the strings from UTF-8 and fall back to the original
     strings if it fails */
  if (unac_string ("UTF-8", ena_desc, strlen (ena_desc), &pure_a, length)) {
    pure_a = ena_desc;
    a_alloc = 0;
  }

  if (unac_string ("UTF-8", enb_desc, strlen (enb_desc), &pure_b, length)) {
    pure_b = enb_desc;
    b_alloc = 0;
  }

  ret_val = strcmp (pure_a, pure_b);

  if (a_alloc)
    free (pure_a);
  if (b_alloc)
    free (pure_b);

  return ret_val;
}

/***************************************************************************
    Sorts the array of entries using the quick sort algorithm.
 */

void
sort_entries (entry_node ** entries, int count, int sort_by)
{
  switch (sort_by)
    {
    case SORT_ENTRIES_BY_FAMILY_N:
      qsort (entries, count, sizeof (entry_node *), cmp_desc_by_family_n);
      break;
    case SORT_ENTRIES_BY_GIVEN_N:
      qsort (entries, count, sizeof (entry_node *), cmp_desc_by_given_n);
      break;
    case SORT_ENTRIES_BY_EMAIL:
      qsort (entries, count, sizeof (entry_node *), cmp_desc_by_email);
      break;
    case SORT_ENTRIES_BY_TEL:
      qsort (entries, count, sizeof (entry_node *), cmp_desc_by_tel);
      break;
    default:
      break;
    }
}

/***************************************************************************
    Filters the given linked list of entries by keeping only those
    nodes that match the filter_string.  Returns the number of nodes
    that are left out.
 */

int
filter_entries (entry_node * entries_ll, const char *filter_string)
{
  int count = 0;
  entry_node *e = NULL;
  entry_node *prev = NULL;

  /* first check if there is a filter string */
  if (NULL != filter_string)
    {
      /* remember: the first node is a dummy node */
      prev = entries_ll;
      e = entries_ll->next;

      while (e != NULL)
        {
          if (NULL == strstr (e->description, filter_string))
            {
              /* must delete node */
              prev->next = e->next;
              free_entry_node (e);
              e = prev->next;
              count++;
            }
          else
            {
              /* skip node, move on */
              prev = e;
              e = e->next;
            }
        }
    }

  /* the number of deleted nodes */
  return count;
}

/***************************************************************************
 */

entry_node **
entries_lltoa (entry_node * entries_ll, int count)
{
  entry_node **entries_a = NULL;
  entry_node *e = NULL;
  int i = 0;

  entries_a = (entry_node **) malloc (sizeof (entry_node *) * count);

  for (i = 0, e = entries_ll->next; i < count; i++)
    {
      entries_a[i] = e;
      e = e->next;
    }

  return entries_a;
}

/***************************************************************************
 */

void
free_entries_array_but_leave_data (entry_node ** entries_array, int count)
{
  int i = 0;
  for (i = 0; i < count; i++)
    {
      free (entries_array[i]);
    }

  free (entries_array);
}

/***************************************************************************
    Frees an entry node.  The user of this function must also
    remember to set the pointer to NULL after calling this function
    for cleanliness.
 */

void
free_entry_node (entry_node * e)
{
  if (NULL != e)
    {
      free (e->description);
      free (e->fpos);
      free (e);
    }
}

/***************************************************************************
    Allocates memory for an entry node and initialises the pointers
    in the structure to NULL.
 */

entry_node *
new_entry_node ()
{
  entry_node *e = NULL;

  e = (entry_node *) malloc (sizeof (entry_node));

  if (NULL == e)
    {
      endwin ();
      fprintf (stderr, "unable to malloc an entry_node\n");
      exit (1);
    }
  else
    {
      e->description = NULL;
      e->fpos = NULL;
      e->next = NULL;
    }

  return e;
}

/***************************************************************************
    Parses the data file for entries one at a time and stuffs them
    into a linked list.
 */

int
get_entries (FILE * fp, entry_node * entries_ll)
{
  int i = 0;
  vc_component *v = NULL;
  vc_component *vc = NULL;
  char *email = NULL;
  char *tel = NULL;
  char *family_name = NULL;
  char *given_name = NULL;
  char *desc = NULL;
  int count = 0;
  entry_node *entry = NULL;
  fpos_t *fpos = NULL;

  fpos = (fpos_t *) malloc (sizeof (fpos_t));
  fgetpos (fp, fpos);
  v = parse_vcard_file (fp);

  while (NULL != v)
    {
      count++;
      entry = new_entry_node ();

      entry->next = entries_ll->next;
      entries_ll->next = entry;

      entry->fpos = fpos;
      fpos = NULL;

      /*
       * retrive the name value
       */

      vc = vc_get_next_by_name (v, VC_NAME);
      family_name = get_val_struct_part (vc_get_value (vc), N_FAMILY);
      given_name = get_val_struct_part (vc_get_value (vc), N_GIVEN);

      /*
       * retrive the preferred email value
       */

      email = vc_get_preferred_email (v);

      /*
       * retrive the preferred telephone value
       */

      tel = vc_get_preferred_tel (v);

      /*
       * put them together
       */

      desc = construct_menu_name (family_name, given_name, email, tel);
      entry->description = desc;

      /*
       * cleanup
       */

      free (family_name);
      free (given_name);
      family_name = NULL;
      given_name = NULL;
      vc_delete_deep (v);
      v = NULL;

      fpos = (fpos_t *) malloc (sizeof (fpos_t));
      fgetpos (fp, fpos);
      v = parse_vcard_file (fp);
    }

  return count;
}

/***************************************************************************
    Constructs a string containing the description part of a
    menu_item.  The user of this function must remember to free the
    returned string at some point in the future.
 */

static char *
construct_menu_name (const char *family_name, const char *given_name,
                     const char *email, const char *tel)
{
  char *menu_name = NULL;
  char menu_print_format[MENU_PRINT_FORMAT_SIZE];
  int x = 0;
  int y = 0;

  /* FIXME: get the y and x vals from arguments passed to this function 
     so that this module will not need to rely on ncurses.h */
  getmaxyx (stdscr, y, x);
  /* Allow enough room for UTF-8 characters */
  menu_name = (char *) malloc (sizeof (char) * 2 * x);

  set_menu_print_format (menu_print_format, family_name, given_name, email);
  sprintf (menu_name, menu_print_format,
           family_name ? family_name : "",
           given_name ? given_name : "", email ? email : "", tel ? tel : "");

  return menu_name;
}

/***************************************************************************
    Sets the print format string given a width.
 */

static void
set_menu_print_format (char *menu_print_format,
		       const char* fn, const char* gn, const char* em)
{
  int fn_len, gn_len, em_len;
  fn_len = gn_len = 12;
  em_len = 30;
  if (fn)
      fn_len += strlen (fn) - g_utf8_strlen (fn, -1);
  if (gn)
      gn_len += strlen (gn) - g_utf8_strlen (gn, -1);
  if (em)
      em_len += strlen (em) - g_utf8_strlen (em, -1);

  snprintf (menu_print_format, MENU_PRINT_FORMAT_SIZE,
	    "%%-%d.%ds %%-%d.%ds %%-%d.%ds %%-18.18s",
	    fn_len, fn_len, gn_len, gn_len, em_len, em_len);

  /* 75 characters long -- fits into 80 character wide screen */
  /* strcpy (menu_print_format, "%-12.12s %-12.12s %-30.30s %-18.18s"); */
}
