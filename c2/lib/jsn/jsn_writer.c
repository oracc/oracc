#include <json.h>
#include <stck.h>

enum jstate { jstart , jarray , jobject , jmember , jelement , jstring , jvalue };

static FILE *jwfp;
static Stck *jstack;
static enum jstate jstate;

void
jw_init(FILE *fp)
{
  jwfp = fp;
  jstack = stck_init(32);
  jstate = jstart;
}

void
jw_term(void)
{
  stck_term(jstack);
  jstack = NULL;
}

void
jw_array_o(void)
{
  stck_push(jstack, jstate);
  jstate = jarray;
  fputc('[', jwfp);
}

void
jw_array_c(void)
{
  jstate = stck_pop(jstack);
  fputc(']', jwfp);
}

void
jw_object_o(void)
{
  stck_push(jstack, jstate);
  jstate = jobject;
  fputc('{', jwfp);
}

void
jw_object_c(void)
{
  jstate = stck_pop(jstack);
  fputc('}', jwfp);
}

void
jw_member(const char *name)
{
  if (jmember == jstate)
    fputc(',', jwfp);
  fprintf(jwfp, "\"%s\" : ", name);
  jstate = jmember;
}

void
jw_string(const char *s)
{
  if (jelement == jstate)
    fputc(',', jwfp);
  fprintf(jwfp, "\"%s\"", jsonify((const unsigned char *)s));
  if (jarray == jstate)
    jstate = jelement;
}

void
jw_number(int i)
{
  if (jelement == jstate)
    fputc(',', jwfp);
  fprintf(jwfp, "%d", i);
  if (jarray == jstate)
    jstate = jelement;
}

void
jw_true(void)
{
  if (jelement == jstate)
    fputc(',', jwfp);
  fputs("true", jwfp);
  if (jarray == jstate)
    jstate = jelement;
}

void
jw_false(void)
{
  if (jelement == jstate)
    fputc(',', jwfp);
  fputs("false", jwfp);
  if (jarray == jstate)
    jstate = jelement;
}

void
jw_null(void)
{
  if (jelement == jstate)
    fputc(',', jwfp);
  fputs("null", jwfp);
  if (jarray == jstate)
    jstate = jelement;
}
