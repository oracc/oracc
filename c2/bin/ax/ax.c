#include <cat.h>

struct cat axcat;

int
main(int argc, const char **argv)
{
  axcat.f = argv[1];
  axcat.c = cat_read(argv[1]);
  cat_dump(axcat.c);
}
