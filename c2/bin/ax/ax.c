#include <stdlib.h>
#include <stdio.h>
#include <mesg.h>
#include <tree.h>
#include <xml.h>
#include <atf.h>
#include <gdl.h>

Mloc xo_loc;
FILE *f_xml;
const char *file;
int verbose;
int status;
int rnvtrace;

extern int atfflextrace , atftrace, cattrace;

struct cat axcat;

extern struct catinfo *ax_check (const char *str,size_t len);
     
struct catconfig ax_cat_config =
  {
    NS_XTF, 	/* namespace */
    "xtf", 	/* head */
    atf_name, 	/* parser function to extract name from chunk */
    ax_check,	/* checker function to test names and get name info */
    1		/* ignore blank lines when processing cattree */
  };

int
main(int argc, const char **argv)
{
  atftrace = cattrace = 0;
  atfflextrace = 0;
  mesg_init();
  gdlxml_setup();
  gvl_setup("ogsl", "ogsl");
  nodeh_register(treexml_o_handlers, NS_XTF, treexml_o_generic);
  nodeh_register(treexml_c_handlers, NS_XTF, treexml_c_generic);
  gdlparse_init();
  /*fprintf(stderr, "catchunk *=%lu\n", sizeof(struct catchunk *));*/
  axcat.f = argv[1];
  axcat.c = atf_read(argv[1]);
  if (cattrace)
    cat_dump(axcat.c);
  axcat.t = cat_tree(axcat.c, &ax_cat_config);
  tree_ns_default(axcat.t, NS_XTF);
  tree_xml(NULL, axcat.t);
  gdlparse_term();
}
