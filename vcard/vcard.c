/*
 * libvcard - vCard library
 * Copyright (C) 2003  Andrew Hsu
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: vcard.c,v 1.4 2003/04/03 14:28:16 ahsu Rel $
 */


#include "vcard.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BUF_LEN 80

/*** STRUCTS ***/

struct vcard_tag
{
  char *group;
  vcard_item *item;
};

struct vcard_item_tag
{
  char *group;
  char *name;
  vcard_item_param *param;
  char *value;
  vcard_item *next;
};

struct vcard_item_param_tag
{
  char *str;
  vcard_item_param *next;
};

struct vcard_item_iter_tag
{
  vcard *vcard;
  vcard_item *current_item;
};

struct vcard_item_param_iter_tag
{
  vcard_item *item;
  vcard_item_param *current_param;
};

/*** STATIC PROTOTYPES ***/

static vcard_item *create_vcard_item ();
static vcard_item_param *create_vcard_item_param ();

/***************************************************************************
 */

vcard *
create_vcard ()
{
  vcard *new_vcard = NULL;

  new_vcard = (vcard *) malloc (sizeof (vcard));

  if (NULL != new_vcard)
    {
      new_vcard->group = NULL;
      new_vcard->item = NULL;
    }

  return new_vcard;
}

/***************************************************************************
 */

void
delete_vcard (vcard *v)
{
  vcard_item *vi = NULL;
  vcard_item *vi_to_free = NULL;
  vcard_item_param *param = NULL;
  vcard_item_param *param_to_free = NULL;

  if (NULL != v)
    {
      free (v->group);
      vi = v->item;
      free (v);
      while (NULL != vi)
        {
          vi_to_free = vi;
          vi = vi->next;

          free (vi_to_free->group);
          free (vi_to_free->name);
          free (vi_to_free->value);

          param = vi_to_free->param;
          while (NULL != param)
            {
              param_to_free = param;
              param = param->next;

              free (param_to_free->str);
            }
        }
    }
}

/***************************************************************************
 */

vcard_item *
insert_vcard_item (vcard *v, const char *name)
{
  vcard_item *new_vcard_item = NULL;

  if (NULL != v)
    {
      new_vcard_item = (vcard_item *) create_vcard_item ();

      if (NULL != new_vcard_item)
        {
          vcard_item *vi = NULL;

          new_vcard_item->name = strdup (name);

          vi = v->item;

          if (NULL == vi)
            {
              v->item = new_vcard_item;
            }
          else
            {
              while (NULL != vi->next)
                {
                  vi = vi->next;
                }
              vi->next = new_vcard_item;
            }
        }
    }

  return new_vcard_item;
}

/***************************************************************************
 */

static vcard_item *
create_vcard_item ()
{
  vcard_item *new_vcard_item = NULL;

  new_vcard_item =
      (vcard_item *) malloc (sizeof (vcard_item));

  if (NULL != new_vcard_item)
    {
      new_vcard_item->group = NULL;
      new_vcard_item->name = NULL;
      new_vcard_item->param = NULL;
      new_vcard_item->value = NULL;
      new_vcard_item->next = NULL;
    }

  return new_vcard_item;
}

/***************************************************************************
 */

void
set_vcard_item_group (vcard_item *vi, const char *group)
{
  if (NULL != vi)
    {
      free (vi->group);
      vi->group = strdup (group);
    }
}

/***************************************************************************
 */

char *
get_vcard_item_group (const vcard_item *vi)
{
  char *result = NULL;

  if (NULL != vi)
    {
      result = vi->group;
    }

  return result;
}

/***************************************************************************
 */

void
set_vcard_item_value (vcard_item *vi, const char *value)
{
  if (NULL != vi)
    {
      free (vi->value);
      vi->value = strdup (value);
    }
}

/***************************************************************************
 */

char *
get_vcard_item_value (const vcard_item *vi)
{
  char *result = NULL;

  if (NULL != vi)
    {
      result = vi->value;
    }

  return result;
}

/***************************************************************************
 */

void
set_vcard_group (vcard *v, const char *group)
{
  if (NULL != v)
    {
      free (v->group);
      v->group = strdup (group);
    }
}

/***************************************************************************
 */

char *
get_vcard_group (const vcard *v)
{
  char *result = NULL;

  if (NULL != v)
    {
      result = v->group;
    }

  return result;
}

/***************************************************************************
 */

void
init_vcard_item_iter (vcard_item_iter *it, vcard *v)
{
  if (NULL != it)
    {
      it->vcard = v;

      if (NULL != v)
        {
          it->current_item = v->item;
        }
      else
        {
          it->current_item = NULL;
        }
    }
}

/***************************************************************************
 */

vcard_item *
next_vcard_item (vcard_item_iter *it)
{
  vcard_item *result_vcard_item = NULL;

  if (NULL != it)
    {
      result_vcard_item = it->current_item;

      if (NULL != result_vcard_item)
        {
          it->current_item = result_vcard_item->next;
        }
    }

  return result_vcard_item;
}

/***************************************************************************
 */

vcard_item_param *
insert_vcard_item_param (vcard_item *vi, const char *str)
{
  vcard_item_param *new_vcard_item_param = NULL;

  if (NULL != vi)
    {
      new_vcard_item_param =
          (vcard_item_param *) create_vcard_item_param ();

      if (NULL != new_vcard_item_param)
        {
          new_vcard_item_param->str = strdup (str);
          new_vcard_item_param->next = vi->param;
          vi->param = new_vcard_item_param;
        }
    }

  return new_vcard_item_param;
}

/***************************************************************************
 */

static vcard_item_param *
create_vcard_item_param ()
{
  vcard_item_param *new_vcard_item_param = NULL;

  new_vcard_item_param =
      (vcard_item_param *) malloc (sizeof (struct
                                                      vcard_item_param_tag));

  if (NULL != new_vcard_item_param)
    {
      new_vcard_item_param->str = NULL;
      new_vcard_item_param->next = NULL;
    }

  return new_vcard_item_param;
}

/***************************************************************************
 */

void
delete_vcard_item_param (vcard_item_param *param)
{
}

/***************************************************************************
 */

void
init_vcard_item_param_iter (vcard_item_param_iter *it,
                            vcard_item * vi)
{
  if (NULL != it)
    {
      it->item = vi;

      if (NULL != vi)
        {
          it->current_param = vi->param;
        }
      else
        {
          it->current_param = NULL;
        }
    }
}

/***************************************************************************
 */

vcard_item_param *
next_vcard_item_param (vcard_item_param_iter *it)
{
  vcard_item_param *result_vcard_item_param = NULL;

  if (NULL != it)
    {
      result_vcard_item_param = it->current_param;

      if (NULL != result_vcard_item_param)
        {
          it->current_param = result_vcard_item_param->next;
        }
    }

  return result_vcard_item_param;
}

/***************************************************************************
 */

void
fprintf_vcard (FILE * fp, const vcard *v)
{
  vcard_item_iter item_it;
  vcard_item_param_iter param_it;
  vcard_item *vi = NULL;
  vcard_item_param *param = NULL;

  if (NULL != v)
    {
      if (NULL != v->group)
        {
          fprintf (fp, "%s.", v->group);
        }

      fprintf (fp, "BEGIN:VCARD\n");

      init_vcard_item_iter (&item_it, v);

      vi = next_vcard_item (&item_it);
      while (NULL != vi)
        {
          if (NULL != vi->group)
            {
              fprintf (fp, "%s.", vi->group);
            }

          fprintf (fp, "%s", vi->name);

          init_vcard_item_param_iter (&param_it, vi);
          param = next_vcard_item_param (&param_it);
          while (NULL != param)
            {
              fprintf (fp, ";%s", param->str);
              param = next_vcard_item_param (&param_it);
            }

          fprintf (fp, ":%s\n", vi->value);

          vi = next_vcard_item (&item_it);
        }

      if (NULL != v->group)
        {
          fprintf (fp, "%s.", v->group);
        }

      fprintf (fp, "END:VCARD\n");
    }
}

/***************************************************************************
 */

vcard_item *
get_vcard_item_by_name (const vcard * v, const char *name)
{
  vcard_item *result_vi = NULL;
  vcard_item *vi = NULL;
  int done = 0;

  if (NULL != v && NULL != name)
    {
      vi = v->item;
      while ((0 == done) && (NULL != vi))
        {
          if (0 == strcmp (name, vi->name))
            {
              result_vi = vi;
              done = 1;
            }

          vi = vi->next;
        }
    }

  return result_vi;
}

/***************************************************************************
 */

char *
get_val_struct_part (const char *str, int part_index)
{
  char buf[BUF_LEN];
  int i = 0;
  int j = 0;
  int part_counter = 0;
  int done = 0;
  char *result = NULL;

  if (NULL != str)
    {
      buf[0] = '\0';

      while ('\0' != str[i] && 1 != done)
        {
          if (part_index == part_counter)
            {
              while (str[i] != ';' && i < BUF_LEN - 1)
                {
                  buf[j] = str[i];
                  j++;
                  i++;
                }

              buf[j] = '\0';
              done = 1;
            }
          else
            {
              if (';' == str[i])
                {
                  part_counter++;
                }
              i++;
            }
        }

      if ('\0' != buf[0])
        {
          result = strdup (buf);
        }
    }

  return result;
}

/***************************************************************************
 */

int
count_vcards (FILE * fp)
{
  char buf[256];
  int counter = 0;

  while (EOF != fscanf (fp, "%s\n", buf))
    {
      if (0 == strcmp (buf, "BEGIN:VCARD"))
        counter++;
    }

  return counter;
}
