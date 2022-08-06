static void
edit_check_entry(struct entry *e)
{
  struct sense *sp = NULL;
  if (NULL != e->ed)
    {
      if (e->ed->target->cf)
	{
	  unsigned const char *closed_t = cgp_str(e->ed->target,0);
	  /* fprintf(stderr, "found e->ed; target=%s\n",closed_t); */
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
      /* fprintf(stderr, "found sp->ed\n") */;
}


# form validation
  if (f2p->base)
    {
      if (!hash_find(e->b_pri, f2p->base))
	{
	  unsigned char *pri = hash_find(e->b_alt, f2p->base);
	  if (pri)
	    vwarning("alt base %s should be primary %s", f2p->base, pri);
	  else
	    vwarning("base %s not found in @bases", f2p->base);
	}
    }
  
