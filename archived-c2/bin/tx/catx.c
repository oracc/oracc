#include <cat.h>

struct cat catx;

int
main(int argc, const char **argv)
{
  catx.f = argv[1];
  catx.c = cat_read(argv[1]);
  cat_dump(catx.c);
}
