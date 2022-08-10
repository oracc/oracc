#include <stdlib.h>
#include <stdio.h>
#include "xmlutil.h"
#include "hash.h"
#include "tokenizer.h"
#include "initterm.h"
#include "f2.h"
#include "sigs.h"

#if 0
int bootstrap_mode = 0;
int fuzzy_aliasing = 0;
int lem_autolem = 0;
int lem_dynalem = 0;
#endif

const char *project = NULL;

int
main(int argc, char **argv)
{
  struct f2 *f2p = malloc(sizeof(struct f2));
  char buf[4096];
  int line = 0;
  char *file = NULL;
  atf_init();
  f2_init();
  sig_context_init();
  if (argc > 0)
    {
      file = argv[1];
      fprintf(stdout, "<?atf-file %s?><x xmlns:g=\"http://oracc.org/ns/gdl/1.0\">", file);
      while (fgets(buf,BUFSIZ-1,stdin))
	{
	  const unsigned char *xbase = NULL;
	  unsigned char *xform = NULL;
	  ++line;
	  f2_parse((unsigned char*)file,line,(unsigned char *)buf,f2p,NULL,global_scp);
	  if (f2p->form)
	    {
	      const unsigned char *tmp = xmlify(f2p->form);
	      xform = malloc(strlen((char*)tmp)+1);
	      strcpy((char*)xform,(char*)tmp);
	    }
	  if (f2p->base && strcmp((char*)f2p->form, (char*)f2p->base))
	    xbase = xmlify(f2p->base);
	  if (xbase)
	    fprintf(stdout, "<?atf-line %d?><s g:me=\"yes\">%s %s</s\n>", line, (char*)xform, (char*)xbase);
	  else
	    fprintf(stdout, "<?atf-line %d?><s g:me=\"yes\">%s</s\n>", line, (char*)xform);
	  if (xform)
	    free(xform);
	}
      fprintf(stdout, "</x>");
      xmlify(NULL);
    }
  f2_term();
  return 0;
}
const char *prog = "sigx";
const char *usage_string = "sigx <signatures from stdin>";
int major_version = 1;
int minor_version = 0;
void help(void) { ; }
int opts() { return 0; }
/*int verbose = 0;*/

