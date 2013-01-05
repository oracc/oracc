#include <ctype.h>
#include "warning.h"
#include "atf.h"
#include "charsets.h"
#define append(s) strcpy(buf+buf_used,s); buf_used+=strlen(s)
#define wcharcat(s) buf[buf_used++] = s
int chartrie_suppress_errors = 0;
static int
akk_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
        case 0x00000:
        case 0x00037:
        case 0x02085:
        case 0x0004b:
        case 0x00064:
        case 0x00059:
        case 0x00045:
        case 0x00079:
        case 0x01e6d:
        case 0x01e2b:
        case 0x000c1:
        case 0x02082:
        case 0x00067:
        case 0x00065:
        case 0x000cd:
        case 0x02084:
        case 0x0004a:
        case 0x000ec:
        case 0x000da:
        case 0x02081:
        case 0x00044:
        case 0x00071:
        case 0x00062:
        case 0x0007a:
        case 0x000ed:
        case 0x00077:
        case 0x000cc:
        case 0x01e6c:
        case 0x000c9:
        case 0x00051:
        case 0x0004d:
        case 0x0014a:
        case 0x0004c:
        case 0x000c8:
        case 0x00058:
        case 0x00050:
        case 0x000c0:
        case 0x01e2a:
        case 0x00160:
        case 0x0014b:
        case 0x01e62:
        case 0x02093:
        case 0x02088:
        case 0x00061:
        case 0x00054:
        case 0x00161:
        case 0x0004e:
        case 0x000fa:
        case 0x00032:
        case 0x0005a:
        case 0x00031:
        case 0x00075:
        case 0x0006b:
        case 0x000d9:
        case 0x00074:
        case 0x000f9:
        case 0x00057:
        case 0x000e9:
        case 0x00073:
        case 0x00042:
        case 0x00048:
        case 0x02086:
        case 0x00049:
        case 0x00047:
        case 0x02089:
        case 0x00055:
        case 0x00072:
        case 0x02087:
        case 0x00078:
        case 0x000e8:
        case 0x0015b:
        case 0x00068:
        case 0x002be:
        case 0x00030:
        case 0x02083:
        case 0x00066:
        case 0x01e63:
        case 0x00069:
        case 0x00036:
        case 0x00041:
        case 0x0006e:
        case 0x00033:
        case 0x00039:
        case 0x0006d:
        case 0x00038:
        case 0x0006c:
        case 0x00034:
        case 0x00070:
        case 0x000e0:
        case 0x0015a:
        case 0x00052:
        case 0x000e1:
        case 0x02080:
        case 0x00035:
        case 0x0006f:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%akk/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
akk_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
          append("S");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x000c1:
          append("Á");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x000cd:
          append("Í");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x000ec:
          append("ì");
          break;
        case 0x000da:
          append("Ú");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x000ed:
          append("í");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x000cc:
          append("Ì");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x000c9:
          append("É");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x000c8:
          append("È");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x000c0:
          append("À");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x000fa:
          append("ú");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x000d9:
          append("Ù");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x000f9:
          append("ù");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x000e9:
          append("é");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x000e8:
          append("è");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x000e0:
          append("à");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x000e1:
          append("á");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x0006f:
          append("o");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys akk_g_keys[] =
  {
    { "J"	, "\xc5\x8a" },
    { "T,"	, "\xe1\xb9\xac" },
    { "t,"	, "\xe1\xb9\xad" },
    { "SZ"	, "\xc5\xa0" },
    { "j"	, "\xc5\x8b" },
    { "s'"	, "\xc5\x9b" },
    { "S'"	, "\xc5\x9a" },
    { "sz"	, "\xc5\xa1" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "'"	, "\xca\xbe" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "S"	, "S" },
    { "7"	, "7" },
    { "K"	, "K" },
    { "d"	, "d" },
    { "Y"	, "Y" },
    { "E"	, "E" },
    { "y"	, "y" },
    { "g"	, "g" },
    { "e"	, "e" },
    { "D"	, "D" },
    { "q"	, "q" },
    { "b"	, "b" },
    { "z"	, "z" },
    { "w"	, "w" },
    { "Q"	, "Q" },
    { "M"	, "M" },
    { "L"	, "L" },
    { "X"	, "X" },
    { "P"	, "P" },
    { "a"	, "a" },
    { "T"	, "T" },
    { "N"	, "N" },
    { "2"	, "2" },
    { "Z"	, "Z" },
    { "1"	, "1" },
    { "u"	, "u" },
    { "k"	, "k" },
    { "t"	, "t" },
    { "W"	, "W" },
    { "s"	, "s" },
    { "B"	, "B" },
    { "H"	, "H" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "U"	, "U" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "h"	, "h" },
    { "0"	, "0" },
    { "f"	, "f" },
    { "i"	, "i" },
    { "6"	, "6" },
    { "A"	, "A" },
    { "n"	, "n" },
    { "3"	, "3" },
    { "9"	, "9" },
    { "m"	, "m" },
    { "8"	, "8" },
    { "l"	, "l" },
    { "4"	, "4" },
    { "p"	, "p" },
    { "R"	, "R" },
    { "5"	, "5" },
    { "o"	, "o" },
    { NULL, NULL },
  };
struct charset akk_g =
  {
    "akk", c_akk , "g", m_graphemic,
    akk_g_keys,
    0,
    akk_g_val,
    akk_g_asc,
    0,
  };

static int
akk_n_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
        case 0x00000:
        case 0x0004b:
        case 0x00064:
        case 0x00059:
        case 0x00045:
        case 0x00079:
        case 0x01e6d:
        case 0x01e2b:
        case 0x000ea:
        case 0x00067:
        case 0x000c2:
        case 0x000ce:
        case 0x00065:
        case 0x00044:
        case 0x00071:
        case 0x00062:
        case 0x0007a:
        case 0x00077:
        case 0x0016b:
        case 0x01e6c:
        case 0x0002d:
        case 0x00051:
        case 0x0004d:
        case 0x00043:
        case 0x0014a:
        case 0x0004c:
        case 0x00112:
        case 0x00058:
        case 0x00050:
        case 0x000ca:
        case 0x00100:
        case 0x0016a:
        case 0x01e2a:
        case 0x00027:
        case 0x00160:
        case 0x0014b:
        case 0x000e2:
        case 0x01e62:
        case 0x00054:
        case 0x00061:
        case 0x0004e:
        case 0x00161:
        case 0x0005a:
        case 0x00075:
        case 0x0006b:
        case 0x0003c:
        case 0x00074:
        case 0x00057:
        case 0x0003e:
        case 0x00042:
        case 0x00073:
        case 0x000fb:
        case 0x0012b:
        case 0x0003f:
        case 0x00048:
        case 0x00113:
        case 0x000ee:
        case 0x00063:
        case 0x0012a:
        case 0x00049:
        case 0x00047:
        case 0x00060:
        case 0x00055:
        case 0x00072:
        case 0x00101:
        case 0x000db:
        case 0x00078:
        case 0x0015b:
        case 0x002be:
        case 0x00068:
        case 0x01e63:
        case 0x00069:
        case 0x00041:
        case 0x0004f:
        case 0x0006e:
        case 0x0006d:
        case 0x0006c:
        case 0x00070:
        case 0x0015a:
        case 0x00052:
        case 0x0006f:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%akk/n",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
akk_n_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
          append("S");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x000ea:
          append("^e");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x000c2:
          append("^A");
          break;
        case 0x000ce:
          append("^I");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x0016b:
          append("=u");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x0002d:
          append("-");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x00112:
          append("=E");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x000ca:
          append("^E");
          break;
        case 0x00100:
          append("=A");
          break;
        case 0x0016a:
          append("=U");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x000e2:
          append("^a");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x0003c:
          append("<");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x0003e:
          append(">");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x000fb:
          append("^u");
          break;
        case 0x0012b:
          append("=i");
          break;
        case 0x0003f:
          append("?");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00113:
          append("=e");
          break;
        case 0x000ee:
          append("^i");
          break;
        case 0x00063:
          append("c");
          break;
        case 0x0012a:
          append("=I");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00060:
          append("`");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00101:
          append("=a");
          break;
        case 0x000db:
          append("^U");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x0004f:
          append("O");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0006f:
          append("o");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys akk_n_keys[] =
  {
    { "^U"	, "\xc3\x9b" },
    { "=U"	, "\xc5\xaa" },
    { "=A"	, "\xc4\x80" },
    { "=I"	, "\xc4\xaa" },
    { "T,"	, "\xe1\xb9\xac" },
    { "^i"	, "\xc3\xae" },
    { "^a"	, "\xc3\xa2" },
    { "SZ"	, "\xc5\xa0" },
    { "t,"	, "\xe1\xb9\xad" },
    { "j"	, "\xc5\x8b" },
    { "=a"	, "\xc4\x81" },
    { "^E"	, "\xc3\x8a" },
    { "=e"	, "\xc4\x93" },
    { "sz"	, "\xc5\xa1" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "=u"	, "\xc5\xab" },
    { "J"	, "\xc5\x8a" },
    { "=E"	, "\xc4\x92" },
    { "^I"	, "\xc3\x8e" },
    { "^A"	, "\xc3\x82" },
    { "^u"	, "\xc3\xbb" },
    { "s'"	, "\xc5\x9b" },
    { "S'"	, "\xc5\x9a" },
    { "=i"	, "\xc4\xab" },
    { "'"	, "\xca\xbe" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "^e"	, "\xc3\xaa" },
    { "S"	, "S" },
    { "K"	, "K" },
    { "d"	, "d" },
    { "Y"	, "Y" },
    { "E"	, "E" },
    { "y"	, "y" },
    { "g"	, "g" },
    { "e"	, "e" },
    { "D"	, "D" },
    { "q"	, "q" },
    { "b"	, "b" },
    { "z"	, "z" },
    { "w"	, "w" },
    { "-"	, "-" },
    { "Q"	, "Q" },
    { "M"	, "M" },
    { "C"	, "C" },
    { "L"	, "L" },
    { "X"	, "X" },
    { "P"	, "P" },
    { "T"	, "T" },
    { "a"	, "a" },
    { "N"	, "N" },
    { "Z"	, "Z" },
    { "u"	, "u" },
    { "k"	, "k" },
    { "<"	, "<" },
    { "t"	, "t" },
    { "W"	, "W" },
    { ">"	, ">" },
    { "B"	, "B" },
    { "s"	, "s" },
    { "?"	, "?" },
    { "H"	, "H" },
    { "c"	, "c" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "`"	, "`" },
    { "U"	, "U" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "h"	, "h" },
    { "i"	, "i" },
    { "A"	, "A" },
    { "O"	, "O" },
    { "n"	, "n" },
    { "m"	, "m" },
    { "l"	, "l" },
    { "p"	, "p" },
    { "R"	, "R" },
    { "o"	, "o" },
    { NULL, NULL },
  };
struct charset akk_n =
  {
    "akk", c_akk , "n", m_normalized,
    akk_n_keys,
    0,
    akk_n_val,
    akk_n_asc,
    0,
  };

static int
arc_a_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00000:
        case 0x00037:
        case 0x00064:
        case 0x01e6d:
        case 0x00079:
        case 0x00067:
        case 0x00071:
        case 0x00062:
        case 0x0007a:
        case 0x00077:
        case 0x0002d:
        case 0x0005b:
        case 0x00021:
        case 0x00023:
        case 0x0005f:
        case 0x0002b:
        case 0x01e25:
        case 0x00027:
        case 0x00161:
        case 0x00032:
        case 0x00031:
        case 0x0006b:
        case 0x0003c:
        case 0x00074:
        case 0x002bf:
        case 0x0003e:
        case 0x00073:
        case 0x0003f:
        case 0x00026:
        case 0x00072:
        case 0x0002a:
        case 0x0015b:
        case 0x00068:
        case 0x002be:
        case 0x00030:
        case 0x0002e:
        case 0x01e63:
        case 0x00036:
        case 0x0006e:
        case 0x00033:
        case 0x0005d:
        case 0x00039:
        case 0x0006d:
        case 0x00038:
        case 0x0006c:
        case 0x00034:
        case 0x00070:
        case 0x00035:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%arc/a",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
arc_a_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00037:
          append("7");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x01e6d:
          append("T");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x0002d:
          append("-");
          break;
        case 0x0005b:
          append("[");
          break;
        case 0x00021:
          append("!");
          break;
        case 0x00023:
          append("#");
          break;
        case 0x0005f:
          append("_");
          break;
        case 0x0002b:
          append("+");
          break;
        case 0x01e25:
          append("x");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x00161:
          append("$");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x0003c:
          append("<");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x002bf:
          append("(");
          break;
        case 0x0003e:
          append(">");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x0003f:
          append("?");
          break;
        case 0x00026:
          append("&");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x0002a:
          append("*");
          break;
        case 0x0015b:
          append("&");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x002be:
          append(")");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x0002e:
          append(".");
          break;
        case 0x01e63:
          append("c");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x0005d:
          append("]");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00035:
          append("5");
          break;
        default:
          break;
	}
    }
  return buf;
}

static wchar_t *
arc_u2u(wchar_t *w, ssize_t len)
{
  static wchar_t *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
          free(buf);
          buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced <= len)
    {
      while  (buf_alloced <= len)
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced * sizeof(wchar_t));
    }

  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00077:
          wcharcat(0x05D5);
          break;
        case 0x00072:
          wcharcat(0x05E8);
          break;
        case 0x00161:
          wcharcat(0xFB2A);
          break;
        case 0x00064:
          wcharcat(0x05D3);
          break;
        case 0x0015b:
          wcharcat(0xFB2B);
          break;
        case 0x00079:
          wcharcat(0x05D9);
          break;
        case 0x01e6d:
          wcharcat(0x05D8);
          break;
        case 0x0006b:
          wcharcat(0x05DB);
          break;
        case 0x002be:
          wcharcat(0x05D0);
          break;
        case 0x00068:
          wcharcat(0x05D4);
          break;
        case 0x00067:
          wcharcat(0x05D2);
          break;
        case 0x00074:
          wcharcat(0x05E9);
          break;
        case 0x01e63:
          wcharcat(0x05E6);
          break;
        case 0x0006e:
          wcharcat(0x05E0);
          break;
        case 0x002bf:
          wcharcat(0x05E2);
          break;
        case 0x0006d:
          wcharcat(0x05DE);
          break;
        case 0x00073:
          wcharcat(0x05E1);
          break;
        case 0x01e25:
          wcharcat(0x05D7);
          break;
        case 0x0006c:
          wcharcat(0x05DC);
          break;
        case 0x00070:
          wcharcat(0x05E4);
          break;
        case 0x00071:
          wcharcat(0x05E7);
          break;
        case 0x00062:
          wcharcat(0x05D1);
          break;
        case 0x0007a:
          wcharcat(0x05D6);
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys arc_a_keys[] =
  {
    { "$"	, "\xc5\xa1" },
    { "w"	, "\x77" },
    { "r"	, "\x72" },
    { "T"	, "\xe1\xb9\xad" },
    { "x"	, "\xe1\xb8\xa5" },
    { "d"	, "\x64" },
    { "y"	, "\x79" },
    { "k"	, "\x6b" },
    { "h"	, "\x68" },
    { "g"	, "\x67" },
    { "p'"	, "\x70" },
    { "t"	, "\x74" },
    { "n"	, "\x6e" },
    { "m'"	, "\x6d" },
    { "m"	, "\x6d" },
    { "s"	, "\x73" },
    { "l"	, "\x6c" },
    { "c"	, "\xe1\xb9\xa3" },
    { "p"	, "\x70" },
    { "k'"	, "\x6b" },
    { ")"	, "\xca\xbe" },
    { "&"	, "\xc5\x9b" },
    { "q"	, "\x71" },
    { "b"	, "\x62" },
    { "c'"	, "\xe1\xb9\xa3" },
    { "n'"	, "\x6e" },
    { "("	, "\xca\xbf" },
    { "z"	, "\x7a" },
    { ""	, "" },
    { "7"	, "7" },
    { "-"	, "-" },
    { "["	, "[" },
    { "!"	, "!" },
    { "#"	, "#" },
    { "_"	, "_" },
    { "+"	, "+" },
    { "'"	, "'" },
    { "2"	, "2" },
    { "1"	, "1" },
    { "<"	, "<" },
    { ">"	, ">" },
    { "?"	, "?" },
    { "*"	, "*" },
    { "0"	, "0" },
    { "."	, "." },
    { "6"	, "6" },
    { "3"	, "3" },
    { "]"	, "]" },
    { "9"	, "9" },
    { "8"	, "8" },
    { "4"	, "4" },
    { "5"	, "5" },
    { NULL, NULL },
  };
struct charset arc_a =
  {
    "arc", c_arc , "a", m_alphabetic,
    arc_a_keys,
    0,
    arc_a_val,
    arc_a_asc,
    arc_u2u,
  };

static int
elx_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
        case 0x00000:
        case 0x02085:
        case 0x0004b:
        case 0x00064:
        case 0x00045:
        case 0x01e2b:
        case 0x02082:
        case 0x00067:
        case 0x00065:
        case 0x02084:
        case 0x02081:
        case 0x00044:
        case 0x00062:
        case 0x0007a:
        case 0x0004d:
        case 0x0004c:
        case 0x00058:
        case 0x00050:
        case 0x01e2a:
        case 0x00160:
        case 0x02093:
        case 0x02088:
        case 0x00054:
        case 0x00061:
        case 0x00161:
        case 0x0004e:
        case 0x0005a:
        case 0x00075:
        case 0x0006b:
        case 0x00074:
        case 0x00073:
        case 0x00042:
        case 0x00048:
        case 0x02086:
        case 0x00049:
        case 0x00047:
        case 0x02089:
        case 0x00055:
        case 0x00072:
        case 0x02087:
        case 0x00078:
        case 0x00068:
        case 0x02083:
        case 0x00069:
        case 0x00041:
        case 0x0006e:
        case 0x0006d:
        case 0x0006c:
        case 0x00070:
        case 0x00052:
        case 0x02080:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%elx/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
elx_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
          append("S");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x02080:
          append("₀");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys elx_g_keys[] =
  {
    { "SZ"	, "\xc5\xa0" },
    { "sz"	, "\xc5\xa1" },
    { "S"	, "S" },
    { "K"	, "K" },
    { "d"	, "d" },
    { "E"	, "E" },
    { "g"	, "g" },
    { "e"	, "e" },
    { "D"	, "D" },
    { "b"	, "b" },
    { "z"	, "z" },
    { "M"	, "M" },
    { "L"	, "L" },
    { "X"	, "X" },
    { "P"	, "P" },
    { "T"	, "T" },
    { "a"	, "a" },
    { "N"	, "N" },
    { "Z"	, "Z" },
    { "u"	, "u" },
    { "k"	, "k" },
    { "t"	, "t" },
    { "s"	, "s" },
    { "B"	, "B" },
    { "H"	, "H" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "U"	, "U" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "h"	, "h" },
    { "i"	, "i" },
    { "A"	, "A" },
    { "n"	, "n" },
    { "m"	, "m" },
    { "l"	, "l" },
    { "p"	, "p" },
    { "R"	, "R" },
    { NULL, NULL },
  };
struct charset elx_g =
  {
    "elx", c_elx , "g", m_graphemic,
    elx_g_keys,
    0,
    elx_g_val,
    elx_g_asc,
    0,
  };

static int
grc_a_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x003bf:
        case 0x00000:
        case 0x003b4:
        case 0x003c9:
        case 0x003c2:
        case 0x003c3:
        case 0x003b6:
        case 0x003b8:
        case 0x003b9:
        case 0x003b5:
        case 0x003c1:
        case 0x003bb:
        case 0x003c4:
        case 0x003bd:
        case 0x003be:
        case 0x003b2:
        case 0x003b3:
        case 0x003b1:
        case 0x003bc:
        case 0x003c6:
        case 0x003c0:
        case 0x003c5:
        case 0x003c7:
        case 0x003ba:
        case 0x003c8:
        case 0x003b7:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%grc/a",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
grc_a_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x003bf:
          append("o");
          break;
        case 0x003b4:
          append("d");
          break;
        case 0x003c9:
          append("o'");
          break;
        case 0x003c2:
          append("s'");
          break;
        case 0x003c3:
          append("s");
          break;
        case 0x003b6:
          append("z");
          break;
        case 0x003b8:
          append("th");
          break;
        case 0x003b9:
          append("i");
          break;
        case 0x003b5:
          append("e");
          break;
        case 0x003c1:
          append("r");
          break;
        case 0x003bb:
          append("l");
          break;
        case 0x003c4:
          append("t");
          break;
        case 0x003bd:
          append("n");
          break;
        case 0x003be:
          append("x");
          break;
        case 0x003b2:
          append("b");
          break;
        case 0x003b3:
          append("g");
          break;
        case 0x003b1:
          append("a");
          break;
        case 0x003bc:
          append("m");
          break;
        case 0x003c6:
          append("ph");
          break;
        case 0x003c0:
          append("p");
          break;
        case 0x003c5:
          append("u");
          break;
        case 0x003c7:
          append("ch");
          break;
        case 0x003ba:
          append("k");
          break;
        case 0x003c8:
          append("p'");
          break;
        case 0x003b7:
          append("e'");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys grc_a_keys[] =
  {
    { "r"	, "\xcf\x81" },
    { "e'"	, "\xce\xb7" },
    { "a"	, "\xce\xb1" },
    { "o'"	, "\xcf\x89" },
    { "ph"	, "\xcf\x86" },
    { "x"	, "\xce\xbe" },
    { "d"	, "\xce\xb4" },
    { "u"	, "\xcf\x85" },
    { "k"	, "\xce\xba" },
    { "ch"	, "\xcf\x87" },
    { "g"	, "\xce\xb3" },
    { "p'"	, "\xcf\x88" },
    { "t"	, "\xcf\x84" },
    { "i"	, "\xce\xb9" },
    { "th"	, "\xce\xb8" },
    { "e"	, "\xce\xb5" },
    { "n"	, "\xce\xbd" },
    { "m"	, "\xce\xbc" },
    { "s"	, "\xcf\x83" },
    { "l"	, "\xce\xbb" },
    { "s'"	, "\xcf\x82" },
    { "p"	, "\xcf\x80" },
    { "b"	, "\xce\xb2" },
    { "z"	, "\xce\xb6" },
    { "o"	, "\xce\xbf" },
    { ""	, "" },
    { NULL, NULL },
  };
struct charset grc_a =
  {
    "grc", c_grc , "a", m_alphabetic,
    grc_a_keys,
    0,
    grc_a_val,
    grc_a_asc,
    0,
  };

static int
hit_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
        case 0x00000:
        case 0x00037:
        case 0x02085:
        case 0x0004b:
        case 0x00064:
        case 0x00059:
        case 0x00045:
        case 0x00079:
        case 0x01e6d:
        case 0x01e2b:
        case 0x000c1:
        case 0x02082:
        case 0x00067:
        case 0x00065:
        case 0x000cd:
        case 0x02084:
        case 0x0004a:
        case 0x000ec:
        case 0x000da:
        case 0x02081:
        case 0x00044:
        case 0x00071:
        case 0x00062:
        case 0x0007a:
        case 0x000ed:
        case 0x00077:
        case 0x000cc:
        case 0x01e6c:
        case 0x000c9:
        case 0x00051:
        case 0x0004d:
        case 0x0004c:
        case 0x000c8:
        case 0x00058:
        case 0x00050:
        case 0x000c0:
        case 0x01e2a:
        case 0x00160:
        case 0x01e62:
        case 0x02093:
        case 0x02088:
        case 0x00054:
        case 0x00061:
        case 0x0004e:
        case 0x00161:
        case 0x000fa:
        case 0x00032:
        case 0x0005a:
        case 0x00031:
        case 0x00075:
        case 0x0006b:
        case 0x000d9:
        case 0x00074:
        case 0x000f9:
        case 0x00057:
        case 0x000e9:
        case 0x00042:
        case 0x00073:
        case 0x00048:
        case 0x02086:
        case 0x00049:
        case 0x00047:
        case 0x02089:
        case 0x00055:
        case 0x00072:
        case 0x02087:
        case 0x00078:
        case 0x000e8:
        case 0x0015b:
        case 0x002be:
        case 0x00068:
        case 0x00030:
        case 0x02083:
        case 0x00066:
        case 0x01e63:
        case 0x00069:
        case 0x00036:
        case 0x00041:
        case 0x0006e:
        case 0x00033:
        case 0x00039:
        case 0x0006d:
        case 0x00038:
        case 0x0006c:
        case 0x00034:
        case 0x00070:
        case 0x000e0:
        case 0x0015a:
        case 0x00052:
        case 0x000e1:
        case 0x02080:
        case 0x00035:
        case 0x0006f:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%hit/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
hit_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
          append("S");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x000c1:
          append("Á");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x000cd:
          append("Í");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x000ec:
          append("ì");
          break;
        case 0x000da:
          append("Ú");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x000ed:
          append("í");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x000cc:
          append("Ì");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x000c9:
          append("É");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x000c8:
          append("È");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x000c0:
          append("À");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x000fa:
          append("ú");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x000d9:
          append("Ù");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x000f9:
          append("ù");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x000e9:
          append("é");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x000e8:
          append("è");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x000e0:
          append("à");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x000e1:
          append("á");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x0006f:
          append("o");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys hit_g_keys[] =
  {
    { "T,"	, "\xe1\xb9\xac" },
    { "t,"	, "\xe1\xb9\xad" },
    { "SZ"	, "\xc5\xa0" },
    { "sz"	, "\xc5\xa1" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "S'"	, "\xc5\x9a" },
    { "s'"	, "\xc5\x9b" },
    { "'"	, "\xca\xbe" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "S"	, "S" },
    { "7"	, "7" },
    { "K"	, "K" },
    { "d"	, "d" },
    { "Y"	, "Y" },
    { "E"	, "E" },
    { "y"	, "y" },
    { "g"	, "g" },
    { "e"	, "e" },
    { "J"	, "J" },
    { "D"	, "D" },
    { "q"	, "q" },
    { "b"	, "b" },
    { "z"	, "z" },
    { "w"	, "w" },
    { "Q"	, "Q" },
    { "M"	, "M" },
    { "L"	, "L" },
    { "X"	, "X" },
    { "P"	, "P" },
    { "T"	, "T" },
    { "a"	, "a" },
    { "N"	, "N" },
    { "2"	, "2" },
    { "Z"	, "Z" },
    { "1"	, "1" },
    { "u"	, "u" },
    { "k"	, "k" },
    { "t"	, "t" },
    { "W"	, "W" },
    { "B"	, "B" },
    { "s"	, "s" },
    { "H"	, "H" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "U"	, "U" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "h"	, "h" },
    { "0"	, "0" },
    { "f"	, "f" },
    { "i"	, "i" },
    { "6"	, "6" },
    { "A"	, "A" },
    { "n"	, "n" },
    { "3"	, "3" },
    { "9"	, "9" },
    { "m"	, "m" },
    { "8"	, "8" },
    { "l"	, "l" },
    { "4"	, "4" },
    { "p"	, "p" },
    { "R"	, "R" },
    { "5"	, "5" },
    { "o"	, "o" },
    { NULL, NULL },
  };
struct charset hit_g =
  {
    "hit", c_hit , "g", m_graphemic,
    hit_g_keys,
    0,
    hit_g_val,
    hit_g_asc,
    0,
  };

static int
peo_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00000:
        case 0x00064:
        case 0x00079:
        case 0x02082:
        case 0x00067:
        case 0x00044:
        case 0x00062:
        case 0x0007a:
        case 0x0010d:
        case 0x0004d:
        case 0x000e7:
        case 0x003b8:
        case 0x00058:
        case 0x00160:
        case 0x00061:
        case 0x00161:
        case 0x0006a:
        case 0x00075:
        case 0x0006b:
        case 0x00074:
        case 0x00076:
        case 0x00042:
        case 0x00073:
        case 0x00048:
        case 0x00047:
        case 0x00055:
        case 0x00072:
        case 0x00078:
        case 0x00068:
        case 0x00066:
        case 0x00069:
        case 0x00041:
        case 0x0006e:
        case 0x0006d:
        case 0x0006c:
        case 0x00070:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%peo/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
peo_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00064:
          append("d");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x02082:
          append("2");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x0010d:
          append("c'");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x000e7:
          append("c,");
          break;
        case 0x003b8:
          append("t'");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x00161:
          append("$");
          break;
        case 0x0006a:
          append("j");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x00076:
          append("v");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00070:
          append("p");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys peo_g_keys[] =
  {
    { "$"	, "\xc5\xa1" },
    { "2"	, "\xe2\x82\x82" },
    { "t'"	, "\xce\xb8" },
    { "c'"	, "\xc4\x8d" },
    { "c,"	, "\xc3\xa7" },
    { ""	, "" },
    { "d"	, "d" },
    { "y"	, "y" },
    { "g"	, "g" },
    { "D"	, "D" },
    { "b"	, "b" },
    { "z"	, "z" },
    { "M"	, "M" },
    { "X"	, "X" },
    { "a"	, "a" },
    { "j"	, "j" },
    { "u"	, "u" },
    { "k"	, "k" },
    { "t"	, "t" },
    { "v"	, "v" },
    { "B"	, "B" },
    { "s"	, "s" },
    { "H"	, "H" },
    { "G"	, "G" },
    { "U"	, "U" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "h"	, "h" },
    { "f"	, "f" },
    { "i"	, "i" },
    { "A"	, "A" },
    { "n"	, "n" },
    { "m"	, "m" },
    { "l"	, "l" },
    { "p"	, "p" },
    { NULL, NULL },
  };
struct charset peo_g =
  {
    "peo", c_peo , "g", m_graphemic,
    peo_g_keys,
    0,
    peo_g_val,
    peo_g_asc,
    0,
  };

static int
peo_n_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
        case 0x00000:
        case 0x01e5a:
        case 0x0004b:
        case 0x00064:
        case 0x00059:
        case 0x00079:
        case 0x00067:
        case 0x0004a:
        case 0x00044:
        case 0x00062:
        case 0x0007a:
        case 0x0016b:
        case 0x00398:
        case 0x000c7:
        case 0x0010d:
        case 0x0004d:
        case 0x00112:
        case 0x0004c:
        case 0x000e7:
        case 0x003b8:
        case 0x00058:
        case 0x00050:
        case 0x00100:
        case 0x0016a:
        case 0x00160:
        case 0x00054:
        case 0x00061:
        case 0x0004e:
        case 0x00161:
        case 0x0006a:
        case 0x0005a:
        case 0x00075:
        case 0x01e5b:
        case 0x0006b:
        case 0x00074:
        case 0x00076:
        case 0x00042:
        case 0x00073:
        case 0x0012b:
        case 0x00048:
        case 0x00113:
        case 0x0012a:
        case 0x00049:
        case 0x00047:
        case 0x00055:
        case 0x00046:
        case 0x00101:
        case 0x00072:
        case 0x0010c:
        case 0x00078:
        case 0x00056:
        case 0x00068:
        case 0x00066:
        case 0x00069:
        case 0x00041:
        case 0x0006e:
        case 0x0006d:
        case 0x0006c:
        case 0x00070:
        case 0x00052:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%peo/n",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
peo_n_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
          append("S");
          break;
        case 0x01e5a:
          append("R,");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x0016b:
          append("=u");
          break;
        case 0x00398:
          append("T'");
          break;
        case 0x000c7:
          append("C,");
          break;
        case 0x0010d:
          append("c'");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00112:
          append("=E");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x000e7:
          append("c,");
          break;
        case 0x003b8:
          append("t'");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00100:
          append("=A");
          break;
        case 0x0016a:
          append("=U");
          break;
        case 0x00160:
          append("%");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00161:
          append("$");
          break;
        case 0x0006a:
          append("j");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x01e5b:
          append("r,");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x00076:
          append("v");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x0012b:
          append("=i");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00113:
          append("=e");
          break;
        case 0x0012a:
          append("=I");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00046:
          append("F");
          break;
        case 0x00101:
          append("=a");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x0010c:
          append("C'");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00056:
          append("V");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00052:
          append("R");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys peo_n_keys[] =
  {
    { "$"	, "\xc5\xa1" },
    { "=U"	, "\xc5\xaa" },
    { "=A"	, "\xc4\x80" },
    { "=I"	, "\xc4\xaa" },
    { "t'"	, "\xce\xb8" },
    { "=a"	, "\xc4\x81" },
    { "=e"	, "\xc4\x93" },
    { "C,"	, "\xc3\x87" },
    { "=u"	, "\xc5\xab" },
    { "=E"	, "\xc4\x92" },
    { "%"	, "\xc5\xa0" },
    { "T'"	, "\xce\x98" },
    { "R,"	, "\xe1\xb9\x9a" },
    { "r,"	, "\xe1\xb9\x9b" },
    { "=i"	, "\xc4\xab" },
    { "c'"	, "\xc4\x8d" },
    { "c,"	, "\xc3\xa7" },
    { "C'"	, "\xc4\x8c" },
    { "S"	, "S" },
    { ""	, "" },
    { "K"	, "K" },
    { "d"	, "d" },
    { "Y"	, "Y" },
    { "y"	, "y" },
    { "g"	, "g" },
    { "J"	, "J" },
    { "D"	, "D" },
    { "b"	, "b" },
    { "z"	, "z" },
    { "M"	, "M" },
    { "L"	, "L" },
    { "X"	, "X" },
    { "P"	, "P" },
    { "T"	, "T" },
    { "a"	, "a" },
    { "N"	, "N" },
    { "j"	, "j" },
    { "Z"	, "Z" },
    { "u"	, "u" },
    { "k"	, "k" },
    { "t"	, "t" },
    { "v"	, "v" },
    { "B"	, "B" },
    { "s"	, "s" },
    { "H"	, "H" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "U"	, "U" },
    { "F"	, "F" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "V"	, "V" },
    { "h"	, "h" },
    { "f"	, "f" },
    { "i"	, "i" },
    { "A"	, "A" },
    { "n"	, "n" },
    { "m"	, "m" },
    { "l"	, "l" },
    { "p"	, "p" },
    { "R"	, "R" },
    { NULL, NULL },
  };
struct charset peo_n =
  {
    "peo", c_peo , "n", m_normalized,
    peo_n_keys,
    0,
    peo_n_val,
    peo_n_asc,
    0,
  };

static int
qcu_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
        case 0x00000:
        case 0x00037:
        case 0x02085:
        case 0x0004b:
        case 0x00064:
        case 0x00059:
        case 0x00045:
        case 0x00079:
        case 0x01e6d:
        case 0x01e2b:
        case 0x000c1:
        case 0x02082:
        case 0x00067:
        case 0x00065:
        case 0x000cd:
        case 0x02084:
        case 0x0004a:
        case 0x000ec:
        case 0x000da:
        case 0x02081:
        case 0x00044:
        case 0x00071:
        case 0x00062:
        case 0x0007a:
        case 0x000ed:
        case 0x00077:
        case 0x000cc:
        case 0x01e6c:
        case 0x000c9:
        case 0x00051:
        case 0x0004d:
        case 0x0004c:
        case 0x000c8:
        case 0x00058:
        case 0x00050:
        case 0x000c0:
        case 0x01e2a:
        case 0x00160:
        case 0x01e62:
        case 0x02093:
        case 0x02088:
        case 0x00054:
        case 0x00061:
        case 0x0004e:
        case 0x00161:
        case 0x000fa:
        case 0x00032:
        case 0x0005a:
        case 0x00031:
        case 0x00075:
        case 0x0006b:
        case 0x000d9:
        case 0x00074:
        case 0x000f9:
        case 0x00057:
        case 0x000e9:
        case 0x00042:
        case 0x00073:
        case 0x00048:
        case 0x02086:
        case 0x00049:
        case 0x00047:
        case 0x02089:
        case 0x00055:
        case 0x00072:
        case 0x02087:
        case 0x00078:
        case 0x000e8:
        case 0x0015b:
        case 0x002be:
        case 0x00068:
        case 0x00030:
        case 0x02083:
        case 0x00066:
        case 0x01e63:
        case 0x00069:
        case 0x00036:
        case 0x00041:
        case 0x0006e:
        case 0x00033:
        case 0x00039:
        case 0x0006d:
        case 0x00038:
        case 0x0006c:
        case 0x00034:
        case 0x00070:
        case 0x000e0:
        case 0x0015a:
        case 0x00052:
        case 0x000e1:
        case 0x02080:
        case 0x00035:
        case 0x0006f:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%qcu/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qcu_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
          append("S");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x000c1:
          append("Á");
          break;
        case 0x02082:
          append("2");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x000cd:
          append("Í");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x000ec:
          append("ì");
          break;
        case 0x000da:
          append("Ú");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x000ed:
          append("í");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x000cc:
          append("Ì");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x000c9:
          append("É");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x000c8:
          append("È");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x000c0:
          append("À");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00160:
          append("%");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00161:
          append("$");
          break;
        case 0x000fa:
          append("ú");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x000d9:
          append("Ù");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x000f9:
          append("ù");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x000e9:
          append("é");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x000e8:
          append("è");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x000e0:
          append("à");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x000e1:
          append("á");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x0006f:
          append("o");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qcu_g_keys[] =
  {
    { "S"	, "S" },
    { ""	, "" },
    { "7"	, "7" },
    { "K"	, "K" },
    { "d"	, "d" },
    { "Y"	, "Y" },
    { "E"	, "E" },
    { "y"	, "y" },
    { "g"	, "g" },
    { "e"	, "e" },
    { "J"	, "J" },
    { "D"	, "D" },
    { "q"	, "q" },
    { "b"	, "b" },
    { "z"	, "z" },
    { "w"	, "w" },
    { "Q"	, "Q" },
    { "M"	, "M" },
    { "L"	, "L" },
    { "X"	, "X" },
    { "P"	, "P" },
    { "T"	, "T" },
    { "a"	, "a" },
    { "N"	, "N" },
    { "2"	, "2" },
    { "Z"	, "Z" },
    { "1"	, "1" },
    { "u"	, "u" },
    { "k"	, "k" },
    { "t"	, "t" },
    { "W"	, "W" },
    { "B"	, "B" },
    { "s"	, "s" },
    { "H"	, "H" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "U"	, "U" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "h"	, "h" },
    { "0"	, "0" },
    { "f"	, "f" },
    { "i"	, "i" },
    { "6"	, "6" },
    { "A"	, "A" },
    { "n"	, "n" },
    { "3"	, "3" },
    { "9"	, "9" },
    { "m"	, "m" },
    { "8"	, "8" },
    { "l"	, "l" },
    { "4"	, "4" },
    { "p"	, "p" },
    { "R"	, "R" },
    { "5"	, "5" },
    { "o"	, "o" },
    { NULL, NULL },
  };
struct charset qcu_g =
  {
    "qcu", c_qcu , "g", m_graphemic,
    qcu_g_keys,
    0,
    qcu_g_val,
    qcu_g_asc,
    0,
  };

static int
qcu_n_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
        case 0x00000:
        case 0x0002f:
        case 0x0004b:
        case 0x00064:
        case 0x00059:
        case 0x00045:
        case 0x00079:
        case 0x01e6d:
        case 0x01e2b:
        case 0x000ea:
        case 0x00067:
        case 0x000c2:
        case 0x000ce:
        case 0x00065:
        case 0x0004a:
        case 0x00044:
        case 0x00071:
        case 0x00062:
        case 0x0007a:
        case 0x00077:
        case 0x0016b:
        case 0x01e6c:
        case 0x0002d:
        case 0x00051:
        case 0x0004d:
        case 0x00043:
        case 0x0004c:
        case 0x00112:
        case 0x00058:
        case 0x00050:
        case 0x000ca:
        case 0x00100:
        case 0x0016a:
        case 0x01e2a:
        case 0x00027:
        case 0x00160:
        case 0x000e2:
        case 0x01e62:
        case 0x00054:
        case 0x00061:
        case 0x0004e:
        case 0x00161:
        case 0x0006a:
        case 0x0005a:
        case 0x00075:
        case 0x0006b:
        case 0x00074:
        case 0x00057:
        case 0x00076:
        case 0x00042:
        case 0x00073:
        case 0x000fb:
        case 0x0012b:
        case 0x0003f:
        case 0x00048:
        case 0x00113:
        case 0x000ee:
        case 0x00063:
        case 0x0012a:
        case 0x00049:
        case 0x00047:
        case 0x00060:
        case 0x00055:
        case 0x00046:
        case 0x00072:
        case 0x00101:
        case 0x000db:
        case 0x00078:
        case 0x00056:
        case 0x0015b:
        case 0x002be:
        case 0x00068:
        case 0x00066:
        case 0x01e63:
        case 0x00069:
        case 0x00041:
        case 0x0004f:
        case 0x0006e:
        case 0x0006d:
        case 0x0006c:
        case 0x00070:
        case 0x0015a:
        case 0x00052:
        case 0x0006f:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%qcu/n",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qcu_n_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
          append("S");
          break;
        case 0x0002f:
          append("/");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x000ea:
          append("^e");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x000c2:
          append("^A");
          break;
        case 0x000ce:
          append("^I");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x0016b:
          append("=u");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x0002d:
          append("-");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x00112:
          append("=E");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x000ca:
          append("^E");
          break;
        case 0x00100:
          append("=A");
          break;
        case 0x0016a:
          append("=U");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x00160:
          append("%");
          break;
        case 0x000e2:
          append("^a");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00161:
          append("$");
          break;
        case 0x0006a:
          append("j");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00076:
          append("v");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x000fb:
          append("^u");
          break;
        case 0x0012b:
          append("=i");
          break;
        case 0x0003f:
          append("?");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00113:
          append("=e");
          break;
        case 0x000ee:
          append("^i");
          break;
        case 0x00063:
          append("c");
          break;
        case 0x0012a:
          append("=I");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00060:
          append("`");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00046:
          append("F");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00101:
          append("=a");
          break;
        case 0x000db:
          append("^U");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00056:
          append("V");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x0004f:
          append("O");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0006f:
          append("o");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qcu_n_keys[] =
  {
    { "S"	, "S" },
    { ""	, "" },
    { "/"	, "/" },
    { "K"	, "K" },
    { "d"	, "d" },
    { "Y"	, "Y" },
    { "E"	, "E" },
    { "y"	, "y" },
    { "g"	, "g" },
    { "e"	, "e" },
    { "J"	, "J" },
    { "D"	, "D" },
    { "q"	, "q" },
    { "b"	, "b" },
    { "z"	, "z" },
    { "w"	, "w" },
    { "-"	, "-" },
    { "Q"	, "Q" },
    { "M"	, "M" },
    { "C"	, "C" },
    { "L"	, "L" },
    { "X"	, "X" },
    { "P"	, "P" },
    { "'"	, "'" },
    { "T"	, "T" },
    { "a"	, "a" },
    { "N"	, "N" },
    { "j"	, "j" },
    { "Z"	, "Z" },
    { "u"	, "u" },
    { "k"	, "k" },
    { "t"	, "t" },
    { "W"	, "W" },
    { "v"	, "v" },
    { "B"	, "B" },
    { "s"	, "s" },
    { "?"	, "?" },
    { "H"	, "H" },
    { "c"	, "c" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "`"	, "`" },
    { "U"	, "U" },
    { "F"	, "F" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "V"	, "V" },
    { "h"	, "h" },
    { "f"	, "f" },
    { "i"	, "i" },
    { "A"	, "A" },
    { "O"	, "O" },
    { "n"	, "n" },
    { "m"	, "m" },
    { "l"	, "l" },
    { "p"	, "p" },
    { "R"	, "R" },
    { "o"	, "o" },
    { NULL, NULL },
  };
struct charset qcu_n =
  {
    "qcu", c_qcu , "n", m_normalized,
    qcu_n_keys,
    0,
    qcu_n_val,
    qcu_n_asc,
    0,
  };

static int
qeb_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
        case 0x00000:
        case 0x00037:
        case 0x02085:
        case 0x0004b:
        case 0x00064:
        case 0x00059:
        case 0x00045:
        case 0x00079:
        case 0x01e6d:
        case 0x01e2b:
        case 0x000c1:
        case 0x02082:
        case 0x00067:
        case 0x00065:
        case 0x000cd:
        case 0x02084:
        case 0x0004a:
        case 0x000ec:
        case 0x000da:
        case 0x02081:
        case 0x00044:
        case 0x00071:
        case 0x00062:
        case 0x0007a:
        case 0x000ed:
        case 0x00077:
        case 0x000cc:
        case 0x01e6c:
        case 0x000c9:
        case 0x00051:
        case 0x0004d:
        case 0x0004c:
        case 0x000c8:
        case 0x00058:
        case 0x00050:
        case 0x000c0:
        case 0x01e2a:
        case 0x00160:
        case 0x01e62:
        case 0x02093:
        case 0x02088:
        case 0x00054:
        case 0x00061:
        case 0x0004e:
        case 0x00161:
        case 0x000fa:
        case 0x00032:
        case 0x0005a:
        case 0x00031:
        case 0x00075:
        case 0x0006b:
        case 0x000d9:
        case 0x00074:
        case 0x000f9:
        case 0x00057:
        case 0x000e9:
        case 0x00042:
        case 0x00073:
        case 0x00048:
        case 0x02086:
        case 0x00049:
        case 0x00047:
        case 0x02089:
        case 0x00055:
        case 0x00072:
        case 0x02087:
        case 0x00078:
        case 0x000e8:
        case 0x0015b:
        case 0x002be:
        case 0x00068:
        case 0x00030:
        case 0x02083:
        case 0x00066:
        case 0x01e63:
        case 0x00069:
        case 0x00036:
        case 0x00041:
        case 0x0006e:
        case 0x00033:
        case 0x00039:
        case 0x0006d:
        case 0x00038:
        case 0x0006c:
        case 0x00034:
        case 0x00070:
        case 0x000e0:
        case 0x0015a:
        case 0x00052:
        case 0x000e1:
        case 0x02080:
        case 0x00035:
        case 0x0006f:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%qeb/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qeb_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
          append("S");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x000c1:
          append("Á");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x000cd:
          append("Í");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x000ec:
          append("ì");
          break;
        case 0x000da:
          append("Ú");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x000ed:
          append("í");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x000cc:
          append("Ì");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x000c9:
          append("É");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x000c8:
          append("È");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x000c0:
          append("À");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x000fa:
          append("ú");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x000d9:
          append("Ù");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x000f9:
          append("ù");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x000e9:
          append("é");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x000e8:
          append("è");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x000e0:
          append("à");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x000e1:
          append("á");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x0006f:
          append("o");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qeb_g_keys[] =
  {
    { "T,"	, "\xe1\xb9\xac" },
    { "t,"	, "\xe1\xb9\xad" },
    { "SZ"	, "\xc5\xa0" },
    { "sz"	, "\xc5\xa1" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "S'"	, "\xc5\x9a" },
    { "s'"	, "\xc5\x9b" },
    { "'"	, "\xca\xbe" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "S"	, "S" },
    { "7"	, "7" },
    { "K"	, "K" },
    { "d"	, "d" },
    { "Y"	, "Y" },
    { "E"	, "E" },
    { "y"	, "y" },
    { "g"	, "g" },
    { "e"	, "e" },
    { "J"	, "J" },
    { "D"	, "D" },
    { "q"	, "q" },
    { "b"	, "b" },
    { "z"	, "z" },
    { "w"	, "w" },
    { "Q"	, "Q" },
    { "M"	, "M" },
    { "L"	, "L" },
    { "X"	, "X" },
    { "P"	, "P" },
    { "T"	, "T" },
    { "a"	, "a" },
    { "N"	, "N" },
    { "2"	, "2" },
    { "Z"	, "Z" },
    { "1"	, "1" },
    { "u"	, "u" },
    { "k"	, "k" },
    { "t"	, "t" },
    { "W"	, "W" },
    { "B"	, "B" },
    { "s"	, "s" },
    { "H"	, "H" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "U"	, "U" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "h"	, "h" },
    { "0"	, "0" },
    { "f"	, "f" },
    { "i"	, "i" },
    { "6"	, "6" },
    { "A"	, "A" },
    { "n"	, "n" },
    { "3"	, "3" },
    { "9"	, "9" },
    { "m"	, "m" },
    { "8"	, "8" },
    { "l"	, "l" },
    { "4"	, "4" },
    { "p"	, "p" },
    { "R"	, "R" },
    { "5"	, "5" },
    { "o"	, "o" },
    { NULL, NULL },
  };
struct charset qeb_g =
  {
    "qeb", c_qeb , "g", m_graphemic,
    qeb_g_keys,
    0,
    qeb_g_val,
    qeb_g_asc,
    0,
  };

static int
qeb_n_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
        case 0x00000:
        case 0x0004b:
        case 0x00064:
        case 0x00059:
        case 0x00045:
        case 0x00079:
        case 0x01e6d:
        case 0x01e2b:
        case 0x000ea:
        case 0x00067:
        case 0x000c2:
        case 0x000ce:
        case 0x00065:
        case 0x00044:
        case 0x00071:
        case 0x00062:
        case 0x0007a:
        case 0x00077:
        case 0x0016b:
        case 0x01e6c:
        case 0x0002d:
        case 0x00051:
        case 0x0004d:
        case 0x00043:
        case 0x0004c:
        case 0x00112:
        case 0x00058:
        case 0x00050:
        case 0x000ca:
        case 0x00100:
        case 0x0016a:
        case 0x01e2a:
        case 0x00027:
        case 0x00160:
        case 0x000e2:
        case 0x01e62:
        case 0x00054:
        case 0x00061:
        case 0x0004e:
        case 0x00161:
        case 0x0005a:
        case 0x00075:
        case 0x0006b:
        case 0x00074:
        case 0x00057:
        case 0x00042:
        case 0x00073:
        case 0x000fb:
        case 0x0012b:
        case 0x00048:
        case 0x00113:
        case 0x000ee:
        case 0x00063:
        case 0x0012a:
        case 0x00049:
        case 0x00047:
        case 0x00055:
        case 0x00072:
        case 0x00101:
        case 0x000db:
        case 0x00078:
        case 0x0015b:
        case 0x002be:
        case 0x00068:
        case 0x01e63:
        case 0x00069:
        case 0x00041:
        case 0x0004f:
        case 0x0006e:
        case 0x0006d:
        case 0x0006c:
        case 0x00070:
        case 0x0015a:
        case 0x00052:
        case 0x0006f:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%qeb/n",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qeb_n_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
          append("S");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x000ea:
          append("^e");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x000c2:
          append("^A");
          break;
        case 0x000ce:
          append("^I");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x0016b:
          append("=u");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x0002d:
          append("-");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x00112:
          append("=E");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x000ca:
          append("^E");
          break;
        case 0x00100:
          append("=A");
          break;
        case 0x0016a:
          append("=U");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x000e2:
          append("^a");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x000fb:
          append("^u");
          break;
        case 0x0012b:
          append("=i");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00113:
          append("=e");
          break;
        case 0x000ee:
          append("^i");
          break;
        case 0x00063:
          append("c");
          break;
        case 0x0012a:
          append("=I");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x00101:
          append("=a");
          break;
        case 0x000db:
          append("^U");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x0004f:
          append("O");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x0006f:
          append("o");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qeb_n_keys[] =
  {
    { "^U"	, "\xc3\x9b" },
    { "=U"	, "\xc5\xaa" },
    { "=A"	, "\xc4\x80" },
    { "=I"	, "\xc4\xaa" },
    { "T,"	, "\xe1\xb9\xac" },
    { "^i"	, "\xc3\xae" },
    { "^a"	, "\xc3\xa2" },
    { "SZ"	, "\xc5\xa0" },
    { "t,"	, "\xe1\xb9\xad" },
    { "=a"	, "\xc4\x81" },
    { "^E"	, "\xc3\x8a" },
    { "=e"	, "\xc4\x93" },
    { "sz"	, "\xc5\xa1" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "=u"	, "\xc5\xab" },
    { "=E"	, "\xc4\x92" },
    { "^I"	, "\xc3\x8e" },
    { "^A"	, "\xc3\x82" },
    { "^u"	, "\xc3\xbb" },
    { "S'"	, "\xc5\x9a" },
    { "s'"	, "\xc5\x9b" },
    { "=i"	, "\xc4\xab" },
    { "'"	, "\xca\xbe" },
    { "^e"	, "\xc3\xaa" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "S"	, "S" },
    { "K"	, "K" },
    { "d"	, "d" },
    { "Y"	, "Y" },
    { "E"	, "E" },
    { "y"	, "y" },
    { "g"	, "g" },
    { "e"	, "e" },
    { "D"	, "D" },
    { "q"	, "q" },
    { "b"	, "b" },
    { "z"	, "z" },
    { "w"	, "w" },
    { "-"	, "-" },
    { "Q"	, "Q" },
    { "M"	, "M" },
    { "C"	, "C" },
    { "L"	, "L" },
    { "X"	, "X" },
    { "P"	, "P" },
    { "T"	, "T" },
    { "a"	, "a" },
    { "N"	, "N" },
    { "Z"	, "Z" },
    { "u"	, "u" },
    { "k"	, "k" },
    { "t"	, "t" },
    { "W"	, "W" },
    { "B"	, "B" },
    { "s"	, "s" },
    { "H"	, "H" },
    { "c"	, "c" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "U"	, "U" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "h"	, "h" },
    { "i"	, "i" },
    { "A"	, "A" },
    { "O"	, "O" },
    { "n"	, "n" },
    { "m"	, "m" },
    { "l"	, "l" },
    { "p"	, "p" },
    { "R"	, "R" },
    { "o"	, "o" },
    { NULL, NULL },
  };
struct charset qeb_n =
  {
    "qeb", c_qeb , "n", m_normalized,
    qeb_n_keys,
    0,
    qeb_n_val,
    qeb_n_asc,
    0,
  };

static int
qpc_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
        case 0x00000:
        case 0x02085:
        case 0x00037:
        case 0x0004b:
        case 0x00059:
        case 0x00045:
        case 0x02082:
        case 0x02084:
        case 0x02081:
        case 0x00044:
        case 0x01e6c:
        case 0x0004d:
        case 0x0014a:
        case 0x0004c:
        case 0x00058:
        case 0x00050:
        case 0x00027:
        case 0x01e2a:
        case 0x00160:
        case 0x01e62:
        case 0x02093:
        case 0x02088:
        case 0x00054:
        case 0x0004e:
        case 0x00032:
        case 0x0005a:
        case 0x00031:
        case 0x00057:
        case 0x00042:
        case 0x00048:
        case 0x02086:
        case 0x00049:
        case 0x00047:
        case 0x02089:
        case 0x00055:
        case 0x02087:
        case 0x00030:
        case 0x02083:
        case 0x00036:
        case 0x00041:
        case 0x00033:
        case 0x00039:
        case 0x00038:
        case 0x00034:
        case 0x00052:
        case 0x02080:
        case 0x00035:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%qpc/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qpc_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
          append("S");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00035:
          append("5");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qpc_g_keys[] =
  {
    { "T,"	, "\xe1\xb9\xac" },
    { "SZ"	, "\xc5\xa0" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "J"	, "\xc5\x8a" },
    { "S"	, "S" },
    { "7"	, "7" },
    { "K"	, "K" },
    { "Y"	, "Y" },
    { "E"	, "E" },
    { "D"	, "D" },
    { "M"	, "M" },
    { "L"	, "L" },
    { "X"	, "X" },
    { "P"	, "P" },
    { "'"	, "'" },
    { "T"	, "T" },
    { "N"	, "N" },
    { "2"	, "2" },
    { "Z"	, "Z" },
    { "1"	, "1" },
    { "W"	, "W" },
    { "B"	, "B" },
    { "H"	, "H" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "U"	, "U" },
    { "0"	, "0" },
    { "6"	, "6" },
    { "A"	, "A" },
    { "3"	, "3" },
    { "9"	, "9" },
    { "8"	, "8" },
    { "4"	, "4" },
    { "R"	, "R" },
    { "5"	, "5" },
    { NULL, NULL },
  };
struct charset qpc_g =
  {
    "qpc", c_qpc , "g", m_graphemic,
    qpc_g_keys,
    0,
    qpc_g_val,
    qpc_g_asc,
    0,
  };

static int
qpe_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
        case 0x00000:
        case 0x02085:
        case 0x00037:
        case 0x0004b:
        case 0x00059:
        case 0x00045:
        case 0x02082:
        case 0x02084:
        case 0x02081:
        case 0x00044:
        case 0x01e6c:
        case 0x0004d:
        case 0x0014a:
        case 0x0004c:
        case 0x00058:
        case 0x00050:
        case 0x00027:
        case 0x01e2a:
        case 0x00160:
        case 0x01e62:
        case 0x02093:
        case 0x02088:
        case 0x00054:
        case 0x0004e:
        case 0x00032:
        case 0x0005a:
        case 0x00031:
        case 0x00057:
        case 0x00042:
        case 0x00048:
        case 0x02086:
        case 0x00049:
        case 0x00047:
        case 0x02089:
        case 0x00055:
        case 0x02087:
        case 0x00030:
        case 0x02083:
        case 0x00036:
        case 0x00041:
        case 0x00033:
        case 0x00039:
        case 0x00038:
        case 0x00034:
        case 0x00052:
        case 0x02080:
        case 0x00035:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%qpe/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qpe_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
          append("S");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00035:
          append("5");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qpe_g_keys[] =
  {
    { "T,"	, "\xe1\xb9\xac" },
    { "SZ"	, "\xc5\xa0" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "J"	, "\xc5\x8a" },
    { "S"	, "S" },
    { "7"	, "7" },
    { "K"	, "K" },
    { "Y"	, "Y" },
    { "E"	, "E" },
    { "D"	, "D" },
    { "M"	, "M" },
    { "L"	, "L" },
    { "X"	, "X" },
    { "P"	, "P" },
    { "'"	, "'" },
    { "T"	, "T" },
    { "N"	, "N" },
    { "2"	, "2" },
    { "Z"	, "Z" },
    { "1"	, "1" },
    { "W"	, "W" },
    { "B"	, "B" },
    { "H"	, "H" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "U"	, "U" },
    { "0"	, "0" },
    { "6"	, "6" },
    { "A"	, "A" },
    { "3"	, "3" },
    { "9"	, "9" },
    { "8"	, "8" },
    { "4"	, "4" },
    { "R"	, "R" },
    { "5"	, "5" },
    { NULL, NULL },
  };
struct charset qpe_g =
  {
    "qpe", c_qpe , "g", m_graphemic,
    qpe_g_keys,
    0,
    qpe_g_val,
    qpe_g_asc,
    0,
  };

static int
qpn_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
        case 0x00000:
        case 0x00037:
        case 0x02085:
        case 0x0004b:
        case 0x00064:
        case 0x00059:
        case 0x00045:
        case 0x00079:
        case 0x01e6d:
        case 0x01e2b:
        case 0x000c1:
        case 0x02082:
        case 0x00067:
        case 0x00065:
        case 0x000cd:
        case 0x02084:
        case 0x0004a:
        case 0x000ec:
        case 0x000da:
        case 0x02081:
        case 0x00044:
        case 0x00071:
        case 0x00062:
        case 0x0007a:
        case 0x000ed:
        case 0x00077:
        case 0x000cc:
        case 0x01e6c:
        case 0x000c9:
        case 0x00051:
        case 0x0004d:
        case 0x0014a:
        case 0x0004c:
        case 0x000c8:
        case 0x00058:
        case 0x00050:
        case 0x000c0:
        case 0x01e2a:
        case 0x00160:
        case 0x0014b:
        case 0x01e62:
        case 0x02093:
        case 0x02088:
        case 0x00054:
        case 0x00061:
        case 0x0004e:
        case 0x00161:
        case 0x000fa:
        case 0x00032:
        case 0x0006a:
        case 0x0005a:
        case 0x00031:
        case 0x00075:
        case 0x0006b:
        case 0x000d9:
        case 0x00074:
        case 0x000f9:
        case 0x00057:
        case 0x000e9:
        case 0x00042:
        case 0x00073:
        case 0x00048:
        case 0x02086:
        case 0x00049:
        case 0x00047:
        case 0x02089:
        case 0x00055:
        case 0x00072:
        case 0x02087:
        case 0x00078:
        case 0x000e8:
        case 0x0015b:
        case 0x002be:
        case 0x00068:
        case 0x00030:
        case 0x02083:
        case 0x00066:
        case 0x01e63:
        case 0x00069:
        case 0x00036:
        case 0x00041:
        case 0x0006e:
        case 0x00033:
        case 0x00039:
        case 0x0006d:
        case 0x00038:
        case 0x0006c:
        case 0x00034:
        case 0x00070:
        case 0x000e0:
        case 0x0015a:
        case 0x00052:
        case 0x000e1:
        case 0x02080:
        case 0x00035:
        case 0x0006f:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%qpn/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qpn_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
          append("S");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x000c1:
          append("Á");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x000cd:
          append("Í");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x000ec:
          append("ì");
          break;
        case 0x000da:
          append("Ú");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x000ed:
          append("í");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x000cc:
          append("Ì");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x000c9:
          append("É");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x000c8:
          append("È");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x000c0:
          append("À");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x000fa:
          append("ú");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x0006a:
          append("j");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x000d9:
          append("Ù");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x000f9:
          append("ù");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x000e9:
          append("é");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x000e8:
          append("è");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x000e0:
          append("à");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x000e1:
          append("á");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x0006f:
          append("o");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qpn_g_keys[] =
  {
    { "T,"	, "\xe1\xb9\xac" },
    { "t,"	, "\xe1\xb9\xad" },
    { "SZ"	, "\xc5\xa0" },
    { "j"	, "\xc5\x8b" },
    { "sz"	, "\xc5\xa1" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "J"	, "\xc5\x8a" },
    { "S'"	, "\xc5\x9a" },
    { "s'"	, "\xc5\x9b" },
    { "'"	, "\xca\xbe" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "S"	, "S" },
    { "7"	, "7" },
    { "K"	, "K" },
    { "d"	, "d" },
    { "Y"	, "Y" },
    { "E"	, "E" },
    { "y"	, "y" },
    { "g"	, "g" },
    { "e"	, "e" },
    { "D"	, "D" },
    { "q"	, "q" },
    { "b"	, "b" },
    { "z"	, "z" },
    { "w"	, "w" },
    { "Q"	, "Q" },
    { "M"	, "M" },
    { "L"	, "L" },
    { "X"	, "X" },
    { "P"	, "P" },
    { "T"	, "T" },
    { "a"	, "a" },
    { "N"	, "N" },
    { "2"	, "2" },
    { "Z"	, "Z" },
    { "1"	, "1" },
    { "u"	, "u" },
    { "k"	, "k" },
    { "t"	, "t" },
    { "W"	, "W" },
    { "B"	, "B" },
    { "s"	, "s" },
    { "H"	, "H" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "U"	, "U" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "h"	, "h" },
    { "0"	, "0" },
    { "f"	, "f" },
    { "i"	, "i" },
    { "6"	, "6" },
    { "A"	, "A" },
    { "n"	, "n" },
    { "3"	, "3" },
    { "9"	, "9" },
    { "m"	, "m" },
    { "8"	, "8" },
    { "l"	, "l" },
    { "4"	, "4" },
    { "p"	, "p" },
    { "R"	, "R" },
    { "5"	, "5" },
    { "o"	, "o" },
    { NULL, NULL },
  };
struct charset qpn_g =
  {
    "qpn", c_qpn , "g", m_graphemic,
    qpn_g_keys,
    0,
    qpn_g_val,
    qpn_g_asc,
    0,
  };

static int
sux_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
        case 0x00000:
        case 0x00037:
        case 0x02085:
        case 0x0004b:
        case 0x0002c:
        case 0x00064:
        case 0x00059:
        case 0x00045:
        case 0x00079:
        case 0x01e6d:
        case 0x01e2b:
        case 0x000c1:
        case 0x02082:
        case 0x00067:
        case 0x00065:
        case 0x000cd:
        case 0x02084:
        case 0x000ec:
        case 0x000da:
        case 0x02081:
        case 0x00071:
        case 0x00044:
        case 0x00062:
        case 0x0007a:
        case 0x000ed:
        case 0x00077:
        case 0x000cc:
        case 0x01e6c:
        case 0x000c9:
        case 0x00051:
        case 0x0004d:
        case 0x00043:
        case 0x0014a:
        case 0x0004c:
        case 0x000c8:
        case 0x00058:
        case 0x00050:
        case 0x000c0:
        case 0x00027:
        case 0x01e2a:
        case 0x00160:
        case 0x0014b:
        case 0x01e62:
        case 0x02093:
        case 0x02088:
        case 0x00054:
        case 0x00061:
        case 0x00161:
        case 0x0004e:
        case 0x000fa:
        case 0x00032:
        case 0x0005a:
        case 0x00031:
        case 0x00075:
        case 0x0006b:
        case 0x000d9:
        case 0x00074:
        case 0x000f9:
        case 0x00057:
        case 0x000e9:
        case 0x00073:
        case 0x00042:
        case 0x00048:
        case 0x02086:
        case 0x00049:
        case 0x00047:
        case 0x000b7:
        case 0x02089:
        case 0x00055:
        case 0x00072:
        case 0x0003a:
        case 0x02087:
        case 0x00078:
        case 0x000e8:
        case 0x000b0:
        case 0x002be:
        case 0x00068:
        case 0x00030:
        case 0x02083:
        case 0x0003b:
        case 0x00066:
        case 0x01e63:
        case 0x00069:
        case 0x00036:
        case 0x00041:
        case 0x0004f:
        case 0x0006e:
        case 0x00033:
        case 0x00039:
        case 0x0006d:
        case 0x00038:
        case 0x0006c:
        case 0x00034:
        case 0x00070:
        case 0x000e0:
        case 0x00052:
        case 0x000e1:
        case 0x02080:
        case 0x00035:
        case 0x0006f:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%sux/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
sux_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
          append("S");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x0002c:
          append(",");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x000c1:
          append("Á");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x000cd:
          append("Í");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x000ec:
          append("ì");
          break;
        case 0x000da:
          append("Ú");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x000ed:
          append("í");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x000cc:
          append("Ì");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x000c9:
          append("É");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x000c8:
          append("È");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x000c0:
          append("À");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x000fa:
          append("ú");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x000d9:
          append("Ù");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x000f9:
          append("ù");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x000e9:
          append("é");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x000b7:
          append("·");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x0003a:
          append(":");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x000e8:
          append("è");
          break;
        case 0x000b0:
          append("°");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x0003b:
          append(";");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x0004f:
          append("O");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x000e0:
          append("à");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x000e1:
          append("á");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x0006f:
          append("o");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys sux_g_keys[] =
  {
    { "T,"	, "\xe1\xb9\xac" },
    { "t,"	, "\xe1\xb9\xad" },
    { "SZ"	, "\xc5\xa0" },
    { "j"	, "\xc5\x8b" },
    { "sz"	, "\xc5\xa1" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "J"	, "\xc5\x8a" },
    { "'"	, "\xca\xbe" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "S"	, "S" },
    { "7"	, "7" },
    { "K"	, "K" },
    { ","	, "," },
    { "d"	, "d" },
    { "Y"	, "Y" },
    { "E"	, "E" },
    { "y"	, "y" },
    { "g"	, "g" },
    { "e"	, "e" },
    { "q"	, "q" },
    { "D"	, "D" },
    { "b"	, "b" },
    { "z"	, "z" },
    { "w"	, "w" },
    { "Q"	, "Q" },
    { "M"	, "M" },
    { "C"	, "C" },
    { "L"	, "L" },
    { "X"	, "X" },
    { "P"	, "P" },
    { "T"	, "T" },
    { "a"	, "a" },
    { "N"	, "N" },
    { "2"	, "2" },
    { "Z"	, "Z" },
    { "1"	, "1" },
    { "u"	, "u" },
    { "k"	, "k" },
    { "t"	, "t" },
    { "W"	, "W" },
    { "s"	, "s" },
    { "B"	, "B" },
    { "H"	, "H" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "U"	, "U" },
    { "r"	, "r" },
    { ":"	, ":" },
    { "x"	, "x" },
    { "h"	, "h" },
    { "0"	, "0" },
    { ";"	, ";" },
    { "f"	, "f" },
    { "i"	, "i" },
    { "6"	, "6" },
    { "A"	, "A" },
    { "O"	, "O" },
    { "n"	, "n" },
    { "3"	, "3" },
    { "9"	, "9" },
    { "m"	, "m" },
    { "8"	, "8" },
    { "l"	, "l" },
    { "4"	, "4" },
    { "p"	, "p" },
    { "R"	, "R" },
    { "5"	, "5" },
    { "o"	, "o" },
    { NULL, NULL },
  };
struct charset sux_g =
  {
    "sux", c_sux , "g", m_graphemic,
    sux_g_keys,
    0,
    sux_g_val,
    sux_g_asc,
    0,
  };

static int
sux_n_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
        case 0x00000:
        case 0x00037:
        case 0x02085:
        case 0x0004b:
        case 0x00064:
        case 0x00059:
        case 0x00045:
        case 0x00079:
        case 0x01e6d:
        case 0x01e2b:
        case 0x02082:
        case 0x00067:
        case 0x00065:
        case 0x02084:
        case 0x02081:
        case 0x00071:
        case 0x00044:
        case 0x00062:
        case 0x0007a:
        case 0x00077:
        case 0x0005c:
        case 0x01e6c:
        case 0x0002d:
        case 0x0007e:
        case 0x0004d:
        case 0x00043:
        case 0x0014a:
        case 0x0004c:
        case 0x00058:
        case 0x00050:
        case 0x0002b:
        case 0x00027:
        case 0x01e2a:
        case 0x00160:
        case 0x0014b:
        case 0x01e62:
        case 0x02093:
        case 0x02088:
        case 0x00054:
        case 0x00061:
        case 0x00161:
        case 0x0004e:
        case 0x00032:
        case 0x0005a:
        case 0x00031:
        case 0x00075:
        case 0x0006b:
        case 0x00074:
        case 0x00057:
        case 0x00073:
        case 0x00042:
        case 0x00048:
        case 0x02086:
        case 0x00026:
        case 0x00049:
        case 0x00047:
        case 0x02089:
        case 0x00055:
        case 0x00072:
        case 0x02087:
        case 0x00078:
        case 0x00068:
        case 0x00030:
        case 0x0002e:
        case 0x00040:
        case 0x02083:
        case 0x00066:
        case 0x01e63:
        case 0x00069:
        case 0x00036:
        case 0x00041:
        case 0x0004f:
        case 0x0006e:
        case 0x00033:
        case 0x00039:
        case 0x0006d:
        case 0x00038:
        case 0x0006c:
        case 0x00034:
        case 0x00070:
        case 0x00052:
        case 0x02080:
        case 0x00035:
        case 0x0006f:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%sux/n",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
sux_n_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
          append("S");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x0005c:
          append("\x5c");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x0002d:
          append("-");
          break;
        case 0x0007e:
          append("~");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x0002b:
          append("+");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00026:
          append("&");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x0002e:
          append(".");
          break;
        case 0x00040:
          append("@");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x0004f:
          append("O");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x0006f:
          append("o");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys sux_n_keys[] =
  {
    { "T,"	, "\xe1\xb9\xac" },
    { "t,"	, "\xe1\xb9\xad" },
    { "SZ"	, "\xc5\xa0" },
    { "j"	, "\xc5\x8b" },
    { "sz"	, "\xc5\xa1" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "J"	, "\xc5\x8a" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "S"	, "S" },
    { "7"	, "7" },
    { "K"	, "K" },
    { "d"	, "d" },
    { "Y"	, "Y" },
    { "E"	, "E" },
    { "y"	, "y" },
    { "g"	, "g" },
    { "e"	, "e" },
    { "q"	, "q" },
    { "D"	, "D" },
    { "b"	, "b" },
    { "z"	, "z" },
    { "w"	, "w" },
    { "\x5c"	, "\x5c" },
    { "-"	, "-" },
    { "~"	, "~" },
    { "M"	, "M" },
    { "C"	, "C" },
    { "L"	, "L" },
    { "X"	, "X" },
    { "P"	, "P" },
    { "+"	, "+" },
    { "'"	, "'" },
    { "T"	, "T" },
    { "a"	, "a" },
    { "N"	, "N" },
    { "2"	, "2" },
    { "Z"	, "Z" },
    { "1"	, "1" },
    { "u"	, "u" },
    { "k"	, "k" },
    { "t"	, "t" },
    { "W"	, "W" },
    { "s"	, "s" },
    { "B"	, "B" },
    { "H"	, "H" },
    { "&"	, "&" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "U"	, "U" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "h"	, "h" },
    { "0"	, "0" },
    { "."	, "." },
    { "@"	, "@" },
    { "f"	, "f" },
    { "i"	, "i" },
    { "6"	, "6" },
    { "A"	, "A" },
    { "O"	, "O" },
    { "n"	, "n" },
    { "3"	, "3" },
    { "9"	, "9" },
    { "m"	, "m" },
    { "8"	, "8" },
    { "l"	, "l" },
    { "4"	, "4" },
    { "p"	, "p" },
    { "R"	, "R" },
    { "5"	, "5" },
    { "o"	, "o" },
    { NULL, NULL },
  };
struct charset sux_n =
  {
    "sux", c_sux , "n", m_normalized,
    sux_n_keys,
    0,
    sux_n_val,
    sux_n_asc,
    0,
  };

static int
xhu_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
        case 0x00000:
        case 0x00037:
        case 0x02085:
        case 0x0004b:
        case 0x00064:
        case 0x00059:
        case 0x00045:
        case 0x00079:
        case 0x01e6d:
        case 0x01e2b:
        case 0x02082:
        case 0x00067:
        case 0x00065:
        case 0x02084:
        case 0x02081:
        case 0x00071:
        case 0x00044:
        case 0x00062:
        case 0x0007a:
        case 0x00077:
        case 0x01e6c:
        case 0x00051:
        case 0x0004d:
        case 0x00043:
        case 0x0014a:
        case 0x0004c:
        case 0x00058:
        case 0x00050:
        case 0x00027:
        case 0x01e2a:
        case 0x00160:
        case 0x0014b:
        case 0x01e62:
        case 0x02093:
        case 0x02088:
        case 0x00054:
        case 0x00061:
        case 0x00161:
        case 0x0004e:
        case 0x00032:
        case 0x0005a:
        case 0x00031:
        case 0x00075:
        case 0x0006b:
        case 0x00074:
        case 0x00057:
        case 0x00073:
        case 0x00042:
        case 0x00048:
        case 0x02086:
        case 0x00049:
        case 0x00047:
        case 0x02089:
        case 0x00055:
        case 0x00072:
        case 0x02087:
        case 0x00078:
        case 0x002be:
        case 0x00068:
        case 0x00030:
        case 0x02083:
        case 0x00066:
        case 0x01e63:
        case 0x00069:
        case 0x00036:
        case 0x00041:
        case 0x0006e:
        case 0x00033:
        case 0x00039:
        case 0x0006d:
        case 0x00038:
        case 0x0006c:
        case 0x00034:
        case 0x00070:
        case 0x00052:
        case 0x02080:
        case 0x00035:
        case 0x0006f:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%xhu/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
xhu_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
          append("S");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x0006f:
          append("o");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys xhu_g_keys[] =
  {
    { "T,"	, "\xe1\xb9\xac" },
    { "t,"	, "\xe1\xb9\xad" },
    { "SZ"	, "\xc5\xa0" },
    { "sz"	, "\xc5\xa1" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "'"	, "\xca\xbe" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "S"	, "S" },
    { "7"	, "7" },
    { "K"	, "K" },
    { "d"	, "d" },
    { "Y"	, "Y" },
    { "E"	, "E" },
    { "y"	, "y" },
    { "g"	, "g" },
    { "e"	, "e" },
    { "q"	, "q" },
    { "D"	, "D" },
    { "b"	, "b" },
    { "z"	, "z" },
    { "w"	, "w" },
    { "Q"	, "Q" },
    { "M"	, "M" },
    { "C"	, "C" },
    { "L"	, "L" },
    { "X"	, "X" },
    { "P"	, "P" },
    { "T"	, "T" },
    { "a"	, "a" },
    { "N"	, "N" },
    { "2"	, "2" },
    { "Z"	, "Z" },
    { "1"	, "1" },
    { "u"	, "u" },
    { "k"	, "k" },
    { "t"	, "t" },
    { "W"	, "W" },
    { "s"	, "s" },
    { "B"	, "B" },
    { "H"	, "H" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "U"	, "U" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "h"	, "h" },
    { "0"	, "0" },
    { "f"	, "f" },
    { "i"	, "i" },
    { "6"	, "6" },
    { "A"	, "A" },
    { "n"	, "n" },
    { "3"	, "3" },
    { "9"	, "9" },
    { "m"	, "m" },
    { "8"	, "8" },
    { "l"	, "l" },
    { "4"	, "4" },
    { "p"	, "p" },
    { "R"	, "R" },
    { "5"	, "5" },
    { "o"	, "o" },
    { NULL, NULL },
  };
struct charset xhu_g =
  {
    "xhu", c_xhu , "g", m_graphemic,
    xhu_g_keys,
    0,
    xhu_g_val,
    xhu_g_asc,
    0,
  };

static int
uga_g_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
        case 0x00000:
        case 0x00037:
        case 0x02085:
        case 0x0004b:
        case 0x00064:
        case 0x00059:
        case 0x00045:
        case 0x00079:
        case 0x01e6d:
        case 0x01e2b:
        case 0x02082:
        case 0x00067:
        case 0x00065:
        case 0x02084:
        case 0x02081:
        case 0x00071:
        case 0x00044:
        case 0x00062:
        case 0x0007a:
        case 0x00077:
        case 0x01e6c:
        case 0x00051:
        case 0x0004d:
        case 0x00043:
        case 0x0014a:
        case 0x0004c:
        case 0x00058:
        case 0x00050:
        case 0x00027:
        case 0x01e2a:
        case 0x00160:
        case 0x0014b:
        case 0x01e62:
        case 0x02093:
        case 0x02088:
        case 0x00054:
        case 0x00061:
        case 0x00161:
        case 0x0004e:
        case 0x00032:
        case 0x0005a:
        case 0x00031:
        case 0x00075:
        case 0x0006b:
        case 0x00074:
        case 0x00057:
        case 0x00073:
        case 0x00042:
        case 0x00048:
        case 0x02086:
        case 0x00049:
        case 0x00047:
        case 0x02089:
        case 0x00055:
        case 0x00072:
        case 0x02087:
        case 0x00078:
        case 0x00068:
        case 0x00030:
        case 0x02083:
        case 0x00066:
        case 0x01e63:
        case 0x00069:
        case 0x00036:
        case 0x00041:
        case 0x0006e:
        case 0x00033:
        case 0x00039:
        case 0x0006d:
        case 0x00038:
        case 0x0006c:
        case 0x00034:
        case 0x00070:
        case 0x00052:
        case 0x02080:
        case 0x00035:
        case 0x0006f:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%uga/g",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
uga_g_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
          append("S");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x02080:
          append("₀");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x0006f:
          append("o");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys uga_g_keys[] =
  {
    { "T,"	, "\xe1\xb9\xac" },
    { "t,"	, "\xe1\xb9\xad" },
    { "SZ"	, "\xc5\xa0" },
    { "sz"	, "\xc5\xa1" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "S"	, "S" },
    { "7"	, "7" },
    { "K"	, "K" },
    { "d"	, "d" },
    { "Y"	, "Y" },
    { "E"	, "E" },
    { "y"	, "y" },
    { "g"	, "g" },
    { "e"	, "e" },
    { "q"	, "q" },
    { "D"	, "D" },
    { "b"	, "b" },
    { "z"	, "z" },
    { "w"	, "w" },
    { "Q"	, "Q" },
    { "M"	, "M" },
    { "C"	, "C" },
    { "L"	, "L" },
    { "X"	, "X" },
    { "P"	, "P" },
    { "'"	, "'" },
    { "T"	, "T" },
    { "a"	, "a" },
    { "N"	, "N" },
    { "2"	, "2" },
    { "Z"	, "Z" },
    { "1"	, "1" },
    { "u"	, "u" },
    { "k"	, "k" },
    { "t"	, "t" },
    { "W"	, "W" },
    { "s"	, "s" },
    { "B"	, "B" },
    { "H"	, "H" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "U"	, "U" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "h"	, "h" },
    { "0"	, "0" },
    { "f"	, "f" },
    { "i"	, "i" },
    { "6"	, "6" },
    { "A"	, "A" },
    { "n"	, "n" },
    { "3"	, "3" },
    { "9"	, "9" },
    { "m"	, "m" },
    { "8"	, "8" },
    { "l"	, "l" },
    { "4"	, "4" },
    { "p"	, "p" },
    { "R"	, "R" },
    { "5"	, "5" },
    { "o"	, "o" },
    { NULL, NULL },
  };
struct charset uga_g =
  {
    "uga", c_uga , "g", m_graphemic,
    uga_g_keys,
    0,
    uga_g_val,
    uga_g_asc,
    0,
  };

static int
qse_n_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
        case 0x00000:
        case 0x02085:
        case 0x00079:
        case 0x01e6d:
        case 0x01e2b:
        case 0x000c1:
        case 0x00067:
        case 0x000ce:
        case 0x00065:
        case 0x0004a:
        case 0x00044:
        case 0x000ed:
        case 0x00077:
        case 0x0016b:
        case 0x000cc:
        case 0x0002d:
        case 0x00051:
        case 0x0004d:
        case 0x00043:
        case 0x00112:
        case 0x000c8:
        case 0x00058:
        case 0x000ca:
        case 0x00100:
        case 0x0014b:
        case 0x000e2:
        case 0x01e62:
        case 0x02093:
        case 0x02088:
        case 0x00061:
        case 0x0004e:
        case 0x000fa:
        case 0x00032:
        case 0x0005a:
        case 0x00075:
        case 0x0006b:
        case 0x000d9:
        case 0x000e9:
        case 0x00042:
        case 0x00073:
        case 0x0003f:
        case 0x00048:
        case 0x00113:
        case 0x02086:
        case 0x000ee:
        case 0x00063:
        case 0x0012a:
        case 0x00055:
        case 0x00046:
        case 0x00101:
        case 0x0003a:
        case 0x0002a:
        case 0x000db:
        case 0x000b0:
        case 0x00056:
        case 0x0015b:
        case 0x002be:
        case 0x00030:
        case 0x00069:
        case 0x0005d:
        case 0x0006d:
        case 0x00038:
        case 0x00052:
        case 0x000e1:
        case 0x00035:
        case 0x0006f:
        case 0x0004b:
        case 0x00037:
        case 0x00064:
        case 0x0002c:
        case 0x00045:
        case 0x00059:
        case 0x000ea:
        case 0x02082:
        case 0x000c2:
        case 0x000cd:
        case 0x02084:
        case 0x000ec:
        case 0x0007c:
        case 0x000da:
        case 0x02081:
        case 0x00062:
        case 0x00071:
        case 0x0007a:
        case 0x01e6c:
        case 0x000c9:
        case 0x0014a:
        case 0x0005b:
        case 0x0004c:
        case 0x00050:
        case 0x000c0:
        case 0x00027:
        case 0x01e2a:
        case 0x0016a:
        case 0x00160:
        case 0x00054:
        case 0x00161:
        case 0x0006a:
        case 0x00031:
        case 0x0003c:
        case 0x00074:
        case 0x00057:
        case 0x000f9:
        case 0x00076:
        case 0x0003e:
        case 0x0012b:
        case 0x000fb:
        case 0x00049:
        case 0x00047:
        case 0x00060:
        case 0x02089:
        case 0x000b7:
        case 0x00072:
        case 0x02087:
        case 0x00078:
        case 0x000e8:
        case 0x00068:
        case 0x02083:
        case 0x00066:
        case 0x0003b:
        case 0x01e63:
        case 0x00041:
        case 0x00036:
        case 0x0006e:
        case 0x0004f:
        case 0x00033:
        case 0x00039:
        case 0x0006c:
        case 0x00070:
        case 0x00034:
        case 0x0015a:
        case 0x000e0:
        case 0x02080:
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%qse/n",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

static char *
qse_n_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
        case 0x00053:
          append("S");
          break;
        case 0x02085:
          append("₅");
          break;
        case 0x00079:
          append("y");
          break;
        case 0x01e6d:
          append("t,");
          break;
        case 0x01e2b:
          append("ḫ");
          break;
        case 0x000c1:
          append("Á");
          break;
        case 0x00067:
          append("g");
          break;
        case 0x000ce:
          append("^I");
          break;
        case 0x00065:
          append("e");
          break;
        case 0x0004a:
          append("J");
          break;
        case 0x00044:
          append("D");
          break;
        case 0x000ed:
          append("í");
          break;
        case 0x00077:
          append("w");
          break;
        case 0x0016b:
          append("=u");
          break;
        case 0x000cc:
          append("Ì");
          break;
        case 0x0002d:
          append("-");
          break;
        case 0x00051:
          append("Q");
          break;
        case 0x0004d:
          append("M");
          break;
        case 0x00043:
          append("C");
          break;
        case 0x00112:
          append("=E");
          break;
        case 0x000c8:
          append("È");
          break;
        case 0x00058:
          append("X");
          break;
        case 0x000ca:
          append("^E");
          break;
        case 0x00100:
          append("=A");
          break;
        case 0x0014b:
          append("j");
          break;
        case 0x000e2:
          append("^a");
          break;
        case 0x01e62:
          append("S,");
          break;
        case 0x02093:
          append("ₓ");
          break;
        case 0x02088:
          append("₈");
          break;
        case 0x00061:
          append("a");
          break;
        case 0x0004e:
          append("N");
          break;
        case 0x000fa:
          append("ú");
          break;
        case 0x00032:
          append("2");
          break;
        case 0x0005a:
          append("Z");
          break;
        case 0x00075:
          append("u");
          break;
        case 0x0006b:
          append("k");
          break;
        case 0x000d9:
          append("Ù");
          break;
        case 0x000e9:
          append("é");
          break;
        case 0x00042:
          append("B");
          break;
        case 0x00073:
          append("s");
          break;
        case 0x0003f:
          append("?");
          break;
        case 0x00048:
          append("H");
          break;
        case 0x00113:
          append("=e");
          break;
        case 0x02086:
          append("₆");
          break;
        case 0x000ee:
          append("^i");
          break;
        case 0x00063:
          append("c");
          break;
        case 0x0012a:
          append("=I");
          break;
        case 0x00055:
          append("U");
          break;
        case 0x00046:
          append("F");
          break;
        case 0x00101:
          append("=a");
          break;
        case 0x0003a:
          append(":");
          break;
        case 0x0002a:
          append("*");
          break;
        case 0x000db:
          append("^U");
          break;
        case 0x000b0:
          append("°");
          break;
        case 0x00056:
          append("V");
          break;
        case 0x0015b:
          append("s'");
          break;
        case 0x002be:
          append("'");
          break;
        case 0x00030:
          append("0");
          break;
        case 0x00069:
          append("i");
          break;
        case 0x0005d:
          append("]");
          break;
        case 0x0006d:
          append("m");
          break;
        case 0x00038:
          append("8");
          break;
        case 0x00052:
          append("R");
          break;
        case 0x000e1:
          append("á");
          break;
        case 0x00035:
          append("5");
          break;
        case 0x0006f:
          append("o");
          break;
        case 0x0004b:
          append("K");
          break;
        case 0x00037:
          append("7");
          break;
        case 0x00064:
          append("d");
          break;
        case 0x0002c:
          append(",");
          break;
        case 0x00045:
          append("E");
          break;
        case 0x00059:
          append("Y");
          break;
        case 0x000ea:
          append("^e");
          break;
        case 0x02082:
          append("₂");
          break;
        case 0x000c2:
          append("^A");
          break;
        case 0x000cd:
          append("Í");
          break;
        case 0x02084:
          append("₄");
          break;
        case 0x000ec:
          append("ì");
          break;
        case 0x0007c:
          append("|");
          break;
        case 0x000da:
          append("Ú");
          break;
        case 0x02081:
          append("₁");
          break;
        case 0x00062:
          append("b");
          break;
        case 0x00071:
          append("q");
          break;
        case 0x0007a:
          append("z");
          break;
        case 0x01e6c:
          append("T,");
          break;
        case 0x000c9:
          append("É");
          break;
        case 0x0014a:
          append("J");
          break;
        case 0x0005b:
          append("[");
          break;
        case 0x0004c:
          append("L");
          break;
        case 0x00050:
          append("P");
          break;
        case 0x000c0:
          append("À");
          break;
        case 0x00027:
          append("'");
          break;
        case 0x01e2a:
          append("Ḫ");
          break;
        case 0x0016a:
          append("=U");
          break;
        case 0x00160:
          append("SZ");
          break;
        case 0x00054:
          append("T");
          break;
        case 0x00161:
          append("sz");
          break;
        case 0x0006a:
          append("j");
          break;
        case 0x00031:
          append("1");
          break;
        case 0x0003c:
          append("<");
          break;
        case 0x00074:
          append("t");
          break;
        case 0x00057:
          append("W");
          break;
        case 0x000f9:
          append("ù");
          break;
        case 0x00076:
          append("v");
          break;
        case 0x0003e:
          append(">");
          break;
        case 0x0012b:
          append("=i");
          break;
        case 0x000fb:
          append("^u");
          break;
        case 0x00049:
          append("I");
          break;
        case 0x00047:
          append("G");
          break;
        case 0x00060:
          append("`");
          break;
        case 0x02089:
          append("₉");
          break;
        case 0x000b7:
          append("·");
          break;
        case 0x00072:
          append("r");
          break;
        case 0x02087:
          append("₇");
          break;
        case 0x00078:
          append("x");
          break;
        case 0x000e8:
          append("è");
          break;
        case 0x00068:
          append("h");
          break;
        case 0x02083:
          append("₃");
          break;
        case 0x00066:
          append("f");
          break;
        case 0x0003b:
          append(";");
          break;
        case 0x01e63:
          append("s,");
          break;
        case 0x00041:
          append("A");
          break;
        case 0x00036:
          append("6");
          break;
        case 0x0006e:
          append("n");
          break;
        case 0x0004f:
          append("O");
          break;
        case 0x00033:
          append("3");
          break;
        case 0x00039:
          append("9");
          break;
        case 0x0006c:
          append("l");
          break;
        case 0x00070:
          append("p");
          break;
        case 0x00034:
          append("4");
          break;
        case 0x0015a:
          append("S'");
          break;
        case 0x000e0:
          append("à");
          break;
        case 0x02080:
          append("₀");
          break;
        default:
          break;
	}
    }
  return buf;
}

struct charset_keys qse_n_keys[] =
  {
    { "^U"	, "\xc3\x9b" },
    { "=U"	, "\xc5\xaa" },
    { "=A"	, "\xc4\x80" },
    { "=I"	, "\xc4\xaa" },
    { "T,"	, "\xe1\xb9\xac" },
    { "^i"	, "\xc3\xae" },
    { "^a"	, "\xc3\xa2" },
    { "SZ"	, "\xc5\xa0" },
    { "t,"	, "\xe1\xb9\xad" },
    { "j"	, "\xc5\x8b" },
    { "=a"	, "\xc4\x81" },
    { "^E"	, "\xc3\x8a" },
    { "=e"	, "\xc4\x93" },
    { "sz"	, "\xc5\xa1" },
    { "S,"	, "\xe1\xb9\xa2" },
    { "=u"	, "\xc5\xab" },
    { "J"	, "\xc5\x8a" },
    { "=E"	, "\xc4\x92" },
    { "^I"	, "\xc3\x8e" },
    { "^A"	, "\xc3\x82" },
    { "^u"	, "\xc3\xbb" },
    { "S'"	, "\xc5\x9a" },
    { "s'"	, "\xc5\x9b" },
    { "=i"	, "\xc4\xab" },
    { "'"	, "\xca\xbe" },
    { "^e"	, "\xc3\xaa" },
    { "s,"	, "\xe1\xb9\xa3" },
    { "S"	, "S" },
    { "y"	, "y" },
    { "g"	, "g" },
    { "e"	, "e" },
    { "D"	, "D" },
    { "w"	, "w" },
    { "-"	, "-" },
    { "Q"	, "Q" },
    { "M"	, "M" },
    { "C"	, "C" },
    { "X"	, "X" },
    { "a"	, "a" },
    { "N"	, "N" },
    { "2"	, "2" },
    { "Z"	, "Z" },
    { "u"	, "u" },
    { "k"	, "k" },
    { "B"	, "B" },
    { "s"	, "s" },
    { "?"	, "?" },
    { "H"	, "H" },
    { "c"	, "c" },
    { "U"	, "U" },
    { "F"	, "F" },
    { ":"	, ":" },
    { "*"	, "*" },
    { "V"	, "V" },
    { "0"	, "0" },
    { "i"	, "i" },
    { "]"	, "]" },
    { "m"	, "m" },
    { "8"	, "8" },
    { "R"	, "R" },
    { "5"	, "5" },
    { "o"	, "o" },
    { "K"	, "K" },
    { "7"	, "7" },
    { "d"	, "d" },
    { ","	, "," },
    { "E"	, "E" },
    { "Y"	, "Y" },
    { "|"	, "|" },
    { "b"	, "b" },
    { "q"	, "q" },
    { "z"	, "z" },
    { "["	, "[" },
    { "L"	, "L" },
    { "P"	, "P" },
    { "T"	, "T" },
    { "1"	, "1" },
    { "<"	, "<" },
    { "t"	, "t" },
    { "W"	, "W" },
    { "v"	, "v" },
    { ">"	, ">" },
    { "I"	, "I" },
    { "G"	, "G" },
    { "`"	, "`" },
    { "r"	, "r" },
    { "x"	, "x" },
    { "h"	, "h" },
    { "f"	, "f" },
    { ";"	, ";" },
    { "A"	, "A" },
    { "6"	, "6" },
    { "n"	, "n" },
    { "O"	, "O" },
    { "3"	, "3" },
    { "9"	, "9" },
    { "l"	, "l" },
    { "p"	, "p" },
    { "4"	, "4" },
    { NULL, NULL },
  };
struct charset qse_n =
  {
    "qse", c_qse , "n", m_normalized,
    qse_n_keys,
    0,
    qse_n_val,
    qse_n_asc,
    0,
  };


struct charset *cset_index[c_count][modes_top];

static void
set_charset(struct charset *cp)
{
  cset_index[cp->lcode][cp->mcode] = cp;
}

struct charset *
get_charset(enum langcode lcode, enum t_modes mcode)
{
  return cset_index[lcode][mcode];
}

static void
free_charset(struct charset *cp)
{
  if (cp->asc)
    cp->asc(NULL,0);
  if (cp->u2u)
    cp->u2u(NULL,0);
  if (cp->to_uni)
    inctrie_free(cp->to_uni);
}

void
charsets_init(void)
{
  set_charset(&akk_g);
  set_charset(&akk_n);
  set_charset(&arc_a);
  set_charset(&elx_g);
  set_charset(&grc_a);
  set_charset(&hit_g);
  set_charset(&peo_g);
  set_charset(&peo_n);
  set_charset(&qcu_g);
  set_charset(&qcu_n);
  set_charset(&qeb_g);
  set_charset(&qeb_n);
  set_charset(&qpc_g);
  set_charset(&qpe_g);
  set_charset(&qpn_g);
  set_charset(&sux_g);
  set_charset(&sux_n);
  set_charset(&xhu_g);
  set_charset(&uga_g);
  set_charset(&qse_n);
    subdig_init();
}

void
charsets_term(void)
{
  free_charset(&akk_g);
  free_charset(&akk_n);
  free_charset(&arc_a);
  free_charset(&elx_g);
  free_charset(&grc_a);
  free_charset(&hit_g);
  free_charset(&peo_g);
  free_charset(&peo_n);
  free_charset(&qcu_g);
  free_charset(&qcu_n);
  free_charset(&qeb_g);
  free_charset(&qeb_n);
  free_charset(&qpc_g);
  free_charset(&qpe_g);
  free_charset(&qpn_g);
  free_charset(&sux_g);
  free_charset(&sux_n);
  free_charset(&xhu_g);
  free_charset(&uga_g);
  free_charset(&qse_n);
    subdig_term();
}

void
chartrie_init(struct charset*cp)
{
  int i;
  cp->to_uni = inctrie_create();
  for (i = 0; cp->keys[i].key; ++i)
    inctrie_insert(cp->to_uni,cp->keys[i].key,cp->keys[i].val,0);
}

void
chartrie_not(const char *str, const char *sofar,struct charset *cp,
	     const char *f, size_t l)
{
    if (!isspace(*sofar) && !chartrie_suppress_errors)
	vwarning2(f,l,"%s: character '%c' (hex %x) not allowed in %s/%s",sofar,*sofar,*sofar,cp->lang,cp->mode);
}

void
charset_init_lang(struct lang_context *lp)
{
  enum t_modes i;
  for (i = 0; i < modes_top; ++i)
    {
      struct charset *cp = get_charset(lp->core->code,i);
      if (cp)
	{
	  if (cp->keys)
	    chartrie_init(cp);
	  lp->cset = cp;
	}
      else
        {
	    /* FIXME: we can't issue a diagnostic here, but we should
                when a lang/mode is actually used */
	  /*cp = get_default_charset();*/
	}
    }
}

