#include <psd_base.h>
#include <f2.h>
extern void usage(void);
int verbose = 0;
int
main (int argc, char **argv)
{
  psl_init();
  if (argv[1] && argv[2])
    {
      int ret = f2_form_signs((const unsigned char *)argv[1], 
			      (const unsigned char *)argv[2]);
      fprintf(stderr, "%s and %s are%s written with the same signs\n",
	      argv[1], argv[2], ret ? "" : " not");
    }
  psl_term();
  exit(0);
}
void
help ()
{
  fprintf (stderr, "  -d  strip directory from filename\n\n");
}
const char *prog="xalias";
const char *opts="";
const char *usage_string="<FORM1> <FORM2>";
int major_version = 1, minor_version = 0;
