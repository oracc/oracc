#include <stdlib.h>
#include <stdio.h>
#include <roco.h>

int
main(int argc, char *const *argv)
{
  Roco *r = roco_load("-");
  roco_write(stdout, r);
}
