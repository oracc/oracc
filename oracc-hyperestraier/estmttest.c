/*************************************************************************************************
 * The command line interface of the MT-safe API
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
#include "myconf.h"

#define URIBUFSIZ      8192              /* size of a buffer for an URI */
#define BRANCHDBNUM    4                 /* number of branch databases */

typedef struct {                         /* type of structure for a thread mission */
  int id;                                /* ID number */
  ESTMTDB *db;                           /* database object */
  ESTMTDB **others;                      /* other's database object */
  int dnum;                              /* number of documents */
} MISSION;

enum {                                   /* enumeration for test documents */
  RD_ENG,                                /* English */
  RD_LAT,                                /* Latin */
  RD_EURO,                               /* European mix */
  RD_ORI,                                /* Oriental */
  RD_JPN,                                /* Japanese */
  RD_CHAO,                               /* chaos */
  RD_RAND                                /* selected at random */
};


/* global variables */
const char *g_progname;                  /* program name */
int g_sigterm = FALSE;                   /* flag for termination signal */


/* function prototypes */
int main(int argc, char **argv);
static void printferror(const char *format, ...);
static void printfinfo(const char *format, ...);
static void dbinform(const char *message, void *opaque);
static void setsignals(void);
static void sigtermhandler(int num);
static void usage(void);
static int runwicked(int argc, char **argv);
static int runregression(int argc, char **argv);
static int procwicked(const char *dbname, int dnum, int tnum);
static int procregression(const char *dbname);
static void *mtwkfunc(void *mission);
static ESTDOC *est_doc_new_from_chaos(int cnum, int snum, int mode);
static char *est_random_str(int cnum, int mode);


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
  if(!strcmp(argv[1], "wicked")){
    setsignals();
    rv = runwicked(argc, argv);
  } else if(!strcmp(argv[1], "regression")){
    setsignals();
    rv = runregression(argc, argv);
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
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  va_list ap;
  va_start(ap, format);
  if(pthread_mutex_lock(&mutex) != 0) return;
  printf("%s: INFO: ", g_progname);
  vprintf(format, ap);
  putchar('\n');
  fflush(stdout);
  pthread_mutex_unlock(&mutex);
  va_end(ap);
}


/* callback function for database events */
static void dbinform(const char *message, void *opaque){
  printfinfo("%s", message);
}


/* set signal handlers */
static void setsignals(void){
  signal(1, sigtermhandler);
  signal(2, sigtermhandler);
  signal(3, sigtermhandler);
  signal(13, sigtermhandler);
  signal(15, sigtermhandler);
}


/* handler of termination signal */
static void sigtermhandler(int num){
  static int tries = 0;
  if(tries++ <= 4){
    signal(num, sigtermhandler);
  } else {
    signal(num, SIG_DFL);
  }
  g_sigterm = TRUE;
  printfinfo("the termination signal %d catched", num);
}


/* print the usage and exit */
static void usage(void){
  fprintf(stderr, "%s: command line utility of Hyper Estraier\n", g_progname);
  fprintf(stderr, "\n");
  fprintf(stderr, "usage:\n");
  fprintf(stderr, "  %s wicked db dnum tnum\n", g_progname);
  fprintf(stderr, "  %s regression db\n", g_progname);
  fprintf(stderr, "\n");
  exit(1);
}


/* parse arguments of the wicked command */
static int runwicked(int argc, char **argv){
  char *dbname, *dnstr, *tnstr;
  int i, dnum, tnum, rv;
  dbname = NULL;
  dnstr = NULL;
  tnstr = NULL;
  for(i = 2; i < argc; i++){
    if(!dbname && argv[i][0] == '-'){
      usage();
    } else if(!dbname){
      dbname = argv[i];
    } else if(!dnstr){
      dnstr = argv[i];
    } else if(!tnstr){
      tnstr = argv[i];
    } else {
      usage();
    }
  }
  if(!dbname || !dnstr || !tnstr) usage();
  if((dnum = atoi(dnstr)) < 1) usage();
  if((tnum = atoi(tnstr)) < 1) usage();
  rv = procwicked(dbname, dnum, tnum);
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


/* perform the wicked command */
static int procwicked(const char *dbname, int dnum, int tnum){
  ESTMTDB *dbs[BRANCHDBNUM];
  MISSION *missions;
  pthread_t *threads;
  void *rv;
  char *name;
  int i, ecode, err;
  time_t curtime;
  curtime = time(NULL);
  for(i = 0; i < BRANCHDBNUM; i++){
    name = cbsprintf("%s-%08d", dbname, i + 1);
    if(!(dbs[i] = est_mtdb_open(name, ESTDBWRITER | ESTDBCREAT | ESTDBTRUNC, &ecode))){
      printferror("%s: %s", name, est_err_msg(ecode));
      while(i >= 0){
        est_mtdb_close(dbs[i], &ecode);
        i--;
      }
      free(name);
      return -1;
    }
    est_mtdb_set_informer(dbs[i], dbinform, NULL);
    est_mtdb_set_cache_size(dbs[i], 1024 * 1024 * 32, 1024, 256, 64);
    est_mtdb_set_special_cache(dbs[i], ESTDATTRURI, 128);
    free(name);
  }
  missions = cbmalloc(sizeof(MISSION) * tnum);
  threads = cbmalloc(sizeof(pthread_t) * tnum);
  err = FALSE;
  if(tnum == 1){
    missions[0].id = 1;
    missions[0].db = dbs[0];
    missions[0].others = dbs;
    missions[0].dnum = dnum;
    if(mtwkfunc(missions) != NULL) err = TRUE;
  } else {
    for(i = 0; i < tnum; i++){
      missions[i].id = i + 1;
      missions[i].db = dbs[i%BRANCHDBNUM];
      missions[i].others = dbs;
      missions[i].dnum = dnum;
      if(pthread_create(threads + i, NULL, mtwkfunc, missions + i) != 0){
        printferror("%d: pthread_create failed", i + 1);
        missions[i].id = -1;
        err = TRUE;
      }
    }
    for(i = 0; i < tnum; i++){
      if(missions[i].id == -1) continue;
      if(pthread_join(threads[i], &rv) != 0){
        printferror("%s: pthread_join failed", missions[i].id);
        err = TRUE;
      } else if(rv){
        err = TRUE;
      }
    }
  }
  for(i = 0; i < BRANCHDBNUM; i++){
    if(!est_mtdb_close(dbs[i], &ecode)){
      printferror("%s: %s", dbname, est_err_msg(ecode));
      err = TRUE;
    }
  }
  free(threads);
  free(missions);
  curtime = time(NULL) - curtime;
  if(!err) printfinfo("finished successfully: elapsed time: %dh %dm %ds",
                      (int)(curtime / 3600), (int)((curtime / 60) % 60), (int)(curtime % 60));
  return err ? 1 : 0;
}


/* perform the regression command */
static int procregression(const char *dbname){
  ESTMTDB *dbs[10], *db, *tdbs[5];
  ESTDOC *doc;
  ESTCOND *cond;
  CBMAP *hints;
  char wbuf[URIBUFSIZ];
  int i, j, ecode, err, *res, rnum;
  time_t curtime;
  curtime = time(NULL);
  printfinfo("# opening databases");
  for(i = 0; i < 10; i++){
    sprintf(wbuf, "%s-%08d", dbname, i + 1);
    if(!(dbs[i] = est_mtdb_open(wbuf, ESTDBWRITER | ESTDBCREAT | ESTDBTRUNC, &ecode))){
      printferror("%s: %s", wbuf, est_err_msg(est_mtdb_error(dbs[i])));
      return 1;
    }
    est_mtdb_set_informer(dbs[i], dbinform, NULL);
  }
  err = FALSE;
  printfinfo("# registering documents");
  for(i = 0; i < 1000 && !g_sigterm; i++){
    doc = est_doc_new();
    sprintf(wbuf, "%d", rand() % (i + 100));
    est_doc_add_attr(doc, ESTDATTRURI, wbuf);
    for(j = 0; j < 100; j++){
      sprintf(wbuf, "%c%c", 'a' + rand() % 6, 'a' + rand() % 6);
      est_doc_add_text(doc, wbuf);
    }
    db = dbs[rand()%10];
    if(!est_mtdb_put_doc(db, doc, ESTPDCLEAN)){
      printferror("%s: %s", est_mtdb_name(db), est_err_msg(ecode));
      err = TRUE;
    }
    est_doc_delete(doc);
  }
  printfinfo("# flushing half of databases");
  for(i = 0; i < 10; i += 2){
    if(!est_mtdb_flush(dbs[i], -1)){
      printferror("%s: %s", est_mtdb_name(dbs[i]), est_err_msg(ecode));
      err = TRUE;
    }
  }
  printfinfo("# searching databases");
  for(i = 0; i < 1000 && !g_sigterm; i++){
    cond = est_cond_new();
    sprintf(wbuf, "%c%c", 'a' + rand() % 6, 'a' + rand() % 6);
    est_cond_set_phrase(cond, wbuf);
    switch(rand() % 5){
    case 0:
      est_cond_set_order(cond, ESTORDIDA);
      break;
    case 1:
      est_cond_set_order(cond, ESTORDSCA);
      break;
    case 2:
      est_cond_set_order(cond, ESTDATTRURI " " ESTORDSTRA);
      break;
    }
    hints = cbmapopen();
    res = est_mtdb_search_meta(dbs, 10, cond, &rnum, hints);
    for(j = 0; j < rnum; j += 2){
      if(res[j] < 0 || res[j] >= 10){
        printferror("%s: search result is invalid", dbname);
        err = TRUE;
        break;
      }
    }
    free(res);
    cbmapclose(hints);
    est_cond_delete(cond);
  }
  printfinfo("# searching certainly same databases");
  tdbs[0] = dbs[0];
  tdbs[1] = dbs[1];
  tdbs[2] = dbs[2];
  tdbs[3] = dbs[0];
  tdbs[4] = dbs[1];
  for(i = 0; i < 100 && !g_sigterm; i++){
    cond = est_cond_new();
    sprintf(wbuf, "%c%c", 'a' + rand() % 6, 'a' + rand() % 6);
    est_cond_set_phrase(cond, wbuf);
    switch(rand() % 5){
    case 0:
      est_cond_set_order(cond, ESTORDIDA);
      break;
    case 1:
      est_cond_set_order(cond, ESTORDSCA);
      break;
    case 2:
      est_cond_set_order(cond, ESTDATTRURI " " ESTORDSTRA);
      break;
    }
    hints = cbmapopen();
    res = est_mtdb_search_meta(tdbs, 5, cond, &rnum, hints);
    for(j = 0; j < rnum; j += 2){
      if(res[j] < 0 || res[j] >= 5){
        printferror("%s: search result is invalid", dbname);
        err = TRUE;
        break;
      }
    }
    free(res);
    cbmapclose(hints);
    est_cond_delete(cond);
  }
  printfinfo("# closing databases");
  for(i = 0; i < 10; i++){
    if(!est_mtdb_close(dbs[i], &ecode)){
      printferror("%s: %s", est_mtdb_name(dbs[i]), est_err_msg(ecode));
      err = TRUE;
    }
  }
  curtime = time(NULL) - curtime;
  if(!err) printfinfo("# finished successfully: elapsed time: %dh %dm %ds",
                      (int)(curtime / 3600), (int)((curtime / 60) % 60), (int)(curtime % 60));
  return err ? 1 : 0;
}


/* Thread function for wicked command */
static void *mtwkfunc(void *mission){
  ESTMTDB *db, **others;
  ESTDOC *doc;
  ESTCOND *cond;
  char uri[URIBUFSIZ], *oper, *value, *first, *second, *phrase;
  int i, j, err, id, dnum, *res, rnum, size;
  double rnd;
  id = ((MISSION *)mission)->id;
  db = ((MISSION *)mission)->db;
  others = ((MISSION *)mission)->others;
  dnum = ((MISSION *)mission)->dnum;
  printfinfo("%d: started", id);
  err = FALSE;
  for(i = 0; i < dnum; i++){
    rnd = est_random();
    if((int)(rnd * INT_MAX) % dnum < 1){
      rnd = est_random();
      if(rnd < 0.3){
        if(!est_mtdb_optimize(db, (int)(est_random() * INT_MAX) % 2 == 0) ? ESTOPTNOPURGE : 0)
          err = TRUE;
      } else if(rnd < 0.8){
        if(!est_mtdb_flush(db, 1024)) err = TRUE;
      } else {
        if(!est_mtdb_sync(db)) err = TRUE;
      }
    } else if(rnd < 0.001){
      est_usleep(1000 * 100);
      printfinfo("%d: [%d:%d]: sleep", id, i + 1, est_mtdb_doc_num(db));
    } else if(rnd < 0.005){
      for(j = 0; j < BRANCHDBNUM; j++){
        est_mtdb_interrupt(others[j]);
      }
      printfinfo("%d: [%d:%d]: interrupt", id, i + 1, est_mtdb_doc_num(db));
    } else if(rnd < 0.05){
      if(est_mtdb_out_doc(db, (int)(est_random() * INT_MAX) % (i + 1) + 1,
                          ((int)(est_random() * INT_MAX) % 2 == 0) ? ESTODCLEAN : 0)){
        printfinfo("%d: [%d:%d]: out", id, i + 1, est_mtdb_doc_num(db));
      } else if(est_mtdb_error(db) != ESTENOITEM){
        err = TRUE;
      }
    } else if(rnd < 0.1){
      if((value = est_mtdb_get_doc_attr(db, (int)(est_random() * INT_MAX) % (i + 1) + 1,
                                        ESTDATTRURI)) != NULL){
        printfinfo("[%d:%d]: attr: %s", i + 1, est_mtdb_doc_num(db), value);
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
      if(est_random() < 0.05) est_cond_add_attr(cond, "@uri STREW 0.est");
      if(est_random() < 0.05) est_cond_set_order(cond, "@uri STRD");
      res = est_mtdb_search(db, cond, &rnum, NULL);
      printfinfo("%d: [%d:%d]: search: %d hits", id, i + 1, est_mtdb_doc_num(db), rnum);
      if(est_random() < 0.01){
        for(j = 0; j < rnum && j < 100; j++){
          if((doc = est_mtdb_get_doc(db, res[j], 0)) != NULL){
            est_doc_delete(doc);
          } else if(est_mtdb_error(db) != ESTENOITEM){
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
      if(!est_mtdb_put_doc(db, doc, est_random() < 0.5 ? ESTPDCLEAN : 0)) err = TRUE;
      if(est_random() < 0.1){
        if(!est_mtdb_set_doc_entity(db, est_doc_id(doc), uri, strlen(uri))) err = TRUE;
        value = est_mtdb_get_doc_entity(db, est_doc_id(doc), &size);
        if(!value || strcmp(value, uri)) err = TRUE;
        free(value);
      }
      est_doc_delete(doc);
    }
    if(err || g_sigterm) break;
  }
  if(err) printferror("%s: %s", est_mtdb_name(db), est_err_msg(est_mtdb_error(db)));
  printfinfo("%d: finished", id);
  return err ? "error" : NULL;
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
  buf = cbdatumopen("", 0);
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
    cbdatumcat(buf, wc, 2);
  }
  str = est_iconv(CB_DATUMPTR(buf), CB_DATUMSIZE(buf), "UTF-16BE", "UTF-8", NULL, NULL);
  cbdatumclose(buf);
  return str;
}



/* END OF FILE */
