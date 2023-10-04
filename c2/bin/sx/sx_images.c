#include <stdlib.h>
#include <signlist.h>
#include <sx.h>

/**sx_images -- load the image manifests into a Roco
 *
 * -the number of columns is the number of image manifests;
 * -the number of rows is the number of OIDs referenced in oid_sort_keys
 * -the value of oid_sort_keys{OID} is the integer index into the rows
 * -each entry in a manifest is stored in row[oid-sort-key][manifest-index]
 *
 */
Roco *
sx_images(struct sl_signlist *sl)
{
  Roco *r = NULL;
  if (sl->images)
    {
      int ncols = list_len(sl->images)+1;
      int nrows = oid_sort_keys->key_count;
      r = roco_create(nrows, ncols);
      Mloc *m;
      for (m = list_first(sl->images); m; m = list_next(sl->images))
	{
	  Roco *mr = roco_load(m->user, 0, NULL, NULL, NULL);
	  if (mr)
	    {
	      int i;
	      for (i = 0; i < mr->nlines; ++i)
		{
		  if (*mr->rows[i][0] != '@' && *mr->rows[i][0] != '#')
		    {
		      int s = 0;
		      if ((s = (uintptr_t)hash_find(oid_sort_keys, mr->rows[i][0])))
			r->rows[i][s] = mr->rows[i][1];
		      else if ((s = (uintptr_t)hash_find(oid_sort_keys, mr->rows[i][1])))
			r->rows[i][s] = mr->rows[i][0];
		      else
			fprintf(stderr, "%s:%d: no OID found for %s or %s\n",
				mr->file, i, (char*)mr->rows[0], (char*)mr->rows[1]);
		    }
		}
	    }
	}
    }
  roco_write(stdout, r);
  return r;
}
