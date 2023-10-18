#include "sas.h"

#define xstrcat(a,b) strcat((char *)a,(const char *)b)
#define xstrchr(a,c) (unsigned char *)strchr((const char *)a,c)
#define xstrcmp(a,b) strcmp((const char *)a,(const char *)b)
#define xstrcpy(a,b) strcpy((char *)a,(const char *)b)
#define xstrlen(a)   strlen((const char *)a)

extern int verbose;
int sas_debug = 0;

static const unsigned char *
alias(struct sas_info *sip, struct sas_map *maps, int mapindex,
      const unsigned char *cf, const unsigned char *gw, 
      const unsigned char *pos);
static int
satisfies_constraint(struct sas_constraint *cp,
		     struct sas_map *maps, int mapindex,
		     const unsigned char *cf, const unsigned char *gw, 
		     const unsigned char *pos);

void
sas_map_ids(struct sas_map *m)
{
  int i;
  for (i = 0; m[i].v; ++i)
    {
      if (!(m[i].a = (unsigned char *)gvl_get_id(m[i].v)))
	m[i].a = m[i].v;
    }
}

struct sas_map *
sas_map_form(const unsigned char *form, int *maplen)
{
  unsigned char *tmp = NULL, *s = NULL;
  struct sas_map *maps;
  int i, postdet_flag;

  if (!form)
    return NULL;

  tmp = malloc(strlen((const char *)form)+1);

  if (verbose)
    sas_debug = 1;

  xstrcpy(tmp,form);
  for (i = 1, s = tmp; *s; ++s)
    {
      if (*s == '-')
	++i;
      else if ((*s == '}' && s[1]  && s[1] != '-') /* predet */
	       || (*s == '{' && s > tmp && s[-1] != '-') /* postdet */)
	++i;
    }

  maps = malloc((i+1)*sizeof(struct sas_map));
  maps[0].tmp_ptr_in_map0 = (char*)tmp;

  i = postdet_flag = 0;
  s = tmp;
  do
    {
      if (i)
	maps[i].tmp_ptr_in_map0 = NULL;
      if (*s == '{')
	{
	  maps[i].det = (postdet_flag ? 1 : -1);
	  *s++ = '\0';
	  maps[i].v = s;
	  s = xstrchr(s,'}');
	  if (s)
	    {
	      *s++ = '\0';
	      if (*s == '-')
		{
		  *s++ = '\0';
		  postdet_flag = 0;
		}
	      else if (*s == '{')
		postdet_flag = (maps[i].det == 1 ? 1 : 0);
	      else
		postdet_flag = 0;
	      ++i;
	    }
	  else
	    {
	      /* FIXME? Warn about missing } ? */
	      goto ret;
	    }
	}
      else
	{
	  maps[i].det = 0;
	  maps[i++].v = s;
	  while (*s && *s != '-' && *s != '{')
	    ++s;
	  if (*s == '-')
	    *s++ = '\0';
	  else if (*s == '{')
	    postdet_flag = 1;
	  /* else *s == '\0', do nothing */
	}
    }
  while (*s);

 ret:
  maps[i].v = NULL;
  if (maplen)
    *maplen = i;

  return maps;
}

unsigned char *
sas_alias_form(struct sas_info *sip, const unsigned char *form, 
	       const unsigned char *cf,
	       const unsigned char *gw,
	       const unsigned char *pos)
{
  unsigned char *tmp = malloc(strlen((const char *)form)+1), *s, *atmp;
  struct sas_map *maps;
  int i, alen, postdet_flag;

  if (verbose)
    sas_debug = 1;

  xstrcpy(tmp,form);
  for (i = 1, s = tmp; *s; ++s)
    {
      if (*s == '-')
	++i;
      else if ((*s == '}' && s[1]  && s[1] != '-') /* predet */
	       || (*s == '{' && s > tmp && s[-1] != '-') /* postdet */)
	++i;
    }

  maps = malloc((i+1)*sizeof(struct sas_map));
  i = postdet_flag = 0;
  s = tmp;
  do
    {
      if (*s == '{')
	{
	  maps[i].det = (postdet_flag ? 1 : -1);
	  *s++ = '\0';
	  maps[i].v = s;
	  s = xstrchr(s,'}');
	  if (s)
	    {
	      *s++ = '\0';
	      if (*s == '-')
		{
		  *s++ = '\0';
		  postdet_flag = 0;
		}
	      else if (*s == '{')
		postdet_flag = (maps[i].det == 1 ? 1 : 0);
	      else
		postdet_flag = 0;
	      ++i;
	    }
	  else
	    {
	      /* unclosed determinative will be warned about elsewhere */
	      return tmp;
	    }
	}
      else
	{
	  maps[i].det = 0;
	  maps[i++].v = s;
	  while (*s && *s != '-' && *s != '{')
	    ++s;
	  if (*s == '-')
	    *s++ = '\0';
	  else if (*s == '{')
	    postdet_flag = 1;
	  /* else *s == '\0', do nothing */
	}
    }
  while (*s);

  maps[i].v = NULL;

  for (alen = i = 0; maps[i].v; ++i)
    {
      maps[i].a = alias(sip,maps,i,cf,gw,pos);
      alen += xstrlen(maps[i].a) + 3; /* leave space for {}- regardless of detflag */
    }

  atmp = malloc(alen+1);

  for (i = 0, *atmp = '\0'; maps[i].v; ++i)
    {
      if (maps[i].det)
	{
	  if (maps[i].det > 0 && maps[i+1].v && maps[i+1].det != 1 && maps[i+1].v[0] != '\\')
	    sprintf((char*)atmp+xstrlen(atmp),"{%s}-",maps[i].a);
	  else
	    sprintf((char*)atmp+xstrlen(atmp),"{%s}",maps[i].a);
	}
      else
	{
	  xstrcat(atmp,maps[i].a);
	  if (maps[i+1].v && maps[i+1].det <= 0)
	    xstrcat(atmp,"-");
	}
    }

  if (verbose)
    {
      if (cf)
	{
	  if (strcmp((char*)form,(char*)atmp))
	    fprintf(stderr,"sas: aliasing %s under %s[%s]%s => %s\n",
		    form,cf,gw,(char*)(pos?pos:(unsigned char*)""), atmp);
	  else
	    fprintf(stderr,"sas: no alias for %s under %s[%s]%s\n",
		    form,cf,gw,(char*)(pos?pos:(unsigned char*)""));
	}
      else
	{
	  if (strcmp((char*)form,(char*)atmp))
	    fprintf(stderr,"sas: dumb aliasing %s => %s\n", form, atmp);
	  else
	    fprintf(stderr,"sas: no dumb alias for %s\n", form);
	}
    }

  free(maps);

  if ((s = hash_find(sip->post,atmp)))
    {
      if (xstrlen(s) > xstrlen(atmp))
	atmp = realloc(atmp,xstrlen(s)+1);
      if (verbose)
	fprintf(stderr,"sas: post-alias forces %s => %s\n", atmp, s);
      xstrcpy(atmp,s);
    }

  free(tmp);
  return atmp;
}

static const unsigned char *
alias(struct sas_info *sip, struct sas_map *maps, int mapindex,
      const unsigned char *cf, const unsigned char *gw, 
      const unsigned char *pos)
{
  List *candlist;
  struct sas_alias *ap;
  const unsigned char *global_head = NULL;
  
  if (!sip || !maps[mapindex].v 
      || !sip->cand || !(candlist = hash_find(sip->cand,maps[mapindex].v)))
    return maps[mapindex].v;
  
  for (ap = list_first(candlist); ap; ap = list_next(candlist))
    {
      struct sas_constraint *cp;
      int ok = 0, any_of = 0, one_of = 0;
      if (ap->constraints)
	{
	  for (any_of = one_of = 0, cp = list_first(ap->constraints); 
	       cp; 
	       cp = list_next(ap->constraints))
	    {
	      if (satisfies_constraint(cp,maps,mapindex,cf,gw,pos))
		{
		  ++ok;
		  if (cp->type == sc_cons_lem && !cp->u.l.neg)
		    ++one_of;
		}
	      else if (cp->type != sc_cons_lem || cp->u.l.neg)
		break;
	      else if (cp->type == sc_cons_lem && !cp->u.l.neg)
		++any_of;
	    }
	  if (ok && (!any_of || one_of) && ok + any_of == list_len(ap->constraints))
	    return ap->head;
	  else
	    {
	      char *tmp = malloc(strlen((char*)maps[mapindex].v)
				 + strlen((char*)ap->head) + 2), *nodumb = NULL;
	      sprintf(tmp, "%s>%s", maps[mapindex].v, ap->head);
	      nodumb = hash_find(sip->nodumb, (unsigned char*)tmp);
	      free(tmp);
	      if (!nodumb)
		global_head = ap->head;
	    }
	}
      else
	global_head = ap->head;
    }
  if (global_head)
    return global_head;
  else
    return maps[mapindex].v;
}

static int
satisfies_constraint(struct sas_constraint *cp,
		     struct sas_map *maps, int mapindex,
		     const unsigned char *cf, const unsigned char *gw, 
		     const unsigned char *pos)
{
  if (cp->type == sc_cons_lem)
    {
      int res = 0;
      if (cf)
	{
	  res = !xstrcmp(cf,cp->u.l.cf) 
	    && (!gw || !cp->u.l.gw || !*cp->u.l.gw || !xstrcmp(gw,cp->u.l.gw))
	    && (!pos || !cp->u.l.pos || !*cp->u.l.pos || !xstrcmp(pos,cp->u.l.pos));
	}
      return cp->u.l.neg ? !res : res;
    }
  else
    {
      int i,j;
      if (cp->u.e.pre_len)
	{
	  /* N.B.: we use aliased values for look-behind ... */
	  if (mapindex >= cp->u.e.pre_len)
	    {
	      for (i = mapindex-cp->u.e.pre_len,j=0; j < cp->u.e.pre_len; ++i,++j)
		if (xstrcmp(maps[i].a,cp->u.e.pre[j]))
		  {
		    if (cp->u.e.pre_neg)
		      break;
		    else
		      return 0;
		  }
	      if (j == cp->u.e.pre_len && cp->u.e.pre_neg)
		return 0;
	    }
	  else
	    return cp->u.e.pre_neg;
	}

      if (cp->u.e.pos_len)
	{
	  /* ... but non-aliased values for look-ahead */
	  for (i = mapindex+1,j=0; maps[i].v && j < cp->u.e.pos_len; ++i,++j)
	    if (xstrcmp(maps[i].v,cp->u.e.pos[j]))
	      {
		if (cp->u.e.pos_neg)
		  break;
		else
		  return 0;
	      }
	  if (j == cp->u.e.pos_len)
	    return !cp->u.e.pos_neg;
	  else
	    return cp->u.e.pos_neg;
	}
      return 1;
    }
}
