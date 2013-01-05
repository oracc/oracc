/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: getline.c,v 0.5 1997/09/08 14:50:04 sjt Exp $
*/

#include <psd_base.h>

static unsigned char *getline_buf = NULL;
static size_t getline_buf_alloced = 0, getline_buf_used;

unsigned char *
getline (FILE *fp)
{
  int ch;

  if (NULL == fp)
    {
      if (getline_buf_alloced)
	free (getline_buf);
      getline_buf_alloced = 0;
      return NULL;
    }

  if (feof(fp))
    return NULL;

  getline_buf_used = 0;
  for (;;) 
    {
      if (getline_buf_used >= getline_buf_alloced)
	{
	  getline_buf_alloced += 256;
	  getline_buf = realloc (getline_buf, getline_buf_alloced+1);
	}
      ch = fgetc(fp);
      if (EOF == ch)
	{
	  if (!getline_buf_used)
	    return NULL;
	  else
	    break;
	}
      else if ('\n' == ch)
	break;
      if ('\r' == ch)
	continue;
      getline_buf[getline_buf_used++] = (unsigned char) ch;
    }

  getline_buf[getline_buf_used] = '\0';
  return getline_buf;
}
