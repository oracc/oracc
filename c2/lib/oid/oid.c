#include <stdio.h>
#include <oid.h>

int
main(int argc, const char **argv)
{
  oid_load();
  printf("next free OID is %d\n", oid_next_id());
}

const char *prog = "oid";
