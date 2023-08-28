#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <roco.h>

int
main(int argc, char *const *argv)
{
  Roco *r = roco_load("-");
  if (argv[1] && !strcmp(argv[1], "-t"))
    roco_write_trtd(stdout, r);
  else
    roco_write(stdout, r);
}
