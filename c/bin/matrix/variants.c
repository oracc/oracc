#include <psd_base.h>
#include <hash.h>
#include "matrix.h"
#include "variants.h"

List *variants_list = NULL;
Hash_table *variants_hash;
static Uchar vars_key_buf[1024];

#define lookup(s)   	hash_find(variants_hash,(s))
#define vars_add(s,d)   hash_add(variants_hash,xstrdup(s),(d))

void
vars_init ()
{
  variants_hash = hash_create (1024);
  variants_list = list_create (LIST_DOUBLE);
}

Uchar *
vars_make_key1 (Variant *vp)
{
  sprintf (vars_key_buf, "%s", vp->block_name);
  return vars_key_buf;
}

Uchar *
vars_make_key2 (Variant *vp)
{
  sprintf (vars_key_buf, "%s:%d", vp->block_name, vp->begin);
  return vars_key_buf;
}

Uchar *
vars_make_key3 (Variant *vp)
{
  sprintf (vars_key_buf, "%s:%d:%s", vp->block_name, vp->begin, vp->siglum_name);
  return vars_key_buf;
}

List *
vars_assq1 (Variant *vp)
{
  return lookup(vars_make_key1(vp));
}

List *
vars_assq2 (Variant *vp)
{
  return lookup(vars_make_key2(vp));
}

Variant *
vars_assq3 (Variant *vp)
{
  return lookup(vars_make_key3(vp));
}

void
vars_register (Variant *vp)
{
  List *lp;
  if (NULL == (lp = lookup(vars_make_key1(vp))))
    {
      lp = list_create(LIST_DOUBLE);
      list_add(lp,vp);
      vars_add(vars_key_buf,lp);
    }
  else
    list_add (lp, vp);

  if (NULL == (lp = lookup(vars_make_key2(vp))))
    {
      lp = list_create(LIST_DOUBLE);
      list_add(lp,vp);
      vars_add(vars_key_buf,lp);
    }
  else
    list_add (lp, vp);

  vars_add(vars_make_key3(vp),vp);
  list_add(variants_list, vp);
}

List *
vars_lookup1 (Uchar *block_name)
{
  sprintf (vars_key_buf, "%s", block_name);
  return hash_find (variants_hash, vars_key_buf);
}

List *
vars_lookup2 (Uchar *block_name, int col)
{
  sprintf (vars_key_buf, "%s:%d", block_name, col);
  return hash_find (variants_hash, vars_key_buf);
}

Variant *
vars_lookup3 (Uchar *block_name, int col, Uchar *siglum_name)
{
  sprintf (vars_key_buf, "%s:%d:%s", block_name, col, siglum_name);
  return hash_find (variants_hash, vars_key_buf);
}

static Uchar vars_variant_text_buf[1024];
static Uchar vars_main_text_buf[1024];

Uchar *
vars_get_variant_text (Variant *vp)
{
  Source_column *sp = vp->sp;
  *vars_variant_text_buf = '\0';
  while (sp)
    {
      strcat (vars_variant_text_buf, sp->text);
      if (sp == vp->sp_end)
	break;
      else
	{
	  strcat (vars_variant_text_buf, " ");
	  sp = sp->right;
	}
    }
  return vars_variant_text_buf;
}

Uchar *
vars_get_expanded_variant_text (Variant *vp)
{
  Source_column *sp = vp->sp;
  *vars_variant_text_buf = '\0';
  while (sp)
    {
      if (sp->filler_flag)
	{
	  ;
	}
      else if ((*sp->text == '-' || *sp->text == '.')
	       && !sp->composite->zero_flag)
	{
	  strcat (vars_variant_text_buf, "0");
	}
      else if (*sp->text == '+' || *sp->text == '*')
	{
	  strcat (vars_variant_text_buf, sp->composite->text);
	}
      else if (*sp->text == ',')
	{
	  strcat (vars_variant_text_buf, "x");
	}
      else if (sp->text_entry_flag)
	{
	  strcat (vars_variant_text_buf, sp->text);
	}
      if (sp == vp->sp_end)
	break;
      else if (sp->right && !sp->right->filler_flag
	       && !sp->composite->zero_flag)
	strcat (vars_variant_text_buf, sp->composite->delim);
      sp = sp->right;
    }
  return vars_variant_text_buf;
}

Uchar *
vars_get_main_text (Variant *vp)
{
  Source_column *sp = vp->sp;
  *vars_main_text_buf = '\0';
  while (sp)
    {
      if (!sp->composite->zero_flag)
	strcat (vars_main_text_buf, sp->composite->text);
      if (sp == vp->sp_end)
	break;
      else
	{
	  if (!sp->composite->zero_flag && '\n' != *sp->composite->delim)
	    strcat (vars_main_text_buf, sp->composite->delim);
	  sp = sp->right;
	}
    }
  return vars_main_text_buf;
}


static Variant *vars_longest;
static void
vars_max (void*v)
{
  Variant *vp = v;
  if (vp->end > vars_longest->end)
    vars_longest = vp;
}
static void
vars_set_len (void*v)
{
  Variant *vp = v;
  if (vp->end < vars_longest->end)
    {
      vp->end = vars_longest->end;
      vp->sp_end = vp->sp + (vars_longest->sp_end - vars_longest->sp);
    }
}
void
vars_normalize_length (List *l)
{
  vars_longest = list_first(l);
  list_exec (l, vars_max);
  list_exec (l, vars_set_len);
}
