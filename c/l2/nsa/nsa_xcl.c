#include <stdarg.h>
#include <string.h>
#include <ctype128.h>
#include "ilem_para.h"
#include "nsa.h"

static int stop_token_pending = 0;

static void
ofunc(struct xcl_context *xc, struct xcl_c *cnode)
{
}
static void
cfunc(struct xcl_context *xc, struct xcl_c *cnode)
{
}
static void
dfunc(struct xcl_context *xc, struct xcl_d *dnode)
{
  if (dnode->type == xcl_d_field_end)
    stop_token_pending = 1;
  else if (dnode->type == xcl_d_line_start)
    stop_token_pending = 1;
}
static void
lfunc(struct xcl_context *xc, struct xcl_l *lnode)
{
  struct ilem_para *p = ilem_para_find(lnode, LPC_nsa, LPT_nsa_stop);
  if (stop_token_pending || p != NULL)
    {
      stop_token_pending = 0;
      nsa_token(xc->user, NSA_P_STOP, NULL, NULL);
    }

  p = ilem_para_find(lnode, LPC_nsa, LPT_nsa_system);
  if (p)
    nsa_system_override_set(p->text);

  /* if the lemma is an 'n', return the written form; otherwise return
     the citation form */
  if ((lnode->f->f2.pos && 'n' == *lnode->f->f2.pos)
      || (!lnode->f->f2.cf || !*lnode->f->f2.cf))
    nsa_token(xc->user, NSA_P_LEMM, lnode, (char*)lnode->f->f2.form);
  else
    nsa_token(xc->user, NSA_P_LEMM, lnode, (char*)lnode->f->f2.cf);
}

void
nsa_xml_input(struct nsa_context *cp, const char *infile)
{
  struct xcl_context *xcp;
  struct nsa_result *res;
  extern const char *textid;
  FILE *xfp;
  xcp = xcl_load(infile, 0);
  textid = xcp->textid;
  if (!xcp->linkbase)
    xcp->linkbase = new_linkbase();
  nsa_xcl_input(xcp, cp);
  res = (struct nsa_result *)xcp->user;
  if (res->success)
    {
      res->label = xcp->textid;
#if 0
      if (printres)
	nsa_print(res, stdout);
#endif
      xfp = fopen("xcl.out","w");
      x2_serialize(xcp,xfp,0);
      fclose(xfp);
    }
  else
    fprintf(stderr,"nsa: parse failed in %s\n",infile);
  nsa_del_result(res);
  return;
}

void
nsa_xcl_input(struct xcl_context *xcp, struct nsa_context *cp)
{
  struct nsa_parser *parser;
  if (!xcp->linkbase)
    xcp->linkbase = new_linkbase();
  xcp->user = parser = nsa_parse_init(cp);
  /* XCL map needs to check to see if any of its lemmata are in a PSU,
     and if so, should pass the PSU to the NSA tokenizer.  The NSA
     tokenizer can then take the initial lemma and install it as a
     grapheme, with text_ref to the PSU.  In this way, NSA can view
     commodities only as the head, simplifying its job; but the actual
     grouping will reflect the complete PSU idiom.
     
     We should also consider doing something other than passing PNs
     in as graphemes, i.e., passing them as NSA_T_POS, say, so that
     disambiguation of metrology by proper noun could be feasible.
   */
  xcl_map(xcp, ofunc, cfunc, dfunc, lfunc);
  nsa_parse(parser);
  if (verbose)
    nsa_show_tokens(parser, stdout);
  parser->context->user = xcp;
  xcp->user = nsa_create_result(parser);
}

void
nsa_xcl_warning(struct xcl_context *xcp, struct xcl_l*head, const char *fmt,...)
{
  extern FILE *f_log;
  extern int with_textid;
  extern const char *textid;
  extern const char *phase;
  va_list args;
  phase = "nsa";
  va_start(args,fmt);
  if (with_textid)
    fprintf(f_log,"%s:%d:%s: ",head->f->file,(int)head->f->lnum,xcp->textid);
  else
    fprintf(f_log,"%s:%d: ",head->f->file,(int)head->f->lnum);
  if (phase)
    fprintf(f_log,"(%s) ", phase);
  (void)vfprintf(f_log,fmt,args);
  va_end(args);
  (void)fputc('\n',f_log);
}

static struct nsa_count *
clone_count(struct nsa_count*c)
{
  struct nsa_count *clone = NULL;
  if (c)
    {
      clone = new_count();
      *clone = *c;
    }
  return clone;
}

const char *
nsa_xcl_render_count(struct nsa_count *c)
{
  static char buf[128];
  if (c->den == 1)
    sprintf(buf,"%lld",c->num);
  else
    {
      long long intval = (c->num/c->den);
      int remval = (c->num%c->den);
      if (intval)
	sprintf(buf,"%lld",intval);
      else
	*buf = '\0';
      if (remval)
	sprintf(buf+strlen(buf)," %d/%d",remval,c->den);	
    }
  return buf;
}

struct nsa_xcl_data *
nsa_xcl_info(struct nsa_amount *a)
{
  if (a->measure->type == NSA_T_MEASURE && a->measure->d.m->system)
    {
      struct nsa_xcl_data *d = calloc(1, sizeof(struct nsa_xcl_data));
      if (!d)
	nsa_mem_die();
      d->sysname = strdup(a->measure->d.m->system->n);
      d->aev = clone_count(a->measure->d.m->aev);
      d->aeu = "";
      d->mev = clone_count(a->measure->d.m->mev);
      if (a->measure->d.m->system->meu)
	d->meu = strdup(a->measure->d.m->system->meu);
      else
	d->meu = NULL;
      d->rendered_mev = strdup(a->measure->d.m->rendered_mev);
      if (a->commodity)
	d->comh = nsa_grapheme_text(a->commodity->children[0]);
      if (measure_sysdet(a->measure))
	d->sysdet = nsa_grapheme_text(measure_sysdet(a->measure));
      return d;
    }
  else if (a->measure->type == NSA_T_COUNT)
    {
      struct nsa_xcl_data *d = calloc(1, sizeof(struct nsa_xcl_data));
      if (!d)
	nsa_mem_die();
      d->sysname = malloc(9); /* base- + \0 + 3 (base could be 120) */
      sprintf((char*)d->sysname,"base-%d",count_base(a->measure));
      d->aev = NULL;
      d->aeu = NULL;
      d->mev = clone_count(a->measure->d.c);
      d->meu = "";
      d->rendered_mev = strdup(nsa_xcl_render_count(d->mev));
      return d;
    }
  else
    return NULL;
}

void
nsa_xcl_dump(FILE *fp,struct nsa_xcl_data *d)
{
  if (d)
    {
      fputs("<nsa:data xmlns:nsa=\"http://oracc.org/ns/nsa/1.0\">",fp);
      fprintf(fp,"<nsa:sysname>%s</nsa:sysname>",d->sysname);
      if (d->comh)
	fprintf(fp,"<nsa:comh>%s</nsa:comh>",d->comh);
      if (d->sysdet)
	fprintf(fp,"<nsa:sysdet>%s</nsa:sysdet>",d->sysdet);
		
      if (!strncmp("base-",d->sysname,5))
	{
	  fprintf(fp,"<nsa:num-axis>%s</nsa:num-axis>",sex_axis_str[d->mev->axis]);
	  fprintf(fp,"<nsa:num-form>%s</nsa:num-form>",sex_form_str[d->mev->form]);
	}
      if (d->aev)
	{
	  fprintf(fp,"<nsa:aev>%lld/%d</nsa:aev>", d->aev->num, d->aev->den);
	  fprintf(fp,"<nsa:aeu>%s</nsa:aeu>", d->aeu);
	}
      fprintf(fp,"<nsa:mev>%lld/%d</nsa:mev>", d->mev->num, d->mev->den);
      fprintf(fp,"<nsa:meu>%s</nsa:meu>", d->meu);
      fprintf(fp,"<nsa:mev-disp>%s</nsa:mev-disp>",d->rendered_mev);
      fputs("</nsa:data>",fp);
    }
}
