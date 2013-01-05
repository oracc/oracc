
typedef void (*keygenp_t)(struct ilem_form *fp);
static void keygen_cf_gw(struct ilem_form *fp);
static int uniq_test(struct ilem_form *fp, void *user, void *setup);
static void uniq_with_keygenp(struct ilem_form *fp, keygenp_t kp);

/*Save some stack by having only one keybuf*/
static unsigned char keybuf[128];
static void (*keygenp)(struct ilem_form *fp);
static void
keygen_cf_gw(struct ilem_form *fp)
{
  sprintf((char*)keybuf,"%s[%s]%s",fp->f2.cf,fp->f2.gw,fp->f2.epos);
}
static void
keygen_cf_sense(struct ilem_form *fp)
{
  sprintf((char*)keybuf,"%s[%s]%s",fp->f2.cf,fp->f2.sense,fp->f2.epos);
}
static void
keygen_norm_gw(struct ilem_form *fp)
{
  sprintf((char*)keybuf,"%s[%s]%s",fp->f2.norm?fp->f2.norm:fp->f2.cf,fp->f2.gw,fp->f2.epos);
}
static void
keygen_norm_sense(struct ilem_form *fp)
{
  sprintf((char*)keybuf,"%s[%s]%s",fp->f2.norm?fp->f2.norm:fp->f2.cf,fp->f2.sense,fp->f2.epos);
}

struct uniq_setup
{
  Hash_table *hash;
  struct npool *pool;
};

static void *
uniq_init(void*user)
{
  static struct uniq_setup u;
  u.hash = hash_create(1);
  u.pool = npool_init();
  return &u;
}

static void
uniq_term(void *user, void *setup)
{
  hash_free(((struct uniq_setup*)setup)->hash,NULL);
  npool_term(((struct uniq_setup*)setup)->pool);
}

static void
uniq_with_keygenp(struct ilem_form *fp, keygenp_t kp)
{
  struct ilem_form **fpp;
  keygenp = kp;

  /* AMBIG LOOPING HERE */

  fpp = ilem_select(fp->finds, fp->fcount, NULL, uniq_init, uniq_test, uniq_term, &fp->fcount);

  memcpy(fp->finds,fpp,(1+fp->fcount)*sizeof(struct ilem_form *));
}

static int
uniq_test(struct ilem_form *fp, void *user, void *setup)
{
  int ret = 0;
  struct uniq_setup *u = setup;

  keygenp(fp);

  if (fp->f2.pos && fp->f2.epos && strcmp((char*)fp->f2.pos,(char*)fp->f2.epos))
    sprintf((char*)keybuf+strlen((char*)keybuf),"'%s",fp->f2.epos);

  if (!(ret = (uintptr_t)hash_find(u->hash,keybuf)))
    hash_add(u->hash,npool_copy(keybuf,u->pool),(void*)1);

  return ret;
}
