/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: alias.h,v 0.4 1997/09/08 14:50:09 sjt Exp $
*/
#ifndef ALIAS_H_
#define ALIAS_H_ 1
#define alias_fast_get_alias(g) hash_find(aliases,(g))
extern Hash_table *aliases;
extern char * alias_get_alias (char *orig_grapheme);
extern int alias_check_date (const char *project, const char *index, Boolean bomb);
extern void alias_fast_init (const char *project, const char *index);
extern void alias_fast_term (void);
extern void alias_index_aliases (const char *project, const char *index);
extern Dbi_index* alias_init (const char *project, const char *index);
extern void alias_term (void);
extern void alias_use(Dbi_index *aliases);
#endif
