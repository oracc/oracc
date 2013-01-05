/*************************************************************************************************
 * The command line interface for the core API
 *                                                      Copyright (C) 2004-2007 Mikio Hirabayashi
 * This file is part of Hyper Estraier.
 * Hyper Estraier is free software; you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation; either
 * version 2.1 of the License or any later version.  Hyper Estraier is distributed in the hope
 * that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 * You should have received a copy of the GNU Lesser General Public License along with Hyper
 * Estraier; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA.
 *************************************************************************************************/


#include "estraier.h"
#include "myconf.h"
#include "mymorph.h"

#define NUMBUFSIZ      32                /* size of a buffer for a number */
#define URIBUFSIZ      8192              /* size of a buffer for an URI */
#define MINIBNUM       31                /* bucket number of a small map */
#define SEARCHMAX      10                /* maximum number of shown documents */
#define SEARCHAUX      32                /* minimum hits to adopt the auxiliary index */
#define SNIPWWIDTH     480               /* whole width of the snippet */
#define SNIPHWIDTH     96                /* width of beginning of the text */
#define SNIPAWIDTH     96                /* width around each highlighted word */
#define CACHEMAX       (1048576*1024)    /* maximum chache size */
#define MEMCHKFREQ     32                /* frequency of checking memory exhaust */
#define MEMCHKALW      (1048576*64)      /* allowance of checking memory exhaust */
#define MEMLOADMAX     0.9               /* max load ratio of the physical memory */
#define LIMITTSIZ      (1024*128)        /* text size limitation */
#define LIMITFSIZ      (1048576*32)      /* file size limitation */
#define DATTRLPATH     "_lpath"          /* name of the attribute of the URI of local path */
#define DATTRLREAL     "_lreal"          /* name of the attribute of the real path */
#define DATTRLFILE     "_lfile"          /* name of the attribute of the file name */
#define DATTRNDSCORE   "#nodescore"      /* name of the pseudo-attribute of score */
#define KWORDNUM       32                /* number of shown keywords */
#define RELWDDMAX      32                /* max number of documents to calculate related words */
#define RELWDNUM       32                /* number of shown related words */
#define RDOCSNUM       6                 /* number of sections of a random document */
#define RDOCCNUM       256               /* number of characters for int a section */

enum {                                   /* enumeration for viewing modes */
  VM_ID,                                 /* ID only */
  VM_URI,                                /* ID and URI */
  VM_ATTR,                               /* all attributes */
  VM_FULL,                               /* all attributes and body text */
  VM_SNIP,                               /* all attributes and snippet */
  VM_HMRD,                               /* human readable */
  VM_XML,                                /* XML */
  VM_DUMP                                /* dump draft files */
};

enum {                                   /* enumeration for file formats */
  FF_AUTO,                               /* automatic detection */
  FF_DRAFT,                              /* draft */
  FF_TEXT,                               /* plain text */
  FF_HTML,                               /* HTML */
  FF_MIME,                               /* MIME */
  FF_NONE                                /* ignored */
};

enum {                                   /* enumeration for file type */
  ST_ALL,                                /* all */
  ST_FILE,                               /* file only */
  ST_DIR                                 /* directory only */
};

enum {                                   /* enumeration for path format */
  SP_REL,                                /* relative path */
  SP_ABS,                                /* absolute path */
  SP_URL                                 /* URL */
};

enum {                                   /* enumeration for test documents */
  RD_ENG,                                /* English */
  RD_LAT,                                /* Latin */
  RD_EURO,                               /* European mix */
  RD_ORI,                                /* Oriental */
  RD_JPN,                                /* Japanese */
  RD_CHAO,                               /* chaos */
  RD_RAND                                /* selected at random */
};

typedef struct {                         /* type of structure for a hitting object */
  const char *word;                      /* face of keyword */
  int pt;                                /* score tuned by TF-IDF */
} KEYSC;


/* global variables */
const char *g_progname;                  /* program name */
int g_sigterm = FALSE;                   /* flag for termination signal */
int g_putopts = 0;                       /* options of registration */
int g_outopts = 0;                       /* options of deletion */
int g_optopts = 0;                       /* options of optimization */
int g_mgopts = 0;                        /* options of merger */
int g_rpopts = 0;                        /* options of repair */
int g_nooper = FALSE;                    /* whether not to operate actually */
const char *g_inputcode = "UTF-8";       /* input encoding */
int g_inputlang = ESTLANGEN;             /* prefered language */
const char *g_outputcode = "UTF-8";      /* output encoding */
int g_bincheck = FALSE;                  /* whether to check binary */
int g_limittsiz = LIMITTSIZ;             /* text size limitation */
int g_limitfsiz = LIMITFSIZ;             /* file size limitation */
const char *g_pathcode = NULL;           /* path encoding */
CBLIST *g_pathattrs = NULL;              /* names of elements in path extension */
CBLIST *g_addattrs = NULL;               /* names and values of additional attribues */
int g_oextmodes = 0;                     /* extra open modes */
int g_viewmode = VM_ID;                  /* viewing mode */
int g_snipwwidth = SNIPWWIDTH;           /* whole width of the snippet */
int g_sniphwidth = SNIPHWIDTH;           /* width of beginning of the text */
int g_snipawidth = SNIPAWIDTH;           /* width around each highlighted word */
int g_filefmt = FF_AUTO;                 /* file format */
CBMAP *g_xcmdmap = NULL;                 /* map of suffixes and filter commands */
int g_filtorig = FALSE;                  /* whether to use filter for original files */
CBLIST *g_rmvsufs = NULL;                /* list of suffixes of files to be removed */
const char *g_ssname = NULL;             /* name of an attribute for substitute score */
int g_stdate = FALSE;                    /* whether to adopt date by stat */
int g_chkmdate = FALSE;                  /* whether to check modification date */
double g_cachesize = -1;                 /* size of the cache */
int g_nochkvmem = FALSE;                 /* whether to omit virtual memory checking */
int g_doforce = FALSE;                   /* whether to force purging or extracting */
int g_kwordnum = KWORDNUM;               /* number of keywords */
int g_usemorph = FALSE;                  /* whether to use morphological analyzers */
int g_rdmode = RD_RAND;                  /* mode of random documents */


/* function prototypes */
int main(int argc, char **argv);
static void printferror(const char *format, ...);
static void printfinfo(const char *format, ...);
static void dbinform(const char *message, void *opaque);
static void setsignals(void);
static void sigtermhandler(int num);
static void usage(void);
static int runcreate(int argc, char **argv);
static int runput(int argc, char **argv);
static int runout(int argc, char **argv);
static int runedit(int argc, char **argv);
static int runget(int argc, char **argv);
static int runlist(int argc, char **argv);
static int runuriid(int argc, char **argv);
static int runmeta(int argc, char **argv);
static int runinform(int argc, char **argv);
static int runoptimize(int argc, char **argv);
static int runmerge(int argc, char **argv);
static int runrepair(int argc, char **argv);
static int runsearch(int argc, char **argv);
static int rungather(int argc, char **argv);
static int runpurge(int argc, char **argv);
static int runextkeys(int argc, char **argv);
static int runwords(int argc, char **argv);
static int rundraft(int argc, char **argv);
static int runbreak(int argc, char **argv);
static int runiconv(int argc, char **argv);
static int runregex(int argc, char **argv);
static int runscandir(int argc, char **argv);
static int runmulti(int argc, char **argv);
static int runrandput(int argc, char **argv);
static int runwicked(int argc, char **argv);
static int runregression(int argc, char **argv);
static int proccreate(const char *dbname, CBMAP *attrs);
static int procput(const char *dbname, const char *filename);
static int procout(const char *dbname, int id, const char *expr);
static int procedit(const char *dbname, int id, const char *expr,
                    const char *name, const char *value);
static int procget(const char *dbname, const CBLIST *pidxs,
                   int id, const char *expr, const char *attr);
static int proclist(const char *dbname, int lp);
static int procuriid(const char *dbname, const CBLIST *pidxs, const char *expr);
static int procmeta(const char *dbname, const char *mname, const char *mvalue);
static int procinform(const char *dbname);
static int procoptimize(const char *dbname);
static int procmerge(const char *dbname, const char *tgname);
static int procrepair(const char *dbname);
static int procsearch(const char *dbname, const CBLIST *pidxs,
                      const char *phrase, const CBLIST *attrs,
                      const char *ord, int max, int sk, int aux, double ec,
                      int opts, int cd, int sim, const char *dis);
static int procgather(const char *dbname, const char *filename);
static int procpurge(const char *dbname, const char *prefix, const CBLIST *attrs);
static int procextkeys(const char *dbname, const char *prefix, const CBLIST *attrs,
                       const char *dfdbname, int ni);
static int procwords(const char *dbname, const char *dfdbname, int kw, int kt);
static int procdraft(const char *filename);
static int procbreak(const char *filename, int wt);
static int prociconv(const char *filename);
static int procregex(const char *regex, const char *filename, const char *repl, int inv, int ci);
static int procscandir(const char *dirname, int tmode, int pmode);
static int procmulti(const CBLIST *dbnames, const char *phrase, const CBLIST *attrs,
                     const char *ord, int max, int sk, int aux, int opts, int cd,
                     const char *dis, int hu);
static int procrandput(const char *dbname, int dnum);
static int procwicked(const char *dbname, int dnum);
static int procregression(const char *dbname);
static void xmlprintf(const char *format, ...);
static int strtoidxtype(const char *str);
static int strtolang(const char *str);
static char *fgetl(FILE *ifp);
static int doputdoc(ESTDB *db, const char *path, const CBLIST *attrs);
static void doremovefile(const char *path);
static const char *exprtouri(const char *expr);
static const char *pathtourl(const char *path);
static const char *pathtolreal(const char *path);
static const char *urltopath(const char *uri);
static int dosbadname(const char *name);
static int est_check_binary(const char *buf, int size);
static ESTDOC *est_doc_new_with_xcmd(const char *buf, int size, const char *path,
                                     const char *xcmd, const char *tmpdir,
                                     const char *penc, int plang);
static ESTDOC *est_doc_new_from_draft_enc(const char *buf, int size, const char *enc);
static ESTDOC *est_doc_new_from_text(const char *buf, int size,
                                     const char *penc, int plang, int bcheck);
static ESTDOC *est_doc_new_from_html(const char *buf, int size,
                                     const char *penc, int plang, int bcheck);
static char *est_html_enc(const char *str);
static char *est_html_raw_text(const char *html);
static ESTDOC *est_doc_new_from_mime(const char *buf, int size, const char *penc,
                                     int plang, int bcheck);
static void est_doc_add_attr_mime(ESTDOC *doc, const char *name, const char *value);
static ESTDOC *est_doc_new_from_chaos(int cnum, int snum, int mode);
static char *est_random_str(int cnum, int mode);
static int keysc_compare(const void *ap, const void *bp);


/* main routine */
int main(int argc, char **argv){
  const char *tmp;
  int rv;
  if((tmp = getenv("ESTDBGFD")) != NULL) dpdbgfd = atoi(tmp);
  est_proc_env_reset();
  g_progname = argv[0];
  g_sigterm = FALSE;
  if(argc < 2) usage();
  rv = 0;
  if(!strcmp(argv[1], "create")){
    setsignals();
    rv = runcreate(argc, argv);
  } else if(!strcmp(argv[1], "put")){
    setsignals();
    rv = runput(argc, argv);
  } else if(!strcmp(argv[1], "out")){
    setsignals();
    rv = runout(argc, argv);
  } else if(!strcmp(argv[1], "edit")){
    setsignals();
    rv = runedit(argc, argv);
  } else if(!strcmp(argv[1], "get")){
    rv = runget(argc, argv);
  } else if(!strcmp(argv[1], "list")){
    rv = runlist(argc, argv);
  } else if(!strcmp(argv[1], "uriid")){
    rv = runuriid(argc, argv);
  } else if(!strcmp(argv[1], "meta")){
    setsignals();
    rv = runmeta(argc, argv);
  } else if(!strcmp(argv[1], "inform")){
    rv = runinform(argc, argv);
  } else if(!strcmp(argv[1], "optimize")){
    setsignals();
    rv = runoptimize(argc, argv);
  } else if(!strcmp(argv[1], "merge")){
    setsignals();
    rv = runmerge(argc, argv);
  } else if(!strcmp(argv[1], "repair")){
    setsignals();
    rv = runrepair(argc, argv);
  } else if(!strcmp(argv[1], "search")){
    rv = runsearch(argc, argv);
  } else if(!strcmp(argv[1], "gather")){
    setsignals();
    rv = rungather(argc, argv);
  } else if(!strcmp(argv[1], "purge")){
    setsignals();
    rv = runpurge(argc, argv);
  } else if(!strcmp(argv[1], "extkeys")){
    setsignals();
    rv = runextkeys(argc, argv);
  } else if(!strcmp(argv[1], "words")){
    setsignals();
    rv = runwords(argc, argv);
  } else if(!strcmp(argv[1], "draft")){
    rv = rundraft(argc, argv);
  } else if(!strcmp(argv[1], "break")){
    rv = runbreak(argc, argv);
  } else if(!strcmp(argv[1], "iconv")){
    rv = runiconv(argc, argv);
  } else if(!strcmp(argv[1], "regex")){
    rv = runregex(argc, argv);
  } else if(!strcmp(argv[1], "scandir")){
    rv = runscandir(argc, argv);
  } else if(!strcmp(argv[1], "multi")){
    rv = runmulti(argc, argv);
  } else if(!strcmp(argv[1], "randput")){
    setsignals();
    rv = runrandput(argc, argv);
  } else if(!strcmp(argv[1], "wicked")){
    setsignals();
    rv = runwicked(argc, argv);
  } else if(!strcmp(argv[1], "regression")){
    setsignals();
    rv = runregression(argc, argv);
  } else if(!strcmp(argv[1], "version") || !strcmp(argv[1], "--version")){
    printf("Hyper Estraier %s on %s\n", est_version, ESTSYSNAME);
    printf("Copyright (C) 2004-2007 Mikio Hirabayashi.\n");
    rv = 0;
  } else {
    usage();
  }
  return rv;
}


/* print formatted error string and flush the buffer */
static void printferror(const char *format, ...){
  va_list ap;
  va_start(ap, format);
  fprintf(stderr, "%s: ERROR: ", g_progname);
  vfprintf(stderr, format, ap);
  fputc('\n', stderr);
  fflush(stderr);
  va_end(ap);
}


/* print formatted information string and flush the buffer */
static void printfinfo(const char *format, ...){
  va_list ap;
  va_start(ap, format);
  printf("%s: INFO: ", g_progname);
  vprintf(format, ap);
  putchar('\n');
  fflush(stdout);
  va_end(ap);
}


/* callback function for database events */
static void dbinform(const char *message, void *opaque){
  printfinfo("%s", message);
}


/* set signal handlers */
static void setsignals(void){
  est_signal(1, sigtermhandler);
  est_signal(2, sigtermhandler);
  est_signal(3, sigtermhandler);
  est_signal(13, SIG_IGN);
  est_signal(14, SIG_IGN);
  est_signal(15, sigtermhandler);
  g_sigterm = FALSE;
}


/* handler of termination signal */
static void sigtermhandler(int num){
  g_sigterm = TRUE;
  printfinfo("the termination signal %d catched", num);
}


/* print the usage and exit */
static void usage(void){
  fprintf(stderr, "%s: command line utility for the core API of Hyper Estraier\n", g_progname);
  fprintf(stderr, "\n");
  fprintf(stderr, "usage:\n");
  fprintf(stderr, "  %s create [-tr] [-apn|-acc] [-xs|-xl|-xh|-xh2|-xh3] [-sv|-si|-sa]"
          " [-attr name type] db\n", g_progname);
  fprintf(stderr, "  %s put [-tr] [-cl] [-ws] [-apn|-acc] [-xs|-xl|-xh|-xh2|-xh3] [-sv|-si|-sa]"
          " db [file]\n", g_progname);
  fprintf(stderr, "  %s out [-cl] [-pc enc] db expr\n", g_progname);
  fprintf(stderr, "  %s edit [-pc enc] db expr name [value]\n", g_progname);
  fprintf(stderr, "  %s get [-nl|-nb] [-pidx path] [-pc enc] db expr [attr]\n", g_progname);
  fprintf(stderr, "  %s list [-nl|-nb] [-lp] db\n", g_progname);
  fprintf(stderr, "  %s uriid [-nl|-nb] [-pidx path] [-pc enc] db expr\n", g_progname);
  fprintf(stderr, "  %s meta db [name [value]]\n", g_progname);
  fprintf(stderr, "  %s inform [-nl|-nb] db\n", g_progname);
  fprintf(stderr, "  %s optimize [-onp] [-ond] db\n", g_progname);
  fprintf(stderr, "  %s merge [-cl] db target\n", g_progname);
  fprintf(stderr, "  %s repair [-rst|-rsh] db\n", g_progname);
  fprintf(stderr, "  %s search [-nl|-nb] [-pidx path] [-ic enc] [-vu|-va|-vf|-vs|-vh|-vx|-dd]"
          " [-sn wnum hnum anum] [-kn num] [-um] [-ec rn] [-gs|-gf|-ga] [-cd] [-ni]"
          " [-sf|-sfr|-sfu|-sfi] [-hs] [-attr expr] [-ord expr] [-max num] [-sk num] [-aux num]"
          " [-dis name] [-sim id] db [phrase]\n", g_progname);
  fprintf(stderr, "  %s gather [-tr] [-cl] [-ws] [-no] [-fe|-ft|-fh|-fm]"
          " [-fx sufs cmd] [-fz] [-fo] [-rm sufs] [-ic enc] [-il lang] [-bc] [-lt num] [-lf num]"
          " [-pc enc] [-px name] [-aa name value] [-apn|-acc] [-xs|-xl|-xh|-xh2|-xh3]"
          " [-sv|-si|-sa] [-ss name] [-sd] [-cm] [-cs num] [-ncm] [-kn num] [-um]"
          " db [file|dir]\n", g_progname);
  fprintf(stderr, "  %s purge [-cl] [-no] [-fc] [-pc enc] [-attr expr] db [prefix]\n",
          g_progname);
  fprintf(stderr, "  %s extkeys [-no] [-fc] [-dfdb file] [-ncm] [-ni] [-kn num] [-um] [-ws]"
          " [-attr expr] db [prefix]\n", g_progname);
  fprintf(stderr, "  %s words [-nl|-nb] [-dfdb file] [-kw|-kt] db\n", g_progname);
  fprintf(stderr, "  %s draft [-ft|-fh|-fm] [-ic enc] [-il lang] [-bc] [-lt num]"
          " [-kn num] [-um] [file]\n", g_progname);
  fprintf(stderr, "  %s break [-ic enc] [-il lang] [-apn|-acc] [-wt] [file]\n", g_progname);
  fprintf(stderr, "  %s iconv [-ic enc] [-il lang] [-oc enc] [file]\n", g_progname);
  fprintf(stderr, "  %s regex [-inv] [-ci] [-repl str] expr [file]\n", g_progname);
  fprintf(stderr, "  %s scandir [-tf|-td] [-pa|-pu] [dir]\n", g_progname);
  fprintf(stderr, "  %s multi [-db db] [-nl|-nb] [-ic enc] [-gs|-gf|-ga] [-cd] [-ni]"
          " [-sf|-sfr|-sfu|-sfi] [-hs] [-hu] [-attr expr] [-ord expr] [-max num] [-sk num]"
          " [-aux num] [-dis name] [phrase]\n", g_progname);
  fprintf(stderr, "  %s randput [-ren|-rla|-reu|-ror|-rjp|-rch] [-cs num] db dnum\n",
          g_progname);
  fprintf(stderr, "  %s wicked db dnum\n", g_progname);
  fprintf(stderr, "  %s regression db\n", g_progname);
  fprintf(stderr, "  %s version\n", g_progname);
  fprintf(stderr, "\n");
  exit(1);
}


/* parse arguments of the create command */
static int runcreate(int argc, char **argv){
  CBMAP *attrs;
  char *dbname;
  int i, rv;
  dbname = NULL;
  attrs = cbmapopenex(MINIBNUM);
  cbglobalgc(attrs, (void (*)(void *))cbmapclose);
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-tr")){
        g_oextmodes |= ESTDBTRUNC;
      } else if(!strcmp(argv[i], "-apn")){
        g_oextmodes |= ESTDBPERFNG;
      } else if(!strcmp(argv[i], "-acc")){
        g_oextmodes |= ESTDBCHRCAT;
      } else if(!strcmp(argv[i], "-xs")){
        g_oextmodes |= ESTDBSMALL;
      } else if(!strcmp(argv[i], "-xl")){
        g_oextmodes |= ESTDBLARGE;
      } else if(!strcmp(argv[i], "-xh")){
        g_oextmodes |= ESTDBHUGE;
      } else if(!strcmp(argv[i], "-xh2")){
        g_oextmodes |= ESTDBHUGE2;
      } else if(!strcmp(argv[i], "-xh3")){
        g_oextmodes |= ESTDBHUGE3;
      } else if(!strcmp(argv[i], "-sv")){
        g_oextmodes |= ESTDBSCVOID;
      } else if(!strcmp(argv[i], "-si")){
        g_oextmodes |= ESTDBSCINT;
      } else if(!strcmp(argv[i], "-sa")){
        g_oextmodes |= ESTDBSCASIS;
      } else if(!strcmp(argv[i], "-attr")){
        if((i += 2) >= argc) usage();
        cbmapput(attrs, argv[i-1], -1, argv[i], -1, TRUE);
      } else {
        usage();
      }
    } else if(!dbname){
      dbname = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname) usage();
  rv = proccreate(dbname, attrs);
  return rv;
}


/* parse arguments of the put command */
static int runput(int argc, char **argv){
  char *dbname, *filename;
  int i, rv;
  dbname = NULL;
  filename = NULL;
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-tr")){
        g_oextmodes |= ESTDBTRUNC;
      } else if(!strcmp(argv[i], "-cl")){
        g_putopts |= ESTPDCLEAN;
      } else if(!strcmp(argv[i], "-ws")){
        g_putopts |= ESTPDWEIGHT;
      } else if(!strcmp(argv[i], "-apn")){
        g_oextmodes |= ESTDBPERFNG;
      } else if(!strcmp(argv[i], "-acc")){
        g_oextmodes |= ESTDBCHRCAT;
      } else if(!strcmp(argv[i], "-xs")){
        g_oextmodes |= ESTDBSMALL;
      } else if(!strcmp(argv[i], "-xl")){
        g_oextmodes |= ESTDBLARGE;
      } else if(!strcmp(argv[i], "-xh")){
        g_oextmodes |= ESTDBHUGE;
      } else if(!strcmp(argv[i], "-xh2")){
        g_oextmodes |= ESTDBHUGE2;
      } else if(!strcmp(argv[i], "-xh3")){
        g_oextmodes |= ESTDBHUGE3;
      } else if(!strcmp(argv[i], "-sv")){
        g_oextmodes |= ESTDBSCVOID;
      } else if(!strcmp(argv[i], "-si")){
        g_oextmodes |= ESTDBSCINT;
      } else if(!strcmp(argv[i], "-sa")){
        g_oextmodes |= ESTDBSCASIS;
      } else {
        usage();
      }
    } else if(!dbname){
      dbname = argv[i];
    } else if(!filename){
      filename = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname) usage();
  rv = procput(dbname, filename);
  return rv;
}


/* parse arguments of the out command */
static int runout(int argc, char **argv){
  char *dbname, *expr;
  int i, id, rv;
  dbname = NULL;
  expr = NULL;
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-cl")){
        g_outopts |= ESTODCLEAN;
      } else if(!strcmp(argv[i], "-pc")){
        if(++i >= argc) usage();
        g_pathcode = argv[i];
      } else {
        usage();
      }
    } else if(!dbname){
      dbname = argv[i];
    } else if(!expr){
      expr = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname || !expr) usage();
  if((id = atoi(expr)) > 0) expr = NULL;
  rv = procout(dbname, id, expr);
  return rv;
}


/* parse arguments of the edit command */
static int runedit(int argc, char **argv){
  char *dbname, *expr, *name, *value;
  int i, id, rv;
  dbname = NULL;
  expr = NULL;
  name = NULL;
  value = NULL;
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-pc")){
        if(++i >= argc) usage();
        g_pathcode = argv[i];
      } else {
        usage();
      }
    } else if(!dbname){
      dbname = argv[i];
    } else if(!expr){
      expr = argv[i];
    } else if(!name){
      name = argv[i];
    } else if(!value){
      value = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname || !expr || !name) usage();
  if((id = atoi(expr)) > 0) expr = NULL;
  rv = procedit(dbname, id, expr, name, value);
  return rv;
}


/* parse arguments of the get command */
static int runget(int argc, char **argv){
  CBLIST *pidxs;
  char *dbname, *expr, *attr;
  int i, id, rv;
  dbname = NULL;
  expr = NULL;
  attr = NULL;
  pidxs = cblistopen();
  cbglobalgc(pidxs, (void (*)(void *))cblistclose);
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-nl")){
        g_oextmodes |= ESTDBNOLCK;
      } else if(!strcmp(argv[i], "-nb")){
        g_oextmodes |= ESTDBLCKNB;
      } else if(!strcmp(argv[i], "-pidx")){
        if(++i >= argc) usage();
        cblistpush(pidxs, argv[i], -1);
      } else if(!strcmp(argv[i], "-pc")){
        if(++i >= argc) usage();
        g_pathcode = argv[i];
      } else {
        usage();
      }
    } else if(!dbname){
      dbname = argv[i];
    } else if(!expr){
      expr = argv[i];
    } else if(!attr){
      attr = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname || !expr) usage();
  if((id = atoi(expr)) > 0) expr = NULL;
  rv = procget(dbname, pidxs, id, expr, attr);
  return rv;
}


/* parse arguments of the list command */
static int runlist(int argc, char **argv){
  char *dbname;
  int i, lp, rv;
  dbname = NULL;
  lp = FALSE;
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-nl")){
        g_oextmodes |= ESTDBNOLCK;
      } else if(!strcmp(argv[i], "-nb")){
        g_oextmodes |= ESTDBLCKNB;
      } else if(!strcmp(argv[i], "-lp")){
        lp = TRUE;
      } else {
        usage();
      }
    } else if(!dbname){
      dbname = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname) usage();
  rv = proclist(dbname, lp);
  return rv;
}


/* parse arguments of the uriid command */
static int runuriid(int argc, char **argv){
  CBLIST *pidxs;
  char *dbname, *expr;
  int i, rv;
  dbname = NULL;
  expr = NULL;
  pidxs = cblistopen();
  cbglobalgc(pidxs, (void (*)(void *))cblistclose);
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-nl")){
        g_oextmodes |= ESTDBNOLCK;
      } else if(!strcmp(argv[i], "-nb")){
        g_oextmodes |= ESTDBLCKNB;
      } else if(!strcmp(argv[i], "-pidx")){
        if(++i >= argc) usage();
        cblistpush(pidxs, argv[i], -1);
      } else if(!strcmp(argv[i], "-pc")){
        if(++i >= argc) usage();
        g_pathcode = argv[i];
      } else {
        usage();
      }
    } else if(!dbname){
      dbname = argv[i];
    } else if(!expr){
      expr = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname || !expr) usage();
  rv = procuriid(dbname, pidxs, expr);
  return rv;
}


/* parse arguments of the meta command */
static int runmeta(int argc, char **argv){
  char *dbname, *mname, *mvalue;
  int i, del, rv;
  dbname = NULL;
  mname = NULL;
  mvalue = NULL;
  del = FALSE;
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      usage();
    } else if(!dbname){
      dbname = argv[i];
    } else if(!mname){
      mname = argv[i];
    } else if(!mvalue){
      mvalue = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname) usage();
  rv = procmeta(dbname, mname, mvalue);
  return rv;
}


/* parse arguments of the inform command */
static int runinform(int argc, char **argv){
  char *dbname;
  int i, rv;
  dbname = NULL;
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-nl")){
        g_oextmodes |= ESTDBNOLCK;
      } else if(!strcmp(argv[i], "-nb")){
        g_oextmodes |= ESTDBLCKNB;
      } else {
        usage();
      }
    } else if(!dbname){
      dbname = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname) usage();
  rv = procinform(dbname);
  return rv;
}


/* parse arguments of the optimize command */
static int runoptimize(int argc, char **argv){
  char *dbname;
  int i, rv;
  dbname = NULL;
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-onp")){
        g_optopts |= ESTOPTNOPURGE;
      } else if(!strcmp(argv[i], "-ond")){
        g_optopts |= ESTOPTNODBOPT;
      } else {
        usage();
      }
    } else if(!dbname){
      dbname = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname) usage();
  rv = procoptimize(dbname);
  return rv;
}


/* parse arguments of the merge command */
static int runmerge(int argc, char **argv){
  char *dbname, *tgname;
  int i, rv;
  dbname = NULL;
  tgname = NULL;
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-cl")){
        g_mgopts |= ESTMGCLEAN;
      } else {
        usage();
      }
    } else if(!dbname){
      dbname = argv[i];
    } else if(!tgname){
      tgname = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname || !tgname) usage();
  rv = procmerge(dbname, tgname);
  return rv;
}


/* parse arguments of the repair command */
static int runrepair(int argc, char **argv){
  char *dbname;
  int i, rv;
  dbname = NULL;
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-rst")){
        g_rpopts |= ESTRPSTRICT;
      } else if(!strcmp(argv[i], "-rsh")){
        g_rpopts |= ESTRPSHODDY;
      } else {
        usage();
      }
    } else if(!dbname){
      dbname = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname) usage();
  rv = procrepair(dbname);
  return rv;
}


/* parse arguments of the search command */
static int runsearch(int argc, char **argv){
  CBDATUM *pbuf;
  CBLIST *pidxs, *attrs;
  char *dbname, *ord, *dis, *phrase, *tmp;
  int i, max, sk, aux, opts, cd, sim, rv;
  double ec;
  g_kwordnum = -1;
  dbname = NULL;
  ord = NULL;
  dis = NULL;
  max = SEARCHMAX;
  sk = 0;
  aux = SEARCHAUX;
  ec = -1.0;
  opts = 0;
  cd = FALSE;
  sim = -1;
  pbuf = cbdatumopen(NULL, -1);
  cbglobalgc(pbuf, (void (*)(void *))cbdatumclose);
  pidxs = cblistopen();
  cbglobalgc(pidxs, (void (*)(void *))cblistclose);
  attrs = cblistopen();
  cbglobalgc(attrs, (void (*)(void *))cblistclose);
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-nl")){
        g_oextmodes |= ESTDBNOLCK;
      } else if(!strcmp(argv[i], "-nb")){
        g_oextmodes |= ESTDBLCKNB;
      } else if(!strcmp(argv[i], "-pidx")){
        if(++i >= argc) usage();
        cblistpush(pidxs, argv[i], -1);
      } else if(!strcmp(argv[i], "-ic")){
        if(++i >= argc) usage();
        g_inputcode = argv[i];
      } else if(!strcmp(argv[i], "-vu")){
        g_viewmode = VM_URI;
      } else if(!strcmp(argv[i], "-va")){
        g_viewmode = VM_ATTR;
      } else if(!strcmp(argv[i], "-vf")){
        g_viewmode = VM_FULL;
      } else if(!strcmp(argv[i], "-vs")){
        g_viewmode = VM_SNIP;
      } else if(!strcmp(argv[i], "-vh")){
        g_viewmode = VM_HMRD;
      } else if(!strcmp(argv[i], "-vx")){
        g_viewmode = VM_XML;
      } else if(!strcmp(argv[i], "-dd")){
        g_viewmode = VM_DUMP;
      } else if(!strcmp(argv[i], "-sn")){
        if(++i >= argc) usage();
        g_snipwwidth = atoi(argv[i]);
        if(++i >= argc) usage();
        g_snipawidth = atoi(argv[i]);
        if(++i >= argc) usage();
        g_sniphwidth = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-kn")){
        if(++i >= argc) usage();
        g_kwordnum = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-um")){
        g_usemorph = TRUE;
      } else if(!strcmp(argv[i], "-ec")){
        if(++i >= argc) usage();
        if(!cbstricmp(argv[i], "serv") || !cbstricmp(argv[i], "s")){
          ec = ESTECLSERV;
        } else if(!cbstricmp(argv[i], "dir") || !cbstricmp(argv[i], "d")){
          ec = ESTECLDIR;
        } else if(!cbstricmp(argv[i], "file") || !cbstricmp(argv[i], "f")){
          ec = ESTECLFILE;
        } else {
          ec = strtod(argv[i], NULL);
          if(ec < 0) ec = ESTECLSIMURL - ec;
        }
      } else if(!strcmp(argv[i], "-gs")){
        opts |= ESTCONDSURE;
      } else if(!strcmp(argv[i], "-gf")){
        opts |= ESTCONDFAST;
      } else if(!strcmp(argv[i], "-ga")){
        opts |= ESTCONDAGITO;
      } else if(!strcmp(argv[i], "-cd")){
        cd = TRUE;
      } else if(!strcmp(argv[i], "-ni")){
        opts |= ESTCONDNOIDF;
      } else if(!strcmp(argv[i], "-sf")){
        opts |= ESTCONDSIMPLE;
      } else if(!strcmp(argv[i], "-sfr")){
        opts |= ESTCONDROUGH;
      } else if(!strcmp(argv[i], "-sfu")){
        opts |= ESTCONDUNION;
      } else if(!strcmp(argv[i], "-sfi")){
        opts |= ESTCONDISECT;
      } else if(!strcmp(argv[i], "-hs")){
        opts |= ESTCONDSCFB;
      } else if(!strcmp(argv[i], "-attr")){
        if(++i >= argc) usage();
        cblistpush(attrs, argv[i], -1);
      } else if(!strcmp(argv[i], "-ord")){
        if(++i >= argc) usage();
        ord = argv[i];
      } else if(!strcmp(argv[i], "-max")){
        if(++i >= argc) usage();
        max = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-sk")){
        if(++i >= argc) usage();
        sk = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-aux")){
        if(++i >= argc) usage();
        aux = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-dis")){
        if(++i >= argc) usage();
        dis = argv[i];
      } else if(!strcmp(argv[i], "-sim")){
        if(++i >= argc) usage();
        sim = atoi(argv[i]);
      } else {
        usage();
      }
    } else if(!dbname){
      dbname = argv[i];
    } else {
      if(cbdatumsize(pbuf) > 0) cbdatumcat(pbuf, " ", 1);
      cbdatumcat(pbuf, argv[i], -1);
    }
  }
  if(!dbname || g_snipwwidth < 0 || g_sniphwidth < 0 || g_snipawidth < 0) usage();
  if(!(phrase = est_iconv(cbdatumptr(pbuf), -1, g_inputcode, "UTF-8", NULL, NULL))){
    printferror("%s: unsupported encoding\n", g_inputcode);
    return 1;
  }
  cbstrtrim(phrase);
  for(i = 0; i < cblistnum(attrs); i++){
    if((tmp = est_iconv(cblistval(attrs, i, NULL), -1, g_inputcode, "UTF-8", NULL, NULL)) != NULL){
      cblistover(attrs, i, tmp, -1);
      free(tmp);
    }
  }
  rv = procsearch(dbname, pidxs, phrase, attrs, ord, max, sk, aux, ec, opts, cd, sim, dis);
  free(phrase);
  return rv;
}


/* parse arguments of the gather command */
static int rungather(int argc, char **argv){
  CBLIST *list;
  const char *elem;
  char *dbname, *filename;
  int i, j, rv;
  g_pathattrs = cblistopen();
  cbglobalgc(g_pathattrs, (void (*)(void *))cblistclose);
  g_addattrs = cblistopen();
  cbglobalgc(g_addattrs, (void (*)(void *))cblistclose);
  g_xcmdmap = cbmapopenex(MINIBNUM);
  cbglobalgc(g_xcmdmap, (void (*)(void *))cbmapclose);
  g_rmvsufs = cblistopen();
  cbglobalgc(g_rmvsufs, (void (*)(void *))cblistclose);
  g_inputcode = NULL;
  g_kwordnum = -1;
  dbname = NULL;
  filename = NULL;
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-tr")){
        g_oextmodes |= ESTDBTRUNC;
      } else if(!strcmp(argv[i], "-cl")){
        g_putopts |= ESTPDCLEAN;
      } else if(!strcmp(argv[i], "-ws")){
        g_putopts |= ESTPDWEIGHT;
      } else if(!strcmp(argv[i], "-no")){
        g_nooper = TRUE;
      } else if(!strcmp(argv[i], "-fe")){
        g_filefmt = FF_DRAFT;
      } else if(!strcmp(argv[i], "-ft")){
        g_filefmt = FF_TEXT;
      } else if(!strcmp(argv[i], "-fh")){
        g_filefmt = FF_HTML;
      } else if(!strcmp(argv[i], "-fm")){
        g_filefmt = FF_MIME;
      } else if(!strcmp(argv[i], "-fx")){
        if((i += 2) >= argc) usage();
        list = cbsplit(argv[i-1], -1, ",");
        for(j = 0; j < cblistnum(list); j++){
          elem = cblistval(list, j, NULL);
          if(elem[0] != '\0') cbmapput(g_xcmdmap, elem, -1, argv[i], -1, FALSE);
        }
        cblistclose(list);
      } else if(!strcmp(argv[i], "-fz")){
        g_filefmt = FF_NONE;
      } else if(!strcmp(argv[i], "-fo")){
        g_filtorig = TRUE;
      } else if(!strcmp(argv[i], "-rm")){
        if(++i >= argc) usage();
        list = cbsplit(argv[i], -1, ",");
        for(j = 0; j < cblistnum(list); j++){
          elem = cblistval(list, j, NULL);
          if(elem[0] != '\0') cblistpush(g_rmvsufs, elem, -1);
        }
        cblistclose(list);
      } else if(!strcmp(argv[i], "-ic")){
        if(++i >= argc) usage();
        g_inputcode = argv[i];
      } else if(!strcmp(argv[i], "-il")){
        if(++i >= argc) usage();
        g_inputlang = strtolang(argv[i]);
      } else if(!strcmp(argv[i], "-bc")){
        g_bincheck = TRUE;
      } else if(!strcmp(argv[i], "-lt")){
        if(++i >= argc) usage();
        g_limittsiz = strtod(argv[i], NULL) * 1024;
      } else if(!strcmp(argv[i], "-lf")){
        if(++i >= argc) usage();
        g_limitfsiz = strtod(argv[i], NULL) * 1024 * 1024;
      } else if(!strcmp(argv[i], "-pc")){
        if(++i >= argc) usage();
        g_pathcode = argv[i];
      } else if(!strcmp(argv[i], "-px")){
        if(++i >= argc) usage();
        cblistpush(g_pathattrs, argv[i], -1);
      } else if(!strcmp(argv[i], "-aa")){
        if(++i >= argc) usage();
        cblistpush(g_addattrs, argv[i], -1);
        if(++i >= argc) usage();
        cblistpush(g_addattrs, argv[i], -1);
      } else if(!strcmp(argv[i], "-apn")){
        g_oextmodes |= ESTDBPERFNG;
      } else if(!strcmp(argv[i], "-acc")){
        g_oextmodes |= ESTDBCHRCAT;
      } else if(!strcmp(argv[i], "-xs")){
        g_oextmodes |= ESTDBSMALL;
      } else if(!strcmp(argv[i], "-xl")){
        g_oextmodes |= ESTDBLARGE;
      } else if(!strcmp(argv[i], "-xh")){
        g_oextmodes |= ESTDBHUGE;
      } else if(!strcmp(argv[i], "-xh2")){
        g_oextmodes |= ESTDBHUGE2;
      } else if(!strcmp(argv[i], "-xh3")){
        g_oextmodes |= ESTDBHUGE3;
      } else if(!strcmp(argv[i], "-sv")){
        g_oextmodes |= ESTDBSCVOID;
      } else if(!strcmp(argv[i], "-si")){
        g_oextmodes |= ESTDBSCINT;
      } else if(!strcmp(argv[i], "-sa")){
        g_oextmodes |= ESTDBSCASIS;
      } else if(!strcmp(argv[i], "-ss")){
        if(++i >= argc) usage();
        g_ssname = argv[i];
      } else if(!strcmp(argv[i], "-sd")){
        g_stdate = TRUE;
      } else if(!strcmp(argv[i], "-cm")){
        g_chkmdate = TRUE;
      } else if(!strcmp(argv[i], "-cs")){
        if(++i >= argc) usage();
        g_cachesize = strtod(argv[i], NULL) * 1024 * 1024;
      } else if(!strcmp(argv[i], "-ncm")){
        g_nochkvmem = TRUE;
      } else if(!strcmp(argv[i], "-kn")){
        if(++i >= argc) usage();
        g_kwordnum = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-um")){
        g_usemorph = TRUE;
      } else {
        usage();
      }
    } else if(!dbname){
      dbname = argv[i];
    } else if(!filename){
      filename = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname || !filename) usage();
  rv = procgather(dbname, filename);
  return rv;
}


/* parse arguments of the purge command */
static int runpurge(int argc, char **argv){
  CBLIST *attrs;
  char *dbname, *prefix;
  int i, rv;
  dbname = NULL;
  prefix = NULL;
  attrs = cblistopen();
  cbglobalgc(attrs, (void (*)(void *))cblistclose);
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-cl")){
        g_outopts |= ESTODCLEAN;
      } else if(!strcmp(argv[i], "-no")){
        g_nooper = TRUE;
      } else if(!strcmp(argv[i], "-fc")){
        g_doforce = TRUE;
      } else if(!strcmp(argv[i], "-pc")){
        if(++i >= argc) usage();
        g_pathcode = argv[i];
      } else if(!strcmp(argv[i], "-attr")){
        if(++i >= argc) usage();
        cblistpush(attrs, argv[i], -1);
      } else {
        usage();
      }
    } else if(!dbname){
      dbname = argv[i];
    } else if(!prefix){
      prefix = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname) usage();
  rv = procpurge(dbname, prefix, attrs);
  return rv;
}


/* parse arguments of the extkeys command */
static int runextkeys(int argc, char **argv){
  CBLIST *attrs;
  char *dbname, *prefix, *dfdbname;
  int i, ni, rv;
  dbname = NULL;
  prefix = NULL;
  dfdbname = NULL;
  ni = FALSE;
  attrs = cblistopen();
  cbglobalgc(attrs, (void (*)(void *))cblistclose);
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-no")){
        g_nooper = TRUE;
      } else if(!strcmp(argv[i], "-fc")){
        g_doforce = TRUE;
      } else if(!strcmp(argv[i], "-dfdb")){
        if(++i >= argc) usage();
        dfdbname = argv[i];
      } else if(!strcmp(argv[i], "-ncm")){
        g_nochkvmem = TRUE;
      } else if(!strcmp(argv[i], "-ni")){
        ni = TRUE;
      } else if(!strcmp(argv[i], "-kn")){
        if(++i >= argc) usage();
        g_kwordnum = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-um")){
        g_usemorph = TRUE;
      } else if(!strcmp(argv[i], "-ws")){
        g_putopts |= ESTPDWEIGHT;
      } else if(!strcmp(argv[i], "-attr")){
        if(++i >= argc) usage();
        cblistpush(attrs, argv[i], -1);
      } else {
        usage();
      }
    } else if(!dbname){
      dbname = argv[i];
    } else if(!prefix){
      prefix = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname || g_kwordnum < 1) usage();
  rv = procextkeys(dbname, prefix, attrs, dfdbname, ni);
  return rv;
}


/* parse arguments of the words command */
static int runwords(int argc, char **argv){
  char *dbname, *dfdbname;
  int i, kw, kt, rv;
  dbname = NULL;
  dfdbname = NULL;
  kw = FALSE;
  kt = FALSE;
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-nl")){
        g_oextmodes |= ESTDBNOLCK;
      } else if(!strcmp(argv[i], "-nb")){
        g_oextmodes |= ESTDBLCKNB;
      } else if(!strcmp(argv[i], "-dfdb")){
        if(++i >= argc) usage();
        dfdbname = argv[i];
      } else if(!strcmp(argv[i], "-kw")){
        kw = TRUE;
      } else if(!strcmp(argv[i], "-kt")){
        kt = TRUE;
      } else {
        usage();
      }
    } else if(!dbname){
      dbname = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname) usage();
  rv = procwords(dbname, dfdbname, kw, kt);
  return rv;
}


/* parse arguments of the draft command */
static int rundraft(int argc, char **argv){
  char *filename;
  int i, rv;
  g_filefmt = FF_DRAFT;
  g_inputcode = NULL;
  g_kwordnum = -1;
  filename = NULL;
  for(i = 2; i < argc; i++){
    if(!filename && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-ft")){
        g_filefmt = FF_TEXT;
      } else if(!strcmp(argv[i], "-fh")){
        g_filefmt = FF_HTML;
      } else if(!strcmp(argv[i], "-fm")){
        g_filefmt = FF_MIME;
      } else if(!strcmp(argv[i], "-ic")){
        if(++i >= argc) usage();
        g_inputcode = argv[i];
      } else if(!strcmp(argv[i], "-il")){
        if(++i >= argc) usage();
        g_inputlang = strtolang(argv[i]);
      } else if(!strcmp(argv[i], "-bc")){
        g_bincheck = TRUE;
      } else if(!strcmp(argv[i], "-lt")){
        if(++i >= argc) usage();
        g_limittsiz = strtod(argv[i], NULL) * 1024;
      } else if(!strcmp(argv[i], "-kn")){
        if(++i >= argc) usage();
        g_kwordnum = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-um")){
        g_usemorph = TRUE;
      } else {
        usage();
      }
    } else if(!filename){
      filename = argv[i];
    } else {
      usage();
    }
  }
  rv = procdraft(filename);
  return rv;
}


/* parse arguments of the break command */
static int runbreak(int argc, char **argv){
  char *filename;
  int i, wt, rv;
  g_inputcode = NULL;
  filename = NULL;
  wt = FALSE;
  for(i = 2; i < argc; i++){
    if(!filename && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-ic")){
        if(++i >= argc) usage();
        g_inputcode = argv[i];
      } else if(!strcmp(argv[i], "-il")){
        if(++i >= argc) usage();
        g_inputlang = strtolang(argv[i]);
      } else if(!strcmp(argv[i], "-apn")){
        g_oextmodes |= ESTDBPERFNG;
      } else if(!strcmp(argv[i], "-acc")){
        g_oextmodes |= ESTDBCHRCAT;
      } else if(!strcmp(argv[i], "-wt")){
        wt = TRUE;
      } else {
        usage();
      }
    } else if(!filename){
      filename = argv[i];
    } else {
      usage();
    }
  }
  rv = procbreak(filename, wt);
  return rv;
}


/* parse arguments of the iconv command */
static int runiconv(int argc, char **argv){
  char *filename;
  int i, rv;
  g_inputcode = NULL;
  filename = NULL;
  for(i = 2; i < argc; i++){
    if(!filename && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-ic")){
        if(++i >= argc) usage();
        g_inputcode = argv[i];
      } else if(!strcmp(argv[i], "-il")){
        if(++i >= argc) usage();
        g_inputlang = strtolang(argv[i]);
      } else if(!strcmp(argv[i], "-oc")){
        if(++i >= argc) usage();
        g_outputcode = argv[i];
      } else {
        usage();
      }
    } else if(!filename){
      filename = argv[i];
    } else {
      usage();
    }
  }
  rv = prociconv(filename);
  return rv;
}


/* parse arguments of the regex command */
static int runregex(int argc, char **argv){
  char *regex, *filename, *repl;
  int i, inv, ci, rv;
  regex = NULL;
  filename = NULL;
  repl = NULL;
  inv = FALSE;
  ci = FALSE;
  for(i = 2; i < argc; i++){
    if(!regex && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-inv")){
        inv = TRUE;
      } else if(!strcmp(argv[i], "-ci")){
        ci = TRUE;
      } else if(!strcmp(argv[i], "-repl")){
        if(++i >= argc) usage();
        repl = argv[i];
      } else {
        usage();
      }
    } else if(!regex){
      regex = argv[i];
    } else if(!filename){
      filename = argv[i];
    } else {
      usage();
    }
  }
  if(!regex) usage();
  rv = procregex(regex, filename, repl, inv, ci);
  return rv;
}


/* parse arguments of the scandir command */
static int runscandir(int argc, char **argv){
  char *dirname;
  int i, tmode, pmode, rv;
  dirname = NULL;
  tmode = ST_ALL;
  pmode = SP_REL;
  for(i = 2; i < argc; i++){
    if(!dirname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-tf")){
        tmode = ST_FILE;
      } else if(!strcmp(argv[i], "-td")){
        tmode = ST_DIR;
      } else if(!strcmp(argv[i], "-pa")){
        pmode = SP_ABS;
      } else if(!strcmp(argv[i], "-pu")){
        pmode = SP_URL;
      } else {
        usage();
      }
    } else if(!dirname){
      dirname = argv[i];
    } else {
      usage();
    }
  }
  if(!dirname) dirname = ESTCDIRSTR;
  rv = procscandir(dirname, tmode, pmode);
  return rv;
}


/* parse arguments of the multi command */
static int runmulti(int argc, char **argv){
  CBDATUM *pbuf;
  CBLIST *dbnames, *attrs;
  char *ord, *phrase, *dis, *tmp;
  int i, max, sk, aux, opts, cd, hu, rv;
  ord = NULL;
  dis = NULL;
  max = SEARCHMAX;
  sk = 0;
  aux = SEARCHAUX;
  opts = 0;
  cd = FALSE;
  hu = FALSE;
  dbnames = cblistopen();
  cbglobalgc(dbnames, (void (*)(void *))cblistclose);
  pbuf = cbdatumopen(NULL, -1);
  cbglobalgc(pbuf, (void (*)(void *))cbdatumclose);
  attrs = cblistopen();
  cbglobalgc(attrs, (void (*)(void *))cblistclose);
  for(i = 2; i < argc; i++){
    if(cbdatumsize(pbuf) < 1 && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-db")){
        if(++i >= argc) usage();
        cblistpush(dbnames, argv[i], -1);
      } else if(!strcmp(argv[i], "-nl")){
        g_oextmodes |= ESTDBNOLCK;
      } else if(!strcmp(argv[i], "-nb")){
        g_oextmodes |= ESTDBLCKNB;
      } else if(!strcmp(argv[i], "-ic")){
        if(++i >= argc) usage();
        g_inputcode = argv[i];
      } else if(!strcmp(argv[i], "-gs")){
        opts |= ESTCONDSURE;
      } else if(!strcmp(argv[i], "-gf")){
        opts |= ESTCONDFAST;
      } else if(!strcmp(argv[i], "-ga")){
        opts |= ESTCONDAGITO;
      } else if(!strcmp(argv[i], "-cd")){
        cd = TRUE;
      } else if(!strcmp(argv[i], "-ni")){
        opts |= ESTCONDNOIDF;
      } else if(!strcmp(argv[i], "-sf")){
        opts |= ESTCONDSIMPLE;
      } else if(!strcmp(argv[i], "-sfr")){
        opts |= ESTCONDROUGH;
      } else if(!strcmp(argv[i], "-sfu")){
        opts |= ESTCONDUNION;
      } else if(!strcmp(argv[i], "-sfi")){
        opts |= ESTCONDISECT;
      } else if(!strcmp(argv[i], "-hs")){
        opts |= ESTCONDSCFB;
      } else if(!strcmp(argv[i], "-hu")){
        hu = TRUE;
      } else if(!strcmp(argv[i], "-attr")){
        if(++i >= argc) usage();
        cblistpush(attrs, argv[i], -1);
      } else if(!strcmp(argv[i], "-ord")){
        if(++i >= argc) usage();
        ord = argv[i];
      } else if(!strcmp(argv[i], "-max")){
        if(++i >= argc) usage();
        max = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-sk")){
        if(++i >= argc) usage();
        sk = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-aux")){
        if(++i >= argc) usage();
        aux = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-dis")){
        if(++i >= argc) usage();
        dis = argv[i];
      } else {
        usage();
      }
    } else {
      if(cbdatumsize(pbuf) > 0) cbdatumcat(pbuf, " ", 1);
      cbdatumcat(pbuf, argv[i], -1);
    }
  }
  if(!(phrase = est_iconv(cbdatumptr(pbuf), -1, g_inputcode, "UTF-8", NULL, NULL))){
    printferror("%s: unsupported encoding\n", g_inputcode);
    return 1;
  }
  cbstrtrim(phrase);
  for(i = 0; i < cblistnum(attrs); i++){
    if((tmp = est_iconv(cblistval(attrs, i, NULL), -1, g_inputcode, "UTF-8", NULL, NULL)) != NULL){
      cblistover(attrs, i, tmp, -1);
      free(tmp);
    }
  }
  rv = procmulti(dbnames, phrase, attrs, ord, max, sk, aux, opts, cd, dis, hu);
  free(phrase);
  return rv;
}


/* parse arguments of the randput command */
static int runrandput(int argc, char **argv){
  char *dbname, *dnstr;
  int i, dnum, rv;
  dbname = NULL;
  dnstr = NULL;
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-ren")){
        g_rdmode = RD_ENG;
      } else if(!strcmp(argv[i], "-rla")){
        g_rdmode = RD_LAT;
      } else if(!strcmp(argv[i], "-reu")){
        g_rdmode = RD_EURO;
      } else if(!strcmp(argv[i], "-ror")){
        g_rdmode = RD_ORI;
      } else if(!strcmp(argv[i], "-rjp")){
        g_rdmode = RD_JPN;
      } else if(!strcmp(argv[i], "-rch")){
        g_rdmode = RD_CHAO;
      } else if(!strcmp(argv[i], "-cs")){
        if(++i >= argc) usage();
        g_cachesize = strtod(argv[i], NULL) * 1024 * 1024;
      } else {
        usage();
      }
    } else if(!dbname){
      dbname = argv[i];
    } else if(!dnstr){
      dnstr = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname || !dnstr) usage();
  if((dnum = atoi(dnstr)) < 1) usage();
  rv = procrandput(dbname, dnum);
  return rv;
}


/* parse arguments of the wicked command */
static int runwicked(int argc, char **argv){
  char *dbname, *dnstr;
  int i, dnum, rv;
  dbname = NULL;
  dnstr = NULL;
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      usage();
    } else if(!dbname){
      dbname = argv[i];
    } else if(!dnstr){
      dnstr = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname || !dnstr) usage();
  if((dnum = atoi(dnstr)) < 1) usage();
  rv = procwicked(dbname, dnum);
  return rv;
}


/* parse arguments of the regression command */
static int runregression(int argc, char **argv){
  char *dbname;
  int i, rv;
  dbname = NULL;
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      usage();
    } else if(!dbname){
      dbname = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname) usage();
  rv = procregression(dbname);
  return rv;
}


/* perform the create command */
static int proccreate(const char *dbname, CBMAP *attrs){
  ESTDB *db;
  const char *kbuf;
  int ecode, ksiz, type;
  if(!(db = est_db_open(dbname, ESTDBWRITER | ESTDBCREAT | g_oextmodes, &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  est_db_set_informer(db, dbinform, NULL);
  cbmapiterinit(attrs);
  while((kbuf = cbmapiternext(attrs, &ksiz)) != NULL){
    type = strtoidxtype(cbmapget(attrs, kbuf, ksiz, NULL));
    if(!est_db_add_attr_index(db, kbuf, type)){
      printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
      est_db_close(db, &ecode);
      return 1;
    }
  }
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  return 0;
}


/* perform the put command */
static int procput(const char *dbname, const char *filename){
  ESTDB *db;
  ESTDOC *doc;
  const char *uri;
  char *draft;
  int ecode;
  if(!(draft = cbreadfile(filename, NULL))){
    printferror("%s: could not open", filename ? filename : "(stdin)");
    return 1;
  }
  if(!(db = est_db_open(dbname, ESTDBWRITER | ESTDBCREAT | g_oextmodes, &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    free(draft);
    return 1;
  }
  est_db_set_informer(db, dbinform, NULL);
  doc = est_doc_new_from_draft(draft);
  if(!est_db_put_doc(db, doc, g_putopts)){
    printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
    est_doc_delete(doc);
    est_db_close(db, &ecode);
    free(draft);
    return 1;
  }
  if(!(uri = est_doc_attr(doc, ESTDATTRURI))) uri = "";
  printfinfo("%d (%s): registered", est_doc_id(doc), uri);
  est_doc_delete(doc);
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    free(draft);
    return 1;
  }
  free(draft);
  return 0;
}


/* perform the out command */
static int procout(const char *dbname, int id, const char *expr){
  ESTDB *db;
  int ecode;
  if(!(db = est_db_open(dbname, ESTDBWRITER, &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  est_db_set_informer(db, dbinform, NULL);
  if(expr && (id = est_db_uri_to_id(db, expr)) < 1 &&
     (id = est_db_uri_to_id(db, exprtouri(expr))) < 1){
    printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
    est_db_close(db, &ecode);
    return 1;
  }
  if(!est_db_out_doc(db, id, g_outopts)){
    printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
    est_db_close(db, &ecode);
    return 1;
  }
  printfinfo("%d: deleted", id);
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  return 0;
}


/* perform the edit command */
static int procedit(const char *dbname, int id, const char *expr,
                    const char *name, const char *value){
  ESTDB *db;
  ESTDOC *doc;
  char *str;
  int ecode;
  if(!(db = est_db_open(dbname, ESTDBWRITER, &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  est_db_set_informer(db, dbinform, NULL);
  if(expr && (id = est_db_uri_to_id(db, expr)) < 1 &&
     (id = est_db_uri_to_id(db, exprtouri(expr))) < 1){
    printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
    est_db_close(db, &ecode);
    return 1;
  }
  if(!(doc = est_db_get_doc(db, id, ESTGDNOTEXT | ESTGDNOKWD))){
    printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
    est_db_close(db, &ecode);
    return 1;
  }
  str = g_pathcode ? est_iconv(value, -1, g_pathcode, "UTF-8", NULL, NULL) : NULL;
  est_doc_add_attr(doc, name, str ? str : value);
  free(str);
  if(!est_db_edit_doc(db, doc)){
    printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
    est_doc_delete(doc);
    est_db_close(db, &ecode);
    return 1;
  }
  est_doc_delete(doc);
  printfinfo("%d: edited", id);
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  return 0;
}


/* perform the get command */
static int procget(const char *dbname, const CBLIST *pidxs,
                   int id, const char *expr, const char *attr){
  ESTDB *db;
  ESTDOC *doc;
  char *draft;
  int i, ecode;
  if(!(db = est_db_open(dbname, ESTDBREADER | g_oextmodes, &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  for(i = 0; i < cblistnum(pidxs); i++){
    est_db_add_pseudo_index(db, cblistval(pidxs, i, NULL));
  }
  if(expr && (id = est_db_uri_to_id(db, expr)) < 1 &&
     (id = est_db_uri_to_id(db, exprtouri(expr))) < 1){
    printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
    est_db_close(db, &ecode);
    return 1;
  }
  if(attr){
    if(!(draft = est_db_get_doc_attr(db, id, attr))){
      printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
      est_db_close(db, &ecode);
      return 1;
    }
    printf("%s\n", draft);
    free(draft);
  } else {
    if(!(doc = est_db_get_doc(db, id, 0))){
      printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
      est_db_close(db, &ecode);
      return 1;
    }
    draft = est_doc_dump_draft(doc);
    printf("%s", draft);
    free(draft);
    est_doc_delete(doc);
  }
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  return 0;
}


/* perform the list command */
static int proclist(const char *dbname, int lp){
  ESTDB *db;
  const char *path;
  char *vbuf;
  int ecode, id;
  if(!(db = est_db_open(dbname, ESTDBREADER | g_oextmodes, &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  if(!est_db_iter_init(db, NULL)){
    printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
    est_db_close(db, &ecode);
    return 1;
  }
  while((id = est_db_iter_next(db)) > 0){
    if((vbuf = est_db_get_doc_attr(db, id, ESTDATTRURI)) != NULL){
      printf("%d\t%s", id, vbuf);
      if(lp && (path = urltopath(vbuf)) != NULL){
        printf("\t%s", path);
      }
      printf("\n");
      free(vbuf);
    }
  }
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  return 0;
}


/* perform the uriid command */
static int procuriid(const char *dbname, const CBLIST *pidxs, const char *expr){
  ESTDB *db;
  int i, ecode, id;
  if(!(db = est_db_open(dbname, ESTDBREADER | g_oextmodes, &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  for(i = 0; i < cblistnum(pidxs); i++){
    est_db_add_pseudo_index(db, cblistval(pidxs, i, NULL));
  }
  if((id = est_db_uri_to_id(db, expr)) < 1 &&
     (id = est_db_uri_to_id(db, exprtouri(expr))) < 1){
    printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
    est_db_close(db, &ecode);
    return 1;
  }
  printf("%d\n", id);
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  return 0;
}


/* perform the meta command */
static int procmeta(const char *dbname, const char *mname, const char *mvalue){
  ESTDB *db;
  CBLIST *names;
  char *vbuf;
  int i, ecode;
  if(!(db = est_db_open(dbname, mvalue ? (ESTDBWRITER | ESTDBCREAT) : (ESTDBREADER | ESTDBLCKNB),
                        &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  if(mname){
    if(mvalue){
      est_db_add_meta(db, mname, mvalue[0] != '\0' ? mvalue : NULL);
    } else {
      if((vbuf = est_db_meta(db, mname)) != NULL){
        printf("%s\n", vbuf);
        free(vbuf);
      }
    }
  } else {
    names = est_db_meta_names(db);
    for(i = 0; i < cblistnum(names); i++){
      printf("%s\n", cblistval(names, i, NULL));
    }
    cblistclose(names);
  }
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  return 0;
}


/* perform the inform command */
static int procinform(const char *dbname){
  ESTDB *db;
  CBLIST *exprs;
  int i, ecode;
  if(!(db = est_db_open(dbname, ESTDBREADER | g_oextmodes, &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  printf("number of documents: %d\n", est_db_doc_num(db));
  printf("number of words: %d\n", est_db_word_num(db));
  printf("number of keywords: %d\n", est_db_keyword_num(db));
  printf("file size: %.0f\n", est_db_size(db));
  printf("inode number: %d\n", est_db_inode(db));
  exprs = est_db_attr_index_exprs(db);
  printf("attribute indexes: ");
  for(i = 0; i < cblistnum(exprs); i++){
    if(i > 0) printf(", ");
    printf("%s", cblistval(exprs, i, NULL));
  }
  printf("\n");
  printf("known options: ");
  i = 0;
  if(est_db_check_option(db, ESTDBPERFNG)){
    if(i > 0) printf(", ");
    printf("perfng");
    i++;
  }
  if(est_db_check_option(db, ESTDBCHRCAT)){
    if(i > 0) printf(", ");
    printf("chrcat");
    i++;
  }
  if(est_db_check_option(db, ESTDBSCVOID)){
    if(i > 0) printf(", ");
    printf("scvoid");
    i++;
  }
  if(est_db_check_option(db, ESTDBSCINT)){
    if(i > 0) printf(", ");
    printf("scint");
    i++;
  }
  if(est_db_check_option(db, ESTDBSCASIS)){
    if(i > 0) printf(", ");
    printf("scasis");
    i++;
  }
  printf("\n");
  cblistclose(exprs);
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  return 0;
}


/* perform the optimize command */
static int procoptimize(const char *dbname){
  ESTDB *db;
  int ecode;
  time_t curtime;
  curtime = time(NULL);
  if(!(db = est_db_open(dbname, ESTDBWRITER, &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  est_db_set_informer(db, dbinform, NULL);
  if(!est_db_optimize(db, g_optopts)){
    printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
    est_db_close(db, &ecode);
    return 1;
  }
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  curtime = time(NULL) - curtime;
  printfinfo("finished successfully: elapsed time: %dh %dm %ds",
             (int)(curtime / 3600), (int)((curtime / 60) % 60), (int)(curtime % 60));
  return 0;
}


/* perform the merge command */
static int procmerge(const char *dbname, const char *tgname){
  ESTDB *db;
  int ecode;
  time_t curtime;
  curtime = time(NULL);
  if(!(db = est_db_open(dbname, ESTDBWRITER, &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  est_db_set_informer(db, dbinform, NULL);
  if(!est_db_merge(db, tgname, g_mgopts)){
    printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
    est_db_close(db, &ecode);
    return 1;
  }
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  curtime = time(NULL) - curtime;
  printfinfo("finished successfully: elapsed time: %dh %dm %ds",
             (int)(curtime / 3600), (int)((curtime / 60) % 60), (int)(curtime % 60));
  return 0;
}


/* perform the repair command */
static int procrepair(const char *dbname){
  int ecode;
  if(!est_db_repair(dbname, g_rpopts, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  return 0;
}


/* perform the search command */
static int procsearch(const char *dbname, const CBLIST *pidxs,
                      const char *phrase, const CBLIST *attrs,
                      const char *ord, int max, int sk, int aux, double ec,
                      int opts, int cd, int sim, const char *dis){
  ESTDB *db;
  ESTCOND *cond;
  ESTDOC *doc, *tdoc;
  CBDATUM *pbuf;
  CBMAP *svmap, *hints, *kwords;
  const CBLIST *texts;
  CBLIST *names, *words, *lines;
  const char *kbuf, *vbuf, *line;
  char *draft, path[URIBUFSIZ], numbuf[NUMBUFSIZ], *word, *pv;
  const int *ary;
  int i, j, ecode, knum, ksiz, vsiz, *res, rnum, hits, snum, id, sc, anum, fin, cnt;
  double curtime;
  if(!(db = est_db_open(dbname, ESTDBREADER | g_oextmodes, &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  for(i = 0; i < cblistnum(pidxs); i++){
    est_db_add_pseudo_index(db, cblistval(pidxs, i, NULL));
  }
  cond = est_cond_new();
  if(sim > 0){
    svmap = est_db_get_keywords(db, sim);
    if(!svmap && (doc = est_db_get_doc(db, sim, ESTGDNOATTR | ESTGDNOKWD)) != NULL){
      knum = g_kwordnum > 0 ? g_kwordnum : KWORDNUM;
      svmap = g_usemorph ? est_morph_etch_doc(doc, knum) :
        est_db_etch_doc((opts & ESTCONDNOIDF) ? NULL : db, doc, knum);
      est_doc_delete(doc);
    }
    if(svmap){
      pbuf = cbdatumopen(ESTOPSIMILAR, -1);
      cbmapiterinit(svmap);
      while((kbuf = cbmapiternext(svmap, &ksiz)) != NULL){
        vbuf = cbmapget(svmap, kbuf, ksiz, &vsiz);
        cbdatumcat(pbuf, " WITH ", -1);
        cbdatumcat(pbuf, vbuf, vsiz);
        cbdatumcat(pbuf, " ", 1);
        cbdatumcat(pbuf, kbuf, ksiz);
      }
      est_cond_set_phrase(cond, cbdatumptr(pbuf));
      cbdatumclose(pbuf);
      cbmapclose(svmap);
    }
    cd = FALSE;
  } else {
    while(*phrase > '\0' && *phrase <= ' '){
      phrase++;
    }
    if(phrase[0] != '\0' || cblistnum(attrs) < 1) est_cond_set_phrase(cond, phrase);
    if(phrase[0] == '[' || phrase[0] == '*') cd = FALSE;
  }
  for(i = 0; i < cblistnum(attrs); i++){
    est_cond_add_attr(cond, cblistval(attrs, i, NULL));
  }
  if(ord) est_cond_set_order(cond, ord);
  if(max >= 0 && sk < 1 && !cd) est_cond_set_max(cond, max);
  est_cond_set_options(cond, opts);
  est_cond_set_auxiliary(cond, aux);
  if(ec > 0.0) est_cond_set_eclipse(cond, ec);
  if(dis) est_cond_set_distinct(cond, dis);
  hints = cbmapopenex(MINIBNUM);
  curtime = est_gettimeofday();
  res = est_db_search(db, cond, &rnum, hints);
  hits = (vbuf = cbmapget(hints, "", 0, NULL)) ? atoi(vbuf) : rnum;
  if(max >= 0 && hits < max + 1 && est_cond_auxiliary_word(cond, "")){
    free(res);
    est_cond_set_auxiliary(cond, -1);
    res = est_db_search(db, cond, &rnum, hints);
    hits = (vbuf = cbmapget(hints, "", 0, NULL)) ? atoi(vbuf) : rnum;
  }
  curtime = est_gettimeofday() - curtime;
  if(g_viewmode == VM_XML){
    xmlprintf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    xmlprintf("<estresult xmlns=\"%@\" version=\"%@\">\n", _EST_XNSEARCH, est_version);
    xmlprintf("<meta>\n");
    xmlprintf("<hit number=\"%d\" auxiliary=\"%@\"/>\n",
              hits, est_cond_auxiliary_word(cond, "") ? "on" : "off");
    cbmapiterinit(hints);
    while((kbuf = cbmapiternext(hints, NULL)) != NULL){
      if(kbuf[0] == '\0') continue;
      vbuf = cbmapget(hints, kbuf, -1, NULL);
      xmlprintf("<hit key=\"%@\" number=\"%@\" auxiliary=\"%@\"/>\n",
                kbuf, vbuf, est_cond_auxiliary_word(cond, kbuf) ? "on" : "off");
    }
    xmlprintf("<time time=\"%.6f\"/>\n", curtime / 1000.0);
    xmlprintf("<total docnum=\"%d\" wordnum=\"%d\"/>\n", est_db_doc_num(db), est_db_word_num(db));
    xmlprintf("</meta>\n");
  } else {
    printf("%s\n", est_border_str());
    printf("VERSION\t%s\n", _EST_PROTVER);
    printf("NODE\tlocal\n");
    printf("HIT\t%d%s\n", hits, est_cond_auxiliary_word(cond, "") ? "+" : "");
    cbmapiterinit(hints);
    cnt = 1;
    while((kbuf = cbmapiternext(hints, NULL)) != NULL){
      if(kbuf[0] == '\0') continue;
      vbuf = cbmapget(hints, kbuf, -1, NULL);
      printf("HINT#%d\t%s\t%s%s\n",
             cnt, kbuf, vbuf, est_cond_auxiliary_word(cond, kbuf) ? "+" : "");
      cnt++;
    }
    printf("TIME\t%.6f\n", curtime / 1000.0);
    printf("DOCNUM\t%d\n", est_db_doc_num(db));
    printf("WORDNUM\t%d\n", est_db_word_num(db));
    switch(g_viewmode){
    case VM_ID:
      printf("VIEW\tID\n");
      break;
    case VM_URI:
      printf("VIEW\tURI\n");
      break;
    case VM_ATTR:
      printf("VIEW\tATTRIBUTE\n");
      break;
    case VM_FULL:
      printf("VIEW\tFULL\n");
      break;
    case VM_SNIP:
      printf("VIEW\tSNIPPET\n");
      break;
    case VM_HMRD:
      printf("VIEW\tHUMAN\n");
      break;
    }
    printf("\n");
    if(g_viewmode == VM_ID || g_viewmode == VM_URI || g_viewmode == VM_DUMP)
      printf("%s\n", est_border_str());
  }
  snum = max >= 0 && max < rnum ? max : rnum;
  for(i = 0; i < rnum && snum > 0; i++){
    id = res[i];
    sc = est_cond_score(cond, i);
    ary = est_cond_shadows(cond, id, &anum);
    switch(g_viewmode){
    case VM_URI:
      if((doc = est_db_get_doc(db, id, (cd ? 0 : ESTGDNOTEXT) | ESTGDNOKWD)) != NULL){
        if((cd && !est_db_scan_doc(db, doc, cond)) || sk-- > 0){
          est_doc_delete(doc);
          continue;
        }
        if(!(vbuf = est_doc_attr(doc, ESTDATTRURI))) vbuf = "";
        printf("%d\t%s", id, vbuf);
        if(sc >= 0) printf("\t%d", sc);
        printf("\n");
        est_doc_delete(doc);
        snum--;
      }
      break;
    case VM_ATTR:
    case VM_FULL:
    case VM_SNIP:
      if((doc = est_db_get_doc(db, id, ((g_viewmode != VM_ATTR || cd || g_kwordnum > 0) ?
                                        0 : ESTGDNOTEXT) | ESTGDNOKWD)) != NULL){
        if((cd && !est_db_scan_doc(db, doc, cond)) || sk-- > 0){
          est_doc_delete(doc);
          continue;
        }
        if(sc >= 0){
          sprintf(numbuf, "%d", sc);
          est_doc_add_attr(doc, DATTRNDSCORE, numbuf);
        }
        printf("%s\n", est_border_str());
        names = est_doc_attr_names(doc);
        for(j = 0; j < cblistnum(names); j++){
          kbuf = cblistval(names, j, NULL);
          vbuf = est_doc_attr(doc, kbuf);
          printf("%s=%s\n", kbuf, vbuf);
        }
        cblistclose(names);
        if((kwords = est_db_get_keywords(db, id)) != NULL || g_kwordnum > 0){
          if(!kwords) kwords = g_usemorph ? est_morph_etch_doc(doc, g_kwordnum) :
            est_db_etch_doc(db, doc, g_kwordnum);
          if(cbmaprnum(kwords) > 0){
            printf("%s", ESTDCNTLVECTOR);
            cbmapiterinit(kwords);
            for(j = 0; (kbuf = cbmapiternext(kwords, NULL)) != NULL; j++){
              if(j > 0) printf(" ");
              printf("\t%s\t%s", kbuf, cbmapget(kwords, kbuf, -1, NULL));
            }
            printf("\n");
          }
          cbmapclose(kwords);
        }
        for(j = 0; j < anum; j += 2){
          if(!(tdoc = est_db_get_doc(db, ary[j], ESTGDNOKWD))) continue;
          if(!(vbuf = est_doc_attr(tdoc, ESTDATTRURI))) vbuf = ".";
          printf("%s\t%s\t%1.3f\n", ESTDCNTLSHADOW, vbuf, ary[j+1] / 10000.0);
          est_doc_delete(tdoc);
        }
        printf("\n");
        if(g_viewmode == VM_FULL){
          texts = est_doc_texts(doc);
          for(j = 0; j < cblistnum(texts); j++){
            printf("%s\n", cblistval(texts, j, NULL));
          }
          vbuf = est_doc_hidden_texts(doc);
          if(vbuf[0] != '\0') printf("\t%s\n", vbuf);
        } else if(g_viewmode == VM_SNIP){
          words = est_hints_to_words(hints);
          draft = est_doc_make_snippet(doc, words, g_snipwwidth, g_sniphwidth, g_snipawidth);
          printf("%s", draft);
          free(draft);
          cblistclose(words);
        }
        est_doc_delete(doc);
        snum--;
      }
      break;
    case VM_HMRD:
      if((doc = est_db_get_doc(db, id, ESTGDNOKWD)) != NULL){
        if((cd && !est_db_scan_doc(db, doc, cond)) || sk-- > 0){
          est_doc_delete(doc);
          continue;
        }
        if(sc >= 0){
          sprintf(numbuf, "%d", sc);
          est_doc_add_attr(doc, DATTRNDSCORE, numbuf);
        }
        printf("%s\n\n", est_border_str());
        if(sc >= 0) printf("Score: %d\n", sc);
        if((vbuf = est_doc_attr(doc, ESTDATTRURI)) != NULL) printf("URI: %s\n", vbuf);
        if((vbuf = est_doc_attr(doc, ESTDATTRTITLE)) != NULL ||
           (vbuf = est_doc_attr(doc, DATTRLFILE)) != NULL) printf("Title: %s\n", vbuf);
        printf("\n");
        words = est_hints_to_words(hints);
        draft = est_doc_make_snippet(doc, words, g_snipwwidth, g_sniphwidth, g_snipawidth);
        lines = cbsplit(draft, -1, "\n");
        fin = TRUE;
        for(j = 0; j < cblistnum(lines); j++){
          line = cblistval(lines, j, NULL);
          if(line[0] != '\0'){
            word = cbmemdup(line, -1);
            if((pv = strchr(word, '\t')) != NULL) *pv = '\0';
            printf("%s", word);
            free(word);
            fin = TRUE;
          } else if(fin){
            printf(" ... ");
            fin = FALSE;
          }
        }
        cblistclose(lines);
        free(draft);
        cblistclose(words);
        printf("\n");
        if(anum > 0){
          printf("\n");
          for(j = 0; j < anum; j += 2){
            if(!(tdoc = est_db_get_doc(db, ary[j], ESTGDNOKWD))) continue;
            if(!(vbuf = est_doc_attr(tdoc, ESTDATTRURI))) vbuf = ".";
            printf("  Shadow: %s (%1.3f)\n", vbuf, ary[j+1] / 10000.0);
            est_doc_delete(tdoc);
          }
        }
        printf("\n\n");
        est_doc_delete(doc);
        snum--;
      }
      break;
    case VM_XML:
      if((doc = est_db_get_doc(db, id, 0)) != NULL){
        if((cd && !est_db_scan_doc(db, doc, cond)) || sk-- > 0){
          est_doc_delete(doc);
          continue;
        }
        if(sc >= 0){
          sprintf(numbuf, "%d", sc);
          est_doc_add_attr(doc, DATTRNDSCORE, numbuf);
        }
        if(!(vbuf = est_doc_attr(doc, ESTDATTRURI))) vbuf = "";
        xmlprintf("<document id=\"%d\" uri=\"%@\">\n", id, vbuf);
        names = est_doc_attr_names(doc);
        for(j = 0; j < cblistnum(names); j++){
          kbuf = cblistval(names, j, NULL);
          if(!strcmp(kbuf, ESTDATTRID) || !strcmp(kbuf, ESTDATTRURI)) continue;
          vbuf = est_doc_attr(doc, kbuf);
          xmlprintf("<attribute name=\"%@\" value=\"%@\"/>\n", kbuf, vbuf);
        }
        cblistclose(names);
        if((kwords = est_db_get_keywords(db, id)) != NULL || g_kwordnum > 0){
          if(!kwords) kwords = g_usemorph ? est_morph_etch_doc(doc, g_kwordnum) :
            est_db_etch_doc(db, doc, g_kwordnum);
          if(cbmaprnum(kwords) > 0){
            xmlprintf("<vector>");
            cbmapiterinit(kwords);
            while((kbuf = cbmapiternext(kwords, NULL)) != NULL){
              xmlprintf("<element key=\"%@\" number=\"%@\"/>",
                        kbuf, cbmapget(kwords, kbuf, -1, NULL));
            }
            xmlprintf("</vector>\n");
          }
          cbmapclose(kwords);
        }
        for(j = 0; j < anum; j += 2){
          if(!(tdoc = est_db_get_doc(db, ary[j], ESTGDNOKWD))) continue;
          if(!(vbuf = est_doc_attr(tdoc, ESTDATTRURI))) vbuf = ".";
          xmlprintf("<shadow uri=\"%@\" similarity=\"%1.3f\"/>\n", vbuf, ary[j+1] / 10000.0);
          est_doc_delete(tdoc);
        }
        words = est_hints_to_words(hints);
        draft = est_doc_make_snippet(doc, words, g_snipwwidth, g_sniphwidth, g_snipawidth);
        lines = cbsplit(draft, -1, "\n");
        fin = TRUE;
        xmlprintf("<snippet>");
        for(j = 0; j < cblistnum(lines); j++){
          line = cblistval(lines, j, NULL);
          if(line[0] != '\0'){
            word = cbmemdup(line, -1);
            if((pv = strchr(word, '\t')) != NULL){
              *pv = '\0';
              pv++;
              xmlprintf("<key normal=\"%@\">%@</key>", pv, word);
            } else {
              xmlprintf("%@", word);
            }
            free(word);
            fin = TRUE;
          } else if(fin){
            xmlprintf("<delimiter/>");
            fin = FALSE;
          }
        }
        xmlprintf("</snippet>\n");
        cblistclose(lines);
        free(draft);
        cblistclose(words);
        xmlprintf("</document>\n");
        est_doc_delete(doc);
        snum--;
      }
      break;
    case VM_DUMP:
      if((doc = est_db_get_doc(db, id, 0)) != NULL){
        if((cd && !est_db_scan_doc(db, doc, cond)) || sk-- > 0){
          est_doc_delete(doc);
          continue;
        }
        if(sc >= 0){
          sprintf(numbuf, "%d", sc);
          est_doc_add_attr(doc, DATTRNDSCORE, numbuf);
        }
        if(!(vbuf = est_doc_attr(doc, ESTDATTRURI))) vbuf = "";
        sprintf(path, "%08d%cest", id, ESTEXTCHR);
        printf("%s\t%s\n", path, vbuf);
        draft = est_doc_dump_draft(doc);
        if(!(cbwritefile(path, draft, -1))) printferror("%s: could not open", path);
        free(draft);
        est_doc_delete(doc);
        snum--;
      }
      break;
    default:
      if(sk-- > 0) continue;
      printf("%d", id);
      if(sc >= 0) printf("\t%d", sc);
      printf("\n");
      snum--;
      break;
    }
  }
  if(g_viewmode == VM_XML){
    xmlprintf("</estresult>\n");
  } else {
    printf("%s:END\n", est_border_str());
  }
  free(res);
  cbmapclose(hints);
  est_cond_delete(cond);
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  return 0;
}


/* perform the gather command */
static int procgather(const char *dbname, const char *filename){
  ESTDB *db;
  CBLIST *list, *clist, *attrs;
  FILE *ifp;
  const char *tmp;
  char *line, *path;
  int i, err, ecode, len;
  time_t curtime;
  struct stat sbuf;
  curtime = time(NULL);
  err = FALSE;
  if(stat(filename, &sbuf) != -1 && S_ISDIR(sbuf.st_mode)){
    printfinfo("reading list from the directory: %s", filename);
    if((db = est_db_open(dbname, ESTDBWRITER | ESTDBCREAT | g_oextmodes, &ecode)) != NULL){
      est_db_set_informer(db, dbinform, NULL);
      if(g_cachesize > 0){
        if(g_cachesize > CACHEMAX) g_cachesize = CACHEMAX;
        est_db_set_cache_size(db, g_cachesize, -1, -1, -1);
      }
      list = cblistopen();
      path = est_realpath(filename);
      cblistunshift(list, path, -1);
      free(path);
      while((line = cblistshift(list, &len)) != NULL){
        if(stat(line, &sbuf) != -1 && S_ISDIR(sbuf.st_mode) && (clist = cbdirlist(line)) != NULL){
          cblistsort(clist);
          for(i = cblistnum(clist) - 1; i >= 0; i--){
            tmp = cblistval(clist, i, NULL);
            if(!strcmp(tmp, ESTCDIRSTR) || !strcmp(tmp, ESTPDIRSTR)) continue;
            if(ESTPATHCHR == '\\' && dosbadname(tmp)) continue;
            path = (len > 0 && line[len-1] == ESTPATHCHR) ? cbsprintf("%s%s", line, tmp) :
              cbsprintf("%s%c%s", line, ESTPATHCHR, tmp);
            cblistunshift(list, path, -1);
            free(path);
          }
          cblistclose(clist);
        } else {
          if(!doputdoc(db, line, NULL)){
            printferror("%s: %s", line, est_err_msg(est_db_error(db)));
            err = TRUE;
          }
          doremovefile(line);
        }
        free(line);
        if(err || g_sigterm) break;
      }
      cblistclose(list);
      if(!est_db_close(db, &ecode)){
        printferror("%s: %s", dbname, est_err_msg(ecode));
        err = TRUE;
      }
    } else {
      printferror("%s: %s", dbname, est_err_msg(ecode));
      err = TRUE;
    }
  } else {
    if(!strcmp(filename, "-")){
      ifp = stdin;
      printfinfo("reading list from the standard input", filename);
    } else if((ifp = fopen(filename, "rb")) != NULL){
      printfinfo("reading list from the file: %s", filename);
    } else {
      printferror("%s: could not open", filename);
      return 1;
    }
    if((db = est_db_open(dbname, ESTDBWRITER | ESTDBCREAT | g_oextmodes, &ecode)) != NULL){
      est_db_set_informer(db, dbinform, NULL);
      if(g_cachesize > 0){
        if(g_cachesize > CACHEMAX) g_cachesize = CACHEMAX;
        est_db_set_cache_size(db, g_cachesize, -1, -1, -1);
      }
      while((line = fgetl(ifp)) != NULL){
        if(line[0] == '\0'){
          free(line);
          continue;
        }
        if(cblistnum(g_pathattrs) > 0){
          attrs = cbsplit(line, -1, "\t");
          path = cblistshift(attrs, NULL);
          if(!doputdoc(db, path, attrs)){
            printferror("%s: %s", path, est_err_msg(est_db_error(db)));
            err = TRUE;
          }
          doremovefile(path);
          free(path);
          cblistclose(attrs);
        } else {
          if(!doputdoc(db, line, NULL)){
            printferror("%s: %s", line, est_err_msg(est_db_error(db)));
            err = TRUE;
          }
          doremovefile(line);
        }
        free(line);
        if(err || g_sigterm) break;
      }
      if(!est_db_close(db, &ecode)){
        printferror("%s: %s", dbname, est_err_msg(ecode));
        err = TRUE;
      }
    } else {
      printferror("%s: %s", dbname, est_err_msg(ecode));
      err = TRUE;
    }
    if(ifp != stdin) fclose(ifp);
  }
  curtime = time(NULL) - curtime;
  if(!err) printfinfo("finished successfully: elapsed time: %dh %dm %ds",
                      (int)(curtime / 3600), (int)((curtime / 60) % 60), (int)(curtime % 60));
  return err ? 1 : 0;
}


/* perform the purge command */
static int procpurge(const char *dbname, const char *prefix, const CBLIST *attrs){
  ESTDB *db;
  ESTCOND *cond;
  ESTDOC *doc;
  const char *luri, *path;
  char *attr;
  int i, ecode, err, *res, rnum;
  time_t curtime;
  struct stat sbuf;
  curtime = time(NULL);
  if(!(db = est_db_open(dbname, ESTDBWRITER, &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  est_db_set_informer(db, dbinform, NULL);
  cond = est_cond_new();
  attr = cbsprintf("%s STRBW %s", DATTRLPATH, prefix ? exprtouri(prefix) : "");
  est_cond_add_attr(cond, attr);
  for(i = 0; i < cblistnum(attrs); i++){
    est_cond_add_attr(cond, cblistval(attrs, i, NULL));
  }
  res = est_db_search(db, cond, &rnum, NULL);
  err = FALSE;
  for(i = 0; i < rnum; i++){
    if(!(doc = est_db_get_doc(db, res[i], ESTGDNOTEXT | ESTGDNOKWD))) continue;
    if((luri = est_doc_attr(doc, DATTRLPATH)) != NULL){
      if((path = urltopath(luri)) != NULL){
        if(!g_doforce && stat(path, &sbuf) != -1){
          printfinfo("%s: passed", path);
        } else {
          if(g_nooper || est_db_out_doc(db, res[i], g_outopts)){
            printfinfo("%d (%s): deleted", res[i], path);
          } else {
            printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
            err = TRUE;
          }
        }
      } else {
        printfinfo("%s: ignored", luri);
      }
    } else if(g_doforce){
      luri = est_doc_attr(doc, ESTDATTRURI);
      if(g_nooper || est_db_out_doc(db, res[i], g_outopts)){
        printfinfo("%d (%s): deleted", res[i], luri ? luri : "-");
      } else {
        printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
        err = TRUE;
      }
    } else {
      printfinfo("(%d): ignored", res[i]);
    }
    est_doc_delete(doc);
    if(err || g_sigterm) break;
  }
  free(res);
  est_cond_delete(cond);
  free(attr);
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  curtime = time(NULL) - curtime;
  if(!err) printfinfo("finished successfully: elapsed time: %dh %dm %ds",
                      (int)(curtime / 3600), (int)((curtime / 60) % 60), (int)(curtime % 60));
  return err ? 1 : 0;
}


/* perform the extkeys command */
static int procextkeys(const char *dbname, const char *prefix, const CBLIST *attrs,
                       const char *dfdbname, int ni){
  static int count = 0;
  ESTDB *db;
  ESTCOND *cond;
  ESTDOC *doc;
  DEPOT *dfdb;
  CBMAP *kwords;
  const char *luri, *path, *vbuf;
  char *attr;
  int i, ecode, err, *res, rnum;
  double weight;
  time_t curtime;
  curtime = time(NULL);
  if(!(db = est_db_open(dbname, ESTDBWRITER, &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  est_db_set_informer(db, dbinform, NULL);
  dfdb = NULL;
  if(!ni && dfdbname){
    if(!(dfdb = dpopen(dfdbname, DP_OREADER, -1))){
      printferror("%s: the document frequency database has some errors", dfdbname);
      return -1;
    }
    est_db_set_dfdb(db, dfdb);
  }
  if(!ni && !dfdb && !g_usemorph && (!prefix || prefix[0] == '\0')) est_db_fill_key_cache(db);
  cond = est_cond_new();
  attr = cbsprintf("%s STRBW %s", DATTRLPATH, prefix ? exprtouri(prefix) : "");
  est_cond_add_attr(cond, attr);
  for(i = 0; i < cblistnum(attrs); i++){
    est_cond_add_attr(cond, cblistval(attrs, i, NULL));
  }
  res = est_db_search(db, cond, &rnum, NULL);
  err = FALSE;
  for(i = 0; i < rnum; i++){
    if(!g_doforce && est_db_measure_doc(db, res[i], ESTMDKWD) > 0){
      printfinfo("%d: passed", res[i]);
      continue;
    }
    if(!(doc = est_db_get_doc(db, res[i], ESTGDNOKWD))) continue;
    luri = est_doc_attr(doc, DATTRLPATH);
    if(!luri && !(luri = est_doc_attr(doc, ESTDATTRURI))) luri = "";
    kwords = g_usemorph ? est_morph_etch_doc(doc, g_kwordnum) :
      est_db_etch_doc(ni ? NULL : db, doc, g_kwordnum);
    weight = -1.0;
    if((g_putopts & ESTPDWEIGHT) && (vbuf = est_doc_attr(doc, ESTDATTRWEIGHT)) != NULL)
      weight = strtod(vbuf, NULL);
    if(weight < 0.0) weight = 1.0;
    if(g_nooper || est_db_put_keywords(db, res[i], kwords, weight)){
      path = urltopath(luri);
      printfinfo("%d (%s): extracted", res[i], path ? path : luri);
    } else {
      printferror("%s: the keyword database has some errors", dbname);
      err = TRUE;
    }
    cbmapclose(kwords);
    est_doc_delete(doc);
    if(count++ % MEMCHKFREQ == 0){
      if(est_memory_usage() > MEMLOADMAX && !est_db_flush(db, -1)){
        printferror("%s: %s", est_db_name(db), est_err_msg(est_db_error(db)));
        err = TRUE;
      }
      if(!g_nochkvmem && !cbvmemavail(MEMCHKALW)){
        printferror("exhausting memory");
        err = TRUE;
      }
    }
    if(err || g_sigterm) break;
  }
  free(res);
  est_cond_delete(cond);
  free(attr);
  if(dfdb) dpclose(dfdb);
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  curtime = time(NULL) - curtime;
  if(!err) printfinfo("finished successfully: elapsed time: %dh %dm %ds",
                      (int)(curtime / 3600), (int)((curtime / 60) % 60), (int)(curtime % 60));
  return err ? 1 : 0;
}


/* perform the words command */
static int procwords(const char *dbname, const char *dfdbname, int kw, int kt){
  ESTDB *db;
  ESTCOND *cond;
  DEPOT *dfdb;
  CBMAP *dfcc, *allkwd, *dockwd;
  KEYSC *scores;
  const char *kbuf, *vbuf;
  char *word, numbuf[NUMBUFSIZ];
  int i, ecode, err, *res, rnum, ksiz, sc, scnum, len, num;
  if(!(db = est_db_open(dbname, ESTDBREADER | g_oextmodes, &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  dfdb = NULL;
  if(dfdbname){
    if(!(dfdb = dpopen(dfdbname, DP_OWRITER | DP_OCREAT, est_db_word_num(db) * 2 + 1))){
      printferror("%s: the document frequency database has some errors", dfdbname);
      est_db_close(db, &ecode);
      return 1;
    }
    dpsetalign(dfdb, 4);
  }
  if(kt){
    kw = TRUE;
    dfcc = cbmapopen();
    est_db_keyword_iter_init(db);
    while((word = est_db_keyword_iter_next(db)) != NULL){
      num = est_db_keyword_rec_size(db, word);
      cbmapput(dfcc, word, -1, (char *)&num, sizeof(int), FALSE);
      free(word);
    }
    cond = est_cond_new();
    est_cond_set_options(cond, ESTCONDNOIDF | ESTCONDSCFB);
    est_cond_set_auxiliary(cond, 1);
  } else {
    dfcc = NULL;
    cond = NULL;
  }
  if(kw ? !est_db_keyword_iter_init(db) : !est_db_word_iter_init(db)){
    printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
    err = TRUE;
  }
  err = FALSE;
  while(!err && (word = kw ? est_db_keyword_iter_next(db) : est_db_word_iter_next(db)) != NULL){
    if(kt){
      est_cond_set_phrase(cond, word);
      res = est_db_search(db, cond, &rnum, NULL);
      allkwd = cbmapopenex(MINIBNUM);
      for(i = 0; i < rnum && i < RELWDDMAX; i++){
        if(!(dockwd = est_db_get_keywords(db, res[i]))) continue;
        cbmapiterinit(dockwd);
        while((kbuf = cbmapiternext(dockwd, &ksiz)) != NULL){
          num = ((vbuf = cbmapget(dfcc, kbuf, ksiz, NULL)) != NULL ? *(int *)vbuf : 0) + 1024;
          sc = ((vbuf = cbmapget(allkwd, kbuf, ksiz, NULL)) != NULL ? atoi(vbuf) : 0) +
            pow((atoi(cbmapget(dockwd, kbuf, ksiz, NULL)) + 100) *
                pow(est_cond_score(cond, i) / 50.0, 1.2) / pow(num, 0.4), 0.7);
          if(ksiz < 3 || (((unsigned char *)kbuf)[0] >= 0xe0 && ksiz < 4)) sc /= 1.414;
          sprintf(numbuf, "%d", sc);
          cbmapput(allkwd, kbuf, ksiz, numbuf, -1, TRUE);
        }
        cbmapclose(dockwd);
      }
      scores = cbmalloc(cbmaprnum(allkwd) * sizeof(KEYSC) + 1);
      scnum = 0;
      cbmapiterinit(allkwd);
      while((kbuf = cbmapiternext(allkwd, &ksiz)) != NULL){
        scores[scnum].word = kbuf;
        scores[scnum].pt = atoi(cbmapget(allkwd, kbuf, ksiz, NULL));
        scnum++;
      }
      qsort(scores, scnum, sizeof(KEYSC), keysc_compare);
      printf("%s\t%d", word, rnum);
      for(i = 0; i < scnum && i < RELWDNUM; i++){
        printf("\t%s\t%d", scores[i].word, scores[i].pt);
      }
      printf("\n");
      free(scores);
      cbmapclose(allkwd);
      free(res);
    } else if(dfdb){
      if((len = dpgetwb(dfdb, word, -1, 0, NUMBUFSIZ - 1, numbuf)) > 0){
        numbuf[len] = '\0';
        num = atoi(numbuf);
      } else {
        num = 0;
      }
      if(kw){
        rnum = est_db_keyword_rec_size(db, word);
      } else {
        rnum = est_db_word_rec_size(db, word);
      }
      sprintf(numbuf, "%d", num + rnum);
      if(!dpput(dfdb, word, -1, numbuf, -1, DP_DOVER)){
        printferror("%s: the document frequency database has some errors", dfdbname);
        err = TRUE;
      }
    } else {
      if(kw){
        rnum = est_db_keyword_rec_size(db, word);
      } else {
        rnum = est_db_word_rec_size(db, word);
      }
      printf("%s\t%d\n", word, rnum);
    }
    free(word);
    if(g_sigterm) break;
  }
  if(cond) est_cond_delete(cond);
  if(dfcc) cbmapclose(dfcc);
  if(dfdb && !dpclose(dfdb)){
    printferror("%s: the document frequency database has some errors", dfdbname);
    err = TRUE;
  }
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  return err ? 1 : 0;
}


/* perform the draft command */
static int procdraft(const char *filename){
  ESTDOC *doc;
  CBMAP *kwords;
  struct stat sbuf;
  const char *kbuf;
  char *buf, *tbuf, *draft;
  int size, ksiz;
  if(!(buf = cbreadfile(filename, &size))){
    printferror("%s: could not open", filename ? filename : "(stdin)");
    return 1;
  }
  switch(g_filefmt){
  case FF_TEXT:
    doc = est_doc_new_from_text(buf, size, g_inputcode, g_inputlang, g_bincheck);
    break;
  case FF_HTML:
    doc = est_doc_new_from_html(buf, size, g_inputcode, g_inputlang, g_bincheck);
    break;
  case FF_MIME:
    doc = est_doc_new_from_mime(buf, size, g_inputcode, g_inputlang, g_bincheck);
    break;
  default:
    doc = est_doc_new_from_draft_enc(buf, size, g_inputcode);
    break;
  }
  if(!doc){
    printferror("%s: extraction failed", filename ? filename : "(stdin)");
    free(buf);
    return 1;
  }
  if(g_limittsiz >= 0) est_doc_slim(doc, g_limittsiz);
  if(filename && stat(filename, &sbuf) == 0){
    if(!est_doc_attr(doc, ESTDATTRURI)) est_doc_add_attr(doc, ESTDATTRURI, pathtourl(filename));
    if(!est_doc_attr(doc, ESTDATTRMDATE)){
      tbuf = cbdatestrwww(sbuf.st_mtime, 0);
      est_doc_add_attr(doc, ESTDATTRMDATE, tbuf);
      free(tbuf);
    }
  }
  if(g_kwordnum > 0){
    kwords = g_usemorph ? est_morph_etch_doc(doc, g_kwordnum) :
      est_db_etch_doc(NULL, doc, g_kwordnum);
    cbmapiterinit(kwords);
    while((kbuf = cbmapiternext(kwords, &ksiz)) != NULL){
      printf("%s\t%s\n", kbuf, cbmapget(kwords, kbuf, ksiz, NULL));
    }
    cbmapclose(kwords);
  } else {
    draft = est_doc_dump_draft(doc);
    printf("%s", draft);
    free(draft);
  }
  est_doc_delete(doc);
  free(buf);
  return 0;
}


/* perform the break command */
static int procbreak(const char *filename, int wt){
  CBLIST *words;
  char *str, *phrase;
  int i, size;
  if(filename && filename[0] == '@'){
    str = cbmemdup(filename + 1, -1);
    size = strlen(str);
  } else if(!(str = cbreadfile(filename, &size))){
    printferror("%s: could not open", filename ? filename : "(stdin)");
    return 1;
  }
  if(!g_inputcode) g_inputcode = est_enc_name(str, size, g_inputlang);
  if(!(phrase = est_iconv(str, size, g_inputcode, "UTF-8", NULL, NULL))){
    printferror("%s: unsupported encoding\n", g_inputcode);
    free(str);
    return 1;
  }
  words = cblistopen();
  if(g_oextmodes & ESTDBPERFNG){
    est_break_text_perfng(phrase, words, TRUE, wt);
  } else if(g_oextmodes & ESTDBCHRCAT){
    est_break_text_chrcat(phrase, words, TRUE);
  } else {
    est_break_text(phrase, words, TRUE, wt);
  }
  for(i = 0; i < cblistnum(words); i++){
    printf("%s\n", cblistval(words, i, NULL));
  }
  cblistclose(words);
  free(phrase);
  free(str);
  return 0;
}


/* perform the iconv command */
static int prociconv(const char *filename){
  char *istr, *ostr;
  int i, isiz, osiz;
  if(filename && filename[0] == '@'){
    istr = cbmemdup(filename + 1, -1);
    isiz = strlen(istr);
  } else if(!(istr = cbreadfile(filename, &isiz))){
    printferror("%s: could not open", filename ? filename : "(stdin)");
    return 1;
  }
  if(!g_inputcode) g_inputcode = est_enc_name(istr, isiz, g_inputlang);
  if(!(ostr = est_iconv(istr, isiz, g_inputcode, g_outputcode, &osiz, NULL))){
    printferror("%s: unsupported encoding\n", g_inputcode);
    free(istr);
    return 1;
  }
  for(i = 0; i < osiz; i++){
    putchar(ostr[i]);
  }
  free(ostr);
  free(istr);
  return 0;
}


/* perform the regex command */
static int procregex(const char *regex, const char *filename, const char *repl, int inv, int ci){
  FILE *ifp;
  char *regstr, *str, *tmp;
  void *regobj;
  regstr = cbsprintf("%s%s", ci ? "*I:" : "", regex);
  if(!(regobj = est_regex_new(regstr))){
    printferror("%s: invalid regular expressions", regex);
    free(regstr);
    return 1;
  }
  if(!(ifp = filename ? fopen(filename, "rb") : stdin)){
    printferror("%s: could not open", filename ? filename : "(stdin)");
    est_regex_delete(regobj);
    free(regstr);
    return 1;
  }
  while((str = fgetl(ifp)) != NULL){
    if(repl){
      if((tmp = est_regex_replace(str, regstr, repl)) != NULL){
        printf("%s\n", tmp);
        free(tmp);
      }
    } else if(inv){
      if(!est_regex_match(regobj, str)) printf("%s\n", str);
    } else {
      if(est_regex_match(regobj, str)) printf("%s\n", str);
    }
    free(str);
  }
  if(ifp != stdin) fclose(ifp);
  est_regex_delete(regobj);
  free(regstr);
  return 0;
}


/* perform the scandir command */
static int procscandir(const char *dirname, int tmode, int pmode){
  CBLIST *list, *clist;
  const char *tmp;
  char *line, *path;
  int i, len;
  struct stat sbuf;
  list = cblistopen();
  cblistunshift(list, dirname, -1);
  while((line = cblistshift(list, &len)) != NULL){
    if(stat(line, &sbuf) != -1 && S_ISDIR(sbuf.st_mode) && (clist = cbdirlist(line)) != NULL){
      if(tmode != ST_FILE){
        switch(pmode){
        default:
          printf("%s\n", line);
          break;
        case SP_ABS:
          path = est_realpath(line);
          printf("%s\n", path);
          free(path);
          break;
        case SP_URL:
          printf("%s\n", pathtourl(line));
          break;
        }
      }
      cblistsort(clist);
      for(i = cblistnum(clist) - 1; i >= 0; i--){
        tmp = cblistval(clist, i, NULL);
        if(!strcmp(tmp, ESTCDIRSTR) || !strcmp(tmp, ESTPDIRSTR)) continue;
        if(ESTPATHCHR == '\\' && dosbadname(tmp)) continue;
        path = (len > 0 && line[len-1] == ESTPATHCHR) ? cbsprintf("%s%s", line, tmp) :
          cbsprintf("%s%c%s", line, ESTPATHCHR, tmp);
        cblistunshift(list, path, -1);
        free(path);
      }
      cblistclose(clist);
    } else if(tmode != ST_DIR){
      switch(pmode){
      default:
        printf("%s\n", line);
        break;
      case SP_ABS:
        path = est_realpath(line);
        printf("%s\n", path);
        free(path);
        break;
      case SP_URL:
        printf("%s\n", pathtourl(line));
        break;
      }
    }
    free(line);
  }
  cblistclose(list);
  return 0;
}


/* perform the multi command */
static int procmulti(const CBLIST *dbnames, const char *phrase, const CBLIST *attrs,
                     const char *ord, int max, int sk, int aux, int opts, int cd,
                     const char *dis, int hu){
  ESTDB **dbs;
  ESTCOND *cond;
  ESTDOC *doc;
  CBMAP *hints;
  const char *dbname, *kbuf, *vbuf;
  char *uri;
  int i, err, dbnum, dnum, wnum, ecode, *res, rnum, hits, cnt, snum, dbidx, id, sc;
  double curtime;
  err = FALSE;
  dbs = cbmalloc(sizeof(ESTDB *) * cblistnum(dbnames) + 1);
  dbnum = 0;
  dnum = 0;
  wnum = 0;
  for(i = 0; i < cblistnum(dbnames); i++){
    dbname = cblistval(dbnames, i, NULL);
    if((dbs[dbnum] = est_db_open(dbname, ESTDBREADER | g_oextmodes, &ecode)) != NULL){
      dnum += est_db_doc_num(dbs[dbnum]);
      wnum += est_db_word_num(dbs[dbnum]);
      dbnum++;
    } else {
      printferror("%s: %s", dbname, est_err_msg(ecode));
      err = TRUE;
    }
  }
  cond = est_cond_new();
  while(*phrase > '\0' && *phrase <= ' '){
    phrase++;
  }
  if(phrase[0] != '\0' || cblistnum(attrs) < 1) est_cond_set_phrase(cond, phrase);
  if(phrase[0] == '[' || phrase[0] == '*') cd = FALSE;
  for(i = 0; i < cblistnum(attrs); i++){
    est_cond_add_attr(cond, cblistval(attrs, i, NULL));
  }
  if(ord) est_cond_set_order(cond, ord);
  if(max >= 0 && sk < 1 && !cd) est_cond_set_max(cond, max);
  est_cond_set_options(cond, opts);
  est_cond_set_auxiliary(cond, aux);
  if(dis) est_cond_set_distinct(cond, dis);
  hints = cbmapopenex(MINIBNUM);
  curtime = est_gettimeofday();
  res = est_db_search_meta(dbs, dbnum, cond, &rnum, hints);
  hits = (vbuf = cbmapget(hints, "", 0, NULL)) ? atoi(vbuf) : rnum / 2;
  if(max >= 0 && hits < max + 1 && est_cond_auxiliary_word(cond, "")){
    free(res);
    est_cond_set_auxiliary(cond, -1);
    res = est_db_search_meta(dbs, dbnum, cond, &rnum, hints);
    hits = (vbuf = cbmapget(hints, "", 0, NULL)) ? atoi(vbuf) : rnum / 2;
  }
  printf("%s\n", est_border_str());
  printf("VERSION\t%s\n", _EST_PROTVER);
  printf("NODE\tlocal\n");
  printf("HIT\t%d%s\n", hits, est_cond_auxiliary_word(cond, "") ? "+" : "");
  cbmapiterinit(hints);
  cnt = 1;
  while((kbuf = cbmapiternext(hints, NULL)) != NULL){
    if(kbuf[0] == '\0') continue;
    vbuf = cbmapget(hints, kbuf, -1, NULL);
    printf("HINT#%d\t%s\t%s%s\n",
           cnt, kbuf, vbuf, est_cond_auxiliary_word(cond, kbuf) ? "+" : "");
    cnt++;
  }
  curtime = est_gettimeofday() - curtime;
  printf("TIME\t%.6f\n", curtime / 1000.0);
  printf("DOCNUM\t%d\n", dnum);
  printf("WORDNUM\t%d\n", wnum);
  printf("VIEW\tMULTI\n");
  printf("\n");
  printf("%s\n", est_border_str());
  snum = max >= 0 && max < rnum ? max : rnum / 2;
  for(i = 0; i < rnum && snum > 0; i += 2){
    dbidx = res[i];
    id = res[i+1];
    sc = est_cond_score(cond, i / 2);
    if(cd || hu){
      if(!(doc = est_db_get_doc(dbs[dbidx], id, (cd ? 0 : ESTGDNOTEXT) | ESTGDNOKWD))) continue;
      if(cd && !est_db_scan_doc(dbs[dbidx], doc, cond)){
        est_doc_delete(doc);
        continue;
      }
      vbuf = hu ? est_doc_attr(doc, ESTDATTRURI) : NULL;
      uri = vbuf ? cbmemdup(vbuf, -1) : NULL;
      est_doc_delete(doc);
    } else {
      uri = NULL;
    }
    if(sk-- > 0){
      free(uri);
      continue;
    }
    printf("%d\t%d", dbidx, id);
    if(sc >= 0) printf("\t%d", sc);
    if(uri) printf("\t%s", uri);
    printf("\n");
    free(uri);
    snum--;
  }
  printf("%s:END\n", est_border_str());
  free(res);
  cbmapclose(hints);
  est_cond_delete(cond);
  for(i = 0; i < dbnum; i++){
    if(!est_db_close(dbs[i], &ecode)){
      printferror("dbs[%d]: %s", i, est_err_msg(ecode));
      err = TRUE;
    }
  }
  free(dbs);
  return err ? 1 : 0;
}


/* perform the randput command */
static int procrandput(const char *dbname, int dnum){
  ESTDB *db;
  ESTDOC *doc;
  const char *mode;
  char uri[URIBUFSIZ];
  int i, ecode, err;
  time_t curtime;
  curtime = time(NULL);
  if(!(db = est_db_open(dbname, ESTDBWRITER | ESTDBCREAT | ESTDBTRUNC, &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  est_db_set_informer(db, dbinform, NULL);
  if(g_cachesize > 0){
    if(g_cachesize > CACHEMAX) g_cachesize = CACHEMAX;
    est_db_set_cache_size(db, g_cachesize, -1, -1, -1);
  }
  err = FALSE;
  for(i = 0; i < dnum; i++){
    doc = est_doc_new_from_chaos(RDOCCNUM, RDOCSNUM, g_rdmode);
    sprintf(uri, "file:///tmp/randput-%08d-%05d.est", i + 1, (int)getpid());
    est_doc_add_attr(doc, ESTDATTRURI, uri);
    if(est_db_put_doc(db, doc, 0)){
      if(!(mode = est_doc_attr(doc, "mode"))) mode = "unknown";
      printfinfo("%d (%s) (%s): registered", est_doc_id(doc), uri, mode);
    } else {
      printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
      err = TRUE;
    }
    est_doc_delete(doc);
    if(err || g_sigterm) break;
  }
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  curtime = time(NULL) - curtime;
  if(!err) printfinfo("finished successfully: elapsed time: %dh %dm %ds",
                      (int)(curtime / 3600), (int)((curtime / 60) % 60), (int)(curtime % 60));
  return err ? 1 : 0;
}


/* perform the wicked command */
static int procwicked(const char *dbname, int dnum){
  ESTDB *db;
  ESTDOC *doc;
  ESTCOND *cond;
  CBLIST *words;
  char uri[URIBUFSIZ], *oper, *value, *first, *second, *phrase;
  int i, j, ecode, err, *res, rnum, size;
  double rnd;
  time_t curtime;
  curtime = time(NULL);
  if(!(db = est_db_open(dbname, ESTDBWRITER | ESTDBCREAT | ESTDBTRUNC, &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  est_db_set_informer(db, dbinform, NULL);
  est_db_set_cache_size(db, 1024 * 1024 * 128, 1024, 256, 64);
  est_db_set_special_cache(db, ESTDATTRURI, 128);
  err = FALSE;
  for(i = 0; i < dnum; i++){
    rnd = est_random();
    if((int)(rnd * INT_MAX) % dnum < 5){
      rnd = est_random();
      if(rnd < 0.3){
        if(!est_db_close(db, &ecode)){
          printferror("%s: %s", dbname, est_err_msg(ecode));
          return 1;
        }
        if(!(db = est_db_open(dbname, ESTDBWRITER, &ecode))){
          printferror("%s: %s", dbname, est_err_msg(ecode));
          return 1;
        }
        est_db_set_informer(db, dbinform, NULL);
        est_db_set_cache_size(db, 1024 * 1024 * 128, 1024, 256, 64);
        est_db_set_special_cache(db, ESTDATTRURI, i / 10 + 1);
      } else if(rnd < 0.5){
        if(!est_db_optimize(db, (int)(est_random() * INT_MAX) % 2 == 0) ? ESTOPTNOPURGE : 0)
          err = TRUE;
      } else if(rnd < 0.8){
        if(!est_db_flush(db, 1024)) err = TRUE;
      } else {
        if(!est_db_sync(db)) err = TRUE;
      }
    } else if(rnd < 0.05){
      if(est_db_out_doc(db, (int)(est_random() * INT_MAX) % (i + 1) + 1,
                        ((int)(est_random() * INT_MAX) % 2 == 0) ? ESTODCLEAN : 0)){
        printfinfo("[%d:%d]: out", i + 1, est_db_doc_num(db));
      } else if(est_db_error(db) != ESTENOITEM){
        err = TRUE;
      }
    } else if(rnd < 0.1){
      if((value = est_db_get_doc_attr(db, (int)(est_random() * INT_MAX) % (i + 1) + 1,
                                      ESTDATTRURI)) != NULL){
        printfinfo("[%d:%d]: attr: %s", i + 1, est_db_doc_num(db), value);
        free(value);
      }
    } else if(rnd < 0.25){
      rnd = est_random();
      if(rnd < 0.5){
        oper = " OR ";
      } else if(rnd < 0.7){
        oper = " AND ";
      } else if(rnd < 0.8){
        oper = " NOTAND ";
      } else if(rnd < 0.9){
        oper = " ";
      } else {
        oper = "";
      }
      first = est_random_str(5, (int)(est_random() * INT_MAX) % RD_RAND);
      second = est_random_str(2, (int)(est_random() * INT_MAX) % RD_RAND);
      phrase = cbsprintf("%s%s%s", first, oper, second);
      cond = est_cond_new();
      est_cond_set_phrase(cond, phrase);
      if(est_random() < 0.25) est_cond_add_attr(cond, "@uri STREW 0.est");
      if(est_random() < 0.25) est_cond_set_order(cond, "@uri STRD");
      if(est_random() < 0.05) est_cond_set_options(cond, ESTCONDSURE | ESTCONDSCFB);
      if(est_random() < 0.05) est_cond_set_options(cond, ESTCONDAGITO | ESTCONDNOIDF);
      res = est_db_search(db, cond, &rnum, NULL);
      printfinfo("[%d:%d]: search: %d hits", i + 1, est_db_doc_num(db), rnum);
      if(est_random() < 0.05){
        for(j = 0; j < rnum && j < 100; j++){
          if((doc = est_db_get_doc(db, res[j], 0)) != NULL){
            if(i % 10 == 0){
              free(est_doc_cat_texts(doc));
              free(est_doc_dump_draft(doc));
              words = cblistopen();
              cblistpush(words, "vw", -1);
              cblistpush(words, "xy", -1);
              cblistpush(words, "z", -1);
              free(est_doc_make_snippet(doc, words, 100, 10, 10));
              cblistclose(words);
            }
            est_doc_delete(doc);
          } else if(est_db_error(db) != ESTENOITEM){
            err = TRUE;
          }
        }
      }
      free(res);
      est_cond_delete(cond);
      free(phrase);
      free(first);
      free(second);
    } else {
      doc = est_doc_new_from_chaos(100, 3, est_random() < 0.5 ? RD_EURO : RD_RAND);
      if(est_random() < 0.2){
        sprintf(uri, "file:///tmp/wicked-%08d-%05d.est",
                (int)(est_random() * INT_MAX) % (i + 1) + 1, (int)getpid());
      } else {
        sprintf(uri, "file:///tmp/wicked-%08d-%05d.est", i + 1, (int)getpid());
      }
      est_doc_add_attr(doc, ESTDATTRURI, uri);
      if(!est_db_put_doc(db, doc, est_random() < 0.5 ? ESTPDCLEAN : 0)) err = TRUE;
      if(est_random() < 0.1){
        if(!est_db_set_doc_entity(db, est_doc_id(doc), uri, strlen(uri))) err = TRUE;
        value = est_db_get_doc_entity(db, est_doc_id(doc), &size);
        if(!value || strcmp(value, uri)) err = TRUE;
        free(value);
      }
      est_doc_delete(doc);
    }
    if(err || g_sigterm) break;
  }
  if(err) printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  curtime = time(NULL) - curtime;
  if(!err) printfinfo("finished successfully: elapsed time: %dh %dm %ds",
                      (int)(curtime / 3600), (int)((curtime / 60) % 60), (int)(curtime % 60));
  return err ? 1 : 0;
}


/* perform the regression command */
static int procregression(const char *dbname){
  ESTDB *db, **dbs;
  ESTDOC *doc, *ndoc;
  ESTCOND *cond, *ncond;
  ESTRESMAPELEM *elems;
  CBMAP *hints, *rmap;
  const int *ary;
  char numbuf[NUMBUFSIZ];
  int i, ecode, err, *res, rnum, anum, onum;
  time_t curtime;
  curtime = time(NULL);
  printfinfo("# opening the database");
  if(!(db = est_db_open(dbname, ESTDBWRITER | ESTDBCREAT | ESTDBTRUNC, &ecode))){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  est_db_set_informer(db, dbinform, NULL);
  err = FALSE;
  if(!err){
    printfinfo("# checking registration of small documents");
    doc = est_doc_new();
    est_doc_add_attr(doc, ESTDATTRURI, "file:///small/one");
    est_doc_add_text(doc, "One!");
    est_doc_add_hidden_text(doc, "(Check it out, come on!)");
    if(!est_db_put_doc(db, doc, ESTPDCLEAN)) err = TRUE;
    est_doc_delete(doc);
    doc = est_doc_new();
    est_doc_add_attr(doc, ESTDATTRURI, "file:///small/two");
    est_doc_add_text(doc, "Two!!");
    est_doc_add_hidden_text(doc, "(Check it out, come on!)");
    if(!est_db_put_doc(db, doc, ESTPDCLEAN)) err = TRUE;
    est_doc_delete(doc);
    doc = est_doc_new();
    est_doc_add_attr(doc, ESTDATTRURI, "file:///small/three");
    est_doc_add_text(doc, "Three!!!");
    est_doc_add_hidden_text(doc, "(Check it out, come on!)");
    if(!est_db_put_doc(db, doc, ESTPDCLEAN)) err = TRUE;
    est_doc_delete(doc);
    doc = est_doc_new();
    est_doc_add_attr(doc, ESTDATTRURI, "file:///empty");
    if(!est_db_put_doc(db, doc, ESTPDCLEAN)) err = TRUE;
    est_doc_delete(doc);
  }
  if(!err){
    printfinfo("# checking registration of an english document");
    doc = est_doc_new();
    est_doc_add_attr(doc, ESTDATTRURI, "file:///english");
    est_doc_add_attr(doc, ESTDATTRTITLE, "Hyper Estraier");
    est_doc_add_text(doc, "% This is a displayed sentence. ;-)");
    est_doc_add_text(doc, "Hyper Estraier is a full-text search system for communities.");
    est_doc_add_text(doc, "A little suffering is good for the soul.");
    est_doc_add_text(doc, "They have been at a great feast of languages, and stolen the scraps.");
    est_doc_add_hidden_text(doc, "(Give it up, Yo!  Give it up, Yo!)");
    est_doc_add_hidden_text(doc, "% This is a hidden sentence. :-<");
    est_doc_add_hidden_text(doc, "(Check it out, come on!)");
    est_doc_add_hidden_text(doc, "");
    if(!est_db_put_doc(db, doc, ESTPDCLEAN)) err = TRUE;
    est_doc_delete(doc);
  }
  if(!err){
    printfinfo("# checking registration of a japanese document");
    doc = est_doc_new();
    est_doc_add_attr(doc, ESTDATTRURI, "file:///japanese");
    est_doc_add_attr(doc, ESTDATTRTITLE, "\xe5\xb9\xb3\xe6\x9e\x97\xe5\xb9\xb9\xe9\x9b\x84");
    est_doc_add_text(doc, "\xe6\x9c\xac\xe6\x97\xa5\xe3\x81\xaf\xe6\x99\xb4\xe5\xa4\xa9\xe3"
                     "\x81\xaa\xe3\x82\x8a\xe3\x80\x82");
    est_doc_add_text(doc, "\xe6\x9c\x95\xe3\x81\xaf\xe5\x9b\xbd\xe5\xae\xb6\xe7\xac\xac\xe4"
                     "\xb8\x80\xe3\x81\xae\xe4\xb8\x8b\xe5\x83\x95\xe3\x81\xa7\xe3\x81"
                     "\x82\xe3\x82\x8b\xe3\x80\x82");
    est_doc_add_hidden_text(doc, "(Check it out, come on!)");
    if(!est_db_put_doc(db, doc, ESTPDCLEAN)) err = TRUE;
    est_doc_delete(doc);
  }
  if(!err){
    printfinfo("# checking duplication of documents");
    doc = est_doc_new();
    est_doc_add_attr(doc, ESTDATTRURI, "file:///duplication");
    est_doc_add_text(doc, "Gamble, you gatta chance to make a Rumble!");
    est_doc_add_hidden_text(doc, "(Check it out, come on!)");
    if(!est_db_put_doc(db, doc, ESTPDCLEAN)) err = TRUE;
    est_doc_delete(doc);
    doc = est_doc_new();
    est_doc_add_attr(doc, ESTDATTRURI, "file:///duplication");
    est_doc_add_text(doc, "bring back hey, one more time!");
    est_doc_add_hidden_text(doc, "(Check it out, come on!)");
    if(!est_db_put_doc(db, doc, ESTPDCLEAN)) err = TRUE;
    est_doc_delete(doc);
    if(est_db_doc_num(db) != 7){
      printferror("%s: the number of documents is invalid", dbname);
      err = TRUE;
    }
  }
  if(!err){
    printfinfo("# checking search for unfixed documents");
    cond = est_cond_new();
    est_cond_set_phrase(cond, "check");
    res = est_db_search(db, cond, &rnum, NULL);
    if(rnum != 6){
      printferror("%s: the number of result is invalid", dbname);
      err = TRUE;
    }
    free(res);
    est_cond_delete(cond);
  }
  if(!err){
    printfinfo("# checking partial flushing of the index");
    if(!est_db_flush(db, 32)) err = TRUE;
  }
  if(!err){
    printfinfo("# checking deletion with cleaning of a document");
    if(!est_db_out_doc(db, 1, ESTODCLEAN)) err = TRUE;
  }
  if(!err){
    printfinfo("# checking synchronization");
    if(!est_db_sync(db)) err = TRUE;
  }
  if(!err){
    printfinfo("# checking deletion without cleaning of a document");
    if(!est_db_out_doc(db, 2, 0)) err = TRUE;
  }
  if(!err){
    printfinfo("# checking word search");
    cond = est_cond_new();
    est_cond_set_phrase(cond, "check it AND on");
    res = est_db_search(db, cond, &rnum, NULL);
    if(rnum != 5){
      printferror("%s: the number of result is invalid", dbname);
      err = TRUE;
    }
    free(res);
    est_cond_set_phrase(cond, "RUMBLE OR \xe3\x80\x82");
    res = est_db_search(db, cond, &rnum, NULL);
    if(rnum != 1){
      printferror("%s: the number of result is invalid", dbname);
      err = TRUE;
    }
    free(res);
    est_cond_delete(cond);
  }
  if(!err){
    printfinfo("# checking attribute search");
    cond = est_cond_new();
    est_cond_add_attr(cond, "@uri !ISTRINC SMaLl");
    res = est_db_search(db, cond, &rnum, NULL);
    if(rnum != est_db_doc_num(db) - 1){
      printferror("%s: the number of result is invalid", dbname);
      err = TRUE;
    }
    free(res);
    est_cond_delete(cond);
    cond = est_cond_new();
    est_cond_add_attr(cond, "@uri STRBW file://");
    est_cond_add_attr(cond, "@title STRINC \xe5\xb9\xb3");
    res = est_db_search(db, cond, &rnum, NULL);
    if(rnum != 1){
      printferror("%s: the number of result is invalid", dbname);
      err = TRUE;
    }
    free(res);
    est_cond_delete(cond);
  }
  if(!err){
    printfinfo("# checking combined search");
    cond = est_cond_new();
    est_cond_set_phrase(cond, "\xe5\x9b\xbd\xe5\xae\xb6\xe7\xac\xac\xe4\xb8\x80");
    est_cond_add_attr(cond, "@uri");
    est_cond_set_order(cond, "@title");
    res = est_db_search(db, cond, &rnum, NULL);
    if(rnum != 1){
      printferror("%s: the number of result is invalid", dbname);
      err = TRUE;
    }
    free(res);
    est_cond_delete(cond);
    cond = est_cond_new();
    est_cond_set_phrase(cond, "one | \xe3\x80\x82 | check & check it ! hogehoge");
    est_cond_add_attr(cond, "@uri STRBW file://");
    est_cond_set_order(cond, "@title STRD");
    est_cond_set_options(cond, ESTCONDSURE | ESTCONDNOIDF | ESTCONDSIMPLE);
    res = est_db_search(db, cond, &rnum, NULL);
    if(rnum != 4){
      printferror("%s: the number of result is invalid", dbname);
      err = TRUE;
    }
    free(res);
    est_cond_delete(cond);
  }
  if(!err){
    printfinfo("# checking optimization");
    if(!est_db_optimize(db, 0)) err = TRUE;
    cond = est_cond_new();
    est_cond_set_phrase(cond, "check");
    res = est_db_search(db, cond, &rnum, NULL);
    if(rnum != 4){
      printferror("%s: the number of result is invalid", dbname);
      err = TRUE;
    }
    free(res);
    est_cond_delete(cond);
  }
  if(!err){
    printfinfo("# checking traversal access");
    cond = est_cond_new();
    est_cond_set_phrase(cond, ESTOPUVSET);
    res = est_db_search(db, cond, &rnum, NULL);
    for(i = 0; i < rnum; i++){
      if(!(doc = est_db_get_doc(db, res[i], 0))){
        printferror("%s: a document cannot be retrieved", dbname);
        err = TRUE;
        break;
      }
      est_doc_delete(doc);
    }
    free(res);
    est_cond_delete(cond);
  }
  if(!err){
    printfinfo("# checking deletion with cleaning of a document");
    cond = est_cond_new();
    est_cond_add_attr(cond, "@uri STRINC");
    res = est_db_search(db, cond, &rnum, NULL);
    for(i = 0; i < rnum; i++){
      if(!est_db_out_doc(db, res[i], ESTODCLEAN)) err = TRUE;
    }
    free(res);
    est_cond_delete(cond);
  }
  if(!err){
    printfinfo("# checking attribute search with attribute indexes");
    if(!est_db_add_attr_index(db, "seq", ESTIDXATTRSEQ)) err = TRUE;
    if(!est_db_add_attr_index(db, "str", ESTIDXATTRSTR)) err = TRUE;
    if(!est_db_add_attr_index(db, "num", ESTIDXATTRNUM)) err = TRUE;
    for(i = 1; i <= 100; i++){
      doc = est_doc_new();
      sprintf(numbuf, "%d", i);
      est_doc_add_attr(doc, ESTDATTRURI, numbuf);
      sprintf(numbuf, "%d", i / 2);
      est_doc_add_attr(doc, "seq", numbuf);
      est_doc_add_attr(doc, "str", numbuf);
      est_doc_add_attr(doc, "num", numbuf);
      if(!est_db_put_doc(db, doc, 0)) err = TRUE;
      est_doc_delete(doc);
    }
    cond = est_cond_new();
    est_cond_add_attr(cond, "seq STREQ 30");
    est_cond_add_attr(cond, "str STREQ 30");
    est_cond_add_attr(cond, "num NUMEQ 30");
    res = est_db_search(db, cond, &rnum, NULL);
    if(rnum != 2){
      printferror("%s: the number of result is invalid", dbname);
      err = TRUE;
    }
    free(res);
    est_cond_delete(cond);
    cond = est_cond_new();
    est_cond_add_attr(cond, "num NUMGE 10");
    est_cond_add_attr(cond, "num NUMLT 20");
    res = est_db_search(db, cond, &rnum, NULL);
    if(rnum != 20){
      printferror("%s: the number of result is invalid", dbname);
      err = TRUE;
    }
    free(res);
    est_cond_delete(cond);
  }
  if(err) printferror("%s: %s", dbname, est_err_msg(est_db_error(db)));
  if(!err){
    printfinfo("# checking object duplication");
    doc = est_doc_new();
    ndoc = est_doc_dup(doc);
    est_doc_delete(doc);
    est_doc_delete(ndoc);
    doc = est_doc_new();
    est_doc_set_id(doc, 100);
    est_doc_add_attr(doc, "@uri", "original.mp3");
    est_doc_add_attr(doc, "@title", "TAKE IT TO THE TOP");
    est_doc_add_text(doc, "fly over the mountains");
    est_doc_add_text(doc, "jump into the sea");
    est_doc_add_text(doc, "move all the planets baby give them all to me");
    est_doc_add_hidden_text(doc, "Don't stop!");
    ndoc = est_doc_dup(doc);
    est_doc_add_attr(doc, "@uri", "duplication.mp3");
    if(!est_db_put_doc(db, doc, ESTPDCLEAN) || !est_db_put_doc(db, ndoc, ESTPDCLEAN)){
      printferror("%s: document object duplication failed", dbname);
      err = TRUE;
    }
    est_doc_delete(ndoc);
    est_doc_delete(doc);
    cond = est_cond_new();
    ncond = est_cond_dup(cond);
    est_cond_delete(cond);
    est_cond_delete(ncond);
    cond = est_cond_new();
    est_cond_set_phrase(cond, "fly sea");
    est_cond_add_attr(cond, "@uri ISTREW .mp3");
    est_cond_add_attr(cond, "@title ISTRINC take");
    est_cond_set_order(cond, "@uri STRD");
    est_cond_set_options(cond, ESTCONDAGITO);
    est_cond_set_options(cond, ESTCONDSIMPLE);
    est_cond_set_auxiliary(cond, 64);
    est_cond_set_eclipse(cond, 0.5);
    res = est_db_search(db, cond, &rnum, NULL);
    ncond = est_cond_dup(cond);
    for(i = 0; i < rnum; i++){
      ary = est_cond_shadows(ncond, res[i], &anum);
      if(anum < 1){
        printferror("%s: condition object duplication failed", dbname);
        err = TRUE;
      }
    }
    free(res);
    est_cond_delete(cond);
    est_cond_delete(ncond);
  }
  printfinfo("# checking meta search");
  dbs = cbmalloc(5 * sizeof(ESTDB *));
  for(i = 0; i < 5; i++){
    dbs[i] = db;
  }
  cond = est_cond_new();
  est_cond_set_phrase(cond, "all OR jump OR planets OR hoge");
  est_cond_set_order(cond, "[IDD]");
  est_cond_set_options(cond, ESTCONDSCFB);
  hints = cbmapopen();
  free(est_db_search(db, cond, &onum, NULL));
  res = est_db_search_meta(dbs, 5, cond, &rnum, hints);
  if(rnum != onum * 10){
    printferror("%s: the number of result is invalid", dbname);
    err = TRUE;
  }
  free(res);
  cbmapclose(hints);
  est_cond_delete(cond);
  cond = est_cond_new();
  est_cond_set_phrase(cond, "[UVSET]");
  est_cond_add_attr(cond, "@uri ISTRRX [a-z2-3]");
  est_cond_set_order(cond, "@title STRD");
  est_cond_set_options(cond, ESTCONDSCFB);
  hints = cbmapopen();
  free(est_db_search(db, cond, &onum, NULL));
  res = est_db_search_meta(dbs, 5, cond, &rnum, hints);
  if(rnum != onum * 10){
    printferror("%s: the number of result is invalid", dbname);
    err = TRUE;
  }
  free(res);
  cbmapclose(hints);
  est_cond_delete(cond);
  free(dbs);
  printfinfo("# closing the database");
  if(!est_db_close(db, &ecode)){
    printferror("%s: %s", dbname, est_err_msg(ecode));
    return 1;
  }
  printfinfo("# checking result map");
  rmap = cbmapopen();
  est_resmap_add(rmap, "one", 10, ESTRMLOSUM);
  est_resmap_add(rmap, "two", 10, ESTRMLOSUM);
  est_resmap_add(rmap, "two", 10, ESTRMLOSUM);
  est_resmap_add(rmap, "three", 10, ESTRMLOSUM);
  est_resmap_add(rmap, "three", 10, ESTRMLOSUM);
  est_resmap_add(rmap, "three", 10, ESTRMLOSUM);
  elems = est_resmap_dump(rmap, 2, &rnum);
  for(i = 0; i < rnum; i++){
    free(cbmemdup(elems[i].key, -1));
  }
  free(elems);
  cbmapclose(rmap);
  curtime = time(NULL) - curtime;
  if(!err) printfinfo("# finished successfully: elapsed time: %dh %dm %ds",
                      (int)(curtime / 3600), (int)((curtime / 60) % 60), (int)(curtime % 60));
  return err ? 1 : 0;
}


/* output escaped string */
static void xmlprintf(const char *format, ...){
  va_list ap;
  char *tmp, cbuf[32];
  unsigned char c;
  int cblen;
  va_start(ap, format);
  while(*format != '\0'){
    if(*format == '%'){
      cbuf[0] = '%';
      cblen = 1;
      format++;
      while(strchr("0123456789 .+-", *format) && *format != '\0' && cblen < sizeof(cbuf) - 1){
        cbuf[cblen++] = *format;
        format++;
      }
      cbuf[cblen++] = *format;
      cbuf[cblen] = '\0';
      switch(*format){
      case 's':
        tmp = va_arg(ap, char *);
        if(!tmp) tmp = "(null)";
        printf(cbuf, tmp);
        break;
      case 'd':
        printf(cbuf, va_arg(ap, int));
        break;
      case 'o': case 'u': case 'x': case 'X': case 'c':
        printf(cbuf, va_arg(ap, unsigned int));
        break;
      case 'e': case 'E': case 'f': case 'g': case 'G':
        printf(cbuf, va_arg(ap, double));
        break;
      case '@':
        tmp = va_arg(ap, char *);
        if(!tmp) tmp = "(null)";
        while(*tmp){
          switch(*tmp){
          case '&': printf("&amp;"); break;
          case '<': printf("&lt;"); break;
          case '>': printf("&gt;"); break;
          case '"': printf("&quot;"); break;
          default:
            if(!((*tmp >= 0 && *tmp <= 0x8) || (*tmp >= 0x0e && *tmp <= 0x1f))) putchar(*tmp);
            break;
          }
          tmp++;
        }
        break;
      case '?':
        tmp = va_arg(ap, char *);
        if(!tmp) tmp = "(null)";
        while(*tmp){
          c = *(unsigned char *)tmp;
          if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
             (c >= '0' && c <= '9') || (c != '\0' && strchr("_-.", c))){
            putchar(c);
          } else {
            printf("%%%02X", c);
          }
          tmp++;
        }
        break;
      case '%':
        putchar('%');
        break;
      }
    } else {
      putchar(*format);
    }
    format++;
  }
  va_end(ap);
}


/* get the index data type value */
static int strtoidxtype(const char *str){
  if(!cbstricmp(str, "str")) return ESTIDXATTRSTR;
  if(!cbstricmp(str, "num")) return ESTIDXATTRNUM;
  return ESTIDXATTRSEQ;
}


/* get the language value */
static int strtolang(const char *str){
  if(!cbstricmp(str, "en")) return ESTLANGEN;
  if(!cbstricmp(str, "ja")) return ESTLANGJA;
  if(!cbstricmp(str, "zh")) return ESTLANGZH;
  if(!cbstricmp(str, "ko")) return ESTLANGKO;
  return ESTLANGMISC;
}


/* read a line */
static char *fgetl(FILE *ifp){
  char *buf;
  int c, len, blen;
  buf = NULL;
  len = 0;
  blen = 1024;
  while((c = fgetc(ifp)) != EOF){
    if(blen <= len) blen *= 2;
    buf = cbrealloc(buf, blen + 1);
    if(c == '\n') c = '\0';
    if(c != '\r') buf[len++] = c;
    if(c == '\0') break;
  }
  if(!buf) return NULL;
  buf[len] = '\0';
  return buf;
}


/* register a document */
static int doputdoc(ESTDB *db, const char *path, const CBLIST *attrs){
  static int count = 0;
  ESTDOC *doc, *edoc;
  CBMAP *kwords;
  const char *uri, *vbuf, *xcmd, *lreal, *lfile;
  char *dbuf, *tbuf, numbuf[NUMBUFSIZ];
  int i, err, fmt, id, dsiz, score;
  time_t emdate, fmdate;
  struct stat sbuf;
  xcmd = NULL;
  if(cbmaprnum(g_xcmdmap) > 0){
    cbmapiterinit(g_xcmdmap);
    while((vbuf = cbmapiternext(g_xcmdmap, NULL)) != NULL){
      if(!strcmp(vbuf, "*") || cbstrbwimatch(path, vbuf)){
        xcmd = cbmapget(g_xcmdmap, vbuf, -1, NULL);
        break;
      }
    }
  }
  fmt = g_filefmt;
  if(g_filefmt == FF_NONE && !xcmd) return TRUE;
  if(g_filefmt == FF_AUTO){
    if(cbstrbwimatch(path, ESTEXTSTR "est")){
      fmt = FF_DRAFT;
    } else if(cbstrbwimatch(path, ESTEXTSTR "txt") || cbstrbwimatch(path, ESTEXTSTR "text") ||
              cbstrbwimatch(path, ESTEXTSTR "asc")){
      fmt = FF_TEXT;
    } else if(cbstrbwimatch(path, ESTEXTSTR "html") || cbstrbwimatch(path, ESTEXTSTR "htm") ||
              cbstrbwimatch(path, ESTEXTSTR "xhtml") || cbstrbwimatch(path, ESTEXTSTR "xht")){
      fmt = FF_HTML;
    } else if(cbstrbwimatch(path, ESTEXTSTR "eml") || cbstrbwimatch(path, ESTEXTSTR "mime") ||
              cbstrbwimatch(path, ESTEXTSTR "mht") || cbstrbwimatch(path, ESTEXTSTR "mhtml")){
      fmt = FF_MIME;
    } else if(!xcmd){
      return TRUE;
    }
  }
  if(stat(path, &sbuf) == -1 || !S_ISREG(sbuf.st_mode) || !(uri = pathtourl(path))){
    printferror("%s: could not open", path);
    return TRUE;
  }
  if(g_limitfsiz >= 0 && sbuf.st_size > g_limitfsiz){
    printferror("%s: exceeding the file size limitation", path);
    return TRUE;
  }
  emdate = -1;
  if(g_chkmdate && (id = est_db_uri_to_id(db, uri)) > 0 &&
     (tbuf = est_db_get_doc_attr(db, id, ESTDATTRMDATE)) != NULL){
    emdate = cbstrmktime(tbuf);
    free(tbuf);
  }
  if(g_stdate && emdate >= 0 && emdate >= sbuf.st_mtime){
    printfinfo("%s: passed (old document)", path);
    return TRUE;
  }
  if(g_filtorig){
    dbuf = cbmemdup("", 0);
    dsiz = 0;
  } else {
    if(!(dbuf = cbreadfile(path, &dsiz))){
      printferror("%s: could not open", path);
      return TRUE;
    }
  }
  if(xcmd){
    doc = est_doc_new_with_xcmd(dbuf, dsiz, path, xcmd, est_db_name(db),
                                g_inputcode, g_inputlang);
    if(doc && g_filtorig){
      sprintf(numbuf, "%d", (int)sbuf.st_size);
      est_doc_add_attr(doc, ESTDATTRSIZE, numbuf);
    }
  } else {
    switch(fmt){
    case FF_TEXT:
      doc = est_doc_new_from_text(dbuf, dsiz, g_inputcode, g_inputlang, g_bincheck);
      break;
    case FF_HTML:
      doc = est_doc_new_from_html(dbuf, dsiz, g_inputcode, g_inputlang, g_bincheck);
      break;
    case FF_MIME:
      doc = est_doc_new_from_mime(dbuf, dsiz, g_inputcode, g_inputlang, g_bincheck);
      break;
    default:
      doc = est_doc_new_from_draft_enc(dbuf, dsiz, g_inputcode);
      break;
    }
  }
  if(!doc || (g_bincheck && est_doc_is_empty(doc))){
    printfinfo("%s: passed (empty document)", path);
    if(doc) est_doc_delete(doc);
    free(dbuf);
    return TRUE;
  }
  if(g_limittsiz >= 0) est_doc_slim(doc, g_limittsiz);
  if(attrs){
    for(i = 0; i < cblistnum(g_pathattrs) && i < cblistnum(attrs); i++){
      est_doc_add_attr(doc, cblistval(g_pathattrs, i, NULL), cblistval(attrs, i, NULL));
    }
  }
  if(!est_doc_attr(doc, ESTDATTRURI)) est_doc_add_attr(doc, ESTDATTRURI, uri);
  if(!est_doc_attr(doc, DATTRLPATH)) est_doc_add_attr(doc, DATTRLPATH, uri);
  if((lreal = pathtolreal(path)) != NULL){
    if(!est_doc_attr(doc, DATTRLREAL)) est_doc_add_attr(doc, DATTRLREAL, lreal);
    lfile = strrchr(lreal, ESTPATHCHR);
    lfile = lfile ? lfile + 1 : lreal;
    if(!est_doc_attr(doc, DATTRLFILE)) est_doc_add_attr(doc, DATTRLFILE, lfile);
  }
  uri = est_doc_attr(doc, ESTDATTRURI);
  if(g_stdate){
    tbuf = cbdatestrwww(sbuf.st_mtime, 0);
    est_doc_add_attr(doc, ESTDATTRMDATE, tbuf);
    free(tbuf);
  }
  if(cblistnum(g_addattrs) > 0){
    for(i = 0; i < cblistnum(g_addattrs); i += 2){
      est_doc_add_attr(doc, cblistval(g_addattrs, i, NULL), cblistval(g_addattrs, i + 1, NULL));
    }
  }
  if(g_chkmdate && emdate == -1 && (id = est_db_uri_to_id(db, uri)) > 0 &&
     (edoc = est_db_get_doc(db, id, ESTGDNOTEXT | ESTGDNOKWD)) != NULL){
    if((vbuf = est_doc_attr(edoc, ESTDATTRMDATE)) != NULL) emdate = cbstrmktime(vbuf);
    est_doc_delete(edoc);
  }
  fmdate = -1;
  if(g_chkmdate && (vbuf = est_doc_attr(doc, ESTDATTRMDATE)) != NULL) fmdate = cbstrmktime(vbuf);
  err = FALSE;
  if(emdate >= 0 && emdate >= fmdate){
    printfinfo("%s: passed (old document)", path);
  } else if(g_nooper){
    printfinfo("%d (%s): registered", -1, path);
  } else {
    if(g_ssname){
      score = (vbuf = est_doc_attr(doc, g_ssname)) != NULL ? cbstrmktime(vbuf) : 0;
      if(score < 0) score = 0;
      est_doc_set_score(doc, score);
    }
    if(g_kwordnum > 0){
      kwords = g_usemorph ? est_morph_etch_doc(doc, g_kwordnum) :
        est_db_etch_doc(NULL, doc, g_kwordnum);
      est_doc_set_keywords(doc, kwords);
      cbmapclose(kwords);
    }
    if(est_db_put_doc(db, doc, g_putopts)){
      printfinfo("%d (%s): registered", est_doc_id(doc), path);
    } else {
      printferror("%s: %s", est_db_name(db), est_err_msg(est_db_error(db)));
      err = TRUE;
    }
  }
  est_doc_delete(doc);
  free(dbuf);
  if(count++ % MEMCHKFREQ == 0){
    if(est_memory_usage() > MEMLOADMAX && !est_db_flush(db, -1)){
      printferror("%s: %s", est_db_name(db), est_err_msg(est_db_error(db)));
      err = TRUE;
    }
    if(!g_nochkvmem && !cbvmemavail(MEMCHKALW)){
      printferror("exhausting memory");
      err = TRUE;
    }
  }
  return err ? FALSE : TRUE;
}


/* remove a file */
static void doremovefile(const char *path){
  const char *elem;
  int i;
  if(g_nooper) return;
  for(i = 0; i < cblistnum(g_rmvsufs); i++){
    elem = cblistval(g_rmvsufs, i, NULL);
    if(elem[0] == '\0') continue;
    if(!strcmp(elem, "*") || cbstrbwimatch(path, elem)){
      if(unlink(path) == -1) printferror("%s: could not remove", path);
      break;
    }
  }
}


/* get the URI of an expression of a path or a URI */
static const char *exprtouri(const char *expr){
  const char *rp;
  rp = expr;
  if(cbstrfwimatch(expr, "urn:")) rp += 4;
  while((*rp >= 'A' && *rp <= 'Z') || (*rp >= 'a' && *rp <= 'z')){
    rp++;
  }
  if(rp > expr && rp[0] == ':' && rp[1] == '/' && rp[2] == '/') return expr;
  if(cbstrfwimatch(expr, "mailto:") && !strchr(expr, '/')) return expr;
  return pathtourl(expr);
}


/* get the URL of a path */
static const char *pathtourl(const char *path){
  static char pbuf[URIBUFSIZ];
  const char *elem;
  char *host, *rpath, *ebuf, *wp;
  CBLIST *list;
  int i, esiz;
  if(strlen(path) >= URIBUFSIZ / 4) return NULL;
  host = NULL;
  if(g_pathcode){
    rpath = est_realpath(path);
    if(!(ebuf = est_iconv(rpath, -1, g_pathcode, "UTF-8", &esiz, NULL))){
      esiz = strlen(rpath);
      ebuf = cbmemdup(rpath, esiz);
    }
    if(ESTPATHCHR == '\\' && cbstrfwmatch(ebuf, "\\\\") &&
       (wp = strchr(ebuf + 2, '\\')) != NULL){
      *wp = '\0';
      host = cbmemdup(ebuf + 2, -1);
      list = cbsplit(wp + 1, -1, ESTPATHSTR);
    } else {
      list = cbsplit(ebuf, esiz, ESTPATHSTR);
    }
    free(ebuf);
    free(rpath);
    for(i = 0; i < cblistnum(list); i++){
      elem = cblistval(list, i, &esiz);
      if((ebuf = est_iconv(elem, esiz, "UTF-8", g_pathcode, &esiz, NULL)) != NULL){
        cblistover(list, i, ebuf, esiz);
        free(ebuf);
      }
    }
  } else {
    rpath = est_realpath(path);
    if(ESTPATHCHR == '\\' && cbstrfwmatch(rpath, "\\\\") &&
       (wp = strchr(rpath + 2, '\\')) != NULL){
      *wp = '\0';
      host = cbmemdup(rpath + 2, -1);
      list = cbsplit(wp + 1, -1, ESTPATHSTR);
    } else {
      list = cbsplit(rpath, -1, ESTPATHSTR);
    }
    free(rpath);
  }
  wp = pbuf;
  wp += sprintf(wp, "file://");
  if(host) wp += sprintf(wp, "%s", host);
  for(i = 0; i < cblistnum(list); i++){
    elem = cblistval(list, i, NULL);
    if(elem[0] == '\0') continue;
    if(ESTPATHCHR == '\\' && i < 1 &&
       ((elem[0] >= 'A' && elem[0] <= 'Z') || (elem[0] >= 'a' && elem[0] <= 'z')) &&
       elem[1] == ':'){
      wp += sprintf(wp, "/%c|", elem[0]);
      continue;
    }
    ebuf = cburlencode(elem, -1);
    wp += sprintf(wp, "/%s", ebuf);
    free(ebuf);
  }
  if(wp == pbuf + 7) *(wp++) = '/';
  *wp = '\0';
  cblistclose(list);
  free(host);
  return pbuf;
}


/* get the real path of a path */
static const char *pathtolreal(const char *path){
  static char pbuf[URIBUFSIZ];
  char *rbuf, *ebuf;
  rbuf = est_realpath(path);
  if((ebuf = est_iconv(rbuf, -1, g_pathcode ? g_pathcode : "ISO-8859-1", "UTF-8",
                       NULL, NULL)) != NULL){
    free(rbuf);
    rbuf = ebuf;
  }
  sprintf(pbuf, "%s", rbuf);
  free(rbuf);
  return pbuf;
}


/* get the local path of a URL */
static const char *urltopath(const char *url){
  static char pbuf[URIBUFSIZ];
  const char *pv, *elem;
  char *wp, *dbuf;
  CBLIST *list;
  int i;
  if(!cbstrfwimatch(url, "file://")) return NULL;
  pv = url + 7;
  if(!(url = strchr(pv, '/'))) return NULL;
  wp = pbuf;
  if(ESTPATHCHR == '\\'){
    if(url[0] == '/' && ((url[1] >= 'A' && url[1] <= 'Z') || (url[1] >= 'a' && url[1] <= 'z')) &&
       url[2] == '|' && url[3] == '/'){
      wp += sprintf(wp, "%c:", url[1]);
      url += 3;
    } else if(url > pv){
      wp += sprintf(wp, "\\\\");
      memcpy(wp, pv, url - pv);
      wp += url - pv;
    }
  }
  list = cbsplit(url, -1, "/");
  for(i = 0; i < cblistnum(list); i++){
    elem = cblistval(list, i, NULL);
    if(elem[0] == '\0') continue;
    dbuf = cburldecode(elem, NULL);
    wp += sprintf(wp, "%c%s", ESTPATHCHR, dbuf);
    free(dbuf);
  }
  *wp = '\0';
  cblistclose(list);
  return pbuf;
}


/* check whether a file name is bad on dosish file system */
static int dosbadname(const char *name){
  static const char *badnames[] = {
    "AUX", "CON", "NUL", "PRN", "CLOCK", "CLOCK$", "CONFIG$",
    "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
    "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9",
    NULL
  };
  int i, len;
  if(name[0] == '\\' || strstr(name, ".\\")) return TRUE;
  for(i = 0; badnames[i]; i++){
    if(cbstrfwimatch(name, badnames[i])){
      len = strlen(badnames[i]);
      if(name[len] == '\0' || name[len] == '.') return TRUE;
    }
  }
  return FALSE;
}


/* check whether a buffer is binary */
static int est_check_binary(const char *buf, int size){
  int i, bin;
  assert(buf && size >= 0);
  if(size < 32) return FALSE;
  /* PDF */
  if(!memcmp(buf, "%PDF-", 5)) return TRUE;
  /* PostScript */
  if(!memcmp(buf, "%!PS-Adobe", 10)) return TRUE;
  /* generic binary */
  size -= 5;
  if(size >= 256) size = 256;
  bin = FALSE;
  for(i = 0; i < size; i++){
    if(buf[i] == 0x0){
      if(buf[i+1] == 0x0 && buf[i+2] == 0x0 && buf[i+3] == 0x0 && buf[i+4] == 0x0) return TRUE;
      bin = TRUE;
    }
  }
  if(!bin) return FALSE;
  /* PNG */
  if(!memcmp(buf, "\x89PNG", 4)) return TRUE;
  /* GIF(87a) */
  if(!memcmp(buf, "GIF87a", 6)) return TRUE;
  /* GIF(89a) */
  if(!memcmp(buf, "GIF89a", 6)) return TRUE;
  /* JFIF */
  if(!memcmp(buf, "\xff\xd8JFIF", 6)) return TRUE;
  /* TIFF(Intel) */
  if(!memcmp(buf, "MM\x00\x2a", 4)) return TRUE;
  /* TIFF(Motorola) */
  if(!memcmp(buf, "II\x2a\x00", 4)) return TRUE;
  /* BMP */
  if(!memcmp(buf, "BM", 2)) return TRUE;
  /* GZIP */
  if(!memcmp(buf, "\x1f\x8b\x08", 3)) return TRUE;
  /* BZIP2 */
  if(!memcmp(buf, "BZh", 3)) return TRUE;
  /* ZIP */
  if(!memcmp(buf, "PK\x03\x04", 4)) return TRUE;
  /* MP3(with ID3) */
  if(!memcmp(buf, "ID3", 3)) return TRUE;
  /* MP3 */
  if(((buf[0] * 0x100 + buf[1]) & 0xfffe) == 0xfffa) return TRUE;
  /* MIDI */
  if(!memcmp(buf, "MThd", 4)) return TRUE;
  /* RPM package*/
  if(!memcmp(buf, "0xed0xab", 2)) return TRUE;
  /* Debian package */
  if(!memcmp(buf, "!<arch>\ndebian", 14)) return TRUE;
  /* ELF */
  if(!memcmp(buf, "\x7f\x45\x4c\x46", 4)) return TRUE;
  /* MS-DOS executable */
  if(!memcmp(buf, "MZ", 2)) return TRUE;
  /* MS-Office */
  if(!memcmp(buf, "\xd0\xcf\x11\xe0\xa1\xb1\x1a\xe1", 8)) return TRUE;
  if(!memcmp(buf, "\xfe\x37\x00\x23", 4)) return TRUE;
  if(!memcmp(buf, "\xdb\xa5-\x00\x00\x00", 6)) return TRUE;
  return FALSE;
}


/* create a document object with an outer command */
static ESTDOC *est_doc_new_with_xcmd(const char *buf, int size, const char *path,
                                     const char *xcmd, const char *tmpdir,
                                     const char *penc, int plang){
  ESTDOC *doc;
  const char *pv, *ext;
  char iname[URIBUFSIZ], oname[URIBUFSIZ], ebuf[URIBUFSIZ], cmd[URIBUFSIZ];
  char *rbuf, numbuf[NUMBUFSIZ];
  int fmt, rsiz;
  assert(buf && size >= 0 && path && xcmd && tmpdir);
  sprintf(ebuf, "ESTORIGFILE=%s", path);
  ext = NULL;
  if((pv = strrchr(path, ESTPATHCHR)) != NULL) path = pv;
  if((pv = strrchr(path, ESTEXTCHR)) != NULL) ext = pv;
  if(!ext || strlen(ext) >= 32 || strchr(ext, '"') || strchr(ext, '\\')) ext = "";
  sprintf(iname, "%s%cxcmd-in-%08d%s", tmpdir, ESTPATHCHR, (int)getpid(), ext);
  sprintf(oname, "%s%cxcmd-out-%08d%cest", tmpdir, ESTPATHCHR, (int)getpid(), ESTEXTCHR);
  fmt = FF_DRAFT;
  if(cbstrfwmatch(xcmd, "T@")){
    fmt = FF_TEXT;
    xcmd += 2;
  } else if(cbstrfwmatch(xcmd, "H@")){
    fmt = FF_HTML;
    xcmd += 2;
  } else if(cbstrfwmatch(xcmd, "M@")){
    fmt = FF_MIME;
    xcmd += 2;
  }
  if(!g_filtorig) cbwritefile(iname, buf, size);
  sprintf(cmd, "%s \"%s\" \"%s\"", xcmd, iname, oname);
  putenv(ebuf);
  system(cmd);
  if((rbuf = cbreadfile(oname, &rsiz)) != NULL){
    switch(fmt){
    case FF_TEXT:
      doc = est_doc_new_from_text(rbuf, rsiz, penc, plang, FALSE);
      break;
    case FF_HTML:
      doc = est_doc_new_from_html(rbuf, rsiz, penc, plang, FALSE);
      break;
    case FF_MIME:
      doc = est_doc_new_from_mime(rbuf, rsiz, penc, plang, FALSE);
      break;
    default:
      doc = est_doc_new_from_draft_enc(rbuf, rsiz, penc);
      break;
    }
    free(rbuf);
  } else {
    doc = est_doc_new();
  }
  if(doc && fmt != FF_DRAFT){
    sprintf(numbuf, "%d", size);
    est_doc_add_attr(doc, ESTDATTRSIZE, numbuf);
    est_doc_add_attr(doc, ESTDATTRTYPE, est_ext_type(ext));
  }
  unlink(oname);
  unlink(iname);
  return doc;
}


/* create a document object from draft data in another encoding */
static ESTDOC *est_doc_new_from_draft_enc(const char *buf, int size, const char *enc){
  ESTDOC *doc;
  char *rbuf;
  assert(buf);
  if(enc && (rbuf = est_iconv(buf, size, enc, "UTF-8", NULL, NULL)) != NULL){
    doc = est_doc_new_from_draft(rbuf);
    free(rbuf);
  } else {
    doc = est_doc_new_from_draft(buf);
  }
  return doc;
}


/* create a document object from plain text */
static ESTDOC *est_doc_new_from_text(const char *buf, int size,
                                     const char *penc, int plang, int bcheck){
  ESTDOC *doc;
  CBLIST *lines;
  CBDATUM *datum;
  const char *enc, *text, *line;
  char *nbuf, numbuf[NUMBUFSIZ];
  int i;
  assert(buf && size >= 0);
  if(bcheck && est_check_binary(buf, size)) return NULL;
  doc = est_doc_new();
  enc = penc ? penc : est_enc_name(buf, size, plang);
  if(!strcmp(enc, "UTF-8")){
    nbuf = NULL;
    text = buf;
  } else {
    text = buf;
    nbuf = est_iconv(buf, size, enc, "UTF-8", NULL, NULL);
    if(nbuf) text = nbuf;
  }
  lines = cbsplit(text, -1, "\n");
  CB_DATUMOPEN(datum);
  for(i = 0; i < CB_LISTNUM(lines); i++){
    line = CB_LISTVAL(lines, i);
    while(*line == ' ' || *line == '\t' || *line == '\r'){
      line++;
    }
    if(line[0] == '\0'){
      est_doc_add_text(doc, CB_DATUMPTR(datum));
      CB_DATUMSETSIZE(datum, 0);
    } else {
      CB_DATUMCAT(datum, " ", 1);
      CB_DATUMCAT(datum, line, strlen(line));
    }
  }
  est_doc_add_text(doc, CB_DATUMPTR(datum));
  CB_DATUMCLOSE(datum);
  CB_LISTCLOSE(lines);
  est_doc_add_attr(doc, ESTDATTRTYPE, "text/plain");
  sprintf(numbuf, "%d", size);
  est_doc_add_attr(doc, ESTDATTRSIZE, numbuf);
  if(nbuf) free(nbuf);
  return doc;
}


/* create a document object from HTML */
static ESTDOC *est_doc_new_from_html(const char *buf, int size,
                                     const char *penc, int plang, int bcheck){
  ESTDOC *doc;
  CBLIST *elems;
  CBMAP *attrs;
  CBDATUM *datum;
  const char *enc, *html, *elem, *next, *value, *name, *content;
  char *nbuf, *nenc, *rbuf, *lbuf, numbuf[NUMBUFSIZ];
  int i, esiz;
  assert(buf && size >= 0);
  if(bcheck && est_check_binary(buf, size)) return NULL;
  doc = est_doc_new();
  enc = est_enc_name(buf, size, plang);
  html = NULL;
  nbuf = NULL;
  if(!strcmp(enc, "UTF-16") || !strcmp(enc, "UTF-16BE") || !strcmp(enc, "UTF-16LE")){
    nbuf = est_iconv(buf, size, enc, "UTF-8", NULL, NULL);
  } else if(!strcmp(enc, "US-ASCII")){
    nbuf = NULL;
  } else {
    if((nenc = penc ? cbmemdup(penc, -1) : est_html_enc(buf)) != NULL){
      if(cbstricmp(nenc, "UTF-8")){
        nbuf = est_iconv(buf, size, nenc, "UTF-8", NULL, NULL);
        if(!nbuf) nbuf = est_iconv(buf, size, enc, "UTF-8", NULL, NULL);
      }
      free(nenc);
    } else {
      nbuf = est_iconv(buf, size, enc, "UTF-8", NULL, NULL);
    }
  }
  if(nbuf) html = nbuf;
  if(!html) html = buf;
  CB_DATUMOPEN(datum);
  elems = cbxmlbreak(html, TRUE);
  for(i = 0; i < CB_LISTNUM(elems); i++){
    elem = CB_LISTVAL2(elems, i, esiz);
    if(!(next = cblistval(elems, i + 1, NULL))) next = "";
    if(elem[0] == '<'){
      if(cbstrfwimatch(elem, "<html")){
        attrs = cbxmlattrs(elem);
        value = cbmapget(attrs, "lang", -1, NULL);
        if(!value) value = cbmapget(attrs, "Lang", -1, NULL);
        if(!value) value = cbmapget(attrs, "LANG", -1, NULL);
        if(!value) value = cbmapget(attrs, "xml:lang", -1, NULL);
        if(value && value[0] != '\0') est_doc_add_attr(doc, ESTDATTRLANG, value);
        cbmapclose(attrs);
      } else if(cbstrfwimatch(elem, "<meta")){
        attrs = cbxmlattrs(elem);
        name = cbmapget(attrs, "name", -1, NULL);
        if(!name) name = cbmapget(attrs, "Name", -1, NULL);
        if(!name) name = cbmapget(attrs, "NAME", -1, NULL);
        if(!name) name = cbmapget(attrs, "http-equiv", -1, NULL);
        if(!name) name = cbmapget(attrs, "Http-equiv", -1, NULL);
        if(!name) name = cbmapget(attrs, "Http-Equiv", -1, NULL);
        if(!name) name = cbmapget(attrs, "HTTP-EQUIV", -1, NULL);
        content = cbmapget(attrs, "content", -1, NULL);
        if(!content) content = cbmapget(attrs, "Content", -1, NULL);
        if(!content) content = cbmapget(attrs, "CONTENT", -1, NULL);
        if(name && content){
          lbuf = cbmemdup(name, -1);
          cbstrtolower(lbuf);
          cbstrsqzspc(lbuf);
          if(!strcmp(lbuf, "author")){
            if(strchr(content, '&')){
              rbuf = est_html_raw_text(content);
              est_doc_add_attr(doc, ESTDATTRAUTHOR, rbuf);
              free(rbuf);
            } else {
              est_doc_add_attr(doc, ESTDATTRAUTHOR, content);
            }
          }
          if(name[0] != '@' && name[0] != '_'){
            if(strchr(content, '&')){
              rbuf = est_html_raw_text(content);
              est_doc_add_attr(doc, lbuf, rbuf);
              free(rbuf);
            } else {
              est_doc_add_attr(doc, lbuf, content);
            }
          }
          free(lbuf);
        }
        cbmapclose(attrs);
      } else if(cbstrfwimatch(elem, "<title") && next[0] != '\0' && next[0] != '<'){
        if(strchr(next, '&')){
          rbuf = est_html_raw_text(next);
          est_doc_add_attr(doc, ESTDATTRTITLE, rbuf);
          est_doc_add_hidden_text(doc, rbuf);
          free(rbuf);
        } else {
          est_doc_add_attr(doc, ESTDATTRTITLE, next);
          est_doc_add_hidden_text(doc, next);
        }
        i++;
      } else if(cbstrfwimatch(elem, "<style") || cbstrfwimatch(elem, "<script")){
        while((next = cblistval(elems, i + 1, NULL)) != NULL &&
              !(next[0] == '<' && next[1] != '!' && next[1] != ' ' && next[1] != '=')){
          i++;
        }
      } else if(cbstrfwimatch(elem, "<h1") || cbstrfwimatch(elem, "<h2") ||
                cbstrfwimatch(elem, "<h3") || cbstrfwimatch(elem, "<h4") ||
                cbstrfwimatch(elem, "<h5") || cbstrfwimatch(elem, "<h6") ||
                cbstrfwimatch(elem, "<p>") || cbstrfwimatch(elem, "<p ") ||
                cbstrfwimatch(elem, "<div") || cbstrfwimatch(elem, "<hr") ||
                cbstrfwimatch(elem, "<ul") || cbstrfwimatch(elem, "<ol") ||
                cbstrfwimatch(elem, "<dl") || cbstrfwimatch(elem, "<li") ||
                cbstrfwimatch(elem, "<dt") || cbstrfwimatch(elem, "<dd") ||
                cbstrfwimatch(elem, "<th") || cbstrfwimatch(elem, "<td") ||
                cbstrfwimatch(elem, "<pre")){
        if(strchr(CB_DATUMPTR(datum), '&')){
          rbuf = est_html_raw_text(CB_DATUMPTR(datum));
          est_doc_add_text(doc, rbuf);
          free(rbuf);
        } else {
          est_doc_add_text(doc, CB_DATUMPTR(datum));
        }
        CB_DATUMSETSIZE(datum, 0);
      }
    } else if (cbstrfwimatch(elem, "<span")) {
      /* CB_DATUMCAT(datum, " ", 1); */ /* SJT suppress spaces generated for span */
      /* CB_DATUMCAT(datum, elem, esiz); */
    } else {
      CB_DATUMCAT(datum, " ", 1);
      CB_DATUMCAT(datum, elem, esiz);
    }
  }
  CB_LISTCLOSE(elems);
  if(strchr(CB_DATUMPTR(datum), '&')){
    rbuf = est_html_raw_text(CB_DATUMPTR(datum));
    est_doc_add_text(doc, rbuf);
    free(rbuf);
  } else {
    est_doc_add_text(doc, CB_DATUMPTR(datum));
  }
  CB_DATUMCLOSE(datum);
  if(nbuf) free(nbuf);
  est_doc_add_attr(doc, ESTDATTRTYPE, "text/html");
  sprintf(numbuf, "%d", size);
  est_doc_add_attr(doc, ESTDATTRSIZE, numbuf);
  return doc;
}


/* get the encoding of an HTML string */
static char *est_html_enc(const char *str){
  CBLIST *elems;
  CBMAP *attrs;
  const char *elem, *equiv, *content;
  char *enc, *pv;
  int i;
  assert(str);
  elems = cbxmlbreak(str, TRUE);
  for(i = 0; i < CB_LISTNUM(elems); i++){
    elem = CB_LISTVAL(elems, i);
    if(elem[0] != '<' || !cbstrfwimatch(elem, "<meta")) continue;
    enc = NULL;
    attrs = cbxmlattrs(elem);
    equiv = cbmapget(attrs, "http-equiv", -1, NULL);
    if(!equiv) equiv = cbmapget(attrs, "HTTP-EQUIV", -1, NULL);
    if(!equiv) equiv = cbmapget(attrs, "Http-Equiv", -1, NULL);
    if(!equiv) equiv = cbmapget(attrs, "Http-equiv", -1, NULL);
    if(equiv && !cbstricmp(equiv, "Content-Type")){
      content = cbmapget(attrs, "content", -1, NULL);
      if(!content) content = cbmapget(attrs, "Content", -1, NULL);
      if(!content) content = cbmapget(attrs, "CONTENT", -1, NULL);
      if(content && ((pv = strstr(content, "charset")) != NULL ||
                     (pv = strstr(content, "Charset")) != NULL ||
                     (pv = strstr(content, "CHARSET")) != NULL)){
        enc = cbmemdup(pv + 8, -1);
        if((pv = strchr(enc, ';')) != NULL || (pv = strchr(enc, '\r')) != NULL ||
           (pv = strchr(enc, '\n')) != NULL || (pv = strchr(enc, ' ')) != NULL) *pv = '\0';
      }
    }
    cbmapclose(attrs);
    if(enc){
      CB_LISTCLOSE(elems);
      return enc;
    }
  }
  CB_LISTCLOSE(elems);
  return NULL;
}


/* unescape entity references of HTML */
static char *est_html_raw_text(const char *html){
  static const char *pairs[] = {
    /* basic symbols */
    "&amp;", "&", "&lt;", "<", "&gt;", ">", "&quot;", "\"", "&apos;", "'",
    /* ISO-8859-1 */
    "&nbsp;", "\xc2\xa0", "&iexcl;", "\xc2\xa1", "&cent;", "\xc2\xa2",
    "&pound;", "\xc2\xa3", "&curren;", "\xc2\xa4", "&yen;", "\xc2\xa5",
    "&brvbar;", "\xc2\xa6", "&sect;", "\xc2\xa7", "&uml;", "\xc2\xa8",
    "&copy;", "\xc2\xa9", "&ordf;", "\xc2\xaa", "&laquo;", "\xc2\xab",
    "&not;", "\xc2\xac", "&shy;", "\xc2\xad", "&reg;", "\xc2\xae",
    "&macr;", "\xc2\xaf", "&deg;", "\xc2\xb0", "&plusmn;", "\xc2\xb1",
    "&sup2;", "\xc2\xb2", "&sup3;", "\xc2\xb3", "&acute;", "\xc2\xb4",
    "&micro;", "\xc2\xb5", "&para;", "\xc2\xb6", "&middot;", "\xc2\xb7",
    "&cedil;", "\xc2\xb8", "&sup1;", "\xc2\xb9", "&ordm;", "\xc2\xba",
    "&raquo;", "\xc2\xbb", "&frac14;", "\xc2\xbc", "&frac12;", "\xc2\xbd",
    "&frac34;", "\xc2\xbe", "&iquest;", "\xc2\xbf", "&Agrave;", "\xc3\x80",
    "&Aacute;", "\xc3\x81", "&Acirc;", "\xc3\x82", "&Atilde;", "\xc3\x83",
    "&Auml;", "\xc3\x84", "&Aring;", "\xc3\x85", "&AElig;", "\xc3\x86",
    "&Ccedil;", "\xc3\x87", "&Egrave;", "\xc3\x88", "&Eacute;", "\xc3\x89",
    "&Ecirc;", "\xc3\x8a", "&Euml;", "\xc3\x8b", "&Igrave;", "\xc3\x8c",
    "&Iacute;", "\xc3\x8d", "&Icirc;", "\xc3\x8e", "&Iuml;", "\xc3\x8f",
    "&ETH;", "\xc3\x90", "&Ntilde;", "\xc3\x91", "&Ograve;", "\xc3\x92",
    "&Oacute;", "\xc3\x93", "&Ocirc;", "\xc3\x94", "&Otilde;", "\xc3\x95",
    "&Ouml;", "\xc3\x96", "&times;", "\xc3\x97", "&Oslash;", "\xc3\x98",
    "&Ugrave;", "\xc3\x99", "&Uacute;", "\xc3\x9a", "&Ucirc;", "\xc3\x9b",
    "&Uuml;", "\xc3\x9c", "&Yacute;", "\xc3\x9d", "&THORN;", "\xc3\x9e",
    "&szlig;", "\xc3\x9f", "&agrave;", "\xc3\xa0", "&aacute;", "\xc3\xa1",
    "&acirc;", "\xc3\xa2", "&atilde;", "\xc3\xa3", "&auml;", "\xc3\xa4",
    "&aring;", "\xc3\xa5", "&aelig;", "\xc3\xa6", "&ccedil;", "\xc3\xa7",
    "&egrave;", "\xc3\xa8", "&eacute;", "\xc3\xa9", "&ecirc;", "\xc3\xaa",
    "&euml;", "\xc3\xab", "&igrave;", "\xc3\xac", "&iacute;", "\xc3\xad",
    "&icirc;", "\xc3\xae", "&iuml;", "\xc3\xaf", "&eth;", "\xc3\xb0",
    "&ntilde;", "\xc3\xb1", "&ograve;", "\xc3\xb2", "&oacute;", "\xc3\xb3",
    "&ocirc;", "\xc3\xb4", "&otilde;", "\xc3\xb5", "&ouml;", "\xc3\xb6",
    "&divide;", "\xc3\xb7", "&oslash;", "\xc3\xb8", "&ugrave;", "\xc3\xb9",
    "&uacute;", "\xc3\xba", "&ucirc;", "\xc3\xbb", "&uuml;", "\xc3\xbc",
    "&yacute;", "\xc3\xbd", "&thorn;", "\xc3\xbe", "&yuml;", "\xc3\xbf",
    /* ISO-10646 */
    "&fnof;", "\xc6\x92", "&Alpha;", "\xce\x91", "&Beta;", "\xce\x92",
    "&Gamma;", "\xce\x93", "&Delta;", "\xce\x94", "&Epsilon;", "\xce\x95",
    "&Zeta;", "\xce\x96", "&Eta;", "\xce\x97", "&Theta;", "\xce\x98",
    "&Iota;", "\xce\x99", "&Kappa;", "\xce\x9a", "&Lambda;", "\xce\x9b",
    "&Mu;", "\xce\x9c", "&Nu;", "\xce\x9d", "&Xi;", "\xce\x9e",
    "&Omicron;", "\xce\x9f", "&Pi;", "\xce\xa0", "&Rho;", "\xce\xa1",
    "&Sigma;", "\xce\xa3", "&Tau;", "\xce\xa4", "&Upsilon;", "\xce\xa5",
    "&Phi;", "\xce\xa6", "&Chi;", "\xce\xa7", "&Psi;", "\xce\xa8",
    "&Omega;", "\xce\xa9", "&alpha;", "\xce\xb1", "&beta;", "\xce\xb2",
    "&gamma;", "\xce\xb3", "&delta;", "\xce\xb4", "&epsilon;", "\xce\xb5",
    "&zeta;", "\xce\xb6", "&eta;", "\xce\xb7", "&theta;", "\xce\xb8",
    "&iota;", "\xce\xb9", "&kappa;", "\xce\xba", "&lambda;", "\xce\xbb",
    "&mu;", "\xce\xbc", "&nu;", "\xce\xbd", "&xi;", "\xce\xbe",
    "&omicron;", "\xce\xbf", "&pi;", "\xcf\x80", "&rho;", "\xcf\x81",
    "&sigmaf;", "\xcf\x82", "&sigma;", "\xcf\x83", "&tau;", "\xcf\x84",
    "&upsilon;", "\xcf\x85", "&phi;", "\xcf\x86", "&chi;", "\xcf\x87",
    "&psi;", "\xcf\x88", "&omega;", "\xcf\x89", "&thetasym;", "\xcf\x91",
    "&upsih;", "\xcf\x92", "&piv;", "\xcf\x96", "&bull;", "\xe2\x80\xa2",
    "&hellip;", "\xe2\x80\xa6", "&prime;", "\xe2\x80\xb2", "&Prime;", "\xe2\x80\xb3",
    "&oline;", "\xe2\x80\xbe", "&frasl;", "\xe2\x81\x84", "&weierp;", "\xe2\x84\x98",
    "&image;", "\xe2\x84\x91", "&real;", "\xe2\x84\x9c", "&trade;", "\xe2\x84\xa2",
    "&alefsym;", "\xe2\x84\xb5", "&larr;", "\xe2\x86\x90", "&uarr;", "\xe2\x86\x91",
    "&rarr;", "\xe2\x86\x92", "&darr;", "\xe2\x86\x93", "&harr;", "\xe2\x86\x94",
    "&crarr;", "\xe2\x86\xb5", "&lArr;", "\xe2\x87\x90", "&uArr;", "\xe2\x87\x91",
    "&rArr;", "\xe2\x87\x92", "&dArr;", "\xe2\x87\x93", "&hArr;", "\xe2\x87\x94",
    "&forall;", "\xe2\x88\x80", "&part;", "\xe2\x88\x82", "&exist;", "\xe2\x88\x83",
    "&empty;", "\xe2\x88\x85", "&nabla;", "\xe2\x88\x87", "&isin;", "\xe2\x88\x88",
    "&notin;", "\xe2\x88\x89", "&ni;", "\xe2\x88\x8b", "&prod;", "\xe2\x88\x8f",
    "&sum;", "\xe2\x88\x91", "&minus;", "\xe2\x88\x92", "&lowast;", "\xe2\x88\x97",
    "&radic;", "\xe2\x88\x9a", "&prop;", "\xe2\x88\x9d", "&infin;", "\xe2\x88\x9e",
    "&ang;", "\xe2\x88\xa0", "&and;", "\xe2\x88\xa7", "&or;", "\xe2\x88\xa8",
    "&cap;", "\xe2\x88\xa9", "&cup;", "\xe2\x88\xaa", "&int;", "\xe2\x88\xab",
    "&there4;", "\xe2\x88\xb4", "&sim;", "\xe2\x88\xbc", "&cong;", "\xe2\x89\x85",
    "&asymp;", "\xe2\x89\x88", "&ne;", "\xe2\x89\xa0", "&equiv;", "\xe2\x89\xa1",
    "&le;", "\xe2\x89\xa4", "&ge;", "\xe2\x89\xa5", "&sub;", "\xe2\x8a\x82",
    "&sup;", "\xe2\x8a\x83", "&nsub;", "\xe2\x8a\x84", "&sube;", "\xe2\x8a\x86",
    "&supe;", "\xe2\x8a\x87", "&oplus;", "\xe2\x8a\x95", "&otimes;", "\xe2\x8a\x97",
    "&perp;", "\xe2\x8a\xa5", "&sdot;", "\xe2\x8b\x85", "&lceil;", "\xe2\x8c\x88",
    "&rceil;", "\xe2\x8c\x89", "&lfloor;", "\xe2\x8c\x8a", "&rfloor;", "\xe2\x8c\x8b",
    "&lang;", "\xe2\x8c\xa9", "&rang;", "\xe2\x8c\xaa", "&loz;", "\xe2\x97\x8a",
    "&spades;", "\xe2\x99\xa0", "&clubs;", "\xe2\x99\xa3", "&hearts;", "\xe2\x99\xa5",
    "&diams;", "\xe2\x99\xa6", "&OElig;", "\xc5\x92", "&oelig;", "\xc5\x93",
    "&Scaron;", "\xc5\xa0", "&scaron;", "\xc5\xa1", "&Yuml;", "\xc5\xb8",
    "&circ;", "\xcb\x86", "&tilde;", "\xcb\x9c", "&ensp;", "\xe2\x80\x82",
    "&emsp;", "\xe2\x80\x83", "&thinsp;", "\xe2\x80\x89", "&zwnj;", "\xe2\x80\x8c",
    "&zwj;", "\xe2\x80\x8d", "&lrm;", "\xe2\x80\x8e", "&rlm;", "\xe2\x80\x8f",
    "&ndash;", "\xe2\x80\x93", "&mdash;", "\xe2\x80\x94", "&lsquo;", "\xe2\x80\x98",
    "&rsquo;", "\xe2\x80\x99", "&sbquo;", "\xe2\x80\x9a", "&ldquo;", "\xe2\x80\x9c",
    "&rdquo;", "\xe2\x80\x9d", "&bdquo;", "\xe2\x80\x9e", "&dagger;", "\xe2\x80\xa0",
    "&Dagger;", "\xe2\x80\xa1", "&permil;", "\xe2\x80\xb0", "&lsaquo;", "\xe2\x80\xb9",
    "&rsaquo;", "\xe2\x80\xba", "&euro;", "\xe2\x82\xac",
    NULL
  };
  char *raw, *wp, buf[2], *tmp;
  int i, j, hit, num, tsiz;
  assert(html);
  CB_MALLOC(raw, strlen(html) * 3 + 1);
  wp = raw;
  while(*html != '\0'){
    if(*html == '&'){
      if(*(html + 1) == '#'){
        if(*(html + 2) == 'x' || *(html + 2) == 'X'){
          num = strtol(html + 3, NULL, 16);
        } else {
          num = atoi(html + 2);
        }
        buf[0] = num / 256;
        buf[1] = num % 256;
        if((tmp = est_uconv_out(buf, 2, &tsiz)) != NULL){
          for(j = 0; j < tsiz; j++){
            *wp = ((unsigned char *)tmp)[j];
            wp++;
          }
          free(tmp);
        }
        while(*html != ';' && *html != ' ' && *html != '\n' && *html != '\0'){
          html++;
        }
        if(*html == ';') html++;
      } else {
        hit = FALSE;
        for(i = 0; pairs[i] != NULL; i += 2){
          if(cbstrfwmatch(html, pairs[i])){
            wp += sprintf(wp, "%s", pairs[i+1]);
            html += strlen(pairs[i]);
            hit = TRUE;
            break;
          }
        }
        if(!hit){
          *wp = *html;
          wp++;
          html++;
        }
      }
    } else {
      *wp = *html;
      wp++;
      html++;
    }
  }
  *wp = '\0';
  return raw;
}


/* create a document object from MIME */
static ESTDOC *est_doc_new_from_mime(const char *buf, int size,
                                     const char *penc, int plang, int bcheck){
  ESTDOC *doc, *tdoc;
  CBMAP *attrs;
  const CBLIST *texts;
  CBLIST *parts, *lines;
  CBDATUM *datum;
  const char *key, *val, *bound, *part, *text, *line;
  char *body, *swap, numbuf[NUMBUFSIZ];
  int i, j, bsiz, psiz, ssiz, mht;
  assert(buf && size >= 0);
  doc = est_doc_new();
  attrs = cbmapopenex(MINIBNUM);
  body = cbmimebreak(buf, size, attrs, &bsiz);
  if((val = cbmapget(attrs, "subject", -1, NULL)) != NULL){
    est_doc_add_attr_mime(doc, ESTDATTRTITLE, val);
    if((val = est_doc_attr(doc, ESTDATTRTITLE)) != NULL) est_doc_add_hidden_text(doc, val);
  }
  if((val = cbmapget(attrs, "from", -1, NULL)) != NULL)
    est_doc_add_attr_mime(doc, ESTDATTRAUTHOR, val);
  if((val = cbmapget(attrs, "date", -1, NULL)) != NULL){
    est_doc_add_attr_mime(doc, ESTDATTRCDATE, val);
    est_doc_add_attr_mime(doc, ESTDATTRMDATE, val);
  }
  est_doc_add_attr(doc, ESTDATTRTYPE, "message/rfc822");
  sprintf(numbuf, "%d", size);
  est_doc_add_attr(doc, ESTDATTRSIZE, numbuf);
  cbmapiterinit(attrs);
  while((key = cbmapiternext(attrs, NULL)) != NULL){
    if((key[0] >= 'A' && key[0] <= 'Z') || key[0] == '@' || key[0] == '_') continue;
    val = cbmapiterval(key, NULL);
    est_doc_add_attr_mime(doc, key, val);
  }
  if((key = cbmapget(attrs, "TYPE", -1, NULL)) != NULL && cbstrfwimatch(key, "multipart/")){
    mht = cbstrfwimatch(key, "multipart/related");
    if((bound = cbmapget(attrs, "BOUNDARY", -1, NULL)) != NULL){
      parts = cbmimeparts(body, bsiz, bound);
      for(i = 0; i < CB_LISTNUM(parts) && i < 8; i++){
        part = CB_LISTVAL2(parts, i, psiz);
        if((tdoc = est_doc_new_from_mime(part, psiz, penc, plang, bcheck)) != NULL){
          if(mht){
            if((text = est_doc_attr(tdoc, ESTDATTRTITLE)) != NULL)
              est_doc_add_attr(doc, ESTDATTRTITLE, text);
            if((text = est_doc_attr(tdoc, ESTDATTRAUTHOR)) != NULL)
              est_doc_add_attr(doc, ESTDATTRAUTHOR, text);
          }
          texts = est_doc_texts(tdoc);
          for(j = 0; j < CB_LISTNUM(texts); j++){
            text = CB_LISTVAL(texts, j);
            est_doc_add_text(doc, text);
          }
          est_doc_delete(tdoc);
        }
      }
      CB_LISTCLOSE(parts);
    }
  } else {
    key = cbmapget(attrs, "content-transfer-encoding", -1, NULL);
    if(key && cbstrfwimatch(key, "base64")){
      swap = cbbasedecode(body, &ssiz);
      free(body);
      body = swap;
      bsiz = ssiz;
    } else if(key && cbstrfwimatch(key, "quoted-printable")){
      swap = cbquotedecode(body, &ssiz);
      free(body);
      body = swap;
      bsiz = ssiz;
    }
    key = cbmapget(attrs, "content-encoding", -1, NULL);
    if(key && (cbstrfwimatch(key, "x-gzip") || cbstrfwimatch(key, "gzip")) &&
       (swap = cbgzdecode(body, bsiz, &ssiz)) != NULL){
      free(body);
      body = swap;
      bsiz = ssiz;
    } else if(key && (cbstrfwimatch(key, "x-deflate") || cbstrfwimatch(key, "deflate")) &&
              (swap = cbinflate(body, bsiz, &ssiz)) != NULL){
      free(body);
      body = swap;
      bsiz = ssiz;
    }
    if(!(key = cbmapget(attrs, "TYPE", -1, NULL)) || cbstrfwimatch(key, "text/plain")){
      if(!bcheck || !est_check_binary(body, bsiz)){
        if(penc && (swap = est_iconv(body, bsiz, penc, "UTF-8", &ssiz, NULL)) != NULL){
          free(body);
          body = swap;
          bsiz = ssiz;
        } else if((key = cbmapget(attrs, "CHARSET", -1, NULL)) != NULL &&
                  (swap = est_iconv(body, bsiz, key, "UTF-8", &ssiz, NULL)) != NULL){
          free(body);
          body = swap;
          bsiz = ssiz;
        }
        lines = cbsplit(body, bsiz, "\n");
        CB_DATUMOPEN(datum);
        for(i = 0; i < CB_LISTNUM(lines); i++){
          line = CB_LISTVAL(lines, i);
          while(*line == ' ' || *line == '>' || *line == '|' || *line == '\t' || *line == '\r'){
            line++;
          }
          if(line[0] == '\0'){
            est_doc_add_text(doc, CB_DATUMPTR(datum));
            CB_DATUMSETSIZE(datum, 0);
          } else {
            CB_DATUMCAT(datum, " ", 1);
            CB_DATUMCAT(datum, line, strlen(line));
          }
        }
        est_doc_add_text(doc, CB_DATUMPTR(datum));
        CB_DATUMCLOSE(datum);
        CB_LISTCLOSE(lines);
      }
    } else if(cbstrfwimatch(key, "text/html") || cbstrfwimatch(key, "application/xhtml+xml")){
      if((tdoc = est_doc_new_from_html(body, bsiz, penc, plang, bcheck)) != NULL){
        if((text = est_doc_attr(tdoc, ESTDATTRTITLE)) != NULL){
          if(!est_doc_attr(doc, ESTDATTRTITLE)) est_doc_add_attr(doc, ESTDATTRTITLE, text);
          est_doc_add_text(doc, text);
        }
        if((text = est_doc_attr(tdoc, ESTDATTRAUTHOR)) != NULL){
          if(!est_doc_attr(doc, ESTDATTRAUTHOR)) est_doc_add_attr(doc, ESTDATTRAUTHOR, text);
          est_doc_add_text(doc, text);
        }
        texts = est_doc_texts(tdoc);
        for(i = 0; i < CB_LISTNUM(texts); i++){
          text = CB_LISTVAL(texts, i);
          est_doc_add_text(doc, text);
        }
        est_doc_delete(tdoc);
      }
    } else if(cbstrfwimatch(key, "message/rfc822")){
      if((tdoc = est_doc_new_from_mime(body, bsiz, penc, plang, bcheck)) != NULL){
        if((text = est_doc_attr(tdoc, ESTDATTRTITLE)) != NULL){
          if(!est_doc_attr(doc, ESTDATTRTITLE)) est_doc_add_attr(doc, ESTDATTRTITLE, text);
          est_doc_add_text(doc, text);
        }
        if((text = est_doc_attr(tdoc, ESTDATTRAUTHOR)) != NULL){
          if(!est_doc_attr(doc, ESTDATTRAUTHOR)) est_doc_add_attr(doc, ESTDATTRAUTHOR, text);
          est_doc_add_text(doc, text);
        }
        texts = est_doc_texts(tdoc);
        for(i = 0; i < CB_LISTNUM(texts); i++){
          text = CB_LISTVAL(texts, i);
          est_doc_add_text(doc, text);
        }
        est_doc_delete(tdoc);
      }
    } else if(cbstrfwimatch(key, "text/")){
      if((tdoc = est_doc_new_from_text(body, bsiz, penc, plang, bcheck)) != NULL){
        texts = est_doc_texts(tdoc);
        for(i = 0; i < CB_LISTNUM(texts); i++){
          text = CB_LISTVAL(texts, i);
          est_doc_add_text(doc, text);
        }
        est_doc_delete(tdoc);
      }
    }
  }
  free(body);
  cbmapclose(attrs);
  return doc;
}


/* set mime value as an attribute of a document */
static void est_doc_add_attr_mime(ESTDOC *doc, const char *name, const char *value){
  char enc[64], *ebuf, *rbuf;
  assert(doc && name && value);
  ebuf = cbmimedecode(value, enc);
  if((rbuf = est_iconv(ebuf, -1, enc, "UTF-8", NULL, NULL)) != NULL){
    est_doc_add_attr(doc, name, rbuf);
    free(rbuf);
  }
  free(ebuf);
}


/* generate a document with random text */
static ESTDOC *est_doc_new_from_chaos(int cnum, int snum, int mode){
  ESTDOC *doc;
  char *str;
  int i;
  doc = est_doc_new();
  snum *= pow(est_random_nd() + 0.5, 3.0);
  if(mode == RD_RAND){
    mode = est_random() * 100;
    if(mode < 20){
      mode = RD_ENG;
      est_doc_add_attr(doc, "mode", "english");
    } else if(mode < 40){
      mode = RD_LAT;
      est_doc_add_attr(doc, "mode", "latin");
    } else if(mode < 60){
      mode = RD_EURO;
      est_doc_add_attr(doc, "mode", "euromix");
    } else if(mode < 65){
      mode = RD_ORI;
      est_doc_add_attr(doc, "mode", "oriental");
    } else if(mode < 95){
      mode = RD_JPN;
      est_doc_add_attr(doc, "mode", "japanese");
    } else {
      mode = RD_CHAO;
      est_doc_add_attr(doc, "mode", "chaos");
    }
  }
  switch(mode){
  case RD_ENG: est_doc_add_attr(doc, "mode", "english"); break;
  case RD_LAT: est_doc_add_attr(doc, "mode", "latin"); break;
  case RD_ORI: est_doc_add_attr(doc, "mode", "oriental"); break;
  case RD_JPN: est_doc_add_attr(doc, "mode", "japanese"); break;
  case RD_EURO: est_doc_add_attr(doc, "mode", "euromix"); break;
  case RD_CHAO: est_doc_add_attr(doc, "mode", "chaos"); break;
  }
  for(i = 0; i <= snum; i++){
    str = est_random_str(cnum, mode);
    if(est_random() < 0.05){
      est_doc_add_hidden_text(doc, str);
    } else {
      est_doc_add_text(doc, str);
    }
    free(str);
  }
  return doc;
}


/* generate random string */
static char *est_random_str(int cnum, int mode){
  const char echrs[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  CBDATUM *buf;
  char wc[2], *str;
  int i, c, wlen, dec, mm, big, n;
  CB_DATUMOPEN(buf);
  cnum *= pow(est_random_nd() + 0.5, 3.0);
  wlen = est_random_nd() * 8 + 4;
  dec = (int)(est_random() * INT_MAX) % 10;
  big = (((int)(est_random() * INT_MAX) % 0x29)) * 0x100;
  for(i = 0; i < cnum; i++){
    switch(mode){
    case RD_ENG: case RD_LAT: case RD_EURO:
      mm = (int)(est_random() * INT_MAX) % 100;
      if((mode == RD_LAT || mode == RD_EURO) && mm < 5){
        c = 0x00a1 + (int)(pow(est_random_nd(), 2.0) * (0x00ff - 0x00a0));
      } else if(mode == RD_EURO && (mm < 30 || dec > 8)){
        if(dec % 2 == 0){
          c = 0x0391 + (int)(pow(est_random_nd(), 2.0) * (0x03d6 - 0x0391));
        } else {
          c = 0x0400 + (int)(pow(est_random_nd(), 2.0) * (0x045f - 0x0400));
        }
      } else if(mm < 95){
        if((n = est_random_nd() * (sizeof(echrs) - 1)) == (sizeof(echrs) - 1)) n = 0;
        c = echrs[n];
      } else {
        c = (int)(est_random() * ('@' - ' ')) + ' ';
      }
      if(--wlen < 1){
        c = ' ';
        wlen = pow(est_random_nd(), 3.0) * 8 + 4;
        dec = (int)(est_random() * INT_MAX) % 10;
      }
      break;
    case RD_ORI:
      c = big + est_random_nd() * 0x100;
      if(--wlen < 1){
        wlen = pow(est_random_nd(), 3.0) * 12 + 6;
        big = (((int)(est_random() * INT_MAX) % 0x29)) * 0x100;
      }
      break;
    case RD_JPN:
      if(dec < 4){
        c = 0x3041 + pow(est_random_nd(), 3.0) * (0x3094 - 0x3041);
      } else if(dec < 7){
        c = 0x30a1 + pow(est_random_nd(), 3.0) * (0x30fe - 0x30a1);
      } else if(dec < 9){
        c = 0x4e00 + pow(est_random_nd(), 3.0) * (0x9faf - 0x4e00);
      } else {
        if(est_random() < 0.7){
          c = 0x00a1 + (int)(pow(est_random_nd(), 2.0) * (0x00ff - 0x00a0));
        } else {
          c = 0x3041 + est_random() * (0xffef - 0x3041);
        }
      }
      if(--wlen < 1){
        wlen = pow(est_random_nd(), 3.0) * 12 + 6;
        dec = (int)(est_random() * INT_MAX) % 10;
      }
      break;
    default:
      if(est_random() < 0.2){
        c = 0x00a1 + (int)est_random() * (0x00ff - 0x00a0);
      } else {
        c = (int)(est_random() * 0x10000);
      }
      break;
    }
    if(c <= 0 || c >= 0x10000) c = 0x0020;
    wc[0] = c / 0x100;
    wc[1] = c % 0x100;
    CB_DATUMCAT(buf, wc, 2);
  }
  str = est_iconv(CB_DATUMPTR(buf), CB_DATUMSIZE(buf), "UTF-16BE", "UTF-8", NULL, NULL);
  CB_DATUMCLOSE(buf);
  return str;
}


/* compare two keywords by scores in descending order */
static int keysc_compare(const void *ap, const void *bp){
  return ((KEYSC *)bp)->pt - ((KEYSC *)ap)->pt;
}



/* END OF FILE */
