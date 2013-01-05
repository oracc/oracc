#ifndef CDLIDATE_H_
#define CDLIDATE_H_

/* CDLI dates have the following structure:
   
   DYNASTY.RULER.YEAR.MONTH.DAY

   All numbers are arabic digits.  Some late
   dates use a double-part DYNASTY; we simply
   treat that as if it were single part 
   dynasty.

 */
struct cdlidate
{
  char *iso_base;
  char *period;
  char *dynasty;
  char *ruler;
  char year[6];
  char month[4];
  char day[4];
};

extern void cdlidate_init(void);
extern void cdlidate_term(void);
extern int cdlidate_parse(struct cdlidate*dp, const char *date);

#endif/*CDLIDATE_H_*/
