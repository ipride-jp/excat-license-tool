/*
 * @(#)version.c
 * $Id$
 */

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "license_int.h"
#include "version.h"

#define ST_NL -1 
#define ST_NA 0 
#define ST_EQ 1 
#define ST_GT 2 
#define ST_GE 3 
#define ST_LT 4 
#define ST_LE 5
#define ST_CM 6 

struct special_token {
  const char* token;
  int type;
};


/**
 * special tokens that can be occured in version strings.
 * NB: THIS ARRAY MUST BE ORDERED DESCENDANTLY ON COMPILE TIME
 */
static struct special_token SPECIAL_TOKENS[] = {
  { ">=", ST_GE },
  { ">", ST_GT },
  { "=", ST_EQ },
  { "<=", ST_LE },
  { "<", ST_LT },
  { ",", ST_CM },
};


/*
 * tokenizes given version string.
 *
 * @param s represents the string to be tokenized.  
 * @param c represents the internal state.  it is ignored on the 1st call.
 *          *c is updated call by call.
 * @return copy of leading token if any.  caller must free it.
 */
static char* tokenize(const char* s, const char** c)
{
  size_t l;
  char* r;
  const char* p;
  int i;

  if (s == NULL) {
    s = *c;
  }

  /* skip leading whitespaces */
  while (*s!=nul && isspace(*s)) {
    s++;
  }
  if (*s == NULL) {
    return NULL;
  }

  p = s + strlen(s);
  for (i=0; i<sizeof(SPECIAL_TOKENS)/sizeof(SPECIAL_TOKENS[0]); i++) {
    const char* t = strstr(s, SPECIAL_TOKENS[i].token);
    if (t == NULL) {
      continue;
    }

    if (t == s) {
      l = strlen(SPECIAL_TOKENS[i].token);
      *c = s + l;
      r = (char*) calloc(1, l+1);
      assert(r != NULL);
      memmove(r, s, l);
      return r;
    } else {
      /* detect leftmost token */
      p = (p < t) ? p : t;
    }
  }

  /* now, *c points the beginning of the next token */
  *c = p--;

  /* remove trailing whitespaces */
  while (isspace(*p)) {
    p--;
  }
  l = p - s + 1;
  r = (char*) calloc(1, l+1);
  assert(r != NULL);
  memmove(r, s, l);

  return r;
}


static int token_type_of(const char* token)
{
  int i;

  if (token == NULL) {
    return ST_NL;
  }

  for (i=0; i<sizeof(SPECIAL_TOKENS)/sizeof(SPECIAL_TOKENS[0]); i++) {
    if (strcmp(token, SPECIAL_TOKENS[i].token) == 0) {
      return SPECIAL_TOKENS[i].type;
    }
  }

  return ST_NA;
}


struct version* license_version_parse_string(const char* verstr)
{
  struct version* vs = NULL;
  char* t = NULL;
  const char* p;

  if (verstr == NULL) {
    goto err;
  }

  t = tokenize(verstr, &p);
  while (t != NULL) {
    int tt;
    struct version* v;

    tt = token_type_of(t);
    if (tt != ST_NA) {
      /* first, application name must come */
      goto err;
    }

    v = (struct version*) calloc(1, sizeof(struct version));
    if (v == NULL) {
      goto err;
    }
    v->next = vs;
    v->application = t;
    vs = v;

    /* [app. name] ( [comp.] [ver.] )? [,]? */
    t = tokenize(NULL, &p);
    tt = token_type_of(t);
    if (tt == ST_NA) {
      /* token that follows [app.] must be [comp.] or [,] */
      goto err;
    }

    if (tt==ST_EQ || tt==ST_GE || tt==ST_GT || tt==ST_LE || tt==ST_LT) {
      free(t);
      v->comparator = tt;

      t = tokenize(NULL, &p);
      tt = token_type_of(t);
      if (tt != ST_NA) {
	/* token that follows [comp.] must be [ver.] */
	goto err;
      }

      v->version = t;

      t = tokenize(NULL, &p);
      tt = token_type_of(t);
    }

    if (tt == ST_CM) {
      free(t);
      t = tokenize(NULL, &p);
      if (t == NULL) {
	/* ver. str. mustn't end with comma */
	goto err;
      }

      continue;
    }

    if (tt != ST_NL) {
      goto err;
    }
  }

  return vs;

 err:
  if (t != NULL) free(t);
  if (vs != NULL) license_version_release(vs);

  return NULL;
}


void license_version_release(struct version* vers)
{
  struct version* v = vers;

  while (v != NULL) {
    struct version* t = v;

    if (v->application != NULL) {
      free(v->application);
    }

    if (v->version != NULL) {
      free(v->version);
    }

    v = v->next;

    free(t);
  }
}


static int rpmvercmp(const char* a, const char* b)
{
  /* TODO: replace with the implementation of rpm(8) */
  return -strcmp(a, b);
}


int license_version_is_accepted(const char* appverstr, const char* licversstr)
{
  int r = false;
  struct version* av = NULL;
  struct version* lv = NULL;
  struct version* p;

  if (appverstr==NULL || licversstr==NULL) {
    goto out;
  }

  av = license_version_parse_string(appverstr);
  if (av == NULL) {
    goto out;
  }

  lv = license_version_parse_string(licversstr);
  if (lv == NULL) {
    goto out;
  }


  /* appverstr must be in ``APPNAME = VER'' */
  if (av->next!=NULL || av->comparator!=VC_EQ || av->version==NULL) {
    goto out;
  }

  for (p=lv; p!=NULL; p=p->next) {
    int c;

    if (strcmp(av->application, lv->application) != 0) {
      /* application does not match */
      break;
    }

    if (lv->comparator==VC_NA && lv->version==NULL) {
      /* version is unspecified in licver */
      continue;
    }

    c = lv->comparator;
    switch (rpmvercmp(av->version, lv->version)) {
    case -1:
      r = c==VC_GT || c==VC_GE;
      break;
    case 0:
      r = c==VC_GE || c==VC_EQ || c==VC_LE;
      break;
    case 1:
      r = c==VC_LE || c==VC_LT;
      break;
    }

    if (!r) {
      break;
    }
  }

 out:
  if (av != NULL) license_version_release(av);
  if (lv != NULL) license_version_release(lv);

  return r;
}


/* the end of file */
