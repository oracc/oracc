#include <stdlib.h>
#include <cat.h>

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
  sxcat.f = argv[1];
  sxcat.c = cat_read(argv[1]);
  /*cat_dump(sxcat.c);*/
  tp = cat_herd(sxcat.c, &sx_cat_config);
  cat_xml(NULL, tp);
}
