#include <hash.h>
#include <pool.h>
#include <gdl.h>

static Hash *bridge_h = NULL;
static Pool *bridge_p = NULL;
extern int asl_flex_debug , gdl_flex_debug, nwarning;

static const char *gb_lang = "sux";

void
gvl_bridge_lang(const char *l)
{
  gb_lang = l;
}

void
gvl_bridge_init(void)
{
  bridge_h = hash_create(1024);
  bridge_p = pool_init();
  if (!strcmp(gb_lang, "qpc"))
    (void)gvl_setup("pcsl", "pcsl");
  else
    (void)gvl_setup("ogsl", "ogsl");
  gdlparse_init();
  mesg_init();
  gdl_flex_debug = 0;
  gvl_no_mesg_add = 1;
}

const char *
gvl_bridge(const char *f,size_t l,const unsigned char *g)
{
  Tree *tp = NULL;

  const char *sentinel = "tumₓ(|TUM×GAN₂@t|)";
  if (!strcmp((ccp)g, sentinel))
    fprintf(stderr, "gvl_bridge: found %s\n", sentinel);
  
  tp = gdlparse_string(mesg_mloc(f,l), (char*)pool_copy(g,bridge_p));

  if (tp && tp->root && tp->root->kids)
    {
      gvl_g *gp = tp->root->kids->user;
      if (gp)
	{
	  hash_add(bridge_h, pool_copy(g,bridge_p), gp);
	  return (ccp)gp->mess;
	}
      else if (tp->root->kids->kids)
	{
	  /* sexified numbers like '1' have a user == NULL at
	     root->kids but the sexified version at
	     root->kids->kids */
	  if (!strcmp(tp->root->kids->kids->name, "g:n"))
	    {
	      gp = tp->root->kids->kids->user;
	      if (gp)
		return (ccp)gp->mess;
	    }
	}
    }
  
  mesg_verr(mesg_mloc(f,l), "(gvl_bridge) unknown error trying to parse %s", g);
  ++nwarning;
  return NULL;
}
