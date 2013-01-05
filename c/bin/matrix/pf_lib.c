#include <psd_base.h>
#include "pf_lib.h"

#define xmalloc  malloc
#define xrealloc realloc

Uchar *
print_escaped (Uchar *s)
{
  static int esc_buf_len;
  static Uchar *esc_buf = NULL;
  Uchar *dest;
  if (!esc_buf)
    {
      esc_buf_len = 128;
      esc_buf = xmalloc (esc_buf_len);
    }
  while (1+(2*strlen(s)) > esc_buf_len)
    {
      esc_buf_len *= 2;
      esc_buf = xrealloc (esc_buf, esc_buf_len);
    }
  dest = esc_buf;
  while (*s)
    {
      switch (*s)
	{
	case '\n':
	  *dest++ = '\\';
	  *dest++ = 'n';
	  ++s;
	  break;
	case '\t':
	  *dest++ = '\\';
	  *dest++ = 't';
	  ++s;
	  break;
	default:
	  *dest++ = *s++;
	  break;
	}
    }
  *dest = '\0';
  return esc_buf;
}

void
pf_print_string_and_newline (Uchar *sp)
{
  xfwrite (ipf->output_fn, FALSE, sp, 1, strlen(sp), ipf->output_fp);
  xfwrite (ipf->output_fn, FALSE, "\n", 1, 1, ipf->output_fp);
}
