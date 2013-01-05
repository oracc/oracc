/******************************************************************************
 *                                                                            *
 * File:   utf     .c         Version  1.01            Date: 05-MAY-1994      *
 *                                                                            *
 * Copyright (C) 1993-1995 by Kosta Kostis - this is freeware!                *
 * Written by kosta@live.robin.de (Kosta Kostis)                              *
 *                                                                            *
 ******************************************************************************
 *                                                                            *
 * History:                                                                   *
 *    05-MAY-1994: KK V1.01                                                   *
 *        - some compilers still don't understand "//" comments               *
 *    09-JUL-1993: KK V1.00                                                   *
 *        - initial coding                                                    *
 *                                                                            *
 *****************************************************************************/

#include "utf.h"

/******************************************************************************

	Function:
		return UTF representation of a Unicode character

	Parameters:
		unsigned short	ch		character to be represented

	Returns:
		unsigned char	*		pointer to UTF-string

******************************************************************************/

unsigned char	*Unicode2UTF
(
	unsigned short	ch
)
{
	static	unsigned char	utf	[MAX_UTF_LENGTH + 1] ;
	int	i ;

	/**********************************************************************

		initialize UTF-string (all empty)

	**********************************************************************/

	for (i = 0 ; i <= MAX_UTF_LENGTH ; ++i)
		utf [i] = '\0' ;

	/**********************************************************************

		UTF is a one..three byte string

		0000 0000 : 0bbb bbbb -> 0bbb bbbb
		0000 0bbb : bbaa aaaa -> 110b bbbb : 10aa aaaa
		cccc bbbb : bbaa aaaa -> 1110 cccc : 10bb bbbb : 10aa aaaa

	**********************************************************************/

				/*	case ch <= 0000 0000 : 0bbb bbbb     */
	if (ch < 0x0080)
	{
				/*	US ASCII compatibility               */
		utf [0] = (unsigned char) ch ;
	}
	else
	{
				/*	case ch <= 0000 0bbb : bbbb bbbb     */
		if (ch < 0x0800)
		{
			utf [0] = 0xC0 | ((ch & 0x07C0) >> 6) ;
			utf [1] = 0x80 |  (ch & 0x3F) ;
		}
				/*	case ch >  0000 0bbb : bbbb bbbb     */
		else
		{
			utf [0] = 0xE0 | ((ch & 0xF000) >> 12) ;
			utf [1] = 0x80 | ((ch & 0x0FC0) >> 6) ;
			utf [2] = 0x80 |  (ch & 0x3F) ;
		}
	}

	return (utf) ;
}

/******************************************************************************

	Function:
		convert UTF representation of a character to Unicode

	Parameters:
		unsigned char	*utf		UTF encoded character

	Returns:
		ISO10646_BAD		illegal value
		unsigned short			value to be converted to Unicode

******************************************************************************/

unsigned short	UTF2Unicode
(
	unsigned char	*utf
)
{
	unsigned short	value	= 0 ;

	/**********************************************************************

		first check values not greater than 0000 0000 : 0bbb bbbb

	**********************************************************************/

	if (utf [0] < 0x80)
		return ((unsigned short) *utf) ;

	/**********************************************************************

		now check values not greater than   0000 0bbb : bbaa aaaa

	**********************************************************************/

	if (utf [0] < 0xC0)
		return (ISO10646_BAD) ;

	if ((utf [1] < 0x80) || (utf [1] >= 0xC0))
		return (ISO10646_BAD) ;

	if (utf [0] < 0xE0)
	{
		if ((utf [1] < 0x80) || (utf [1] >= 0xC0))
			return (ISO10646_BAD) ;

		value  = (unsigned short) ((utf [0] & 0x1F) << 6) ;
		value += (unsigned short) (utf [1] & 0x3F) ;

		return (value) ;
	}

	/**********************************************************************

		now there should be a value greater than 0000 0bbb : bbaa aaaa

	**********************************************************************/

	if ((utf [2] < 0x80) || (utf [2] >= 0xC0))
		return (ISO10646_BAD) ;

	value  = (unsigned short) ((utf [0] & 0x0F) << 12) ;
	value += (unsigned short) ((utf [1] & 0x3F) << 6) ;
	value += (unsigned short) (utf [2] & 0x3F) ;

	return (value) ;
}
