/*
 * @(#)der2hex.c
 * $Id$
 */

#include <assert.h>
#include <stdio.h>


#define nul '\0'
#define null ((void*) 0)


int main(int argc, char* argv[])
{
  FILE* fi;
  FILE* fo;
  char bs[10];
  size_t sz;

  fi = fopen("cacert.der", "r");
  assert(fi != null);

  fo = fopen("cacert.hex", "w");
  assert(fo != null);

  while ((sz=fread(bs, 1, sizeof(bs), fi))) {
    size_t i;
    for (i=0; i<sz; i++) {
      fprintf(fo, " 0x%02x,", bs[i]&0xff);
    }
    fprintf(fo, "\n");
  }

  fclose(fo);
  fclose(fi);

  return 0;
}

/* the end of file */
