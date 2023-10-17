#include "gx.h"

extern FILE *f_edit;
static int edit_status;

static void
edit_script_entry(struct entry *e)
{
  struct sense *sp = NULL;
  if (NULL != e->ed)
    {
      if (e->ed->target->cf)
	{
	  unsigned const char *closed_t = cgp_str(e->ed->target,0);
	  if (hash_find(e->owner->hentries, closed_t))
	    {
	      /* if we are renaming this is an error */
	      if (e->ed->type == REN_E)
		++edit_status;
	      else
		{
		  fprintf(f_edit, "@%d\n", e->ed->lp->first_line);
		  fprintf(f_edit, ":ent @entry %s\n", ((struct entry *)(e->ed->owner))->cgp->tight);
		  fprintf(f_edit, ":rnm >@entry %s\n", closed_t);
		}
	    }
	  else
	    {
	      /* if we are merging this is an error */
	      if (e->ed->type == MRG_E && !e->ed->force)
		++edit_status;
	      else
		{
		  fprintf(f_edit, "@%d\n", e->ed->lp->first_line);
		  fprintf(f_edit, ":ent @entry %s\n", ((struct entry *)(e->ed->owner))->cgp->tight);
		  fprintf(f_edit, ":mrg =@entry %s\n", closed_t);
		}
	    }
	}
      else if (e->ed->type == ADD_E)	
	{
	  fprintf(f_edit, "@%d\n", e->ed->lp->first_line);
	  fprintf(f_edit, ":add +@entry %s\n", ((struct entry *)(e->ed->owner))->cgp->tight);
	}
      else if (e->ed->type == DEL_E)
	{
	  fprintf(f_edit, "@%d\n", e->ed->lp->first_line);
#if 0
	  /* This is emitted by cbdedit.plx but it's redundant and not aligned with :add +@entry */
	  fprintf(f_edit, ":ent -@entry %s\n", ((struct entry *)(e->ed->owner))->cgp->tight);
#endif
	  fprintf(f_edit, ":del -@entry %s\n", e->cgp->tight);
	  fprintf(f_edit, ":why %s\n", e->ed->why);
	}
      else
	{
	  fprintf(stderr, "edit_script_entry unknown ed->type %d\n", e->ed->type);
	  ++edit_status;
	}
    }
  for (sp = list_first(e->senses); sp; sp = list_next(e->senses))
    if (sp->ed)
      {
	fprintf(f_edit, "@%d\n", sp->ed->lp->first_line);
	fprintf(f_edit, ":ent @entry %s\n", ((struct entry *)(e->ed->owner))->cgp->tight);
	if (sp->ed->type == ADD_S)
	  fprintf(f_edit, ":add +@sense %s %s\n", sp->pos, sp->mng);
	else if (sp->ed->type == DEL_S)
	  fprintf(f_edit, ":del -@sense %s %s\n", sp->pos, sp->mng);
	else
	  fprintf(stderr, "edit_script_entry unprocessed edit type %d\n", sp->ed->type);
      }
}

int
edit_script(struct cbd *c)
{
  edit_status = 0;

  f_edit = xfopen("gx.edit", "w");
  
  /* header */
  fprintf(f_edit, ":cbd %s\n", c->l.file);

  /* edit instructions */
  list_exec(c->entries, (void (*)(void*))edit_script_entry);

  xfclose("edit.edit", f_edit);
  f_edit = NULL;
  
  return edit_status;
}
