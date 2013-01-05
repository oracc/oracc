#include <stdio.h>
struct tab
{
  const char *str;
  int f;
} tab[] = { 
  { "F2_FLAGS_PSU_STOP    ", 0x000001 }, 
  { "F2_FLAGS_PSU_SKIP    ", 0x000002 }, 
  { "F2_FLAGS_LEM_NEW     ", 0x000004 }, 
  { "F2_FLAGS_SHADOW      ", 0x000008 }, 
  { "F2_FLAGS_COF_NEW     ", 0x000010 }, 
  { "F2_FLAGS_INVALID     ", 0x000020 }, 
  { "F2_FLAGS_CF_QUOTED   ", 0x000040 }, 
  { "F2_FLAGS_NO_FORM     ", 0x000080 }, 
  { "F2_FLAGS_PARTIAL     ", 0x000100 }, 
  { "F2_FLAGS_SAME_REF    ", 0x000200 }, 
  { "F2_FLAGS_COF_HEAD    ", 0x000400 }, 
  { "F2_FLAGS_COF_TAIL    ", 0x000800 }, 
  { "F2_FLAGS_IS_PSU      ", 0x001000 }, 
  { "F2_FLAGS_NGRAM_MATCH ", 0x002000 }, 
  { "F2_FLAGS_NGRAM_KEEP  ", 0x004000 }, 
  { "F2_FLAGS_LEM_BY_NORM ", 0x008000 }, 
  { "F2_FLAGS_NORM_IS_CF  ", 0x010000 }, 
  { "F2_FLAGS_NEW_BY_PROJ ", 0x020000 }, 
  { "F2_FLAGS_NOT_IN_SIGS ", 0x040000 }, 
  { "F2_FLAGS_EXTENDED_OK ", 0x080000 }, 
  { "F2_FLAGS_NEW_BY_LANG ", 0x100000 }, 
  { "F2_FLAGS_COF_INVALID ", 0x200000 }, 
  { "F2_FLAGS_FROM_CACHE  ", 0x400000 }, 
  { 0, -1 }, 
  };

int 
main(int argc, char **argv)
{
  int i;
  int arg = atoi(argv[1]);
  for (i = 0; tab[i].str; ++i)
    if (arg & tab[i].f)
      printf("%s\n", tab[i].str);
  return 0;
}
