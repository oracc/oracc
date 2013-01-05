#include <psd_base.h>
#include <fname.h>
extern void usage(void);
int
main (int argc, char **argv)
{
  char *npath = NULL;
  if (argv[1] && !strcmp(argv[1], "-d"))
    {
      npath = "";
      --argc;
      ++argv;
    }
  if (argc < 2 || argc > 3)
    usage();
  printf ("%s", 
	  new_path(new_ext(xstrdup(argv[1]), argv[2] ? argv[2] : "", TRUE),
		   npath,
		   TRUE));
  exit(0);
}
void
help ()
{
  fprintf (stderr, "  -d  strip directory from filename\n\n");
}
const char *prog="newext";
const char *opts="";
const char *usage_string="[-d] <filename> <new-extension>";
int major_version = 1, minor_version = 0;
