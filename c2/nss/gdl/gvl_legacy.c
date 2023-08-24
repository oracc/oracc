
const char *
gvl_legacy(const char *f,size_t l,const unsigned char *g,int t);
{
  const char *report = "http://oracc.museum.upenn.edu/ogsl/reportingnewvalues/";
  vwarning("%s: unknown sign-name grapheme. To request adding it please visit:\n\t%s",g,report);
  exit_status = 1;
  --status;
  bad_grapheme = 1;
}

int
gvl_psl_lookup(unsigned const char *g)
{
  return gvl_lookup(g) != NULL;
}
