#include <string.h>
#include <ctype128.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "charsets.h"
#include "atf.h"
#include "lang.h"

#define vwarning(a,b) fprintf(stderr,a,b)

#define xstrcpy(a,b) strcpy(((char *)(a)),((char*)(b)))
#define xstrlen(a)   strlen(((const char *)(a)))
#define xstrncmp(a,b,c) strncmp(((const char *)(a)),((const char *)(b)),((ssize_t)(c)))

static unsigned char *Unicode2UTF(unsigned short ch);
#include "uni2utf.h"

static char unitab[u_top][4];
static char *unidig[128];

void
atf2utf_init(void)
{
#define uinit(ch,u)  xstrcpy(unitab[ch],Unicode2UTF(u))
  uinit(u_sz,0x0161);
  uinit(u_SZ,0x0160);
  uinit(u_s0,0x2080); unidig['0'] = unitab[u_s0];
  uinit(u_s1,0x2081); unidig['1'] = unitab[u_s1];
  uinit(u_s2,0x2082); unidig['2'] = unitab[u_s2];
  uinit(u_s3,0x2083); unidig['3'] = unitab[u_s3];
  uinit(u_s4,0x2084); unidig['4'] = unitab[u_s4];
  uinit(u_s5,0x2085); unidig['5'] = unitab[u_s5];
  uinit(u_s6,0x2086); unidig['6'] = unitab[u_s6];
  uinit(u_s7,0x2087); unidig['7'] = unitab[u_s7];
  uinit(u_s8,0x2088); unidig['8'] = unitab[u_s8];
  uinit(u_s9,0x2089); unidig['9'] = unitab[u_s9];
  uinit(u_splus,0x208a);
  uinit(u_s_x,0x2093);
  uinit(u_eng,0x14b);
  uinit(u_ENG,0x14a);
  uinit(u_heth,0x1e2b);
  uinit(u_HETH,0x1e2a);
  uinit(u_sadhe,0x1e63);
  uinit(u_SADHE,0x1e62);
  uinit(u_sin,0x15b);
  uinit(u_SIN,0x15a);
  uinit(u_tet,0x1e6d);
  uinit(u_TET,0x1e6c);
  uinit(u_aleph,0x2be);
  uinit(u_amacr,0x0101);
  uinit(u_emacr,0x0113);
  uinit(u_imacr,0x012b);
  uinit(u_umacr,0x016b);
  uinit(u_Amacr,0x0100);
  uinit(u_Emacr,0x0112);
  uinit(u_Imacr,0x012a);
  uinit(u_Umacr,0x016a);
  uinit(u_acirc,0x00e2);
  uinit(u_ecirc,0x00ea);
  uinit(u_icirc,0x00ee);
  uinit(u_ucirc,0x00fb);
  uinit(u_Acirc,0x00c2);
  uinit(u_Ecirc,0x00ca);
  uinit(u_Icirc,0x00ce);
  uinit(u_Ucirc,0x00db);
  uinit(u_times,0x00d7);
#undef uinit
}

int
is_signlist(register const unsigned char *s)
{
  /* fprintf(stderr,"is_signlist: %s\n",s); */
  if (*s && s[1] && ((s[1] == 'S' && s[2] == 'L')
		     || (s[2] && s[2] == 'S' && s[3] == 'L')))
    return 1;
  switch (*s)
    {
    case 'A':
      return !xstrncmp(s,"ABZ",3);
    case 'B':
      return !xstrncmp(s,"BAU",3);
    case 'E':
      return !xstrncmp(s,"ELLES",5);
    case 'H':
      return !xstrncmp(s,"HZL",3);
    case 'K':
      return !xstrncmp(s,"KWU",3);
    case 'L':
      return !xstrncmp(s,"LAK",3);
    case 'M':
      return !s[1] || isdigit(s[1]) || !xstrncmp(s,"MEA",3) || !xstrncmp(s,"MZL",3);
    case 'N': /* cheat and use signlist mechanism to suppress
		 conversion of protocun N+digit convention */
      return !s[1] || isdigit(s[1]);
    case 'R':
      return !xstrncmp(s,"REC",3) || !xstrncmp(s,"RSP",3);
    case 'Z':
      return !xstrncmp(s,"ZATU",3);
    default:
      return 0;
    }
}

enum a2u_state { a2u_none , a2u_rx_curly , a2u_rx_square , a2u_in_cg };

unsigned char *
atf2utf(register const unsigned char *s, int rx)
{
  static unsigned char buf[1024];
  register unsigned char *t = buf;
  int slflag = 0;
  enum a2u_state state = a2u_none;

  if (isupper(s[1]) && strlen((char*)s) > 3)
    slflag = is_signlist(s);
  else if (*s == 'N' && isdigit(s[1]))
    slflag = 1;

  *buf = '\0';
  if (slflag)
    {
      while (t-buf<1020 && (isupper(*s) || isdigit(*s)))
	*t++ = *s++;
    }

  while (*s)
    {
      if (t-buf > 1020)
	abort();
      switch (*s)
	{
	case '{':
	  if (rx)
	    state = a2u_rx_curly;
	  *t++ = *s++;
	  break;
	case '}':
	  if (rx)
	    state = a2u_none;
	  *t++ = *s++;
	  break;
	case '[':
	  if (rx)
	    state = a2u_rx_square;
	  *t++ = *s++;
	  break;
	case ']':
	  if (rx)
	    state = a2u_none;
	  *t++ = *s++;
	  break;
	case '\\':
	  if (s[1] == '&')
	    {
	      ++s;
	    }
	  else
	    {
	      *t++ = '\\';
	      ++s;
	      if (*s)
		*t++ = *s++;
	    }
	  break;
	case '|':
	  if (state == a2u_in_cg)
	    state = a2u_none;
	  else
	    state = a2u_in_cg;
	  *t++ = *s++;
	  break;
	case '.':
	case '+':
	case '&':
	case '%':
	case '@':
	case ':':
	case '-':
	  *t++ = *s++;
	  if (isupper(s[1]) && strlen((char*)s)>3 && is_signlist(s))
	    {
	      while (t-buf<1020 && (isupper(*s) || isdigit(*s)))
		*t++ = *s++;
	    }
	  break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	  if (!rx || state != a2u_rx_curly)
	    {
	      xstrcpy(t,unidig[*s]);
	      t+=xstrlen(t);
	      ++s;
	      if (isdigit(*s))
		{
		  xstrcpy(t,unidig[*s]);
		  t+=xstrlen(t);
		  ++s;
		}
	    }
	  else
	    *t++ = *s++;
	  break;
#if 0
	case 'c':
	  xstrcpy(t,unitab[u_sz]);
	  t+=xstrlen(t);
	  ++s;
	  break;
	case 'C':
	  xstrcpy(t,unitab[u_SZ]);
	  t+=xstrlen(t);
	  ++s;
	  break;
#endif
	case 's':
	  switch (s[1])
	    {
	    case ',':
	    case 'y':
	      xstrcpy(t,unitab[u_sadhe]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case '\'':
	      xstrcpy(t,unitab[u_sin]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'z':
	      xstrcpy(t,unitab[u_sz]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    default:
	      *t++ = *s++;
	    }
	  break;
	case 'S':
	  switch (s[1])
	    {
	    case ',':
	    case 'y':
	      xstrcpy(t,unitab[u_SADHE]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case '\'':
	      xstrcpy(t,unitab[u_SIN]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'Z':
	      xstrcpy(t,unitab[u_SZ]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    default:
	      *t++ = *s++;
	    }
	  break;
	case 'x':
	  if (state == a2u_in_cg)
	    {
	      xstrcpy(t,unitab[u_times]);
	      t+=xstrlen(t);
	      ++s;
	    }
	  else
	    {
	      if (*buf && isalpha(s[-1]) 
		  && (!s[1] || s[1] == '-' || isspace(s[1]) || s[1] == '.'))
		{
		  xstrcpy(t,unitab[u_s_x]);
		  t+=xstrlen(t);
		  ++s;
		}
	      else
		*t++ = *s++;
	    }
	  break;
	case 'X':
	  if (*buf && isalpha(s[-1]))
	    {
	      xstrcpy(t,unitab[u_s_x]);
	      t+=xstrlen(t);
	      ++s;
	    }
	  else
	    *t++ = *s++;
	  break;
	case 'j':
	  xstrcpy(t,unitab[u_eng]);
	  t+=xstrlen(t);
	  ++s;
	  break;
	case 'J':
	  xstrcpy(t,unitab[u_ENG]);
	  t+=xstrlen(t);
	  ++s;
	  break;
	case 't':
	  if (s[1] == ',' || s[1] == 'y')
	    {
	      xstrcpy(t,unitab[u_tet]);
	      t+=xstrlen(t);
	      s+=2;
	    }
	  else
	    *t++ = *s++;
	  break;
	case 'T':
	  if (s[1] == ',' || s[1] == 'y')
	    {
	      xstrcpy(t,unitab[u_TET]);
	      t+=xstrlen(t);
	      s+=2;
	    }
	  else
	    *t++ = *s++;
	  break;
	case '\'':
	  xstrcpy(t,unitab[u_aleph]);
	  t+=xstrlen(t);
	  ++s;
	  break;
	case '`':
	  xstrcpy(t,unitab[u_ayin]);
	  t+=xstrlen(t);
	  ++s;
	  break;
#if 0
	  /* 2008-04-29: interpret bare h/H as HETH; use h, and H, for
	     /h/ */
	case 'h':
	  if (s[1] != ',')
	    {
	      xstrcpy(t,unitab[u_heth]);
	      t+=xstrlen(t);
	      s+=2;
	    }
	  else
	    *t++ = *s++;
	  break;
	case 'H':
	  if (s[1] != ',')
	    {
	      xstrcpy(t,unitab[u_HETH]);
	      t+=xstrlen(t);
	      s+=2;
	    }
	  else
	    *t++ = *s++;
	  break;
#else
	case 'h':
	  if (s[1] == ',')
	    {
	      xstrcpy(t,unitab[u_heth]);
	      t+=xstrlen(t);
	      s+=2;
	    }
	  else
	    *t++ = *s++;
	  break;
	case 'H':
	  if (s[1] == ',')
	    {
	      xstrcpy(t,unitab[u_HETH]);
	      t+=xstrlen(t);
	      s+=2;
	    }
	  else
	    *t++ = *s++;
	  break;
#endif
	case '^':
	  switch (s[1])
	    {
	    case 'a':
	      xstrcpy(t,unitab[u_acirc]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'e':
	      xstrcpy(t,unitab[u_ecirc]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'i':
	      xstrcpy(t,unitab[u_icirc]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'u':
	      xstrcpy(t,unitab[u_ucirc]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'A':
	      xstrcpy(t,unitab[u_Acirc]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'E':
	      xstrcpy(t,unitab[u_Ecirc]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'I':
	      xstrcpy(t,unitab[u_Icirc]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'U':
	      xstrcpy(t,unitab[u_Ucirc]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    default:
	      vwarning("%s: unknown accent sequence",s);
	      ++s;
	      break;
	    }
	  break;
	case '=':
	  switch (s[1])
	    {
	    case 'a':
	      xstrcpy(t,unitab[u_amacr]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'e':
	      xstrcpy(t,unitab[u_emacr]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'i':
	      xstrcpy(t,unitab[u_imacr]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'u':
	      xstrcpy(t,unitab[u_umacr]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'A':
	      xstrcpy(t,unitab[u_Amacr]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'E':
	      xstrcpy(t,unitab[u_Emacr]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'I':
	      xstrcpy(t,unitab[u_Imacr]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    case 'U':
	      xstrcpy(t,unitab[u_Umacr]);
	      t+=xstrlen(t);
	      s+=2;
	      break;
	    default:
	      vwarning("%s: unknown accent sequence",s);
	      ++s;
	      break;
	    }
	  break;
	default:
	  *t++ = *s++;
	  break;
	}
    }
  *t = '\0';
  return buf;
}
unsigned char *
utf8_times(void)
{
  return (unsigned char *)unitab[u_times];
}

#include "gatf2utf.c"

const unsigned char *
natf2utf(const char *str, const char *end, int rx, const char *file, int lnum)
{
  if (curr_lang && curr_lang->cset)
    return inctrie_map(curr_lang->cset->to_uni,str,end,rx,CHARSET_ERR_FUNC,
		       curr_lang->cset,file,lnum);
  else
    return (const unsigned char *)str;
}
