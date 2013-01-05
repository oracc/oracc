/*************************************************************************************************
 * A union meta searcher of Hyper Estraier
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
#include "estnode.h"
#include "myconf.h"

#define CONFSUFFIX     ".conf"           /* suffix of the configuration file */
#define CACHESUFFIX    ".ecc"            /* suffix of temporary files */
#define NUMBUFSIZ      32                /* size of a buffer for a number */
#define OUTBUFSIZ      262144            /* size of the output buffer */
#define MINIBNUM       31                /* bucket number of map for trivial use */
#define CONDPHRASEMAX  9                 /* maximum number of phrase conditions */
#define CONDATTRMAX    9                 /* maximum number of attribute conditions */
#define SEARCHMAX      10                /* maximum number of shown documents */
#define CCPURGEFREQ    512               /* frequency of purging temporary files */

typedef struct {                         /* type of structure for a search thread */
  const char *url;                       /* target URL */
  CBMAP *rmap;                           /* result object */
  int hnum;                              /* number of corresponding documents */
  int alive;                             /* whether to be alive */
} TARGSRCH;


/* global variables for configurations */
int g_runcnt = 0;                        /* count of running */
const char *g_conffile = NULL;           /* path of the configuration file */
const CBLIST *g_targetlist = NULL;       /* list of real indexes */
int g_stmode = FALSE;                    /* whether to be single thread mode */
int g_score = -1;                        /* scoring method when logical operation */
const char *g_tmpdir = NULL;             /* path of the directory for temporary files */
int g_cclife = 0;                        /* lifetime of cache files */
const CBLIST *g_sharelist = NULL;        /* list of cache sharing hosts */
const char *g_failfile = NULL;           /* path of the fail rate file */
const char *g_logfile = NULL;            /* path of the log file */
const char *g_logformat = NULL;          /* format of the log */


/* global variables for parameters */
const char *p_querykey = "";             /* unique key for the query string */
const char *p_shareurl = "";             /* URL of shared cache */
const CBLIST *p_phraselist = NULL;       /* list of phrase conditions */
const CBLIST *p_attrlist = NULL;         /* list of attribute conditions */
const char *p_distinct = NULL;           /* name of distinction attribute */
int p_max = -1;                          /* number of output documents */
int p_fresh = FALSE;                     /* whether to retrieve fresh result */
int p_stmode = FALSE;                    /* whether to be single thread mode */
const char *p_cache = "";                /* key of read cache */
const char *p_logmsg = "";               /* additional log message */


/* other global variables */
char g_outbuf[OUTBUFSIZ];                /* output buffer */
const char *g_scriptname = NULL;         /* name of the script */
const char *g_scripturl = NULL;          /* URL of the script */
int g_hnum = 0;                          /* number of corresponding documents */
int g_cache = FALSE;                     /* whether cache was used */
int g_error = FALSE;                     /* whether an error has occured */
double g_speed = 0.0;                    /* speed of response */


/* function prototypes */
int main(int argc, char **argv);
static int realmain(int argc, char **argv);
static void showerror(const char *msg);
static const char *skiplabel(const char *str);
static void savecache(const char *key);
static CBMAP *getparameters(void);
static char *myencode(const char *str);
static void showcache(void);
static void showresult(void);
static void *procsearch(void *targ);
static void outputlog(void);


/* main routine */
int main(int argc, char **argv){
#if defined(MYFCGI)
  est_proc_env_reset();
  est_init_net_env();
  while(FCGI_Accept() >= 0){
    g_runcnt++;
    p_querykey = "";
    p_shareurl = "";
    p_phraselist = NULL;
    p_attrlist = NULL;
    p_max = -1;
    p_distinct = NULL;
    p_fresh = FALSE;
    p_stmode = FALSE;
    p_cache = "";
    p_logmsg = "";
    g_scripturl = NULL;
    g_hnum = 0;
    g_cache = FALSE;
    g_error = FALSE;
    g_speed = 0.0;
    realmain(argc, argv);
    fflush(stdout);
  }
  est_free_net_env();
  return 0;
#else
  int rv;
  est_proc_env_reset();
  est_init_net_env();
  rv = realmain(argc, argv);
  est_free_net_env();
  return rv;
#endif
}


/* main routine */
static int realmain(int argc, char **argv){
  CBLIST *lines, *tlist, *slist, *plist, *alist;
  CBMAP *params;
  const char *rp;
  char *tmp, *wp, numbuf[NUMBUFSIZ];
  int i, len;
  double failrate;
  /* set configurations */
  setvbuf(stdout, g_outbuf, _IOFBF, OUTBUFSIZ);
  g_scriptname = argv[0];
  if((rp = getenv("SCRIPT_NAME")) != NULL){
    g_scriptname = rp;
    if((rp = getenv("HTTP_HOST")) != NULL){
      tmp = cbsprintf("http://%s%s", rp, g_scriptname);
      cbglobalgc(tmp, free);
      g_scripturl = tmp;
    }
  }
  if((rp = strrchr(g_scriptname, '/')) != NULL) g_scriptname = rp + 1;
  tmp = cbmalloc(strlen(g_scriptname) + strlen(CONFSUFFIX) + 1);
  sprintf(tmp, "%s", g_scriptname);
  cbglobalgc(tmp, free);
  if(!(wp = strrchr(tmp, '.'))) wp = tmp + strlen(tmp);
  sprintf(wp, "%s", CONFSUFFIX);
  g_conffile = tmp;
  if(!(lines = cbreadlines(g_conffile))) showerror("the configuration file is missing.");
  cbglobalgc(lines, (void (*)(void *))cblistclose);
  tlist = cblistopen();
  cbglobalgc(tlist, (void (*)(void *))cblistclose);
  slist = cblistopen();
  cbglobalgc(slist, (void (*)(void *))cblistclose);
  for(i = 0; i < cblistnum(lines); i++){
    rp = cblistval(lines, i, NULL);
    if(cbstrfwimatch(rp, "targeturl:")){
      rp = skiplabel(rp);
      if(*rp != '\0') cblistpush(tlist, rp, -1);
    } else if(cbstrfwimatch(rp, "stmode:")){
      g_stmode = atoi(skiplabel(rp)) > 0;
    } else if(cbstrfwimatch(rp, "score:")){
      g_score = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "failfile:")){
      g_failfile = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "tmpdir:")){
      g_tmpdir = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "cclife:")){
      g_cclife = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "shareurl:")){
      rp = skiplabel(rp);
      if(*rp != '\0') cblistpush(slist, rp, -1);
    } else if(cbstrfwimatch(rp, "logfile:")){
      g_logfile = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "logformat:")){
      g_logformat = skiplabel(rp);
    }
  }
  if(cblistnum(tlist) < 1) showerror("targeturl is undefined.");
  g_targetlist = tlist;
  if(g_score < 0) showerror("score is undefined.");
  if(!g_failfile) showerror("failfile is undefined.");
  if(!g_tmpdir) showerror("tmpdir is undefined.");
  g_sharelist = slist;
  if(!g_logfile) showerror("logfile is undefined.");
  if(!g_logformat) showerror("logformat is undefined.");
  if(*g_failfile != '\0' && (tmp = cbreadfile(g_failfile, NULL)) != NULL){
    failrate = strtod(tmp, NULL);
    free(tmp);
  } else {
    failrate = 0.0;
  }
  if(failrate > 0.0 && failrate > est_random()){
    /* show failure */
    printf("Status: 503 Service Unavailable\r\n");
    printf("Content-Type: text/plain\r\n");
    printf("Cache-Control: no-cache, must-revalidate, no-transform\r\n");
    printf("Pragma: no-cache\r\n");
    printf("\r\n");
    printf("The clients are too busy now (%f).\n", failrate);
  } else if(*g_tmpdir != '\0' && (rp = getenv("PATH_INFO")) != NULL && *rp == '/' &&
            (++rp) != '\0' && !strchr(rp, '/') && !strchr(rp, '\\') &&
            strcmp(rp, ESTCDIRSTR) && strcmp(rp, ESTPDIRSTR)){
    /* save cache data */
    savecache(rp);
  } else {
    /* read parameters */
    params = getparameters();
    cbglobalgc(params, (void (*)(void *))cbmapclose);
    plist = cblistopen();
    cbglobalgc(plist, (void (*)(void *))cblistclose);
    alist = cblistopen();
    cbglobalgc(alist, (void (*)(void *))cblistclose);
    for(i = 1; i <= CONDPHRASEMAX; i++){
      len = sprintf(numbuf, "phrase%d", i);
      rp = cbmapget(params, numbuf, len, NULL);
      if(!rp && i == 1) rp = cbmapget(params, "phrase", -1, NULL);
      if(!rp) rp = "";
      while(*rp == ' ' || *rp == '\t'){
        rp++;
      }
      cblistpush(plist, rp, -1);
    }
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
    if(!(p_cache = cbmapget(params, "cache", -1, NULL))) p_cache = "";
    if((rp = cbmapget(params, "fresh", -1, NULL)) != NULL) p_fresh = atoi(rp) > 0;
    if((rp = cbmapget(params, "stmode", -1, NULL)) != NULL) p_stmode = atoi(rp) > 0;
    if(!(p_logmsg = cbmapget(params, "logmsg", -1, NULL))) p_logmsg = "";
    p_phraselist = plist;
    p_attrlist = alist;
    if(*p_cache != '\0' && *g_tmpdir != '\0'){
      /* show the cache */
      showcache();
    } else {
      /* show the result */
      showresult();
      /* output the log message */
      outputlog();
    }
  }
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


/* save cache data */
static void savecache(const char *key){
  int maxlen = 1024 * 1024 * 32;
  const char *rp;
  char *buf, tmppath[PATH_MAX];
  int i, len, c;
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
  if(*g_tmpdir == '@'){
    sprintf(tmppath, "%s%c%02x%c%s%s", g_tmpdir + 1, ESTPATHCHR,
            dpinnerhash(key, -1) % 0x100, ESTPATHCHR, key, CACHESUFFIX);
  } else {
    sprintf(tmppath, "%s%c%s%s", g_tmpdir, ESTPATHCHR, key, CACHESUFFIX);
  }
  if(buf && cbwritefile(tmppath, buf, len)){
    printf("Content-Type: text/plain\r\n");
    printf("Cache-Control: no-cache, must-revalidate, no-transform\r\n");
    printf("Pragma: no-cache\r\n");
    printf("\r\n");
    printf("saved a cache file as \"%s\"\n", key);
  } else {
    printf("Status: 400 Bad Request\r\n");
    printf("Content-Type: text/plain\r\n");
    printf("Cache-Control: no-cache, must-revalidate, no-transform\r\n");
    printf("Pragma: no-cache\r\n");
    printf("\r\n");
    printf("invalid parameter\n");
  }
  free(buf);
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
    key = myencode(buf);
    cbglobalgc(key, free);
    p_querykey = key;
    if(cblistnum(g_sharelist) > 0) p_shareurl =
      cblistval(g_sharelist, dpouterhash(p_querykey, -1) % cblistnum(g_sharelist), NULL);
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


/* encode a string for file name */
static char *myencode(const char *str){
  char *mp, *rp, *wp, *ep;
  mp = est_make_crypt(str);
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
  return ep;
}


/* show the cache */
static void showcache(void){
  char tmppath[PATH_MAX], *str;
  int len;
  struct stat sbuf;
  if(*g_tmpdir == '@'){
    sprintf(tmppath, "%s%c%02x%c%s%s", g_tmpdir + 1, ESTPATHCHR,
            dpinnerhash(p_cache, -1) % 0x100, ESTPATHCHR, p_cache, CACHESUFFIX);
  } else {
    sprintf(tmppath, "%s%c%s%s", g_tmpdir, ESTPATHCHR, p_cache, CACHESUFFIX);
  }
  if(stat(tmppath, &sbuf) != -1){
    if(g_cclife < 0 || time(NULL) - sbuf.st_mtime <= g_cclife){
      if((str = cbreadfile(tmppath, &len)) != NULL){
        printf("Content-Type: text/plain\r\n");
        printf("Content-Length: %d\r\n", len);
        printf("Cache-Control: no-cache, must-revalidate, no-transform\r\n");
        printf("Pragma: no-cache\r\n");
        printf("\r\n");
        fwrite(str, 1, len, stdout);
        free(str);
        return;
      }
    } else {
      unlink(tmppath);
    }
  }
  printf("Status: 404 Not Found\r\n");
  printf("Content-Type: text/plain\r\n");
  printf("Cache-Control: no-cache, must-revalidate, no-transform\r\n");
  printf("Pragma: no-cache\r\n");
  printf("\r\n");
  printf("the cache was not found\n");
}


/* show the result */
static void showresult(void){
  pthread_t *thlist;
  TARGSRCH *arglist;
  ESTRESMAPELEM *elems;
  CBMAP *rmap;
  CBLIST *list;
  CBDATUM *obuf;
  const char *rp;
  char tmppath[PATH_MAX], *str;
  int i, len, code, hnum, num;
  struct stat sbuf;
  time_t now;
  printf("Content-Type: text/plain\r\n");
  printf("Cache-Control: no-cache, must-revalidate, no-transform\r\n");
  printf("Pragma: no-cache\r\n");
  printf("X-Run-Count: %d\r\n", g_runcnt);
  printf("\r\n");
  now = time(NULL);
  if(*g_tmpdir != '\0' && *g_tmpdir != '@' && g_cclife >= 0 &&
     (now + g_runcnt) % CCPURGEFREQ == 0 && (list = cbdirlist(g_tmpdir)) != NULL){
    for(i = 0; i < cblistnum(list); i++){
      rp = cblistval(list, i, NULL);
      if(!cbstrbwmatch(rp, CACHESUFFIX)) continue;
      sprintf(tmppath, "%s%c%s", g_tmpdir, ESTPATHCHR, rp);
      if(stat(tmppath, &sbuf) != -1 && now - sbuf.st_mtime > g_cclife) unlink(tmppath);
    }
    cblistclose(list);
  }
  if(*g_tmpdir != '\0' && !p_fresh && *p_querykey != '\0'){
    if(*g_tmpdir == '@'){
      num = dpinnerhash(p_querykey, -1) % 0x100;
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
              g_tmpdir + 1, ESTPATHCHR, num, ESTPATHCHR, p_querykey, CACHESUFFIX);
    } else {
      sprintf(tmppath, "%s%c%s%s", g_tmpdir, ESTPATHCHR, p_querykey, CACHESUFFIX);
    }
  } else {
    *tmppath = '\0';
  }
  if(*tmppath != '\0' && stat(tmppath, &sbuf) != -1){
    if((g_cclife < 0 || now - sbuf.st_mtime <= g_cclife) &&
       (str = cbreadfile(tmppath, &len)) != NULL){
      g_hnum = atoi(str);
      fwrite(str, 1, len, stdout);
      if(g_cclife >= 0){
        if(now % 4 == 0 && getpid() % 4 == 0 && est_random() < 0.1){
          unlink(tmppath);
        } else {
          est_utime(tmppath, sbuf.st_mtime + (now - sbuf.st_mtime) / 2);
        }
      }
      free(str);
      g_cache = TRUE;
      return;
    } else {
      unlink(tmppath);
    }
  } else if(!p_fresh && *p_querykey != '\0' && *p_shareurl != '\0' &&
            strcmp(p_shareurl, g_scripturl)){
    str = cbsprintf("%s?cache=%s", p_shareurl, p_querykey);
    obuf = cbdatumopen(NULL, -1);
    if(est_url_shuttle(str, NULL, -1, -1, -1, NULL, NULL, NULL, -1, &code, NULL, obuf) &&
       code == 200){
      g_hnum = atoi(cbdatumptr(obuf));
      fwrite(cbdatumptr(obuf), 1, cbdatumsize(obuf), stdout);
      if(*tmppath != '\0') cbwritefile(tmppath, cbdatumptr(obuf), cbdatumsize(obuf));
      cbdatumclose(obuf);
      free(str);
      return;
    }
    cbdatumclose(obuf);
    free(str);
  }
  rmap = cbmapopen();
  thlist = cbmalloc(cblistnum(g_targetlist) * sizeof(pthread_t));
  arglist = cbmalloc(cblistnum(g_targetlist) * sizeof(TARGSRCH));
  for(i = 0; i < cblistnum(g_targetlist); i++){
    arglist[i].url = cblistval(g_targetlist, i, NULL);
    arglist[i].rmap = rmap;
    arglist[i].hnum = 0;
  }
  g_speed = est_gettimeofday();
  for(i = 0; i < cblistnum(g_targetlist); i++){
    if(g_stmode || p_stmode){
      arglist[i].alive = FALSE;
      procsearch(arglist + i);
    } else {
      arglist[i].alive = TRUE;
      if(pthread_create(thlist + i, NULL, procsearch, arglist + i) != 0)
        arglist[i].alive = FALSE;
    }
  }
  hnum = 0;
  for(i = 0; i < cblistnum(g_targetlist); i++){
    if(arglist[i].alive) pthread_join(thlist[i], NULL);
    hnum += arglist[i].hnum;
  }
  g_speed = est_gettimeofday() - g_speed;
  free(arglist);
  free(thlist);
  elems = est_resmap_dump(rmap, 1, &num);
  if(num < p_max + 1 && num < hnum) hnum = num;
  g_hnum = hnum;
  obuf = cbdatumopen(NULL, -1);
  cbdatumprintf(obuf, "%d\n", g_hnum);
  for(i = 0; i < num && i < p_max; i++){
    cbdatumprintf(obuf, "%s\t%d\n", elems[i].key, elems[i].score);
  }
  fwrite(cbdatumptr(obuf), 1, cbdatumsize(obuf), stdout);
  if(*p_querykey != '\0' && !g_error){
    if(*p_shareurl != '\0'){
      str = cbsprintf("%s/%s", p_shareurl, p_querykey);
      est_url_shuttle(str, NULL, -1, -1, -1, NULL, NULL,
                      cbdatumptr(obuf), cbdatumsize(obuf), NULL, NULL, NULL);
      free(str);
    } else {
      cbwritefile(tmppath, cbdatumptr(obuf), cbdatumsize(obuf));
    }
  }
  cbdatumclose(obuf);
  free(elems);
  cbmapclose(rmap);
}


/* search an index */
static void *procsearch(void *targ){
  static pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
  TARGSRCH *argp;
  CBMAP *rmap;
  CBLIST *fields;
  CBDATUM *url, *body;
  const char *rp, *pv, *key;
  int i, code, num, rnum;
  argp = (TARGSRCH *)targ;
  url = cbdatumopen(argp->url, -1);
  rmap = argp->rmap;
  cbdatumprintf(url, "?", -1);
  for(i = 0; i < cblistnum(p_phraselist); i++){
    rp = cblistval(p_phraselist, i, NULL);
    if(*rp != '\0') cbdatumprintf(url, "phrase%d=%?&", i + 1, rp);
  }
  for(i = 0; i < cblistnum(p_attrlist); i++){
    rp = cblistval(p_attrlist, i, NULL);
    if(*rp != '\0') cbdatumprintf(url, "attr%d=%?&", i + 1, rp);
  }
  cbdatumprintf(url, "max=%d", p_max + 1);
  if(*p_distinct != '\0') cbdatumprintf(url, "&distinct=%?", p_distinct);
  if(p_fresh) cbdatumprintf(url, "&fresh=1");
  if(p_stmode) cbdatumprintf(url, "&stmode=1");
  if(*p_logmsg) cbdatumprintf(url, "&logmsg=%?", p_logmsg);
  body = cbdatumopen(NULL, -1);
  if(est_url_shuttle(cbdatumptr(url), NULL, -1, -1, -1, NULL, NULL, NULL, -1,
                     &code, NULL, body) && code == 200){
    rp = cbdatumptr(body);
    num = atoi(rp);
    if(num > 0 && (rp = strchr(rp, '\n')) != NULL){
      argp->hnum = num;
      if(pthread_mutex_lock(&mymutex) == 0){
        rnum = 0;
        while((pv = strchr(rp, '\n')) != NULL){
          fields = cbsplit(rp, pv - rp, "\t");
          if(cblistnum(fields) > 1){
            key = cblistval(fields, 0, NULL);
            num = atoi(cblistval(fields, 1, NULL));
            if(*key != '\0'){
              est_resmap_add(rmap, key, num, g_score);
              rnum++;
            }
          }
          cblistclose(fields);
          rp = pv + 1;
        }
        if(rnum < p_max + 1 && rnum < argp->hnum) argp->hnum = rnum;
        pthread_mutex_unlock(&mymutex);
      }
    }
  } else {
    g_error = TRUE;
  }
  cbdatumclose(body);
  cbdatumclose(url);
  return NULL;
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
      } else if(cbstrfwmatch(rp, "{error}")){
        fprintf(ofp, "%d", g_error);
        rp += 6;
      } else if(cbstrfwmatch(rp, "{speed}")){
        fprintf(ofp, "%.3f", g_speed / 1000);
        rp += 6;
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
