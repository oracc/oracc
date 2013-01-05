/*************************************************************************************************
 * The pseudo master of node servers
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
#include "mastermod.h"

#define CONFSUFFIX     ".conf"           /* suffix of the configuration file */
#define OUTBUFSIZ      262144            /* size of the output buffer */
#define PPOUTDOC       "_OUTDOC_"        /* pseudo parameter to delete a document */
#define PPPOSTCMD      "_POSTCMD_"       /* pseudo parameter of the post command */


/* global variables for configurations */
const char *g_conffile = NULL;           /* path of the configuration file */
const char *g_indexdir = NULL;           /* path of the directory containing indexes */
int g_runmode = 0;                       /* running mode */
const char *g_pidxsuffix = NULL;         /* suffix of pseudo indexes */
int g_pidxdocmax = -1;                   /* maximum number of documents in each pseudo index */
int g_pidxdocmin = -1;                   /* minimum number of documents in each pseudo index */
int g_lockindex = FALSE;                 /* whether to perform file locking to the database */
int g_searchmax = 0;                     /* maximum number of documents to send */
int g_rateuri = FALSE;                   /* whether to rate URI for scoring */
int g_mergemethod = -1;                  /* merge method of meta search */
int g_scoreexpr = -1;                    /* score expression */
int g_wildmax = -1;                      /* maximum number of expansion of wild cards */
int g_snipwwidth = -1;                   /* whole width of the snippet */
int g_sniphwidth = -1;                   /* width of beginning of the text */
int g_snipawidth = -1;                   /* width around each highlighted word */
int g_scancheck = FALSE;                 /* whether to check documents by scanning */
int g_smlrvnum = -1;                     /* number of keywords for similarity search */
int g_extdelay = 0;                      /* number of documents for delay of extraction */


/* other global variables */
char g_outbuf[OUTBUFSIZ];                /* output buffer */
const char *g_scriptname = NULL;         /* name of the script */


/* function prototypes */
int main(int argc, char **argv);
static int realmain(int argc, char **argv);
static void die(const char *msg);
static const char *skiplabel(const char *str);
static CBMAP *getparameters(void);
static void senderror(int code, const char *msg);
static void sendnodecmdinform(ESTMTDB *db, const char *myurl, const char *mylabel,
                              CBMAP *params);
static void sendnodecmdcacheusage(ESTMTDB *db, const char *myurl, const char *mylabel,
                                  CBMAP *params);
static void sendnodecmdsearch(ESTMTDB *db, const char *myurl, const char *mylabel,
                              CBMAP *params);
static void sendnodecmdgetdoc(ESTMTDB *db, const char *myurl, const char *mylabel,
                              CBMAP *params);
static void sendnodecmdgetdocattr(ESTMTDB *db, const char *myurl, const char *mylabel,
                                  CBMAP *params);
static void sendnodecmdetchdoc(ESTMTDB *db, const char *myurl, const char *mylabel,
                               CBMAP *params);
static void sendnodecmduritoid(ESTMTDB *db, const char *myurl, const char *mylabel,
                               CBMAP *params);
static void sendnodecmdputdoc(ESTMTDB *db, const char *myurl, const char *mylabel,
                              CBMAP *params);
static void sendnodecmdoutdoc(ESTMTDB *db, const char *myurl, const char *mylabel,
                              CBMAP *params);
static void sendnodecmdeditdoc(ESTMTDB *db, const char *myurl, const char *mylabel,
                               CBMAP *params);
static void sendnodecmdsync(ESTMTDB *db, const char *myurl, const char *mylabel,
                            CBMAP *params);
static void sendnodecmdoptimize(ESTMTDB *db, const char *myurl, const char *mylabel,
                                CBMAP *params);
static void procpostcmdoutdoc(const char *idxname, const char *uri);
static void procpostcmdsync(const char *idxname);
static void procpostcmdoptimize(const char *idxname);


/* main routine */
int main(int argc, char **argv){
#if defined(MYFCGI)
  static int cnt = 0;
  est_proc_env_reset();
  while(FCGI_Accept() >= 0){
    realmain(argc, argv);
    fflush(stdout);
    if(++cnt >= 64) exit(0);
  }
  return 0;
#else
  est_proc_env_reset();
  return realmain(argc, argv);
#endif
}


/* real main routine */
static int realmain(int argc, char **argv){
  ESTMTDB *db;
  CBLIST *lines;
  CBMAP *params;
  CBDATUM *myurl;
  const char *rp, *idxname, *cmd;
  char *tmp, *wp;
  int i, omode, ecode;
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
  if(!(lines = cbreadlines(g_conffile))) die("the configuration file is missing.");
  cbglobalgc(lines, (void (*)(void *))cblistclose);
  for(i = 0; i < cblistnum(lines); i++){
    rp = cblistval(lines, i, NULL);
    if(cbstrfwimatch(rp, "indexdir:")){
      g_indexdir = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "runmode:")){
      g_runmode = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "pidxsuffix:")){
      g_pidxsuffix = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "pidxdocmax:")){
      g_pidxdocmax = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "pidxdocmin:")){
      g_pidxdocmin = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "lockindex:")){
      g_lockindex = atoi(skiplabel(rp)) > 0;
    } else if(cbstrfwimatch(rp, "searchmax:")){
      g_searchmax = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "rateuri:")){
      g_rateuri = atoi(skiplabel(rp)) > 0;
    } else if(cbstrfwimatch(rp, "mergemethod:")){
      g_mergemethod = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "scoreexpr:")){
      g_scoreexpr = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "wildmax:")){
      g_wildmax = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "snipwwidth:")){
      g_snipwwidth = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "sniphwidth:")){
      g_sniphwidth = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "snipawidth:")){
      g_snipawidth = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "scancheck:")){
      g_scancheck = atoi(skiplabel(rp)) > 0;
    } else if(cbstrfwimatch(rp, "smlrvnum:")){
      g_smlrvnum = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "extdelay:")){
      g_extdelay = atoi(skiplabel(rp));
    }
  }
  if(!g_indexdir) die("indexdir is undefined.");
  if(g_runmode < RM_NORMAL || g_runmode > RM_RDONLY) die("runmode is undefined.");
  if(!g_pidxsuffix) die("pidxsuffix is undefined.");
  if(g_pidxdocmax < 0) die("pidxdocmax is undefined.");
  if(g_pidxdocmin < 0) die("pidxdocmin is undefined.");
  if(g_searchmax < 0) die("searchmax is undefined.");
  if(g_rateuri < 0) die("rateuri is undefined.");
  if(g_mergemethod < 0) die("mergemethod is undefined.");
  if(g_scoreexpr < 0) die("scoreexpr is undefined.");
  if(g_wildmax < 0) die("wildmax is undefined.");
  if(g_snipwwidth < 0) die("snipwwidth is undefined.");
  if(g_sniphwidth < 0) die("sniphwidth is undefined.");
  if(g_snipawidth < 0) die("snipawidth is undefined.");
  if(g_smlrvnum < 0) die("smlrvnum is undefined.");
  /* read parameters */
  if(!(idxname = getenv("PATH_INFO"))) idxname = "";
  if(*idxname == '/') idxname++;
  if((rp = strrchr(idxname, '/')) != NULL){
    tmp = cbmemdup(idxname, rp - idxname);
    idxname = tmp;
    cbglobalgc(tmp, free);
    if(strstr(idxname, ESTPATHSTR ESTCDIRSTR ESTPATHSTR) ||
       strstr(idxname, ESTPATHSTR ESTPDIRSTR ESTPATHSTR)) idxname = "";
    cmd = rp + 1;
  } else {
    cmd = "";
  }
  params = getparameters();
  cbglobalgc(params, (void (*)(void *))cbmapclose);
  cbmapout(params, PPOUTDOC, -1);
  cbmapout(params, PPPOSTCMD, -1);
  myurl = cbdatumopen(NULL, -1);
  if((rp = getenv("HTTP_HOST")) != NULL) cbdatumprintf(myurl, "http://%s", rp);
  if((rp = getenv("SCRIPT_NAME")) != NULL) cbdatumprintf(myurl, "%s", rp);
  cbdatumprintf(myurl, "/%s", idxname);
  /* open the database */
  tmp = cbsprintf("%s%c%s", g_indexdir, ESTPATHCHR, idxname);
  omode = ESTDBREADER;
  if(!g_lockindex && strcmp(cmd, "put_doc") && strcmp(cmd, "out_doc")) omode |= ESTDBNOLCK;
  if((db = est_mtdb_open(tmp, omode, &ecode)) != NULL && *g_pidxsuffix != '\0'){
    free(tmp);
    tmp = cbsprintf("%s%c%s%s", g_indexdir, ESTPATHCHR, idxname, g_pidxsuffix);
    est_mtdb_add_pseudo_index(db, tmp);
  }
  free(tmp);
  /* send the result */
  if(!db){
    senderror(404, "Not Found (the node does not exist)");
  } else if(!strcmp(cmd, "inform")){
    sendnodecmdinform(db, cbdatumptr(myurl), idxname, params);
  } else if(!strcmp(cmd, "cacheusage")){
    sendnodecmdcacheusage(db, cbdatumptr(myurl), idxname, params);
  } else if(!strcmp(cmd, "search")){
    sendnodecmdsearch(db, cbdatumptr(myurl), idxname, params);
  } else if(!strcmp(cmd, "get_doc")){
    sendnodecmdgetdoc(db, cbdatumptr(myurl), idxname, params);
  } else if(!strcmp(cmd, "get_doc_attr")){
    sendnodecmdgetdocattr(db, cbdatumptr(myurl), idxname, params);
  } else if(!strcmp(cmd, "etch_doc")){
    sendnodecmdetchdoc(db, cbdatumptr(myurl), idxname, params);
  } else if(!strcmp(cmd, "uri_to_id")){
    sendnodecmduritoid(db, cbdatumptr(myurl), idxname, params);
  } else if(!strcmp(cmd, "put_doc")){
    if(g_runmode == RM_RDONLY){
      senderror(503, "Service Unavailable (read only)");
    } else {
      sendnodecmdputdoc(db, cbdatumptr(myurl), idxname, params);
    }
  } else if(!strcmp(cmd, "out_doc")){
    if(g_runmode == RM_RDONLY){
      senderror(503, "Service Unavailable (read only)");
    } else {
      sendnodecmdoutdoc(db, cbdatumptr(myurl), idxname, params);
    }
  } else if(!strcmp(cmd, "edit_doc")){
    if(g_runmode == RM_RDONLY){
      senderror(503, "Service Unavailable (read only)");
    } else {
      sendnodecmdeditdoc(db, cbdatumptr(myurl), idxname, params);
    }
  } else if(!strcmp(cmd, "sync")){
    if(g_runmode == RM_RDONLY){
      senderror(503, "Service Unavailable (read only)");
    } else {
      sendnodecmdsync(db, cbdatumptr(myurl), idxname, params);
    }
  } else if(!strcmp(cmd, "optimize")){
    if(g_runmode == RM_RDONLY){
      senderror(503, "Service Unavailable (read only)");
    } else {
      sendnodecmdoptimize(db, cbdatumptr(myurl), idxname, params);
    }
  } else if(!strcmp(cmd, "_set_user") || !strcmp(cmd, "_set_link")){
    senderror(501, "Not Implemented (editing meta data is not supported)");
  } else {
    senderror(400, "Bad Request (the command is invalid)");
  }
  /* release resources */
  if(db) est_mtdb_close(db, &ecode);
  cbdatumclose(myurl);
  /* perform the post commands */
  if((cmd = cbmapget(params, PPOUTDOC, -1, NULL)) != NULL) procpostcmdoutdoc(idxname, cmd);
  if((cmd = cbmapget(params, PPPOSTCMD, -1, NULL)) != NULL){
    if(!strcmp(cmd, "sync")){
      procpostcmdsync(idxname);
    } else if(!strcmp(cmd, "optimize")){
      procpostcmdoptimize(idxname);
    }
  }
  return 0;
}


/* show the error page and exit */
static void die(const char *msg){
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
  const char *type, *rp, *body;
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
    if(!(type = getenv("CONTENT_TYPE"))) type = "text/plain";
    if(cbstrfwmatch(type, "multipart/form-data") && (rp = strstr(rp, "boundary=")) != NULL){
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
    } else if(cbstrfwmatch(type, ESTDRAFTTYPE)){
      cbmapput(map, "draft", -1, buf, -1, FALSE);
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


/* send the error page */
static void senderror(int code, const char *msg){
  printf("Status: %d %s\r\n", code, msg);
  printf("Content-Type: text/plain; charset=UTF-8\r\n");
  printf("\r\n");
  printf("%s\n", msg);
}


/* send the result of the inform command */
static void sendnodecmdinform(ESTMTDB *db, const char *myurl, const char *mylabel, CBMAP *params){
  printf("Content-Type: %s; charset=UTF-8\r\n", ESTINFORMTYPE);
  printf("\r\n");
  printf("%s\t%s\t%d\t%d\t%.0f\n",
         myurl, mylabel, est_mtdb_doc_num(db) + est_mtdb_pseudo_doc_num(db),
         est_mtdb_word_num(db), est_mtdb_size(db));
  printf("\n");
  printf("dummy-admin\n");
  printf("\n");
  printf("dummy-user\n");
  printf("\n");
}


/* send the result of the cacheusage command */
static void sendnodecmdcacheusage(ESTMTDB *db, const char *myurl, const char *mylabel,
                                  CBMAP *params){
  printf("Content-Type: text/plain; charset=UTF-8\r\n");
  printf("\r\n");
  printf("%0.6f\n", 0.0);
}


/* send the result of the search command */
static void sendnodecmdsearch(ESTMTDB *db, const char *myurl, const char *mylabel,
                              CBMAP *params){
  RESMAP *resmap;
  RESDOC **resdocs, *resdoc;
  ESTCOND *cond;
  CBMAP *hints, *kwords;
  const CBLIST *texts;
  CBLIST *attrs, *words, *list;
  ESTDOC *doc;
  const char *rp, *phrase, *order, *distinct, *bordstr, *kbuf, *vbuf;
  char name[NUMBUFSIZ], *snippet;
  int i, j, len, max, options, auxiliary, wwidth, hwidth, awidth, skip, lmax, rateuri;
  int *res, rnum, hnum, miss, cnt, score, down, dnum, wnum, end, num, id;
  double curtime, itime, weight, fsiz;
  if(!(phrase = cbmapget(params, "phrase", -1, NULL))) phrase = "";
  while(*phrase == ' ' || *phrase == '\t'){
    phrase++;
  }
  attrs = cblistopen();
  cbglobalgc(attrs, (void (*)(void *))cblistclose);
  if((rp = cbmapget(params, "attr", -1, NULL)) != NULL){
    while(*rp == ' ' || *rp == '\t'){
      rp++;
    }
    if(*rp != '\0') cblistpush(attrs, rp, -1);
  }
  for(i = 0; i <= CONDATTRMAX; i++){
    len = sprintf(name, "attr%d", i);
    if((rp = cbmapget(params, name, len, NULL)) != NULL){
      while(*rp == ' ' || *rp == '\t'){
        rp++;
      }
      if(*rp != '\0') cblistpush(attrs, rp, -1);
    }
  }
  if(!(order = cbmapget(params, "order", -1, NULL))) order = "";
  while(*order == ' ' || *order == '\t'){
    order++;
  }
  max = -1;
  if((rp = cbmapget(params, "max", -1, NULL)) != NULL) max = atoi(rp);
  if(max < 1) max = DEFMAXSRCH;
  if(max > g_searchmax) max = g_searchmax;
  options = -1;
  if((rp = cbmapget(params, "options", -1, NULL)) != NULL) options = atoi(rp);
  auxiliary = INT_MIN;
  if((rp = cbmapget(params, "auxiliary", -1, NULL)) != NULL) auxiliary = atoi(rp);
  if(!(distinct = cbmapget(params, "distinct", -1, NULL))) distinct = "";
  while(*distinct == ' ' || *distinct == '\t'){
    distinct++;
  }
  wwidth = -1;
  if((rp = cbmapget(params, "wwidth", -1, NULL)) != NULL) wwidth = atoi(rp);
  if(wwidth < 0) wwidth = g_snipwwidth;
  hwidth = -1;
  if((rp = cbmapget(params, "hwidth", -1, NULL)) != NULL) hwidth = atoi(rp);
  if(hwidth < 0) hwidth = g_snipwwidth;
  awidth = -1;
  if((rp = cbmapget(params, "awidth", -1, NULL)) != NULL) awidth = atoi(rp);
  if(awidth < 0) awidth = g_snipawidth;
  skip = -1;
  if((rp = cbmapget(params, "skip", -1, NULL)) != NULL) skip = atoi(rp);
  if(skip < 0) skip = 0;
  bordstr = est_border_str();
  cond = est_cond_new();
  if(*phrase != '\0') est_cond_set_phrase(cond, phrase);
  for(i = 0; i < cblistnum(attrs); i++){
    est_cond_add_attr(cond, cblistval(attrs, i, NULL));
  }
  if(*order != '\0') est_cond_set_order(cond, order);
  if(options > 0) est_cond_set_options(cond, options);
  est_cond_set_options(cond, ESTCONDSCFB);
  if(auxiliary != INT_MIN) est_cond_set_auxiliary(cond, auxiliary);
  if(*distinct != '\0') est_cond_set_distinct(cond, distinct);
  resmap = resmap_new();
  hints = cbmapopenex(MINIBNUM);
  curtime = est_gettimeofday();
  lmax = max + skip + 1;
  res = est_mtdb_search(db, cond, &rnum, hints);
  hnum = (vbuf = cbmapget(hints, "", 0, NULL)) ? atoi(vbuf) : rnum;
  if(max >= 0 && hnum < max + 1 && est_cond_auxiliary_word(cond, "")){
    free(res);
    est_cond_set_auxiliary(cond, -1);
    res = est_mtdb_search(db, cond, &rnum, hints);
    hnum = (vbuf = cbmapget(hints, "", 0, NULL)) ? atoi(vbuf) : rnum;
  }
  miss = 0;
  itime = est_gettimeofday() - curtime;
  cnt = 0;
  rateuri = g_rateuri && !cbstrfwmatch(phrase, ESTOPSIMILAR);
  for(i = 0; i < rnum && cnt < lmax; i++){
    if(!(doc = est_mtdb_get_doc(db, res[i], 0))) continue;
    if(g_scancheck && *phrase != '\0' && *phrase != '[' && *phrase != '*' &&
       res[i] < ESTPDOCIDMIN && !est_mtdb_scan_doc(db, doc, cond)){
      est_doc_delete(doc);
      miss++;
      continue;
    }
    score = est_cond_score(cond, i);
    if(rateuri && g_scoreexpr != SE_ASIS){
      if((vbuf = est_doc_attr(doc, ESTDATTRURI)) != NULL){
        if(score < 100) score = 100;
        down = 4;
        if(cbstrfwimatch(vbuf, "file://")){
          vbuf += 7;
        } else if(cbstrfwimatch(vbuf, "ftp://")){
          vbuf += 6;
        } else if(cbstrfwimatch(vbuf, "http://")){
          vbuf += 7;
        } else if(cbstrfwimatch(vbuf, "https://")){
          vbuf += 8;
        } else {
          down += 3;
        }
        while(vbuf[0] != '\0'){
          if(vbuf[0] == '?' || vbuf[0] == '#'){
            down++;
            break;
          }
          if(vbuf[0] == '/' && vbuf[1] != '\0') down++;
          vbuf++;
        }
        score *= 8.0 / (double)down;
      } else {
        score = 0;
      }
    }
    est_doc_add_attr(doc, DATTRNDURL, myurl);
    est_doc_add_attr(doc, DATTRNDLABEL, mylabel);
    if(score >= 0){
      sprintf(name, "%d", score);
      est_doc_add_attr(doc, DATTRNDSCORE, name);
    }
    if(g_scoreexpr != SE_ASIS){
      weight = 1.0;
      if((vbuf = est_doc_attr(doc, ESTDATTRWEIGHT)) != NULL){
        weight = strtod(vbuf, NULL);
        weight = weight >= 0.01 ? weight : 0.01;
      }
      weight /= 10.0;
      switch(g_mergemethod){
      case MM_SCORE:
        score = score * weight;
        break;
      case MM_SCRK:
        score = score * (max * 2 - cnt) * weight;
        break;
      case MM_RANK:
        score = SELFCREDIT * (max - cnt);
        break;
      }
    }
    resmap_put(resmap, score, doc, NULL, NULL);
    cnt++;
  }
  free(res);
  words = est_hints_to_words(hints);
  dnum = est_mtdb_doc_num(db) + est_mtdb_pseudo_doc_num(db);
  wnum = est_mtdb_word_num(db);
  fsiz = est_mtdb_size(db);
  end = max + skip;
  curtime = est_gettimeofday() - curtime;
  printf("Content-Type: %s; charset=UTF-8\r\n", ESTSEARCHTYPE);
  printf("\r\n");
  printf("%s\n", bordstr);
  printf("VERSION\t%s\n", _EST_PROTVER);
  printf("NODE\t%s\n", myurl);
  printf("HIT\t%d\n", hnum - miss);
  cbmapiterinit(hints);
  num = 1;
  while((kbuf = cbmapiternext(hints, NULL)) != NULL){
    if(*kbuf == '\0') continue;
    printf("HINT#%d\t%s\t%s\n", num, kbuf, cbmapiterval(kbuf, NULL));
    num++;
  }
  printf("DOCNUM\t%d\n", dnum);
  printf("WORDNUM\t%d\n", wnum);
  printf("TIME\t%.6f\n", curtime / 1000.0);
  printf("TIME#i\t%.6f\n", itime / 1000.0);
  printf("TIME#0\t%.6f\n", curtime / 1000.0);
  printf("LINK#0\t%s\t", myurl);
  printf("%s\t%d\t%d\t%d\t%.0f\t%d\n", mylabel, SELFCREDIT, dnum, wnum, fsiz, hnum - miss);
  printf("VIEW\tSNIPPET\n");
  printf("\n");
  if(*order == '\0' && cbstrfwmatch(phrase, ESTOPSIMILAR)) order = DATTRNDSCORE " " ESTORDNUMD;
  resdocs = resmap_list(resmap, &rnum, order, NULL);
  for(i = skip; i < rnum && i < end; i++){
    resdoc = resdocs[i];
    if(!resdoc->doc) continue;
    printf("%s\n", bordstr);
    list = est_doc_attr_names(resdoc->doc);
    for(j = 0; j < cblistnum(list); j++){
      vbuf = cblistval(list, j, NULL);
      printf("%s=%s\n", vbuf, est_doc_attr(resdoc->doc, vbuf));
    }
    cblistclose(list);
    if(g_smlrvnum > 0){
      printf("%s", ESTDCNTLVECTOR);
      id = est_doc_id(resdoc->doc);
      kwords = id > 0 ? est_mtdb_get_keywords(db, id) : NULL;
      if(!kwords){
        if(g_extdelay < 0){
          kwords = est_morph_etch_doc(resdoc->doc, g_smlrvnum);
        } else {
          kwords = est_mtdb_etch_doc(db, resdoc->doc, g_smlrvnum);
        }
      }
      cbmapiterinit(kwords);
      while((kbuf = cbmapiternext(kwords, NULL)) != NULL){
        printf("\t%s\t%s", kbuf, cbmapiterval(kbuf, NULL));
      }
      cbmapclose(kwords);
      printf("\n");
    }
    printf("\n");
    if(wwidth < 0){
      texts = est_doc_texts(resdoc->doc);
      for(j = 0; j < cblistnum(texts); j++){
        printf("%s\n", cblistval(texts, j, NULL));
      }
      vbuf = est_doc_hidden_texts(resdoc->doc);
      if(vbuf[0] != '\0')  printf("\t%s\n", vbuf);
    } else if(wwidth > 0){
      snippet = est_doc_make_snippet(resdoc->doc, words, wwidth, hwidth, awidth);
      printf("%s", snippet);
      free(snippet);
    }
  }
  free(resdocs);
  printf("%s:END\n", bordstr);
  cblistclose(words);
  cbmapclose(hints);
  resmap_delete(resmap);
  est_cond_delete(cond);
}


/* send the result of the get_doc command */
static void sendnodecmdgetdoc(ESTMTDB *db, const char *myurl, const char *mylabel,
                              CBMAP *params){
  ESTDOC *doc;
  const char *rp, *uri;
  char *draft;
  int id;
  id = (rp = cbmapget(params, "id", -1, NULL)) ? atoi(rp) : 0;
  if(!(uri = cbmapget(params, "uri", -1, NULL))) uri = "";
  if(id < 1 && uri[0] == '\0'){
    senderror(400, "Bad Request (the parameters lack)");
    return;
  }
  if(id < 1) id = est_mtdb_uri_to_id(db, uri);
  if(id > 0 && (doc = est_mtdb_get_doc(db, id, 0)) != NULL){
    est_doc_add_attr(doc, DATTRNDURL, myurl);
    est_doc_add_attr(doc, DATTRNDLABEL, mylabel);
    draft = est_doc_dump_draft(doc);
    printf("Content-Type: %s; charset=UTF-8\r\n", ESTDRAFTTYPE);
    printf("\r\n");
    printf("%s", draft);
    free(draft);
    est_doc_delete(doc);
  } else {
    senderror(400, "Bad Request (maybe, the document does not exist)");
  }
}


/* send the result of the get_doc_attr command */
static void sendnodecmdgetdocattr(ESTMTDB *db, const char *myurl, const char *mylabel,
                                  CBMAP *params){
  const char *rp, *uri, *attr;
  char *value;
  int id;
  id = (rp = cbmapget(params, "id", -1, NULL)) ? atoi(rp) : 0;
  if(!(uri = cbmapget(params, "uri", -1, NULL))) uri = "";
  if(!(attr = cbmapget(params, "attr", -1, NULL))) attr = "";
  if((id < 1 && uri[0] == '\0') || attr[0] == '\0'){
    senderror(400, "Bad Request (the parameters lack)");
    return;
  }
  if(id < 1) id = est_mtdb_uri_to_id(db, uri);
  if(id > 0 && (value = est_mtdb_get_doc_attr(db, id, attr)) != NULL){
    printf("Content-Type: text/plain; charset=UTF-8\r\n");
    printf("\r\n");
    printf("%s\n", value);
    free(value);
  } else {
    senderror(400, "Bad Request (maybe, the document or the attribute does not exist)");
  }
}


/* send the result of the etch_doc command */
static void sendnodecmdetchdoc(ESTMTDB *db, const char *myurl, const char *mylabel,
                               CBMAP *params){
  ESTDOC *doc;
  CBMAP *kwords;
  const char *rp, *uri, *kbuf;
  int id, ksiz;
  id = (rp = cbmapget(params, "id", -1, NULL)) ? atoi(rp) : 0;
  if(!(uri = cbmapget(params, "uri", -1, NULL))) uri = "";
  if(id < 1 && uri[0] == '\0'){
    senderror(400, "Bad Request (the parameters lack)");
    return;
  }
  if(id < 1) id = est_mtdb_uri_to_id(db, uri);
  kwords = NULL;
  if(id > 0){
    kwords = est_mtdb_get_keywords(db, id);
    if(!kwords && (doc = est_mtdb_get_doc(db, id, 0)) != NULL){
      kwords = est_mtdb_etch_doc(db, doc, g_smlrvnum > 0 ? g_smlrvnum : KWORDNUM);
      est_doc_delete(doc);
    }
  }
  if(kwords){
    printf("Content-Type: text/plain; charset=UTF-8\r\n");
    printf("\r\n");
    cbmapiterinit(kwords);
    while((kbuf = cbmapiternext(kwords, &ksiz)) != NULL){
      printf("%s\t%s\n", kbuf, cbmapiterval(kbuf, NULL));
    }
    cbmapclose(kwords);
  } else {
    senderror(400, "Bad Request (maybe, the document does not exist)");
  }
}


/* send the result of the uri_to_id command */
static void sendnodecmduritoid(ESTMTDB *db, const char *myurl, const char *mylabel,
                               CBMAP *params){
  const char *uri;
  int id;
  uri = cbmapget(params, "uri", -1, NULL);
  if(!uri){
    senderror(400, "Bad Request (the parameters lack)");
    return;
  }
  if((id = est_mtdb_uri_to_id(db, uri)) > 0){
    printf("Content-Type: text/plain; charset=UTF-8\r\n");
    printf("\r\n");
    printf("%d\n", id);
  } else {
    senderror(400, "Bad Request (maybe, the URI is not registered)");
  }
}


/* send the result of the put_doc command */
static void sendnodecmdputdoc(ESTMTDB *db, const char *myurl, const char *mylabel,
                              CBMAP *params){
  ESTDOC *doc;
  CBMAP *kwords;
  const char *draft, *uri;
  char *dbuf, *tmp;
  draft = cbmapget(params, "draft", -1, NULL);
  if(!draft){
    senderror(400, "Bad Request (the parameters lack)");
    return;
  }
  doc = est_doc_new_from_draft(draft);
  if(!(uri = est_doc_attr(doc, ESTDATTRURI))){
    senderror(400, "Bad Request (the parameters lack)");
    est_doc_delete(doc);
    return;
  }
  dbuf = NULL;
  if(g_smlrvnum > 0 && g_extdelay < 0 && !est_doc_keywords(doc)){
    kwords = est_morph_etch_doc(doc, g_smlrvnum);
    est_doc_set_keywords(doc, kwords);
    cbmapclose(kwords);
    dbuf = est_doc_dump_draft(doc);
    draft = dbuf;
  }
  tmp = cbsprintf("%s%s%c%012d%06d.est",
                  est_mtdb_name(db), g_pidxsuffix, ESTPATHCHR, (int)time(NULL), (int)getpid());
  if(cbwritefile(tmp, draft, -1)){
    if(est_mtdb_uri_to_id(db, uri) > 0) cbmapput(params, PPOUTDOC, -1, uri, -1, TRUE);
    if(est_mtdb_pseudo_doc_num(db) > g_pidxdocmax)
      cbmapput(params, PPPOSTCMD, -1, "sync", -1, TRUE);
    printf("Content-Type: text/plain; charset=UTF-8\r\n");
    printf("\r\n");
    printf("OK\n");
  } else {
    senderror(403, "Forbidden (writing a pseudo document failed)");
  }
  free(tmp);
  free(dbuf);
  est_doc_delete(doc);
}


/* send the result of the out_doc command */
static void sendnodecmdoutdoc(ESTMTDB *db, const char *myurl, const char *mylabel,
                              CBMAP *params){
  const char *rp, *uri;
  char *tmp;
  int id;
  id = (rp = cbmapget(params, "id", -1, NULL)) ? atoi(rp) : 0;
  if(!(uri = cbmapget(params, "uri", -1, NULL))) uri = "";
  if(id < 1 && uri[0] == '\0'){
    senderror(400, "Bad Request (the parameters lack)");
    return;
  }
  if(id < 1) id = est_mtdb_uri_to_id(db, uri);
  if(id > 0){
    if((tmp = est_mtdb_get_doc_attr(db, id, ESTDATTRURI)) != NULL){
      cbmapput(params, PPOUTDOC, -1, tmp, -1, TRUE);
      free(tmp);
    }
    printf("Content-Type: text/plain; charset=UTF-8\r\n");
    printf("\r\n");
    printf("OK\n");
  } else {
    senderror(400, "Bad Request (maybe, the document does not exist)");
  }
}


/* send the result of the edit_doc command */
static void sendnodecmdeditdoc(ESTMTDB *db, const char *myurl, const char *mylabel,
                               CBMAP *params){
  ESTDOC *doc, *odoc, *ndoc;
  const CBLIST *texts;
  CBLIST *list, *names;
  const char *draft, *uri, *name, *vbuf;
  char *tmp, numbuf[NUMBUFSIZ], *odraft, *ndraft;
  int i, j, id, done;
  draft = cbmapget(params, "draft", -1, NULL);
  if(!draft){
    senderror(400, "Bad Request (the parameters lack)");
    return;
  }
  doc = est_doc_new_from_draft(draft);
  if(!(uri = est_doc_attr(doc, ESTDATTRURI))){
    senderror(400, "Bad Request (the parameters lack)");
    est_doc_delete(doc);
    return;
  }
  if((id = est_mtdb_uri_to_id(db, uri)) < 1){
    senderror(400, "Bad Request (maybe, the document does not exist)");
    est_doc_delete(doc);
    return;
  }
  if(id >= ESTPDOCIDMIN){
    est_doc_add_attr(doc, ESTDATTRID, NULL);
    tmp = cbsprintf("%s%s", est_mtdb_name(db), g_pidxsuffix);
    list = cbdirlist(tmp);
    free(tmp);
    done = FALSE;
    if(list){
      cblistsort(list);
      for(i = 0; !done && i < cblistnum(list); i++){
        name = cblistval(list, i, NULL);
        if(!strcmp(name, ESTCDIRSTR) || !strcmp(name, ESTPDIRSTR)) continue;
        tmp = cbsprintf("%s%s%c%s", est_mtdb_name(db), g_pidxsuffix, ESTPATHCHR, name);
        if((odraft = cbreadfile(tmp, NULL)) != NULL){
          odoc = est_doc_new_from_draft(odraft);
          if((vbuf = est_doc_attr(odoc, ESTDATTRURI)) != NULL && !strcmp(vbuf, uri)){
            ndoc = est_doc_new();
            names = est_doc_attr_names(doc);
            for(j = 0; j < cblistnum(names); j++){
              vbuf = cblistval(names, j, NULL);
              if(!strcmp(vbuf, ESTDATTRID)) continue;
              est_doc_add_attr(ndoc, vbuf, est_doc_attr(doc, vbuf));
            }
            texts = est_doc_texts(odoc);
            for(j = 0; j < cblistnum(texts); j++){
              est_doc_add_text(ndoc, cblistval(texts, j, NULL));
            }
            if((vbuf = est_doc_hidden_texts(odoc)) != NULL && vbuf[0] != '\0')
              est_doc_add_hidden_text(ndoc, vbuf);
            ndraft = est_doc_dump_draft(ndoc);
            if(cbwritefile(tmp, ndraft, -1)) done = TRUE;
            free(ndraft);
            cblistclose(names);
            est_doc_delete(ndoc);
          }
          est_doc_delete(odoc);
          free(odraft);
        }
        free(tmp);
      }
      cblistclose(list);
    }
    if(done){
      printf("Content-Type: text/plain; charset=UTF-8\r\n");
      printf("\r\n");
      printf("OK\n");
    } else {
      senderror(403, "Forbidden (writing a pseudo document failed)");
    }
  } else {
    sprintf(numbuf, "%d", id);
    est_doc_add_attr(doc, ESTDATTRID, numbuf);
    if(est_mtdb_edit_doc(db, doc)){
      printf("Content-Type: text/plain; charset=UTF-8\r\n");
      printf("\r\n");
      printf("OK\n");
    } else {
      senderror(400, "Bad Request (maybe, the document is invalid)");
    }
  }
  est_doc_delete(doc);
}


/* send the result of the sync command */
static void sendnodecmdsync(ESTMTDB *db, const char *myurl, const char *mylabel,
                            CBMAP *params){
  cbmapput(params, PPPOSTCMD, -1, "sync", -1, TRUE);
  printf("Content-Type: text/plain; charset=UTF-8\r\n");
  printf("\r\n");
  printf("OK\n");
}


/* send the result of the optimize command */
static void sendnodecmdoptimize(ESTMTDB *db, const char *myurl, const char *mylabel,
                                CBMAP *params){
  cbmapput(params, PPPOSTCMD, -1, "optimize", -1, TRUE);
  printf("Content-Type: text/plain; charset=UTF-8\r\n");
  printf("\r\n");
  printf("OK\n");
}


/* perform the sync post command */
static void procpostcmdoutdoc(const char *idxname, const char *uri){
  ESTMTDB *db;
  ESTDOC *doc;
  CBLIST *list;
  const char *name, *vbuf;
  char *tmp, *draft;
  int i, ecode, done, id;
  tmp = cbsprintf("%s%c%s", g_indexdir, ESTPATHCHR, idxname);
  if(!(db = est_mtdb_open(tmp, ESTDBWRITER, &ecode))){
    free(tmp);
    return;
  }
  free(tmp);
  done = FALSE;
  if((id = est_mtdb_uri_to_id(db, uri)) > 0 && est_mtdb_out_doc(db, id, ESTODCLEAN)) done = TRUE;
  if(!done){
    tmp = cbsprintf("%s%c%s%s", g_indexdir, ESTPATHCHR, idxname, g_pidxsuffix);
    list = cbdirlist(tmp);
    free(tmp);
    if(list){
      cblistsort(list);
      for(i = 0; !done && i < cblistnum(list); i++){
        name = cblistval(list, i, NULL);
        if(!strcmp(name, ESTCDIRSTR) || !strcmp(name, ESTPDIRSTR)) continue;
        tmp = cbsprintf("%s%c%s%s%c%s",
                        g_indexdir, ESTPATHCHR, idxname, g_pidxsuffix, ESTPATHCHR, name);
        if((draft = cbreadfile(tmp, NULL)) != NULL){
          doc = est_doc_new_from_draft(draft);
          if((vbuf = est_doc_attr(doc, ESTDATTRURI)) != NULL && !strcmp(vbuf, uri)){
            unlink(tmp);
            done = TRUE;
          }
          est_doc_delete(doc);
          free(draft);
        }
        free(tmp);
      }
      cblistclose(list);
    }
  }
  est_mtdb_close(db, &ecode);
}


/* perform the sync post command */
static void procpostcmdsync(const char *idxname){
  ESTMTDB *db;
  ESTDOC *doc;
  CBLIST *list;
  const char *name;
  char *tmp, *draft;
  int i, ecode, num;
  tmp = cbsprintf("%s%c%s", g_indexdir, ESTPATHCHR, idxname);
  if(!(db = est_mtdb_open(tmp, ESTDBWRITER, &ecode))){
    free(tmp);
    return;
  }
  free(tmp);
  tmp = cbsprintf("%s%c%s%s", g_indexdir, ESTPATHCHR, idxname, g_pidxsuffix);
  list = cbdirlist(tmp);
  free(tmp);
  if(list){
    cblistsort(list);
    num = cblistnum(list) - g_pidxdocmin;
    for(i = 0; i < num; i++){
      name = cblistval(list, i, NULL);
      if(!strcmp(name, ESTCDIRSTR) || !strcmp(name, ESTPDIRSTR)) continue;
      tmp = cbsprintf("%s%c%s%s%c%s",
                      g_indexdir, ESTPATHCHR, idxname, g_pidxsuffix, ESTPATHCHR, name);
      if((draft = cbreadfile(tmp, NULL)) != NULL){
        doc = est_doc_new_from_draft(draft);
        if(est_mtdb_put_doc(db, doc, ESTPDCLEAN)) unlink(tmp);
        est_doc_delete(doc);
        free(draft);
      }
      free(tmp);
    }
    cblistclose(list);
  }
  est_mtdb_close(db, &ecode);
}


/* perform the optimize post command */
static void procpostcmdoptimize(const char *idxname){
  ESTMTDB *db;
  char *tmp;
  int ecode;
  tmp = cbsprintf("%s%c%s", g_indexdir, ESTPATHCHR, idxname);
  if(!(db = est_mtdb_open(tmp, ESTDBWRITER, &ecode))){
    free(tmp);
    return;
  }
  free(tmp);
  est_mtdb_optimize(db, ESTOPTNOPURGE);
  est_mtdb_close(db, &ecode);
}



/* END OF FILE */
