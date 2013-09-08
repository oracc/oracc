#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#if 0
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif
#include "oraccd.h"

static void load_oraxxen(struct oraccd_config *cfg);
static const char *messages_dir(void);

void
configure(struct oraccd_config *cfg)
{
  cfg->oraccd_mode = ORACCD_NONE;
  cfg->max_concurrent_builds = 3;
  cfg->oraxxen = NULL;
  cfg->oraxxen_max = 0;

  cfg->lock_params.l_type = F_WRLCK; /* exclusive lock */
  cfg->lock_params.l_whence = SEEK_SET;
  cfg->lock_params.l_start = 0; /* on entire file */
  cfg->lock_params.l_len = 0;

  if (!access("/home/oracc",R_OK))
    cfg->oracc_home = "/home/oracc";
  else if (!access("/Users/oracc",R_OK))
    cfg->oracc_home = "/Users/oracc";
  else
    {
      fprintf(stderr, "oraccd: unable to set oracc_home. Stop.\n");
      exit(1);
    }
  gethostname(cfg->hostname, HOSTLEN);
  if (*cfg->hostname)
    {
      char *dot = strchr(cfg->hostname, '.');
      if (dot)
	*dot = '\0';
      printf("oraccd: hostname set to %s\n", cfg->hostname);
    }
  else
    {
      printf("oraccd: unable to set hostname.  Stop.\n");
      exit(1);
    }
 load_oraxxen(cfg);
 cfg->messages_dir = messages_dir();
}

static void
load_oraxxen(struct oraccd_config *cfg)
{
  const char *oraxxen_file = "lib/data/oraxxen";
  FILE *o = fopen(oraxxen_file, "r");
  if (o)
    {
      char *tmp = NULL, *ptr;
      long len = -1;
      int i;

      fseek(o, 0L, SEEK_END);
      len = ftell(o);
      fseek(o, 0L, SEEK_SET);
      tmp = malloc(len);
      fread(tmp, 1, len, o);
      fclose(o);

      for (i = 0, ptr = tmp; *ptr; ++ptr)
	if ('\n' == *ptr)
	  ++i;
      if ('\n' != ptr[-1])
	++i;
      cfg->oraxxen = malloc((1+i) * sizeof(char*));
      for (i = 0, ptr = tmp; *ptr; ++i)
	{
	  cfg->oraxxen[i] = ptr;
	  while (*ptr && '\n' != *ptr)
	    ++ptr;
	  if ('\n' == *ptr)
	    *ptr++ = '\0';
	}
      cfg->oraxxen[i] = NULL;
      for (i = 0; cfg->oraxxen[i]; ++i)
	{
	  ptr = cfg->oraxxen[i];
	  if (strlen(ptr) > cfg->oraxxen_max)
	    cfg->oraxxen_max = strlen(ptr);
	  while (*ptr)
	    {
	      if (!isalnum(*ptr))
		{
		  printf("oraccd: bad host in %s.  Stop", oraxxen_file);
		  exit(1);
		}
	    }
	}
    }
  else
    {
      printf("oraccd: unable to open %s: no server messages will be generated\n", oraxxen_file);
      cfg->oraxxen = NULL;
    }
}

static const char *
messages_dir(void)
{
  static const char *file = NULL;
  if (!file)
    {
      if (!access("/home/oracc/Dropbox/messages",R_OK))
	file = "/home/oracc/Dropbox/messages";
      else if (!access("/Users/oracc/Dropbox/messages",R_OK))
	file = "/Users/oracc/Dropbox/messages";
      else
	file = ""; /* opendir will fail on this */
    }
  return file;
}
