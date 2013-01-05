/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: file_io.c,v 0.4 1998/12/19 06:02:43 s Exp $
*/

#include <psd_base.h>
#include <file_io.h>
#include <io_mac.h>
#include <list.h>

File *curr_file = NULL;
static List *file_stack_base = NULL;
static List *file_name_list = NULL;

File *
file_new ()
{
  File *tmp = malloc (sizeof (File));
  tmp->name = NULL;
  tmp->index = 0;
  tmp->line = 0L;
  tmp->bufp = tmp->buffer = tmp->unput_buffer = NULL;
  tmp->buf_used = tmp->buf_allocated = tmp->unput_used = tmp->unput_allocated = 0;
  tmp->buf_flag = tmp->stdio_flag = FALSE;
  return tmp;
}

File *
file_push (File *fp)
{
  if (NULL == file_stack_base)
    file_stack_base = list_create (LIST_LIFO);
  list_add (file_stack_base, fp);
  return fp;
}

File *
file_pop ()
{
  File *old = list_pop (file_stack_base);

  if (FALSE == old->stdio_flag)
    xfclose (old->name, old->fp);
  if (curr_file->unput_allocated)
    free (curr_file->unput_buffer);
  if (curr_file->buf_allocated)
    free (curr_file->buffer);
  free (old);

  if (0 == file_stack_base->count)
    {
      free (file_stack_base);
      file_stack_base = NULL;
      return NULL;
    }
  else
    return file_stack_base->last->data;
}

void
file_add_name (char *name)
{
  if (NULL == file_name_list)
    file_name_list = list_create (LIST_DOUBLE);
  list_add (file_name_list, name);
}

void
file_free_names ()
{
  list_free (file_name_list, list_xfree);
  file_name_list = NULL;
}

int
file_num ()
{
  return file_stack_base->count;
}

void
file_open (const char *name, const char *mode)
{
  File *new;
  FILE *tmp_fp;

  if (NULL == name)
    error (NULL, "attempt to open NULL filename");

  if (*name)
    {
      tmp_fp = xfopen (name, mode);
      if (NULL == tmp_fp)
        return;	/* leave input stack in consistent state if open fails
		 * and exit_on_error is switched off
		 */
      new = file_new ();
      new->fp = tmp_fp;
      new->stdio_flag = FALSE;
      new->name = xstrdup (name);
      file_add_name (new->name);
    }
  else
    {
      register const char *mp = mode;
      new = file_new ();
      while (*mp)
        if ('r' == *mp)
	  break;
      new->name = "-";
      if (*mp)
        new->fp = stdin;
      else
        new->fp = stdout;
      new->stdio_flag = TRUE;
#if 0
#if defined(MSDOS) | defined(OS2)
      mp = mode;
      while (*mp)
        if ('b' == *mp)
	  break;
      if (*mp)
        xsetmode (fileno(stdin), O_BINARY);
      else
        xsetmode (fileno(stdin), O_TEXT);
#endif
#endif
    }
  curr_file = file_push (new);
}

Boolean
file_close ()
{
  curr_file = file_pop ();
  return curr_file == NULL ? FALSE : TRUE;
}

Uchar
slow_unput (int ch)
{
  if (NULL == curr_file->unput_buffer)
    {
      curr_file->unput_allocated = 128;
      curr_file->unput_buffer = malloc (curr_file->unput_allocated * sizeof (Uchar));
      curr_file->unput_used = 0;
    }
  else
    {
      curr_file->unput_allocated *= 2;
      curr_file->unput_buffer = realloc (curr_file->unput_buffer, curr_file->unput_allocated * sizeof (Uchar));
    }
  return curr_file->unput_buffer[curr_file->unput_used++] = (Uchar) ch;
}

void
unputs (Uchar *s)
{
  if (NULL != s && *s)
    {
      Uchar *end = s + strlen((const char*)s);
      while (end > s)
        unput (*--end);
    }
}
