/*
 * @(#)test-main.c
 * $Id$
 */

#include <stdio.h>

#include "license.h"


int main(int argc, char* argv[])
{
  int r;

  if (argc < 2) {
    fprintf(stderr, "usage: test-main license_file application_version\n");
    fprintf(stderr, "\tlicense_file: name of PEM encoded license file\n");
    fprintf(stderr, "\tapplication_version: name and application version in which the license file contains.\n");
    return -1;
  }

  r = license_is_valid_file(argv[1], argv[2]);

  printf("%s is%s valid license for %s.\n", argv[1], r ? "" : " NOT", argv[2]);

  return r ? 0 : -2;
}

/* the end of file */
