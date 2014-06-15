#include <stdlib.h>
#include <stdio.h>
#include <ctype128.h>
#include <string.h>
#include <psd_base.h>
#include <dbi.h>
#include "lang.h"
#include "se.h"

struct expr_rules *rules = NULL, *ret_type_rules = NULL;

static int expr_id = 0;

#undef C
#define C(x) #x,
const char *const se_toks_names[] = 
  { 
    C(se_not_used)
    C(se_and)C(se_or)C(se_not)C(se_groupo)C(se_groupc)
    C(se_near)C(se_sans)C(se_logo)C(se_sign)
    C(se_filter)C(se_filter1)C(se_lang)C(se_index)C(se_expr)C(se_rexp)
    C(se_hyphen)C(se_space)C(se_tilde)C(se_xspace)C(se_project)
    "se_top",
    "n_record" , "n_field" , "n_word" , "n_grapheme" 
  };

const char * const near_dir_names[] = 
  {
    "n_before" , "n_after" , "n_either" ,
  };

extern enum result_granularity res_gran;
extern int textresult;
static int firstindex = 1;

int isexpr[256];
int isrexp[256];
int isatf[256];
int isnear[256];
const char *expr_nonalpha_chars="<>/-_~:;,\\+^=";
const char *rexp_chars = ".*?|[]^$";

enum se_toks default_expr_boolean = se_and;

static struct token *toks = NULL;
static struct token *last_tok = NULL;
static int ntoks = 0;

#define NTOK_ALLOC 16

extern char *strdup(const char *);

static struct token cat_toks(struct token *begin, struct token*end);
static void expr_toks(char *e);
static unsigned char *mangle_wrapper(unsigned char *s, int r, char sep);
static struct near parse_near(const char *p, const unsigned char **s);
static struct token *phase1(const char **tokptrs);
static struct token *phase2(struct token*p1toks, int *ntoks);

extern void set_index(struct token *t);
extern void set_project(struct token *t);
extern Dbi_index *curr_dip;

static void
mangle(struct token *t, char sep)
{
  int mrules = 0;
  if (t->type == se_expr)
    {
      if (t->rules)
	mrules = rules->se_manglerules;
      else
	mrules = rules->se_manglerules;
    }
  else if (t->type == se_rexp)
    mrules = KM_ATF2UTF|KM_GRAPHEME;
  else if (rules)
    mrules = rules->se_manglerules;

  if (mrules)
    t->mangled = mangle_wrapper((unsigned char *)t->data, mrules, sep);
  else
    t->mangled = t->data;
}

struct token *
add_token(enum se_toks t)
{
  static int toks_used = 0;

  if (t == se_notused)
    {
      toks_used = 0;
      return NULL;
    }

  if (toks_used == ntoks)
    {
      toks = realloc(toks, (ntoks += NTOK_ALLOC) * sizeof(struct token));
      memset(toks+toks_used,'\0',NTOK_ALLOC*sizeof(struct token));
    }
  if (toks_used)
    last_tok = &toks[toks_used];
  toks[toks_used].type = t;
  toks[toks_used].mangled = NULL;
  return &toks[toks_used++];
}

static int
se_boolean(struct token *t)
{
  return t->type == se_and 
    || t->type == se_or
    || t->type == se_not;
}

static void
default_boolean()
{
  struct token *curr_tok = NULL;
  curr_tok = add_token(se_and);
  curr_tok->data = malloc(sizeof(struct near));
  *((struct near*)curr_tok->data) 
    = parse_near(rules ? rules->default_bool_scope : "+-0t", NULL);
}

void
tokinit(void)
{
  int i;
  const char *c;
  for (i = 128; i < 256; ++i)
    isexpr[i] = 1;
  for (i = 32; i < 128; ++i)
    isexpr[i] = isalnum(i);
  for (c = expr_nonalpha_chars; *c; ++c)
    isexpr[(int)*c] = 1;
  isexpr['*'] = 1;
  isexpr['+'] = 1;
  isexpr['|'] = 1;
  isexpr['.'] = 1;
  isexpr['['] = 1;
  isexpr[']'] = 1;
  isexpr['?'] = 1;
  isexpr['\''] = 1;
  for (c = rexp_chars; *c; ++c)
    isrexp[(int)*c] = 1;
  for (c = "0123456780jJ,"; *c; ++c)
    isatf[(int)*c] = 1;
  for (c = "wltfrWLTFR"; *c; ++c)
    isnear[(int)*c] = 1;
}

const char *
scan_name(struct token *tokp, const char *s)
{
  static char buf[128];
  char *p = buf;

  if (!s)
    {
       tokp->data = strdup("");
    }
  else
    {
      while (*s && !isspace(*s))
	*p++ = *s++;
      *p = '\0';
      tokp->data = strdup(buf);
    }

  return s;
}

static void
setup_index(struct token*curr_tok)
{
  extern int l2;
  set_index(curr_tok);

  /*FIXME: if more than translation indexes start to use virtualized
    IDs this will need to be better parameterized */
  if (!l2 && !strcmp(curr_tok->data,"tra"))
    se_vids_init("tra");

  if (curr_dip->h.ht_user < d_NONE)
    {
      rules = &rulestab[curr_dip->h.ht_user];
      if (!ret_type_rules)
	ret_type_rules = rules;
      progress("se: set rules to type %s\n",
	       datum_type_names[curr_dip->h.ht_user]);
      if (firstindex)
	{
	  firstindex = 0;
	  textresult = rules->granularity == n_record;
	  res_gran = rules->granularity;
	  progress("se: result granularity = %s\n",
		   textresult ? "text" : "field");
	}
      if (rules->se_manglerules&KM_STEM)
	km_use_stemmer();
    }
}

struct token *
tokenize(const char **tokptrs, int *ntoks)
{
  struct token*p1toks = NULL;
  if (any_index)
    {
      firstindex = 1;
      (void)add_token(se_notused);
    }
  p1toks = phase1(tokptrs);
  return phase2(p1toks,ntoks);
}

static void
filter_prefix(char *fname)
{
  scan_name(add_token(!strcmp(fname,"s") ? se_sign : 
		      (!strcmp(fname,"l") ? se_logo : se_filter1)), fname);
}

static struct token *
phase1(const char **tokptrs)
{
  const char **tokptrs_orig = tokptrs;
  struct token *curr_tok = NULL;
  int groups = 0;

  while (tokptrs && *tokptrs)
    {
      char *s;
      s = (char*)*tokptrs++;
      while (*s)
	{
	  if (*s == ',' && (!s[1] || isspace(s[1])))
	    *s++ = 0x01;
	  else if (isspace(*s) && s[1] == ',' && (!s[2] || isspace(s[2])))
	    s[1] = 0x01 , s += 3;
	  else
	    ++s;
	}
    }
  tokptrs = tokptrs_orig;

  while (tokptrs && *tokptrs)
    {
      static const unsigned char *s;
      const unsigned char *start;

      s = start = (unsigned char*) *tokptrs++;
      while (s && *s)
	{
	  switch (*s)
	    {
	    case '@':
	      curr_tok = add_token(se_filter);
	      s = (unsigned char*)scan_name(curr_tok,(char*)++s);
	      {
		char *p = (char *)curr_tok->data;
		if (p[strlen(p)-1] == '@')
		  p[strlen(p)-1] = '\t';
	      }
	      break;
	    case '%':
	      curr_tok = add_token(se_lang);
	      s = (unsigned char *)scan_name(curr_tok,(char*)++s);
	      curr_tok->data = lang_switch(NULL,curr_tok->data,NULL,"query",0);
	      break;
	    case '(':
	      (void)add_token(se_groupo);
	      ++groups;
	      ++s;
	      break;
	    case ')':
	      (void)add_token(se_groupc);
	      --groups;
	      ++s;
	      break;
	    case '!':
	    case 0x01:
	      curr_tok = add_token(se_index);
	      if (*s == 0x01)
		s = (unsigned char *)scan_name(curr_tok,"cat");
	      else
		s = (unsigned char *)scan_name(curr_tok,(char*)++s);
	      setup_index(curr_tok);
	      break;
	    case '#':
	      curr_tok = add_token(se_project);
	      s = (unsigned char *)scan_name(curr_tok,(char*)++s);
	      set_project(curr_tok);
	      break;
	    case '&':
	    case ';':
	      if (!se_boolean(curr_tok))
		curr_tok = add_token(se_near);
	      curr_tok->data = malloc(sizeof(struct near));
	      ++s;
	      if ('+' == *s || '-' == *s || isdigit(*s) || isnear[*s])
		{
		  *((struct near*)curr_tok->data) = parse_near((const char*)s, &s);
		  ++s;
		}
	      else
		*((struct near*)curr_tok->data) = parse_near("0w", NULL);
	      break;
	    case ' ':
	    case '\t':
	    case '\n':
	      curr_tok = add_token(se_space);
	      ++s;
	      break;
	    default:
	      if (firstindex)
		{
		  curr_tok = add_token(se_index);
		  (void)scan_name(curr_tok,"txt");
		  setup_index(curr_tok);
		}

	      if (*s == '/')
		{
		  static char rxbuf[1024];
		  char *p;

		  p = rxbuf;
		  ++s;
		  while (*s && '/' != *s)
		    *p++ = *s++;
		  if ('/' != *s)
		    {
		      fprintf(stderr,"se: / missing at end of regexp");
		      exit(1);
		    }
		  ++s;
		  *p = '\0';

		  if (curr_tok && (curr_tok->type == se_rexp || curr_tok->type == se_expr))
		    default_boolean();

		  curr_tok = add_token(se_groupo);
		  curr_tok = add_token(se_rexp);
		  curr_tok->data = strdup(rxbuf);
		  curr_tok->flag = 1;
		  curr_tok->expr_id = ++expr_id;
		  curr_tok->rules = rules;
		  curr_tok = add_token(se_groupc);
		}
	      else if (isexpr[(unsigned)*s])
		{
		  static char exbuf[128];
		  struct se_tok_tab *setok;
		  char *p;
		  p = exbuf;
		  while (isexpr[(int)*s])
		    *p++ = *s++;
		  *p = '\0';
		  if (curr_tok 
		      /* && curr_tok->type == se_space */
		      && (NULL != (setok = setoks(exbuf,strlen(exbuf)))))
		    {
		      curr_tok = add_token(setok->tok);
		      curr_tok->data = malloc(sizeof(struct near));
		      *((struct near*)curr_tok->data) 
			= parse_near(rules ? rules->default_bool_scope : "+-0t", NULL);
		      curr_tok->rules = rules;
		      *exbuf = '\0';
		    }
		  else
		    {
		      expr_toks(exbuf);
		    }
		}
	      else
		fprintf(stderr,"%c: unrecognized token-start\n",*s++);
	    }
	}
    }
  (void)add_token(se_top);
  if (groups)
    {
      fprintf(stderr, "se: unbalanced groups: %d un%s\n", 
	      groups > 0  ? groups : groups*-1,
	      groups > 0 ? "closed" : "opened");
      exit(1);
    }
  return toks;
}

static struct near
parse_near(const char *p, const unsigned char **s)
{
  static struct near n;
  int found_lev = 0;
  if (*p == '+' && p[1] != '-')
    n.dir = n_after;
  else if (*p == '-' && p[1] != '+')
    n.dir = n_before;
  else
    n.dir = n_either;
  n.minus = n.plus = n.range = n.redup = 0;
  while (*p == '-' || *p == '+')
    {
      ++p;
      if (*p == '-')
	++n.minus;
      else
	++n.plus;
    }
  if (isdigit(*p))
    {
      n.range = atoi(p);
      while (isdigit(*p))
	++p;
    }
  if (*p)
    {
      switch (*p)
	{
	case 'r': /* record */
	case 't': /* text */
	  n.lev = n_record;
	  break;
	case 'f': /* field */
	case 'l': /* line */
	case 's': /* sentence */
	  n.lev = n_field;
	  break;
	case 'w':
	  n.lev = n_word;
	  break;
	case 'g':
	  n.lev = n_grapheme;
	  break;
	default:
	  fprintf(stderr,"se: %c: bad level in near\n",*p);
	  n.dir = n_bad_dir;
	  return n;
	  break;
	}
      found_lev = 1;
    }
  if (!found_lev)
    n.lev = rules ? rules->granularity : g_record;
  if (s)
    *s = (const unsigned char *)p;
  return n;
}

#if 0
static unsigned char *
morph_label(unsigned char *l)
{
  unsigned char *m = (unsigned char*)strchr((char*)l,'=');
  if (m && m > l && isdigit(m[-1]))
    return m;
  else
    return NULL;
}
#endif

static unsigned char *
mangle_wrapper(unsigned char *s, int r, char sep)
{
  unsigned char *mangled;
  if ('=' == sep)
    {
      mangled = (unsigned char*)strdup((const char *)keymangler(s,KM_FOLD,"search",1, NULL));
    }
  else
    {
      mangled = (unsigned char*)strdup((const char *)keymangler(s,r,"search",1, NULL));
    }
  return mangled;
}

void
final_zero(const unsigned char *g, struct token *src)
{
  if (strlen((const char *)g) > 3)
    {
      const unsigned char *e = g + strlen((const char *)g);
      if (e[-1] == 0x80 && e[-2] == 0x82 && e[-3] == 0xe2)
	{
	  /* we end in sub-0 */
	  if (strlen((const char *)g) < 6 
	      || (e[-4] < 0x80 && e[-4] > 0x89)
	      || e[-5] != 0x82
	      || e[-6] != 0xe2)
	    {
	      /* sub-0 doesn't have a digit before it */
	      char buf[128];
	      strncpy(buf,(const char *)g,strlen((const char *)g)-3);
	      strcpy(buf+(strlen((const char *)g)-3),"[\xe2\x82\x80-\xe2\x82\x89]*");
	      src->type = se_rexp;
	      src->mangled = (unsigned char*)strdup(buf);
	    }
	}
    }
}

static struct token *
phase2(struct token*p1toks, int *ntoks)
{
  struct token *src = p1toks, *dst = p1toks;
  while (src->type != se_top)
    {
      if (src->flag)
	{
	  struct token *end = src+1;
	  /* this was flagged as rexp in phase1 */
	  /* check if preceding token is an expr to catch a|ba */
	  if (src > p1toks && src[-1].type == se_expr)
	    --src,--dst;
	  while (end->type != se_top && (end->flag || end->type == se_expr))
	    ++end;
	  *dst++ = cat_toks(src,end);
	  src = end;
	}
      else if (src->type == se_space)
	{
	  if ((src == p1toks || src[1].type == se_top) 
	      || (src[-1].type != se_expr && src[-1].type != se_rexp)
	      || (src[1].type != se_expr && src[1].type != se_rexp))
	    ++src;
	  else
	    {
	      src->data = malloc(sizeof(struct near));
	      *((struct near*)src->data) = parse_near("+1g", NULL);
	      *dst++ = *src++;
	    }	  
	}
      else if (src->type == se_expr)
	{
	  const unsigned char *g;

	  if (strchr(src->data, '*'))
	    {
	      char buf[128], *s = (char*)src->data, *t = buf;
	      char *last_star = buf;
	      src->type = se_rexp;
	      buf[0] = '\0';
	      while (*s)
		{
		  if (*s == '*')
		    {
		      const char *mangled;
		      char manglebuf[128];
		      *t = '\0';
		      strcpy(manglebuf,last_star);
		      if (src->rules)
			{
			  mangled = (const char *)mangle_wrapper((unsigned char *)buf,
								 src->rules->se_manglerules,
								 '\0');
			  strcpy(last_star,mangled);
			}
		      else
			strcpy(last_star,buf);
		      t = buf + strlen(buf);
		      *t++ = '.';
		      *t++ = '*';
		      last_star = t;
		      ++s;
		    }
		  else
		    *t++ = *s++;		      
		}
	      *t = '\0';
	      src->mangled = (unsigned char*)strdup(buf);
	      g = src->mangled;
	    }
	  else
	    {
	      if (src->rules)
		{
		  src->mangled = mangle_wrapper((unsigned char*)src->data,
						src->rules->se_manglerules,'\0');
		  g = src->mangled;
		}
	      else
		g = src->data;
	    }
	  final_zero(g,src);
	  if (src == dst)
	    ++src,++dst;
	  else
	    *dst++ = *src++;
	}
      else if (src->type == se_rexp)
	{
	  final_zero(src->mangled,src);
	  if (src == dst)
	    ++src,++dst;
	  else
	    *dst++ = *src++;
	}
      else if (src == dst)
	++src,++dst;
      else
	*dst++ = *src++;
    }
  *dst++ = *src;
  *ntoks = dst - p1toks;
  for (src = p1toks; src->type != se_top; ++src)
    if (src->type == se_xspace)
      src->type = se_space;
  return p1toks;
}

static struct token
cat_toks(struct token *begin, struct token*end)
{
  int i, top, n;
  char *res;
  static struct token t;
  for (i = 0, top = end - begin; i < top; ++i)
    n += strlen((const char *)begin[i].data);
  res = malloc(n + 1);
  *res = '\0';
  for (i = 0; i < top; ++i)
    strcat(res,(const char *)begin[i].data);
  t.data = res;
  t.flag = 1;
  t.type = se_rexp;
  return t;
}

static int
regex_chars(const char *s)
{
  return (intptr_t)strpbrk(s,"|[]?+");
}

static void
expr_toks(char *e)
{
  struct token *curr_tok = last_tok;

  if (curr_tok && curr_tok->type == se_groupc)
    default_boolean();

  curr_tok = add_token(se_groupo);

  ++expr_id;
  if (!rules || !rules->sepchars)
    {
      curr_tok = add_token(regex_chars(e) ? se_rexp : se_expr);
      curr_tok->data = strdup(e);
      curr_tok->expr_id = expr_id;
      mangle(curr_tok,'\0');
    }
  else
    {
      char ebuf[128], *p = ebuf;
      while (*e && !isspace(*e))
	{
	  if (strchr(rules->sepchars, *e))
	    {
	      enum se_toks etok;
	      if (p > ebuf)
		{
		  char *colon;
		  *p = '\0';
#if 0
		  if (curr_tok && (curr_tok->type == se_rexp || curr_tok->type == se_expr))
		    default_boolean();
#endif
		  colon = strchr(ebuf,':');
		  if (colon)
		    {
		      *colon = '\0';
		      filter_prefix(ebuf);
		      memmove(ebuf,colon+1,strlen(colon+1)+1);
		    }
		  curr_tok = add_token(regex_chars(ebuf) ? se_rexp : se_expr);
		  curr_tok->data = strdup(ebuf);
		  curr_tok->expr_id = expr_id;
		  mangle(curr_tok, *e);
		  p = ebuf;
		}
	      switch (*e)
		{
		case '_':
		  etok = se_xspace;
		  break;
		case '~':
		  etok = se_tilde;
		  break;
		case '=':
		case '-':
		case '.':
		  etok = se_hyphen;
		  break;
		default:
		  fprintf(stderr,"se: rules %s has unhandled septok %c\n",
			  datum_type_names[rules->type], *e);
		  exit(1);
		}
	      curr_tok = add_token(etok);
	      curr_tok->data = malloc(sizeof(struct near));
	      *((struct near*)curr_tok->data) = parse_near("+1g", NULL);
	      ++e;
	    }
	  else
	    *p++ = *e++;
	}
      if (p > ebuf)
	{
	  char *colon;
	  *p = '\0';
#if 0
	  if (curr_tok && (curr_tok->type == se_rexp || curr_tok->type == se_expr))
	    default_boolean();
#endif
	  colon = strchr(ebuf,':');
	  if (colon)
	    {
	      *colon = '\0';
	      filter_prefix(ebuf);
	      memmove(ebuf,colon+1,strlen(colon+1)+1);
	    }
	  
	  curr_tok = add_token(regex_chars(ebuf) ? se_rexp : se_expr);
	  curr_tok->data = strdup(ebuf);
	  curr_tok->expr_id = expr_id;
	  mangle(curr_tok,'\0');
	}
    }
  curr_tok->rules = rules;
  curr_tok = add_token(se_groupc);
}

static void
print_token(struct token *tp)
{
  struct near *np;
  char buf[32];
  if (tp->expr_id)
    sprintf(buf,"(%d)",tp->expr_id);
  else
    *buf = '\0';
  switch (tp->type)
    {
    case se_near:
    case se_space:
    case se_tilde:
    case se_hyphen:
    case se_and:
    case se_or:
    case se_not:
      np = (struct near*)tp->data;
      fprintf(stderr,"%s\t%s within %d %s\n",
	      se_toks_names[tp->type],
	      near_dir_names[np->dir],
	      np->range,
	      se_toks_names[np->lev]);
      break;
    default:
      fprintf(stderr,
	      "%s%s\t%s",
	      se_toks_names[tp->type],
	      buf,
	      tp->data ? (const char *)tp->data : ""
	      );
      if (tp->mangled)
	fprintf(stderr," => %s\n",tp->mangled);
      else
	fputc('\n',stderr);
    }
}

void
showtoks(struct token *tokens, int ntoks)
{
  int i;
  for (i = 0; i < ntoks; ++i)
    print_token(&tokens[i]);
}
