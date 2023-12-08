/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: collate.h,v 0.4 2001/11/30 20:50:24 s Exp s $
*/
#ifndef COLLATE_H_
#define COLLATE_H_ 1
#include "colltype.h"
/*extern struct Collate_info *curr_collate;*/
extern struct Collate_info_list_node collate_infos[];
int collate_cmp_utf8 (const Uchar *k1, const Uchar *k2);
int collate_cmp_graphemes (const Uchar *k1, const Uchar *k2);
Uchar *collate_makekey (Uchar *s);
Uchar *collate_makekey_i (Uchar *s);
Uchar *collate_significant (Uchar *s, Boolean foldcase);
void collate_set_tiles (void);
void collate_set_tiles_i (void);
extern int collate_obey_delims;
extern int graphemic_compare (const unsigned char *a, const unsigned char *b);
extern void graphemic_init (void);
extern const Uchar *grapheme_base (const Uchar *txt, Boolean foldcase);
extern void collate_init(const unsigned char *name);
#endif
