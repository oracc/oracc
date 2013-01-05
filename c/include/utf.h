/******************************************************************************
 *                                                                            *
 * File:   utf     .h         Version  1.01            Date: 05-MAY-1994      *
 *                                                                            *
 * Copyright (C) 1993-1995 by Kosta Kostis - this is freeware!                *
 * Written by kosta@live.robin.de (Kosta Kostis)                              *
 *                                                                            *
 ******************************************************************************
 *                                                                            *
 * History:                                                                   *
 *    05-MAY-1994: KK V1.01                                                   *
 *        - initial coding                                                    *
 *                                                                            *
 *****************************************************************************/


/******************************************************************************
									      
	Function Constants

 *****************************************************************************/

#define MAX_UTF_LENGTH		3

#define	ISO10646_BAD		0xFFFE
 
/******************************************************************************
									      
	Function Prototypes

 *****************************************************************************/

extern  unsigned char   *Unicode2UTF (unsigned short ch) ;
extern	unsigned short	UTF2Unicode (unsigned char *utf) ;
