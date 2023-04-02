#include <stdlib.h>
#include <stdio.h>
#include <mesg.h>
#include <tree.h>
#include <xml.h>
#include <atf.h>
#include <gdl.h>
#include <oraccsys.h>

Mloc xo_loc;
FILE *f_xml;
const char *file;
int verbose;
int status;
int rnvtrace;

extern int atfflextrace , atftrace, cattrace, gdlflextrace, gdltrace;

int check_mode = 0;
int trace_mode = 0;

struct cat axcat;

extern struct catinfo *ax_check (const char *str,size_t len);
     
struct catconfig ax_cat_config =
  {
    NS_XTF, 	/* namespace */
    "xtf", 	/* head */
    atf_name, 	/* parser function to extract name from chunk */
    ax_check,	/* checker function to test names and get name info */
    1,		/* blank lines in record are not errors when processing cattree */
    0		/* blank lines are not record-separators in cattree */
  };

void
ax_input(const char *f)
{
  mesg_init();
  gdlparse_init();
  axcat.f = (f ? f : "<stdin>");
  axcat.c = atf_read(f);
  if (cattrace)
    cat_dump(axcat.c);
  axcat.t = cat_tree(axcat.c, &ax_cat_config);
  if (!check_mode)
    {
      tree_ns_default(axcat.t, NS_XTF);
      tree_xml(NULL, axcat.t);
    }
  atf_term();
  gdlparse_term();
  mesg_print(stderr);
  mesg_term();
}

int
main(int argc, char **argv)
{
  static int multifile = 0;
  options(argc, argv, "ct");

  atfflextrace = atftrace = cattrace = gdlflextrace = gdltrace = trace_mode;

  gdlxml_setup();
  gvl_setup("ogsl", "ogsl");
  nodeh_register(treexml_o_handlers, NS_XTF, treexml_o_generic);
  nodeh_register(treexml_c_handlers, NS_XTF, treexml_c_generic);

  if (argv[optind])
    {
      int fnum = optind;
      multifile = argv[fnum] && argv[fnum+1];
      if (multifile)
	printf("<xtf-multi>");
      while (argv[fnum])
	ax_input(argv[fnum++]);
      if (multifile)
	printf("</xtf-multi>");
    }
  else
    ax_input(NULL);
}

int
opts(int opt, char *arg)
{
  switch (opt)
    {
    case 'c':
      check_mode = 1;
      break;
    case 't':
      trace_mode = 1;
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

void
help(void)
{
  fprintf(stderr, "ax: read input from stdin; use -c to check only\n");
}
