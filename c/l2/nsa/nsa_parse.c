#include <string.h>
#include <ctype128.h>
#include "nsa.h"

static nsa_system_override = NULL;
void
nsa_system_override_set(const char *sys)
{
  nsa_system_override = sys;
}
const char *
nsa_system_override_get(void)
{
  return nsa_system_override;
}

struct nsa_parser *
nsa_parse_init(struct nsa_context *cp)
{
  struct nsa_parser *p = new_parser();
  p->toks = list_create(LIST_DOUBLE);
  p->context = cp;
  p->pool = npool_init();
  return p;
}

/* Caller must call this when done with the result */
void
nsa_parse_term(struct nsa_result *rp)
{
  nsa_del_parser(rp->parser);
  nsa_del_result(rp);
}

void
nsa_token(struct nsa_parser *p, enum nsa_ptypes type, void *ref, const char *s)
{
  struct nsa_token *t = new_token();

  if (type == NSA_P_STOP)
    {
      t->type = NSA_T_STOP;
    }
  else
    {
      unsigned char *s2 = npool_copy((const unsigned char *)s,p->pool), *brack;
      t->type = NSA_T_GRAPHEME;
      grapheme(t) = new_grapheme();
      grapheme_overt(t) = 1;
      grapheme_text_ref(t) = new_text_ref();
      grapheme_text_ref(t)->ptype = type;
      switch (type)
	{
	case NSA_P_LEMM:
	  grapheme_text_ref(t)->t.lemmptr = ref;
	  break;
	case NSA_P_LITERAL:
	  grapheme_text_ref(t)->t.literal = ref;
	  break;
	case NSA_P_LINK:
	  grapheme_text_ref(t)->t.linkptr = ref;
	  break;
	default:
	  break;
	}
      if ((brack = (unsigned char *)strchr((const char *)s2,'('))
	  && (isdigit(*s2) || ((brack-s2)==1 && (*s2 == 'n' || *s2 == 'N'))))
	{
	  grapheme_num(t) = (char *)s2;
	  *brack++ = '\0';
	  grapheme_unit(t) = (char *)brack;
	  while (*brack && ')' != *brack)
	    ++brack;
	  *brack = '\0';
	}
    }
  list_add(p->toks,t);
}

static void
set_systems(struct nsa_parser *p)
{
  struct nsa_token *t;
  for (t = list_first(p->toks); t; t = list_next(p->toks))
    {
      if (t->type == NSA_T_AMOUNT && amount_measure(t) && amount_measure(t)->type == NSA_T_MEASURE)
	{
	  if (!measure_system(amount_measure(t)))
	    {
	      const char *sysname = NULL;
	      if (measure_sysdet(amount_measure(t)))
		{
		  struct nsa_step*m;
		  struct nsa_system *s = hash_find(p->context->sysdets,
						   nsa_grapheme_text(measure_sysdet(amount_measure(t))));
		  for (m = list_first(measure_cands(amount_measure(t)));
		       m;
		       m = list_next(measure_cands(amount_measure(t))))
		    {
		      if (!strcmp(s->n, m->sys->n))
			{
			  sysname = s->n;
			  break;
			}
		    }
		}
#define ccands(t) commodity_cands(amount_commodity(t))
	      else if (amount_commodity(t) && ccands(t))
		{
		  
		  Hash_table *acands = hash_create(list_len(ccands(t)));
		  struct nsa_step *s;
		  for (sysname = list_first(ccands(t)); 
		       sysname; 
		       sysname = list_next(ccands(t)))
		    {
		      static int defined = 1;
		      if (!hash_find(acands,(unsigned char *)sysname))
			hash_add(acands,(unsigned char *)sysname,&defined);
		    }
		  for (sysname = NULL,s = list_first(measure_cands(amount_measure(t)));
		       s;
		       s = list_next(measure_cands(amount_measure(t))))
		    {
		      if (hash_find(acands,(unsigned char *)s->sys->n))
			sysname = s->sys->n;
		    }
		}
	      else
		; /* we can't resolve the ambiguity */
	      if (sysname)
		{
		  list_free(measure_cands(amount_measure(t)),NULL);
		  measure_cands(amount_measure(t)) = NULL;
		  measure_system(amount_measure(t)) 
		    = hash_find(p->context->systems,(unsigned char *)sysname);
		}
	    }
	  if (measure_system(amount_measure(t)))
	    nsa_measure_wrapup(amount_measure(t),p);
	}
    }
}

void
nsa_parse(struct nsa_parser *p)
{
  if (verbose > 1) { nsa_divider("before nsa_sexnum", stdout); nsa_show_tokens(p,stdout); }
  nsa_sexnum(p);
  if (verbose > 1) { nsa_divider("before nsa_ucount", stdout); nsa_show_tokens(p,stdout); }
  nsa_ucount(p);
  if (verbose > 1) { nsa_divider("before nsa_quantity", stdout); nsa_show_tokens(p,stdout); }
  nsa_quantity(p);
  if (verbose > 1) { nsa_divider("before nsa_measure", stdout); nsa_show_tokens(p,stdout); }
  nsa_measure(p);
  if (verbose > 1) { nsa_divider("before nsa_amount", stdout); nsa_show_tokens(p,stdout); }
  nsa_amount(p);
  if (verbose > 1) { nsa_divider("before set_systems", stdout); nsa_show_tokens(p,stdout); }
  set_systems(p);
  if (verbose > 1) { nsa_divider("final result",stdout); }
}
