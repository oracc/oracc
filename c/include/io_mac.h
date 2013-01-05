/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: io_mac.h,v 0.3 1997/09/08 14:49:57 sjt Exp $
*/

#ifndef IO_MAC_H_
#define IO_MAC_H_ 1
#define input()	(FALSE == curr_file->buf_flag \
		 ? (curr_file->unput_used \
		    ? curr_file->unput_buffer[--curr_file->unput_used] \
		    : getc(curr_file->fp)) \
		 : (curr_file->unput_used \
		    ? curr_file->unput_buffer[--curr_file->unput_used] \
		    : *curr_file->bufp++ || *--curr_file->bufp))

#define unput(ch) \
		(curr_file->unput_used < curr_file->unput_allocated \
		 ? curr_file->unput_buffer[curr_file->unput_used++] = (Uchar)ch \
		 : slow_unput(ch))

#endif /* IO_MAC_H_ */
