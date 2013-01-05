/*************************************************************************************************
 * The command line interface for the node API
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
#include "estmtdb.h"
#include "estnode.h"
#include "myconf.h"

#define NUMBUFSIZ      32                /* size of a buffer for a number */
#define IOBUFSIZ       8192              /* size of a buffer for I/O */
#define MINIBNUM       31                /* bucket number of a small map */
#define RESLIMSIZE     (1048576*128)     /* limitation of the entity body of response */
#define SEARCHMAX      10                /* maximum number of shown documents */
#define SEARCHAUX      32                /* minimum hits to adopt the auxiliary index */
#define DATTRNDURL     "#nodeurl"        /* name of the pseudo-attribute of the node URL */
#define DATTRNDSCORE   "#nodescore"      /* name of the pseudo-attribute of the node score */

enum {                                   /* enumeration for informing modes */
  IM_BASIC,                              /* basical information */
  IM_ADMINS,                             /* list of administrators */
  IM_USERS,                              /* list of users */
  IM_LINKS                               /* list of links */
};

enum {                                   /* enumeration for viewing modes */
  VM_PROT,                               /* protocol simulation */
  VM_URI,                                /* ID and URI */
  VM_XML                                 /* XML */
};


/* global variables */
const char *g_progname = NULL;           /* program name */
const char *g_pxhost = NULL;             /* host name of the proxy */
int g_pxport = 0;                        /* host name of the proxy */
int g_timeout = -1;                      /* timeout in seconds */
const char *g_authname = NULL;           /* name of authority */
const char *g_authpass = NULL;           /* password of authority */
int g_infomode = IM_BASIC;               /* informing mode */
int g_viewmode = VM_PROT;                /* viewing mode */
int g_nonparse = FALSE;                  /* whether to output response headers */
CBLIST *g_exheaders = NULL;              /* extension headers */


/* function prototypes */
int main(int argc, char **argv);
static void usage(void);
static void printferror(const char *format, ...);
static int runput(int argc, char **argv);
static int runout(int argc, char **argv);
static int runedit(int argc, char **argv);
static int runget(int argc, char **argv);
static int runetch(int argc, char **argv);
static int runuriid(int argc, char **argv);
static int runinform(int argc, char **argv);
static int runsync(int argc, char **argv);
static int runoptimize(int argc, char **argv);
static int runsearch(int argc, char **argv);
static int runlist(int argc, char **argv);
static int runsetuser(int argc, char **argv);
static int runsetlink(int argc, char **argv);
static int runraw(int argc, char **argv);
static void xmlprintf(const char *format, ...);
static int procput(const char *nurl, const char *file);
static int procout(const char *nurl, int id, const char *uri);
static int procedit(const char *nurl, int id, const char *uri,
                    const char *name, const char *value);
static int procget(const char *nurl, int id, const char *uri, const char *attr);
static int procetch(const char *nurl, int id, const char *uri);
static int procuriid(const char *nurl, const char *uri);
static int procinform(const char *nurl);
static int procsync(const char *nurl);
static int procoptimize(const char *nurl);
static int procsearch(const char *nurl, const char *phrase, const CBLIST *attrs, const char *ord,
                      int max, int sk, int aux, int opts, int depth, int mask, double ec,
                      const char *dis);
static int proclist(const char *nurl);
static int procsetuser(const char *nurl, const char *name, int mode);
static int procsetlink(const char *nurl, const char *url, const char *label, int credit);
static int procraw(const char *url, const char *file);


/* main routine */
int main(int argc, char **argv){
  int rv;
  est_proc_env_reset();
  g_progname = argv[0];
  if(!est_init_net_env()){
    printferror("could not initialize network environment");
    exit(1);
  }
  atexit(est_free_net_env);
  if(argc < 2) usage();
  rv = 0;
  if(!strcmp(argv[1], "put")){
    rv = runput(argc, argv);
  } else if(!strcmp(argv[1], "out")){
    rv = runout(argc, argv);
  } else if(!strcmp(argv[1], "edit")){
    rv = runedit(argc, argv);
  } else if(!strcmp(argv[1], "get")){
    rv = runget(argc, argv);
  } else if(!strcmp(argv[1], "etch")){
    rv = runetch(argc, argv);
  } else if(!strcmp(argv[1], "uriid")){
    rv = runuriid(argc, argv);
  } else if(!strcmp(argv[1], "inform")){
    rv = runinform(argc, argv);
  } else if(!strcmp(argv[1], "sync")){
    rv = runsync(argc, argv);
  } else if(!strcmp(argv[1], "optimize")){
    rv = runoptimize(argc, argv);
  } else if(!strcmp(argv[1], "search")){
    rv = runsearch(argc, argv);
  } else if(!strcmp(argv[1], "list")){
    rv = runlist(argc, argv);
  } else if(!strcmp(argv[1], "setuser")){
    rv = runsetuser(argc, argv);
  } else if(!strcmp(argv[1], "setlink")){
    rv = runsetlink(argc, argv);
  } else if(!strcmp(argv[1], "raw")){
    rv = runraw(argc, argv);
  } else {
    usage();
  }
  return rv;
}


/* print the usage and exit */
static void usage(void){
  fprintf(stderr, "%s: command line utility for the node API of Hyper Estraier\n", g_progname);
  fprintf(stderr, "\n");
  fprintf(stderr, "usage:\n");
  fprintf(stderr, "  %s put [-proxy host port] [-tout num] [-auth user pass]"
          " nurl [file]\n", g_progname);
  fprintf(stderr, "  %s out [-proxy host port] [-tout num] [-auth user pass]"
          " nurl expr\n", g_progname);
  fprintf(stderr, "  %s edit [-proxy host port] [-tout num] [-auth user pass]"
          " nurl expr name [value]\n", g_progname);
  fprintf(stderr, "  %s get [-proxy host port] [-tout num] [-auth user pass]"
          " nurl expr [attr]\n", g_progname);
  fprintf(stderr, "  %s etch [-proxy host port] [-tout num] [-auth user pass]"
          " nurl expr\n", g_progname);
  fprintf(stderr, "  %s uriid [-proxy host port] [-tout num] [-auth user pass]"
          " nurl uri\n", g_progname);
  fprintf(stderr, "  %s inform [-proxy host port] [-tout num] [-auth user pass]"
          " [-ia|-iu|-il] nurl\n", g_progname);
  fprintf(stderr, "  %s sync [-proxy host port] [-tout num] [-auth user pass]"
          " nurl\n", g_progname);
  fprintf(stderr, "  %s optimize [-proxy host port] [-tout num] [-auth user pass]"
          " nurl\n", g_progname);
  fprintf(stderr, "  %s search [-proxy host port] [-tout num] [-auth user pass]"
          " [-vu|-vx] [-ec rn] [-sf] [-attr expr] [-ord expr] [-max num] [-sk num] [-aux num]"
          " [-dis name] [-dpt num] [-mask num] nurl [phrase]\n", g_progname);
  fprintf(stderr, "  %s list [-proxy host port] [-tout num] [-auth user pass]"
          " nurl\n", g_progname);
  fprintf(stderr, "  %s setuser [-proxy host port] [-tout num] [-auth user pass]"
          " nurl name mode\n", g_progname);
  fprintf(stderr, "  %s setlink [-proxy host port] [-tout num] [-auth user pass]"
          " nurl url label credit\n", g_progname);
  fprintf(stderr, "  %s raw [-proxy host port] [-tout num] [-auth user pass] [-np] [-eh expr]"
          " url [file]\n", g_progname);
  fprintf(stderr, "\n");
  exit(1);
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


/* parse arguments of the put command */
static int runput(int argc, char **argv){
  char *nurl, *file;
  int i, rv;
  nurl = NULL;
  file = NULL;
  for(i = 2; i < argc; i++){
    if(!nurl && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-proxy")){
        if(++i >= argc) usage();
        g_pxhost = argv[i];
        if(++i >= argc) usage();
        g_pxport = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-tout")){
        if(++i >= argc) usage();
        g_timeout = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-auth")){
        if(++i >= argc) usage();
        g_authname = argv[i];
        if(++i >= argc) usage();
        g_authpass = argv[i];
      } else {
        usage();
      }
    } else if(!nurl){
      nurl = argv[i];
    } else if(!file){
      file = argv[i];
    } else {
      usage();
    }
  }
  if(!nurl) usage();
  rv = procput(nurl, file);
  return rv;
}


/* parse arguments of the out command */
static int runout(int argc, char **argv){
  char *nurl, *expr;
  int i, id, rv;
  nurl = NULL;
  expr = NULL;
  id = 0;
  for(i = 2; i < argc; i++){
    if(!nurl && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-proxy")){
        if(++i >= argc) usage();
        g_pxhost = argv[i];
        if(++i >= argc) usage();
        g_pxport = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-tout")){
        if(++i >= argc) usage();
        g_timeout = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-auth")){
        if(++i >= argc) usage();
        g_authname = argv[i];
        if(++i >= argc) usage();
        g_authpass = argv[i];
      } else {
        usage();
      }
    } else if(!nurl){
      nurl = argv[i];
    } else if(!expr){
      expr = argv[i];
    } else {
      usage();
    }
  }
  if(!nurl || !expr) usage();
  if((id = atoi(expr)) > 0) expr = NULL;
  rv = procout(nurl, id, expr);
  return rv;
}


/* parse arguments of the edit command */
static int runedit(int argc, char **argv){
  char *nurl, *expr, *name, *value;
  int i, id, rv;
  nurl = NULL;
  expr = NULL;
  name = NULL;
  value = NULL;
  id = 0;
  for(i = 2; i < argc; i++){
    if(!nurl && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-proxy")){
        if(++i >= argc) usage();
        g_pxhost = argv[i];
        if(++i >= argc) usage();
        g_pxport = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-tout")){
        if(++i >= argc) usage();
        g_timeout = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-auth")){
        if(++i >= argc) usage();
        g_authname = argv[i];
        if(++i >= argc) usage();
        g_authpass = argv[i];
      } else {
        usage();
      }
    } else if(!nurl){
      nurl = argv[i];
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
  if(!nurl || !expr || !name) usage();
  if((id = atoi(expr)) > 0) expr = NULL;
  rv = procedit(nurl, id, expr, name, value);
  return rv;
}


/* parse arguments of the get command */
static int runget(int argc, char **argv){
  char *nurl, *expr, *attr;
  int i, id, rv;
  nurl = NULL;
  expr = NULL;
  attr = NULL;
  for(i = 2; i < argc; i++){
    if(!nurl && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-proxy")){
        if(++i >= argc) usage();
        g_pxhost = argv[i];
        if(++i >= argc) usage();
        g_pxport = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-tout")){
        if(++i >= argc) usage();
        g_timeout = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-auth")){
        if(++i >= argc) usage();
        g_authname = argv[i];
        if(++i >= argc) usage();
        g_authpass = argv[i];
      } else {
        usage();
      }
    } else if(!nurl){
      nurl = argv[i];
    } else if(!expr){
      expr = argv[i];
    } else if(!attr){
      attr = argv[i];
    } else {
      usage();
    }
  }
  if(!nurl || !expr) usage();
  if((id = atoi(expr)) > 0) expr = NULL;
  rv = procget(nurl, id, expr, attr);
  return rv;
}


/* parse arguments of the etch command */
static int runetch(int argc, char **argv){
  char *nurl, *expr;
  int i, id, rv;
  nurl = NULL;
  expr = NULL;
  for(i = 2; i < argc; i++){
    if(!nurl && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-proxy")){
        if(++i >= argc) usage();
        g_pxhost = argv[i];
        if(++i >= argc) usage();
        g_pxport = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-tout")){
        if(++i >= argc) usage();
        g_timeout = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-auth")){
        if(++i >= argc) usage();
        g_authname = argv[i];
        if(++i >= argc) usage();
        g_authpass = argv[i];
      } else {
        usage();
      }
    } else if(!nurl){
      nurl = argv[i];
    } else if(!expr){
      expr = argv[i];
    } else {
      usage();
    }
  }
  if(!nurl || !expr) usage();
  if((id = atoi(expr)) > 0) expr = NULL;
  rv = procetch(nurl, id, expr);
  return rv;
}


/* parse arguments of the uriid command */
static int runuriid(int argc, char **argv){
  char *nurl, *uri;
  int i, rv;
  nurl = NULL;
  uri = NULL;
  for(i = 2; i < argc; i++){
    if(!nurl && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-proxy")){
        if(++i >= argc) usage();
        g_pxhost = argv[i];
        if(++i >= argc) usage();
        g_pxport = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-tout")){
        if(++i >= argc) usage();
        g_timeout = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-auth")){
        if(++i >= argc) usage();
        g_authname = argv[i];
        if(++i >= argc) usage();
        g_authpass = argv[i];
      } else {
        usage();
      }
    } else if(!nurl){
      nurl = argv[i];
    } else if(!uri){
      uri = argv[i];
    } else {
      usage();
    }
  }
  if(!nurl || !uri) usage();
  rv = procuriid(nurl, uri);
  return rv;
}


/* parse arguments of the inform command */
static int runinform(int argc, char **argv){
  char *nurl;
  int i, rv;
  nurl = NULL;
  for(i = 2; i < argc; i++){
    if(!nurl && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-proxy")){
        if(++i >= argc) usage();
        g_pxhost = argv[i];
        if(++i >= argc) usage();
        g_pxport = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-tout")){
        if(++i >= argc) usage();
        g_timeout = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-auth")){
        if(++i >= argc) usage();
        g_authname = argv[i];
        if(++i >= argc) usage();
        g_authpass = argv[i];
      } else if(!strcmp(argv[i], "-ia")){
        g_infomode = IM_ADMINS;
      } else if(!strcmp(argv[i], "-iu")){
        g_infomode = IM_USERS;
      } else if(!strcmp(argv[i], "-il")){
        g_infomode = IM_LINKS;
      } else {
        usage();
      }
    } else if(!nurl){
      nurl = argv[i];
    } else {
      usage();
    }
  }
  if(!nurl) usage();
  rv = procinform(nurl);
  return rv;
}


/* parse arguments of the sync command */
static int runsync(int argc, char **argv){
  char *nurl;
  int i, rv;
  nurl = NULL;
  for(i = 2; i < argc; i++){
    if(!nurl && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-proxy")){
        if(++i >= argc) usage();
        g_pxhost = argv[i];
        if(++i >= argc) usage();
        g_pxport = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-tout")){
        if(++i >= argc) usage();
        g_timeout = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-auth")){
        if(++i >= argc) usage();
        g_authname = argv[i];
        if(++i >= argc) usage();
        g_authpass = argv[i];
      } else {
        usage();
      }
    } else if(!nurl){
      nurl = argv[i];
    } else {
      usage();
    }
  }
  if(!nurl) usage();
  rv = procsync(nurl);
  return rv;
}


/* parse arguments of the optimize command */
static int runoptimize(int argc, char **argv){
  char *nurl;
  int i, rv;
  nurl = NULL;
  for(i = 2; i < argc; i++){
    if(!nurl && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-proxy")){
        if(++i >= argc) usage();
        g_pxhost = argv[i];
        if(++i >= argc) usage();
        g_pxport = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-tout")){
        if(++i >= argc) usage();
        g_timeout = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-auth")){
        if(++i >= argc) usage();
        g_authname = argv[i];
        if(++i >= argc) usage();
        g_authpass = argv[i];
      } else {
        usage();
      }
    } else if(!nurl){
      nurl = argv[i];
    } else {
      usage();
    }
  }
  if(!nurl) usage();
  rv = procoptimize(nurl);
  return rv;
}


/* parse arguments of the search command */
static int runsearch(int argc, char **argv){
  CBDATUM *pbuf;
  CBLIST *attrs;
  char *nurl, *ord, *dis;
  int i, max, sk, aux, opts, depth, mask, rv;
  double ec;
  nurl = NULL;
  ord = NULL;
  dis = NULL;
  max = SEARCHMAX;
  sk = 0;
  aux = SEARCHAUX;
  opts = 0;
  depth = 0;
  mask = 0;
  ec = -1.0;
  pbuf = cbdatumopen(NULL, -1);
  cbglobalgc(pbuf, (void (*)(void *))cbdatumclose);
  attrs = cblistopen();
  cbglobalgc(attrs, (void (*)(void *))cblistclose);
  for(i = 2; i < argc; i++){
    if(!nurl && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-proxy")){
        if(++i >= argc) usage();
        g_pxhost = argv[i];
        if(++i >= argc) usage();
        g_pxport = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-tout")){
        if(++i >= argc) usage();
        g_timeout = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-auth")){
        if(++i >= argc) usage();
        g_authname = argv[i];
        if(++i >= argc) usage();
        g_authpass = argv[i];
      } else if(!strcmp(argv[i], "-vu")){
        g_viewmode = VM_URI;
      } else if(!strcmp(argv[i], "-vx")){
        g_viewmode = VM_XML;
      } else if(!strcmp(argv[i], "-ec")){
        if(++i >= argc) usage();
        ec = strtod(argv[i], NULL);
      } else if(!strcmp(argv[i], "-sf")){
        opts |= ESTCONDSIMPLE;
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
      } else if(!strcmp(argv[i], "-dpt")){
        if(++i >= argc) usage();
        depth = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-mask")){
        if(++i >= argc) usage();
        mask = atoi(argv[i]);
      } else {
        usage();
      }
    } else if(!nurl){
      nurl = argv[i];
    } else {
      if(cbdatumsize(pbuf) > 0) cbdatumcat(pbuf, " ", 1);
      cbdatumcat(pbuf, argv[i], -1);
    }
  }
  if(!nurl || depth < 0) usage();
  rv = procsearch(nurl, cbdatumptr(pbuf), attrs, ord, max, sk, aux, opts, depth, mask, ec, dis);
  return rv;
}


/* parse arguments of the list command */
static int runlist(int argc, char **argv){
  char *nurl;
  int i, rv;
  nurl = NULL;
  for(i = 2; i < argc; i++){
    if(!nurl && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-proxy")){
        if(++i >= argc) usage();
        g_pxhost = argv[i];
        if(++i >= argc) usage();
        g_pxport = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-tout")){
        if(++i >= argc) usage();
        g_timeout = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-auth")){
        if(++i >= argc) usage();
        g_authname = argv[i];
        if(++i >= argc) usage();
        g_authpass = argv[i];
      } else {
        usage();
      }
    } else if(!nurl){
      nurl = argv[i];
    } else {
      usage();
    }
  }
  if(!nurl) usage();
  rv = proclist(nurl);
  return rv;
}


/* parse arguments of the setuser command */
static int runsetuser(int argc, char **argv){
  char *nurl, *name, *mstr;
  int i, mode, rv;
  nurl = NULL;
  name = NULL;
  mstr = NULL;
  mode = 0;
  for(i = 2; i < argc; i++){
    if(!nurl && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-proxy")){
        if(++i >= argc) usage();
        g_pxhost = argv[i];
        if(++i >= argc) usage();
        g_pxport = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-tout")){
        if(++i >= argc) usage();
        g_timeout = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-auth")){
        if(++i >= argc) usage();
        g_authname = argv[i];
        if(++i >= argc) usage();
        g_authpass = argv[i];
      } else {
        usage();
      }
    } else if(!nurl){
      nurl = argv[i];
    } else if(!name){
      name = argv[i];
    } else if(!mstr){
      mstr = argv[i];
    } else {
      usage();
    }
  }
  if(!nurl || !name || !mstr || (mode = atoi(mstr)) < 0) usage();
  rv = procsetuser(nurl, name, mode);
  return rv;
}


/* parse arguments of the setlink command */
static int runsetlink(int argc, char **argv){
  char *nurl, *url, *label, *cstr;
  int i, credit, rv;
  nurl = NULL;
  url = NULL;
  label = NULL;
  cstr = NULL;
  for(i = 2; i < argc; i++){
    if(!nurl && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-proxy")){
        if(++i >= argc) usage();
        g_pxhost = argv[i];
        if(++i >= argc) usage();
        g_pxport = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-tout")){
        if(++i >= argc) usage();
        g_timeout = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-auth")){
        if(++i >= argc) usage();
        g_authname = argv[i];
        if(++i >= argc) usage();
        g_authpass = argv[i];
      } else {
        usage();
      }
    } else if(!nurl){
      nurl = argv[i];
    } else if(!url){
      url = argv[i];
    } else if(!label){
      label = argv[i];
    } else if(!cstr){
      cstr = argv[i];
    } else {
      usage();
    }
  }
  if(!nurl || !url || !label || !cstr) usage();
  credit = atoi(cstr);
  rv = procsetlink(nurl, url, label, credit);
  return rv;
}


/* parse arguments of the raw command */
static int runraw(int argc, char **argv){
  char *url, *file;
  int i, rv;
  g_exheaders = cblistopen();
  cbglobalgc(g_exheaders, (void (*)(void *))cblistclose);
  url = NULL;
  file = NULL;
  for(i = 2; i < argc; i++){
    if(!url && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-proxy")){
        if(++i >= argc) usage();
        g_pxhost = argv[i];
        if(++i >= argc) usage();
        g_pxport = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-tout")){
        if(++i >= argc) usage();
        g_timeout = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-auth")){
        if(++i >= argc) usage();
        g_authname = argv[i];
        if(++i >= argc) usage();
        g_authpass = argv[i];
      } else if(!strcmp(argv[i], "-np")){
        g_nonparse = TRUE;
      } else if(!strcmp(argv[i], "-eh")){
        if(++i >= argc) usage();
        cblistpush(g_exheaders, argv[i], -1);
      } else {
        usage();
      }
    } else if(!url){
      url = argv[i];
    } else if(!file){
      file = argv[i];
    } else {
      usage();
    }
  }
  if(!url) usage();
  rv = procraw(url, file);
  return rv;
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


/* perform the put command */
static int procput(const char *nurl, const char *file){
  ESTNODE *node;
  ESTDOC *doc;
  char *draft;
  int err;
  if(!(draft = cbreadfile(file, NULL))){
    printferror("%s: could not open", file ? file : "(stdin)");
    return 1;
  }
  err = FALSE;
  node = est_node_new(nurl);
  if(g_pxhost && g_pxport > 0) est_node_set_proxy(node, g_pxhost, g_pxport);
  if(g_timeout >= 0) est_node_set_timeout(node, g_timeout);
  if(g_authname && g_authpass) est_node_set_auth(node, g_authname, g_authpass);
  doc = est_doc_new_from_draft(draft);
  if(!est_node_put_doc(node, doc)){
    printferror("failed: %d", est_node_status(node));
    err = TRUE;
  }
  est_doc_delete(doc);
  est_node_delete(node);
  free(draft);
  return err ? 1 : 0;
}


/* perform the out command */
static int procout(const char *nurl, int id, const char *uri){
  ESTNODE *node;
  int err;
  err = FALSE;
  node = est_node_new(nurl);
  if(g_pxhost && g_pxport > 0) est_node_set_proxy(node, g_pxhost, g_pxport);
  if(g_timeout >= 0) est_node_set_timeout(node, g_timeout);
  if(g_authname && g_authpass) est_node_set_auth(node, g_authname, g_authpass);
  if(uri ? !est_node_out_doc_by_uri(node, uri) : !est_node_out_doc(node, id)){
    printferror("failed: %d", est_node_status(node));
    err = TRUE;
  }
  est_node_delete(node);
  return err ? 1 : 0;
}


/* perform the edit command */
static int procedit(const char *nurl, int id, const char *uri,
                    const char *name, const char *value){
  ESTNODE *node;
  ESTDOC *doc;
  int err;
  err = FALSE;
  node = est_node_new(nurl);
  if(g_pxhost && g_pxport > 0) est_node_set_proxy(node, g_pxhost, g_pxport);
  if(g_timeout >= 0) est_node_set_timeout(node, g_timeout);
  if(g_authname && g_authpass) est_node_set_auth(node, g_authname, g_authpass);
  if((doc = uri ? est_node_get_doc_by_uri(node, uri) : est_node_get_doc(node, id)) != NULL){
    est_doc_add_attr(doc, name, value);
    if(!(est_node_edit_doc(node, doc))){
      printferror("failed: %d", est_node_status(node));
      err = TRUE;
    }
    est_doc_delete(doc);
  } else {
    printferror("failed: %d", est_node_status(node));
    err = TRUE;
  }
  est_node_delete(node);
  return err ? 1 : 0;
}


/* perform the get command */
static int procget(const char *nurl, int id, const char *uri, const char *attr){
  ESTNODE *node;
  ESTDOC *doc;
  char *draft;
  int err;
  err = FALSE;
  node = est_node_new(nurl);
  if(g_pxhost && g_pxport > 0) est_node_set_proxy(node, g_pxhost, g_pxport);
  if(g_timeout >= 0) est_node_set_timeout(node, g_timeout);
  if(g_authname && g_authpass) est_node_set_auth(node, g_authname, g_authpass);
  err = FALSE;
  if(attr){
    if((draft = uri ? est_node_get_doc_attr_by_uri(node, uri, attr) :
        est_node_get_doc_attr(node, id, attr)) != NULL){
      printf("%s\n", draft);
      free(draft);
    } else {
      printferror("failed: %d", est_node_status(node));
      err = TRUE;
    }
  } else {
    if((doc = uri ? est_node_get_doc_by_uri(node, uri) : est_node_get_doc(node, id)) != NULL){
      draft = est_doc_dump_draft(doc);
      printf("%s", draft);
      free(draft);
      est_doc_delete(doc);
    } else {
      printferror("failed: %d", est_node_status(node));
      err = TRUE;
    }
  }
  est_node_delete(node);
  return err ? 1 : 0;
}


/* perform the etch command */
static int procetch(const char *nurl, int id, const char *uri){
  ESTNODE *node;
  CBMAP *kwords;
  const char *kbuf;
  int err, ksiz;
  err = FALSE;
  node = est_node_new(nurl);
  if(g_pxhost && g_pxport > 0) est_node_set_proxy(node, g_pxhost, g_pxport);
  if(g_timeout >= 0) est_node_set_timeout(node, g_timeout);
  if(g_authname && g_authpass) est_node_set_auth(node, g_authname, g_authpass);
  err = FALSE;
  if((kwords = uri ? est_node_etch_doc_by_uri(node, uri) : est_node_etch_doc(node, id)) != NULL){
    cbmapiterinit(kwords);
    while((kbuf = cbmapiternext(kwords, &ksiz)) != NULL){
      printf("%s\t%s\n", kbuf, cbmapget(kwords, kbuf, ksiz, NULL));
    }
    cbmapclose(kwords);
  } else {
    printferror("failed: %d", est_node_status(node));
    err = TRUE;
  }
  est_node_delete(node);
  return err ? 1 : 0;
}


/* perform the uriid command */
static int procuriid(const char *nurl, const char *uri){
  ESTNODE *node;
  int err, id;
  err = FALSE;
  node = est_node_new(nurl);
  if(g_pxhost && g_pxport > 0) est_node_set_proxy(node, g_pxhost, g_pxport);
  if(g_timeout >= 0) est_node_set_timeout(node, g_timeout);
  if(g_authname && g_authpass) est_node_set_auth(node, g_authname, g_authpass);
  err = FALSE;
  if((id = est_node_uri_to_id(node, uri)) != -1){
    printf("%d\n", id);
  } else {
    printferror("failed: %d", est_node_status(node));
    err = TRUE;
  }
  est_node_delete(node);
  return err ? 1 : 0;
}


/* perform the inform command */
static int procinform(const char *nurl){
  ESTNODE *node;
  const CBLIST *admins, *users, *links;
  const char *name, *label;
  int i, err, dnum, wnum;
  double size, ratio;
  err = FALSE;
  node = est_node_new(nurl);
  if(g_pxhost && g_pxport > 0) est_node_set_proxy(node, g_pxhost, g_pxport);
  if(g_timeout >= 0) est_node_set_timeout(node, g_timeout);
  if(g_authname && g_authpass) est_node_set_auth(node, g_authname, g_authpass);
  err = FALSE;
  name = est_node_name(node);
  label = est_node_label(node);
  dnum = est_node_doc_num(node);
  wnum = est_node_word_num(node);
  size = est_node_size(node);
  ratio = est_node_cache_usage(node);
  admins = est_node_admins(node);
  users = est_node_users(node);
  links = est_node_links(node);
  if(name && label && dnum >= 0 && wnum >= 0 && size >= 0.0 && ratio >= 0.0 &&
     admins && users && links){
    switch(g_infomode){
    default:
      printf("%s\t%s\t%d\t%d\t%.0f\t%.6f\n", name, label, dnum, wnum, size, ratio);
      break;
    case IM_ADMINS:
      for(i = 0; i < cblistnum(admins); i++){
        printf("%s\n", cblistval(admins, i, NULL));
      }
      break;
    case IM_USERS:
      for(i = 0; i < cblistnum(users); i++){
        printf("%s\n", cblistval(users, i, NULL));
      }
      break;
    case IM_LINKS:
      for(i = 0; i < cblistnum(links); i++){
        printf("%s\n", cblistval(links, i, NULL));
      }
      break;
    }
  } else {
    printferror("failed: %d", est_node_status(node));
    err = TRUE;
  }
  est_node_delete(node);
  return err ? 1 : 0;
}


/* perform the sync command */
static int procsync(const char *nurl){
  ESTNODE *node;
  int err;
  err = FALSE;
  node = est_node_new(nurl);
  if(g_pxhost && g_pxport > 0) est_node_set_proxy(node, g_pxhost, g_pxport);
  if(g_timeout >= 0) est_node_set_timeout(node, g_timeout);
  if(g_authname && g_authpass) est_node_set_auth(node, g_authname, g_authpass);
  err = FALSE;
  if(!est_node_sync(node)){
    printferror("failed: %d", est_node_status(node));
    err = TRUE;
  }
  est_node_delete(node);
  return err ? 1 : 0;
}


/* perform the optimize command */
static int procoptimize(const char *nurl){
  ESTNODE *node;
  int err;
  err = FALSE;
  node = est_node_new(nurl);
  if(g_pxhost && g_pxport > 0) est_node_set_proxy(node, g_pxhost, g_pxport);
  if(g_timeout >= 0) est_node_set_timeout(node, g_timeout);
  if(g_authname && g_authpass) est_node_set_auth(node, g_authname, g_authpass);
  err = FALSE;
  if(!est_node_optimize(node)){
    printferror("failed: %d", est_node_status(node));
    err = TRUE;
  }
  est_node_delete(node);
  return err ? 1 : 0;
}


/* perform the inform command */
static int procsearch(const char *nurl, const char *phrase, const CBLIST *attrs, const char *ord,
                      int max, int sk, int aux, int opts, int depth, int mask, double ec,
                      const char *dis){
  ESTNODE *node;
  ESTNODERES *nres;
  ESTRESDOC *rdoc, **sdocs;
  ESTCOND *cond;
  CBMAP *hints;
  CBLIST *names, *elems, *lines;
  const char *kbuf, *vbuf, *ndurl, *ndscore;
  char tbuf[NUMBUFSIZ];
  int i, j, err, ksiz, vsiz, dnum, wnum, snum, id, ld;
  err = FALSE;
  node = est_node_new(nurl);
  if(g_pxhost && g_pxport > 0) est_node_set_proxy(node, g_pxhost, g_pxport);
  if(g_timeout >= 0) est_node_set_timeout(node, g_timeout);
  if(g_authname && g_authpass) est_node_set_auth(node, g_authname, g_authpass);
  err = FALSE;
  cond = est_cond_new();
  while(*phrase > '\0' && *phrase <= ' '){
    phrase++;
  }
  if(phrase[0] != '\0' || cblistnum(attrs) < 1) est_cond_set_phrase(cond, phrase);
  for(i = 0; i < cblistnum(attrs); i++){
    est_cond_add_attr(cond, cblistval(attrs, i, NULL));
  }
  if(ord) est_cond_set_order(cond, ord);
  if(max >= 0){
    est_cond_set_max(cond, ec >= 0.01 ? max * ((2.0 / ec) + 0.5) : max);
  } else {
    max = INT_MAX;
  }
  if(sk > 0) est_cond_set_skip(cond, sk);
  est_cond_set_options(cond, opts);
  est_cond_set_auxiliary(cond, aux);
  if(dis) est_cond_set_distinct(cond, dis);
  est_cond_set_mask(cond, mask);
  if((nres = est_node_search(node, cond, depth)) != NULL){
    hints = est_noderes_hints(nres);
    if(ec > 0.0) est_noderes_eclipse(nres, max, ec);
    switch(g_viewmode){
    case VM_URI:
      printf("%s\n", est_border_str());
      cbmapiterinit(hints);
      while((kbuf = cbmapiternext(hints, &ksiz)) != NULL){
        printf("%s\t%s\n", kbuf, cbmapget(hints, kbuf, ksiz, NULL));
      }
      printf("\n");
      printf("%s\n", est_border_str());
      for(i = 0; i < max && i < est_noderes_doc_num(nres); i++){
        rdoc = est_noderes_get_doc(nres, i);
        ndurl = est_resdoc_attr(rdoc, DATTRNDURL);
        ndscore = est_resdoc_attr(rdoc, DATTRNDSCORE);
        printf("%s\t%s\t%s\n", est_resdoc_uri(rdoc), ndurl ? ndurl : "", ndscore ? ndscore : "");
      }
      printf("%s:END\n", est_border_str());
      break;
    case VM_XML:
      xmlprintf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
      xmlprintf("<estresult xmlns=\"%@\" version=\"%@\">\n", _EST_XNSEARCH, est_version);
      xmlprintf("<meta>\n");
      if((vbuf = cbmapget(hints, "NODE", -1, NULL)) != NULL)
        xmlprintf("<node url=\"%@\"/>\n", vbuf);
      if((vbuf = cbmapget(hints, "HIT", -1, NULL)) != NULL)
        xmlprintf("<hit number=\"%@\"/>\n", vbuf);
      for(i = 1; i < cbmaprnum(hints); i++){
        sprintf(tbuf, "HINT#%d", i);
        if(!(vbuf = cbmapget(hints, tbuf, -1, &vsiz))) break;
        elems = cbsplit(vbuf, vsiz, "\t");
        if(cblistnum(elems) == 2){
          xmlprintf("<hit key=\"%@\" number=\"%@\"/>\n",
                    cblistval(elems, 0, NULL), cblistval(elems, 1, NULL));
        }
        cblistclose(elems);
      }
      if((vbuf = cbmapget(hints, "TIME", -1, NULL)) != NULL){
        xmlprintf("<time total=\"%@\"", vbuf);
        if((vbuf = cbmapget(hints, "TIME#i", -1, NULL)) != NULL)
          xmlprintf(" index=\"%@\"", vbuf);
        xmlprintf("/>\n");
      }
      dnum = 0;
      wnum = 0;
      if((vbuf = cbmapget(hints, "DOCNUM", -1, NULL)) != NULL) dnum = atoi(vbuf);
      if((vbuf = cbmapget(hints, "WORDNUM", -1, NULL)) != NULL) wnum = atoi(vbuf);
      xmlprintf("<total docnum=\"%d\" wordnum=\"%d\"/>\n", dnum, wnum);
      for(i = 0; i < cbmaprnum(hints); i++){
        sprintf(tbuf, "LINK#%d", i);
        if(!(vbuf = cbmapget(hints, tbuf, -1, &vsiz))) break;
        elems = cbsplit(vbuf, vsiz, "\t");
        if(cblistnum(elems) == 7){
          xmlprintf("<link url=\"%@\" label=\"%@\" credit=\"%@\" docnum=\"%@\" wordnum=\"%@\""
                    " size=\"%@\" hit=\"%@\"",
                    cblistval(elems, 0, NULL), cblistval(elems, 1, NULL),
                    cblistval(elems, 2, NULL), cblistval(elems, 3, NULL),
                    cblistval(elems, 4, NULL), cblistval(elems, 5, NULL),
                    cblistval(elems, 6, NULL));
          sprintf(tbuf, "TIME#%d", i);
          if((vbuf = cbmapget(hints, tbuf, -1, &vsiz)) != NULL)
            xmlprintf(" time=\"%@\"", vbuf);
          xmlprintf("/>\n");
        }
        cblistclose(elems);
      }
      xmlprintf("</meta>\n");
      for(i = 0; i < max && i < est_noderes_doc_num(nres); i++){
        rdoc = est_noderes_get_doc(nres, i);
        id = -1;
        if((vbuf = est_resdoc_attr(rdoc, ESTDATTRID)) != NULL) id = atoi(vbuf);
        xmlprintf("<document id=\"%d\" uri=\"%@\">\n", id, est_resdoc_uri(rdoc));
        names = est_resdoc_attr_names(rdoc);
        for(j = 0; j < cblistnum(names); j++){
          kbuf = cblistval(names, j, NULL);
          if(!strcmp(kbuf, ESTDATTRID) || !strcmp(kbuf, ESTDATTRURI)) continue;
          xmlprintf("<attribute name=\"%@\" value=\"%@\"/>\n",
                    kbuf, est_resdoc_attr(rdoc, kbuf));
        }
        cblistclose(names);
        vbuf = est_resdoc_keywords(rdoc);
        if(vbuf[0] != '\0'){
          xmlprintf("<vector>");
          elems = cbsplit(vbuf, -1, "\t");
          for(j = 0; j < cblistnum(elems) - 1; j += 2){
            xmlprintf("<element key=\"%@\" number=\"%@\"/>",
                      cblistval(elems, j, NULL), cblistval(elems, j + 1, NULL));
          }
          cblistclose(elems);
          xmlprintf("</vector>\n");
        }
        sdocs = est_resdoc_shadows(rdoc, &snum);
        for(j = 0; j < snum; j++){
          xmlprintf("<shadow uri=\"%@\" similarity=\"%.3f\"/>\n",
                    est_resdoc_uri(sdocs[j]), est_resdoc_similarity(sdocs[j]));
        }
        xmlprintf("<snippet>");
        lines = cbsplit(est_resdoc_snippet(rdoc), -1, "\n");
        ld = TRUE;
        for(j = 0; j < cblistnum(lines); j++){
          vbuf = cblistval(lines, j, &vsiz);
          elems = cbsplit(vbuf, vsiz, "\t");
          if(vbuf[0] == '\0'){
            if(!ld) xmlprintf("<delimiter/>");
            ld = TRUE;
          } else if(cblistnum(elems) == 1){
            xmlprintf("%@", cblistval(elems, 0, NULL));
            ld = FALSE;
          } else {
            xmlprintf("<key normal=\"%@\">%@</key>",
                      cblistval(elems, 1, NULL), cblistval(elems, 0, NULL));
            ld = FALSE;
          }
          cblistclose(elems);
        }
        cblistclose(lines);
        xmlprintf("</snippet>\n");
        xmlprintf("</document>\n");
      }
      xmlprintf("</estresult>\n");
      break;
    default:
      printf("%s\n", est_border_str());
      cbmapiterinit(hints);
      while((kbuf = cbmapiternext(hints, &ksiz)) != NULL){
        printf("%s\t%s\n", kbuf, cbmapget(hints, kbuf, ksiz, NULL));
      }
      printf("\n");
      for(i = 0; i < max && i < est_noderes_doc_num(nres); i++){
        printf("%s\n", est_border_str());
        rdoc = est_noderes_get_doc(nres, i);
        names = est_resdoc_attr_names(rdoc);
        for(j = 0; j < cblistnum(names); j++){
          kbuf = cblistval(names, j, NULL);
          printf("%s=%s\n", kbuf, est_resdoc_attr(rdoc, kbuf));
        }
        cblistclose(names);
        vbuf = est_resdoc_keywords(rdoc);
        if(vbuf[0] != '\0') printf("%s\t%s\n", ESTDCNTLVECTOR, vbuf);
        sdocs = est_resdoc_shadows(rdoc, &snum);
        for(j = 0; j < snum; j++){
          printf("%s\t%s\t%.3f\n",
                 ESTDCNTLSHADOW, est_resdoc_uri(sdocs[j]), est_resdoc_similarity(sdocs[j]));
        }
        printf("\n");
        printf("%s", est_resdoc_snippet(rdoc));
      }
      printf("%s:END\n", est_border_str());
      break;
    }
    est_noderes_delete(nres);
  } else {
    printferror("failed: %d", est_node_status(node));
    err = TRUE;
  }
  est_cond_delete(cond);
  est_node_delete(node);
  return err ? 1 : 0;
}


/* perform the list command */
static int proclist(const char *nurl){
  ESTNODE *node;
  ESTDOC *doc;
  const char *tmp;
  int err;
  node = est_node_new(nurl);
  if(g_pxhost && g_pxport > 0) est_node_set_proxy(node, g_pxhost, g_pxport);
  if(g_timeout >= 0) est_node_set_timeout(node, g_timeout);
  if(g_authname && g_authpass) est_node_set_auth(node, g_authname, g_authpass);
  err = FALSE;
  if(est_node_iter_init(node)){
    while((doc = est_node_iter_next(node)) != NULL){
      tmp = est_doc_attr(doc, ESTDATTRID);
      printf("%s\t", tmp ? tmp : "");
      tmp = est_doc_attr(doc, ESTDATTRURI);
      printf("%s\t", tmp ? tmp : "");
      tmp = est_doc_attr(doc, ESTDATTRDIGEST);
      printf("%s\t", tmp ? tmp : "");
      tmp = est_doc_attr(doc, ESTDATTRCDATE);
      printf("%s\t", tmp ? tmp : "");
      tmp = est_doc_attr(doc, ESTDATTRMDATE);
      printf("%s\t", tmp ? tmp : "");
      tmp = est_doc_attr(doc, ESTDATTRADATE);
      printf("%s\t", tmp ? tmp : "");
      tmp = est_doc_attr(doc, ESTDATTRTITLE);
      printf("%s\t", tmp ? tmp : "");
      tmp = est_doc_attr(doc, ESTDATTRAUTHOR);
      printf("%s\t", tmp ? tmp : "");
      tmp = est_doc_attr(doc, ESTDATTRTYPE);
      printf("%s\t", tmp ? tmp : "");
      tmp = est_doc_attr(doc, ESTDATTRLANG);
      printf("%s\t", tmp ? tmp : "");
      tmp = est_doc_attr(doc, ESTDATTRGENRE);
      printf("%s\t", tmp ? tmp : "");
      tmp = est_doc_attr(doc, ESTDATTRSIZE);
      printf("%s\t", tmp ? tmp : "");
      tmp = est_doc_attr(doc, ESTDATTRWEIGHT);
      printf("%s\t", tmp ? tmp : "");
      tmp = est_doc_attr(doc, ESTDATTRMISC);
      printf("%s\n", tmp ? tmp : "");
      est_doc_delete(doc);
    }
  } else {
    printferror("failed: %d", est_node_status(node));
    err = TRUE;
  }
  est_node_delete(node);
  return err ? 1 : 0;
}


/* perform the setuser command */
static int procsetuser(const char *nurl, const char *name, int mode){
  ESTNODE *node;
  int err;
  err = FALSE;
  node = est_node_new(nurl);
  if(g_pxhost && g_pxport > 0) est_node_set_proxy(node, g_pxhost, g_pxport);
  if(g_timeout >= 0) est_node_set_timeout(node, g_timeout);
  if(g_authname && g_authpass) est_node_set_auth(node, g_authname, g_authpass);
  err = FALSE;
  if(!est_node_set_user(node, name, mode)){
    printferror("failed: %d", est_node_status(node));
    err = TRUE;
  }
  est_node_delete(node);
  return err ? 1 : 0;
}


/* perform the setlink command */
static int procsetlink(const char *nurl, const char *url, const char *label, int credit){
  ESTNODE *node;
  int err;
  err = FALSE;
  node = est_node_new(nurl);
  if(g_pxhost && g_pxport > 0) est_node_set_proxy(node, g_pxhost, g_pxport);
  if(g_timeout >= 0) est_node_set_timeout(node, g_timeout);
  if(g_authname && g_authpass) est_node_set_auth(node, g_authname, g_authpass);
  err = FALSE;
  if(!est_node_set_link(node, url, label, credit)){
    printferror("failed: %d", est_node_status(node));
    err = TRUE;
  }
  est_node_delete(node);
  return err ? 1 : 0;
}


/* perform the raw command */
static int procraw(const char *url, const char *file){
  CBMAP *resheads;
  CBDATUM *resbody;
  const char *ptr;
  char *auth, *reqbody;
  int i, err, rbsiz, rescode, size;
  if(file){
    if(!(reqbody = cbreadfile(strcmp(file, "-") ? file : NULL, &rbsiz))){
      printferror("%s: could not open", file);
      return 1;
    }
  } else {
    reqbody = NULL;
    rbsiz = 0;
  }
  err = FALSE;
  if(g_pxport < 1) g_pxport = 80;
  resheads = cbmapopenex(MINIBNUM);
  resbody = cbdatumopen(NULL, -1);
  auth = (g_authname && g_authpass) ? cbsprintf("%s:%s", g_authname, g_authpass) : NULL;
  if(est_url_shuttle(url, g_pxhost, g_pxport, g_timeout, RESLIMSIZE, auth,
                     g_exheaders, reqbody, rbsiz, &rescode, resheads, resbody)){
    if(rescode < 200 && rescode >= 300) err = TRUE;
    if(g_nonparse){
      printf("%s\r\n", cbmapget(resheads, "", 0, NULL));
      cbmapiterinit(resheads);
      while((ptr = cbmapiternext(resheads, &size)) != NULL){
        if(size < 1) continue;
        printf("%s: %s\r\n", ptr, cbmapget(resheads, ptr, size, NULL));
      }
      printf("\r\n");
    }
    ptr = cbdatumptr(resbody);
    size = cbdatumsize(resbody);
    for(i = 0; i < size; i++){
      putchar(ptr[i]);
    }
  } else {
    printferror("%s: connection failed", url);
    err = TRUE;
  }
  free(auth);
  cbdatumclose(resbody);
  cbmapclose(resheads);
  free(reqbody);
  return err ? 1 : 0;
}



/* END OF FILE */
