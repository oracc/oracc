/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: wildcard.c,v 0.3 1997/09/08 14:50:06 sjt Exp $
*/

/**This module defines a standard access point for programs which
 * want to receive a list of expanded files in the argument vector.
 * 
 * At present we use the EMX routine, _wildcard, but this should be 
 * fixed for portability.
 *
 */
#include <psd_base.h>
#include <options.h>
#include <wildcard.h>

#if defined (_QC) || defined (__TURBOC__)
#include <misc.h>
#endif

void
wildcard (int *pargc, char ***pargv)
{
#ifndef UNIX
  _wildcard(pargc, pargv);
  _internal_argc = *pargc;
  _internal_argv = *pargv;
#endif
}

/**The definition of _wildcard will inevitably involve some system
 * dependencies. This routine is modified from _wildcard in Eberhard
 * Mattes' emx-0.8f library. It's not as sophisticated as his, but
 * is simpler.
 */
#if defined (_QC) || defined (__TURBOC__)
#include <string.h>
#include <dos.h>
#include <xshells.h>

#define WPUT(x) \
    if (new_argc >= new_alloc) \
      { \
        new_alloc += 20; \
        new_argv = xrealloc (new_argv, new_alloc * sizeof (char *)); \
      } \
    new_argv[new_argc++] = x

void 
_wildcard (int *argcp, char ***argvp)
{
  int i, old_argc, new_argc, new_alloc;
  char **old_argv, **new_argv;
  char line[256], *p, *q;
  struct find_t find;

  old_argc = *argcp; old_argv = *argvp;
  for (i = 1; i < old_argc; ++i)
    if (old_argv[i] != NULL && strpbrk (old_argv[i], "?*") != NULL)
      break;
  if (i >= old_argc)
    return;                 /* do nothing */
  new_argv = NULL; new_alloc = 0; new_argc = 0;
  for (i = 0; i < old_argc; ++i)
    {
      if (i == 0 || old_argv[i] == NULL
          || strpbrk (old_argv[i], "?*") == NULL
          || _dos_findfirst (old_argv[i], _A_NORMAL, &find) != 0)
        {
          WPUT (old_argv[i]);
        }
      else
        {
          strcpy (line, old_argv[i]);
          p = q = line;
          while (*q != 0)
            {
              if (*q == ':' || *q == '\\' || *q == '/')
                p = q + 1;
              ++q;
            }
          do  {
            if (strcmp (find.name, ".") != 0 &&
                strcmp (find.name, "..") != 0)
              {
                strcpy (p, find.name);
                q = xstrdup (line);
                WPUT (q);
              }
          } while (_dos_findnext (&find) == 0);
        }
    }
  WPUT (NULL); 
  --new_argc;
  *argcp = new_argc; *argvp = new_argv;
  return;
}
#endif
