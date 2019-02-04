#include <stdio.h>
#include <stdlib.h>
#include <ctype128.h>
#include <string.h>
#include "globals.h"
#include "cdf.h"
#include "xmlnames.h"
#include "tree.h"
#include "tokenizer.h"
#include "hash.h"
#include "list.h"
#include "warning.h"
#include "graphemes.h"
#include "npool.h"
#include "gdlopts.h"
#include "atf.h"
#include "charsets.h"
#include "memblock.h"

#define NEW_ERROR_RECOVERY

struct lang_context *saved_lang;

#define hash_lookup(keyp,tablep) hash_find(tablep,keyp)

#define C(x) #x,

struct mb *tok_mem, *s_tok_mem;

struct npool *tokpool;
#define pool_copy(x) npool_copy((x),tokpool)

extern FILE*f_graphemes;
extern int backslash_is_formvar;
extern int cbd_rules, use_legacy, use_unicode;
extern int show_toks, word_matrix;

int last_token;

int temp_no_norm = 0;

static List *meta_graphemes = NULL;

static char *one,*damaged;

int is_flag[256];
static const char *flag_chars = "!?*#";

static const char *const flag_strings[] = 
{
  "",
  "#", "?", "!", "*", 
  "#?", "#!", "#*", "!?", "?*", "!*", 
  "#?!", "#!*", "#?*", "?!*", 
  "#?!*", 
};

const char *const flag_xtfs[] = 
{
  /* */         "",
  /* # */	" g:break=\"damaged\"", 
  /* ? */	" g:queried=\"1\"", 
  /* ! */	" g:remarked=\"1\"", 
  /* * */	" g:collated=\"1\"", 
  /* #? */	" g:break=\"damaged\" g:queried=\"1\"", 
  /* #! */	" g:break=\"damaged\" g:remarked=\"1\"", 
  /* #* */	" g:break=\"damaged\" g:collated=\"1\"", 
  /* !? */	" g:remarked=\"1\" g:queried=\"1\"", 
  /* ?* */	" g:queried=\"1\" g:collated=\"1\"", 
  /* !* */	" g:remarked=\"1\" g:collated=\"1\"", 
  /* #?! */	" g:break=\"damaged\" g:queried=\"1\" g:remarked=\"1\"", 
  /* #!* */	" g:break=\"damaged\" g:remarked=\"1\" g:collated=\"1\"", 
  /* #?* */	" g:break=\"damaged\" g:queried=\"1\" g:collated=\"1\"",
  /* ?!* */	" g:queried=\"1\" g:remarked=\"1\" g:collated=\"1\"", 
  /* #?!* */	" g:break=\"damaged\" g:queried=\"1\" g:remarked=\"1\" g:collated=\"1\"", 
};

struct flags flag_info[] =
  {
    { f_none,0,0,0,0,0,NULL,NULL, },
    { f_h,   1,0,0,0,1,NULL,NULL, },
    { f_q,   0,1,0,0,1,NULL,NULL, },
    { f_b,   0,0,1,0,1,NULL,NULL, },
    { f_s,   0,0,0,1,1,NULL,NULL, },
    { f_hq,  1,1,0,0,2,NULL,NULL, },
    { f_hb,  1,0,1,0,2,NULL,NULL, },
    { f_hs,  1,0,0,1,2,NULL,NULL, },
    { f_qb,  0,1,1,0,2,NULL,NULL, },
    { f_qs,  0,1,0,1,2,NULL,NULL, },
    { f_bs,  0,0,1,1,2,NULL,NULL, },
    { f_hqb, 1,1,1,0,3,NULL,NULL, },
    { f_hbs, 1,0,1,1,3,NULL,NULL, },
    { f_hqs, 1,1,0,1,3,NULL,NULL, },
    { f_qbs, 0,1,1,1,3,NULL,NULL, },
    { f_hqbs,1,1,1,1,4,NULL,NULL, },
  };

static const char *const uflag_strings[] = 
{
  "",
  "U#", "U?", "U!", "U*", 
  "U#?", "U#!", "U#*", "U!?", "U?*", "U!*", 
  "U#?!", "U#!*", "U#?*", "U?!*", 
  "U#?!*", 
};

const char *const uflag_xtfs[] = 
{
  /* */         "",
  /* U# */	" g:uflag1=\"1\"", 
  /* U? */	" g:uflag2=\"1\"", 
  /* U! */	" g:uflag3=\"1\"", 
  /* U* */	" g:uflag4=\"1\"", 
  /* U#? */	" g:uflag1=\"1\" g:uflag2=\"1\"", 
  /* U#! */	" g:uflag1=\"1\" g:uflag3=\"1\"", 
  /* U#* */	" g:uflag1=\"1\" g:uflag4=\"1\"", 
  /* U!? */	" g:uflag3=\"1\" g:uflag2=\"1\"", 
  /* U?* */	" g:uflag2=\"1\" g:uflag4=\"1\"", 
  /* U!* */	" g:uflag3=\"1\" g:uflag4=\"1\"", 
  /* U#?! */	" g:uflag1=\"1\" g:uflag2=\"1\" g:uflag3=\"1\"", 
  /* U#!* */	" g:uflag1=\"1\" g:uflag3=\"1\" g:uflag4=\"1\"", 
  /* U#?* */	" g:uflag1=\"1\" g:uflag2=\"1\" g:uflag4=\"1\"",
  /* U?!* */	" g:uflag2=\"1\" g:uflag3=\"1\" g:uflag4=\"1\"", 
  /* U#?!* */	" g:uflag1=\"1\" g:uflag2=\"1\" g:uflag3=\"1\" g:uflag4=\"1\"", 
};

struct uflags uflag_info[] =
  {
    { uf_none,0,0,0,0,0,NULL,NULL, },
    { uf_h,   1,0,0,0,1,NULL,NULL, },
    { uf_q,   0,1,0,0,1,NULL,NULL, },
    { uf_b,   0,0,1,0,1,NULL,NULL, },
    { uf_s,   0,0,0,1,1,NULL,NULL, },
    { uf_hq,  1,1,0,0,2,NULL,NULL, },
    { uf_hb,  1,0,1,0,2,NULL,NULL, },
    { uf_hs,  1,0,0,1,2,NULL,NULL, },
    { uf_qb,  0,1,1,0,2,NULL,NULL, },
    { uf_qs,  0,1,0,1,2,NULL,NULL, },
    { uf_bs,  0,0,1,1,2,NULL,NULL, },
    { uf_hqb, 1,1,1,0,3,NULL,NULL, },
    { uf_hbs, 1,0,1,1,3,NULL,NULL, },
    { uf_hqs, 1,1,0,1,3,NULL,NULL, },
    { uf_qbs, 0,1,1,1,3,NULL,NULL, },
    { uf_hqbs,1,1,1,1,4,NULL,NULL, },
  };

static const char *const class_names[] = { T_CLASSES };

const char *const type_names[] = 
  { 
    T_MISC T_SHIFT T_O T_C T_BOUND T_GRAPH T_MODS 
    T_DISAMB T_NORM T_PROX T_OL T_VAR T_UB T_NMARK T_NOOP
  };

const char *const type_data[] =
  {
    "(nothing)",
    "&", NULL, ",", NULL, NULL, NULL,
    "%", "$", "~", 
    "{", "{{", "[", "[#", "<", "<(", "{(", "(", "<<", "_", "a(", "<(", "<$", "(=", "((",
    "}", "}}", "]", "#]", ">", ")>", ")}", ")", ">>", "_", "a)", ")>", "$>", ")", "))",
    " ", "-", "/", ":", "...", "//", ";", "(#...#)","+",".","",
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "---", "[-]","-->",
    NULL, NULL, NULL, NULL,
    NULL, NULL, "$","<(=>",
    "#sol", "#eol",
    "(:", ":)",
    "+.", "-.",
    "^^",
    "\\0",
  };

/* array of characters which can be boundaries */
static int boundary[256];

/* array of characters which can be first in a grapheme, excluding
   those which can only occur within compound graphemes */
static int is_grapheme1[256];
static const char *grapheme1_chars
  = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'|";

/* array of characters which can be medial/final in a grapheme, 
   excluding those which can only occur within compound graphemes */
static int is_grapheme2[256];
static const char *grapheme2_chars
  = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789',~@\\[]";

/* array of characters which can occur in normalized text */
static int is_norm[256];
static const char *norm_chars
  = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'|=^,";

/* array of characters which can occur in alphabetic text */
static int is_abc[256];
static const char *abc_chars
  = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'=^,.$()";

/* array of characters which can occur in morphological transcription */
static int is_morph[256];
static const char *morph_chars
  = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'|";

static int *modechars[modes_top]
  = { is_grapheme1, is_norm, is_morph, is_abc };

/* #define TEST */

static char *istr[] = { "0","1","2","3","4","5","6","7","8","9" };

#define MAX_TOKS 2048
struct token*tokens[MAX_TOKS];
struct token *static_tokens[type_top];
struct token *prox_tokens[10];
struct token *em_token;
static struct token *ilig_token, *wm_absent_token, *wm_broken_token, *wm_linecont_token;
int tokindex = 0;

static Hash_table table, *tablep;

static List *medial_info_list = NULL;
static unsigned char *medial_square(unsigned char *g, struct medial_info *mip);
static struct medial_info *new_medial_info(void);

#if 0
#define BLOCK_COUNT  1
#define BLOCK_SIZE 2048
static struct token *blocks[BLOCK_COUNT];
static int block_lastused = -1, reset_block_lastused = -1;
static int block_lastallocated = -1;
static int lastused = 0, reset_lastused = 0;
static int lastnode = 0;
#endif

static int in_icomment = 0;
static int left_square_pending = 0;

static const char *lexshorts = "#\"~|=^@";

#define SURRIMPL_MAX 8
static int surrimpl_top = -1;
static enum t_type surrimpl_stack[SURRIMPL_MAX];
static enum t_type pop_surrimpl(void);
static void push_surrimpl(enum t_type t);

/* N.B.: all 'd' args of create_token whose 'c' arg is "text" must be freeable */
static struct token *create_token(enum t_class c, enum t_type t, const void *d);
static struct token *s_create_token(enum t_class c, enum t_type t, const void *d);
static unsigned char *grapheme_parens(register unsigned char *l, unsigned char **gp);
static struct token *icmt_token(const unsigned char *tok);
static struct token *newtoken(void);
static struct token *s_newtoken(void);
static void set_matching_ago(int agc, struct token *ago);
static unsigned char *tokenize_grapheme(register unsigned char *l, unsigned char **gp, 
					unsigned char **following, enum t_type *tp);
static unsigned char *tokenize_alphabetic(register unsigned char*l, 
					  unsigned char **gp, unsigned char **following, 
					  enum t_type *tp);
static unsigned char *tokenize_normalized(register unsigned char*l, 
					  unsigned char **gp, unsigned char **following, 
					  enum t_type *tp);
#ifdef TEST
static void printer(char *string, void *data);
static void showhash(void);
#endif

int
is_uflag(unsigned char *p)
{
  return p && (strlen((const char *)p)>2) && p[0] == 0xe2 && p[1] == 0x80 && p[2] >= 0xa0 && p[2] <= 0xa3;
}

static void *
hash_insert(unsigned char *keyp, void *datap, Hash_table *tablep)
{
  if (!hash_find(tablep,keyp))
    hash_add(tablep,keyp,datap);
  return datap;
}

static struct token*
clone_token(struct token *stok)
{
  struct token*tp = newtoken();
  *tp = *stok;
  if (curr_lang)
    {
      tp->lang = curr_lang;
      texttag_register(curr_lang->fulltag);
    }
  else
    {
      tp->lang = global_lang;
    }
  return tp;
}

static struct token*
create_token_sub(struct token *tp, enum t_class c, enum t_type t, const void *d)
{
  tp->class= c;
  tp->type = t;
  tp->data = d;
  if (curr_lang)
    {
      tp->lang = curr_lang;
      if (curr_lang->altlang)
	tp->altlang = strdup(curr_lang->altlang);
      texttag_register(curr_lang->fulltag);
    }
  else
    {
      tp->lang = global_lang;
    }
  return tp;
}

static struct token*
create_token(enum t_class c, enum t_type t, const void *d)
{
  return create_token_sub(newtoken(),c,t,d);
}

static struct token*
s_create_token(enum t_class c, enum t_type t, const void *d)
{
  return create_token_sub(s_newtoken(),c,t,d);
}

/* Caller should ensure that l points at the initial paren of the 
   group that should be matched */
static unsigned char *
grapheme_parens(register unsigned char *l, unsigned char **gp)
{
  int nest = 0;
  ++l;
  while (*l && (')' != *l || nest))
    {
      if ('|' == *l)
	{
	  while ('|' != *l)
	    ++l;
	  if ('|' != *l)
	    {
	      warning("mismatched |...| within (...)");
	      *gp = NULL;
	      return l;
	    }
	  else
	    ++l;
	}
      else if ('(' == *l)
	{
	  ++nest;
	  ++l;
	}
      else if (')' == *l && nest)
	{
	  --nest;
	  ++l;
	}
      else
	++l;
    }
  if (!*l)
    {
      warning("unclosed (...) group");
      *gp = NULL;
    }
  return l;
}

static unsigned char *
lex_shorthand(unsigned char *l, int *tokindexp)
{
  unsigned char *fld;
  
  tokens[*tokindexp] = clone_token(static_tokens[cell]);
  tokens[(*tokindexp)+1] = clone_token(static_tokens[field]);
  if ('[' == *l)
    ++l;
  switch (*l)
    {
    case '=':
      fld = (unsigned char *)"!eq";
      break;
    case '#':
      fld = (unsigned char *)"!sv";
      break;
    case '"':
      fld = (unsigned char *)"!pr";
      break;
    case '~':
      fld = (unsigned char *)"!sg";
      break;
    case '|':
      fld = (unsigned char *)"!sn";
      break;
    case '^':
      fld = (unsigned char *)"!wp";
      break;
    case '@':
      fld = (unsigned char *)"!cs"; /* contained sign */
      break;
    default:
      vwarning("%s: unknown lex shorthand");
      fld = (unsigned char *)"!??";
      break;
    }
  tokens[(*tokindexp)+2] = hash_lookup(fld,&table);
  ++l;
  while (isspace(*l))
    ++l;
  *tokindexp = *tokindexp+3;
  return l;
}

static struct token*
newtoken()
{
  return mb_new(tok_mem);
}

static struct token*
s_newtoken()
{
  return mb_new(s_tok_mem);
}

enum f_type
parse_flags(unsigned char *fptr, int *nflags)
{
  int bang = 0, query = 0, hash = 0, star = 0;
  unsigned char *start = fptr;
  while (is_flag[*fptr])
    {
      switch (*fptr++)
	{
	case '!':
	  if (bang) warning("repeated ! flag");
	  else ++bang;
	  break;
	case '?':
	  if (query) warning("repeated ? flag");
	  else ++query;
	  break;
	case '#':
	  if (hash) warning("repeated # flag");
	  else ++hash;
	  break;
	case '*':
	  if (star) warning("repeated * flag");
	  else ++star;
	  break;
	default:
	  abort();
	}
    }
  *nflags = fptr - start;
  if (hash)
    {
      if (query)
	{
	  if (bang)
	    return star ? f_hqbs : f_hqb;
	  else
	    return star ? f_hqs : f_hq;
	}
      else if (bang) 
	return star ? f_hbs : f_hb;
      else
	return star ? f_hs : f_h;
    }
  else if (query)
    {
      if (bang)
	return star ? f_qbs : f_qb;
      else
	return star ? f_qs : f_q;
    }
  else if (bang)
    return star ? f_bs : f_b;
  else
    return star ? f_s : f_none;
}

enum uf_type
parse_uflags(unsigned char *fptr, int *unflags)
{
  int bang = 0, query = 0, hash = 0, star = 0;
  unsigned char *start = fptr;
  while (is_uflag(fptr))
    {
      switch (fptr[2])
	{
	case 0xa0:
	  if (bang) warning("repeated U# flag");
	  else ++hash;
	  break;
	case 0xa1:
	  if (query) warning("repeated U? flag");
	  else ++query;
	  break;
	case 0xa2:
	  if (hash) warning("repeated U! flag");
	  else ++bang;
	  break;
	case 0xa3:
	  if (star) warning("repeated U* flag");
	  else ++star;
	  break;
	default:
	  abort();
	}
      fptr += 3;
    }
  *unflags = fptr - start;
  if (hash)
    {
      if (query)
	{
	  if (bang)
	    return star ? uf_hqbs : uf_hqb;
	  else
	    return star ? uf_hqs : uf_hq;
	}
      else if (bang) 
	return star ? uf_hbs : uf_hb;
      else
	return star ? uf_hs : uf_h;
    }
  else if (query)
    {
      if (bang)
	return star ? uf_qbs : uf_qb;
      else
	return star ? uf_qs : uf_q;
    }
  else if (bang)
    return star ? uf_bs : uf_b;
  else
    return star ? uf_s : uf_none;
}

void
print_token(struct token *tp)
{
  if (tp->type==flag)
    {
      fprintf(stderr,"%s\t%s\t%s\n",
	      ((struct flags*)(tp->data))->atf,
	      class_names[tp->class],
	      type_names[tp->type]);
    }
  else if (tp->type==uflag)
    {
      fprintf(stderr,"%s\t%s\t%s\n",
	      ((struct uflags*)(tp->data))->atf,
	      class_names[tp->class],
	      type_names[tp->type]);
    }
  else if (tp->class==text)
    {
      const char *textp = NULL;
      if (tp->data)
	textp = (tp->type == norm ? (char *)tp->data : (char*)((struct grapheme*)(tp->data))->atf);
      else
	textp = type_data[tp->type];

      fprintf(stderr,"%s\t%s\t%s\t%s [%s]\n",
	      textp,
	      class_names[tp->class],
	      type_names[tp->type],
	      tp->lang->fulltag,
	      tp->lang->script
	      );
    }
  else
    {
      fprintf(stderr,"%s\t%s\t%s\n",
	      tp->type == g_corr ? (char*)((struct grapheme*)(tp->data))->atf : (char*)tp->data,
	      class_names[tp->class],
	      type_names[tp->type]);
    }
}

void
showtoks()
{
  int i;
  for (i = 0; i < tokindex; ++i)
    print_token(tokens[i]);
}

static void
set_128(int *start)
{
  int i;
  for (i = 0; i <128; ++i)
    start[i] = 1;
}

void
tokenize_init()
{
  volatile int i;
  register int c;

  tok_mem = mb_init(sizeof(struct token),1024);
  s_tok_mem = mb_init(sizeof(struct token),256);

  tokpool = npool_init();

  one = (char*)pool_copy((unsigned char *)"1");
  damaged = (char*)pool_copy((unsigned char *)"damaged");

  for (c = 0; grapheme1_chars[c]; ++c)
    is_grapheme1[(int)grapheme1_chars[c]] = 1;
  set_128(is_grapheme1+128);

  for (c = 0; grapheme2_chars[c]; ++c)
    is_grapheme2[(int)grapheme2_chars[c]] = 1;
  set_128(is_grapheme2+128);
  if (cbd_rules)
    is_grapheme2['*'] = 1;

  for (c = 0; flag_chars[c]; ++c)
    is_flag[(int)flag_chars[c]] = 1;

  for (c = 0; norm_chars[c]; ++c)
    is_norm[(int)norm_chars[c]] = 1;
  set_128(is_norm+128);

  for (c = 0; morph_chars[c]; ++c)
    is_morph[(int)morph_chars[c]] = 1;
  set_128(is_morph+128);
  
  for (c = 0; abc_chars[c]; ++c)
    is_abc[(int)abc_chars[c]] = 1;
  set_128(is_abc+128);
  for (c = 0; c < 256; ++c)
    if (isspace(c))
      is_abc[c] = 0;

  memset(boundary,'\0',256*sizeof(int));
  boundary[' '] = space;
  boundary['\t'] = space;
  boundary['-'] = hyphen;
  boundary['/'] = slash;
  boundary[':'] = colon;
  boundary['+'] = plus;
  boundary['.'] = period;

  tablep = hash_create(4999);
  table = *tablep;

  for (i = 0; i < (int)type_top; ++i)
    {
      /* fprintf(stderr,"i=%d; type_top=%d\n", i, type_top); */
      if (type_data[i])
	static_tokens[i] = hash_insert((unsigned char*)type_data[i],
				       s_create_token((!type_data[i][1]
						       && boundary[(int)*type_data[i]])
						      ? bound : meta,
						      i, type_data[i]),
				       &table);
    }

#if 1
  em_token = s_create_token(bound, hyphen, "—");
#else
  em_token = s_create_token(bound, hyphen, "--");
#endif

  for (i = 0; i < 10; ++i)
    prox_tokens[i] = s_create_token(meta, prox, ucc(istr[i]));

  ilig_token = s_create_token(bound,ilig,NULL);
  wm_absent_token = s_create_token(text,wm_absent,NULL);
  wm_broken_token = s_create_token(text,wm_broken,NULL);
  wm_linecont_token = s_create_token(text,wm_linecont,NULL);

  (void)hash_insert((unsigned char*)"!sv",s_create_token(meta,ftype,"sv"),&table);
  (void)hash_insert((unsigned char*)"!pr",s_create_token(meta,ftype,"pr"),&table);
  (void)hash_insert((unsigned char*)"!sg",s_create_token(meta,ftype,"sg"),&table);
  (void)hash_insert((unsigned char*)"!sn",s_create_token(meta,ftype,"sn"),&table);
  (void)hash_insert((unsigned char*)"!eq",s_create_token(meta,ftype,"eq"),&table);
  (void)hash_insert((unsigned char*)"!wp",s_create_token(meta,ftype,"wp"),&table);
  (void)hash_insert((unsigned char*)"!cs",s_create_token(meta,ftype,"cs"),&table);
  (void)hash_insert((unsigned char*)"!bu",s_create_token(meta,ftype,"bu"),&table);

  for (c = 0; c < f_type_top; ++c)
    {
      int i = 0;
      flag_info[c].t = s_create_token(meta,flag,&flag_info[c]);
      flag_info[c].atf = flag_strings[c];
      if (flag_info[c].h)
	{
#if 1
	  flag_info[c].a[i].a = a_g_break;
	  flag_info[c].a[i].s = damaged;
#else
	  flag_info[c].a[i] = abases[a_g_break];
	  flag_info[c].a[i].valpair[1] = flag_info[c].a[i].renpair[1] = damaged;
#endif
	  ++i;
	}
#define setup_flag(f,a_t)\
      if (flag_info[c].f) \
	{\
	  flag_info[c].a[i].a = a_t; \
	  flag_info[c].a[i].s = one; \
	  ++i;\
	}
      setup_flag(q,a_g_queried);
      setup_flag(b,a_g_remarked);
      setup_flag(s,a_g_collated);
    }

  for (c = 0; c < uf_type_top; ++c)
    {
      int i = 0;
      uflag_info[c].t = s_create_token(meta,uflag,&uflag_info[c]);
      uflag_info[c].atf = uflag_strings[c];
#define setup_uflag(f,a_t)\
      if (uflag_info[c].f) \
	{\
	  uflag_info[c].a[i].a = a_t; \
	  uflag_info[c].a[i].s = one; \
	  ++i;\
	}
      setup_uflag(h,a_g_uflag1);
      setup_uflag(q,a_g_uflag2);
      setup_uflag(b,a_g_uflag3);
      setup_uflag(s,a_g_uflag4);
    }

  meta_graphemes = list_create(LIST_SINGLE);
  medial_info_list = list_create(LIST_SINGLE);

#if 0
  reset_block_lastused = block_lastused;
  reset_lastused = lastused;
#endif
}

void
tokenize_reinit()
{
  mb_reset(tok_mem);
  tokindex = 0;
  surrimpl_top = -1;
  if (medial_info_list)
    list_free(medial_info_list,list_xfree);
  medial_info_list = list_create(LIST_SINGLE);
}

/* Entries in table which have class text must be free'able;
   they can only be free'd at the end, because most of them are
   grapheme tokens which are may be reused repeatedly over the
   run */
static void
text_free(void *vp)
{
  struct token *tp = vp;
  if (tp && tp->class == text && tp->data)
    {
      struct grapheme *gp = (struct grapheme*)tp->data;
      switch (gp->type)
	{
	case g_n:
	  free(gp->g.n.n);
	  break;
	case g_p:
	  free(gp->g.p.g);
	  break;
	case g_q:
	  free(gp->g.q.g);
	  free(gp->g.q.q);
	  break;
	default:
	  break;
	}
      free((void*)tp->data);
      tp->data = NULL;
    }
}

void
tokenize_term()
{
  npool_term(tokpool);
  tokpool = NULL;
  /* Free tablep first because it references tokens which may
     be freed by the block-free following */
  hash_free(tablep,text_free);
  list_free(meta_graphemes,free);
  list_free(medial_info_list, list_xfree);
  mb_free(tok_mem);
  mb_free(s_tok_mem);
  tok_mem = s_tok_mem = NULL;
}

static int
is_varo(register unsigned char *l)
{
  while (*l && isdigit(*l))
    ++l;
  return *l == ':';
}
static int
is_varc(register unsigned char *l)
{
  while (*l && isdigit(*l))
    ++l;
  return *l == ')';
}

static enum f_type
add_hash(enum f_type t)
{
  switch (t)
    {
    case f_q:
      return f_hq;
    case f_b:
      return f_hb;
    case f_s:
      return f_hs;
    case f_qb:
      return f_hqb;
    case f_qs:
      return f_hqs;
    case f_bs:
      return f_hbs;
    case f_qbs:
      return f_hqbs;
    default:
      return t;
    }
}

static char
first_alnum(const unsigned char *c)
{
  while (*c)
    {
      if (*c < 128 && isalnum(*c))
	return *c;
      else
	++c;
    }
  return 0;
}

static wchar_t
word_matrix_char(register unsigned char *l, wchar_t *bufp)
{
  int count = 0;
  if (*l > 127)
    {
      count = mbtowc(bufp,(char*)l,6);
      if (*bufp == WORD_MATRIX_ABSENT_CHAR || *bufp == WORD_MATRIX_BROKEN_CHAR
	  || *bufp == WORD_MATRIX_LINECONT_CHAR)
	return count;
    }
  *bufp = 0;
  return 0;
}

int
is_wordnum(unsigned char *l)
{
  while (*l == '#' || *l == '*' || *l == '?' || *l == '!')
    ++l;
  return *l && (isdigit(*l) || ('n' == *l && !is_grapheme1[l[1]]));
}

int
tok_is_closer(struct token *tp)
{
  const char *name = type_names[tp->type];
  if (!strcmp(name,"maybec"))
    return 1;
  return 0;
}

void
tokenize(register unsigned char *l,unsigned char *e)
{
  struct token *tp;
  int in_uscore = 0, hash_flag = 0 /*, hash_pending = 0*/;
  int taglen = 0;
  enum t_class last_text_or_bound = meta;
  static wchar_t wmbuf[2];
  int wm_used = 0;
  struct lang_context *lang_on_entry = curr_lang, *effective_lang = curr_lang;

  /*   unsigned char *init_l = l; */

  lang_reset();
  lstatus = tokindex = 0;
  surrimpl_top = -1;
  if (lexical && strchr(lexshorts,*l) && (isspace(l[1]) || !l[1]))
    {
      l = lex_shorthand(l,&tokindex);
      if (l == NULL)
	goto ret;
      if (strchr(lexshorts,*l) && (isspace(l[1]) || !l[1]))
	--l;
    }
  if (f_graphemes)
    {
      static int did_first_line = 0;
      if (!did_first_line)
	++did_first_line;
      else
	fputc('\n',f_graphemes);
      list_locator(f_graphemes);
      /* fprintf(f_graphemes,"%d ", lnum); */
    }
  while (*l)
    {
      if (tokindex == MAX_TOKS)
	{
	  fprintf(stderr,"%s:%d: too many tokens in line\n", file, lnum);
	  exit(2);
	}
      if (l[0] == 0xe2 && l[1]
	  && l[1] == 0xb8 && l[2]
	  && l[2] >= 0xa2 && l[2] <= 0xa5)
	{
	  /* it's a unicode half bracket */
	  if (use_unicode)
	    {
	      switch (l[2])
		{
		case 0xa2:
		  if (hash_flag)
		    warning("multiple left half-square brackets in a row");
		  hash_flag = 1;
		  break;
		case 0xa3:
		  if (!hash_flag)
		    warning("right half-square bracket with no opener");
		  hash_flag = 0;
		  break;
		case 0xa4:
		case 0xa5:
		  if (use_legacy)
		    {
		      tokens[tokindex++] = clone_token(l[2] == 0xa2 
						       ? static_tokens[hdamago] 
						       : static_tokens[hdamagc]);
		    }
		  else
		    warning("lower square brackets are not supported");
		  break;
		}
	      last_text_or_bound = meta;
	    }
	  else
	    {
	      warning("unicode half brackets only allowed with #atf: use unicode");
	    }
	  l += 3;
	}
      /* WATCHME: the new is_grapheme1[l[1]] may break some texts;
	 although ':' as punct should require following space;
	 addendum: rare but legal notation 10.:3 must allow number 
	 after punct
      */
      else if ((is_grapheme1[*l]
	   || (('*' == *l 
		|| (':' == *l 
		    && !is_varc(l+1) 
		    && (/*l[1] < 256
			  &&*/ ((!is_grapheme1[l[1]] 
			    || is_wordnum(l+1) 
			    || ('r' == l[1] && ':' == l[2]))
			    || (l[1] == 0xe2 
				&& l[2] && l[2] == 0xb8 
				&& l[3] && l[3] >= 0xa2 && l[3] <= 0xa5))))
		|| ('/' == *l && (isspace(l[1]) || !l[1] || '(' == l[1])))))
	  && ((last_text_or_bound != text)
	      || (curr_lang->mode != m_graphemic 
		  && modechars[curr_lang->mode][*l])
	      || (isdigit(*l) && ':' == l[-1]) /* looser than 10.:3 notation but probably OK */
	      ))
	{
	  unsigned char *g,*following;
	  enum t_type t;
	  if (word_matrix && (wm_used = word_matrix_char(l, wmbuf)))
	    {
	      switch (*wmbuf)
		{
		case WORD_MATRIX_ABSENT_CHAR:
		  tokens[tokindex++] = clone_token(wm_absent_token);
		  break;
		case WORD_MATRIX_BROKEN_CHAR:
		  tokens[tokindex++] = clone_token(wm_broken_token);
		  break;
		case WORD_MATRIX_LINECONT_CHAR:
		  tokens[tokindex++] = clone_token(wm_linecont_token);
		  break;
		}
	      l += wm_used;
	      last_text_or_bound = text;
	      continue;
	    }

	  if (last_text_or_bound == text)
	    {
	      if (tokens[tokindex-1]->type == g_p
		  && tokens[tokindex-1]->data
		  /*		  && !strcmp((char*)tokens[tokindex-1]->data, ":") */
		  && !strcmp((char*)((struct grapheme*)(tokens[tokindex-1]->data))->atf, ":")
		  && isdigit(first_alnum(l)))
		{
		  /* Insert implicit ligature */
		  tokens[tokindex++] = ilig_token;
		}
	      else if (curr_lang->mode == m_graphemic)
		{
		  warning("successive graphemes not allowed");
		  goto ret;
		}
	    }
	  switch (effective_lang->mode)
	    {
	    case m_graphemic:
	    m_graphemic_norm:
	      if (temp_no_norm)
		{
		  char *tag = malloc(strlen(curr_lang->fulltag)), *mtag;
		  const char *ctag = curr_lang->fulltag;
		  mtag = tag;
		  *tag = '%';
		  while (isalpha(*ctag) || '-' == *ctag)
		    *tag++ = *ctag++;
		  if (tag[-1] == '-')
		    tag[-1] = '\0';
		  else
		    *tag = '\0';
		  saved_lang = curr_lang;
		  curr_lang = lang_switch(curr_lang, mtag, NULL, file, lnum);
		  free(mtag);
		}
	      l = tokenize_grapheme(l,&g,&following,&t);
	      if (g)
		{
		  unsigned char save = *following;
		  struct medial_info *mip = NULL;
		  *following = '\0';
		  /* WATCHME: when you come back to this to implement
		     restrictions on flags inside compounds for CDLI,
		     make sure you don't break qualifiers in legacy
		     mode */
		  if (/*'|' != *g*/ !strchr((const char *)g, '|') && strpbrk((const char *)g,"[]"))
		    {
		      mip = new_medial_info();
		      g = medial_square(g, mip);
		      if (!use_legacy)
			{
			  if (mip->status)
			    vwarning("%s: square brackets not allowed inside grapheme", 
				     mip->raw);
			}
		      else
			{
			  if (mip->status == FINAL_CLOSE)
			    {
			      *l-- = save;
			      *l = '\0';
			      following = l;
			      save = ']';
			    }
			  else
			    {
			      if (mip->status & MEDIAL_CLOSE)
				{
				  if (tokindex && tokens[tokindex-1]->type == damago)
				    --tokindex;
				  else
				    {
				      int tokrover = tokindex, damagc_tok;
				      while (tokrover 
					     && tokens[tokrover-1]->type != flag
					     && tokens[tokrover-1]->class != text
					     && tokens[tokrover-1]->type != ellipsis
					     )
					--tokrover;
				      while (tokens[tokrover]->class == meta 
					     && tok_is_closer(tokens[tokrover]))
					++tokrover;
				      damagc_tok = tokrover;
				      for (tokrover = tokindex; tokrover > damagc_tok; --tokrover)
					tokens[tokrover] = tokens[tokrover-1];
				      tokens[damagc_tok] = clone_token(static_tokens[damagc]);
				      ++tokindex;
				    }
				}
			      if (mip->status & MEDIAL_OPEN)
				left_square_pending = 1;
			      *l-- = save;
			      *l = '\0';
			      following = l;
			      save = '#';
			    }
			}
		    }
		  /* suppress recognition of ':' and '/' as boundaries */
		  if ((*g != ':' && *g != '/')
		      && 0 /*NULL != (tp = hash_lookup(g,&table))*/)
		    {
		      if ((!tp->data && tp->type != wm_absent && !tp->data && tp->type != wm_linecont 
			   && tp->type != wm_broken && tp->type != ilig)
			  || (((struct grapheme*)tp->data)->gflags&GFLAGS_BAD)) 
			/* subsequent accesses of bad grapheme */
			{
			  vwarning("%s: additional instance of unknown grapheme",g);
			  status = 1;
			  /* goto ret; */
			}
		      /* else */
		      tokens[tokindex++] = clone_token(tp);

		      if (curr_lang->altlang)
			tokens[tokindex-1]->altlang = strdup(curr_lang->altlang);
		      else
			tokens[tokindex-1]->altlang = NULL;

		      if (f_graphemes)
			{
			  static unsigned char buf[1024];
			  unsigned char *insertp = buf;
			  insertp = render_g(((struct grapheme *)(tp->data))->xml,insertp,buf);
			  *insertp = '\0';
			  fprintf(f_graphemes,"%s ",buf);
			}
		    }
		  else if (('*' == *g || ':' == *g) 
			   && ('\0' == g[1]
			       || !isalnum(g[1])
			       || (g[1] == 0xe2 && g[2]
                                   && g[2] == 0xb8 && g[3]
				   && g[3] >= 0xa2 && g[3] <= 0xa5)))
		    {
		      struct token *puncttok = NULL;
		      puncttok = s_create_token(text,t,gparse(pool_copy(g),t));
		      tokens[tokindex++] = puncttok;
		    }
		  else
		    {
		      struct grapheme *gp = gparse(pool_copy(g),t);
		      if (gp && gp->type == icmt)
			{
			  struct token *icmtp = icmt_token(gp->g.s.base);
			  tokens[tokindex++] = hash_insert(pool_copy(g), icmtp, &table);
			  free(gp);
			}
		      else
			{
			  tokens[tokindex++] 
			    = hash_insert(pool_copy(g),
					  s_create_token(text,t,gp),
					  &table);
			  if (gp && (gp->gflags&GFLAGS_HAVE_SQUARE))
			    {
			      unsigned char *f2 = following;
			      *f2 = save;
			      while (*f2)
				{
				  if (*f2 == '#')
				    {
				      unsigned char save = f2[1];
				      f2[1] = '\0';
				      vwarning("%s: grapheme has [...] and #",g);
				      f2[1] = save;
				      break;
				    }
				  else if (*f2 == '*' || *f2 == '!' || *f2 == '?')
				    ++f2;
				  else
				    break;
				}
			    }
			}
		    }
		  last_text_or_bound = text;
		  *following = save;

		  if (hash_flag)
		    tokens[tokindex++] = flag_info[f_h].t;

		  if ('$' == *l && '>' != l[1])
		    {
		      if (isdigit(l[1]))
			{
			  int zone = strtol((char*)l+1,NULL,10);
			  l+=2;
			  if (zone < 10)
			    tokens[tokindex++] = prox_tokens[zone];
			  else
			    {
			      vwarning("%d: zone code too big (limited to 0..9)",zone);
			      goto ret;
			    }
			}
		      else
			{
			  vwarning("%s: malformed zone-code (should be $+digits)",l);
			  goto ret;
			}
		    }
		  if (temp_no_norm)
		    {
		      curr_lang = saved_lang;
		      temp_no_norm = 0;
		    }
		}
	      else
		{
#ifdef NEW_ERROR_RECOVERY
#if 0 /* no, this is bogus; */
		  --tokindex; /* ignore bad grapheme token */
#endif
#else
		  if (temp_no_norm)
		    {
		      curr_lang = saved_lang;
		      temp_no_norm = 0;
		    }
		  status = 1;
		  goto ret;
#endif
		}
	      break;
	    case m_normalized:
	      if (tokindex
		  && (tokens[tokindex-1]->type == deto
		      || (tokindex >= 2
			  && (tokens[tokindex-1]->type == plus
			      || tokens[tokindex-1]->type == damago)
			  && tokens[tokindex-2]->type == deto)
		  || (tokindex >= 3
		      && (tokens[tokindex-1]->type == damago
			  || tokens[tokindex-2]->type == plus)
		      && tokens[tokindex-3]->type == deto)))
		{
		  temp_no_norm = 1;
		  goto m_graphemic_norm;
		}
	      l = tokenize_normalized(l, &g, &following, &t);
	      if (g)
		{
		  if (l - g > 0)
		    {
		      unsigned char save = *following;
		      unsigned char *res = NULL;
		      *following = '\0';
		      if (!use_unicode)
			res = (unsigned char*)natf2utf((char *)g,(char*)g+strlen((const char*)g),0,
						       file, lnum);
		      else
			res = g;
		      /*FIXME: cache normalized toks in their own hash
		       */
		      if (res)
			{
			  tokens[tokindex++] = create_token(text, norm, pool_copy(res));
			  
			  if (hash_flag)
			    tokens[tokindex++] = flag_info[f_h].t;
			}
		      last_text_or_bound = text;
		      *following = save;
		    }
		  else if (('*' == *g || ':' == *g) 
			   && ('\0' == g[1] 
			       || !isalnum(g[1])
			       || (g[1] == 0xe2 && g[2]
				   && g[2] == 0xb8 && g[3]
				   && g[3] >= 0xa2 && g[3] <= 0xa5)))
		    {
		      struct token *puncttok = NULL;
		      t = g_p;
		      puncttok = s_create_token(text,t,gparse(pool_copy(g),t));
		      tokens[tokindex++] = puncttok;
		      ++l;
		    }
		  else
		    {
		      warning("unknown error in normalized text");
		      status = 1;
		      goto ret;
		    }
		}
	      else
		{
		  status = 1;
		  goto ret;
		}
	      break;
	    case m_alphabetic:
	      l = tokenize_alphabetic(l, &g, &following, &t);
	      if (g)
		{
		  if (l - g > 0)
		    {
		      unsigned char save = *following;
		      unsigned char *res = NULL;
		      *following = '\0';
		      if (!use_unicode)
			res = (unsigned char *)natf2utf((char*)g,(char*)g+strlen((const char*)g),0,
							file, lnum);
		      else
			res = g;
		      
		      /*FIXME: cache alphabetic toks in their own hash
		       */
		      if (res)
			{
			  tokens[tokindex++] = create_token(text,norm,
							    pool_copy(res));
			  if (hash_flag)
			    tokens[tokindex++] = flag_info[f_h].t;
			}
		      
		      last_text_or_bound = text;
		      *following = save;
		    }
		  else if (('*' == *g || ':' == *g) && ('\0' == g[1] 
							|| isspace(g[1])
							|| (g[1] > 128 
							    || !isalnum(g[1]))))
		    {
		      struct token *puncttok = NULL;
		      char gbuf[2];
		      t = g_p;
		      *gbuf = *g; gbuf[1] = '\0';
		      puncttok = s_create_token(text,t,
						gparse(pool_copy((const unsigned char *)gbuf),t));
		      tokens[tokindex++] = puncttok;
		      last_text_or_bound = text;
		      ++l;
		    }
		  else
		    {
		      warning("unknown error in alphabetic text");
		      status = 1;
		      goto ret;
		    }
		}
	      else
		{
		  status = 1;
		  goto ret;
		}
	      break;
	    case m_morphology:
	      warning("morphology notation not yet handled");
	      goto ret;
	      break;
	    case modes_top:
	      warning("internal error: reached case mode_top");
	      exit(2);
	    }
	}
      else if ((is_flag[*l] || is_uflag(l))
	       && (last_text_or_bound == text 
		   || (last_text_or_bound == meta
		       && tokindex 
		       && tokens[tokindex-1]->type != cell
		       && tokens[tokindex-1]->type != cellspan
		       && tokens[tokindex-1]->type != field
		       ))
	       && !in_icomment)
	{
	  int nflags, unflags;
	  enum f_type type;
	  enum uf_type utype;

          type = parse_flags(l,&nflags);
          utype = parse_uflags(l,&unflags);

	  if (type != f_none)
	    {
	      if (tokindex && tokens[tokindex-1]->type == flag)
		tokens[tokindex-1] = flag_info[add_hash(type)].t;
	      else
		tokens[tokindex++] = flag_info[type].t;
	    }

	  if (utype != uf_none)
	    tokens[tokindex++] = uflag_info[utype].t;
	  
	  l += nflags+unflags;

	  /* a!(b): scan graphemes in parens and give them the regular type 
	     for their form but give them class=meta instead of class=text;
	     this means the parser/tree builder will have a sequence, e.g.,
	     text/g_v,flag/!,meta/g_s from ki!(DI)
	   */
	  if (flag_info[type].b && *l == '(')
	    {
	      unsigned char *g = l+1, *saved_l = l;
	      
	      l = grapheme_parens(l,&g);
	      if (g)
		{
		  *l++ = '\0';
		  while (1)
		    {
		      unsigned char *following, *l2,*g2;
		      enum t_type t;
		      l2 = tokenize_grapheme(g,&g2,&following,&t);
		      if (g2 && following && g2 == following)
			{
			  l[-1] = ')';
			  l = saved_l;
			  break;
			}
		      if (l2 && g2 && following)
			{
			  unsigned char save = *following;
			  struct token *gtokp = NULL;
			  *following = '\0';
			  gtokp = hash_lookup(g2,&table);
			  if (!gtokp)
			    gtokp = hash_insert(pool_copy(g2),
						s_create_token(text,t,
							     gparse(pool_copy(g2),t)),
						&table);
			  else
			    if ((!gtokp->data && gtokp->type != wm_absent && gtokp->type != wm_broken && gtokp->type != wm_linecont)
				|| (((struct grapheme*)gtokp->data)->gflags&GFLAGS_BAD))
			      /* subsequent accesses of bad grapheme */
			      {
				vwarning("%s: additional instance of unknown grapheme",g2);
				status = 1;
				/* goto ret; */
			      }

			  tp = clone_token(gtokp);
			  tp->lnum = lnum;
			  tp->class=meta;
			  tp->type=g_corr;
			  tokens[tokindex++] = tp;
			  *following = save;
			  if (*following == '?')
			    {
			      /* add query flags */
			      tokens[tokindex++] = flag_info[2].t;
			      ++following;
			    }
			  if (*following == ')')
			    break;
			  if (*following)
			    {
			      switch (*following)
				{
				case '-':
				  /* case ':': */
				  tokens[tokindex++] = clone_token(static_tokens[boundary[*following]]);
				break;
				default:
				  vwarning("%c: correction limited to graphemes and '-'",
					   *following);
				  break;
				}
			      g = l2+1;
			    }
			  else
			    break;
			}
		      else
			break;
		    }
		}
	      else
		{
		  status = 1;
		  goto ret;
		}
	    }
	  if ('$' == *l && '>' != l[1])
	    {
	      if (isdigit(l[1]))
		{
		  int zone = strtol((char *)l+1,NULL,10);
		  l+=2;
		  if (zone < 10)
		    tokens[tokindex++] = prox_tokens[zone];
		  else
		    {
		      vwarning("%d: zone code too big (limited to 0..9)",zone);
		      goto ret;
		    }
		}
	      else
		{
		  vwarning("%s: malformed zone-code (should be $+digits)",l);
		  goto ret;
		}
	    }
	}
      else 
	{
	  if (':' == *l && isdigit(l[1]))
	    {
	      unsigned char *l2 = l+2;
	      while (isdigit(*l2))
		++l2;
	      if (')' == *l2)
		{
		  struct token *t;
		  *l2 = '\0';
		  if (NULL != (t = hash_lookup(l,&table)))
		    tokens[tokindex++] = t;
		  else
		    tokens[tokindex++] 
		      = hash_insert(pool_copy(l),
				    s_create_token(meta,varc,pool_copy(l+1)),
				    &table);
		  l = l2+1;
		  last_text_or_bound = meta;
		  continue;
		}
	    }	
	retry:
	  switch (*l)
	    {
	    case ' ':
	    case '\t':
	      {
		register unsigned char *l2 = l+1;
		while (isspace(*l2))
		  ++l2;
		if ('&' == *l2 || ',' == *l2)
		  {
		    if ('&' == *l2 && ('P' == l2[1] || 'Q' == l2[1] || 'X' == l2[1]))
		      {
			unsigned char *p = l2+2;
			while (isdigit(*p))
			  ++p;
			if (p - l2 >= 8)
			  {
			    warning("ID detected in middle of line");
			    goto ret;
			  }
		      }
		    l = l2;
		    last_text_or_bound = bound;
		    goto retry;
		  }
		else if (lexical && strchr(lexshorts,*l2) && (isspace(l2[1]) || !l2[1]))
		  {
		    l = lex_shorthand(l2,&tokindex);
		    if (l == NULL)
		      goto ret;
		    if (*l && strchr(lexshorts, *l) && (isspace(l[1]) || !l[1]))
		      --l;
		    last_text_or_bound = bound;
		    continue;
		  }
		else if (bilingual && l2[0] == '=' && (isspace(l2[1]) || !l2[1]))
		  {
		    l = lex_shorthand(l2,&tokindex);
		    last_text_or_bound = bound;
		    continue;
		  }
	      }
	      /* falls through */
	    case '/':
	      if (*l == '/' && '/' == l[1])
		{
		  /* fprintf(stderr, "%d: linebreak\n", lnum); */
		  tokens[tokindex++] = static_tokens[linebreak];
		  l += 2;
		  last_text_or_bound = meta;
		  break;
		}
	      /* / = word-divider can't get here b/c it's tokenized as grapheme */
	    case '-':
	    case ':':
	    case '+':
	      if (last_text_or_bound == bound)
		{
		  /* pass silently over -- in saa_mode */
		  if (saa_mode && *l == '-' 
		      && tokindex && *(char*)tokens[tokindex-1]->data == '-')
		    {
		      tokens[tokindex-1] = em_token;
		    }
		  else if ((*l == '+' || *l == '-') && l[1] == '.')
		    {
		      extern int sentences_used;
		      /* handle inline sentence boundaries */
		      tokens[tokindex++] = clone_token(static_tokens[*l == '+' ? ub_plus : ub_minus]);
		      l += 2;
		      last_text_or_bound = meta;
		      ++sentences_used;
		      continue; /* skip the whitespace skipping */
		    }
		  else
		    {
#if 1
		      vwarning("double boundary at: %s", l);
		      goto ret;
#else
		      if (!use_legacy || !isspace(*l))
			{
			  vwarning("double boundary at: %s", l);
			  goto ret;
			}
		      /* fall through to whitespace skipping */
#endif
		    }
		}
	      else
		{
		  last_text_or_bound = bound;
		  tokens[tokindex++] = clone_token(static_tokens[boundary[*l]]);
		}
	      if (left_square_pending)
		{
		  tokens[tokindex++] = clone_token(static_tokens[damago]);
		  left_square_pending = 0;
		}
	      
	      /* don't skip whitespace if it is ': ', '+ ' etc. */
	      if (isspace(*l))
		while (isspace(*l))
		  ++l;
	      else
		++l;
	      break;
	    case '{':
	      /* suppress successive text/bound warnings */
#if 0
	      if (!strncmp(curr_lang->core->name,"egy",3))
		fprintf(stderr, "found { in %%egy\n");
#endif
	      (void)lang_push(curr_lang);
	      last_text_or_bound = meta;
	      if ('(' == l[1])
		{
		  tokens[tokindex++] = clone_token(static_tokens[smetao]);
		  l+=2;
		}
	      else if ('{' == l[1])
		{
		  tokens[tokindex++] = clone_token(static_tokens[glosso]);
		  l+=2;
		}
	      else
		{
		  tokens[tokindex++] = clone_token(static_tokens[deto]);
		  ++l;
		}
	      break;
	    case '}':
	      if (!(curr_lang = lang_pop()))
		curr_lang = text_lang;	    
	      last_text_or_bound = meta;
	      if ('}' == l[1])
		{
		  tokens[tokindex++] = clone_token(static_tokens[glossc]);
		  l+=2;
		}
	      else
		{
		  tokens[tokindex++] = clone_token(static_tokens[detc]);
		  ++l;
		}
	      break;
	    case '[':
	      tokens[tokindex++] = clone_token(static_tokens[damago]);
	      ++l;
	      break;
	    case ']':
	      if (!left_square_pending)
		tokens[tokindex++] = clone_token(static_tokens[damagc]);
	      else
		left_square_pending = 0;
	      ++l;
	      break;
	    case '(':
	      if ('#' == l[1] || '$' == l[1])
		{
		  /* this will rarely be used */
		  struct token *icmtp = NULL;
		  char match = l[1], *start = (char*)l+1;
		  icmtp = clone_token(static_tokens[icmt]);
		  tokens[tokindex++] = icmtp;
		  while (*l && (*l != match || l[1] != ')'))
		    ++l;
		  if (*l)
		    {
		      *l++ = '\0';
		      last_text_or_bound = meta;
		      icmtp->data = pool_copy((unsigned char *)start);
		    }
		  else
		    {
		      warning("unterminated (#...#) or ($...$)");
		      goto ret;
		    }
		}
	      else if ('(' == l[1])
		{
		  tokens[tokindex++] = clone_token(static_tokens[someo]);
		  ++l;
		}
	      else
		{
		  /* Variant processing */
		  if (isdigit(l[1]) && is_varo(l+1))
		    {
		      unsigned char *l2 = l+2;
		      while (isdigit(*l2))
			++l2;
		      if (':' == *l2)
			{
			  struct token *t;
			  *l2 = '\0';
			  if (NULL != (t = hash_lookup(l,&table)))
			    {
			      tokens[tokindex++] = t;
			      l = l2+1;
			    }
			  else
			    {
			      tokens[tokindex++] 
				= hash_insert(pool_copy(l),
					      s_create_token(meta,varo,pool_copy(l+1)),
					      &table);
			      if (l2[1] == ':' && l2[2] == ')')
				{
				  tokens[tokindex++] 
				    = hash_insert(pool_copy(l),
						  s_create_token(meta,varc,pool_copy(l+1)),
						  &table);
				  l = l2+3;
				}
			      else
				l = l2+1;
			    
			    }
			  last_text_or_bound = meta;
			  continue;
			}
		    }
		  else if ('=' == l[1])
		    {
		      unsigned char *np;
		      l += 2;
		      np = l;
		      while (*l && *l != ')')
			++l;
		      if ('\0' == *l)
			warning("unclosed normalized text starting with '(='");
		      else
			*l++ = '\0';
		      tokens[tokindex++] = clone_token(static_tokens[normo]);
		      tokens[tokindex++] = create_token(text,norm,
							pool_copy(use_unicode 
								  ? natf2utf((char *)np,
									     (char*)np+strlen((const char *)np),
									     0, file, lnum)
								  : np)
							);
		      tokens[tokindex++] = clone_token(static_tokens[normc]);
		      continue;
		    }
		  else
		    tokens[tokindex++] = clone_token(static_tokens[maybeo]);
		}
	      ++l;
	      break;
	    case ')':
	      if ('>' == l[1])
		{
		  last_text_or_bound = meta;
		  tokens[tokindex++] = clone_token(static_tokens[pop_surrimpl()]);
		  if (tokindex && tokens[tokindex-1]->type == surrc)
		    if (!(curr_lang = lang_pop()))
		      curr_lang = text_lang;
		  l+=2;
		}
	      else if ('}' == l[1])
		{
		  /*Bogosity, surely?
		    last_text_or_bound = (last_text_or_bound == bound ? text : bound);
		  */
		  if (!(curr_lang = lang_pop()))
		    curr_lang = text_lang;
		  last_text_or_bound = meta;
		  tokens[tokindex++] = clone_token(static_tokens[smetac]);
		  l+=2;
		}
	      else if (')' == l[1])
		{
		  tokens[tokindex++] = clone_token(static_tokens[somec]);
		  l+=2;
		}
	      else if (agroups && (isdigit(l[1]) || islower(l[1])))
		{
		  unsigned char agcode[6], *agcptr = agcode+1;
		  unsigned char *aptr = l+1;
		  struct token *ago, *agc;
		  while (isdigit(*aptr) || islower(*aptr))
		    {
		      if (agcptr - agcode > 4)
			{
			  *agcptr = '\0';
			  vwarning("%s: alignment-group code too long (max 4 characters)",
				   agcode);
			  goto ret;
			}
		      else
			*agcptr++ = *aptr++;
		    }
		  *agcptr = '\0';
		  *agcode = '(';
		  if ((ago = hash_lookup(agcode,&table)))
		    {
		      *agcode = ')';
		      agc = hash_lookup(agcode,&table);
		    }
		  else
		    {
		      unsigned char *code = pool_copy(agcode+1);
		      ago = hash_insert(agcode,s_create_token(meta,agroupo,code),&table);
		      *agcode = ')';
		      agc = hash_insert(agcode,s_create_token(meta,agroupc,code),&table);
		    }
		  tokens[tokindex++] = agc;
		  set_matching_ago(tokindex-1,ago);
		  l+=2;
		}
	      else
		{
		  tokens[tokindex++] = clone_token(static_tokens[maybec]);
		  ++l;
		}
	      break;
	    case '<':
	      if ('<' == l[1])
		{
		  tokens[tokindex++] = clone_token(static_tokens[exciso]);
		  l+=2;
		}
	      else if ('(' == l[1])
		{
		  if (last_text_or_bound == text 
		      || (tokindex && tokens[tokindex-1]->type == detc))
		    {
		      int lastg = tokindex-1;
		      (void)lang_push(curr_lang);
		      while (tokens[lastg]->class != text && lastg >= 0)
			--lastg;
		      if (tokens[lastg]->class == text)
			{
#if 1
			  memmove(&tokens[lastg+1], &tokens[lastg], (tokindex-lastg) * sizeof(struct token*));
			  ++tokindex;
			  tokens[lastg] = create_token(meta,surro_mark,"<(=>");
#else
			  int smark = lastg, count = tokindex - lastg;
			  lastg = tokindex;
			  while (count--)
			    tokens[tokindex++] = tokens[lastg--];
			  tokens[smark] = create_token(meta,surro_mark,"<(=>");
#endif
			}
		      tokens[tokindex++] = clone_token(static_tokens[surro]);
		      push_surrimpl(surrc);
		      last_text_or_bound = meta;
		    }
		  else
		    {
		      tokens[tokindex++] = clone_token(static_tokens[implo]);
		      push_surrimpl(implc);
		    }
		  l+=2;
		}
	      else if ('$' == l[1])
		{
		  tokens[tokindex++] = clone_token(static_tokens[eraso]);
		  l+=2;
		}
	      else
		{
		  tokens[tokindex++] = clone_token(static_tokens[supplo]);
		  ++l;
		}
	      break;
	    case '>':
	      if ('>' == l[1])
		{
		  tokens[tokindex++] = clone_token(static_tokens[excisc]);
		  l+=2;
		}
	      else
		{
		  tokens[tokindex++] = clone_token(static_tokens[supplc]);
		  ++l;
		}
	      break;
	    case '$':
	      if ('>' == l[1])
		{
		  tokens[tokindex++] = clone_token(static_tokens[erasc]);
		  l+=2;
		}
	      else
		{
		  tokens[tokindex++] = clone_token(static_tokens[sforce]);
		  ++l;
		}
	      break;
	    case '~':
	      tokens[tokindex++] = clone_token(static_tokens[lforce]);
	      ++l;
	      break;
	    case '_':
	      if (curr_lang->core->uppercase == m_logo)
		{
		  if (in_uscore)
		    {
		      logo_lang = NULL;
		      curr_lang->altlang = NULL;
		      effective_lang = curr_lang;
		    }
		  else
		    ; /* don't do anything here, just assign language switches to altlang */
		}
	      else
		{
		  if (in_uscore)
		    {
		      if (!(curr_lang = lang_pop()))
			curr_lang = text_lang;
		    }
		  else
		    {
		      (void)lang_push(curr_lang);
		      if (!(curr_lang = lang_switch(NULL, 
						    curr_lang->altlang 
						    ? curr_lang->altlang 
						    : curr_lang->core->altlang,
						    NULL, file, lnum)))
			curr_lang = text_lang;
		    }
		}
	      in_uscore = !in_uscore;
	      ++l;
	      break;
	    case '%':
	      if (in_uscore && curr_lang->core->uppercase == m_logo)
		{
		  effective_lang = logo_lang = lang_switch(curr_lang,(const char*)l,&taglen,file,lnum);
		  curr_lang->altlang = logo_lang->tag->tag;
		}
	      else
		{
		  if (!(curr_lang = lang_switch(curr_lang,(const char*)l,&taglen,file,lnum)))
		    effective_lang = curr_lang = text_lang;
		  else
		    effective_lang = curr_lang;
		}
	      l += taglen;
	      while (isspace(*l))
		++l;
	      break;
	    case '&':
	      tokens[tokindex++] = clone_token(static_tokens[cell]);
	      ++l;
	      if (isdigit(*l))
		{
		  unsigned char *n = l;
		  while (isdigit(*l))
		    ++l;
		  if (!isspace(*l) && *l != '!')
		    warning("cell-span (&<digits>) must be followed by space or !");
		  else
		    {
		      char lch = *l;
		      *l = '\0';
		      tokens[tokindex++] = create_token(meta,cellspan,pool_copy(n));
		      *l = lch;		    
		    }
		}
	      while (isspace(*l))
		++l;
	      break;
	    case ',':
	      tokens[tokindex++] = clone_token(static_tokens[field]);
	      ++l;
	      while (isspace(*l))
		++l;
	      break;
	    case '!':
	      if (!tokindex || tokens[tokindex-1]->type != field)
		tokens[tokindex++] = clone_token(static_tokens[field]);
	      if (islower(l[1]) && islower(l[2]) && !islower(l[3]))
		{
		  unsigned char fld[4];
		  xstrncpy(fld,l,3);
		  fld[3] = '\0';
		  if (NULL != (tp = hash_lookup(fld,&table)))
		    tokens[tokindex++] = tp;
		  else
		    tokens[tokindex++] = hash_insert(pool_copy(fld),
						     s_create_token(meta,ftype,
								    pool_copy(fld+1)), 
						     &table);
		  l+=3;
		  while (isspace(*l))
		    ++l;
		}
	      else
		{
		  vwarning("bad field-type token near %s", l);
		  goto ret;
		}
	      break;
	    case ';':
	      last_text_or_bound = meta;
	      tokens[tokindex++] = clone_token(static_tokens[newline]);
	      ++l;
	      break;
	    case '.':
	      if ('.' == l[1] && '.' == l[2])
		{
		  if (left_square_pending)
		    {
		      tokens[tokindex++] = clone_token(static_tokens[damago]);
		      left_square_pending = 0;
		    }
		  tokens[tokindex++] = clone_token(static_tokens[ellipsis]);
		  l+= 3;
		  if ('.' == *l)
		    {
		      warning("ellipsis only allowed three dots (...)");
		      while ('.' == *l)
			++l;
		    }
		  last_text_or_bound = text;
		}
	      else
		{
		  if ('.' == l[1])
		    {
		      warning("ellipsis must have three dots (...)");
		      l+=2;
		      last_text_or_bound = text;
		    }
		  else
		    {
		      /*FIXME: this should probably be documented and generalized*/
		      if ((curr_lang && curr_lang->core->uppercase == m_logo)
			  || math_mode)
			{
			  /* SAA's a.a and logograms of form LU2.KUR2 get
			     mapped to grapheme groups in XTF */
			  last_text_or_bound = bound;
			  tokens[tokindex++] = clone_token(static_tokens[boundary['.']]);
			}
		      else
			{
			  warning("period only legal in compounds and ...");
			  last_text_or_bound = bound;
			}
		      if (left_square_pending)
			{
			  tokens[tokindex++] = clone_token(static_tokens[damago]);
			  left_square_pending = 0;
			}
		      ++l;
		    }
		}
	      break;
	    case '^':
	      {
		unsigned char *mark = ++l;
		while (*l && '^' != *l)
		  ++l;
		if (*l)
		  {
		    *l++ = '\0';
		    tokens[tokindex++] = create_token(meta,notemark,pool_copy(mark));
		  }
		else
		  warning("unclosed note mark");
	      }
	      break;
	    case '\\':
	      if (backslash_is_formvar)
		{
		  vwarning("orphan formvar at %s",l);
		  while (isalnum(*l) || '\\' == *l)
		    ++l;
		}
	      else
		{
		  unsigned char save = '\0', *tok;
		  tok = l;
		  while (isalnum(*l) || '\\' == *l)
		    ++l;
		  save = *l;
		  *l = '\0';
		  tokens[tokindex++] = create_token(meta,g_disamb,pool_copy(tok));
		  *l = save;
		}
	      break;
	    default:
	      vwarning("unknown token at: %s",l);
	      goto ret;
	    }
	}
    }
 ret:
#if 0
  if (line_is_unit)
    {
      if (tokens[tokindex-1]->type != ub_minus
	  && tokens[tokindex-1]->type != ub_plus)
	{
	  if (tokens[tokindex-1]->class != bound
	      && tokens[tokindex-1]->type != field
	      && tokens[tokindex-1]->type != ftype)
	    tokens[tokindex++] = clone_token(static_tokens[space]);
	  tokens[tokindex++] = clone_token(static_tokens[ub_plus]);
	}
    }
#endif
  tokens[tokindex] = NULL;
  last_token = tokindex;
  curr_lang = lang_on_entry;
  if (show_toks)
    showtoks();
}

static void
set_matching_ago(int agc, struct token *ago)
{
  int nesting = 0;
  while (agc >= 0)
    {
      if (tokens[agc]->type == maybeo)
	{
	  if (nesting)
	    --nesting;
	  else
	    {
	      tokens[agc] = ago;
	      return;
	    }
	}
      else if (tokens[agc]->type == maybec)
	{
	  ++nesting;
	}
      else
	--agc;
    }
  warning("never found matching alignment-group opener");
  return;
}

/* return a pointer to the character immediately following the grapheme */
static unsigned char *
tokenize_grapheme(register unsigned char*l, 
		  unsigned char **gp, unsigned char **following, 
		  enum t_type *tp)
{
  *gp = l;
  if ('|' == *l && l[1] != ' ')
    {
      *tp = g_c;
      while (*l && *++l != '|')
	;
      if (*l)
	{
	  if ('(' == l[1])
	    {
	      *tp = g_q;
	      l = grapheme_parens(l+1, gp);
	      if (*gp == NULL)
		return l;
	      else
		*following = ++l;
	    }
	  else
	    *following = ++l;
	}
      else
	{
	  warning("mismatched |...|");
	  *gp = NULL;
	  return l;
	}
    }
  else
    {
      if (((isdigit(*l) || (*l == 'n' && !is_grapheme2[l[1]]))
	   /* && !saa_mode && !math_mode */)
	  || '*' == *l || ':' == *l || '/' == *l || '|' == *l)
	{
	  *tp = ((*l=='*'||*l==':'||*l=='/'||*l=='|') ? g_p : g_n);
	  if ('n' == *l)
	    {
	      if ( '+' == l[1])
		{
		  l += 2;
		  if (isdigit(*l))
		    while(isdigit(*l))
		      ++l;
		  else
		    {
		      warning("expect digit after n+ in number grapheme");
		      *gp = NULL;
		      return l;
		    }
		}
	      else
		++l;
	    }
	  else if (*tp == g_p)
	    {
	      ++l;
	      if (*l == ':' || *l == '.' || *l == '\'' || *l == '"' || (*l == 'r' && l[-1] == ':' && l[1] == ':'))
		++l;
	      if ('r' == l[-1])
		++l;
	    }
	  else
	    while (isdigit(*l))
	      ++l;
	  if ('/' == *l && l[-1] != '*' && l[-1] != ':')
	    {
	      ++l;
	      while (isdigit(*l))
		++l;
	    }
	  if ('(' == *l)
	    {
	      l = grapheme_parens(l, gp);
	      if (*gp == NULL)
		return l;
	      else
		{
		  if (l[1] == '~')
		    {
		      l += 2;
		      if ('-' == *l || '+' == *l)
			{
			  ++l;
			}
		      else
			{
			  while (is_grapheme2[*l])
			    ++l;
			}
		      *following = l;
		    }
		  else if (l[1] == '@' || l[1] == '\\')
		    {
		      l += 2;
		      while (is_grapheme2[*l])
			++l;
		      *following = l;
		    }
		  else
		    *following = ++l;
		}
	    }
	  else if (l[-1] == 'n' || *tp == g_p)
	    {
	      *following = l;
	      goto ret;
	    }
	  else
	    {
	      if (math_mode || saa_mode)
		{
		  while (is_grapheme2[*l])
		    ++l;
		  *following = l;
		}
	      else
		{
		  warning("malformed number grapheme");
		  *following = NULL;
		  *gp = NULL;
		}
	      goto ret;
	    }
	}
      else
	{
	  int possibly_mixed = 0;
	  *tp = isdigit(*l) ? g_n : (u_isupper(l) ? g_s : g_v);
	  if (*tp == g_s)
	    possibly_mixed = 1;
	  while (*l)
	    {
	      if (*tp == g_s)
		{
		  if (possibly_mixed)
		    {
		      if (u_islower(l))
			*tp = g_v;
		      else if (*l == '@' || *l == '~' || *l == '\\' || isdigit(*l))
			possibly_mixed = 0;
		    }
		}
	      if (is_grapheme2[*l])
		{
		  int nbytes = u_charbytes(l);
		  if (nbytes > 0)
		    {
		      if ('~' == *l && ('-' == l[1] || '+' == l[1]))
			l += 2;
		      else if (nbytes == 3 && is_uflag(l))
			{
			  *following = l;
			  goto ret;
			}
		      else
			l += nbytes;
		    }
		  else
		    break;
		}
	      else if (*l == '(' && l[1] != '=')
		{
		  *tp = g_q;
		  l = grapheme_parens(l, gp);
		  if (*gp == NULL)
		    return l;
		  else
		    ++l;
		}
	      else
		break;
	    }
	  *following = l;
	}
    }
 ret:
  while (l[-1] == '[' || l[-1] == ']')
    {
      --l;
      *following = l;
    }
  if (gp && *gp && (l - *gp) >= 3
      && l[-3] == 0xe2
      && l[-2] == 0xb8
      && l[-1] >= 0xa2 && l[-1] <= 0xa5)
    {
      l -= 3;
      *following = l;
    }
#if 0
  while (gp && *gp && (l - *gp) >= 3
	 && is_uflag(&l[-3]))
    {
      l -= 3;
      *following = l;
    }  
#endif
  
  return l;
}

/* return a pointer to the character immediately following the word */
static unsigned char *
tokenize_alphabetic(register unsigned char*l, 
		    unsigned char **gp, unsigned char **following, 
		    enum t_type *tp)
{
  *gp = l;
  while (*l && modechars[m_alphabetic][*l]) /*!isspace(*l))*/
    {
      if (l[0] == 0xe2 && l[1]
	  && l[1] == 0xb8 && l[2]
	  && l[2] >= 0xa2 && l[2] <= 0xa5)
	break;
      ++l;
    }
  *following = l;
  return l;
}

/* return a pointer to the character immediately following the normalized word */
static unsigned char *
tokenize_normalized(register unsigned char*l, 
		    unsigned char **gp, unsigned char **following, 
		    enum t_type *tp)
{
  *gp = l;
  while (is_norm[*l] || (*l == '/' && curr_lang && curr_lang->core->code == c_qcu))
    {
      if (l[0] == 0xe2 && l[1]
	  && l[1] == 0xb8 && l[2]
	  && l[2] >= 0xa2 && l[2] <= 0xa5)
	break;
      ++l;
    }
  *following = l;
  return l;
}

static void
push_surrimpl(enum t_type t)
{
  if (surrimpl_top == SURRIMPL_MAX)
    {
      vwarning("sorry, I can't deal with <(...)> nested more than %d deep");
    }
  else
    surrimpl_stack[++surrimpl_top] = t;
}

static enum t_type 
pop_surrimpl()
{
  if (surrimpl_top >= 0)
    return surrimpl_stack[surrimpl_top--];
  else
    {
      warning("mismatched <(...)>");
      return implo;
    }
}

static struct token *
icmt_token(const unsigned char *tok)
{
  struct token *icmtp = NULL;
  unsigned char *ntok = malloc(xxstrlen(tok)+2);
  *ntok = '$';
  xstrcpy(ntok+1,tok);
  icmtp = clone_token(static_tokens[icmt]);
  icmtp->data = ntok;
  /* fprintf(stderr,"icmtp->data = %s\n", icmtp->data); */
  return icmtp;
}

static struct medial_info *
new_medial_info(void)
{
  struct medial_info *mip = calloc(1,sizeof(struct medial_info));
  return mip;
}

static unsigned char *
medial_square(unsigned char *g, struct medial_info *mip)
{
  unsigned char *dest = g, *src = g;
  unsigned char last_square = '\0';
  mip->raw = pool_copy(g);
  while (*src)
    {
      if (*src == '|')
	{
	  do
	    *dest++ = *src++;
	  while (*src && '|' != *src);
	  if (!*src)
	    break;
	}
      if (*src == '[')
	{
	  if  (*src == last_square)
	    vwarning("double [ in grapheme");
	  last_square = '[';
	  ++src;
	}
      else if (*src == ']')
	{
	  if (!last_square)
	    {
	      if (src[1])
		mip->status |= MEDIAL_CLOSE;
	      else
		mip->status |= FINAL_CLOSE;
	    }
	  else if (*src == last_square)
	    vwarning("double ] in grapheme");
	  else
	    mip->status |= MEDIAL_HASH;
	  last_square = ']';
	  ++src;
	}
      else if (*src > 0x80)
	{
	  do
	    *dest++ = *src++;
	  while (*src > 0x80);
	}
      else
	*dest++ = *src++;
    }
  if (last_square == '[')
    mip->status |= MEDIAL_OPEN;
  *dest = '\0';
  return g;
}
