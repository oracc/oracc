#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <oraccsys.h>
#include <loadfile.h>
#include <hash.h>
#include <mesg.h>
#include <roco.h>
#include <oid.h>

#define ccp const char *

Oide *
oid_load_edits(const char *file)
{
  Oide *e = calloc(1, sizeof(Oide));
  e->r = roco_load(file);
  return e;
}

int
oid_parse_edits(Oide *e)
{
  int status = 0;
  size_t i;
  e->h = hash_create(1024);
  e->e = calloc(e->r->nlines, sizeof(struct oid_edit));
  e->ee = calloc(e->r->nlines, sizeof(struct oid_edit*));
  for (i = 0; i < e->r->nlines; ++i)
    {
      unsigned char **r = e->r->rows[i];
      e->ee[i] = &e->e[i];
      if (r[0])
	{
	  if (*r[0])
	    {
	      e->ee[i]->ymd = (ccp)r[0];
	      /* validate form here */
	    }
	  else
	    {
	      /* ymd is optional; we supply it if the field is empty */
	      char buf[9];
	      time_t t = time(NULL);
	      (void)strftime(buf, 8, "%Y%m%d", gmtime(&t));
	    }
	}
      if (r[1] && *r[1])
	{
	  e->ee[i]->oid = (ccp)r[1];
	  /* validate OID here--this means we need to track OID redirections */
	}
      else
	{
	  mesg_verr(mesg_mloc(e->r->file,i), "missing OID in edit line");
	  ++status;
	  continue;
	}
      if (r[2] && *r[2])
	{
	  e->ee[i]->key = r[2];
	  /* sanity check KEY here to ensure it matches OID */
	}
      else
	{
	  mesg_verr(mesg_mloc(e->r->file,i), "missing KEY in edit line");
	  ++status;
	  continue;
	}
      if (r[3] && *r[3])
	e->ee[i]->what = (enum oid_what)*r[3]; /* validation trap in switch below */
      else
	{
	  mesg_verr(mesg_mloc(e->r->file,i), "missing edit code in edit line");
	  ++status;
	  continue;
	}
      switch (e->ee[i]->what)
	{
	case ow_add:
	  /* nothing further */
	  break;
	case ow_delete:
	  if (r[4] && *r[4])
	    e->ee[i]->comment = r[4];
	  else
	    mesg_verr(mesg_mloc(e->r->file,i), "missing comment from 'delete'"), ++status;
	  break;
	case ow_merge:
	  if (r[4] && *r[4])
	    {
	      if (r[5] && *r[5])
		e->ee[i]->refs = oid_ok_pair((ccp)r[4], (ccp)r[5]);
	      else
		mesg_verr(mesg_mloc(e->r->file,i), "incomplete 'merge': missing KEY"), ++status;
	    }
	  else
	    mesg_verr(mesg_mloc(e->r->file,i), "incomplete 'merge': missing OID/KEY"), ++status;
	  break;
	case ow_rename:
	  if (r[4] && *r[4])
	    e->ee[i]->refs = oid_ok_pair((ccp)r[1], (ccp)r[4]);
	  else
	    mesg_verr(mesg_mloc(e->r->file,i), "incomplete 'rename': missing new KEY"), ++status;
	  break;
	case ow_split:
	  if (r[4] && *r[4])
	    {
	      int oref, kref;
	      for (oref=4,kref=5; r[oref] && *r[oref]; oref+=2,kref+=2)
		{
		  if (r[kref] && *r[kref])
		    {
		      struct oid_ok_pair *olp = oid_ok_pair((ccp)r[oref], (ccp)r[kref]);
		      if (e->ee[i]->refs)
			{
			  struct oid_ok_pair *last = oid_ok_pair_last(e->ee[i]->refs);
			  last->next = olp;
			}
		      else
			e->ee[i]->refs = olp;
		    }
		  else
		    mesg_verr(mesg_mloc(e->r->file,i), "incomplete OID/KEY pair in 'split': missing KEY"), ++status;
		}
	    }
	  else
	    mesg_verr(mesg_mloc(e->r->file,i), "incomplete 'split': missing OID/KEY pair"), ++status;
 	  break;
 	default:
	  mesg_verr(mesg_mloc(e->r->file, i), "invalid edit code %c", (char)e->ee[i]->what), ++status;
	  break;
	}
    }
  return status;
}
