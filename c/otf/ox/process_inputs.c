#include <stdlib.h>
#include <string.h>
#include <ctype128.h>
#include <fname.h>
#include "loadfile.h"
#include "c1_list.h"
#include "warning.h"
#include "atffile.h"
#include "globals.h"
#include "cdt.h"
#include "run.h"
#include "ox.h"
extern int optind;
extern int use_legacy;

#define ucc(x) ((unsigned const char*)x)
char *input_file;
static int began_file = 0;
static int files_processed = 0;
static void process_atf(struct run_context *run);
static void process_either(struct run_context *run, unsigned const char *input);
static void process_otf(struct run_context *run);

void
process_inputs(struct run_context *run, int argc, char **argv)
{
  if (inputs_from_file)
    {
      unsigned char **files = NULL, *fmem;
      size_t nfiles = 0, i;
      files = loadfile_lines3(ucc(inputs_from_file),&nfiles,&fmem);
      for (i = 0; i < nfiles; ++i)
	{
	  char *s = NULL;
	  for (s = (char*)files[i]; *s; ++s)
	    if (!isspace(*s))
	      break;
	  if (*s)
	    {
	      input_file = s;
	      if (verbose)
		fprintf(stderr,"%s\n",input_file);
	      process_either(run, (unsigned const char*)input_file);
	      atffile = cdtfile = NULL;
	    }
	}
    }
  else
    {
      if (!atffile && !cdtfile)
	{
	  while (optind < argc)
	    {
	      process_either(run, (unsigned const char*)argv[optind++]);
	      atffile = cdtfile = NULL;
	    }
	}
      else
	{
	  process_either(run, (unsigned const char*)input_file);
	}
    }

  if (!files_processed && !rncfile)
    {
      fprintf(stderr,"ox: nothing to do\n");
      exit_status = 2;
      return;
    }
  else if (began_file)
    {
      began_file = 0;
      end_file();
    }
}

static void
process_either(struct run_context *run, unsigned const char *input)
{
  const char *ext = NULL;
  if (!atffile && !cdtfile)
    {
      ext = strrchr((char*)input, '.');
      if (ext)
	{
	  if (force_atf_extension || !strcmp(ext, ".atf") || !strcmp(ext, ".catf"))
	    atffile = (const char*)input;
	  else if (force_otf_extension || !strcmp(ext, ".otf"))
	    cdtfile = (const char*)input;
	  else if (!strcmp(ext, ".ods"))
	    atffile = (const char*)input;
	  else
	    fprintf(stderr,"ox: file must end in .atf or .otf, or use -a or -o options (found '%s')\n", ext);
	}
    }

  if (verbose)
    fprintf(stderr,"%s\n",input);

  if (logfile)
    {
      if (*logfile == '=')
	logfile = new_ext_no_alloc((char*)npool_copy(input,run->pool),".log");
      if (f_log)
	fclose(f_log);
      f_log = fopen(logfile,"w");
      if (!f_log)
	{
	  fprintf(stderr,"ox: unable to write to log file %s\n", logfile);
	  exit(2);
	}
    }

  if (xmlfile)
    {
      if (!strcmp(xmlfile,"="))
	xmlfile = new_ext_no_alloc((char*)npool_copy(input,run->pool),".xml");
      f_xml = fopen(xmlfile,"w");
      if (!f_xml)
	{
	  fprintf(stderr,"ox: unable to write to xml file %s\n",xmlfile);
	  exit(2);
	}
      if (setvbuf(f_xml,NULL,_IOFBF,32768))
	{
	  fprintf(stderr,"setvbuf failed\n");
	  exit(2);
	}
    }
  else
    f_xml = stdout;

  if (atffile)
    {
      ++files_processed;
      process_atf(run);
    }
  else if (cdtfile)
    {
      ++files_processed;
      use_legacy = 1;
      process_otf(run);
    }
}

static void
process_atf(struct run_context *run)
{
  if (!began_file)
    {
      began_file = 1;
      begin_file();
    }
  if (process_file(run, atffile))
    exit_status = 2;
}

static void
process_otf(struct run_context *run)
{
  cdt_run = run;

  cdt_master_list = list_create(LIST_SINGLE);
  cdt_meta_list = list_create(LIST_SINGLE);
  cdt_styles_list = list_create(LIST_SINGLE);
  cdt_reader_init(); /* ADD _term() to initialize curr_section List */
  cdt_styles_init();

  if (verbose)
    fprintf(stderr,"%s\n",cdtfile);
  if (process_cdt_input(run,cdtfile,cdt_master_list,cdt_meta_list,NULL))
    exit_status = 2;
  if (!check_only)
    cdt_writer(stdout,cdt_master_list,cdt_meta_list);

  cdt_reader_term();
  cdt_styles_term();
  list_free(cdt_master_list, NULL);
  list_free(cdt_meta_list, NULL);
  list_free(cdt_styles_list, NULL);
}
