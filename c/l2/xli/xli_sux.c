#include <ctype.h>
#include "warning.h"
#include "f2.h"
#include "npool.h"
#include "sigs.h"
#include "xli.h"
#include "xcl.h"

static void sux_norm0(struct f2 *f, struct npool *pool);

const char *xli_sux_argp[] = 
{ 
  "perl","/usr/local/oracc/bin/sma.plx","-slave","-xli",(const char*)0 
};
void
xli_sux_handler(struct xli *xlip, struct xcl_l *lemp, struct ilem_form *f)
{
  const char *ophase = phase;
  phase = "sma";
  if (f && (f->f2.pos || f->fcount >0) && f->f2.cf)
    {
      const char *pos = (char*)(f->f2.pos ? f->f2.pos : f->finds[0]->f2.pos);
      if (pos && isupper(*pos))
	{
	  if (!f->f2.morph)
	    {
	      char *buf = NULL;
	      unsigned char *res;
	      unsigned char *oform = (unsigned char *)f->f2.form, *gw = NULL; 

	      if (f->fcount > 0)
		{
		  pos = (char *)f->finds[0]->f2.pos;
		  gw = (unsigned char *)f->finds[0]->f2.gw;
		}
	      else
		{
		  pos = (char*)f->f2.pos;
		  gw = (unsigned char *)f->f2.gw;
		}
	      /* We do pre-emptive aliasing so that if we get a successful
		 parse we can use the proper form in the sig */
	      f2_alias(lemp->xc->sigs, &f->f2, 
		       (f->finds && f->finds[0]) ? (&f->finds[0]->f2) : NULL);
	      buf = malloc(2*(strlen((char*)f->f2.form) 
			      + strlen((char*)f->f2.cf) 
			      + strlen(pos) + strlen(pos)
			      + ((char*)f->f2.base ? strlen((char*)f->f2.base) : 0)
			      + 7));
	      sprintf(buf,"%s\t%s[%s]%s",f->f2.form,f->f2.cf,gw,pos);
	      if (f->f2.base)
		sprintf(buf+strlen(buf),"\t%s",f->f2.base);
	      strcat(buf,"\n");
	      xli_send(xlip,(unsigned char *)buf);
	      res = xli_recv(xlip);

	      if (*res == '\0')
		{
		  if (strstr((char*)res, "no parses found"))
		    vwarning2((char*)f->file,f->lnum,"no parse for FORM %s>%s=%s[%s]%s", oform,
			      f->f2.form, f->f2.cf, gw, pos);
		  else
		    vwarning2((char*)f->file,f->lnum,"%s",res+1);
		  f->f2.base = f->f2.cont = f->f2.morph = (unsigned char*)"X";
		  BIT_SET(f->f2.flags,F2_FLAGS_INVALID);
		}
	      else
		{
		  unsigned char *base, *cont, *morph, *stem;
		  base = res;
		  cont = (unsigned char *)strchr((char*)res,'\t')+1;
		  cont[-1] = '\0';
		  morph = (unsigned char *)strchr((char*)cont,'\t')+1;
		  morph[-1] = '\0';
		  stem = (unsigned char *)strchr((char*)morph,'\t')+1;
		  stem[-1] = '\0';
		  if (!f->f2.base || !*f->f2.base)
		    f->f2.base = npool_copy(base,lemp->xc->pool);
		  if (!f->f2.cont)
		    f->f2.cont = npool_copy(cont,lemp->xc->pool);
		  if (!f->f2.morph)
		    f->f2.morph = npool_copy(morph,lemp->xc->pool);
		  if (!f->f2.stem)
		    {
		      if (stem && *stem)
			f->f2.stem = npool_copy(stem,lemp->xc->pool);
		      else
			f->f2.stem = f->f2.cf;
		    }
		  f->f2.gw = (unsigned char*)gw;
		  f->f2.pos = (unsigned char*)pos;
		  if (!f->f2.sense)
		    f->f2.sense = f->finds[0]->f2.sense;
		  if (!f->f2.epos)
		    f->f2.epos = f->finds[0]->f2.epos;
		  sux_norm0(&f->f2,lemp->xc->pool);
		  BIT_CLR(f->f2.flags,F2_FLAGS_NO_FORM);
		  BIT_CLR(f->f2.flags,F2_FLAGS_INVALID);
		  BIT_SET(f->f2.flags,F2_FLAGS_EXTENDED_OK);
		  if (f->fcount > 1)
		    {
		      f->finds[1] = NULL;
		      f->fcount = 1;
		    }
		  
		  /* if (xli_debug) */
		    vnotice2((char*)f->file,f->lnum,"%s=%s[%s]%s parsed as ^@form %s /%s +%s #%s", oform,
			      f->f2.cf, f->f2.gw, f->f2.pos, f->f2.form, base, cont, morph);

		  /*f->result |= (NEW_BASE|NEW_CONT|NEW_MORPH);*/
		}
	    }
	}
    }
  phase = ophase;
}

static void
sux_norm0(struct f2 *f, struct npool *pool)
{
  if (!f->norm && f->morph)
    {
      unsigned char buf[1024], *b, *m;
      for (b = buf, m = (unsigned char *)f->morph; *m; ++m)
	{
	  if (*m == '~')
	    {
	      strcpy((char*)b,(char*)(f->stem?f->stem:f->cf));
	      b += strlen((const char*)b);
	    }
	  else
	    {
	      *b++ = *m;
	    }
	}
      *b = '\0';
      f->norm = npool_copy(buf,pool);
    }
}
