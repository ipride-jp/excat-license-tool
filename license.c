/*
 * @(#)license.c
 * $Id$
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <openssl/pem.h>

#include "license_int.h"

#include "certificate.h"
#include "version.h"

#define ENCODE_KEY_LEN 16

static bool initialized_p = false;



void license_initialize(void)
{
  if (initialized_p) {
    return;
  }

  OpenSSL_add_all_algorithms();
}



int license_is_valid_file(const char* certfile, const char* appverstr)
{
  int r = false;
  X509* c = NULL;
#if USE_MAC_ADDRESS
  char* m = NULL;
#endif /* USE_MAC_ADDRESS */
  char* v = NULL;


  assert(certfile != NULL);
  assert(appverstr != NULL);


  license_initialize();

  c = open_certificate_file(certfile);
  if (c == NULL) {
    goto out;
  }

  if (!license_certificate_is_valid(c, NULL)) {
    goto out;
  }

#if USE_MAC_ADDRESS
  m = license_certificate_get_mac_address(c);
  if (m == NULL) {
    goto out;
  }
#endif /* USE_MAC_ADDRESS */

  v = license_certificate_get_info(c);
  if (v == NULL) {
    goto out;
  }

  r = license_version_is_accepted(appverstr, v);

 out:
  if (v != NULL) free(v);
#if USE_MAC_ADDRESS
  if (m != NULL) free(m);
#endif /* USE_MAC_ADDRESS */
  if (c != NULL) X509_free(c);

  return r;
}


char* license_get_info(const char* certfile)
{
  char* r = NULL;
  X509* c = open_certificate_file(certfile);
  if (c == NULL) {
    goto out;
  }

  r = license_certificate_get_info(c);

 out:
  if (c != NULL) X509_free(c);

  return r;
}


char* license_get_begin_date(const char* certfile)
{
  char* r = NULL;
  X509* c = open_certificate_file(certfile);
  if (c == NULL) {
    goto out;
  }

  r = license_certificate_get_begin_date(c);

 out:
  if (c != NULL) X509_free(c);

  return r;
}


char* license_get_end_date(const char* certfile)
{
  char* r = NULL;
  X509* c = open_certificate_file(certfile);
  if (c == NULL) {
    goto out;
  }

  r = license_certificate_get_end_date(c);

 out:
  if (c != NULL) X509_free(c);

  return r;
}


char* license_get_subject(const char* certfile)
{
  char* r = NULL;
  X509* c = open_certificate_file(certfile);
  if (c == NULL) {
    goto out;
  }

  r = license_certificate_get_subject(c);

 out:
  if (c != NULL) X509_free(c);

  return r;
}


/*2007/05/18 modified by jiang
 when license file is changed,we want the user to be able to
 read the file file encoded with the old license file.
 so use unchanged CN section of the license file as the encode key.
 for prarams which used this function not to change,
 we will not change the name of this function */
char* license_get_public_key(const char* certfile)
{
	char* begin;
	char* end;
	char* encodeKey;
	char* pos;
	int i;
	char* subject = license_get_subject(certfile);
	
    if(subject == NULL)
		return NULL;
  
	begin = strstr(subject,"/CN=");
	if(begin == NULL)
    {
		free(subject);
		return NULL;
	}

	begin = begin + 4;
	end = strstr(begin,"/");
    if(end == NULL)
	{
       end = subject + strlen(subject);
	}
    encodeKey = (char*)malloc(ENCODE_KEY_LEN + 1);
	memset(encodeKey,'0',ENCODE_KEY_LEN + 1);
	pos = begin;
	i = 0;
	while(pos < end && i < ENCODE_KEY_LEN)
	{
		encodeKey[i] = *pos;
		pos++;
		i++;
	}
    encodeKey[ENCODE_KEY_LEN] = 0;
	free(subject);
	return encodeKey;
}

/* add by Qiu Song on 20090929 for MACアドレスのチェック */
char* license_get_mac_address(const char* certfile)
{
	int r = false;
	char* m = null;
	char* v = null;

	X509* c = open_certificate_file(certfile);
	if (c == null) {
		printf("Cannot open license file.\n");
		goto out;
	}

	if (!license_certificate_is_valid(c, null)) {
		printf("The license file is not valid.\n");
		goto out;
	}

  m = license_certificate_get_mac_address(c);
  if (m == null) {
    goto out;
  }
  return m;

 out:
  if (v != null) free(v);
  if (m != null) free(m);
  if (c != null) X509_free(c);

  return null;
}
/* end of add by Qiu Song on 20090929 for MACアドレスのチェック */

/*: the end of file */
