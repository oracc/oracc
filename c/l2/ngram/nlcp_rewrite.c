#define NULL (void*)0
#include "warning.h"
#include "ngram.h"
#include "ilem_form.h"

static void
replace_finds(struct ilem_form *ilemp, struct f2**f2s, int nf2s)
{
  int i;
  if (!ilemp->finds)
    ilemp->finds = malloc(nf2s * sizeof(struct ilem_form*)); /* FIXME: this should use better mem alloc */
  for (i = 0; i < nf2s; ++i)
    if (f2s[i]->owner) /* COF TAILs have null owner; is this right? */
      ilemp->finds[i] = f2s[i]->owner;
  ilemp->fcount = nf2s;
}

void
nlcp_rewrite(struct xcl_context *xcp, struct ML *mlp)
{
  int j;

  for (j = 0; j < mlp->matches_used; ++j)
    {
      struct match *mp = &mlp->matches[j];

      if (!mp->lp->f->finds)
	continue;

      if (!mp->wild)
	{
	  int i;
	  for (i = 0; mp->matching_f2s[i]; ++i)
	    {
	      if (!BIT_ISSET(mp->matching_f2s[i]->flags,F2_FLAGS_READ_ONLY))
		{
		  if (mp->tt && mp->tt->f2)
		    {
		      if (mp->tt->clear)
			f2_clear(mp->matching_f2s[i]);
		      f2_inherit(mp->matching_f2s[i], mp->tt->f2);
		    }
		  BIT_SET(mp->matching_f2s[i]->flags, F2_FLAGS_READ_ONLY);
		}
	    }
	  if (mp->lp->f->ambig)
	    {
	      struct ilem_form *ifp = NULL, *rover = NULL;
	      for (i = 0; i < mp->nmatches; )
		{
		  struct ilem_form *this = mp->matching_f2s[i]->owner;
		  int j = 1;
		  while ((i+j) < mp->nmatches)
		    {
		      if (mp->matching_f2s[i+j]->owner == this)
			++j;
		      else
			break;
		    }
		  replace_finds(mp->matching_f2s[i]->owner, &mp->matching_f2s[i], j);
		  if (ifp)
		    {
		      rover->ambig = mp->matching_f2s[i]->owner;
		      rover = rover->ambig;
		    }
		  else
		    {
		      rover = ifp = mp->matching_f2s[i]->owner;
		      if (ifp != mp->lp->f)
			{
			  /* The match is not from the first of the ambiguous alternates
			     so we need to set some of its members from the head */
			  ifp->ref = mp->lp->ref;
			}
		    }
		  i += j;
		}
	      if (rover)
		rover->ambig = NULL;
	      mp->lp->f = ifp;
	    }
	  else
	    {
	      replace_finds(mp->lp->f, mp->matching_f2s, mp->nmatches);
	    }
	}
    }
}

#if 0
static void
apply_tts(struct f2 *fp, struct CF *tt)
{
  int i;
  if (tt && tt->preds)
    {
      for (i = 0; tt->preds[i]; ++i)
	{
	  const char *aname = tt->preds[i]->key;
	  const char *aval = tt->preds[i]->value;
	  if (aval && *aval && strcmp(aval,"*"))
	    {
	      struct tok_tab *tp = NULL/*xfftok(aname,strlen(aname))*/;
	      if (tp)
		{
		  /*fbyo(fp,tp->offset) = (char*)aval;*/
		  ngdebug("[apply_tts] key=%s; value=%s",aname,aval);
		}
	      else
		{
		  vwarning("ngramify: internal error in apply_tts: %s not in struct form",aname);
		  return;
		}
	    }
	}
    }
}
#endif

void
xnlcp_rewrite(struct xcl_context *xcp, struct ML *mlp)
{
  int j;

  for (j = 0; j < mlp->matches_used; ++j)
    {
      struct match *mp = &mlp->matches[j];

      /* FIXME: HOW CAN THIS HAPPEN (possibly when relemming something that has ATF errors) */
      if (!mp->lp->f->finds)
	continue;

      if (!mp->wild)
	{
	  int i;
	  for (i = 0; mp->matching_f2s[i]; ++i)
	    {
	      if (!BIT_ISSET(mp->matching_f2s[i]->flags /*mp->lp->f->finds[i]->f2.flags*/,
			     F2_FLAGS_READ_ONLY))
		{
		  f2_inherit(mp->matching_f2s[i], mp->tt->f2);
		  BIT_SET(mp->matching_f2s[i]->flags /*mp->lp->f->finds[i]->f2.flags*/,
			  F2_FLAGS_READ_ONLY);
#if 0
		  apply_tts(mp->matching_f2s[i], mp->tt);
		  if (mp->tt && mp->tt->f2)
		    {
		      /* mp->lp->f->finds[i]->f2 = *mp->tt->f2; */
		      if (mp->tt->f2->sense)
			mp->lp->f->finds[i]->f2.sense = mp->tt->f2->sense;
		    }
		  else
		    mp->lp->f->finds[i]->f2 = *mp->matching_f2s[i];
#endif
		}
	    }
#if 1
	  mp->lp->f->finds[i] = NULL;
	  mp->lp->f->fcount = i;
#else
	  if (mp->matching_f2s[1])
	    {
	      struct ilem_form *rover;
	      for (i = 1, rover=mp->lp->f; mp->matching_f2s[i]; ++i, rover = rover->ambig)
		{
		  rover->ambig->f2 = *mp->matching_f2s[i];
		  ++mp->lp->f->acount;
		}
	      rover->ambig = NULL;
	    }
	  else
	    {
	    }
#endif
	}
    }
}
