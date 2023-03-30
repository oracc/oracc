#include <stdlib.h>
#include <stdio.h>
#include <mesg.h>
#include <tree.h>
#include <xml.h>
#include <cat.h>
#include <gdl.h>

Mloc xo_loc;
FILE *f_xml;
const char *file;
int verbose;
int status;
int rnvtrace;

struct cat sxcat;

extern struct catinfo *sx_check (const char *str,size_t len);
     
struct catconfig sx_cat_config =
  {
    NS_SL, 	/* namespace */
    "signs", 	/* head */
    cat_name, 	/* parser function to extract name from chunk */
    sx_check,	/* checker function to test names and get name info */
    0		/* error on blank lines when processing cattree */
  };

int
main(int argc, const char **argv)
{
  mesg_init();
  gdlxml_setup();
  gvl_setup("ogsl", "ogsl");
  nodeh_register(treexml_o_handlers, NS_SL, treexml_o_generic);
  /* nodeh_register(treexml_p_handlers, NS_GDL, gdl_xml_handler); */
  nodeh_register(treexml_c_handlers, NS_SL, treexml_c_generic);
  gdlparse_init();
  sxcat.f = argv[1];
  sxcat.c = cat_read(argv[1]);
  sxcat.t = cat_tree(sxcat.c, &sx_cat_config);
  tree_ns_default(sxcat.t, NS_SL);
  tree_xml(NULL, sxcat.t);
  gdlparse_term();
}
