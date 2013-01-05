#define	ISO10646_BAD		0xFFFE

/*
 This routine comes from the transiso distribution, with minor mods to work
 as an independent inline function.

 The original is:

 Copyright (C) 1993-1995 by Kosta Kostis - this is freeware!
 Written by kosta@live.robin.de (Kosta Kostis)

 One bug fix by stinney@sas.upenn.edu (see comment below).
 */

static inline unsigned short
UTF2Unicode (unsigned const char *utf)
{
  unsigned short value = 0 ;

  if (utf [0] < 0x80)
    return ((unsigned short) *utf) ;
  
  if (utf [0] < 0xC0)
    return (ISO10646_BAD) ;
  
  if ((utf [1] < 0x80) || (utf [1] >= 0xC0))
    return (ISO10646_BAD) ;
  
  if (utf [0] < 0xE0)
    {
      if ((utf [1] < 0x80) || (utf [1] >= 0xC0))
	return (ISO10646_BAD) ;
      
      value  = (unsigned short) ((utf [0] & 0x1F) << 6) ; /* orig <<5 -- sjt */
      value += (unsigned short) (utf [1] & 0x3F) ;
      
      return (value);
    }
  
  if ((utf [2] < 0x80) || (utf [2] >= 0xC0))
    return (ISO10646_BAD) ;
  
  value  = (unsigned short) ((utf [0] & 0x0F) << 12) ;
  value += (unsigned short) ((utf [1] & 0x3F) << 6) ;
  value += (unsigned short) (utf [2] & 0x3F) ;
  
  return (value) ;
}

