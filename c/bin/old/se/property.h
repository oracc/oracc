/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: property.h,v 0.4 1997/09/08 14:50:12 sjt Exp s $
*/
#ifndef PROPERTY_H_
#define PROPERTY_H_ 1
#define PROP_PN   0x01 /* personal name */
#define PROP_DN   0x02 /* divine name */
#define PROP_RN   0x04 /* royal name */
#define PROP_FN   0x08 /* field name */
#define PROP_TN   0x10 /* temple name */
#define PROP_GN   0x20 /* geographic name */
#define PROP_XN   0x40 /* other proper noun */
#define PROP_NM   0x80 /* noun morpheme */
#define PROP_VM  0x100 /* verb morpheme */
#define PROP_NL  0x200 /* noun lexeme */
#define PROP_VL  0x400 /* verb lexeme */
#define PROP_OL  0x800 /* other lexeme */
#define PROP_LB 0x1000 /* line begin */
#define PROP_LE 0x2000 /* line end */
#define PROP_WB 0x4000 /* word begin */
#define PROP_WE 0x8000 /* word end */

#define SIGN_NAME_PROP 0x10000

#define set_property(var,attr)	(var|=(attr))
#define has_property(var,attr)	(var&attr)
#define no_word_props(var) (var&=(PROP_LB|PROP_LE|PROP_WB|PROP_WE))

extern struct prop_tab *propmask(register const char *str,register unsigned int len);

enum prop_group { pg_properties = 0x1000000, pg_start_column = 0x2000000 };

#define pg_mask(x) ((x)&~(pg_properties|pg_start_column))

struct prop_tab
{
  const char *name;
  enum prop_group group;
  int prop;
};

#endif
