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
      int nrows = oid_sort_keys->key_count+1;
      Roco *r = roco_create(nrows, ncols+1); /* we added one for the OID;
						this +1 is for the NULL
						term on each row of cols */
      Mloc *m;
      int nm;
      sl->iheaders = calloc(list_len(sl->images), sizeof(struct sx_iheader));
      r->rows[0][0] = (ucp)"OID";
      for (m = list_first(sl->images), nm=0; m; m = list_next(sl->images), ++nm)
	{
	  Roco *mr = roco_load(m->user, 0, NULL, NULL, NULL);
	  char buf[16];
	  sprintf(buf, "i%d", nm);
	  sl->iheaders[nm].r = mr;
	  sl->iheaders[nm].id = pool_copy(buf, sl->p);
	  if (mr)
	    {
	      int i;
	      for (i = 0; i < mr->nlines; ++i)
		{
		  switch (*mr->rows[i][0])
		    {
		    case '@':
		      if (!strcmp((ccp)mr->rows[i][0], "@label"))
			{
			  sl->iheaders[nm].label = (ccp)mr->rows[i][1];
			  r->rows[0][nm+1] = (ucp)sl->iheaders[nm].label;
			}
		      break;
		    case '#':
		      break;
		    default:
		      {
			int s = 0;
			if ((s = (uintptr_t)hash_find(oid_sort_keys, mr->rows[i][0])))
			  {
			    if (!r->rows[s][0])
			      r->rows[s][0] = mr->rows[i][0];
			    r->rows[s][nm+1] = mr->rows[i][1];
			  }
			else if ((s = (uintptr_t)hash_find(oid_sort_keys, mr->rows[i][1])))
			  {
			    if (!r->rows[s][0])
			      r->rows[s][0] = mr->rows[i][1];
			    r->rows[s][nm+1] = mr->rows[i][0];
			  }
			else
			  fprintf(stderr, "%s:%d: no OID found for %s or %s\n",
				  mr->file, i, (char*)mr->rows[0], (char*)mr->rows[1]);
		      }
		    }
		}
	    }
	}
      sl->iarray = r;
    }
}

void
sx_images_dump(FILE *fp, struct sl_signlist *sl)
{
  roco_write(fp, sl->iarray);
}
