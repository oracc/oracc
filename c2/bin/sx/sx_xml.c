#include <xml.h>
#include <ns-asl.h>
#include <rnvif.h>
#include <rnvxml.h>
#include <sx.h>

void
sx_xml_init()
{
  rnvxml_init_err();
  rnvif_init();
  rnvxml_init(&asl_data, "asl");
}

void
sx_xml(struct sl_signlist *sl)
{
  extern Mloc *xo_loc;
  struct rnvval_atts *ratts;
  sx_xml_init();
  mesg_init();
  xo_loc->file = "stdin"; xo_loc->line = 1;
  ratts = rnvval_aa("x", "project", "ogsl", NULL);
  rnvval_ea("sl:signlist", ratts);
  rnvxml_ea("sl:signlist", ratts);
  mesg_print(stderr);
}
