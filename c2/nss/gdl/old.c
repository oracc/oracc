#if 0
/* After refactoring the parser this became redundant but I'm keeping it around in case I ever want it again */
void
gdl_append(Node *ynp, const char *s)
{
  if (ynp)
    {
      if (ynp->data)
	{
	  char *p = (char*)pool_alloc(strlen(ynp->data)+strlen(s)+1, gdlpool);
	  (void)sprintf(p, "%s%s", ynp->data, s);
	  ynp->data = p;
	}
    }
  else
    {
      fprintf(stderr, "gdl_append passed NULL ynp\n");
    }
}
#endif
