/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: file_io.h,v 0.3 1997/09/08 14:49:56 sjt Exp $
*/

#ifndef FILE_IO_H_
#define FILE_IO_H_ 	1

struct File
{ 
  FILE*	fp;
  /*const*/ char *	name;
  Unsigned32 	line;
  Uchar *	buffer;
  const Uchar *	bufp;
  size_t	buf_used;
  size_t	buf_allocated;
  Uchar *	unput_buffer;
  size_t	unput_used;
  size_t	unput_allocated;
  Boolean	stdio_flag;
  Boolean	buf_flag;
  int		index;
};
typedef struct File File;

extern File *curr_file;

extern void	buf_open (const Uchar *buf, const char *name, long line);
extern Boolean	buf_close (void);
extern void	file_open (const char *name, const char *mode);
extern Boolean	file_close (void);
extern Uchar	slow_unput (int ch);
extern void	unputs (Uchar *s);
extern File *	file_new (void);
extern File *	file_push (File *fp);
extern File *	file_pop (void);
extern void	file_add_name (char *name);
extern void	file_free_names (void);
extern int	file_num (void);
extern Uchar *	file_line (void);
#endif /* FILE_IO_H_ */
