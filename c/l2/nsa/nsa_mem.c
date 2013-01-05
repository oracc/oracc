#include "nsa.h"

static void nsa_del_text_ref(struct nsa_text_ref *);
static void nsa_del_grapheme(struct nsa_grapheme *);
static void nsa_del_unit(struct nsa_unit *);
static void nsa_del_count(struct nsa_count *);
static void nsa_del_quantity(struct nsa_quantity *);
static void nsa_del_measure(struct nsa_measure *);
static void nsa_del_commodity(struct nsa_commodity *);
static void nsa_del_amount(struct nsa_amount *);
static void nsa_del_hash_data(struct nsa_hash_data *);
#if 0
static void nsa_del_step(struct nsa_step *);
#endif
static void nsa_del_result_chunk(struct nsa_result_chunk *);

void
nsa_mem_die(void)
{
  fprintf(stderr,"nsa: ran out of memory\n");
  exit(1);
}
struct nsa_token **
new_children(int i)
{
  void *tmp = calloc(i+1,sizeof(struct nsa_token*));
  if (!tmp)
    nsa_mem_die();
  return tmp;
}
struct nsa_text_ref *
new_text_ref(void)
{
  void *tmp = calloc(1,sizeof(struct nsa_text_ref));
  if (!tmp)
    nsa_mem_die();
  return tmp;
}
static void
nsa_del_text_ref(struct nsa_text_ref *p)
{
  free(p);
}
struct nsa_token *
new_token(void)
{
  struct nsa_token *tmp = calloc(1,sizeof(struct nsa_token));
  if (!tmp)
    nsa_mem_die();
  tmp->type = NSA_T_NONE;
  return tmp;
}
void
nsa_del_token(struct nsa_token *p)
{
  switch (p->type)
    {
    case NSA_T_GRAPHEME:
      nsa_del_grapheme(p->d.g);
      break;
    case NSA_T_COUNT:
      nsa_del_count(p->d.c);
      break;
    case NSA_T_UNIT:
      nsa_del_unit(p->d.u);
      break;
    case NSA_T_QUANTITY:
      nsa_del_quantity(p->d.q);
      break;
    case NSA_T_MEASURE:
      nsa_del_measure(p->d.m);
      break;
    case NSA_T_COMMODITY:
      nsa_del_commodity(p->d.o);
      break;
    case NSA_T_AMOUNT:
      nsa_del_amount(p->d.a);
      break;
    case NSA_T_UNKNOWN:
      break;
    case NSA_T_NONE:
    case NSA_T_STOP:
      break;
    }
  if (p->children)
    {
      int i;
      for (i = 0; p->children[i]; ++i)
	nsa_del_token(p->children[i]);
      free(p->children);
    }
  free(p);
}
struct nsa_grapheme *
new_grapheme(void)
{
  void *tmp = calloc(1,sizeof(struct nsa_grapheme));
  if (!tmp)
    nsa_mem_die();
  return tmp;
}
static void
nsa_del_grapheme(struct nsa_grapheme *p)
{
  nsa_del_text_ref(p->text);
  free(p);
}
struct nsa_unit *
new_unit(void)
{
  void *tmp = calloc(1,sizeof(struct nsa_unit));
  if (!tmp)
    nsa_mem_die();
  return tmp;
}
static void
nsa_del_unit(struct nsa_unit *p)
{
#if 0
  if (p->cands)
    list_free(p->cands,NULL);
#endif
  free(p);
}
struct nsa_count *
new_count(void)
{
  struct nsa_count *tmp = calloc(1,sizeof(struct nsa_count));
  if (!tmp)
    nsa_mem_die();
  tmp->den = 1;
  return tmp;
}
static void
nsa_del_count(struct nsa_count *p)
{
  free(p);
}
struct nsa_quantity *
new_quantity(void)
{
  void *tmp = calloc(1,sizeof(struct nsa_quantity));
  if (!tmp)
    nsa_mem_die();
  return tmp;
}
static void
nsa_del_quantity(struct nsa_quantity *p)
{
  nsa_del_token(p->count);
  nsa_del_token(p->unit);
  nsa_del_count(p->aev);
  free(p);
}
struct nsa_measure *
new_measure(void)
{
  void *tmp = calloc(1,sizeof(struct nsa_measure));
  if (!tmp)
    nsa_mem_die();
  return tmp;
}
static void
nsa_del_measure(struct nsa_measure *p)
{
#if 0
  if (p->cands)
    list_free(p->cands,NULL);
#endif
  nsa_del_count(p->aev);
  nsa_del_count(p->mev);
  /* OK to free this, because it gets strdup'ed before being returned to
     the outside world */
  free(p->rendered_mev);
  /* don't free measure_sysdet because that points to a grapheme token
     which gets freed elsewhere */
  free(p);
}
struct nsa_xcl_data *
new_xcl_data(void)
{
  void *tmp = calloc(1,sizeof(struct nsa_xcl_data));
  if (!tmp)
    nsa_mem_die();
  return tmp;
}
void
nsa_del_xcl_data(struct nsa_xcl_data *p)
{
  free(p);
}
struct nsa_commodity *
new_commodity(void)
{
  void *tmp = calloc(1,sizeof(struct nsa_amount));
  if (!tmp)
    nsa_mem_die();
  return tmp;
}
static void
nsa_del_commodity(struct nsa_commodity *p)
{
  free(p);
}
struct nsa_amount *
new_amount(void)
{
  void *tmp = calloc(1,sizeof(struct nsa_amount));
  if (!tmp)
    nsa_mem_die();
  return tmp;
}
static void
nsa_del_amount(struct nsa_amount *p)
{
  nsa_del_token(p->measure);
  if (p->commodity)
    nsa_del_token(p->commodity);
  free(p);
}
struct nsa_system *
new_system(void)
{
  struct nsa_system *tmp = calloc(1,sizeof(struct nsa_system));
  if (!tmp)
    nsa_mem_die();
  /*fprintf(stderr,"new_system: %p\n",tmp);*/
  tmp->steps = list_create(LIST_DOUBLE);
  return tmp;
}
static void
l_del_step(void *p)
{
  struct nsa_step *s = p;
  free(s->aev);
  free(s);
}
void
nsa_del_system(struct nsa_system *p)
{
  if (p->steps)
    list_free(p->steps,l_del_step);
  if (p->mev)
    nsa_del_count(p->mev);
  /*fprintf(stderr,"del_system: %p\n",p);*/
  free(p);
}
struct nsa_context *
new_context(void)
{
  struct nsa_context *tmp = calloc(1,sizeof(struct nsa_context));
  if (!tmp)
    nsa_mem_die();
  tmp->systems = hash_create(10);
  tmp->step_index = hash_create(100);
  tmp->sysdets = hash_create(5);
  tmp->syskeys = hash_create(100);
  tmp->comheads = hash_create(1000);
  tmp->gal2_tokens = hash_create(4);
  tmp->igigal_keys = hash_create(10);
  tmp->la2_tokens = hash_create(4);
  tmp->morph_suffixes = hash_create(10);
  tmp->sexfracs = hash_create(10);
  tmp->cpool = npool_init();
  return tmp;
}
static void
nsa_del_comhead_list(void *x)
{
  list_free(x, NULL);
}
static void
h_del_hash_data(void *x)
{
  struct nsa_hash_data *d = x;
  if (d->continuations)
    {
      hash_exec(d->continuations,h_del_hash_data);
      hash_free(d->continuations,NULL);
    }
  if (d->cands)
    {
      list_free(d->cands,NULL);
      d->cands = NULL;
    }
  nsa_del_hash_data(d);
}
void
nsa_del_context(struct nsa_context *p)
{
  hash_free(p->systems,(hash_free_func*)nsa_del_system);
  hash_exec(p->step_index,h_del_hash_data);
  hash_free(p->step_index,NULL);
  hash_free(p->sysdets,NULL);
  hash_free(p->syskeys,NULL);
  hash_exec(p->comheads,nsa_del_comhead_list);
  hash_free(p->comheads,NULL);
  hash_free(p->igigal_keys,NULL);
  hash_free(p->gal2_tokens,NULL);
  hash_free(p->la2_tokens,NULL);
  hash_free(p->morph_suffixes,NULL);
  npool_term(p->cpool);
  free(p);
}
struct nsa_hash_data *
new_hash_data(void)
{
  void *tmp = calloc(1,sizeof(struct nsa_hash_data));
  if (!tmp)
    nsa_mem_die();
  return tmp;
}
static void
nsa_del_hash_data(struct nsa_hash_data *p)
{
  free(p);
}
struct nsa_step *
new_step(void)
{
  void *tmp = calloc(1,sizeof(struct nsa_step));
  if (!tmp)
    nsa_mem_die();
  ((struct nsa_step*)tmp)->sex = nsa_sex_disz_axis;
  return tmp;
}
#if 0
static void
nsa_del_step(struct nsa_step *p)
{
  free(p);
}
#endif
struct nsa_parser *
new_parser(void)
{
  void *tmp = calloc(1,sizeof(struct nsa_parser));
  if (!tmp)
    nsa_mem_die();
  return tmp;
}
void
nsa_del_parser(struct nsa_parser *p)
{
  if (p)
    {
      /* Don't delete the context here; we may create/delete a parser
	 for each text but the context can be reused */
      list_free(p->toks,(list_exec_func*)nsa_del_token);
      npool_term(p->pool);
      free(p);
    }
}
struct nsa_result_chunk *
new_result_chunk(void)
{
  void *tmp = calloc(1,sizeof(struct nsa_result_chunk));
  if (!tmp)
    nsa_mem_die();
  return tmp;
}
static void
nsa_del_result_chunk(struct nsa_result_chunk *p)
{
  free(p->refs);
  /* don't free the linkset because it is designed to be used in the XCL
     tree; caller has to take responsibility for freeing it */
  free(p);
}
struct nsa_result *
new_result(void)
{
  void *tmp = calloc(1,sizeof(struct nsa_result));
  if (!tmp)
    nsa_mem_die();
  return tmp;
}
void
nsa_del_result(struct nsa_result *p)
{
  if (p->results)
    {
      int i;
      for (i = 0; p->results[i]; ++i)
	nsa_del_result_chunk(p->results[i]);
      free(p->results);
    }
  free(p);
}
