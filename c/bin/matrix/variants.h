#ifndef VARIANTS_H_
#define VARIANTS_H_

#include <list.h>

struct VARIANT 
{
  int begin;
  int end;
  Uchar *block_name;
  Uchar *siglum_name;
  struct SOURCE_COLUMN *sp, *sp_end;
  struct VARIANT *prev; /* previous variant to this column/sequence of columns */
  struct VARIANT *next; /* next variant to this column/sequence of columns*/
};

typedef struct VARIANT Variant;

extern List *variants_list;
extern List * vars_assq1 (Variant *vp);
extern List * vars_assq2 (Variant *vp);
extern Variant * vars_assq3 (Variant *vp);
extern Uchar * vars_make_key1 (Variant *vp);
extern Uchar * vars_make_key2 (Variant *vp);
extern Uchar * vars_make_key3 (Variant *vp);
extern void vars_init (void);
extern void vars_register (Variant *vp);
extern List * vars_lookup1 (Uchar *block_name);
extern List * vars_lookup2 (Uchar *block_name, int col);
extern Variant * vars_lookup3 (Uchar *block_name, int col, Uchar *siglum_name);
extern Uchar *vars_get_variant_text (Variant *vp);
extern Uchar *vars_get_expanded_variant_text (Variant *vp);
extern Uchar *vars_get_main_text (Variant *vp);
extern void vars_normalize_length (List *l);

#endif
