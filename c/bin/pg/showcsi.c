#include <stdio.h>
#include <sortinfo.h>

int verbose;
const char *project;
struct sortinfo*sip;

extern void si_debug_dump_csi(struct sortinfo *dsi);
int
main(int argc, char **argv)
{
  project = argv[1];
  sip = si_load_csi();
  if (sip)
    si_debug_dump_csi(sip);
  else
    printf("open failed on %s\n",argv[1]);
  return 0;
}
const char *prog = "showcsi";
void help() { }
int options() { return 0; }
