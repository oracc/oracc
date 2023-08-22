#include <stdio.h>
#include <mesg.h>
#include <oid.h>

int
main(int argc, const char **argv)
{
  Oids *o = NULL;

  mesg_init();
  o = oid_load();
  printf("next free OID is %d\n", oid_next_id(o));
  if (oid_parse(o, ot_oids))
    {
      mesg_print(stderr);
      printf("parse errors in %s\n", o->file);
    }
  else
    printf("%s parsed OK\n", o->file);
}

const char *prog = "oid";
