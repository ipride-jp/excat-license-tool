/*
 * @(#)checker.c
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include "license.h"

int main(int argc, char* argv[])
{
  if (argc < 2) {
    printf("usage: checker [license] [version]\n");
    exit(-1);
  }

  if (license_is_valid_file(argv[1], argv[2])) {
    printf("valid!\n");
    return 0;
  } else {
    printf("invalid!\n");
    return -1;
  }
}

/* the end of file */
