#include <ctype128.h>
#include "cdt.h"
#include "loadfile.h"

struct props
{
  const char *name;
  const char *value;
};
struct props pm1_props[] = 
  {
    { "fo:page-width", "8.5in" },
    { "fo:page-height", "11in" },
    { "style:num-format", "1" },
    { "style:print-orientation", "portrait" },
    { "fo:margin-top", "0.7864in" },
    { "fo:margin-bottom", "0.7864in" },
    { "fo:margin-left", "0.7864in" },
    { "fo:margin-right", "0.7864in" },
    { "style:writing-mode", "lr-tb" },
    { "style:footnote-max-height", "1in" },
    { NULL, NULL },
  };
struct props fnsep_props[] =
  {
    { "style:width" , "0.0071in" },
    { "style:distance-before-sep" , "0.0398in" },
    { "style:distance-after-sep" , "0.0398in" },
    { "style:adjustment" , "left" },
    { "style:rel-width" , "25%" },
    { "style:color" , "#000000" },
    { NULL , NULL },
  };

#if 0
static const char *akk_style =
  "    <style:style style:name=\"akk\" style:family=\"text\">\n"
  "      <style:text-properties fo:font-style=\"italic\"/>\n"
  "    </style:style>\n";
static const char *elx_style =
  "    <style:style style:name=\"elx\" style:family=\"text\">\n"
  "      <style:text-properties fo:font-style=\"italic\"/>\n"
  "    </style:style>\n";
static const char *sub_style =
  "    <style:style style:name=\"sub\" style:family=\"text\">\n"
  "      <style:text-properties style:text-position=\"sub 70%\"/>\n"
  "    </style:style>\n";
static const char *sup_style =
  "    <style:style style:name=\"sup\" style:family=\"text\">\n"
  "      <style:text-properties style:text-position=\"super 70%\"/>\n"
  "    </style:style>\n";
#endif

static FILE *wfile;
struct styles common_styles;
static void dump_props(struct props*pp);
static unsigned char *scan_property(unsigned char *s, unsigned char **endp, struct cdt_node *np);

void
cdt_styles_init(void)
{
  common_styles.faces  = list_create(LIST_SINGLE);
  common_styles.styles = list_create(LIST_SINGLE);
  common_styles.defaults = list_create(LIST_SINGLE);
  common_styles.faces_hash  = hash_create(100);
  common_styles.styles_hash = hash_create(100);
  common_styles.defaults_hash = hash_create(100);
}

void
cdt_styles_term(void)
{
  list_free(common_styles.faces,NULL);
  list_free(common_styles.styles,NULL);
  list_free(common_styles.defaults,NULL);
  hash_free(common_styles.faces_hash,NULL);
  hash_free(common_styles.styles_hash,NULL);
  hash_free(common_styles.defaults_hash,NULL);
}

void
cdt_style_handler(struct cdt_node *np)
{
  struct style_node *sp = calloc(1, sizeof(struct style_node));
  unsigned char *s = np->text;
  List *curr_props;
  curr_props = sp->props = list_create(LIST_SINGLE);
  sp->family = s;
  while (*s && !isspace(*s))
    ++s;
  if (*s)
    *s++ = '\0';
  while (isspace(*s))
    ++s;
  sp->name = scan_property(s,&s,np);
  if (!strcmp((const char *)sp->family,"font"))
    {
      sp->type = style_face;
      if (!hash_find(common_styles.faces_hash,sp->name))
	list_add(common_styles.faces,sp);
      hash_add(common_styles.faces_hash,sp->name,sp);
    }
  else if (!strncmp((const char *)sp->name,"#default",8))
    {
      sp->type = style_default;
      ++sp->name;
      if (!hash_find(common_styles.defaults_hash,sp->name))
	list_add(common_styles.defaults,sp);
      hash_add(common_styles.defaults_hash,sp->name,sp);
    }
  else
    {
      sp->type = style_style;
      if (!hash_find(common_styles.styles_hash,sp->name))
	list_add(common_styles.styles,sp);
      hash_add(common_styles.styles_hash,sp->name,sp);
    }
  while (*s)
    {
      if ('[' == *s)
	{
	  struct style_xfam *xp = calloc(1, sizeof(struct style_xfam));
	  if (!sp->xfams)
	    sp->xfams = list_create(LIST_SINGLE);
	  list_add(sp->xfams, xp);
	  xp->name = ++s;
	  while (*s && ']' != *s)
	    ++s;
	  if (*s)
	    {
	      *s++ = '\0';
	      while (isspace(*s))
		++s;
	      curr_props = xp->props = list_create(LIST_SINGLE);
	    }
	  else
	    cdt_warning(np->file,np->lnum,"missing ] to end family switch in style");    
	}
      else
	{
	  struct style_prop *p = malloc(sizeof(struct style_prop));
	  p->name = s;
	  while (*s && '=' != *s)
	    ++s;
	  if (*s)
	    *s++ = '\0';
	  p->value = scan_property(s,&s,np);
	  list_add(curr_props,p);
	}
    }
}

static void
write_prop(struct style_prop *pp)
{
  fprintf(wfile, " %s=\"%s\"", pp->name,pp->value);
}

static void
write_props(const unsigned char *fam, List *props)
{
  fprintf(wfile,"<style:%s-properties",fam);
  list_exec(props,(list_exec_func*)write_prop);
  fputs("/>",wfile);
}

static enum style_props curr_sp_prop = 0;
static struct style_prop_fam * (*curr_sp_func)(const char *prop, unsigned int len);

static struct style_prop_fam*
section_props(const char *name, unsigned int len)
{
  static struct style_prop_fam sectprops[] =
    {
      { "fo:column-count" , sp_column },
      { "fo:column-gap" , sp_column },
      { NULL , sp_section },
    };
  struct style_prop_fam *sp;
  for (sp = sectprops; sp->name; ++sp)
    if (!strcmp(sp->name, name))
      return sp;
  return sp;
}

static void
write_tab_stops(const char *tabs, FILE *w)
{
  int tab_stop_is_open = 0;
  fputs("<style:tab-stops>",w);
  while (*tabs)
    {
      if (isdigit(*tabs))
	{
	  if (tab_stop_is_open)
	    fputs("/>",w);
	  else
	    tab_stop_is_open = 1;
	  fputs("<style:tab-stop",w);
	  fputs(" style:position=\"",w);
	  while (*tabs && !isspace(*tabs))
	    fputc(*tabs++,w);
	  fputc('"',w);	  
	}
      else if (isalpha(*tabs))
	{
	  if (tab_stop_is_open)
	    {
	      fputs(" style:type=\"",w);
	      while (*tabs && !isspace(*tabs))
		fputc(*tabs++,w);
	      fputc('"',w);	  
	    }
	  else
	    ; /* FIXME: error reporting */
	}
      else if (isspace(*tabs))
	while (isspace(*tabs))
	  ++tabs;
      else
	; /* FIXME: error reporting */
    }
  if (tab_stop_is_open)
    fputs("/>",w);
  fputs("</style:tab-stops>",w);
}

static void
write_prop_selected(struct style_prop *pp)
{
  struct style_prop_fam *sp = curr_sp_func((char*)pp->name,strlen((char*)pp->name));
  if (sp && sp->type == curr_sp_prop)
    fprintf(wfile, " %s=\"%s\"", pp->name,pp->value);
}

static void
write_props4(const unsigned char *fam, List *props, 
	     struct style_prop_fam * (*sp_func)(const char *prop, unsigned int len),
	     enum style_props type)
{
  curr_sp_prop = type;
  curr_sp_func = sp_func;
  if ('!' == *fam)
    fprintf(wfile,"<%s",fam+1);
  else
    fprintf(wfile,"<style:%s-properties",fam);
  list_exec(props,(list_exec_func*)write_prop_selected);
  if (!strcmp((char*)fam,"section"))
    {
      fputc('>',wfile);
      write_props4((unsigned char *)"!style:columns",props, 
		   section_props, sp_column);
      fputs("</style:section-properties>",wfile);
    }
  else if (!strcmp((char *)fam,"paragraph"))
    {
      struct style_prop *sp = NULL;
      fputc('>',wfile);
      for (sp = list_first(props); sp; sp = list_next(props))
	{
	  if (!strcmp((char*)sp->name, "cdl:tab-stops"))
	    {
	      write_tab_stops((char*)sp->value,wfile);
	      break;
	    }
	}
      fputs("</style:paragraph-properties>",wfile);
    }
  else
    fputs("/>",wfile);
}

static void
write_xfam(struct style_xfam *xfam)
{
  write_props(xfam->name,xfam->props);
}

static void
write_style_props(List *props, FILE *wfile)
{
  struct style_prop *sp = NULL;
  for (sp = list_first(props); sp; sp = list_next(props))
    {
      if (!strcmp((char*)sp->name, "style:parent-name")
	  || !strcmp((char*)sp->name, "style:master-page-name"))
	{
	  fprintf(wfile, " %s=\"%s\"",sp->name,sp->value);
	}
    }  
}

void
write_style(struct style_node *sp)
{
  switch (sp->type)
    {
    case style_default:
      sp = hash_find(common_styles.defaults_hash,sp->name);
      if (strcmp((const char *)sp->family,"page-layout"))
	fprintf(wfile,"<style:%s style:family=\"%s\">",sp->name,sp->family);
      else
	fprintf(wfile,"<style:%s>",sp->name);
      write_props(sp->family, sp->props);
      if (sp->xfams)
	list_exec(sp->xfams,(list_exec_func*)write_xfam);
      fprintf(wfile,"</style:%s>",sp->name);
      break;
    case style_face:
      sp = hash_find(common_styles.faces_hash,sp->name);
      fprintf(wfile,"<style:font-face style:name=\"%s\"",sp->name);
      list_exec(sp->props,(list_exec_func*)write_prop);
      fputs("/>",wfile);
      break;
    case style_style:
      sp = hash_find(common_styles.styles_hash,sp->name);
      fprintf(wfile,"<style:style style:name=\"%s\" style:family=\"%s\"",sp->name,sp->family);
      write_style_props(sp->props,wfile);
      fputc('>',wfile);
      if (!strcmp((char*)sp->family, "section"))
	{
	  write_props4((unsigned char*)"section",sp->props, 
		       section_props, sp_section);
	}
      else if (!strcmp((char*)sp->family, "paragraph"))
	{
	  write_props4((unsigned char*)"paragraph",sp->props,
		       style_props,sp_paragraph);
	  write_props4((unsigned char *)"text",sp->props,
		       style_props,sp_text);
	}
      else
	write_props(sp->family, sp->props);
      if (sp->xfams)
	list_exec(sp->xfams,(list_exec_func*)write_xfam);
      fputs("</style:style>",wfile);
      break;
    }
}

void
xml_echo(const char *file, FILE *fp)
{
  FILE *cfg = NULL;
  int ch;
  cfg = xfopen(file,"r");
  while (EOF != (ch = fgetc(cfg)))
    fputc(ch,fp);
  xfclose(file,cfg);
}

void
xml_subset(const char *file, FILE *fp)
{
  const char *f = (const char *)loadfile((unsigned char*)file,NULL);
  if (!strncmp(f,"<wrapper",8))
    {
      const char *start = NULL, *end = NULL;
      start = strchr(f,'>');
      if (start)
	{
	  ++start;
	  while (isspace(*start))
	    ++start;
	}
      end = strrchr(f,'<');
      if (end && !strncmp(end,"</wrapper>",10))
	{
	  fwrite(start,1,end-start,fp);
	  return;
	}
    }
  fprintf(stderr,"cdt2cdx: badly formed automatic-styles.xml\n");
}

void
cdt_style_writer(FILE*fp)
{
  wfile = fp;
  fputs("<office:document-styles>",wfile);
  fputs("<office:font-face-decls>",wfile);
  list_exec(common_styles.faces, (list_exec_func*)write_style);
  fputs("</office:font-face-decls>",wfile);
  fputs("<office:styles>",wfile);
  list_exec(common_styles.defaults, (list_exec_func*)write_style);
  if (styles_styles)
    xml_subset(styles_styles,wfile);
  fputs("</office:styles>",wfile);
  fputs("<office:automatic-styles>",wfile);
  if (automatic_styles)
    xml_subset(automatic_styles,wfile);
  else
    {
      fputs("<style:page-layout style:name=\"pm1\"",wfile);
      dump_props(pm1_props);
      fputs("><style:footnote-sep",wfile);
      dump_props(fnsep_props);
      fputs("/></style:page-layout>",wfile);
      fputs("<style:header-style/>",wfile);
      fputs("<style:footer-style/>",wfile);
    }
  fputs("</office:automatic-styles>",wfile);
  if (master_styles)
    xml_echo(master_styles,wfile);
  else
    fputs("<office:master-styles><style:master-page style:name=\"Standard\" style:page-layout-name=\"pm1\"/></office:master-styles>",wfile);
  fputs("</office:document-styles>",wfile);
}

static void
dump_props(struct props*pp)
{
  while (pp->name)
    {
      fprintf(wfile, " %s=\"%s\"",pp->name,pp->value);
      ++pp;
    }
}

static unsigned char *
scan_property(unsigned char *s, unsigned char **endp, struct cdt_node *np)
{
  unsigned char *ret;
  if ('"' == *s)
    {
      ret = ++s;
      while (*s && '"' != *s && '\\' != s[-1])
	++s;
      if (*s)
	*s++ = '\0';
      else
	cdt_warning(np->file,np->lnum,"missing \" to end string in style property");
      while (isspace(*s))
	++s;
    }
  else
    {
      ret = s;
      while (*s && !isspace(*s))
	++s;
      if (*s)
	*s++ = '\0';
      while (isspace(*s))
	++s;
    }
  while (isspace(*s))
    ++s;
  if (endp)
    *endp = s;
  return ret;
}
