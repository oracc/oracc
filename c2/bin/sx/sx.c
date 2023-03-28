#include <stdlib.h>
#include <tree.h>
#include <cat.h>
#include <gdl.h>

struct cat sxcat;

extern struct catinfo *sx_check (const char *str,size_t len);
     
struct catconfig sx_cat_config =
  {
    "signs", 	/* head */
    cat_name, 	/* parser function to extract name from chunk */
    sx_check	/* checker function to test names and get name info */
  };

int
main(int argc, const char **argv)
{
  mesg_init();
  gvl_setup("ogsl", "ogsl");
  nodehandler_register(treexmlhandlers, NT_GDL, gdl_xml_handler);
  gdlparse_init();
  sxcat.f = argv[1];
  sxcat.c = cat_read(argv[1]);
  sxcat.t = cat_herd(sxcat.c, &sx_cat_config);
  tree_xml(NULL, sxcat.t);

  gdlparse_term();
}
