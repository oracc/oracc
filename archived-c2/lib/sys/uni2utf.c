#define MAX_UTF_LENGTH		4
unsigned char *
Unicode2UTF (unsigned short ch)
{
  static unsigned char utf [MAX_UTF_LENGTH + 1] ;
  int i ;

  for (i = 0 ; i <= MAX_UTF_LENGTH ; ++i)
    utf [i] = '\0' ;

  if (ch < 0x0080)
    {
      utf [0] = (unsigned char) ch ;
    }
  else
    {
      if (ch < 0x0800)
	{
	  utf [0] = 0xC0 | ((ch & 0x07C0) >> 6) ;
	  utf [1] = 0x80 |  (ch & 0x3F) ;
	}
      else
	{
	  utf [0] = 0xE0 | ((ch & 0xF000) >> 12) ;
	  utf [1] = 0x80 | ((ch & 0x0FC0) >> 6) ;
	  utf [2] = 0x80 |  (ch & 0x3F) ;
	}
    }
  
  return utf;
}
#undef MAX_UTF_LENGTH
