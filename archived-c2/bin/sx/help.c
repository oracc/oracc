#include <stdlib.h>
#include <stdio.h>
#include <help.h>

FILE *hout = NULL;
static int help_max_opt_len = 1;
static int help_mode = 1; /* 1 = CRT ; 2 = XHTML DIV */
static int help_one_nl = 0;

void
help_longest_opt(const char *s)
{
  help_max_opt_len = strlen(s);
}

void
help_crt(const char *s, int prenl)
{
  if (prenl > 0)
    fputc('\n', hout);
  fprintf(hout, "%s\n", s);
  if (prenl > 1)
    {
      int i;
      for (i = 0; i < strlen(s); ++i)
	fputc(prenl == 2 ? '=' : '*', hout);
      fputc('\n', hout);
    }    
}

void
help_tab(void)
{
  fputc('\t', hout);
}

void
help_title(const char *s)
{
  hout = stdout;
  help_str(s, 3);
}

void
help_usage(const char *s)
{
  help_str(s, 1);
}

void
help_heading(const char *s)
{
  help_str(s, 2);
  help_one_nl = 1;
}

const char *
help_tabs(const char *o)
{
  int olen = 3 + strlen(o);
  int mlen = 3 + help_max_opt_len;

  /* We don't want too much tabbing, so if mlen is excessively long we
     still just do two tabs */
  if (mlen >= 8 && olen < 8)
    return "\t\t";
  else
    return "\t";
}

void
help_option(const char *opt, const char *s)
{
  if (help_mode == 1)
    {
      char *x = NULL;
      int i = 0;
      const char *tabs = help_tabs(opt);
      const char *hyph = (opt && *opt) ? "-" : "";
      i = snprintf(x, 0, "  %s%s%s%s", hyph, opt, tabs, s);
      x = malloc(++i);
      i = snprintf(x, i, "  %s%s%s%s", hyph, opt, tabs, s);
      help_str(x,help_one_nl--);
      free(x);
    }
  else
    {
      /* help_mode == 2 == XHTML will be tabular */
    }
}
