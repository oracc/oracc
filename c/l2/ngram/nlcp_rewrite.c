#define NULL (void*)0
#include "warning.h"
#include "ngram.h"
#include "ilem_form.h"

static void
apply_tts(struct f2 *fp, struct CF *tt)
{
  int i;
  if (tt)
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

void
nlcp_rewrite(struct xcl_context *xcp, struct ML *mlp)
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
	      if (!BIT_ISSET(mp->lp->f->finds[i]->f2,F2_FLAGS_READ_ONLY))
		{
		  apply_tts(mp->matching_f2s[i], mp->tt);
		  mp->lp->f->finds[i]->f2 = *mp->matching_f2s[i];
		  BIT_SET(mp->lp->f->finds[i]->f2,F2_FLAGS_READ_ONLY));
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
