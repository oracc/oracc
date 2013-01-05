#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cdf.h"
#include "block.h"
#include "blocktok.h"
#include "hash.h"
#include "pool.h"
#include "label.h"
#include "text.h"
#include "translate.h"
#include "warning.h"

static Hash_table *my_label_table = NULL;
static Hash_table *xid_to_label_table = NULL;
static char m_label[1024];
int m_label_col_index = 0;
unsigned char line_label_buf[1024];
static unsigned char frag_buf[128];
static enum block_levels frag_level = bl_top;
static char ncname[256];
static int complained_already = 0;
extern unsigned char last_label_buf[128];

void
ncname_init(void)
{
  register int i;
  memset(ncname,'\0',256);
  for (i = 0; i < 128; ++i)
    if (isalnum(i) || i == '_' || i == '.')
      ncname[i] = 1;
}

void
label_frag(struct node *current,unsigned const char *l)
{
  xstrcpy(frag_buf,l);
  /* this means that all fragments in the same text must occur at the same
     level in the hierarchy */
  if (frag_level == bl_top)
    frag_level = current->level;
  if (frag_level == TEXT)
    {
      warning("@fragment must have previous object, surface or column");
      frag_level = OBJECT;
    }
}

void
label_segtab(const char *st, unsigned const char *tok)
{
  static char div_labels[4][128];
  enum div_label_code { dlc_version , dlc_tablet , dlc_segment , dlc_top } dlc;

  if (!st)
    {
      div_labels[0][0] = div_labels[1][0] = div_labels[2][0]  = div_labels[3][0] = '\0';
      return;
    }

  if (!strcmp(st,"Seg.") && isupper(tok[0]) && islower(tok[1]))
    st = "";
  if (doctype == e_composite || *m_label)
    {
      char *have_stp = NULL;
      const char *sp;
      if (!strcmp(st, "Ver."))
	{
	  dlc = dlc_version;
	  div_labels[1][0] = div_labels[2][0] = div_labels[3][0] = '\0';
	}
      else if (!strcmp(st,"Tab."))
	dlc = dlc_tablet;
      else if (!strcmp(st,"Seg."))
	dlc = dlc_segment;
      else
	dlc = dlc_top;
      if (st && st[strlen(st)-1] == '.')
	sp = "";
      else
	sp = " ";
      sprintf(div_labels[dlc],"%s%s%s%s",st,sp,tok,*tok?",":"");
      if (dlc < dlc_top)
	{
	  if (div_labels[0][0])
	    strcpy((char*)line_label_buf,div_labels[0]);
	  else
	    *line_label_buf = '\0';
	  if (div_labels[1][0])
	    {
	      if (*line_label_buf)
		strcat((char*)line_label_buf," ");
	      sprintf((char*)line_label_buf+strlen((char*)line_label_buf),div_labels[1]);
	    }
	  if (div_labels[2][0])
	    {
	      if (*line_label_buf)
		strcat((char*)line_label_buf," ");
	      sprintf((char*)line_label_buf+strlen((char*)line_label_buf),div_labels[2]);
	    }
	  m_label_col_index = 0;
	}
      else
	{
	  strcpy((char*)line_label_buf,div_labels[dlc]);
	  have_stp = strstr(cc(line_label_buf),st);
	  if (have_stp)
	    m_label_col_index = have_stp - (char*)line_label_buf;
	}
      update_mlabel(e_div,line_label_buf);
      *line_label_buf = '\0'; /* hack of hacks */
    }
  else
    sprintf((char*)line_label_buf,"%s%s%s",st,tok,*tok?",":"");  
}

/* Note that this function is overloaded to clean the user portion of
   the ID so it contains only NCName chars */
static unsigned char *
prepend_text_id(unsigned const char *s)
{
  if (textid && s)
    {
      unsigned char *new = malloc(xxstrlen(textid)+xxstrlen(s) + 10); /* 2 + '.' + 7 for digits */
      register unsigned char *tmp = NULL;
      
      sprintf((char*)new,"%s.%s",textid,s);
      tmp = (unsigned char *)strchr((char*)new,'.'); /* don't assume textid is a valid P/Q/X */
      ++tmp;
      while (*tmp)
	if (!ncname[*tmp])
	  *tmp++ = '_';
	else
	  ++tmp;
      sprintf((char*)tmp,".%d", ++line_id);
      return new;
    }
  else
    return NULL;
}

void
reset_mlabel()
{
  *m_label = '\0';
  m_label_col_index = 0;
}

void
update_mlabel(enum e_type type, unsigned const char *tok)
{
  switch (type)
    {
    case e_object:
      /*      break; */
    case e_surface:
    case e_div:
      m_label_col_index += sprintf(m_label+m_label_col_index,"%s ",tok);
      break;
    case e_column:
      sprintf(m_label+m_label_col_index,"%s ",
	      isdigit(*tok) ? cc(roman[atoi(cc(tok))]) : cc(tok));
      break;
    case e_enum_top:
      sprintf(m_label+strlen(m_label),"%s ",tok);
      break;
    default:
      abort(); /* programmer error */
      break;
    }
}

const unsigned char *
line_label(const unsigned char *tok,
	   enum e_tu_types transtype,
	   const unsigned char *xid)
{
  const unsigned char *label = NULL;
  /* FIXME: these mallocs/strdups are probably unnecessary; 
     does the label always get saved immediately as an attr? */
  if (*line_label_buf)
    {
      label = malloc(xxstrlen(line_label_buf)+xxstrlen(tok)+2);
      sprintf((char*)label,"%s%s",line_label_buf,(char*)tok);
    }
  else if (*m_label)
    {
      label = malloc(xxstrlen(m_label)+xxstrlen(tok)+2);
      sprintf((char*)label,"%s%s",m_label,(char*)tok);
    }
  else
    {
      char *buf = malloc(xxstrlen(tok)+1);
      strcpy(buf,(char*)tok);
      label = (unsigned char *)buf;
    }
  if (check_label(label,transtype,xid))
    return label;
  else
    {
      free((void*)label);
      return NULL;
    }
}

void
reset_labels(void)
{
  complained_already = 0;
  *line_label_buf = '\0';
  *frag_buf = '\0';
  frag_level = bl_top;
  *m_label = '\0';
  hash_free(my_label_table,NULL);
  my_label_table = hash_create(1024);
  hash_free(xid_to_label_table,NULL);
  xid_to_label_table = hash_create(1024);
  reset_mlabel();
}

void
label_term(void)
{
  hash_free(my_label_table,NULL);
  hash_free(xid_to_label_table,NULL);
  my_label_table = NULL;
}

/* return NULL if label is duplicate or, for translation check, undefined
   id value if label is unique or, for translation check, defined */
unsigned const char *
check_label(unsigned const char *lab,enum e_tu_types transtype,
	    unsigned const char *xid)
{
  unsigned const char *hashdata = (xid ? xid : (unsigned const char *)"nowt"), *labelp;
  void *ok = NULL;
  extern int start_lnum;
  if (!lab || !*lab)
    {
      static unsigned char default_ret[10];
      sprintf((char*)default_ret,"%s.1",xid);
      return default_ret;
    }
  ok = hash_find(my_label_table,uc(lab));
  if (transtype)
    {
      if (!ok)
	{
	  static char tmplabel[128], primes[6];
	  char *tmp;

	  if (strncmp((char*)lab,"o ",2))
	    {
	      sprintf(tmplabel,"o %s",lab);
	      ok = hash_find(my_label_table,uc(tmplabel));
	      if (ok)
		return ok;
	    }

	  *primes = '\0';
	  strcpy(tmplabel,(const char*)lab);
	  tmp = tmplabel+strlen(tmplabel);
	  while ('\'' == tmp[-1])
	    --tmp;
	  if (*tmp)
	    {
	      strcpy(primes,tmp);
	      *tmp = '\0';
	    }
	  if (strlen(tmplabel) > 1 && isalpha(tmplabel[strlen(tmplabel)-1]))
	    {
	      tmplabel[strlen(tmplabel)-1] = '\0';
	      ok = hash_find(my_label_table,(unsigned char *)tmplabel);
	      if (ok)
		return ok;

	      strcat(tmplabel,*primes ? primes : "'");
	      ok = hash_find(my_label_table,(unsigned char *)tmplabel);
	      if (ok)
		return ok;

	      if (tmplabel[strlen(tmplabel)-1] == '\''
		  && isalpha(tmplabel[strlen(tmplabel)-1]))
		{
		  tmplabel[strlen(tmplabel)-2] = '\'';
		  tmplabel[strlen(tmplabel)-1] = '\0';
		  ok = hash_find(my_label_table,(unsigned char *)tmplabel);
		  if (ok)
		    return ok;
		}
	      tmplabel[strlen(tmplabel)-2] = '\0';
	    }

	  strcat(tmplabel,"'");
	  ok = hash_find(my_label_table,(unsigned char *)tmplabel);
	  if (ok)
	    return ok;

	  if (strlen(tmplabel) > 1 && isalpha(tmplabel[strlen(tmplabel)-1]))
	    {
	      tmplabel[strlen(tmplabel)-1] = '\0';
	      ok = hash_find(my_label_table,(unsigned char *)tmplabel);
	      if (ok)
		return ok;
	    }

	  if (isdigit(*lab) || strchr("orltreivx",*lab))
	    {
	      const char *trythese[] = { "o ","b.e. ","r ","t.e. ","l.e. ","r.e. ","e.", NULL }, **tryp;
	      char *lab2 = NULL;
	      static char tmplabel2[128] /*, primes2[6]*/;
	      strcpy(tmplabel2,(const char *)lab);
	      lab2 = tmplabel2;
	      if (*lab2 == 'r')
		lab2 += 2;

	      tmp = lab2+strlen(lab2);
	      while ('\'' == tmp[-1])
		--tmp;
	      if (*tmp)
		{
		  strcpy(primes,tmp);
		  *tmp = '\0';
		}

	      for (tryp = trythese; *tryp; ++tryp)
		{
		  strcpy(tmplabel,(const char*)*tryp);
		  strcat(tmplabel,(const char*)lab2);
		  strcat(tmplabel,*primes ? primes : "'");
		  ok = hash_find(my_label_table,(unsigned char *)tmplabel);
		  if (ok)
		    return ok;

		  strcpy(tmplabel,(const char*)*tryp);
		  strcat(tmplabel,(const char*)lab2);
		  ok = hash_find(my_label_table,(unsigned char *)tmplabel);
		  if (ok)
		    return ok;

		  strcpy(tmplabel,(const char *)*tryp);
		  strcat(tmplabel,lab2);
		  if (strlen(tmplabel) > 1 && isalpha(tmplabel[strlen(tmplabel)-1]))
		    {
		      tmplabel[strlen(tmplabel)-1] = '\0';
		      strcat(tmplabel,*primes ? primes : "'");
		      ok = hash_find(my_label_table,(unsigned char *)tmplabel);
		      if (ok)
			return ok;

		      tmplabel[strlen(tmplabel)-strlen(*primes?primes:"'")] = '\0';
		      ok = hash_find(my_label_table,(unsigned char *)tmplabel);
		      if (ok)
			return ok;
		    }
		}
	    }
	  vwarning2(file,start_lnum,"%s: translation uses undefined label",lab);
	  return NULL;
	}
    }
  else
    {
      if (ok)
	{
	  if (!complained_already)
	    {
	      vwarning2(file,start_lnum == -1 ? lnum : start_lnum,
			"%s: duplicate label (double obv/rev/col/line? One per text reported)",
			lab);
	      complained_already = 1;
	    }
	  return NULL;
	}
      ok = pool_copy(uc(hashdata));
      labelp = pool_copy(uc(lab));
      hash_add(my_label_table,labelp,ok);
      hash_add(xid_to_label_table,ok,(void*)labelp);
    }
  return ok;
}

struct label *
newlabel(char type, enum block_levels level, const void *p)
{
  struct label *lp = calloc(1,sizeof(struct label));
  lp->type = type;
  lp->level = level;
  lp->ptr = p;
  return lp;
}

/* This routine is responsible for leaving a trailing space in line_label_buf
   if it is needed in the line labels before the line number */
void
update_labels(struct node *current,enum e_tu_types transtype)
{
  struct label*ancestors[3] = { NULL , NULL, NULL };
  unsigned char idbuf[1024],*idbufp, *idtmp;
  struct node *surfnode = NULL;

  switch (current->level)
    {
    case OBJECT:
      surfnode = NULL;
      ancestors[0] = current->user;
      break;
    case SURFACE:
      surfnode = current;
      ancestors[1] = (struct label*)current->user;
      ancestors[0] = (struct label*)current->parent->user;
      break;
    case COLUMN:
      surfnode = current->parent;
      ancestors[2] = (struct label*)current->user;
      ancestors[1] = (struct label*)current->parent->user;
      ancestors[0] = (struct label*)current->parent->parent->user;
    default:
      /* can't happen */
      break;
    }

  *line_label_buf = '\0';

  if (ancestors[0])
    {
      if (ancestors[0]->type == 'b')
	{
	  const char *s = ((struct block_token*)(ancestors[0]->ptr))->nano;
	  if (s && !xstrcmp(s,"env"))
	    sprintf((char*)line_label_buf, "%s ", s);
	}
#if 0      
      else
	sprintf((char*)line_label_buf, "%s ", (const char *)ancestors[0]->ptr);	
#endif
    }

  if (*frag_buf && frag_level == OBJECT)
    sprintf((char*)line_label_buf+xxstrlen(line_label_buf), "frg.%s ", frag_buf);

  if (ancestors[1])
    {
      const char *flag = "";
      if (*(getAttr(surfnode,"g:queried")) == '1')
	flag = "?";
      else if (*(getAttr(surfnode,"g:remarked")) == '1')
	flag = "!";
      if (ancestors[1]->type == 'b')
	{
	  const char *s = ((struct block_token*)(ancestors[1]->ptr))->nano;
	  if (s)
	    sprintf((char*)line_label_buf+xxstrlen(line_label_buf), "%s%s ", s, flag);
	  else
	    {
	      sprintf((char*)line_label_buf+xxstrlen(line_label_buf), "%s%s ", 
		      ((struct block_token*)(ancestors[1]->ptr))->abbr, flag);
	    }
	}
      else
	sprintf((char*)line_label_buf+xxstrlen(line_label_buf), "%s%s ",
		(const char *)ancestors[1]->ptr, flag);
    }

  if (ancestors[2] && '1' != *(getAttr(current,"implicit")))
    sprintf((char*)line_label_buf+xxstrlen(line_label_buf),
	    "%s ", (const char *)ancestors[2]->ptr);

  if (*frag_buf && frag_level == COLUMN)
    sprintf((char*)line_label_buf+xxstrlen(line_label_buf), "frg.%s ", frag_buf);

#if 0 /* can't happen */
  if (*frag_buf && frag_level == LINE)
    sprintf((char*)line_label_buf+xxstrlen(line_label_buf), "frg.%s ", frag_buf);
#endif

  xstrcpy(idbuf,line_label_buf);
  idbufp = idbuf+xxstrlen(idbuf);
  while (idbufp > idbuf && isspace(idbufp[-1]))
    --idbufp;
  *idbufp = '\0';

  if (!*(getAttr(current,"label"))
      && '1' != *(getAttr(current,"implicit")))
    {
      if (!*idbuf)
	{
	  static int xid = 0;
	  char buf[5];
	  sprintf(buf,"x%d",xid++);
	  strcpy((char*)idbuf,buf);
	}
      setAttr(current,a_label,idbuf);
      for (idbufp = idbuf; *idbufp; ++idbufp)
	if (isspace(*idbufp))
	  *idbufp = '.';
      idtmp = prepend_text_id(idbuf);
      if (idtmp)
      	{
	  const unsigned char *tmp2 = check_label(idbuf,transtype,idtmp);
	                                            /*(unsigned char*)strdup((char*)idtmp));*/
	  setAttr(current,a_xml_id,tmp2);
	  free(idtmp);
	}
    }
  
  if (xxstrlen(line_label_buf) > 1000)
    {
      fprintf(stderr,"overfull line_label_buf = %d\n",(int)xxstrlen(line_label_buf));
      exit(2);
    }
}

const unsigned char *
label_from_line_id(const unsigned char *line_id)
{
  return hash_find(xid_to_label_table,line_id);
}
