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
void
sx_images(struct sl_signlist *sl)
{
  if (sl->images)
    {
      int ncols = list_len(sl->images)+1;
      int nrows = oid_sort_keys->key_count;
      Roco *r = roco_create(nrows, ncols+1); /* we added one for the OID;
						this +1 is for the NULL
						term */
      Mloc *m;
      int nm;
      sl->ic = calloc(list_len(sl->images), sizeof(struct sx_iconfig));

      for (m = list_first(sl->images), nm=0; m; m = list_next(sl->images), ++nm)
	{
	  Roco *mr = roco_load(m->user, 0, NULL, NULL, NULL);
	  sl->iconfig[nm].r = mr;
	  if (mr)
	    {
	      int i;
	      for (i = 0; i < mr->nlines; ++i)
		{
		  if (*mr->rows[i][0] != '@' && *mr->rows[i][0] != '#')
		    {
		      int s = 0;
		      if ((s = (uintptr_t)hash_find(oid_sort_keys, mr->rows[i][0])))
			{
			  if (!r->rows[s-1][0])
			    r->rows[s-1][0] = mr->rows[i][0];
			  r->rows[s-1][nm+1] = mr->rows[i][1];
			}
		      else if ((s = (uintptr_t)hash_find(oid_sort_keys, mr->rows[i][1])))
			{
			  if (!r->rows[s-1][0])
			    r->rows[s-1][0] = mr->rows[i][1];
			  r->rows[s-1][nm+1] = mr->rows[i][0];
			}
		      else
			fprintf(stderr, "%s:%d: no OID found for %s or %s\n",
				mr->file, i, (char*)mr->rows[0], (char*)mr->rows[1]);
		    }
		  else if (!strcmp(mr->rows[i][0], "@label"))
		    sl->ic[nm].label = mr->rows[i][1];
		}
	    }
	}
      sl->iarray = r;
    }
}

void
sx_images_dump(struct sl_signlist *sl, FILE *fp)
{
  roco_write(fp, sl->iarray);
}
