/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: alias.c,v 0.5 1997/09/08 14:50:18 sjt Exp $
*/
#include <psd_base.h>
#include <ctype128.h>
#include <messages.h>
#include <hash.h>
#include <dbi.h>
#include <misc.h>
#include "alias.h"
#include "se.h"
#include "selib.h"

#include <sys/unistd.h>

#ifndef strdup
extern char *strdup(const char *);
#endif

Hash_table *aliases;
Dbi_index *aliases_dip;

/* The aliases are kept in an alias file <index>/aliases,
   and should already be in UTF-8. The first grapheme of
   each series is the one used for indexing, and each series generally
   occupies one line. Indented lines can be used if a line isn't 
   enough.

   The aliases are preloaded by first initialising an index entry for
   the head grapheme, and then loading a special hash table mapping all
   remaining graphemes in the series to the index location of the 
   head grapheme */
/*#define PADDED_GRAPHEME_LEN 32*/
void
alias_index_aliases (const char *project,const char *index)
{
  const char *alias_fn, *alias_dir;
  FILE *alias_fp;
  Dbi_index *alias_dip;
  Uchar padded_grapheme[PADDED_GRAPHEME_LEN], head_grapheme[PADDED_GRAPHEME_LEN];
  Uchar *s, *t;
  int last_grapheme_len = PADDED_GRAPHEME_LEN;
  int line_num = 0;
  int hg_len = 0;

  alias_fn = strdup(se_file(project, index, "aliases"));
  alias_dir = se_dir(project,index);
  
  if (xaccess(alias_fn,W_OK|R_OK,0))
    {
      if (verbose)
	mwarning(NULL,"no aliases file %s; proceeding without aliasing",alias_fn);
      return;
    }

  alias_fp = xfopen (alias_fn, "r");
  alias_dip = dbi_create ("aliases", alias_dir, 1024, 1, DBI_BALK);
  dbi_set_cache (alias_dip, 32);
  while (NULL != (s = getline (alias_fp)))
    {
      if ('#' == *s)
	continue;

      if (!isspace(*s))
	{
	  memset (head_grapheme, '\0', last_grapheme_len);
	  t = head_grapheme;
	  while (*s && !isspace(*s))
	    {
	      if (t - head_grapheme == PADDED_GRAPHEME_LEN)
		{
		  *t = '\0';
		  error (ewfile(alias_fn, line_num), 
			 "%s...: grapheme too long (max %d chars)",
			 head_grapheme, PADDED_GRAPHEME_LEN-1);
		}
	      *t++ = *s++;
	    }
	  *t++ = '\0';
	  hg_len = t - head_grapheme;
	  while (*s && isspace(*s))
	    ++s;
	}
      else
	{
	  do
	    ++s;
	  while (*s && isspace(*s));
	}
      while (*s)
	{
	  t = padded_grapheme;
	  while (*s && !isspace(*s))
	    {
	      if (t - padded_grapheme == PADDED_GRAPHEME_LEN)
		{
		  *t = '\0';
		  error (ewfile(alias_fn, line_num), "%s...: grapheme too long (max %d chars)",padded_grapheme,PADDED_GRAPHEME_LEN-1);
		}
	      *t++ = *s++;
	    }
	  *t = '\0';
	  last_grapheme_len = t - padded_grapheme;
	  if (DBI_BALK == dbi_add (alias_dip, padded_grapheme, head_grapheme, hg_len))
	    mwarning (NULL, "duplicate grapheme alias %s -> %s", padded_grapheme, head_grapheme);
	  while (*s && isspace(*s))
	    ++s;
	}
    }
  dbi_flush (alias_dip);
  xfclose (alias_fn, alias_fp);
}

int
alias_check_date (const char *project, const char *index, Boolean bomb)
{
  const char *alias_src, *alias_idx;
  alias_src = strdup(se_file (project, index, "aliases"));
  alias_idx = strdup(se_file (project, index, "aliases.dbh"));
  
  if (fcheckf (alias_idx, alias_src))
    {
      if (bomb)
	error (NULL, "aliases index is out of date");
      else
	return 1;
    }
  return 0;
}
static int initialized = 0;

Dbi_index *
alias_init (const char *project, const char *index)
{
  const char *aname;

  if (NULL == index)
    index = "";

#if 0
  if (*index)
    error (NULL, "per-index aliases are not yet implemented");

  ++initialized;  
  if (initialized > 1)
    return;
#endif

  aname = se_file(project, index, "aliases.dbh");

  if (xaccess(aname,R_OK,0))
    return NULL;

  aname = se_dir(project, index);
  aliases_dip = dbi_open ("aliases", aname); /* don't care if it's NULL */
  return aliases_dip;
}

/* return the original grapheme unless it is an alias */
char *
alias_get_alias (char *orig_grapheme)
{
  if (aliases_dip)
    {
      dbi_find (aliases_dip, (unsigned char *)orig_grapheme);
      if (aliases_dip->nfound)
	return dbi_detach_data (aliases_dip, NULL);
    }
  return orig_grapheme;
}

void
alias_term ()
{
  if (initialized)
    {
      --initialized;
      if (!initialized)
	dbi_close (aliases_dip);
    }
}

void
alias_use(Dbi_index *aliases)
{
  aliases_dip = aliases;
}

/* suck the aliases db into an in-memory hash table for 
   faster access by sf_index; callers using this should
   check alias status with the macro `alias_fast_get_alias' */
void
alias_fast_init (const char *project, const char *index)
{
  Uchar *key;
  alias_init (project, index);
  if (aliases_dip)
    {
      aliases = hash_create (0);
      while (NULL != (key = dbi_each (aliases_dip)))
	hash_add (aliases, (unsigned char *)xstrdup ((const char*)key), 
		  dbi_detach_data (aliases_dip, NULL));
      alias_term ();
    }
  else
    aliases = NULL;
}

void
alias_fast_term ()
{
  hash_free (aliases, hash_xfree);
}
