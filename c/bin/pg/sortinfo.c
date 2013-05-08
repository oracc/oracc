#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "sortinfo.h"

#define MAX_PATH 1024

extern const char *prog;
extern const char *project;
extern int verbose;
extern struct sortinfo *sip;

static unsigned char *
findpool(unsigned char *tab,u4*nstring)
{
  const char *pool;
  const char *nstringp;
  pool = strchr((const char *)tab,'\0');
  if (pool)
    {
      while (pool > (const char *)tab && pool[-1] != '\n')
	--pool;
      if (pool == (const char *)tab)
	{
	  fprintf(stderr,"%s: corrupt sortinfo.tab: no newline before stringpool\n",pool);
	  exit(1);
	}
      nstringp = pool-1;
      while (nstringp > (const char *)tab && nstringp[-1] != '\n')
	--nstringp;
      if (nstringp > (const char *)tab)
	{
	  if (!strncmp(nstringp,"#nstring",8))
	    {
	      while (*nstringp && !isdigit(*nstringp))
		++nstringp;
	      *nstring = atoi(nstringp);
	      return (unsigned char *)pool;
	    }
	  else
	    {
	      fprintf(stderr,"%s: corrupt sortinfo.tab: no #string line\n",prog);
	      exit(1);
	    }
	}
      else
	{
	  fprintf(stderr,"%s: corrupt sortinfo.tab: pool not preceded by #string\n",prog);
	  exit(1);
	}
    }
  else
    {
      fprintf(stderr,"%s: corrupt sortinfo.tab: pool not found\n",prog);
      exit(1);
    }
  /* not reached */
}

struct sortinfo *
si_load_tab(unsigned char *tab, size_t tabsize)
{
  unsigned char *s = tab;
  u4*offset_tab;
  u4 nstring;

  sip = malloc(sizeof(struct sortinfo));
  s = sip->pool = findpool(tab, &nstring);
  if (sip->pool)
    {
      int top = tabsize - (s - tab), k, l;
      offset_tab = malloc(nstring*sizeof(u4));
      sip->pool_len = top;
      for (k = l = 0; k < top; ++k)
	{
	  offset_tab[l++] = k;
	  while (k < top && s[k])
	    ++k;
	}
    }
  s = tab;
  if (!strncmp((const char *)s,"#nfields",8))
    {
      s += 8;
      while (isspace(*s))
	++s;
      sip->nfields = atoi((const char *)s);
      while (isdigit(*s))
	++s;
      if ('\n' == *s)
	{
	  int nfields = 0;
	  ++s;
	  if (!strncmp((const char *)s,"#nmapentries",12))
	    {
	      int nmapents = 0;
	      s += 12;
	      while (isspace(*s))
		++s;
	      sip->nmapentries = atoi((const char *)s);
	      sip->fields = malloc(sip->nmapentries * sizeof(struct si_field));
	      while (*s && '\n' != *s)
		++s;
	      if (*s)
		++s;
	      while (!strncmp((const char *)s,"#field",6))
		{
		  unsigned char *n,*hr;
		  s += 6;
		  while (isspace(*s))
		    ++s;
		  n = s;
		  while (*s && *s != '=')
		    ++s;
		  if ('=' == *s)
		    {
		      *s++ = '\0';
		      hr = s;
		      while (*s && '\n' != *s)
			++s;
		      if (isspace(*s))
			{
			  *s++ = '\0';
			  while (isspace(*s))
			    ++s;
			}
		      else
			{
			  fprintf(stderr,"expected newline in #field\n");
			  goto badret;
			}
		      while (*n)
			{
			  unsigned char *endn = (unsigned char *)strchr((const char*)n,' ');
			  *sip->fields[nmapents].n = '\0';
			  if (endn)
			    {
			      strncat((char*)sip->fields[nmapents].n,(const char*)n,endn-n);
			      n = endn;
			      while (isspace(*n))
				++n;
			    }
			  else
			    strcpy((char*)sip->fields[nmapents].n,(const char*)n);
			  strcpy((char*)sip->fields[nmapents].hr,(const char*)hr);
			  sip->fields[nmapents].field_index = nfields;
			  ++nmapents;
			}
		    }
		  else
		    {
		      fprintf(stderr,"bad format in #fields\n");
		      goto badret;
		    }
		  ++nfields;
		}
	      if (!strncmp((const char *)s,"#nmembers",9))
		{
		  int i;
		  int p = 0;
		  s += 9;
		  while (isspace(*s))
		    ++s;
		  sip->nmember = atoi((const char *)s);
		  while (isdigit(*s))
		    ++s;
		  if ('\n' == *s)
		    {
		      ++s;
		      sip->idlist = malloc(sip->nmember * sizeof(u4));
		      sip->scodes = malloc(sip->nmember * (sip->nfields * sizeof(s4)));
		      sip->pindex = malloc(sip->nmember * (sip->nfields * sizeof(u4)));
		      for (i = 0; i < sip->nmember; ++i)
			{
			  int j;
			  if (*s != 'P' && *s != 'Q' && *s != 'X')
			    {
			      fprintf(stderr,"expected P or Q at start of line\n");
			      goto badret;
			    }
			  if (*s == 'P')
			    ++p;
			  ++s;
			  sip->idlist[i] = strtoul((const char *)s,NULL,10);
#if 0
			  if (s[-1] == 'Q')
			    setQ(sip->idlist[i]);
#endif
			  while (isdigit(*s))
			    ++s;
			  for (j = 0; j < sip->nfields; ++j)
			    {
			      int jindex = j+(i*sip->nfields);
			      sip->scodes[jindex] = strtol((const char *)s,NULL,10);
			      while (*s && '=' != *s)
				++s;
			      if ('=' == *s++)
				{
				  sip->pindex[jindex] 
				    = offset_tab[strtoul((const char *)s,NULL,10)];
				  while (*s && !isspace(*s))
				    ++s;
				  ++s;
				}
			      else
				{
				  fprintf(stderr,"bad format in values\n");
				  goto badret;
				}
			    }
			}
		      sip->qoffset = p;
		    }
		  return sip;
		}
	      else
		{
		  fprintf(stderr,"expected #nmembers\n");
		  goto badret;
		}
	    }
	}
    }
 badret:
  fprintf(stderr,"pgcsi: compilation failed\n");
  return NULL;
}

void
si_dump_csi(unsigned char *csiname, struct sortinfo *sip)
{
  int outfd;
  struct stat sbuf;
  struct sortinfo writable_si;
  if (stat((const char *)csiname,&sbuf) < 0)
    {
      outfd = open((const char *)csiname,O_CREAT|O_WRONLY);
      fchmod(outfd,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    }
  else
    outfd = open((const char *)csiname,O_TRUNC|O_WRONLY);
  if (outfd < 0)
    {
      fprintf(stderr,"%s: %s: can't open for write\n",prog,csiname);
      return;
    }
  writable_si = *sip;
  writable_si.fields = (void*)sizeof(struct sortinfo);
  writable_si.idlist = (void*)(((char*)writable_si.fields) 
			       + (sip->nmapentries * sizeof(struct si_field)));
  writable_si.scodes = (void*)(((char*)writable_si.idlist) 
			       + (sip->nmember * sizeof(u4)));
  writable_si.pindex = (void*)(((char*)writable_si.scodes) 
			       + ((sip->nfields * sip->nmember) * sizeof(s4)));
#if 0
  writable_si.poffsets = (void*)(((intptr_t)writable_si.pindex)
				 + ((sip->nfields * sip->nmember) * sizeof(s4)));
#endif
  write(outfd,&writable_si,sizeof(struct sortinfo));
  write(outfd,sip->fields,sip->nmapentries * sizeof(struct si_field));
  write(outfd,sip->idlist,sip->nmember * sizeof(u4));
  write(outfd,sip->scodes,(sip->nfields * sip->nmember) * sizeof(s4));
  write(outfd,sip->pindex,(sip->nfields * sip->nmember) * sizeof(u4));
#if 0
  write(outfd,sip->poffsets,sip->nstring * sizeof(u4));
#endif
  write(outfd,sip->pool,sip->pool_len);
  if (verbose)
    fprintf(stderr,"%s: compiled sortinfo written to %s\n",prog,csiname);
}

struct sortinfo *
si_load_csi()
{
  struct stat csistat;
  int csifd;
  void *csimap;
  char csiname[MAX_PATH];

  sprintf(csiname,"/usr/local/oracc/pub/%s/sortinfo.csi",project);

  if (stat(csiname,&csistat))
    {
      fprintf(stderr,"%s: %s: stat failed\n",prog,csiname);
      return NULL;
    }
  if ((csifd = open((const char *)csiname,O_RDONLY)) < 0)
    {
      fprintf(stderr,"%s: %s: open failed\n",prog,csiname);
      return NULL;
    }
  if (csistat.st_size < sizeof(struct sortinfo))
    {
      fprintf(stderr,"%s: %s: .csi file too small to be valid\n",prog,csiname);
      return NULL;
    }
  if (MAP_FAILED == (csimap = mmap(0, csistat.st_size, 
				   PROT_READ, MAP_SHARED, csifd, 0)))
    {
      fprintf(stderr,"%s: %s: mmap failed\n",prog,csiname);
      return NULL;
    }
  sip = malloc(sizeof(struct sortinfo));
  *sip = *(struct sortinfo*)csimap;
  sip->fields = (struct si_field*)((char*)csimap + (uintptr_t)sip->fields);
  sip->idlist = (u4*)((char*)csimap + (uintptr_t)sip->idlist);
  sip->scodes = (s4*)((char*)csimap + (uintptr_t)sip->scodes);
  sip->pindex = (u4*)((char*)csimap + (uintptr_t)sip->pindex);
  sip->pool = (unsigned char *)((char*)csimap + (csistat.st_size - sip->pool_len));
  return sip;
}

void
si_debug_dump_csi(struct sortinfo *dsi)
{
  int i, j;
  s4*scodes;
  for (i = 0; i < dsi->nmember; ++i)
    {
      printf("%c%06d",i>=sip->qoffset?'Q':'P',sip->idlist[i]);
      for (j = 0,scodes=&sip->scodes[i*sip->nfields]; j < dsi->nfields; ++j)
	{
	  printf("\t%7d",scodes[j]);
	}
      putchar ('\n');
    }
}

#if 0
void
si_dump_xml(const char *csiname)
{
  struct sortinfo *sp = si_load_csi(csiname);
  puts("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
  printf("<csi n=\"%s\">",csiname);
  printf("");
  puts("</csi>");
}
#endif
