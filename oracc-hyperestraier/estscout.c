/*************************************************************************************************
 * A intersection meta searcher of Hyper Estraier
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


#if defined(MYFCGI)
#include <fcgi_stdio.h>
#endif
#include "estraier.h"
#include "estmtdb.h"
#include "estnode.h"
#include "myconf.h"

#define CONFSUFFIX     ".conf"           /* suffix of the configuration file */
#define CACHESUFFIX    ".ecc"            /* suffix of temporary files */
#define NUMBUFSIZ      32                /* size of a buffer for a number */
#define OUTBUFSIZ      262144            /* size of the output buffer */
#define MINIBNUM       31                /* bucket number of map for trivial use */
#define CONDATTRMAX    9                 /* maximum number of attribute conditions */
#define SEARCHMAX      10                /* maximum number of shown documents */
#define DUPCHKMAX      65536             /* maximum number of duplication checked documents */
#define LOCKRETRYNUM   8                 /* number of retries when locking failure */
#define CCPURGEFREQ    512               /* frequency of purging temporary files */
#define CCSCVOIDMAX    1024              /* maximum number of narrowing scores to void cache */
#define CCGENMINTIME   1978              /* minimum elapsed milliseconds to generate cache */

typedef struct {                         /* type of structure for a search thread */
  ESTMTDB *db;                           /* database object */
  ESTCOND *cond;                         /* condition object */
  const int *scores;                     /* array of narrowing scores */
  int snum;                              /* number of the score array */
  CBMAP *rmap;                           /* result object */
  time_t now;                            /* the current time */
  int hnum;                              /* number of corresponding documents */
  int alive;                             /* whether to be alive */
} TARGSRCH;


/* global variables for configurations */
int g_runcnt = 0;                        /* count of running */
const char *g_conffile = NULL;           /* path of the configuration file */
const CBLIST *g_indexlist = NULL;        /* list of real indexes */
int g_lockindex = FALSE;                 /* whether to perform file locking to the database */
int g_condgstep = -1;                    /* step of N-gram */
int g_dotfidf = FALSE;                   /* whether to do TF-IDF tuning */
int g_scancheck = -1;                    /* number of checked documents by scanning */
int g_phraseform = 0;                    /* mode of phrase form */
int g_wildmax = -1;                      /* maximum number of extension of wild cards */
int g_stmode = FALSE;                    /* whether to be single thread mode */
const char *g_idattr = NULL;             /* attribute for identification */
const CBLIST *g_idsuflist = NULL;        /* list of suffixes for identification */
const char *g_ordexpr = NULL;            /* attribute for ordering when attribute search */
int g_dupcheck = FALSE;                  /* whether to check duplication of identifiers */
int g_union = FALSE;                     /* whether to perform union meta search */
int g_score = -1;                        /* scoring method when logical operation */
const char *g_tmpdir = NULL;             /* path of the directory for temporary files */
int g_cclife = 0;                        /* lifetime of cache files */
const char *g_logfile = NULL;            /* path of the log file */
const char *g_logformat = NULL;          /* format of the log */


/* global variables for parameters */
const CBLIST *p_phraselist = NULL;       /* list of phrase conditions */
const CBLIST *p_attrlist = NULL;         /* list of attribute conditions */
int p_max = -1;                          /* number of output documents */
const char *p_distinct = NULL;           /* name of distinction attribute */
int p_fresh = FALSE;                     /* whether to retrieve fresh result */
int p_stmode = FALSE;                    /* whether to be single thread mode */
const char *p_logmsg = "";               /* additional log message */


/* other global variables */
char g_outbuf[OUTBUFSIZ];                /* output buffer */
const char *g_scriptname = NULL;         /* name of the script */
ESTMTDB **g_dbs = NULL;                  /* database handles */
int g_metamin = 0;                       /* minimum number of occurence times */
int g_sort = FALSE;                      /* whether to sort explicitrly */
int g_hnum = 0;                          /* number of corresponding documents */
int g_cache = FALSE;                     /* whether cache was used */


/* function prototypes */
int main(int argc, char **argv);
static int realmain(int argc, char **argv);
static void showerror(const char *msg);
static const char *skiplabel(const char *str);
static CBMAP *getparameters(void);
static void showresult(void);
static void *procsearch(void *targ);
static char *myencode(ESTCOND *cond);
static char *caturisuffixes(ESTMTDB *db, int id, const char *uri);
static void outputlog(void);


/* main routine */
int main(int argc, char **argv){
#if defined(MYFCGI)
  int i, ecode;
  while(FCGI_Accept() >= 0){
    g_runcnt++;
    p_phraselist = NULL;
    p_attrlist = NULL;
    p_max = -1;
    p_distinct = NULL;
    p_fresh = FALSE;
    p_stmode = FALSE;
    p_logmsg = "";
    g_metamin = 0;
    g_sort = FALSE;
    g_hnum = 0;
    g_cache = FALSE;
    realmain(argc, argv);
    fflush(stdout);
    if(g_runcnt >= 64){
      for(i = 0; i < cblistnum(g_indexlist); i++){
        if(g_dbs[i]){
          est_mtdb_close(g_dbs[i], &ecode);
        }
      }
      exit(0);
    }
  }
  return 0;
#else
  int i, rv, ecode;
  est_proc_env_reset();
  rv = realmain(argc, argv);
  fflush(stdout);
  for(i = 0; i < cblistnum(g_indexlist); i++){
    if(g_dbs[i]){
      est_mtdb_close(g_dbs[i], &ecode);
    }
  }
  return rv;
#endif
}


/* main routine */
static int realmain(int argc, char **argv){
  CBLIST *lines, *ilist, *slist, *plist, *alist;
  CBMAP *params;
  const char *rp;
  char *tmp, *wp, numbuf[NUMBUFSIZ];
  int i, j, len, omode, ecode;
  /* set configurations */
  setvbuf(stdout, g_outbuf, _IOFBF, OUTBUFSIZ);
  g_scriptname = argv[0];
  if((rp = getenv("SCRIPT_NAME")) != NULL) g_scriptname = rp;
  if((rp = strrchr(g_scriptname, '/')) != NULL) g_scriptname = rp + 1;
  tmp = cbmalloc(strlen(g_scriptname) + strlen(CONFSUFFIX) + 1);
  sprintf(tmp, "%s", g_scriptname);
  cbglobalgc(tmp, free);
  if(!(wp = strrchr(tmp, '.'))) wp = tmp + strlen(tmp);
  sprintf(wp, "%s", CONFSUFFIX);
  g_conffile = tmp;
  if(!(lines = cbreadlines(g_conffile))) showerror("the configuration file is missing.");
  cbglobalgc(lines, (void (*)(void *))cblistclose);
  ilist = cblistopen();
  cbglobalgc(ilist, (void (*)(void *))cblistclose);
  slist = cblistopen();
  cbglobalgc(slist, (void (*)(void *))cblistclose);
  for(i = 0; i < cblistnum(lines); i++){
    rp = cblistval(lines, i, NULL);
    if(cbstrfwimatch(rp, "indexname:")){
      rp = skiplabel(rp);
      if(*rp != '\0') cblistpush(ilist, rp, -1);
    } else if(cbstrfwimatch(rp, "lockindex:")){
      g_lockindex = atoi(skiplabel(rp)) > 0;
    } else if(cbstrfwimatch(rp, "condgstep:")){
      g_condgstep = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "dotfidf:")){
      g_dotfidf = atoi(skiplabel(rp)) > 0;
    } else if(cbstrfwimatch(rp, "scancheck:")){
      g_scancheck = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "phraseform:")){
      g_phraseform = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "wildmax:")){
      g_wildmax = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "stmode:")){
      g_stmode = atoi(skiplabel(rp)) > 0;
    } else if(cbstrfwimatch(rp, "idattr:")){
      g_idattr = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "idsuffix:")){
      rp = skiplabel(rp);
      if(*rp != '\0') cblistpush(slist, rp, -1);
    } else if(cbstrfwimatch(rp, "ordexpr:")){
      g_ordexpr = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "dupcheck:")){
      g_dupcheck = atoi(skiplabel(rp)) > 0;
    } else if(cbstrfwimatch(rp, "union:")){
      g_union = atoi(skiplabel(rp)) > 0;
    } else if(cbstrfwimatch(rp, "score:")){
      g_score = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "tmpdir:")){
      g_tmpdir = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "cclife:")){
      g_cclife = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "logfile:")){
      g_logfile = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "logformat:")){
      g_logformat = skiplabel(rp);
    }
  }
  if(cblistnum(ilist) < 1) showerror("indexname is undefined.");
  g_indexlist = ilist;
  if(g_condgstep < 1) showerror("condgstep is undefined.");
  if(g_scancheck < 0) showerror("scancheck is undefined.");
  if(g_phraseform < 1) showerror("phraseform is undefined.");
  if(g_wildmax < 0) showerror("wildmax is undefined.");
  if(!g_idattr) showerror("idattr is undefined.");
  g_idsuflist = slist;
  if(!g_ordexpr) showerror("ordexpr is undefined.");
  if(g_score < 0) showerror("score is undefined.");
  if(!g_tmpdir) showerror("tmpdir is undefined.");
  if(!g_logfile) showerror("logfile is undefined.");
  if(!g_logformat) showerror("logformat is undefined.");
  /* read parameters */
  params = getparameters();
  cbglobalgc(params, (void (*)(void *))cbmapclose);
  plist = cblistopen();
  cbglobalgc(plist, (void (*)(void *))cblistclose);
  alist = cblistopen();
  cbglobalgc(alist, (void (*)(void *))cblistclose);
  if(g_union && ((rp = cbmapget(params, "phrase", -1, NULL)) ||
                 (rp = cbmapget(params, "phrase1", -1, NULL)))){
    while(*rp == ' ' || *rp == '\t'){
      rp++;
    }
    for(i = 0; i < cblistnum(g_indexlist); i++){
      cblistpush(plist, rp, -1);
    }
  } else {
    for(i = 1; i <= cblistnum(g_indexlist); i++){
      len = sprintf(numbuf, "phrase%d", i);
      rp = cbmapget(params, numbuf, len, NULL);
      if(!rp && i == 1) rp = cbmapget(params, "phrase", -1, NULL);
      if(!rp) rp = "";
      while(*rp == ' ' || *rp == '\t'){
        rp++;
      }
      cblistpush(plist, rp, -1);
    }
  }
  if((rp = cbmapget(params, "attr", -1, NULL)) != NULL) cblistpush(alist, rp, -1);
  for(i = 1; i <= CONDATTRMAX; i++){
    len = sprintf(numbuf, "attr%d", i);
    rp = cbmapget(params, numbuf, len, NULL);
    if(!rp && i == 1) rp = cbmapget(params, "attr", -1, NULL);
    if(!rp) rp = "";
    while(*rp == ' ' || *rp == '\t'){
      rp++;
    }
    if(*rp != '\0') cblistpush(alist, rp, -1);
  }
  if((rp = cbmapget(params, "max", -1, NULL)) != NULL) p_max = atoi(rp);
  if(p_max < 0) p_max = SEARCHMAX;
  if(!(p_distinct = cbmapget(params, "distinct", -1, NULL))) p_distinct = "";
  if((rp = cbmapget(params, "fresh", -1, NULL)) != NULL) p_fresh = atoi(rp) > 0;
  if((rp = cbmapget(params, "stmode", -1, NULL)) != NULL) p_stmode = atoi(rp) > 0;
  if(!(p_logmsg = cbmapget(params, "logmsg", -1, NULL))) p_logmsg = "";
  p_phraselist = plist;
  p_attrlist = alist;
  /* open databases */
  if(!g_dbs){
    g_dbs = cbmalloc(cblistnum(g_indexlist) * sizeof(ESTMTDB));
    cbglobalgc(g_dbs, free);
    for(i = 0; i < cblistnum(g_indexlist); i++){
      g_dbs[i] = NULL;
    }
  }
  g_metamin = 0;
  g_sort = TRUE;
  omode = ESTDBREADER;
  if(!g_lockindex) omode |= ESTDBNOLCK;
  for(i = 0; i < cblistnum(g_indexlist); i++){
    if((rp = cblistval(p_phraselist, i, NULL)) != NULL && *rp != '\0'){
      if(!g_dbs[i]){
        for(j = 0; j <= LOCKRETRYNUM; j++){
          if((g_dbs[i] = est_mtdb_open(cblistval(g_indexlist, i, NULL), omode,
                                       &ecode)) != NULL) break;
          est_usleep(1000 * 1000);
        }
        if(!g_dbs[i]) showerror("one of the indexes is missing or broken or being updated.");
        est_mtdb_set_cache_size(g_dbs[i], -1, 512, 128, 0);
        est_mtdb_set_wildmax(g_dbs[i], g_wildmax);
      }
      g_metamin++;
      g_sort = FALSE;
    }
  }
  rp = cblistval(p_phraselist, 0, NULL);
  if(*rp == '\0' && cblistnum(p_attrlist) > 0){
    if(!g_dbs[0]){
      for(j = 0; j <= LOCKRETRYNUM; j++){
        if((g_dbs[0] = est_mtdb_open(cblistval(g_indexlist, 0, NULL), omode,
                                     &ecode)) != NULL) break;
        est_usleep(1000 * 1000);
      }
      if(!g_dbs[0]) showerror("one of the indexes is missing or broken or being updated.");
      est_mtdb_set_cache_size(g_dbs[0], -1, 512, 128, 0);
      est_mtdb_set_wildmax(g_dbs[0], g_wildmax);
    }
    g_metamin++;
  }
  if(g_union) g_metamin = 1;
  /* show the result */
  showresult();
  /* output the log message */
  outputlog();
  return 0;
}


/* show the error page and exit */
static void showerror(const char *msg){
  printf("Status: 500 Internal Server Error\r\n");
  printf("Content-Type: text/plain; charset=UTF-8\r\n");
  printf("\r\n");
  printf("Error: %s\n", msg);
  exit(1);
}


/* skip the label of a line */
static const char *skiplabel(const char *str){
  if(!(str = strchr(str, ':'))) return "";
  str++;
  while(*str != '\0' && (*str == ' ' || *str == '\t')){
    str++;
  }
  return str;
}


/* get CGI parameters */
static CBMAP *getparameters(void){
  int maxlen = 1024 * 1024 * 32;
  CBMAP *map, *attrs;
  CBLIST *pairs, *parts;
  const char *rp, *body;
  char *buf, *key, *val, *dkey, *dval, *wp, *bound, *fbuf, *aname;
  int i, len, c, blen, flen;
  map = cbmapopenex(37);
  buf = NULL;
  len = 0;
  if((rp = getenv("REQUEST_METHOD")) != NULL && !strcmp(rp, "POST") &&
     (rp = getenv("CONTENT_LENGTH")) != NULL && (len = atoi(rp)) > 0){
    if(len > maxlen) len = maxlen;
    buf = cbmalloc(len + 1);
    for(i = 0; i < len && (c = getchar()) != EOF; i++){
      buf[i] = c;
    }
    buf[i] = '\0';
    if(i != len){
      free(buf);
      buf = NULL;
    }
  } else if((rp = getenv("QUERY_STRING")) != NULL){
    buf = cbmemdup(rp, -1);
    len = strlen(buf);
  }
  if(buf && len > 0){
    if((rp = getenv("CONTENT_TYPE")) != NULL && cbstrfwmatch(rp, "multipart/form-data") &&
       (rp = strstr(rp, "boundary=")) != NULL){
      rp += 9;
      bound = cbmemdup(rp, -1);
      if((wp = strchr(bound, ';')) != NULL) *wp = '\0';
      parts = cbmimeparts(buf, len, bound);
      for(i = 0; i < cblistnum(parts); i++){
        body = cblistval(parts, i, &blen);
        attrs = cbmapopen();
        fbuf = cbmimebreak(body, blen, attrs, &flen);
        if((rp = cbmapget(attrs, "NAME", -1, NULL)) != NULL){
          cbmapput(map, rp, -1, fbuf, flen, FALSE);
          aname = cbsprintf("%s-filename", rp);
          if((rp = cbmapget(attrs, "FILENAME", -1, NULL)) != NULL)
            cbmapput(map, aname, -1, rp, -1, FALSE);
          free(aname);
        }
        free(fbuf);
        cbmapclose(attrs);
      }
      cblistclose(parts);
      free(bound);
    } else {
      pairs = cbsplit(buf, -1, "&");
      for(i = 0; i < cblistnum(pairs); i++){
        key = cbmemdup(cblistval(pairs, i, NULL), -1);
        if((val = strchr(key, '=')) != NULL){
          *(val++) = '\0';
          dkey = cburldecode(key, NULL);
          dval = cburldecode(val, NULL);
          cbmapput(map, dkey, -1, dval, -1, FALSE);
          free(dval);
          free(dkey);
        }
        free(key);
      }
      cblistclose(pairs);
    }
  }
  free(buf);
  return map;
}


/* show the result */
static void showresult(void){
  pthread_t *thlist;
  TARGSRCH *arglist;
  ESTRESMAPELEM *elems;
  ESTCOND *cond;
  CBMAP *rmap;
  CBLIST *list;
  const char *rp;
  char tmppath[PATH_MAX], *expr;
  int i, j, num, hnum, max;
  struct stat sbuf;
  time_t now;
  printf("Content-Type: text/plain\r\n");
  printf("Cache-Control: no-cache, must-revalidate, no-transform\r\n");
  printf("Pragma: no-cache\r\n");
  printf("X-Run-Count: %d\r\n", g_runcnt);
  printf("\r\n");
  now = time(NULL);
  if(*g_tmpdir != '\0' && *g_tmpdir != '@' && g_cclife >= 0 &&
     (now + g_runcnt) % CCPURGEFREQ == 1 && (list = cbdirlist(g_tmpdir)) != NULL){
    for(i = 0; i < cblistnum(list); i++){
      rp = cblistval(list, i, NULL);
      if(!cbstrbwmatch(rp, CACHESUFFIX)) continue;
      sprintf(tmppath, "%s%c%s", g_tmpdir, ESTPATHCHR, rp);
      if(stat(tmppath, &sbuf) != -1 && now - sbuf.st_mtime > g_cclife) unlink(tmppath);
    }
    cblistclose(list);
  }
  rmap = cbmapopen();
  thlist = cbmalloc(cblistnum(g_indexlist) * sizeof(pthread_t));
  arglist = cbmalloc(cblistnum(g_indexlist) * sizeof(TARGSRCH));
  max = p_max * 1.3 + 1;
  for(i = 0; i < cblistnum(g_indexlist); i++){
    arglist[i].db = g_dbs[i];
    cond = est_cond_new();
    if((rp = cblistval(p_phraselist, i, NULL)) != NULL && *rp != '\0')
      est_cond_set_phrase(cond, rp);
    if(g_union || i == 0){
      for(j = 0; j < cblistnum(p_attrlist); j++){
        est_cond_add_attr(cond, cblistval(p_attrlist, j, NULL));
      }
    }
    if(*g_ordexpr == '$'){
      expr = cbsprintf("%s %s", g_ordexpr + 1, ESTORDNUMD);
      est_cond_set_order(cond, expr);
      free(expr);
    } else if(*g_ordexpr != '\0' && g_sort){
      est_cond_set_order(cond, g_ordexpr);
    }
    if(g_metamin < 2 && cblistnum(p_attrlist) < 1) est_cond_set_max(cond, max);
    switch(g_condgstep){
    case 1:
      est_cond_set_options(cond, ESTCONDSURE);
      break;
    case 2:
      est_cond_set_options(cond, ESTCONDUSUAL);
      break;
    case 3:
      est_cond_set_options(cond, ESTCONDFAST);
      break;
    case 4:
      est_cond_set_options(cond, ESTCONDAGITO);
      break;
    }
    if(!g_dotfidf) est_cond_set_options(cond, ESTCONDNOIDF);
    switch(g_phraseform){
    case 2:
      est_cond_set_options(cond, ESTCONDSIMPLE);
      break;
    case 3:
      est_cond_set_options(cond, ESTCONDROUGH);
      break;
    case 4:
      est_cond_set_options(cond, ESTCONDUNION);
      break;
    case 5:
      est_cond_set_options(cond, ESTCONDISECT);
      break;
    }
    est_cond_set_options(cond, ESTCONDSCFB);
    if(*p_distinct != '\0') est_cond_set_distinct(cond, p_distinct);
    arglist[i].cond = cond;
    arglist[i].scores = NULL;
    arglist[i].snum = -1;
    arglist[i].rmap = rmap;
    arglist[i].now = now;
    arglist[i].hnum = 0;
  }
  if(g_stmode || (!g_union && g_metamin < 2) || p_stmode){
    for(i = cblistnum(g_indexlist) - 1; i >= 0; i--){
      if(i < cblistnum(g_indexlist) - 1){
        arglist[i].scores = arglist[i+1].scores;
        arglist[i].snum = arglist[i+1].snum;
      }
      procsearch(arglist + i);
      if(!g_union && arglist[i].hnum == 0) break;
    }
  } else {
    for(i = 0; i < cblistnum(g_indexlist); i++){
      arglist[i].alive = TRUE;
      if(pthread_create(thlist + i, NULL, procsearch, arglist + i) != 0) arglist[i].alive = FALSE;
    }
    for(i = 0; i < cblistnum(g_indexlist); i++){
      if(arglist[i].alive) pthread_join(thlist[i], NULL);
    }
  }
  hnum = 0;
  for(i = 0; i < cblistnum(g_indexlist); i++){
    est_cond_delete(arglist[i].cond);
    if(arglist[i].hnum > 0) hnum += arglist[i].hnum;
  }
  free(arglist);
  free(thlist);
  elems = est_resmap_dump(rmap, g_metamin, &num);
  if(num < max && num < hnum) hnum = num;
  g_hnum = g_metamin < 2 ? hnum : num;
  printf("%d\n", g_hnum);
  for(i = 0; i < num && i < p_max; i++){
    printf("%s\t%d\n", elems[i].key, elems[i].score);
  }
  free(elems);
  cbmapclose(rmap);
}


/* search an index */
static void *procsearch(void *targ){
  static pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
  TARGSRCH *argp;
  FILE *ofp;
  ESTMTDB *db;
  ESTCOND *cond;
  ESTDOC *doc;
  CBMAP *rmap, *hints, *umap;
  CBLIST *list;
  const char *rp, *uri;
  char tmppath[PATH_MAX], *value, *expr, *ord, *enc, *pv, *mp, numbuf[NUMBUFSIZ];
  const int *scores;
  int i, *res, rnum, snum, check, len, num;
  struct stat sbuf;
  double etime;
  time_t now;
  argp = (TARGSRCH *)targ;
  db = argp->db;
  cond = argp->cond;
  rmap = argp->rmap;
  now = argp->now;
  if(!db || (!est_cond_phrase(cond) && !est_cond_attrs(cond))){
    argp->hnum = -1;
    return NULL;
  }
  if(*g_tmpdir != '\0' && !p_fresh && !est_cond_phrase(cond) &&
     (!argp->scores || argp->snum > CCSCVOIDMAX)){
    enc = myencode(cond);
    if(*g_tmpdir == '@'){
      num = dpinnerhash(enc, -1) % 0x100;
      if(g_cclife >= 0 && (now + g_runcnt) % (CCPURGEFREQ / 16) == 0){
        sprintf(tmppath, "%s%c%02x", g_tmpdir + 1, ESTPATHCHR, num);
        if((list = cbdirlist(tmppath)) != NULL){
          for(i = 0; i < cblistnum(list); i++){
            rp = cblistval(list, i, NULL);
            if(!cbstrbwmatch(rp, CACHESUFFIX)) continue;
            sprintf(tmppath, "%s%c%02x%c%s",
                    g_tmpdir + 1, ESTPATHCHR, num, ESTPATHCHR, rp);
            if(stat(tmppath, &sbuf) != -1 && now - sbuf.st_mtime > g_cclife) unlink(tmppath);
          }
          cblistclose(list);
        }
      }
      sprintf(tmppath, "%s%c%02x%c%s%s",
              g_tmpdir + 1, ESTPATHCHR, num, ESTPATHCHR, enc, CACHESUFFIX);
    } else {
      sprintf(tmppath, "%s%c%s%s", g_tmpdir, ESTPATHCHR, enc, CACHESUFFIX);
    }
    free(enc);
  } else {
    *tmppath = '\0';
  }
  if(*tmppath != '\0' && stat(tmppath, &sbuf) != -1){
    if((g_cclife < 0 || now - sbuf.st_mtime <= g_cclife) &&
       (value = cbreadfile(tmppath, &len)) != NULL){
      if(pthread_mutex_lock(&mymutex) == 0){
        rp = value;
        if((pv = strchr(rp, '\n')) != NULL){
          argp->hnum += atoi(rp);
          rp = pv + 1;
        }
        while((pv = strchr(rp, '\n')) != NULL){
          *pv = '\0';
          if((mp = strchr(rp, '\t')) != NULL){
            *mp = '\0';
            est_resmap_add(rmap, rp, atoi(mp + 1), ESTRMLOMAX);
          }
          rp = pv + 1;
        }
        pthread_mutex_unlock(&mymutex);
      }
      free(value);
      if(g_cclife >= 0){
        if(now % 4 == 0 && getpid() % 4 == 0 && est_random() < 0.1){
          unlink(tmppath);
        } else {
          est_utime(tmppath, sbuf.st_mtime + (now - sbuf.st_mtime) / 2);
        }
      }
      g_cache = TRUE;
      return NULL;
    } else {
      unlink(tmppath);
    }
  }
  if(argp->scores && *g_idattr == '\0' && !g_union)
    est_cond_set_narrowing_scores(cond, argp->scores, argp->snum);
  hints = cbmapopenex(MINIBNUM);
  etime = est_gettimeofday();
  res = est_mtdb_search(db, cond, &rnum, hints);
  etime = est_gettimeofday() - etime;
  if((scores = est_cond_scores(cond, &snum)) != NULL && (!argp->scores || snum < argp->snum)){
    argp->scores = scores;
    argp->snum = snum;
  }
  check = (rp = est_cond_phrase(cond)) != NULL && *rp != '\0' && *rp != '[' && *rp != '*' ?
    g_scancheck : 0;
  ofp = *tmppath != '\0' && etime >= CCGENMINTIME ? fopen(tmppath, "w") : NULL;
  num = (rp = cbmapget(hints, "", 0, NULL)) != NULL ? atoi(rp) : 0;
  if(rnum < est_cond_max(cond) && num < rnum) num = rnum;
  argp->hnum += num;
  if(ofp) fprintf(ofp, "%d\n", num);
  if(pthread_mutex_lock(&mymutex) == 0){
    umap = g_dupcheck && g_metamin > 1 ? cbmapopenex(DUPCHKMAX - 1) : NULL;
    if(*g_idattr == '\0'){
      for(i = 0; i < rnum; i++){
        if(check > 0){
          if(!(doc = est_mtdb_get_doc(db, res[i], 0))) continue;
          if(!est_mtdb_scan_doc(db, doc, cond)){
            est_doc_delete(doc);
            continue;
          }
          est_doc_delete(doc);
          check--;
        }
        if((num = est_cond_score(cond, i)) > 0){
          sprintf(numbuf, "%d", num);
          if(!umap || cbmaprnum(umap) > DUPCHKMAX || cbmapput(umap, numbuf, -1, "", 0, FALSE)){
            if(cblistnum(g_idsuflist) > 0){
              expr = caturisuffixes(db, res[i], numbuf);
              uri = expr;
            } else {
              expr = NULL;
              uri = numbuf;
            }
            if(*g_ordexpr == '$'){
              ord = est_mtdb_get_doc_attr(db, res[i], g_ordexpr + 1);
              num = ord ? cbstrmktime(ord) : 0;
              free(ord);
            }
            est_resmap_add(rmap, uri, num, g_score);
            if(ofp) fprintf(ofp, "%s\t%d\n", uri, num);
            free(expr);
          }
        } else if((value = est_mtdb_get_doc_attr(db, res[i], ESTDATTRURI)) != NULL){
          if(!umap || cbmaprnum(umap) > DUPCHKMAX || cbmapput(umap, value, -1, "", 0, FALSE)){
            if(cblistnum(g_idsuflist) > 0){
              expr = caturisuffixes(db, res[i], value);
              uri = expr;
            } else {
              expr = NULL;
              uri = value;
            }
            if(*g_ordexpr == '$'){
              ord = est_mtdb_get_doc_attr(db, res[i], g_ordexpr + 1);
              num = ord ? cbstrmktime(ord) : 0;
              free(ord);
            } else {
              num = atoi(value);
            }
            est_resmap_add(rmap, uri, num, g_score);
            if(ofp) fprintf(ofp, "%s\t%d\n", uri, num);
            free(expr);
          }
          free(value);
        }
      }
    } else {
      for(i = 0; i < rnum; i++){
        if(check > 0){
          if(!(doc = est_mtdb_get_doc(db, res[i], 0))) continue;
          if(!est_mtdb_scan_doc(db, doc, cond)){
            est_doc_delete(doc);
            continue;
          }
          if((rp = est_doc_attr(doc, g_idattr)) != NULL){
            num = est_cond_score(cond, i);
            if(cblistnum(g_idsuflist) > 0){
              expr = caturisuffixes(db, res[i], rp);
              uri = expr;
            } else {
              expr = NULL;
              uri = rp;
            }
            if(*g_ordexpr == '$'){
              ord = est_mtdb_get_doc_attr(db, res[i], g_ordexpr + 1);
              num = ord ? cbstrmktime(ord) : 0;
              free(ord);
            }
            est_resmap_add(rmap, uri, num, g_score);
            if(ofp) fprintf(ofp, "%s\t%d\n", uri, num);
            free(expr);
          }
          est_doc_delete(doc);
          check--;
        } else if((value = est_mtdb_get_doc_attr(db, res[i], g_idattr)) != NULL){
          num = est_cond_score(cond, i);
          if(cblistnum(g_idsuflist) > 0){
            expr = caturisuffixes(db, res[i], value);
            uri = expr;
          } else {
            expr = NULL;
            uri = value;
          }
          if(*g_ordexpr == '$'){
            ord = est_mtdb_get_doc_attr(db, res[i], g_ordexpr + 1);
            num = ord ? cbstrmktime(ord) : 0;
            free(ord);
          } else if(num < 1){
            num = atoi(value);
          }
          est_resmap_add(rmap, uri, num, g_score);
          if(ofp) fprintf(ofp, "%s\t%d\n", uri, num);
          free(expr);
          free(value);
        }
      }
    }
    if(umap) cbmapclose(umap);
    pthread_mutex_unlock(&mymutex);
  }
  if(ofp) fclose(ofp);
  free(res);
  cbmapclose(hints);
  return NULL;
}


/* encode a string for file name */
static char *myencode(ESTCOND *cond){
  const CBLIST *attrs;
  CBDATUM *sbuf;
  const char *phrase, *order;
  char *mp, *rp, *wp, *ep;
  int i;
  sbuf = cbdatumopen(NULL, -1);
  if((phrase = est_cond_phrase(cond)) != NULL) cbdatumprintf(sbuf, "phrase=%s\n", phrase);
  if((attrs = est_cond_attrs(cond)) != NULL){
    for(i = 0; i < cblistnum(attrs); i++){
      cbdatumprintf(sbuf, "attr[%d]=%s\n", i + 1, cblistval(attrs, i, NULL));
    }
  }
  if((order = est_cond_order(cond)) != NULL) cbdatumprintf(sbuf, "order=%s\n", order);
  cbdatumprintf(sbuf, "max=%d\n", est_cond_max(cond));
  cbdatumprintf(sbuf, "options=%d\n", est_cond_options(cond));
  mp = est_make_crypt(cbdatumptr(sbuf));
  rp = mp;
  wp = mp;
  while(*rp != '\0'){
    if(*rp <= '0' && *rp >= '9'){
      *wp = (*rp - '0') * 16;
    } else {
      *wp = (*rp - 'a' + 10) * 16;
    }
    rp++;
    if(*rp <= '0' && *rp >= '9'){
      *wp += (*rp - '0');
    } else {
      *wp += *rp - 'a' + 10;
    }
    if(*rp != '\0') rp++;
    wp++;
  }
  ep = cbbaseencode(mp, wp - mp);
  wp = ep;
  while(*wp != '\0'){
    if(*wp == '/' || *wp == '+'){
      *wp = '_';
    } else if(*wp == '='){
      *wp = '\0';
    }
    wp++;
  }
  free(mp);
  cbdatumclose(sbuf);
  return ep;
}


/* concatenate suffixes after URI */
static char *caturisuffixes(ESTMTDB *db, int id, const char *uri){
  CBDATUM *buf;
  char *value;
  int i;
  buf = cbdatumopen(uri, -1);
  for(i = 0; i < cblistnum(g_idsuflist); i++){
    cbdatumcat(buf, "@", 1);
    if((value = est_mtdb_get_doc_attr(db, id, cblistval(g_idsuflist, i, NULL))) != NULL){
      cbdatumcat(buf, value, -1);
      free(value);
    }
  }
  return cbdatumtomalloc(buf, NULL);
}


/* output the log message */
static void outputlog(void){
  FILE *ofp;
  CBDATUM *condbuf;
  const char *rp, *pv;
  char *name, *value;
  int i;
  if(g_logfile[0] == '\0') return;
  condbuf = cbdatumopen(NULL, -1);
  for(i = 0; i < cblistnum(p_phraselist); i++){
    cbdatumcat(condbuf, "{{p:", -1);
    cbdatumcat(condbuf, cblistval(p_phraselist, i, NULL), -1);
    cbdatumcat(condbuf, "}}", -1);
  }
  for(i = 0; i < cblistnum(p_attrlist); i++){
    cbdatumcat(condbuf, "{{a:", -1);
    cbdatumcat(condbuf, cblistval(p_attrlist, i, NULL), -1);
    cbdatumcat(condbuf, "}}", -1);
  }
  if(cbdatumsize(condbuf) < 1 || !(ofp = fopen(g_logfile, "ab"))){
    cbdatumclose(condbuf);
    return;
  }
  rp = g_logformat;
  while(*rp != '\0'){
    switch(*rp){
    case '\\':
      if(rp[1] != '\0') rp++;
      switch(*rp){
      case 't':
        fputc('\t', ofp);
        break;
      case 'n':
        fputc('\n', ofp);
        break;
      default:
        fputc(*rp, ofp);
        break;
      }
      break;
    case '{':
      if(cbstrfwmatch(rp, "{cond}")){
        pv = cbdatumptr(condbuf);
        while(*pv != '\0'){
          if(*pv > '\0' && *pv < ' '){
            fputc(' ', ofp);
          } else {
            fputc(*pv, ofp);
          }
          pv++;
        }
        rp += 5;
      } else if(cbstrfwmatch(rp, "{time}")){
        value = cbdatestrwww(-1, 0);
        fprintf(ofp, "%s", value);
        free(value);
        rp += 5;
      } else if(cbstrfwmatch(rp, "{hnum}")){
        fprintf(ofp, "%d", g_hnum);
        rp += 5;
      } else if(cbstrfwmatch(rp, "{cache}")){
        fprintf(ofp, "%d", g_cache);
        rp += 6;
      } else if(cbstrfwmatch(rp, "{metamin}")){
        fprintf(ofp, "%d", g_metamin);
        rp += 8;
      } else if(cbstrfwmatch(rp, "{logmsg}")){
        fprintf(ofp, "%s", p_logmsg);
        rp += 7;
      } else if((pv = strchr(rp, '}')) != NULL){
        rp++;
        name = cbmemdup(rp, pv - rp);
        value = getenv(name);
        if(value) fprintf(ofp, "%s", value);
        free(name);
        rp = pv;
      } else {
        fputc(*rp, ofp);
      }
      break;
    default:
      fputc(*rp, ofp);
      break;
    }
    rp++;
  }
  fclose(ofp);
  cbdatumclose(condbuf);
}



/* END OF FILE */
