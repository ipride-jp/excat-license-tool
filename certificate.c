/*
 * @(#)certificate.c
 * $Id$
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/asn1.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

#include "license_int.h"
#include "certificate.h"


static X509* ROOT_CERTIFICATE = NULL;
static unsigned char ROOT_CERTIFICATE_ARRAY[] = {
#include "cacert.hex"
};


static int initialize_root_certificate()
{
  int r = false;
  BIO* b = NULL;
  X509* c = NULL;

  if (ROOT_CERTIFICATE != NULL) {
    goto out;
  }

  b = BIO_new_mem_buf(ROOT_CERTIFICATE_ARRAY, sizeof(ROOT_CERTIFICATE_ARRAY));
  if (b == NULL) {
    goto out;
  }

  c = d2i_X509_bio(b, NULL);
  if (c == NULL) {
    goto out;
  }

  ROOT_CERTIFICATE = c;
  r = true;

 out:
  if (b != NULL) BIO_free(b);

  return r;
}


/**
 * validate certificate in term of X.509 client certificate
 * (i.e., certified by built-in root certficate, and not expired)
 * @param cert is client certificate to be validated
 * @return true if it is valid
 */
int license_certificate_is_valid(X509* cert, time_t* time)
{
  int r = false;
  X509_STORE* s = NULL;
  X509_STORE_CTX* c = NULL;

  assert(cert != NULL);
#if !OPENSSL_IS_NEW_ENOUGH
  assert(time == NULL);
#endif /* !OPENSSL_IS_NEW_ENOUGH */

  s = X509_STORE_new();
  if (s == NULL) {
    goto out;
  }

  if (ROOT_CERTIFICATE==NULL && !initialize_root_certificate()) {
    goto out;
  }

  if (!X509_STORE_add_cert(s, ROOT_CERTIFICATE)) {
    goto out;
  }

  c = X509_STORE_CTX_new();
  if (c == NULL) {
    goto out;
  }

#if OPENSSL_IS_NEW_ENOUGH
  if (time != NULL) {
    X509_VERIFY_PARAM_set_time(c->param, *time);
  }
#endif /* OPENSSL_IS_NEW_ENOUGH */

  if (!X509_STORE_CTX_init(c, s, cert, NULL)) {
    goto out;
  }

  r = X509_verify_cert(c);

 out:
  if (c != NULL) X509_STORE_CTX_free(c);
  if (s != NULL) X509_STORE_free(s);

  return r;
}


static char* get_extension_value(X509* cert, const char* name)
{
  char* v = NULL;
  ASN1_OBJECT* o = NULL;
  X509_EXTENSION* x;
  ASN1_OCTET_STRING* s;

  assert(cert != NULL);
  assert(name != NULL);

  o = OBJ_txt2obj(name, 0);
  if (o == NULL) {
    goto out;
  }

  x = X509_get_ext(cert, X509_get_ext_by_OBJ(cert, o, 0));
  if (x == NULL) {
    goto out;
  }

  s = x->value;
  if (s == NULL) {
    goto out;
  }

  v = (char*) calloc(1, M_ASN1_STRING_length(s)+1);
  memmove(v, M_ASN1_STRING_data(s), M_ASN1_STRING_length(s));

 out:
  if (o != NULL) ASN1_OBJECT_free(o);

  return v;
}

/*
#if !HAVE_STRDUP
static char* strdup(const char* s)
{
  char* r;

  if (s == NULL) {
    return NULL;
  }

  r = (char*) calloc(1, strlen(s)+1);
  if (r == NULL) {
    return NULL;
  }
  memmove(r, s, strlen(s));

  return r;
}
#endif
 */


char* license_certificate_get_mac_address(X509* cert)
{
  char* t;
  char* v;

  assert(cert != NULL);
  t = get_extension_value(cert, "1.3.6.1.1.1.1.22");
  if (t == NULL) {
    return NULL;
  }

  v = strdup(t+2);
  free(t);

  return v;
}


char* license_certificate_get_info(X509* cert)
{
  char* t;
  char* v;

  assert(cert != NULL);
  t = get_extension_value(cert, "0.9.2342.19200300.100.1.4");
  if (t == NULL) {
    return NULL;
  }

  v = strdup(t+2);
  free(t);

  return v;
}


char* license_certificate_get_begin_date(X509* cert)
{
  const int SIZE = 6;
  ASN1_TIME* t;
  char* v;

  assert(cert != NULL);
  t = X509_get_notBefore(cert);
  v = (char*)calloc(1, SIZE+1);
  if (v == NULL) {
    return NULL;
  }
  memmove(v, t->data, SIZE);

  return v;
}


char* license_certificate_get_end_date(X509* cert)
{
  const int SIZE = 6;
  ASN1_TIME* t;
  char* v;

  assert(cert != NULL);
  t = X509_get_notAfter(cert);
  v = (char*)calloc(1, SIZE+1);
  if (v == NULL) {
    return NULL;
  }
  memmove(v, t->data, SIZE);

  return v;
}


char* license_certificate_get_subject(X509* cert)
{
  const int SIZE = 1024;
  char* v;

  assert(cert != NULL);
  v = (char*)calloc(1, SIZE);
  if (v == NULL) {
    return NULL;
  }
  X509_NAME_oneline(X509_get_subject_name(cert), v, SIZE-1);

  return v;
}


char* license_certificate_get_public_key(X509* cert)
{
  BIO* o;
  char* v;

  assert(cert != NULL);
  o = BIO_new(BIO_s_mem());
  if (o == NULL) {
    return NULL;
  }

  do {
    EVP_PKEY* k = X509_get_pubkey(cert);
    assert(k != NULL);

    /* NB: we can assume that the key is RSA as we make such license files */
    if (k->type == EVP_PKEY_RSA) {
      RSA_print(o, k->pkey.rsa, 0);
    } else if (k->type == EVP_PKEY_DSA) {
      DSA_print(o, k->pkey.dsa, 0);
    }
    EVP_PKEY_free(k);
  } while (false);

  do {
    char* t;
    size_t s;

    s = (size_t) BIO_get_mem_data(o, &t);
    v = (char *) calloc(1, s+1);
    if (v == NULL) {
      BIO_free(o);
      return NULL;
    }
    memmove(v, t, s);

  } while (false);

  BIO_free(o);

  return v;
}


X509* open_certificate_file(const char* certfile)
{
  X509* c = NULL;
  FILE* f = NULL;
  assert(certfile != NULL);

  license_initialize();

  f = fopen(certfile, "r");
  if (f == NULL) {
    printf("Cannot open license file.\n");
    return NULL;
  }

  c = PEM_read_X509(f, NULL, NULL, NULL);
  if (c == NULL) {
    printf("Cannot read license file.\n");
    fclose(f);
    return NULL;
  }

  fclose(f);
  return c;
}

/* the end of file */
