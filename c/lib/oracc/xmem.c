/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: xmem.c,v 0.3 1997/09/08 14:50:06 sjt Exp $
*/

#include <psd_base.h>
#include <exitcode.h>
#if defined(_QC)
#include <process.h>
#endif

extern const char *prog;
static void memexit (void);

void *
xcalloc (size_t count, size_t bytes)
{
  void *tmp;

  if (!count || !bytes)
    return NULL;
  tmp = calloc (count, bytes);
  if (NULL == tmp)
    memexit ();
  return tmp;
}

void *
xmalloc (size_t bytes)
{
  void *tmp;
  if (!bytes)
    return NULL;
  tmp = malloc (bytes);
  if (NULL == tmp)
    memexit ();
  return tmp;
}

void *
xrealloc (void *ptr, size_t bytes)
{
  void *tmp;
  if (!bytes)
    return NULL;
  tmp = realloc (ptr, bytes);
  if (NULL == tmp)
    memexit ();
  return tmp;
}

void
xxfree (void **ptr)
{
  if (NULL != ptr && NULL != *ptr)
    {
      free (*ptr);
      *ptr = NULL;
    }
}

static void
memexit ()
{
  (void) fputs (prog, stderr);
  (void) fputs (": out of memory\n", stderr);
  exit (EXIT_OUT_OF_CORE);
}
