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
 * $Id: vcard.h,v 1.3 2003/04/03 14:28:15 ahsu Rel $
 */

#ifndef __VCARD_H
#define __VCARD_H

#include <stdio.h>

/*** TYPEDEFS ***/

typedef struct vcard_component_tag vcard_component;
typedef struct vcard_component_param_tag vcard_component_param;

/*** PROTOTYPES ***/

/* create function for vcard_component */
vcard_component *vc_new ();

/* modify functions for vcard_component */
vcard_component *vc_set_group (vcard_component * vc, const char *group);
vcard_component *vc_set_name (vcard_component * vc, char *name);
vcard_component *vc_add_param (vcard_component * vc,
                               vcard_component_param * vc_param);
vcard_component *vc_set_value (vcard_component * vc, const char *value);
vcard_component *vc_link (vcard_component * head, vcard_component * tail);

/* convenience function */
vcard_component *vc_append_with_name (vcard_component * vc, char *name);

/* read functions for vcard_component */
char *vc_get_group (const vcard_component * vc);
char *vc_get_name (const vcard_component * vc);
vcard_component_param *vc_get_param (const vcard_component * vc);
char *vc_get_value (const vcard_component * vc);

/* create function for vcard_component_param */
vcard_component_param *vc_param_new ();

/* modify functions for vcard_component_param */
vcard_component_param *vc_param_set_name (vcard_component_param * vc_param,
                                          const char *name);
vcard_component_param *vc_param_set_value (vcard_component_param * vc_param,
                                           const char *value);
vcard_component_param *vc_param_set_str (vcard_component_param * vc_param,
                                         const char *str);
vcard_component_param *vc_param_link (vcard_component_param * head,
                                      vcard_component_param * tail);

/* read functions for vcard_component_param */
char *vc_param_get_name (const vcard_component_param * vc_param);
char *vc_param_get_value (const vcard_component_param * vc_param);

/* functions for scrolling through vcard_components */
vcard_component *vc_get_next (const vcard_component * vc);
vcard_component *vc_get_next_by_name (vcard_component * vc, const char *name);

/* functions for scrolling through vcard_component_params */
vcard_component_param *vc_param_get_next (const vcard_component_param
                                          * vc_param);
vcard_component_param *vc_param_get_next_by_name (vcard_component_param
                                                  * vc_param,
                                                  const char *name);

/* clean-up functions */
void vc_delete (vcard_component * vc);
void vc_delete_deep (vcard_component * vc);
void vc_param_delete (vcard_component_param * vc_param);
void vc_param_delete_deep (vcard_component_param * vc_param);

/* printing functions */
void
fprintf_vcard_component_param (FILE * fp, vcard_component_param * vc_param);
void fprintf_vcard_component (FILE * fp, vcard_component * vc);
void fprintf_vcard (FILE * fp, vcard_component * vcard);

/* parsing functions */
vcard_component *parse_vcard_file (FILE * fp);
int count_vcards (FILE * fp);
char *get_val_struct_part (const char *n, int part);

/*** ENUMS ***/

enum n_parts
{ N_FAMILY, N_GIVEN, N_MIDDLE, N_PREFIX, N_SUFFIX };

enum adr_parts
{ ADR_PO_BOX, ADR_EXT_ADDRESS, ADR_STREET, ADR_LOCALITY,
  ADR_REGION, ADR_POSTAL_CODE, ADR_COUNTRY
};

enum org_parts
{ ORG_NAME, ORG_UNIT_1, ORG_UNIT_2, ORG_UNIT_3, ORG_UNIT_4 };

enum geo_parts
{ GEO_LATITUDE, GEO_LONGITUDE };

/*** DEFINES ***/

#define VC_ADDRESS              "ADR"
#define VC_AGENT                "AGENT"
#define VC_BIRTHDAY             "BDAY"
#define VC_CATEGORIES           "CATEGORIES"
#define VC_CLASS                "CLASS"
#define VC_DELIVERY_LABEL       "LABEL"
#define VC_EMAIL                "EMAIL"
#define VC_FORMATTED_NAME       "FN"
#define VC_GEOGRAPHIC_POSITION  "GEO"
#define VC_KEY                  "KEY"
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
#define VC_SOUND                "SOUND"
#define VC_TELEPHONE            "TEL"
#define VC_TIME_ZONE            "TZ"
#define VC_TITLE                "TITLE"
#define VC_URL                  "URL"
#define VC_VCARD                "VCARD"
#define VC_VERSION              "VERSION"

#endif /* __VCARD_H */
