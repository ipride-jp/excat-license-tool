/*
 * @(#)test-certificate.c
 * $Id$
 */

#include <assert.h>
#include <stdio.h>

#include <openssl/pem.h>
#include <openssl/x509.h>

#include "license_int.h"
#include "certificate.h"


static int valid(const char* file, time_t* time)
{
  int r = false;

  FILE* f = null;
  X509* c = null;

  f = fopen(file, "r");
  if (f == null) {
    goto out;
  }

  c = PEM_read_X509(f, null, null, null);
  if (c == null) {
    goto out;
  }

  r = license_certificate_is_valid(c, time);

 out:
  if (c != null) X509_free(c);
  if (f != null) fclose(f);

  return r;
}


/*
 * TODO
 * ADD TEST CASES FOR license_get_xxxx()
 */


#include <openssl/ssl.h>
int main(int argc, char* argv[])
{
  OpenSSL_add_all_algorithms();

  /* ok */
  assert(valid("c-2cert.pem", null));

  /* invalid (expired) */
  assert(!valid("c-3cert.pem", null));

  /* invalid (certified by illegal CA) */
  assert(!valid("c-4cert.pem", null));

  printf("OK!\n");

  return 0;
}

/* the end of file */
