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
 * $Id: vcard.h,v 1.1 2003/02/16 05:03:32 ahsu Exp $
 */

#ifndef __VCARD_H
#define __VCARD_H

#include <stdio.h>

typedef struct vcard_tag vcard;
typedef struct vcard_item_tag vcard_item;
typedef struct vcard_item_param_tag vcard_item_param;
typedef struct vcard_item_iter_tag vcard_item_iter;
typedef struct vcard_item_param_iter_tag vcard_item_param_iter;

/* *** PROTOTYPES *** */

/* vcard operations */

vcard *create_vcard();
void delete_vcard(vcard * v);
void fprintf_vcard(FILE * fp, vcard * v);

void set_vcard_group(vcard * v, const char *group);
char *get_vcard_group(const vcard * v);

/* vcard item operations */

vcard_item *insert_vcard_item(vcard * v, const char *name);

void set_vcard_item_group(vcard_item * vi, const char *group);
char *get_vcard_item_group(const vcard_item * vi);

void set_vcard_item_value(vcard_item * vi, const char *value);
char *get_vcard_item_value(const vcard_item * vi);

void init_vcard_item_iter(vcard_item_iter * it, vcard * v);
vcard_item *next_vcard_item(vcard_item_iter * it);

/* vcard item parameter operations */

vcard_item_param *insert_vcard_item_param(vcard_item * vi, const char *str);

void init_vcard_item_param_iter(vcard_item_param_iter * it, vcard_item * vi);
vcard_item_param *next_vcard_item_param(vcard_item_param_iter * it);

/* parsing functions */
vcard *parse_vcard_file(FILE * fp);
int count_vcards(FILE * fp);

/* query functions */
vcard_item *get_vcard_item_by_name(const vcard * v, const char *name);

char *get_val_struct_part(const char *n, int part);

enum n_parts { N_FAMILY, N_GIVEN, N_MIDDLE, N_PREFIX, N_SUFFIX };
enum adr_parts { ADR_PO_BOX, ADR_EXT_ADDRESS, ADR_STREET, ADR_LOCALITY,
  ADR_REGION, ADR_POSTAL_CODE, ADR_COUNTRY
};

#define VC_ADDRESS              "ADR"
#define VC_AGENT                "AGENT"
#define VC_BIRTHDAY             "BDAY"
#define VC_CATEGORIES           "CATEGORIES"
#define VC_CLASS                "CLASS"
#define VC_DELIVERY_LABEL       "LABEL"
#define VC_EMAIL                "EMAIL"
#define VC_FORMATTED_NAME       "FN"
#define VC_GEOGRAPHIC_POSITION  "GEO"
#define VC_LOGO                 "LOGO"
#define VC_MAILER               "MAILER"
#define VC_NAME                 "N"
#define VC_NICKNAME             "NICKNAME"
#define VC_NOTE                 "NOTE"
#define VC_ORGANIZATION         "ORG"
#define VC_PHOTO                "PHOTO"
#define VC_PRODUCT_IDENTIFIER   "PRODID"
#define VC_REVISION             "REV"
#define VC_ROLE                 "ROLE"
#define VC_SORT_STRING          "SORT-STRING"
#define VC_TELEPHONE            "TEL"
#define VC_TIME_ZONE            "TZ"
#define VC_TITLE                "TITLE"
#define VC_URL                  "URL"
#define VC_VCARD                "VCARD"
#define VC_VERSION              "VERSION"

enum datatype_tag {
  DATATYPE_UNKNOWN,
  DATATYPE_ANY,
  DATATYPE_STRING,
  DATATYPE_VCSTRUCT
};

#endif                          /* __VCARD_H */
