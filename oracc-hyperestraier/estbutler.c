/*************************************************************************************************
 * The search helper for the node master
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


#include "mastermod.h"


/* global variables */
const char *g_progname;                  /* program name */


/* function prototypes */
int main(int argc, char **argv);
static void usage(void);
static int runinform(int argc, char **argv);
static int runsearch(int argc, char **argv);
static int rungetdoc(int argc, char **argv);
static int rungetdocattr(int argc, char **argv);
static int runetchdoc(int argc, char **argv);
static int runuritoid(int argc, char **argv);
static int procinform(const char *dbname, const char *outfile);
static int procsearch(const char *dbname, CBMAP *params, const char *outfile,
                      const char *myurl, const char *mylabel, int rateuri, int mergemethod,
                      int scoreexpr, int searchmax, int wildmax,
                      int wwidth, int hwidth, int awidth, int scancheck, int smlrvnum);
static int procgetdoc(const char *dbname, const char *outfile, int id, const char *uri);
static int procgetdocattr(const char *dbname, const char *outfile,
                          int id, const char *uri, const char *attr);
static int procetchdoc(const char *dbname, const char *outfile,
                       int id, const char *uri, int knum);
static int procuritoid(const char *dbname, const char *outfile, const char *uri);


/* main routine */
int main(int argc, char **argv){
  const char *tmp;
  int rv;
  if((tmp = getenv("ESTDBGFD")) != NULL) dpdbgfd = atoi(tmp);
  est_proc_env_reset();
  g_progname = argv[0];
  if(argc < 2) usage();
  rv = 0;
  if(!strcmp(argv[1], "inform")){
    rv = runinform(argc, argv);
  } else if(!strcmp(argv[1], "search")){
    rv = runsearch(argc, argv);
  } else if(!strcmp(argv[1], "getdoc")){
    rv = rungetdoc(argc, argv);
  } else if(!strcmp(argv[1], "getdocattr")){
    rv = rungetdocattr(argc, argv);
  } else if(!strcmp(argv[1], "etchdoc")){
    rv = runetchdoc(argc, argv);
  } else if(!strcmp(argv[1], "uritoid")){
    rv = runuritoid(argc, argv);
  } else {
    usage();
  }
  return rv;
}


/* print the usage and exit */
static void usage(void){
  fprintf(stderr, "%s: the search helper for the node master\n", g_progname);
  fprintf(stderr, "\n");
  fprintf(stderr, "usage:\n");
  fprintf(stderr, "  %s inform db outfile\n", g_progname);
  fprintf(stderr, "  %s search db outfile params url label details ...\n", g_progname);
  fprintf(stderr, "  %s getdoc db outfile id uri\n", g_progname);
  fprintf(stderr, "  %s getdocattr db outfile id uri attr\n", g_progname);
  fprintf(stderr, "  %s etchdoc db outfile id uri knum\n", g_progname);
  fprintf(stderr, "  %s uritoid db outfile uri\n", g_progname);
  fprintf(stderr, "\n");
  exit(1);
}


/* parse arguments of the inform command */
static int runinform(int argc, char **argv){
  const char *dbname, *outfile;
  int rv;
  if(argc < 4) usage();
  dbname = argv[2];
  outfile = argv[3];
  rv = procinform(dbname, outfile);
  return 0;
}


/* parse arguments of the search command */
static int runsearch(int argc, char **argv){
  CBMAP *params;
  const char *dbname, *outfile;
  char *pbuf, *myurl, *mylabel;
  int rv, size, scoreexpr, rateuri, mergemethod, searchmax;
  int wildmax, wwidth, hwidth, awidth, scancheck, smlrvnum;
  if(argc < 17) usage();
  dbname = argv[2];
  outfile = argv[3];
  pbuf = cbbasedecode(argv[4], &size);
  params = cbmapload(pbuf, size);
  myurl = cbbasedecode(argv[5], NULL);
  mylabel = cbbasedecode(argv[6], NULL);
  rateuri = atoi(argv[7]);
  mergemethod = atoi(argv[8]);
  scoreexpr = atoi(argv[9]);
  searchmax = atoi(argv[10]);
  wildmax = atoi(argv[11]);
  wwidth = atoi(argv[12]);
  hwidth = atoi(argv[13]);
  awidth = atoi(argv[14]);
  scancheck = atoi(argv[15]);
  smlrvnum = atoi(argv[16]);
  if(searchmax < 0 || wildmax < 0 || wwidth < 0 || hwidth < 0 || awidth < 0) usage();
  rv = procsearch(dbname, params, outfile, myurl, mylabel, rateuri, mergemethod,
                  scoreexpr, searchmax, wildmax, wwidth, hwidth, awidth, scancheck, smlrvnum);
  free(mylabel);
  free(myurl);
  cbmapclose(params);
  free(pbuf);
  return rv;
}


/* parse arguments of the getdoc command */
static int rungetdoc(int argc, char **argv){
  const char *dbname, *outfile;
  char *uri;
  int id, rv;
  if(argc < 6) usage();
  dbname = argv[2];
  outfile = argv[3];
  id = atoi(argv[4]);
  uri = cbbasedecode(argv[5], NULL);
  rv = procgetdoc(dbname, outfile, id, uri);
  free(uri);
  return rv;
}


/* parse arguments of the getdocattr command */
static int rungetdocattr(int argc, char **argv){
  const char *dbname, *outfile;
  char *uri, *attr;
  int id, rv;
  if(argc < 7) usage();
  dbname = argv[2];
  outfile = argv[3];
  id = atoi(argv[4]);
  uri = cbbasedecode(argv[5], NULL);
  attr = cbbasedecode(argv[6], NULL);
  rv = procgetdocattr(dbname, outfile, id, uri, attr);
  free(uri);
  free(attr);
  return rv;
}


/* parse arguments of the etchdoc command */
static int runetchdoc(int argc, char **argv){
  const char *dbname, *outfile;
  char *uri;
  int id, knum, rv;
  if(argc < 7) usage();
  dbname = argv[2];
  outfile = argv[3];
  id = atoi(argv[4]);
  uri = cbbasedecode(argv[5], NULL);
  knum = atoi(argv[6]);
  rv = procetchdoc(dbname, outfile, id, uri, knum);
  free(uri);
  return rv;
}


/* parse arguments of the uritoid command */
static int runuritoid(int argc, char **argv){
  const char *dbname, *outfile;
  char *uri;
  int rv;
  if(argc < 5) usage();
  dbname = argv[2];
  outfile = argv[3];
  uri = cbbasedecode(argv[4], NULL);
  rv = procuritoid(dbname, outfile, uri);
  free(uri);
  return rv;
}


/* perform the inform command */
static int procinform(const char *dbname, const char *outfile){
  ESTDB *db;
  char outbuf[NUMBUFSIZ*3];
  int ecode, err;
  if(!(db = est_db_open(dbname, ESTDBREADER | ESTDBNOLCK, &ecode))) return 1;
  err = FALSE;
  sprintf(outbuf, "%d\t%d\t%.0f\n", est_db_doc_num(db), est_db_word_num(db), est_db_size(db));
  if(!cbwritefile(outfile, outbuf, -1)) err = TRUE;
  if(!est_db_close(db, &ecode)) err = TRUE;
  return err ? 1 : 0;
}


/* perform the search command */
static int procsearch(const char *dbname, CBMAP *params, const char *outfile,
                      const char *myurl, const char *mylabel, int rateuri, int mergemethod,
                      int scoreexpr, int searchmax, int wildmax,
                      int wwidth, int hwidth, int awidth, int scancheck, int smlrvnum){
  RESMAP *resmap;
  RESDOC **resdocs, *resdoc;
  ESTDB *db;
  ESTCOND *cond;
  ESTDOC *doc;
  CBMAP *hints, *kwords;
  const CBLIST *texts;
  CBLIST *words, *list;
  CBDATUM *datum;
  const char *bordstr, *tmp, *kbuf, *vbuf, *phrase, *order, *distinct;
  char name[NUMBUFSIZ], *snippet;
  int i, j, ecode, err, max, skip, num, ksiz, vsiz, lmax, cnt, score, down, dnum, wnum;
  int end, *res, rnum, hnum, id;
  double curtime, itime, weight, fsiz;
  if(!(db = est_db_open(dbname, ESTDBREADER | ESTDBNOLCK, &ecode))) return 1;
  err = FALSE;
  bordstr = est_border_str();
  cond = est_cond_new();
  max = DEFMAXSRCH;
  skip = 0;
  if((tmp = cbmapget(params, "skip", -1, NULL)) != NULL && (num = atoi(tmp)) > 0)
    skip = num;
  if((tmp = cbmapget(params, "phrase", -1, NULL)) != NULL && tmp[0] != '\0')
    est_cond_set_phrase(cond, tmp);
  if((tmp = cbmapget(params, "attr", -1, NULL)) != NULL && tmp[0] != '\0')
    est_cond_add_attr(cond, tmp);
  for(i = 0; i <= CONDATTRMAX; i++){
    num = sprintf(name, "attr%d", i);
    if((tmp = cbmapget(params, name, num, NULL)) != NULL && tmp[0] != '\0')
      est_cond_add_attr(cond, tmp);
  }
  if((tmp = cbmapget(params, "order", -1, NULL)) != NULL && tmp[0] != '\0')
    est_cond_set_order(cond, tmp);
  if((tmp = cbmapget(params, "max", -1, NULL)) != NULL && (num = atoi(tmp)) >= 0)
    max = num;
  max = max > searchmax ? searchmax : max;
  if((tmp = cbmapget(params, "options", -1, NULL)) != NULL && (num = atoi(tmp)) > 0)
    est_cond_set_options(cond, num);
  if((tmp = cbmapget(params, "auxiliary", -1, NULL)) != NULL)
    est_cond_set_auxiliary(cond, atoi(tmp));
  if((tmp = cbmapget(params, "distinct", -1, NULL)) != NULL && tmp[0] != '\0')
    est_cond_set_order(cond, tmp);
  if((tmp = cbmapget(params, "wwidth", -1, NULL)) != NULL) wwidth = atoi(tmp);
  if((tmp = cbmapget(params, "hwidth", -1, NULL)) != NULL && (num = atoi(tmp)) >= 0)
    hwidth = num;
  if((tmp = cbmapget(params, "awidth", -1, NULL)) != NULL && (num = atoi(tmp)) >= 0)
    awidth = num;
  resmap = resmap_new();
  hints = cbmapopenex(MINIBNUM);
  curtime = est_gettimeofday();
  lmax = max + skip + 1;
  est_cond_set_options(cond, ESTCONDSCFB);
  res = est_db_search(db, cond, &rnum, hints);
  hnum = (vbuf = cbmapget(hints, "", 0, NULL)) ? atoi(vbuf) : rnum;
  if(max >= 0 && hnum < max + 1 && est_cond_auxiliary_word(cond, "")){
    free(res);
    est_cond_set_auxiliary(cond, -1);
    res = est_db_search(db, cond, &rnum, hints);
    hnum = (vbuf = cbmapget(hints, "", 0, NULL)) ? atoi(vbuf) : rnum;
  }
  itime = est_gettimeofday() - curtime;
  cnt = 0;
  phrase = est_cond_phrase(cond);
  rateuri = rateuri && !(phrase && cbstrfwmatch(phrase, ESTOPSIMILAR));
  for(i = 0; i < rnum && cnt < lmax; i++){
    if(!(doc = est_db_get_doc(db, res[i], 0))) continue;
    if(scancheck && phrase && phrase[0] != '[' && phrase[0] != '*' &&
       !est_db_scan_doc(db, doc, cond)){
      est_doc_delete(doc);
      continue;
    }
    score = est_cond_score(cond, i);
    if(rateuri && scoreexpr != SE_ASIS){
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
    if(scoreexpr != SE_ASIS){
      weight = 1.0;
      if((vbuf = est_doc_attr(doc, ESTDATTRWEIGHT)) != NULL){
        weight = strtod(vbuf, NULL);
        weight = weight >= 0.01 ? weight : 0.01;
      }
      weight /= 10.0;
      switch(mergemethod){
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
  dnum = est_db_doc_num(db);
  wnum = est_db_word_num(db);
  fsiz = est_db_size(db);
  end = max + skip;
  curtime = est_gettimeofday() - curtime;
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "%s\n", bordstr);
  cbdatumprintf(datum, "VERSION\t%s\n", _EST_PROTVER);
  cbdatumprintf(datum, "NODE\t%s\n", myurl);
  cbdatumprintf(datum, "HIT\t%d\n", hnum);
  cbmapiterinit(hints);
  num = 1;
  while((kbuf = cbmapiternext(hints, &ksiz)) != NULL){
    if(ksiz < 1) continue;
    cbdatumprintf(datum, "HINT#%d\t%s\t%s\n", num, kbuf, cbmapiterval(kbuf, NULL));
    num++;
  }
  cbdatumprintf(datum, "DOCNUM\t%d\n", dnum);
  cbdatumprintf(datum, "WORDNUM\t%d\n", wnum);
  cbdatumprintf(datum, "TIME\t%.6f\n", curtime / 1000.0);
  cbdatumprintf(datum, "TIME#i\t%.6f\n", itime / 1000.0);
  cbdatumprintf(datum, "TIME#0\t%.6f\n", curtime / 1000.0);
  cbdatumprintf(datum, "LINK#0\t%s\t", myurl);
  cbdatumprintf(datum, "%s\t%d\t%d\t%d\t%.0f\t%d\n",
                mylabel, SELFCREDIT, dnum, wnum, fsiz, hnum);
  cbdatumprintf(datum, "VIEW\tSNIPPET\n");
  cbdatumprintf(datum, "\n");
  phrase = est_cond_phrase(cond);
  order = est_cond_order(cond);
  if(!order && phrase && cbstrfwmatch(phrase, ESTOPSIMILAR))
    order = DATTRNDSCORE " " ESTORDNUMD;
  distinct = est_cond_distinct(cond);
  resdocs = resmap_list(resmap, &rnum, order, distinct);
  for(i = skip; i < rnum && i < end; i++){
    resdoc = resdocs[i];
    if(!resdoc->doc) continue;
    cbdatumprintf(datum, "%s\n", bordstr);
    list = est_doc_attr_names(resdoc->doc);
    for(j = 0; j < cblistnum(list); j++){
      vbuf = cblistval(list, j, NULL);
      cbdatumprintf(datum, "%s=%s\n", vbuf, est_doc_attr(resdoc->doc, vbuf));
    }
    cblistclose(list);
    if(smlrvnum > 0){
      cbdatumprintf(datum, "%s", ESTDCNTLVECTOR);
      id = est_doc_id(resdoc->doc);
      kwords = id > 0 ? est_db_get_keywords(db, id) : NULL;
      if(!kwords) kwords = est_db_etch_doc(db, resdoc->doc, smlrvnum);
      cbmapiterinit(kwords);
      while((kbuf = cbmapiternext(kwords, &ksiz)) != NULL){
        cbdatumcat(datum, "\t", 1);
        cbdatumcat(datum, kbuf, ksiz);
        cbdatumcat(datum, "\t", 1);
        vbuf = cbmapiterval(kbuf, &vsiz);
        cbdatumcat(datum, vbuf, vsiz);
      }
      cbmapclose(kwords);
      cbdatumcat(datum, "\n", 1);
    }
    cbdatumcat(datum, "\n", 1);
    if(wwidth < 0){
      texts = est_doc_texts(resdoc->doc);
      for(j = 0; j < cblistnum(texts); j++){
        vbuf = cblistval(texts, j, &vsiz);
        cbdatumcat(datum, vbuf, vsiz);
        cbdatumcat(datum, "\n", 1);
      }
      vbuf = est_doc_hidden_texts(resdoc->doc);
      if(vbuf[0] != '\0')  cbdatumprintf(datum, "\t%s\n", vbuf);
    } else if(wwidth > 0){
      snippet = est_doc_make_snippet(resdoc->doc, words, wwidth, hwidth, awidth);
      cbdatumcat(datum, snippet, -1);
      free(snippet);
    }
  }
  free(resdocs);
  cbdatumprintf(datum, "%s:END\n", bordstr);
  if(!cbwritefile(outfile, cbdatumptr(datum), cbdatumsize(datum))) err = TRUE;
  cbdatumclose(datum);
  cblistclose(words);
  cbmapclose(hints);
  resmap_delete(resmap);
  est_cond_delete(cond);
  if(!est_db_close(db, &ecode)) err = TRUE;
  return err ? 1 : 0;
}


/* perform the getdoc command */
static int procgetdoc(const char *dbname, const char *outfile, int id, const char *uri){
  ESTDB *db;
  ESTDOC *doc;
  char *draft;
  int ecode, err;
  if(!(db = est_db_open(dbname, ESTDBREADER | ESTDBNOLCK, &ecode))) return 1;
  err = FALSE;
  if(id < 1) id = est_db_uri_to_id(db, uri);
  if(id > 0 && (doc = est_db_get_doc(db, id, 0)) != NULL){
    draft = est_doc_dump_draft(doc);
    est_doc_delete(doc);
  } else {
    draft = cbmemdup("", 0);
  }
  if(!cbwritefile(outfile, draft, -1)) err = TRUE;
  free(draft);
  if(!est_db_close(db, &ecode)) err = TRUE;
  return err ? 1 : 0;
}


/* perform the getdocattr command */
static int procgetdocattr(const char *dbname, const char *outfile,
                          int id, const char *uri, const char *attr){
  ESTDB *db;
  char *value;
  int ecode, err;
  if(!(db = est_db_open(dbname, ESTDBREADER | ESTDBNOLCK, &ecode))) return 1;
  err = FALSE;
  if(id < 1) id = est_db_uri_to_id(db, uri);
  if(id > 0 && (value = est_db_get_doc_attr(db, id, attr)) != NULL){
    value = cbrealloc(value, strlen(value) + 2);
    strcat(value, "\n");
  } else {
    value = cbmemdup("", 0);
  }
  if(!cbwritefile(outfile, value, -1)) err = TRUE;
  free(value);
  if(!est_db_close(db, &ecode)) err = TRUE;
  return err ? 1 : 0;
}


/* perform the etchdoc command */
static int procetchdoc(const char *dbname, const char *outfile,
                       int id, const char *uri, int knum){
  ESTDB *db;
  ESTDOC *doc;
  CBMAP *kwords;
  CBDATUM *outbuf;
  const char *kbuf, *vbuf;
  int ecode, err, ksiz, vsiz;
  if(!(db = est_db_open(dbname, ESTDBREADER | ESTDBNOLCK, &ecode))) return 1;
  err = FALSE;
  outbuf = cbdatumopen(NULL, -1);
  if(id < 1) id = est_db_uri_to_id(db, uri);
  if(id > 0 && (kwords = est_db_get_keywords(db, id)) != NULL){
    cbmapiterinit(kwords);
    while((kbuf = cbmapiternext(kwords, &ksiz)) != NULL){
      vbuf = cbmapiterval(kbuf, &vsiz);
      cbdatumcat(outbuf, kbuf, ksiz);
      cbdatumcat(outbuf, "\t", 1);
      cbdatumcat(outbuf, vbuf, vsiz);
      cbdatumcat(outbuf, "\n", 1);
    }
    cbmapclose(kwords);
  } else if(id > 0 && (doc = est_db_get_doc(db, id, 0)) != NULL){
    kwords = est_db_etch_doc(db, doc, knum > 0 ? knum : 32);
    cbmapiterinit(kwords);
    while((kbuf = cbmapiternext(kwords, &ksiz)) != NULL){
      vbuf = cbmapiterval(kbuf, &vsiz);
      cbdatumcat(outbuf, kbuf, ksiz);
      cbdatumcat(outbuf, "\t", 1);
      cbdatumcat(outbuf, vbuf, vsiz);
      cbdatumcat(outbuf, "\n", 1);
    }
    cbmapclose(kwords);
    est_doc_delete(doc);
  }
  if(!cbwritefile(outfile, cbdatumptr(outbuf), cbdatumsize(outbuf))) err = TRUE;
  cbdatumclose(outbuf);
  if(!est_db_close(db, &ecode)) err = TRUE;
  return err ? 1 : 0;
}


/* perform the uritoid command */
static int procuritoid(const char *dbname, const char *outfile, const char *uri){
  ESTDB *db;
  char outbuf[NUMBUFSIZ];
  int ecode, err, id;
  if(!(db = est_db_open(dbname, ESTDBREADER | ESTDBNOLCK, &ecode))) return 1;
  err = FALSE;
  if((id = est_db_uri_to_id(db, uri)) > 0){
    sprintf(outbuf, "%d\n", id);
  } else {
    outbuf[0] = '\0';
    err = TRUE;
  }
  if(!cbwritefile(outfile, outbuf, -1)) err = TRUE;
  if(!est_db_close(db, &ecode)) err = TRUE;
  return err ? 1 : 0;
}



/* END OF FILE */
