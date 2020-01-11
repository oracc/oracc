#include <stdio.h>
#include "pool.h"
#include "ilem_form.h"
#include "props.h"

#define xstrcmp(a,b) strcmp((const char *)a,(const char *)b)
static int prop_id = 0;
static const char *
next_prop_id(void)
{
  char buf[12];
  sprintf(buf,"p%08x",prop_id++);
  return (const char *)pool_copy((unsigned char *)buf);
}

/* return 0 if the prop is added; 1 if it was skipped because it was duplicate 

   FIXME: props should be added to every form in an ambiguity chain.
 */
int
props_add_prop(struct ilem_form *f, const unsigned char *group,
	       const unsigned char *name, const unsigned char *value, 
	       const char *ref, const char *xml_id, const char *pref,
	       int ngram_id)
{
  struct prop *p;

  if (!f)
    return -1;
  
  /* Should probably revise this to add multiple instances of same
     prop now that they come with an ngram_id */
  if (f->props && (p = props_find_prop(f,name,value)))
    {
      if (!p->ref)
	p->ref = ref;
      else
	; /*FIXME: p->ref should probably be a list*/
      if (!p->group)
	p->group = group;
      if (!p->value)
	p->value = value;
      if (!p->xml_id && xml_id && *xml_id)
	p->xml_id = xml_id;
      if (!p->p && pref && *pref)
	p->p = pref;
      return 1;
    }
  if (!(p = calloc(1,sizeof(struct prop))))
    {
      fputs("out of core\n",stderr);
      exit(2);
    }
  p->group = group;
  p->name = name;
  p->value = value;
  p->ngram_id = ngram_id;
  if (ref && *ref)
    p->ref = ref;
  if (xml_id && *xml_id)
    p->xml_id = xml_id;
  else
    p->xml_id = next_prop_id();
  if (pref && *pref)
    p->p = pref;
  p->next = f->props;
  f->props = p;
  return 0;
}

void
props_dump_props(struct ilem_form *f, FILE *fp)
{
  if (f->props)
    {
      struct prop *p;
      fputs("<props>",fp);
      for (p = f->props; p; p = p->next)
	{
	  fprintf(fp,"<prop name=\"%s\"\n",p->name);
	  if (p->group)
	    fprintf(fp," group=\"%s\"",p->group);
	  if (p->value)
	    fprintf(fp," value=\"%s\"",p->value);
	  fprintf(fp," ngram=\"%d\"",p->ngram_id);
	  if (p->ref)
	    fprintf(fp," ref=\"%s\"",p->ref);
	  if (p->p)
	    fprintf(fp," p=\"%s\"",p->p);
	  if (p->xml_id)
	    fprintf(fp," xml:id=\"%s\"",p->xml_id);
	  fputs("/>",fp);
	}
      fputs("</props>",fp);
    }
}

struct prop*
props_find_prop(struct ilem_form *f, const unsigned char *name, const unsigned char *value)
{
  struct prop *p = NULL;
  for (p = f->props; p; p = p->next)
    if (!xstrcmp(p->name,name) 
	&& (!value || !p->value || (!value || !xstrcmp(p->value,value))))
      return p;
  return NULL;
}

struct prop*
props_find_prop_group(struct ilem_form *f, const unsigned char *group)
{
  struct prop *p = NULL;
  for (p = f->props; p; p = p->next)
    if (p->group && !xstrcmp(p->group,group))
      return p;
  return NULL;
}

/* Add the prop at the front of the list */
struct prop*
props_add_prop_sub(struct prop *props, const unsigned char *group,
		   const unsigned char *name, const unsigned char *value, 
		   const char *ref, const char *xml_id, const char *pref,
		   int ngram_id)
{
  struct prop *p;

  /* Should probably revise this to add multiple instances of same
     prop now that they come with an ngram_id */
  if (props && (p = props_find_prop_sub(props,name,value)))
    {
      if (!p->ref)
	p->ref = ref;
      else
	; /*FIXME: p->ref should probably be a list*/
      if (!p->group)
	p->group = group;
      if (!p->value)
	p->value = value;
      if (!p->xml_id && xml_id && *xml_id)
	p->xml_id = xml_id;
      if (!p->p && pref && *pref)
	p->p = pref;
      return props;
    }
  if (!(p = calloc(1,sizeof(struct prop))))
    {
      fputs("out of core\n",stderr);
      exit(2);
    }
  p->group = group;
  p->name = name;
  p->value = value;
  p->ngram_id = ngram_id;
  if (ref && *ref)
    p->ref = ref;
  if (xml_id && *xml_id)
    p->xml_id = xml_id;
  else
    p->xml_id = next_prop_id();
  if (pref && *pref)
    p->p = pref;
  p->next = props;
  return p;
}

void
props_dump_props_sub(struct prop *props, FILE *fp)
{
  if (props)
    {
      struct prop *p;
      fputs("<props>",fp);
      for (p = props; p; p = p->next)
	{
	  fprintf(fp,"<prop name=\"%s\"\n",p->name);
	  if (p->group)
	    fprintf(fp," group=\"%s\"",p->group);
	  if (p->value)
	    fprintf(fp," value=\"%s\"",p->value);
	  fprintf(fp," ngram=\"%d\"",p->ngram_id);
	  if (p->ref)
	    fprintf(fp," ref=\"%s\"",p->ref);
	  if (p->p)
	    fprintf(fp," p=\"%s\"",p->p);
#if 0
	  if (p->xml_id)
	    fprintf(fp," xml:id=\"%s\"",p->xml_id);
#endif
	  fputs("/>",fp);
	}
      fputs("</props>",fp);
    }
}

struct prop*
props_find_prop_sub(struct prop * p, const unsigned char *name, const unsigned char *value)
{
  for (; p; p = p->next)
    if (!xstrcmp(p->name,name) 
	&& (!value || !p->value || (!value || !xstrcmp(p->value,value))))
      return p;
  return NULL;
}

struct prop*
props_find_prop_group_sub(struct prop *p, const unsigned char *group)
{
  for (; p; p = p->next)
    if (p->group && !xstrcmp(p->group,group))
      return p;
  return NULL;
}
