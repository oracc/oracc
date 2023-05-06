#include <stdio.h>
#include <ctype128.h>
#include "nsa.h"
char *PQ = NULL;

int checkpoints = 0;
int label = 0;
int printres = 0;
static char *curr_label;
static struct nsa_result *nsa_line(struct nsa_context *cp, char *l);

static const char *
checkpoint_location(void)
{
  static char buf[1024];
  extern const char *input_file;
  if (input_file)
    strcpy(buf,input_file);
  else
    strcpy(buf,"-");
  sprintf(buf+strlen(buf),":%d",(int)lnum);
  if (PQ || label)
    {
      if (PQ)
	sprintf(buf+strlen(buf)," (%s %s)",PQ,curr_label);
      else
	sprintf(buf+strlen(buf)," (%s)",curr_label);
    }
  return buf;
}

void
nsa_text_input(struct nsa_context *cp, FILE *in)
{
  struct nsa_result *res;
  char lp[256], *p, *endp;

  while ((p = fgets(lp,255,in)))
    {
      ++lnum;
      endp = p + strlen(p);
      while (endp > p && (endp[-1] == '\n' || endp[-1] == '\r'))
	--endp;
      *endp = '\0';
      if (*lp == '&')
	{
	  if (PQ)
	    free(PQ);
	  PQ = p = &lp[1];
	  while (*p && !isspace(*p))
	    ++p;
	  *p = '\0';
	  PQ = strdup(PQ);
	  continue;
	}
      else if (label)
	{
	  curr_label = p;
	  while (*p && !isspace(*p))
	    ++p;
	  if (*p)
	    *p++ = '\0';
	}
      if (checkpoints)
	fprintf(stderr,"nsa: begin parse of %s\n", checkpoint_location());
      if ((res = nsa_line(cp, p)))
	{
	  if (res->success)
	    {
	      if (printres)
		nsa_print(res,stdout);
	    }
	  else
	    fprintf(stderr,"nsa: parse failed: %s\n", lp);
	  nsa_parse_term(res);
	}
      /* NULL return means empty line */
    }
  if (PQ)
    {
      free(PQ);
      PQ = NULL;
    }
  nsa_del_context(cp);
}

static struct nsa_result *
nsa_line(struct nsa_context *cp, char *l)
{
  char *t = NULL;
  struct nsa_parser *parser = NULL;
  if ((t = strtok(l," \t")))
    {
      parser = nsa_parse_init(cp);
      while (1)
	{
	  if (',' != *t || t[1])
	    nsa_token(parser,NSA_P_LITERAL,t,t);
	  else
	    nsa_token(parser,NSA_P_STOP,NULL,NULL);
	  if (!(t = strtok(NULL," \t")))
	    break;
	}
      nsa_parse(parser);
      if (verbose)
	nsa_show_tokens(parser, stdout);
      return nsa_create_result(parser);
    }
  return NULL;
}
