#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lang.h"
#include "tokenizer.h"
#include "warning.h"

extern int gdl_fragment_ok, use_legacy;

enum t_type in_group[type_top];

enum t_group 
{
  g_block , g_boundary , g_grapheme , 
  g_opener, g_closer , g_shift, g_force, 
  g_ftype , g_flag , g_illegal ,
  t_group_top
};

const enum t_type g_block_m[] = { cell , field , nong , type_top };
const enum t_type g_ftype_m[] = { ftype , type_top };
const enum t_type g_boundary_m[] = { space , colon , hyphen , slash , plus , period ,
				     cell , field , eol , ilig , type_top };
const enum t_type g_grapheme_m[] = { g_c , g_v , g_n , g_s , g_q , g_p,
				     g_corr , g_g , ellipsis , linebreak , newline , icmt ,
				     norm, noop , g_disamb , 
				     wm_absent , wm_broken , wm_linecont , 
				     type_top };
const enum t_type g_flag_m[] = { flag, type_top };
const enum t_type g_opener_m[] = { deto , glosso , damago , hdamago , supplo , exciso ,
				   implo , smetao , maybeo , uscoreo , agroupo, 
				   surro , eraso , varo , normo , someo , type_top };
const enum t_type g_closer_m[] = { detc , glossc , damagc , hdamagc , supplc , excisc ,
				   implc , smetac , maybec , uscorec , agroupc,
				   surrc , erasc , varc , normc , somec , type_top };
const enum t_type g_shift_m[] = { percent , type_top };
const enum t_type g_force_m[] = { sforce , lforce , type_top };
const enum t_type g_illegal_m[] = { g_b , g_m , g_a , g_f ,
				    notoken , sol , prox , 
				    ub_plus , ub_minus , cellspan ,
				    notemark , 
				    type_top };

struct tok_class 
{
  enum t_group type;
  const enum t_type *members;
} token_classes[t_group_top] = 
  {
    { g_block    , g_block_m },
    { g_boundary , g_boundary_m },
    { g_grapheme , g_grapheme_m },
    { g_opener   , g_opener_m },
    { g_closer   , g_closer_m },
    { g_shift    , g_shift_m },
    { g_force    , g_force_m },
    { g_ftype    , g_ftype_m },
    { g_flag     , g_flag_m },
    { g_illegal  , g_illegal_m },
  };

struct clmap
{
  enum t_type o;
  enum t_type c;
} o2cmap[] = 
  {
    { deto    , detc },
    { glosso  , glossc },
    { damago  , damagc },
    { hdamago , hdamagc },
    { supplo  , supplc },
    { exciso  , excisc },
    { implo   , implc },
    { smetao  , smetac },
    { maybeo  , maybec },
    { uscoreo , uscorec },
    { agroupo , agroupc },
    { surro   , surrc },
    { eraso   , erasc },
    { varo    , varc },
    { normo   , normc },
    { someo   , somec },
  };

static enum t_type c2o[type_top];
static enum t_type o2c[type_top];

static int is_opener[type_top];
static int is_closer[type_top];
static int opclstate[type_top];

extern int agroups;

static enum t_type legaltoks[type_top][type_top];
static void allow_all(enum t_type t);
static void allow_group(enum t_type t, enum t_group g);
static void allow_token(enum t_type t, enum t_type ok);
static void clmap_init(void);
static void type_init(enum t_group g, int *ip);

static void
allow_all(enum t_type t)
{
  enum t_group g;
  for (g = 0; g < t_group_top; ++g)
    allow_group(t,g);
}

static void
allow_group(enum t_type t, enum t_group g)
{
  struct tok_class *gp = &token_classes[g];
  const enum t_type*tp;
  if (gp->type != g)
    abort(); /* programmer error */
  tp = gp->members;
  while (*tp < type_top)
    {
      legaltoks[t][*tp] = 1;
      ++in_group[*tp];
      ++tp;
    }
}

static void
allow_token(enum t_type t, enum t_type ok)
{
  legaltoks[t][ok] = 1;
}

static void
check_in_group()
{
  enum t_type t = 0;
  while (t < type_top)
    {
      if (!in_group[t])
	fprintf(stderr,"%s not in any group\n",type_names[t]);
      ++t;
    }
}

void
tokcheck_init()
{
  enum t_type t = 0;
  while (t < type_top)
    {
      switch (t)
	{
	case cell:
	  allow_token(t,cellspan);
	  /* falls through */
	case cellspan:
	  allow_token(t,field);
	  /* falls through */
	case field:
	  allow_token(t,ftype); /* i.e., "& !wp" is legal--OK? */
	  /* falls through */
	case ftype:
	case sol:
	case space:
	  allow_token(t,ub_plus);
	  allow_token(t,ub_minus);
	  if (use_legacy)
	    allow_token(t,damagc);
	case ilig:
	case plus: /*FIXME: putting plus here is a bit hinky, no?*/
	  allow_group(t,g_block);
	  allow_group(t,g_opener);
	  allow_group(t,g_grapheme);
	  allow_group(t,g_shift);
	  allow_group(t,g_force);
	  allow_token(t,icmt);
	  break;
	case hyphen:
	case colon:
	case period:
	  allow_group(t,g_opener);
	  /* falls through */
	case slash:
	  allow_group(t,g_grapheme);
	  allow_group(t,g_force);
	  break;
	case percent:
	  allow_group(t,g_grapheme);
	  allow_group(t,g_force);
	  allow_group(t,g_opener);
	  allow_group(t,g_closer);
	  allow_group(t,g_boundary);
	  allow_token(t,norm);
	  allow_token(t,percent);
	  break;
	case g_c:
	case g_s:
	  allow_token(t,normo);
	case g_v:
	case g_n:
	case g_p:
	case g_q:
	case g_corr:
	case g_g:
	  allow_token(t,g_disamb);
	case g_disamb:
	case norm:
	case wm_absent:
	case wm_broken:
	case wm_linecont:
	  allow_token(t,newline);
	  allow_token(t,flag);
	  allow_token(t,prox);
	  allow_group(t,g_boundary);
	  /* FIXME: this should really look ahead to check for det after opener */
	  allow_group(t,g_opener);
	  allow_group(t,g_closer);
	  allow_token(t,deto);
	  allow_token(t,glosso);
	  allow_token(t,surro);
	  allow_group(t,g_block);
	  allow_token(t,notemark);
	  allow_token(t,norm);
	  break;
	case flag:
	  allow_token(t,norm);
	  allow_token(t,prox);
	  allow_token(t,newline);
	  allow_token(t,notemark);
	case prox:
	  allow_token(t,deto);
	  allow_token(t,g_corr);
	  allow_token(t,glosso);
	  allow_token(t,smetao);
	  allow_group(t,g_closer);
	  allow_group(t,g_boundary);
	  allow_group(t,g_block);
	  allow_token(t,surro);
	  break;
	case damago:
	  if (use_legacy)
	    allow_token(t,space);
	case someo:
	case hdamago:
	case supplo:
	case exciso:
	case implo:
	case surro:
	case maybeo:
	case eraso:
	case varo:
	  allow_token(t,smetao);
	case smetao:
	  allow_token(t,glosso);
	case glosso:
	  allow_token(t,deto);
	case deto:
	  allow_token(t,uscoreo);
	  allow_group(t,g_shift);
	  allow_group(t,g_grapheme);
	  allow_group(t,g_opener);
	  break;
	case somec:
	case damagc:
	case hdamagc:
	  allow_token(t,surro);
	  allow_token(t,g_p);
	case supplc:
	case excisc:
	case implc:
	case surrc:
	case maybec:
	case glossc:
	case erasc:
	case varc:
	  allow_token(t,smetac);
	case smetac:
	  allow_token(t,detc);
	case detc:
	  allow_group(t,g_force);
	  allow_token(t,uscoreo);
	  allow_token(t,deto);
	  allow_group(t,g_closer);
	  allow_group(t,g_boundary);
	  allow_group(t,g_block);
	  allow_token(t,newline);
	  allow_token(t,notemark);
	  allow_token(t,norm);
	  allow_group(t,g_grapheme);
	  break;
	case ellipsis:
	  allow_group(t,g_boundary);
	  allow_group(t,g_closer);
	  allow_token(t,flag);
	  allow_token(t,deto);
	  allow_token(t,glosso);
	  allow_token(t,notemark);
	  break;
	case newline:
	  allow_group(t,g_boundary);
	  allow_group(t,g_opener);
	  allow_token(t,flag);
	  /*allow_token(t,damago);*/
	  break;
	case uscoreo:
	  allow_group(t,g_opener);
	  allow_group(t,g_closer);
	  allow_group(t,g_grapheme);
	  allow_group(t,g_shift);
	  break;
	case uscorec:
	  allow_group(t,g_opener);
	  allow_group(t,g_closer);
	  allow_group(t,g_boundary);
	  break;
	case agroupo:
	  allow_group(t,g_grapheme);
	  allow_token(t,agroupo);
	  break;
	case agroupc:
	  allow_group(t,g_boundary);
	  allow_token(t,agroupc);
	  break;
	case normo:
	  allow_token(t,norm);
	  break;
	case normc:
	  allow_group(t,g_boundary);
	  allow_group(t,g_closer);
	  allow_group(t,g_block);
	  break;
	case sforce:
	case lforce:
	  /* this is weak; we should check somewhere that the grapheme
	     is a g_s or g_c, or that it is a g_n or g_q with a g_s or g_c
	     principal constituent */
	  allow_group(t,g_grapheme);
	  break;
	case nong:
	case icmt:
	  allow_all(t);
	  break;
	case ub_plus:
	case ub_minus:
	  allow_token(t,space);
	  allow_token(t,eol);
	  break;
	case notemark:
	  allow_group(t,g_boundary);
	  allow_token(t,notemark);
	  break;
	case noop:
	  allow_group(t,g_opener);
	  allow_group(t,g_grapheme);
	  break;
	case g_b:
	case g_m:
	case g_a:
	case g_f:
	case notoken:
	case eol:
	case type_top:
	  /* nothing allowed after any of these */
	  break;
	  /*no default; we want the compiler error if we miss something */
	}
      ++t;
    }
  allow_token(ftype,eol); /* allow empty fields/cells at end of line */
  allow_token(field,eol);
  allow_token(cell,eol);
  allow_token(space,slash);
  allow_token(slash,space);
  allow_token(space,linebreak);
  allow_token(linebreak,space);
  allow_token(linebreak,flag);
  allow_token(deto,plus);
  allow_token(deto,ilig);
  allow_token(detc,damago);
  allow_token(detc,damagc);
  allow_token(detc,hdamago);
  allow_token(detc,hdamagc);
  allow_token(detc,deto);
  allow_token(detc,glosso);
  allow_token(damagc,deto);
  allow_token(damagc,glosso);
  allow_token(hdamagc,deto);
  allow_token(hdamagc,glosso);
  allow_token(varo,varc);
  allow_group(varc,g_grapheme);
  allow_group(detc,g_grapheme);
  allow_group(detc,g_opener);
  allow_group(glossc,g_grapheme);
  allow_group(glossc,g_opener);
  check_in_group();
  type_init(g_opener,is_opener);
  type_init(g_closer,is_closer);
  clmap_init();
}

static void
clmap_init()
{
  enum t_type t = 0;
  while (t < sizeof(o2cmap)/sizeof(struct clmap))
    {
      o2c[o2cmap[t].o] = o2cmap[t].c;
      c2o[o2cmap[t].c] = o2cmap[t].o;
      ++t;
    }
}

static void
type_init(enum t_group g, int *ip)
{
  struct tok_class *gp = &token_classes[g];
  const enum t_type*tp;
  if (gp->type != g)
    abort(); /* programmer error */
  tp = gp->members;
  while (*tp < type_top)
    ip[*tp++] = 1;
}

static int
except(enum t_type prev, enum t_type curr, int index)
{
  switch (curr)
    {
    case g_v:
    case g_s:
    case g_q:
    case g_n:
    case g_c:
      switch (prev)
	{
	case damagc:
	case hdamagc:
	case supplc:
	case excisc:
	case implc:
	  if (index > 2)
	    {
	      switch (tokens[index-2]->type)
		{
		case detc:
		case glossc:
		case smetac:
		  return 1;
		default:
		  break;
		}
	      break;
	    default:
	      break;
	    }
	}
      break;
    case supplo:
    case implo:
    case damago:
    case hdamagc:
    case exciso:
      if (index < (last_token-1)) /* or just last_token? */
	{
	  switch (prev)
	    {
	    case g_c:
	    case g_n:
	    case g_q:
	    case g_v:
	    case g_s:
	    case flag:
	      switch (tokens[index+1]->type)
		{
		case deto:
		case glosso:
		case smetao:
		  return 1;
		default:
		  break;
		}
	      break;
	    default:
	      break;
	    }
	}
      break;
    default:
      break;
    }
  return 0;
}

static const char *
set_type_name(enum t_type t)
{
  const char *p = "(unknown token)";
  switch (t)
    {
    case g_s:
    case g_n:
    case g_c:
    case g_q:
    case g_v:
      p = "grapheme";
      break;
    case sol:
      p = "start of line";
      break;
    case eol:
      p = "end of line";
      break;
    default:
      p = type_data[t];
      if (!p)
	p = type_names[t];
    }
  return p;
}

static int
check(enum t_type prev,enum t_type curr, int index)
{
  extern int in_split_word;
  if (!legaltoks[prev][curr] && !except(prev,curr,index)
      && (!in_split_word || curr != hyphen) /* allow leading - after preceding -;<EOL> */
      && (!agroups || (prev != maybeo && curr != damago))
      && (!gdl_fragment_ok || prev != sol || curr != hyphen)
      && (index && ((!tokens[index-1]->lang || tokens[index-1]->lang->mode != m_alphabetic
		     || (!tokens[index]->lang || tokens[index]->lang->mode != m_alphabetic))))
      )
    {
      const char *pname = set_type_name(prev);
      const char *cname = set_type_name(curr);
      if (!cname)
	cname = "(null)";
      else if (!*cname)
	cname = "(empty)";
      else if (!strcmp(cname," "))
	cname = "(space)";
      vwarning("illegal sequence: %s then %s", pname, cname);
      return 1;
    }
  return 0;
}

int
tokcheck()
{
  extern int in_split_word;
  int start = 1, end = last_token;
  register enum t_type t;

  if (!tokens[0])
    return 0;

  memset(opclstate,'\0',type_top*sizeof(int));
  if (last_token >= 0)
    {
      if (check(sol,tokens[0]->type,0))
	return 1;
      else if (in_split_word)
	{
	  int i = 0;
#if 1
	  while (tokens[i]->type == percent)
	    ++i;
#else
	  while (tokens[i]->type == mtype
		 || tokens[i]->type == lang)
	    ++i;
#endif
	  if (tokens[i]->type != hyphen)
	    {
	      warning("after line ending in '-;' the next line must start with '-'");
	      in_split_word = 0;
	      return 1;
	    }
	}

      if (check(tokens[last_token-1]->type,eol,0))
	return 1;
      if (is_opener[tokens[0]->type])
	++opclstate[tokens[0]->type];
      while (start < end && tokens[start])
	{
	  t = tokens[start]->type;
	  if (check(tokens[start-1]->type,t,start))
	    return 1;
	  if (is_opener[t])
	    {
	      if (opclstate[t] && t != deto)
		{
		  vwarning("nested %s not allowed",type_data[t]);
		  return 1;
		}
	      else
		++opclstate[t];
	    }
	  else if (is_closer[t])
	    {
	      enum t_type o = c2o[t];
	      if (!opclstate[o] && t != detc)
		{
		  vwarning("%s without opener", type_data[t]);
		  return 1;
		}
	      else
		--opclstate[o];
	    }
	  else if (opclstate[damago] 
		   && t == flag && ((struct flags*)tokens[start]->data)->h)
	    {
	      warning("# used inside [...]");
	    }
	  ++start;
	}
    }

  /* check unclosed openers */
  /* if (!status) */ /* new policy: report all errors */
    for (t = 0; t < type_top; ++t)
      if (opclstate[t])
	{
	  vwarning("unclosed %s", type_data[t]);
	  return 1;
	}

  return 0;
}
