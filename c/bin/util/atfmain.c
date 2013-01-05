/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: psdmain.c,v 0.3 1997/09/08 14:50:33 sjt Exp $
*/
#define CHAR_ONLY
#define FILTER_ONLY
#include "transtab.c"
extern int xml_safe;
int
main (int argc, char **argv)
{
  if (argc > 0 && argv[1] && !strcmp(argv[1],"-x"))
    xml_safe = 1;
  trans_init ("atf");
  trans_filter (stdin);
  return 0;
}
