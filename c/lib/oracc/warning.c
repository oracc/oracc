#include <stdio.h>
#include <stdarg.h>

#define NEW_ERROR_RECOVERY

char *file = NULL;
int atf_cbd_err;
int cbd_err_line = 0;
int exit_status = 0;
int lnum = 1;
int status = 0;
int lstatus = 0;
int with_textid = 1;
FILE*f_log;

int nwarning = 0;
int nnotice = 0;

int use_msglist = 0;

const char *phase = NULL;
const char *textid = NULL;

void
warning_init(void)
{
  f_log = stderr;
}

void
warning_msglist(void)
{
  use_msglist = 1;
}

void
vwarning(const char *fmt,...)
{
  va_list args;
#ifndef NEW_ERROR_RECOVERY
  if (!lstatus++)
    {
#endif
      if (use_msglist)
	{
	  va_start(args, fmt);
	  msglist_vwarning(file, atf_cbd_err ? cbd_err_line : lnum, fmt, args);
	  va_end(args);
	}
      else
	{
	  va_start(args,fmt);
	  if (atf_cbd_err)
	    fprintf(f_log,"%d: ",cbd_err_line);
	  else if (with_textid)
	    fprintf(f_log,"%s:%d:%s: ",file ? file : "",lnum,textid ? textid : textid);
	  else
	    fprintf(f_log,"%s:%d: ",file,lnum);
	  if (phase)
	    fprintf(f_log,"(%s) ", phase);
	  (void)vfprintf(f_log,fmt,args);
	  va_end(args);
	  (void)fputc('\n',f_log);
	}
      ++nwarning;
#ifndef NEW_ERROR_RECOVERY
    }
#endif
  ++status;
}

void
vwarning2(const char *myfile, int mylnum, const char *fmt,...)
{
  va_list args;
  if (!f_log)
    f_log = stderr;
#ifndef NEW_ERROR_RECOVERY
  if (!lstatus++)
    {
#endif
      va_start(args,fmt);
      if (atf_cbd_err)
	fprintf(f_log,"%d: ",cbd_err_line);
      else if (with_textid)
	fprintf(f_log,"%s:%d:%s: ",myfile,mylnum,textid);
      else
	fprintf(f_log,"%s:%d: ",myfile,mylnum);
      if (phase)
	fprintf(f_log,"(%s) ", phase);
      (void)vfprintf(f_log,fmt,args);
      va_end(args);
      (void)fputc('\n',f_log);
      ++nwarning;
#ifndef NEW_ERROR_RECOVERY
    }
#endif
  ++status;
}

void
warning(const char *str)
{    
#ifndef NEW_ERROR_RECOVERY
  if (!lstatus++)
    {
#endif
      if (use_msglist)
	{
	  msglist_warning(file, atf_cbd_err ? cbd_err_line : lnum, str);
	}
      else
	{
	  if (atf_cbd_err)
	    fprintf(f_log,"%d: ",cbd_err_line);
	  else if (with_textid)
	    fprintf(f_log,"%s:%d:%s: ",file,lnum,textid);
	  else
	    fprintf(f_log,"%s:%d: ",file,lnum);
	  
	  if (phase)
	    fprintf(f_log,"(%s) ", phase);
	  
	  fprintf(f_log, "%s\n", str);
	}
      ++nwarning;

#ifndef NEW_ERROR_RECOVERY
    }
#endif
      
  ++status;
}

void
notice(const char *str)
{
#ifndef NEW_ERROR_RECOVERY
  if (!lstatus++)
    {
#endif
      if (atf_cbd_err)
	fprintf(f_log,"%d: ",cbd_err_line);
      else if (with_textid)
	fprintf(f_log,"%s:%d:%s: %s\n",file,lnum,textid,str);
      else
	fprintf(f_log,"%s:%d: %s\n",file,lnum,str);
#ifndef NEW_ERROR_RECOVERY
    }
#endif
  /*  ++status; */
}

void
notice2(const char *myfile, int mylnum, const char *str)
{
  if (!lstatus++)
    {
      if (with_textid)
	fprintf(f_log,"%s:%d:%s: %s\n",myfile,mylnum,textid,str);
      else
	fprintf(f_log,"%s:%d: %s\n",myfile,mylnum,str);
      ++nnotice;
    }
  /*  ++status; */

}

void
vnotice(const char *fmt,...)
{
  va_list args;
#ifndef NEW_ERROR_RECOVERY
  if (!lstatus++)
    {
#endif
      va_start(args,fmt);
      if (atf_cbd_err)
	fprintf(f_log,"%d: ",cbd_err_line);
      else if (with_textid)
	fprintf(f_log,"%s:%d:%s: ",file ? file : "",lnum,textid ? textid : "");
      else
	fprintf(f_log,"%s:%d: ",file,lnum);
      if (phase)
	fprintf(f_log,"(%s) ", phase);
      (void)vfprintf(f_log,fmt,args);
      va_end(args);
      (void)fputc('\n',f_log);
      ++nnotice;
#ifndef NEW_ERROR_RECOVERY
    }
#endif
}

void
vnotice2(const char *myfile, int mylnum, const char *fmt,...)
{
  va_list args;
#ifndef NEW_ERROR_RECOVERY
  if (!lstatus++)
    {
#endif
      va_start(args,fmt);
      if (atf_cbd_err)
	fprintf(f_log,"%d: ",cbd_err_line);
      else if (with_textid)
	fprintf(f_log,"%s:%d:%s: ",myfile,mylnum,textid);
      else
	fprintf(f_log,"%s:%d: ",myfile,mylnum);
      if (phase)
	fprintf(f_log,"(%s) ", phase);
      (void)vfprintf(f_log,fmt,args);
      va_end(args);
      (void)fputc('\n',f_log);
      ++nnotice;
#ifndef NEW_ERROR_RECOVERY
    }
#endif
}

void
print_error_count(FILE *fp, int force)
{
  extern const char *prog;
  if (nwarning || nnotice || force)
    fprintf(fp, "ATF processor %s issued %d warnings and %d notices\n",
	    prog, nwarning, nnotice);
}
