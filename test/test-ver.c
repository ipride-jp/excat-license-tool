/*
 * @(#)test-version.c
 * $Id$
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "license_int.h"
#include "version.h"


static size_t size(const char* s)
{
  struct version* vs = license_version_parse_string(s);
  size_t n = 0;
  struct version* v;
  for (v=vs; v!=null; v=v->next, n++) ;
  license_version_release(vs);
  return n;
}


static int error(const char* s)
{
  struct version* vs = license_version_parse_string(s);
  return vs == null;
}


static int value(const char* application,
		 enum version_comparator comparator,
		 const char* version,
		 int n,
		 const char* s)
{
  struct version* vs = license_version_parse_string(s);
  struct version* v;

  if (size(s) < n) {
    goto err;
  }

  v = vs;
  while (n--) {
    v = v->next;
  }

  if ((v->application==null) != (application==null)) {
    goto err;
  }
  if (v->application!=application && strcmp(v->application, application) != 0) {
    goto err;
  }

  if (v->comparator != comparator) {
    goto err;
  }

  if ((v->version==null) != (version==null)) {
    goto err;
  }
  if (v->version!=version && strcmp(v->version, version) != 0) {
    goto err;
  }

  license_version_release(vs);
  return true;

 err:
  license_version_release(vs);
  return false;
}


static int accepted(const char* appverstr, const char* licversstr)
{
  return license_version_is_accepted(appverstr, licversstr);
}


int main(int argc, char* argv[])
{
  char* s;


  /* non-null expected */
  assert(error(null));

  /* non-empty expected */
  assert(error(""));

  /* ok */
  s = "x";
  assert(!error(s));
  assert(size(s) == 1);
  assert(value("x", VC_NA, null, 0, s));

  /* ok */
  s = "  x  ";
  assert(!error(s));
  assert(size(s) == 1);
  assert(value("x", VC_NA, null, 0, s));

  /* ok */
  s = " foo bar ";
  assert(!error(s));
  assert(size(s) == 1);
  assert(value("foo bar", VC_NA, null, 0, s));

  /* version expected */
  assert(error("x >= "));

  /* ok */
  s = "x=1.0";
  assert(!error(s));
  assert(size(s) == 1);
  assert(value("x", VC_EQ, "1.0", 0, s));

  /* ok */
  s = " x = 1.0 ";
  assert(!error(s));
  assert(size(s) == 1);
  assert(value("x", VC_EQ, "1.0", 0, s));
  
  /* ok */
  s = "x>1.0";
  assert(!error(s));
  assert(size(s) == 1);
  assert(value("x", VC_GT, "1.0", 0, s));

  /* ok */
  s = "x>=1.0";
  assert(!error(s));
  assert(size(s) == 1);
  assert(value("x", VC_GE, "1.0", 0, s));

  /* ok */
  s = "x<1.0";
  assert(!error(s));
  assert(size(s) == 1);
  assert(value("x", VC_LT, "1.0", 0, s));

  /* ok */
  s = "x<=1.0";
  assert(!error(s));
  assert(size(s) == 1);
  assert(value("x", VC_LE, "1.0", 0, s));

  /* ok */
  s = " foo bar = baz quux ";
  assert(!error(s));
  assert(size(s) == 1);
  assert(value("foo bar", VC_EQ, "baz quux", 0, s));

  /* 2nd application expected */
  assert(error("x,"));

  /* 2nd application expected */
  assert(error("x>=1.0,"));

  /* shuould be ok, but currently broken */
  s = "foo=1.0,bar=2.0";
  assert(!error(s));
  assert(size(s) == 2);
  /* EXPECTED */
#if 0
  assert(value("foo", VC_EQ, "1.0", 0, s));
  assert(value("bar", VC_EQ, "2.0", 1, s));
#endif


  /* non-null expected */
  assert(!accepted(null, "foo=1.0"));

  /* non-null expected */
  assert(!accepted("foo=1.0", null));

  /* non-empty expected */
  assert(!accepted("", "foo=1.0"));

  /* non-empty expected */
  assert(!accepted("foo=1.0", ""));

  /* only 1 application expected for 1st arg. */
  assert(!accepted("foo=1.0,bar=2.0", "foo=1.0"));

  /* version expected for 1st arg. */
  assert(!accepted("foo", "foo=1.0"));

  /* = expected for 1st arg. */
  assert(!accepted("foo>1.0", "foo=1.0"));

  /* = expected for 1st arg. */
  assert(!accepted("foo>=1.0", "foo=1.0"));

  /* = expected for 1st arg. */
  assert(!accepted("foo<1.0", "foo=1.0"));

  /* = expected for 1st arg. */
  assert(!accepted("foo<=1.0", "foo=1.0"));

  /* ok */
  assert(accepted("foo=1.0", "foo=1.0"));

  /* ok */
  assert(accepted("foo=1.0", "foo>=1.0"));

  /* ok */
  assert(accepted("foo=1.0", "foo<=1.0"));

  /* not accepted */
  assert(!accepted("foo=1.0", "foo>1.0"));

  /* not accepted */
  assert(!accepted("foo=1.0", "foo<1.0"));

  /* ok */
  assert(accepted("foo=1.0", "foo>0.9"));

  /* ok */
  assert(accepted("foo=1.0", "foo<1.1"));

  /* ok */
  assert(accepted("foo=1.0", "foo>0.9,foo<1.1"));

  /* not accepted (2nd interpreted as and condition) */
  assert(!accepted("foo=1.0", "foo>0.9,foo>1.0"));

  /* not accepted */
  assert(!accepted("foo=1.0", "bar=1.0"));

  printf("OK!\n");

  return 0;
}

/* the end of file */
