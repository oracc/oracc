#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

unsigned int lnum = 1;

/*
 * The utf8_check() function scans the '\0'-terminated string starting
 * at s. It returns a pointer to the first byte of the first malformed
 * or overlong UTF-8 sequence found, or NULL if the string contains
 * only correct UTF-8. It also spots UTF-8 sequences that could cause
 * trouble if converted to UTF-16, namely surrogate characters
 * (U+D800..U+DFFF) and non-Unicode positions (U+FFFE..U+FFFF). This
 * routine is very likely to find a malformed sequence if the input
 * uses any other encoding than UTF-8. It therefore can be used as a
 * very effective heuristic for distinguishing between UTF-8 and other
 * encodings.
 *
 * Markus Kuhn <http://www.cl.cam.ac.uk/~mgk25/> -- 2005-03-30
 */
unsigned char *utf8_check(unsigned char *s)
{
  while (*s) {
    if (*s < 0x80) {
      /* 0xxxxxxx */
      if ('\n' == *s || '\r' == *s)
	++lnum;
      s++;
    } else if ((s[0] & 0xe0) == 0xc0) {
      /* 110XXXXx 10xxxxxx */
      if ((s[1] & 0xc0) != 0x80 ||
	  (s[0] & 0xfe) == 0xc0)                        /* overlong? */
	return s;
      else
	s += 2;
    } else if ((s[0] & 0xf0) == 0xe0) {
      /* 1110XXXX 10Xxxxxx 10xxxxxx */
      if ((s[1] & 0xc0) != 0x80 ||
	  (s[2] & 0xc0) != 0x80 ||
	  (s[0] == 0xe0 && (s[1] & 0xe0) == 0x80) ||    /* overlong? */
	  (s[0] == 0xed && (s[1] & 0xe0) == 0xa0) ||    /* surrogate? */
	  (s[0] == 0xef && s[1] == 0xbf &&
	   (s[2] & 0xfe) == 0xbe))                      /* U+FFFE or U+FFFF? */
	return s;
      else
	s += 3;
    } else if ((s[0] & 0xf8) == 0xf0) {
      /* 11110XXX 10XXxxxx 10xxxxxx 10xxxxxx */
      if ((s[1] & 0xc0) != 0x80 ||
	  (s[2] & 0xc0) != 0x80 ||
	  (s[3] & 0xc0) != 0x80 ||
	  (s[0] == 0xf0 && (s[1] & 0xf0) == 0x80) ||    /* overlong? */
	  (s[0] == 0xf4 && s[1] > 0x8f) || s[0] > 0xf4) /* > U+10FFFF? */
	return s;
      else
	s += 4;
    } else
      return s;
  }

  return NULL;
}

int
main(int argc, char **argv)
{
  char*fname = argv[1];
  struct stat finfo;
  unsigned char *ftext;
  size_t fsize;
  int status = 0;
  int fdesc;

  if (fname == NULL)
    {
      fprintf(stderr,"utf8v: must give filename argument\n");
      exit(2);
    }
  if (-1 == stat(fname,&finfo))
    {
      fprintf(stderr,"utf8v: stat failed on %s\n",fname);
      exit(2);
    }
  if (!S_ISREG(finfo.st_mode))
    {
      fprintf(stderr,"utf8v: %s not a regular file\n",fname);
      exit(2);
    }
  fsize = finfo.st_size;
  if (NULL == (ftext = malloc(fsize+1)))
    {
      fprintf(stderr,"utf8v: %s: couldn't malloc %ld bytes\n",
	      fname,fsize);
      exit(2);
    }
  fdesc = open(fname,O_RDONLY);
  if (fdesc >= 0)
    {
      ssize_t ret = read(fdesc,ftext,fsize);
      if (ret == fsize)
	{
	  unsigned char *c = ftext;
	  unsigned char *e = ftext+fsize;
	  unsigned char *l = ftext;
	  ftext[fsize+1] = '\0';
	  while (c < e) {
	    unsigned char *u = utf8_check(c);
	    if (u != NULL)
	      {
		while (l < u)
		  {
		    if ('\n' == *ftext)
		      ++lnum;
		    ++l;
		  }
		fprintf(stderr,"%s:%d: bad character\n",fname,lnum);
		status = 1;
		c = u;
		while (*c && '\n' != *c && '\r' != *c)
		  ++c;
		if ('\0' == *c)
		  break;
		else
		  {
		    ++c;
		    ++lnum;
		    l = c;
		  }
	      }
	    else
	      c = e;
	    }
	}
      else
	{
	  fprintf(stderr,"utf8v: read %ld bytes failed\n",fsize);
	  exit(2);
	}
    }
  else
    {
      fprintf(stderr, "utf8v: %s: open failed\n", fname);
      exit(2);
    }
  return status;
}
