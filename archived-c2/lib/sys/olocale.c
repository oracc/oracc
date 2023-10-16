#include <oracclocale.h>

int
olocale(void)
{
  if (!setlocale(LC_ALL,ORACC_LOCALE))
    if (!setlocale(LC_ALL, "en_US.UTF-8"))
      if (!setlocale(LC_ALL, "C"))
        return 1;
  return 0;
}
