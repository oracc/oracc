#include "ctype128.h"
#include "gx.h"

static int edit_status;
FILE *f_edit = NULL;

int
edit_add(unsigned char **ll, struct entry *e)
{
  unsigned char *ea = ll[0];
  char ctxt = '\0';
  struct edit *ed = NULL;
  struct sense *snode = NULL;

  /* set the ctxt */
  if (!strncmp((ccp)&ll[0][1], "@entry", strlen("@entry")))
    ctxt = 'e';
  else if (!strncmp((ccp)&ll[0][1], "@sense", strlen("@sense")))
    ctxt = 's';
  else if (!strncmp((ccp)ll[-1], "@entry", strlen("@entry")))
    ctxt = 'e';
  else if (!strncmp((ccp)ll[-1], "@sense", strlen("@sense")))
    ctxt = 's';
  else
    {
      warning("edit_add unable to set ctxt");
      return -1;
    }

  if (ctxt == 's')
    snode = list_last(e->senses);
  
  ed = calloc(1, sizeof(struct edit));
  
  /* set the type */
  if (*ea == '+')
    {
      if (ctxt == 'e')
	{
	  ed->type = ADD_E;
	  ed->owner = e;
	  ed->lp = &e->l;
	}
      else if (ctxt == 's')
	{
	  ed->type = ADD_S;
	  ed->owner = snode;
	  ed->lp = &snode->l;
	}
      else
	{
	  warning("edit_add + in unknown ctxt");
	  return -1;
	}
    }
  else if (*ea == '>')
    {
      unsigned char *t = ea;
      if (t[1] == '>')
	{
	  ++t;
	  if (ctxt == 'e')
	    {
	      ed->type = MRG_E;
	      ed->owner = e;
	      ed->lp = &e->l;
	    }
	  else
	    {
	      ed->type = MRG_S;
	      ed->owner = snode;
	      ed->lp = &snode->l;
	    }
	  if (t[1] == '!')
	    {
	      ed->force = 1;
	      ++t;
	    }
	}
      else
	{
	  if (ctxt == 'e')
	    {
	      ed->type = REN_E;
	      ed->owner = e;
	      ed->lp = &e->l;
	    }
	  else
	    {
	      ed->type = REN_S;
	      ed->owner = snode;
	      ed->lp = &snode->l;
	    }
	}
      ++t;
      while (isspace(*t))
	++t;
      if (ctxt == 'e')
	cgp_parse(&ed->target,t,ed->lp);
      else
	ed->sp = parse_sense_sub(t,ed->lp);
    }
  else if (*ea == '-')
    {
      if (ctxt == 'e')
	{
	  ed->type = DEL_E;
	  ed->owner = e;
	  ed->lp = &e->l;
	}
      else
	{
	  ed->type = DEL_S;
	  ed->owner = snode;
	  ed->lp = &snode->l;
	}
    }
  else
    {
      fprintf(stderr, "edit_add passed unparseable data %s\n", (ccp)ea);
      return -1;
    }

  /* warn or set */
  if (ctxt == 'e')
    e->ed = ed;
  else if (ctxt == 's')
    snode->ed = ed;

  return 0;
}

static void
edit_check_entry(struct entry *e)
{
  struct sense *sp = NULL;
  if (NULL != e->ed)
    {
      if (e->ed->target.cf)
	{
	  unsigned const char *closed_t = cgp_str(&e->ed->target,0);
	  fprintf(stderr, "found e->ed; target=%s\n",closed_t);
	  if (hash_find(e->owner->hentries, closed_t))
	    {
	      fprintf(stderr, "target %s OK\n", closed_t);
	      /* if we are renaming this is an error */
	      if (e->ed->type == REN_E)
		vwarning2(e->ed->lp->file, e->ed->lp->line, "(edit) can't rename to existing entry %s", closed_t);
	    }
	  else
	    {
	      fprintf(stderr, "target %s NOT\n", closed_t);
	      /* if we are merging this is an error */
	      if (e->ed->type == MRG_E && !e->ed->force)
		vwarning2(e->ed->lp->file, e->ed->lp->line,
			  "(edit) can't merge to non-existent entry %s (use >>! to override)", closed_t);
	    }
	}
    }
  for (sp = list_first(e->senses); sp; sp = list_next(e->senses))
    if (sp->ed)
      fprintf(stderr, "found sp->ed\n");
}

int
edit_check(struct cbd *c)
{
  edit_status = 0;
  list_exec(c->entries, (void (*)(void*))edit_check_entry);
  return edit_status;
}

static void
edit_script_entry(struct entry *e)
{
  struct sense *sp = NULL;
  if (NULL != e->ed)
    {
      if (e->ed->target.cf)
	{
	  unsigned const char *closed_t = cgp_str(&e->ed->target,0);
	  if (hash_find(e->owner->hentries, closed_t))
	    {
	      /* if we are renaming this is an error */
	      if (e->ed->type == REN_E)
		++edit_status;
	      else
		{
		  fprintf(f_edit, "@%d\n", e->ed->lp->line);
		  fprintf(f_edit, ":ren %s\n", closed_t);
		}
	    }
	  else
	    {
	      /* if we are merging this is an error */
	      if (e->ed->type == MRG_E && !e->ed->force)
		++edit_status;
	      else
		{
		  fprintf(f_edit, "@%d\n", e->ed->lp->line);
		  fprintf(f_edit, ":mrg %s\n", closed_t);
		}
	    }
	}
      else if (e->ed->type == ADD_E)	
	{
	  fprintf(f_edit, ":add %s\n", ((struct entry *)(e->ed->owner))->cgp.closed);
	}
      else if (e->ed->type == DEL_E)
	{
	  fprintf(f_edit, ":del\n");
	  fprintf(f_edit, ":why\n");
	}
      else
	{
	  fprintf(stderr, "edit_script_entry unknown ed->type %d\n", e->ed->type);
	  ++edit_status;
	}
    }
  for (sp = list_first(e->senses); sp; sp = list_next(e->senses))
    if (sp->ed)
      fprintf(stderr, "found sp->ed\n");
}

int
edit_script(struct cbd *c)
{
  edit_status = 0;

  f_edit = xfopen("edit.edit", "w");
  
  /* header */
  fprintf(f_edit, ":cbd %s\n", c->l.file);

  /* edit instructions */
  list_exec(c->entries, (void (*)(void*))edit_script_entry);

  xfclose("edit.edit", f_edit);
  f_edit = NULL;
  
  return edit_status;
}
