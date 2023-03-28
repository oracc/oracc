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
  Tree *tp = NULL;

  mesg_init();
  gvl_setup("ogsl", "ogsl");
  nodehandler_register(catxmlhandlers, NT_GDL, gdl_xml_handler);
  gdlparse_init();
  sxcat.f = argv[1];
  sxcat.c = cat_read(argv[1]);

  /*cat_dump(sxcat.c);*/
  tp = cat_herd(sxcat.c, &sx_cat_config);

  cat_xml(NULL, tp);

  gdlparse_term();
}
