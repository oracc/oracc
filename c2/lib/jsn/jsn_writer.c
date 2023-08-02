#include <json.h>
#include <stck.h>

enum jstate { jstart , jarray , jobject , jmember , jstring , jvalue };

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
  fputc('[', jwfp);
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
  fprintf(jwfp, "\"%s\" : ", name);
}

void
jw_string(const char *s)
{
  fprintf(jwfp, "\"%s\"", jsonify((const unsigned char *)s));
}

void
jw_number(int i)
{
  fprintf(jwfp, "%d", i);
}

void
jw_true(void)
{
  fputs("true", jwfp);
}

void
jw_false(void)
{
  fputs("false", jwfp);
}

void
jw_null(void)
{
  fputs("null", jwfp);
}
