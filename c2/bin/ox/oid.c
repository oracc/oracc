#include <stdio.h>
#include <oraccsys.h>
#include <mesg.h>
#include <oid.h>

int oo_assign = 0;
int oo_check = 0;
const char *oo_domain = NULL;
const char *oo_edits = NULL;
int oo_identity = 0;
const char *oo_keyfile = NULL;
int oo_nowrite = 0;
const char *oo_oidfile = NULL;
const char *oo_outfile = NULL;
int oo_tsv_mode = 0;
const char *oo_project = NULL;
int oo_wants = 0;
extern int oo_xids;

FILE *oo_out_fp = NULL;

char tbuf[16384];

int
main(int argc, char * const*argv)
{
  Oids *o = NULL, *k = NULL;
  Oide *e = NULL;

  mesg_init();

  options(argc, argv, "acd:e:f:ik:no:p:tvwx");

  if (oo_outfile)
    {
      if (!(oo_out_fp = fopen(oo_outfile, "w")))
	fprintf(stderr, "oid: unable to write to outfile %s. Stop.\n", oo_outfile);
    }
  else
    oo_out_fp = stdout;

  if (oo_oidfile)
    oid_set_oidtab(oo_oidfile);
  else if (oo_xids)
    oid_set_oidtab("xid");

  if (oo_edits)
    {
      e = oid_load_edits(oo_edits);
      oid_parse_edits(e);
      if (oo_identity)
	{
	  oid_write_edits(stdout,e);
	  /* don't output edits and oid in a single run */
	  oo_identity = 0;
	}
    }
  
  if (oo_assign)
    {
      struct oid_domain *dp = NULL;
      if (oo_domain)
	{
	  if (!(dp = oid_domain(oo_domain, strlen(oo_domain))))
	    {
	      fprintf(stderr, "oid: unknown domain %s. Stop.\n", oo_domain);
	      exit(1);
	    }
	}
      else
	{
	  fprintf(stderr, "oid: must give -d [DOMAIN] with -a [ASSIGN] argument. Stop.\n");
	  exit(1);
	}
      if (oo_project)
	{
	  if (strcmp(oo_project, dp->auth))
	    {
	      fprintf(stderr, "oid: project %s does not authority to assign OIDs in domain %s\n",
		      oo_project, oo_domain);
	      exit(1);
	    }
	}
      else
	{
	  fprintf(stderr, "oid: must give -p [PROJECT] with -a [ASSIGN] argument. Stop.\n");
	  exit(1);
	}
    }

  if (oo_assign || oo_wants)
    {
      if (oo_keyfile)
	k = oid_load_keys(oo_keyfile);
      else
	k = oid_load_keys("-");
      if (oid_parse(k, ot_keys))
	{
	  mesg_print(stderr);
	  fprintf(stderr, "oid: parse errors in %s. Stop.\n", o->file);
	  exit(1);
	}
    }
  
  o = oid_load();

  if (oid_parse(o, ot_oids))
    {
      mesg_print(stderr);      
      fprintf(stderr, "oid: parse errors in %s. Stop.\n", o->file);
      exit(1);
    }
  else
    {
      if (oo_check)
	{
	  if (oo_verbose)
	    {
	      fprintf(stderr, "oid: next free OID is %s\n", oid_next_oid(o));
	      fprintf(stderr, "oid: %s parsed OK\n", o->file);
	    }
	}
      else if (oo_identity)
	oid_write(oo_out_fp, o);
      else if (oo_assign || oo_wants)
	{
	  List *w = oid_wants(o,k);
	  if (oo_wants)
	    {
	      struct oid *op;
	      for (op = list_first(w); op; op = list_next(w))
		fprintf(oo_out_fp, "%s\t%s\t%s\t%s\n",
			op->domain, op->key, op->type, op->extension ? op->extension : "");
	    }
	  else
	    {
	      List *w = oid_assign(o, k);
	      if (list_len(w))
		{
		  struct oid *op;
		  for (op = list_first(w); op; op = list_next(w))
		    fprintf(oo_out_fp, "add %s\t%s\t%s\t%s => %s\n",
			    op->domain, op->key, op->type, op->extension ? op->extension : "", op->id);
		}
	      if (!oo_nowrite)
		oid_write(oo_out_fp, o);
	    }
	}
      else if (oo_tsv_mode)
	{
	  char *l;
	  
	  while ((l = fgets(tbuf, 16384, stdin)))
	    {
	      if ('\n' == l[strlen(l)-1])
		{
		  char tab = '\0';
		  char *t = l;
		  struct oid *op;
		  while (*t && '\t' != *t)
		    ++t;
		  if (t)
		    {
		      tab = '\t';
		      *t = '\0';
		    }
		  if ((op = hash_find(o->h, (uccp)oid_domainify(oo_domain, l))))
		    fputs(op->id, stdout);
		  else
		    fprintf(stderr, "oid: key %s not found in domain %s\n", l, oo_domain);
		  if (tab)
		    *t = tab;
		  fputc('\t', stdout);
		}
	      else
		fprintf(stderr, "overlong line\n");
	      fputs(l, stdout);
	    }
	}
	       
    }
}

int
opts(int opt, char *arg)
{
  switch (opt)
    {
    case 'a':
      oo_assign = 1;
      break;
    case 'c':
      oo_check = 1;
      break;
    case 'd':
      oo_domain = arg;
      break;      
    case 'e':
      oo_edits = arg;
      break;
    case 'i':
      oo_identity = 1; /* replaces -dump in oid.plx */
      break;
    case 'f':
      oo_outfile = arg; /* replaces -output in oid.plx (f-for-file) */
      break;
    case 'k':
      oo_keyfile = arg;
      break;
    case 'n':
      oo_nowrite = 1;
    case 'o':
      oo_oidfile = arg;
      break;
    case 'p':
      oo_project = arg;
      break;
    case 't':
      oo_tsv_mode = 1; /* read tsv and prepend an OID column */
      break;
    case 'v':
      ++oo_verbose;
      break;
    case 'w':
      oo_wants = 1;
      break;
    case 'x':
      oo_xids = 1;
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

void
help(void)
{
  fprintf(stderr, "ox: oid processor\n");
}

const char *prog = "oid";
