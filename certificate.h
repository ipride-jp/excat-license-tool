/*
 * @(#)certificate.h
 * $Id$
 */

#if !defined(___CERTIFICATE_H___)
#define ___CERTIFICATE_H___

#include <openssl/x509.h>
#include <sys/types.h>

int license_certificate_is_valid(X509* cert, time_t*);
char* license_certificate_get_mac_address(X509* cert);
char* license_certificate_get_info(X509* cert);
char* license_certificate_get_begin_date(X509* cert);
char* license_certificate_get_end_date(X509* cert);
char* license_certificate_get_subject(X509* cert);
char* license_certificate_get_public_key(X509* cert);
X509* open_certificate_file(const char* certfile);

#endif /* !defined(___CERTIFICATE_H___) */

/* the end of file */
