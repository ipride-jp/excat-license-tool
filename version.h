/*
 * @(#)version.h
 * $Id$
 */

#if !defined(___VERSION_H___)
#define ___VERSION_H___

#define VC_NA 0
#define VC_EQ 1
#define VC_GT 2
#define VC_GE 3
#define VC_LT 4
#define VC_LE 5

struct version {
  char* application;
  char* version;
  int comparator;
  struct version* next;
};

struct version* license_version_parse_string(const char* verstr);
void license_version_release(struct version* vers);
int license_version_is_accepted(const char* appverstr, const char* licversstr);

#endif /* !defined(___VERSION_H___) */

/* the end of file */
