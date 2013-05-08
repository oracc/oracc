#include <ctype128.h>
#include <string.h>
#include "scan.h"

int
block_peek(unsigned char *p, scan_block_tester f)
{
  unsigned char save, *tokp,*endp;
  int ret = 0;
  tokp = scan_token(p,&endp,&save);
  if (f(tokp))
    ret = 1;
  *endp = save;
  return ret;
}

char *
scan_curly(char *p, char **endp)
{
  char *start;
  /*struct cdt_node *span;*/
  int nesting = 0;
  *p++ = '\0';
  start = p;
  while (*p)
    {
      if ('{' == *p && '\\' != p[-1])
	++nesting;
      else if ('}' == *p && '\\' != p[-1])
	{
	  if (nesting > 0)
	    --nesting;
	  else
	    {
	      *p++ = '\0';
	      break;
	    }
	}
      ++p;
    }
  if (endp)
    *endp = p;
  return start;
}

unsigned char *
scan_token(unsigned char *tokp, unsigned char **endtokp, unsigned char *savep)
{
  unsigned char *endp = ++tokp;
  if (isalpha(*endp))
    {
      while (isalpha(*endp))
	++endp;
    }
  else if (endp[1])
    ++endp;
#if 0
  else
    cdt_warning(x,x,"end-of-file found after '@'-sign");
#endif
  *savep = *endp;
  *endp = '\0';
  *endtokp = endp;
  return tokp;
}

void
scan_square(struct cdt_node *np, unsigned char *endtok, unsigned char **text_start, 
	    size_t *local_lnump)
{
  size_t local_lnum = *local_lnump;
  if (*endtok == '[')
    {
      unsigned char *endattr = (unsigned char*)strchr((const char *)endtok,']');
      unsigned char *newline;
      if (endattr)
	{
	  *endattr = '\0';
	  np->attr = ++endtok;	
	  if ((newline = (unsigned char *)strchr((const char *)endtok,'\n')))
	    {
	      while (newline)
		{
		  ++local_lnum;
		  if (newline[-1] == '\\')
		    newline[-1] = newline[0] = ' ';
		  else
		    cdt_warning(np->file,np->lnum,"newline in attribute group (missing ']'?)");
		  newline = (unsigned char *)strchr((const char *)newline,'\n');
		}
	    }
	  *text_start = endattr+1;
	}
      else
	{
	  cdt_warning(np->file,np->lnum,"missing ']' on attribute group");
	  /* error recovery: reset text_start to end of current line */
	  while (*endtok && '\n' != *endtok)
	    ++endtok;
	  *text_start = endtok;
	}
    }
  else
    *text_start = endtok;
  *local_lnump = local_lnum; 
}

void
scan_text(struct cdt_node *np, unsigned char *text_start, 
	  unsigned char **text_end, size_t *local_lnump,
	  scan_block_tester f)
{
  size_t local_lnum = *local_lnump;
  unsigned char *end, *trailing_white;
  switch (np->term)
    {
    case cdt_white: /* end at token */
      while (' ' == *text_start || '\t' == *text_start)
	++text_start;
      end = text_start;
      break;
    case cdt_line: /* end at first newline */
      while (' ' == *text_start || '\t' == *text_start)
	++text_start;
      if (!np->text)
	np->text = end = text_start;
      else
	end = text_start;
      while (*end && '\n' != *end)
	++end;
      trailing_white = end;
      while (trailing_white > text_start && isspace(trailing_white[-1]))
	--trailing_white;
      if (trailing_white != end)
	*trailing_white = '\0';
      if (*end)
	{
	  ++local_lnum;
	  *end++ = '\0';
	}
      break;
    case cdt_para: /* end at blank line or cdt block token */
      while (' ' == *text_start || '\t' == *text_start)
	++text_start;
      np->text = end = text_start;
      while (*end)
	{
	  if ('\n' == *end)
	    {
	      unsigned char *start_of_end = end;
	      ++end;
	      ++local_lnum;
	      while (' ' == *end || '\t' == *end)
		++end;
	      if ('\n' == *end) /* blank line */
		{
		  *start_of_end = '\0';
		  ++local_lnum;
		  ++end;
		  break;
		}
	      else if ('@' == *end && block_peek(end,f))
		{
		  ++local_lnum;
		  trailing_white = start_of_end;
		  while (trailing_white > text_start && isspace(trailing_white[-1]))
		    --trailing_white;
		  if (trailing_white != start_of_end)
		    *trailing_white = '\0';
		  else
		    *start_of_end = '\0'; /* zero out the newline */
		  goto ret;
		}
	    }
	  else
	    ++end;
	}
      break;
    case cdt_end:
      np->text = end = text_start;
      while (*end)
	{
	  if ('\n' == *end)
	    {
	      ++local_lnum;
	      if (!strncmp((const char *)end+1,"@end",4))
		{
		  unsigned char *start_of_end = end;
		  int namelen = strlen(np->name);
		  end += 5;
		  while (' ' == *end || '\t' == *end)
		    ++end;
		  if (!strncmp((const char *)end,np->name,namelen) 
		      && (isspace(end[namelen]) || !end[namelen]))
		    {
		      *start_of_end = '\0';
		      end += namelen;
		      while (isspace(*end))
			if ('\n' == *end++)
			  ++local_lnum;
		      goto ret;
		    }
		}
	    }
	  ++end;
	}
      break;
    default:
      break;
    }
 ret:
  *text_end = end;
  *local_lnump = local_lnum;
  return;
}

unsigned char *
skip_white(unsigned char *ftext, size_t *lnump)
{
  size_t local_lnum = *lnump;
  while (isspace(*ftext))
    if ('\n' == *ftext++)
      ++local_lnum;
  *lnump = local_lnum;
  return ftext;
}
