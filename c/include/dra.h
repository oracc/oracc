/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: dra.h,v 0.3 1997/09/08 14:49:56 sjt Exp $
*/

#ifndef _DRA_H
#define _DRA_H	1

struct Array
{ 
  void *data;
  size_t element_size;	/* size of one element. num of els = in_use/element_size*/
  size_t increment;
  size_t allocated;	/* amount of space allocated, in bytes */
  size_t in_use;	/* amount of space in use, in bytes */
};
typedef struct Array Array;

#define dra_elem(ap_arg,off_arg) \
		      ( \
		        (VOIDMATH((ap_arg)->data)) \
		        + (off_arg * (ap_arg)->element_size) \
		      )
#define dra_count(ap) ((ap)->in_use/(ap)->element_size)
#define dra_data(ap) ((ap)->data)
#define dra_elt_size(ap) ((ap)->element_size)

extern Array *dra_attach (void *data, size_t element_size, size_t count);
extern Array *dra_create (size_t element_size, size_t initial_alloc, size_t increment);
extern size_t dra_add (Array*ap, const void *data, size_t count);
extern void dra_free (Array*ap);
extern void *dra_trim (Array *ap, size_t count);
extern void dra_resize (Array*ap);
extern void *dra_detach (Array*ap, size_t *count);
extern Array *dra_read (const char *fn, FILE *fp);
extern void dra_write (Array *ap, const char *fn, FILE *fp);

#endif /* _DRA_H */
