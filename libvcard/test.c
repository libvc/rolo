/*
 * $Id: main.c,v 1.21 2002/12/06 08:24:49 hsu Exp $
 */

#include "vcard.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>

int
main(int argc, char *argv[])
{
  vcard *v = NULL;
  FILE *fp = NULL;
  fpos_t *fpos = NULL;

  if (2 != argc) {
    fprintf(stderr, "invalid number of arguments.\n");
    exit(1);
  }

  fp = fopen(argv[1], "r");

  fpos = (fpos_t *) malloc(sizeof(fpos_t));
  fgetpos(fp, fpos);
  fprintf(stderr, "---storing file position: %lli\n", *fpos);
  fprintf(stderr, "+++ftell: %li\n", ftell(fp));
  v = parse_vcard_file(fp);

  while (NULL != v) {
    fprintf_vcard(stdout, v);
    delete_vcard(v);
    v = NULL;

    fpos = (fpos_t *) malloc(sizeof(fpos_t));
    fgetpos(fp, fpos);
    fprintf(stderr, "---storing file position: %lli\n", *fpos);
    fprintf(stderr, "+++ftell: %li\n", ftell(fp));

    v = parse_vcard_file(fp);
  }

  fclose(fp);
  return 0;
}
