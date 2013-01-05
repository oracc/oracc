#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "npool.h"
#include "cdlidate.h"

static struct npool *cdlidate_pool;
static int iso_sub(const char *base, const char *year);

void
cdlidate_init(void)
{
  cdlidate_pool = npool_init();
}

void
cdlidate_term(void)
{
  npool_term(cdlidate_pool);
}

/* Return 1 on failure, 0 on success */
int
cdlidate_parse(struct cdlidate*dp, const char *date)
{
  char *dest, *src, *end, *dot, *copy;

  if (!dp || !date)
    return 1;

  copy = malloc(strlen(date)+1);
  strcpy(copy,date);

  end = copy + strlen(copy);
  if (!(dot = strrchr(end,'.')))
    return 1;
  src = dot + 1;
  dest = dp->day;
  while (src < end)
    {
      if (isdigit(*src))
	*dest++ = *src++;
      else
	++src;
    }
  if (dest - dp->day < 2)
    {
      *dest++ = dp->day[0];
      dp->day[0] = '0';
    }
  if (!strcmp(dp->day, "00"))
    dest = dp->day;
  *dest = '\0';

  end = dot;
  if (!(dot = strrchr(end,'.')))
    return 1;
  src = dot + 1;
  dest = dp->month;
  while (src < end)
    {
      if (isdigit(*src))
	*dest++ = *src++;
      else
	++src;
    }
  if (dest - dp->month < 2)
    {
      *dest++ = dp->month[0];
      dp->month[0] = '0';
    }
  if (!strcmp(dp->month, "00"))
    dest = dp->month;
  *dest = '\0';

  end = dot;
  if (!(dot = strrchr(end,'.')))
    return 1;
  src = dot + 1;
  dest = dp->year;
  while (src < end)
    {
      if (isdigit(*src))
	*dest++ = *src++;
      else
	++src;
    }
  if (!strcmp(dp->year, "00"))
    dest = dp->year;
  *dest = '\0';

  end = dot;
  if (!(dot = strrchr(end,'.')))
    return 1;
  *end = '\0';
  dp->ruler = (char*)npool_copy((unsigned char*)dot+1,cdlidate_pool);
  
  *dot = '\0';
  dp->dynasty = (char*)npool_copy((unsigned char*)copy,cdlidate_pool);
  
  free(copy);

  return 0;
}

char *
cdlidate_w3c(struct cdlidate*dp)
{
  static char buf[128];
  char *ret = NULL;
  if (dp->iso_base)
    {
      if (*dp->year)
	sprintf(buf, "%04d", iso_sub(dp->iso_base, dp->year));
      else
	strcpy(buf, dp->iso_base);
      if (*dp->month)
	{
	  sprintf(buf+strlen(buf),"-%s",dp->month);
	  if (*dp->day)
	    sprintf(buf+strlen(buf),"-%s",dp->day);
	  else
	    strcat(buf,"-00");
	}
      else 
	{
	  if (dp->day)
	    sprintf(buf+strlen(buf),"-00-%s",dp->day);
	  else
	    strcat(buf,"-00-00");
	}
    }
  return ret;
}

static int
iso_sub(const char *base, const char *year)
{
  int b, y;
  b = atoi(base);
  y = atoi(year);
  if (b < 0) /* BC */
    return b - y;
  else
    return b + y;
}
