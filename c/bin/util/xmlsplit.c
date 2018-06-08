#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype128.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <xsystem.h>

#define progress(a,b)

extern int options(int, char**,const char*);

const char *xmldecl = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
FILE *changefile = NULL;
FILE *filelist_fp = NULL;
FILE *outfile = NULL;
struct stat st;
int changed = 0;
int count = 0;
int decl = 0;
int xmlsplit_debug = 0;
int xmlsplit_filelist = 0;
#define PI_BUF_LEN 1023
char pi_buf[PI_BUF_LEN+1];
char atf_file_buf[1024];
char *changed_list = NULL;
char dest_file_buf[1024];
char ibuf[16384], obuf[16384];

char *cmpbuf = NULL;
int cmpbuf_alloced = 0;

char *xtf = NULL;
int xtf_len = 0;
int xtf_used = 0;

int output_ok = 0;
int update = 0;
int verbose = 0;

static void create_parent_dir(char *pathname);

static void
append_char(int ch)
{
  if (!output_ok)
    return;

  if (xtf_used == xtf_len)
    {
      xtf_len += 8196;
      xtf = realloc(xtf,xtf_len);
    }
  xtf[xtf_used++] = ch;
}

static void
append_zero()
{
  if (xtf_used == xtf_len)
    {
      xtf_len += 1;
      xtf = realloc(xtf,xtf_len);
    }
  xtf[xtf_used] = '\0';
}

static void
append_str(const char *s)
{
  while (*s)
    append_char(*s++);
}

char *
cmp_file(char *filename, size_t len)
{
  int fd;

  if (!filename || !*filename)
    return 0;

  if (len > cmpbuf_alloced)
    cmpbuf_alloced = len+1;
  cmpbuf = realloc(cmpbuf,cmpbuf_alloced);
  fd = open(filename,O_RDONLY);
  read(fd,cmpbuf,len);
  close(fd);
  cmpbuf[len] = '\0';
  return cmpbuf;
}

void
cmp_n_dump(char *filename)
{
  struct stat stat_old;
  int statret;

  if (!filename || !*filename)
    return;

  if (xmlsplit_filelist)
    fprintf(filelist_fp, "%s\n", filename);

  statret = stat(filename,&stat_old);
    
  if (!update || statret < 0)
    {
      int outfd;
      if (statret < 0)
	{
	  create_parent_dir(filename);
	  outfd = open(filename,O_CREAT|O_WRONLY,0644);
	  fchmod(outfd,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	}
      else
	outfd = open(filename,O_TRUNC|O_WRONLY);
      if (outfd < 0)
	{
	  fprintf(stderr,"xmlsplit: %s: can't open for write\n",filename);
	  return;
	}
      write(outfd,xtf,xtf_used);
      close(outfd);
      if (verbose)
	fprintf(stderr, "xmlsplit: created %s\n", filename);
      if (changefile)
	fprintf(changefile,"%s\n",filename);
      ++changed;
    }
  else
    {
      append_zero();
      if (stat_old.st_size != xtf_used
	  || strcmp(xtf,cmp_file(filename,stat_old.st_size)))
	{
	  int outfd = -1;
	  if (xmlsplit_debug)
	    {
	      char *n = malloc(strlen(filename)+5);
	      sprintf(n,"%s.old",filename);
	      xrename(filename, n);
	      free(n);
	    }
	  outfd = open(filename,O_WRONLY|O_TRUNC);
	  if (outfd < 0)
	    {
	      fprintf(stderr,"xmlsplit: %s: can't open for write\n",filename);
	      return;
	    }
	  write(outfd,xtf,xtf_used);
	  close(outfd);
	  if (verbose)
	    fprintf(stderr, "xmlsplit: updated %s\n", filename);
	  if (changefile)
	    fprintf(changefile,"%s\n",filename);
	  ++changed;
	}
      else
	{
	  if (verbose)
	    fprintf(stderr, "xmlsplit: skipped %s\n", filename);
	}
    }
}

static void
create_parent_dir(char *pathname)
{
  char *p = pathname+strlen(pathname);
  while (1)
    {
      while (p > pathname && p[-1] != '/')
	--p;
      if (p > pathname)
	{
	  *--p = '\0';
	  if (stat(pathname,&st))
	    {
	      create_parent_dir(pathname);
	      progress("creating %s\n",pathname);
	      mkdir(pathname,0775);
	      *p = '/';
	      return;
	    }
	  else
	    {	      
	      *p = '/';
	      return;
	    }
	}
      else
	return;
    }
}

void
init_file(char *filename)
{
  strcpy(dest_file_buf,filename);
  output_ok = 1;
  xtf_used = 0;
  if (decl)
    append_str(xmldecl);
  if (*atf_file_buf)
    {
      append_str("<?atf-file ");
      append_str(atf_file_buf);
      append_str("?>");
    }
  if (!(++count % 3000) && verbose)
    fputc('.',stderr);
  progress("%s\n",filename);
}

void
scan_pi()
{
  register char *pi = pi_buf;
  int c2;
  while (EOF != (c2 = getchar()))
    {
      if (c2 == '?')
	{
	  int c3 = getchar();
	  if (c3 == '>')
	    {
	      *pi = '\0';
	      return;
	    }
	  else
	    {
	      if (pi - pi_buf < (PI_BUF_LEN-1))
		{
		  *pi++ = '?';
		  *pi++ = c3;
		}
	    }
	}
      else
	{
	  if (pi-pi_buf < PI_BUF_LEN)
	    *pi++ = c2;
	}
    }
}

void
destfile()
{
  register char *filename = pi_buf+8;

  if (xtf_used)
    cmp_n_dump(dest_file_buf);

  while (isspace(*filename))
    ++filename;
  if (*filename)
    {
      if (!strncmp(filename, "#close",6))
	{
	  if (outfile)
	    fclose(outfile);
	  outfile = NULL;
	  output_ok = 0;
	}
      else
	init_file(filename);
    }
  else
    {
      xtf_used = 0;
    }
}

void
atf_file()
{
  register char *filename = pi_buf+8;
  while (isspace(*filename))
    ++filename;
  if (*filename)
    strcpy(atf_file_buf,filename);
}

int
main(int argc, char **argv)
{
  register int c;
  options(argc,argv,"c:dfuv");
  *dest_file_buf = '\0';
  if (changed_list)
    {
      changefile = fopen(changed_list,"w");
      if (NULL == changefile)
	{
	  fprintf(stderr,"xmlsplit: unable to open change list %s\n",changed_list);
	  exit(1);
	}
    }
  if (xmlsplit_filelist)
    {
      filelist_fp = fopen("01bld/destfiles.lst","w");
      if (NULL == filelist_fp)
	{
	  fprintf(stderr,"xmlsplit: unable to open change list 01bld/destfile.lst\n");
	  exit(1);
	}
    }
  *atf_file_buf = '\0';
  setvbuf(stdin,ibuf,_IOFBF,16384);
  while (EOF != (c = getchar()))
    {
      if ('<' == c)
	{
	  register int c2 = getchar();
	  if ('?' == c2)
	    {
	      scan_pi();
	      if (!strncmp(pi_buf,"destfile",8))
		{
		  destfile();
		  break;
		}
	      else if (!strncmp(pi_buf,"atf-file",8))
		atf_file();
	      /* else discard--we are not producing output yet */
	    }
	}
    }
  while (EOF != (c = getchar()))
    {
      if ('<' == c)
	{
	  register int c2 = getchar();
	  if ('?' == c2)
	    {
	      scan_pi();
	      if (!strncmp(pi_buf,"destfile",8))
		destfile();
	      else if (!strncmp(pi_buf,"atf-file",8))
		atf_file();
	      else
		{
		  append_str("<?");
		  append_str(pi_buf);
		  append_str("?>");
		}
	    }
	  else if (EOF == c2)
	    {
	      return 1;
	    }
	  else
	    {
	      append_char(c);
	      append_char(c2);
	    }
	}
      else
	{
	  append_char(c);
	}
    }
  if (verbose)
    fprintf(stderr,"xmlsplit: %d files processed; %d updated\n",count,changed);
  if (changefile)
    fclose(changefile);
  if (xmlsplit_filelist && filelist_fp)
    fclose(filelist_fp);
  return 0;
}

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'c':
      changed_list = arg;
      break;
    case 'd':
      xmlsplit_debug = 1;
      break;
    case 'f':
      xmlsplit_filelist = 1;
      break;
    case 'u':
      update = 1;
      break;
    case 'v':
      verbose = 1;
      break;
    default:
      return 1;
    }
  return 0;
}
const char *prog = "xmlsplit";
int major_version = 1, minor_version = 0;
const char *usage_string = "[-u -v]";
void
help ()
{
  printf("  -u update: output only if non-existent or different\n");
  printf("  -v verbose: say what's happening\n");
}
