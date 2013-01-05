/*************************************************************************************************
 * The command line interface of web crawler
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


#include "wavermod.h"

#define SLEEPUSEC      100000            /* sleep time in micro seconds */
#define MINPRIOR       0.01              /* minimal priority of non-seed documents */
#define NODERTTNUM     5000              /* number of documents for node rotation */

enum {                                   /* enumeration for crawling modes */
  CM_CONTINUE,                           /* continue */
  CM_RESTART,                            /* restart */
  CM_REVISIT,                            /* revisit */
  CM_REVCONT                             /* revisit and continue */
};

typedef struct {                         /* type of structure for interaction of a URL */
  int thid;                              /* thread ID number */
  WAVER *waver;                          /* waver handle */
  char *url;                             /* URL */
  int depth;                             /* depth */
  int pid;                               /* ID number of the parent document */
  double psim;                           /* similarity of the parent document */
  time_t mdate;                          /* last-modified date */
} TARGSURL;


/* global variables */
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;  /* global mutex */
int g_thnum = 0;                         /* number of running threads */
int g_thseq = 0;                         /* sequence of thread ID */
CBMAP *g_tasks = NULL;                   /* URLs of running tasks */
const char *g_progname;                  /* program name */
int g_sigterm = FALSE;                   /* flag for termination signal */
const char *g_pxhost = NULL;             /* host name of the proxy */
int g_pxport = 0;                        /* host name of the proxy */
int g_timeout = -1;                      /* timeout in seconds */
int g_inputlang = ESTLANGEN;             /* prefered language */


/* function prototypes */
int main(int argc, char **argv);
static void usage(void);
static void setsignals(void);
static void sigtermhandler(int num);
static char *dequeue(WAVER *waver, int *depthp, int *pidp, double *psimp);
static void enqueuelinks(WAVER *waver, const char *base, CBLIST *links, CBMAP *kwords,
                         int depth, int id, int pid, double psim);
static int runinit(int argc, char **argv);
static int runcrawl(int argc, char **argv);
static int rununittest(int argc, char **argv);
static int runfetch(int argc, char **argv);
static int procinit(const char *rootdir, int opts);
static int proccrawl(const char *rootdir, int mode);
static int procunittest(const char *rootdir);
static int procfetch(const char *url);
static int strtolang(const char *str);
static char *capitalize(const char *str);
static void seedurldocs(WAVER *waver, const char *url, int depth, double bias, CBMAP *ulinks);
static int accessthnum(int inc);
static int puttask(const char *url);
static void outtask(const char *url);
static void *geturldoc(void *args);
static char *urltosavepath(const char *savedir, const char *url);


/* main routine */
int main(int argc, char **argv){
  const char *tmp;
  int rv;
  if((tmp = getenv("ESTDBGFD")) != NULL) dpdbgfd = atoi(tmp);
  est_proc_env_reset();
  g_progname = argv[0];
  g_sigterm = FALSE;
  g_tasks = cbmapopenex(MINIBNUM);
  cbglobalgc(g_tasks, (void (*)(void *))cbmapclose);
  if(!est_init_net_env()){
    log_print(LL_ERROR, "could not initialize network environment");
    exit(1);
  }
  atexit(est_free_net_env);
  if(argc < 2) usage();
  rv = 0;
  if(!strcmp(argv[1], "init")){
    rv = runinit(argc, argv);
  } else if(!strcmp(argv[1], "crawl")){
    setsignals();
    rv = runcrawl(argc, argv);
  } else if(!strcmp(argv[1], "unittest")){
    rv = rununittest(argc, argv);
  } else if(!strcmp(argv[1], "fetch")){
    rv = runfetch(argc, argv);
  } else {
    usage();
  }
  return rv;
}


/* print the usage and exit */
static void usage(void){
  fprintf(stderr, "%s: command line interface of web crawler\n", g_progname);
  fprintf(stderr, "\n");
  fprintf(stderr, "usage:\n");
  fprintf(stderr, "  %s init [-apn|-acc] [-xs|-xl|-xh] [-sv|-si|-sa] rootdir\n", g_progname);
  fprintf(stderr, "  %s crawl [-restart|-revisit|-revcont] rootdir\n", g_progname);
  fprintf(stderr, "  %s unittest rootdir\n", g_progname);
  fprintf(stderr, "  %s fetch [-proxy host port] [-tout num] [-il lang] url\n", g_progname);
  fprintf(stderr, "\n");
  exit(1);
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
  fprintf(stderr, "%s: the termination signal %d catched\n", g_progname, num);
}


/* dequeue a URL from the priority queue (it has a critical section inside) */
static char *dequeue(WAVER *waver, int *depthp, int *pidp, double *psimp){
  char *buf, *rp, *rv;
  if(time(NULL) > waver->stime + waver->period) return NULL;
  if(pthread_mutex_lock(&g_mutex) != 0){
    log_print(LL_ERROR, "could not get mutex");
    return NULL;
  }
  if(waver->curnum > waver->docnum){
    pthread_mutex_unlock(&g_mutex);
    return NULL;
  }
  while(queue_rnum(waver->queue) < 1 && g_thnum > 0){
    pthread_mutex_unlock(&g_mutex);
    est_usleep(SLEEPUSEC);
    if(pthread_mutex_lock(&g_mutex) != 0){
      log_print(LL_ERROR, "could not get mutex");
      return NULL;
    }
  }
  rv = NULL;
  *depthp = 0;
  *pidp = 0;
  *psimp = 0.0;
  while(TRUE){
    if(!(buf = queue_dequeue(waver->queue))) break;
    if((rp = strchr(buf, '\t')) != NULL && (*depthp = atoi(buf)) >= 0){
      rp++;
      rv = cbmemdup(rp, -1);
      if((rp = strchr(buf, ':')) != NULL){
        rp++;
        *pidp = atoi(rp);
        if((rp = strchr(rp, ':')) != NULL){
          rp++;
          *psimp = strtod(rp, NULL);
        }
      }
      free(buf);
      break;
    }
    free(buf);
  }
  pthread_mutex_unlock(&g_mutex);
  return rv;
}


/* add traced URLs (it should be called in a critical section) */
static void enqueuelinks(WAVER *waver, const char *base, CBLIST *links, CBMAP *kwords,
                         int depth, int id, int pid, double psim){
  PMRULE *pmrule;
  CBMAP *pkwords, *ulinks;
  const char *vbuf;
  char numbuf[NUMBUFSIZ], *ubuf, *pv, *benc, *tenc;
  int i, j, vsiz, lnum, llen, slash, vnum, *svec, *tvec, num, allow;
  double similarity, lnumtune, depthtune, masstune, priority, remoteness;
  if(depth >= waver->maxdepth) return;
  if(!kwords || cbmaprnum(waver->kwords) < 1){
    similarity = psim * 0.7;
    psim = similarity;
  } else {
    vnum = waver->seedkeynum;
    svec = cbmalloc(vnum * sizeof(int));
    tvec = cbmalloc(vnum * sizeof(int));
    est_vector_set_seed(waver->kwords, svec, vnum);
    est_vector_set_target(waver->kwords, kwords, tvec, vnum);
    similarity = est_vector_cosine(svec, tvec, vnum) * 0.9 + 0.1;
    similarity = similarity * (1.0 - waver->inherit) + psim * waver->inherit;
    psim = similarity;
    free(tvec);
    free(svec);
    if(pid > 0 && (pkwords = est_mtdb_get_keywords(waver->index, pid))){
      vnum = waver->savekeynum;
      svec = cbmalloc(vnum * sizeof(int));
      tvec = cbmalloc(vnum * sizeof(int));
      est_vector_set_seed(pkwords, svec, vnum);
      est_vector_set_target(pkwords, kwords, tvec, vnum);
      similarity *= 1.0 - pow(est_vector_cosine(svec, tvec, vnum), 3.14) * 0.8;
      cbmapclose(pkwords);
      free(tvec);
      free(svec);
    }
  }
  if((pv = strstr(base, "://")) != NULL && (pv = strchr(pv + 3, '/')) != NULL){
    vbuf = cbmapget(waver->sites, base, pv - base + 1, NULL);
    num = (vbuf ? atoi(vbuf) : 0) + 1;
    sprintf(numbuf, "%d", num);
    cbmapput(waver->sites, base, pv - base + 1, numbuf, -1, TRUE);
    if(cbmaprnum(waver->sites) > (waver->queuesize / 3.0 + 1) * 1.4){
      log_print(LL_INFO, "site map sliming: %d", cbmaprnum(waver->sites));
      kwords_reduce(waver->sites, waver->queuesize / 3.0 + 1, TRUE);
    }
  }
  ulinks = cbmapopenex(MINIBNUM);
  cbmapput(ulinks, base, -1, "", 0, FALSE);
  lnum = cblistnum(links) + 4;
  lnumtune = pow(lnum, 0.7);
  depthtune = pow(depth + 7, 0.8);
  for(i = 0; i < cblistnum(links) && i < 1024; i++){
    vbuf = cblistval(links, i, &vsiz);
    ubuf = cbmemdup(vbuf, vsiz);
    if((pv = strchr(ubuf, '#')) != NULL) *pv = '\0';
    llen = strlen(ubuf);
    if(llen > 1024 || cbmapget(ulinks, ubuf, llen, NULL)){
      free(ubuf);
      continue;
    }
    cbmapput(ulinks, ubuf, -1, "", 0, FALSE);
    allow = FALSE;
    for(j = 0; j < cblistnum(waver->pmrules); j++){
      pmrule = (PMRULE *)cblistval(waver->pmrules, j, NULL);
      switch(pmrule->visit){
      case 1:
        if(est_regex_match(pmrule->regex, ubuf)) allow = TRUE;
        break;
      case -1:
        if(est_regex_match(pmrule->regex, ubuf)) allow = FALSE;
        break;
      }
    }
    if(!allow){
      free(ubuf);
      continue;
    }
    masstune = 1.0;
    if((pv = strstr(ubuf, "://")) != NULL && (pv = strchr(pv + 3, '/')) != NULL){
      vbuf = cbmapget(waver->sites, ubuf, pv - ubuf + 1, NULL);
      num = (vbuf ? atoi(vbuf) : 0) + 1;
      if(num > waver->masscheck) masstune /= sqrt((double)num / waver->masscheck);
    }
    slash = 6;
    for(pv = ubuf; *pv != '\0'; pv++){
      switch(*pv){
      case '/': slash += 1; break;
      case '?': slash += 5; break;
      case '&': slash += 1; break;
      case ';': slash += 1; break;
      }
    }
    remoteness = 8.0;
    benc = cbmemdup(base, -1);
    tenc = cbmemdup(ubuf, -1);
    if((pv = strchr(benc, '?')) != NULL) pv[0] = '\0';
    if((pv = strchr(tenc, '?')) != NULL) pv[0] = '\0';
    if(!strcmp(tenc, benc)){
      priority *= 0.7;
      remoteness = 1.5;
    } else {
      if((pv = strrchr(benc, '/')) != NULL) pv[1] = '\0';
      if((pv = strrchr(tenc, '/')) != NULL) pv[1] = '\0';
      if(cbstrfwmatch(tenc, benc)){
        priority *= 0.9;
        remoteness = 4.0;
      }
    }
    free(tenc);
    free(benc);
    switch(waver->strategy){
    case CS_BALANCED:
      priority = (similarity * 128 * masstune) / depthtune / lnumtune / slash;
      priority *= (lnum - (i / remoteness)) / lnum;
      if(llen > 80) priority /= pow(llen / 80.0, 0.7);
      break;
    case CS_SIMILARITY:
      priority = similarity;
      priority *= 0.9 + ((lnum - i) / (double)lnum) / 50;
      break;
    case CS_DEPTH:
      priority = (depth + 1) * 0.001;
      priority = priority > 1.0 ? 1.0 : priority;
      priority *= 0.9 + ((lnum - i) / (double)lnum) / 50;
      break;
    case CS_WIDTH:
      priority = 1.0 / (depth + 1);
      priority *= 0.9 + ((lnum - i) / (double)lnum) / 50;
      break;
    case CS_RANDOM:
      priority = (est_random() * 128 * masstune) / depthtune / lnumtune / slash;
      priority *= (lnum - (i / remoteness)) / lnum;
      if(llen > 80) priority /= pow(llen / 80.0, 0.7);
      break;
    default:
      priority = (128 * masstune) / depthtune / lnumtune / slash;
      priority *= (lnum - (i / remoteness)) / lnum;
      if(llen > 80) priority /= pow(llen / 80.0, 0.7);
      break;
    }
    tenc = cbsprintf("%d:%d:%.5f\t%s", depth + 1, id, psim, ubuf);
    queue_enqueue(waver->queue, tenc, 1.0 - priority);
    free(tenc);
    free(ubuf);
  }
  cbmapclose(ulinks);
  if(queue_rnum(waver->queue) > waver->queuesize * 1.4){
    log_print(LL_INFO, "queue sliming: %d", queue_rnum(waver->queue));
    if(!queue_slim(waver->queue, waver->queuesize))
      log_print(LL_ERROR, "queue sliming failed");
  }
}


/* parse arguments of the init command */
static int runinit(int argc, char **argv){
  char *rootdir;
  int i, opts, rv;
  rootdir = NULL;
  opts = 0;
  for(i = 2; i < argc; i++){
    if(!rootdir && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-apn")){
        opts |= WI_PERFNG;
      } else if(!strcmp(argv[i], "-acc")){
        opts |= WI_CHRCAT;
      } else if(!strcmp(argv[i], "-xs")){
        opts |= WI_SMALL;
      } else if(!strcmp(argv[i], "-xl")){
        opts |= WI_LARGE;
      } else if(!strcmp(argv[i], "-xh")){
        opts |= WI_HUGE;
      } else if(!strcmp(argv[i], "-sv")){
        opts |= WI_SCVOID;
      } else if(!strcmp(argv[i], "-si")){
        opts |= WI_SCINT;
      } else if(!strcmp(argv[i], "-sa")){
        opts |= WI_SCASIS;
      } else {
        usage();
      }
    } else if(!rootdir){
      rootdir = argv[i];
    } else {
      usage();
    }
  }
  if(!rootdir) usage();
  rv = procinit(rootdir, opts);
  return rv;
}


/* parse arguments of the crawl command */
static int runcrawl(int argc, char **argv){
  char *rootdir;
  int i, mode, rv;
  rootdir = NULL;
  mode = CM_CONTINUE;
  for(i = 2; i < argc; i++){
    if(!rootdir && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-restart")){
        mode = CM_RESTART;
      } else if(!strcmp(argv[i], "-revisit")){
        mode = CM_REVISIT;
      } else if(!strcmp(argv[i], "-revcont")){
        mode = CM_REVCONT;
      } else {
        usage();
      }
    } else if(!rootdir){
      rootdir = argv[i];
    } else {
      usage();
    }
  }
  if(!rootdir) usage();
  rv = proccrawl(rootdir, mode);
  return rv;
}


/* parse arguments of the unittest command */
static int rununittest(int argc, char **argv){
  char *rootdir;
  int i, rv;
  rootdir = NULL;
  for(i = 2; i < argc; i++){
    if(!rootdir && argv[i][0] == '-'){
      usage();
    } else if(!rootdir){
      rootdir = argv[i];
    } else {
      usage();
    }
  }
  if(!rootdir) usage();
  rv = procunittest(rootdir);
  return rv;
}


/* parse arguments of the fetch command */
static int runfetch(int argc, char **argv){
  char *url;
  int i, rv;
  url = NULL;
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
      } else if(!strcmp(argv[i], "-il")){
        if(++i >= argc) usage();
        g_inputlang = strtolang(argv[i]);
      } else {
        usage();
      }
    } else if(!url){
      url = argv[i];
    } else {
      usage();
    }
  }
  if(!url) usage();
  rv = procfetch(url);
  return rv;
}


/* perform the init command */
static int procinit(const char *rootdir, int opts){
  if(!waver_init(rootdir, opts)){
    log_print(LL_ERROR, "initializing the root directory failed");
    return 1;
  }
  log_open(rootdir, LOGFILE, LL_INFO, FALSE);
  log_print(LL_INFO, "the root directory created");
  return 0;
}


/* perform the init command */
static int proccrawl(const char *rootdir, int mode){
  pthread_t th;
  TARGSURL *targs;
  WAVER *waver;
  CBDATUM *kwbuf;
  CBMAP *ulinks;
  const char *kbuf, *rp;
  char *url, *rec, *tmp, *endurl;
  int i, err, depth, pid, thid, locked, ended;
  double psim;
  time_t t, mdate;
  if(!(waver = waver_open(rootdir))){
    log_print(LL_ERROR, "%s: could not open", rootdir);
    return 1;
  }
  err = FALSE;
  switch(mode){
  default:
    log_print(LL_INFO, "crawling started (continue)");
    break;
  case CM_RESTART:
    log_print(LL_INFO, "crawling started (restart)");
    break;
  case CM_REVISIT:
    log_print(LL_INFO, "crawling started (revisit)");
    break;
  case CM_REVCONT:
    log_print(LL_INFO, "crawling started (revcont)");
    break;
  }
  if(mode == CM_RESTART){
    while((tmp = queue_dequeue(waver->queue)) != NULL){
      free(tmp);
    }
  }
  endurl = NULL;
  if(mode == CM_REVISIT || mode == CM_REVCONT){
    t = time(NULL);
    criterinit(waver->trace);
    while((url = criternext(waver->trace, NULL)) != NULL){
      if((rec = crget(waver->trace, url, -1, 0, -1, NULL)) != NULL){
        if(est_mtdb_uri_to_id(waver->index, url) > 0){
          mdate = (time_t)strtod(rec, NULL);
          depth = 0;
          pid = 0;
          psim = 0.0;
          if((rp = strchr(rec, ':')) != NULL){
            rp++;
            depth = atoi(rp);
            if((rp = strchr(rp, ':')) != NULL){
              rp++;
              pid = atoi(rp);
              if((rp = strchr(rp, ':')) != NULL){
                rp++;
                psim = strtod(rp, NULL);
              }
            }
          }
          tmp = cbsprintf("%d:%d:%.5f\t%s", depth, pid, psim, url);
          queue_enqueue(waver->queue, tmp, (mdate / t) * MINPRIOR);
          free(tmp);
        }
        free(rec);
      }
      free(endurl);
      endurl = url;
    }
  }
  if(mode == CM_RESTART){
    criterinit(waver->trace);
    while((url = criternext(waver->trace, NULL)) != NULL){
      crout(waver->trace, url, -1);
      free(url);
    }
  }
  cbmapiterinit(waver->seeds);
  ulinks = cbmapopen();
  while((kbuf = cbmapiternext(waver->seeds, NULL)) != NULL){
    seedurldocs(waver, kbuf, 0, strtod(cbmapget(waver->seeds, kbuf, -1, NULL), NULL), ulinks);
  }
  cbmapclose(ulinks);
  kwords_reduce(waver->kwords, waver->seedkeynum, FALSE);
  kwbuf = cbdatumopen(NULL, -1);
  cbmapiterinit(waver->kwords);
  for(i = 0; (kbuf = cbmapiternext(waver->kwords, NULL)) != NULL; i++){
    if(i > 0) cbdatumprintf(kwbuf, ", ");
    cbdatumprintf(kwbuf, "%s (%s)", kbuf, cbmapget(waver->kwords, kbuf, -1, NULL));
  }
  log_print(LL_DEBUG, "seed keywords: %s", cbdatumptr(kwbuf));
  cbdatumclose(kwbuf);
  ended = FALSE;
  thid = 0;
  while(!g_sigterm && !ended && (tmp = dequeue(waver, &depth, &pid, &psim)) != NULL){
    if(endurl && !strcmp(tmp, endurl)){
      est_usleep(SLEEPUSEC);
      if(mode == CM_REVISIT){
        ended = TRUE;
      } else {
        log_print(LL_INFO, "waiting for threads: %d", accessthnum(0));
        t = time(NULL);
        while(accessthnum(0) > 0 && time(NULL) < t + waver->timeout * 2 + 1){
          est_usleep(SLEEPUSEC);
        }
        free(endurl);
        endurl = NULL;
      }
    }
    mdate = 0;
    if(pthread_mutex_lock(&g_mutex) == 0){
      if((rec = crget(waver->trace, tmp, -1, 0, -1, NULL)) != NULL){
        mdate = (time_t)strtod(rec, NULL);
        free(rec);
      }
      pthread_mutex_unlock(&g_mutex);
      if(mdate + waver->revisit >= time(NULL)){
        log_print(LL_DEBUG, "not modified: %s", tmp);
        free(tmp);
        continue;
      }
    } else {
      log_print(LL_ERROR, "could not get mutex");
    }
    if(cbmaprnum(waver->nodes) > 0 &&
       (waver->curnode < 1 || waver->minload >= 1.0 || thid % NODERTTNUM == 0 ||
        (thid % (NODERTTNUM / 10) == 0 && waver_current_node_load(waver) > 0.85))){
      waver_set_current_node(waver);
      log_print(LL_INFO, "current node changed: %d: %f", waver->curnode, waver->minload);
    }
    thid++;
    targs = cbmalloc(sizeof(TARGSURL));
    targs->thid = thid;
    targs->waver = waver;
    targs->url = tmp;
    targs->depth = depth;
    targs->pid = pid;
    targs->psim = psim;
    targs->mdate = mdate;
    if(waver->thnum > 1){
      while(accessthnum(0) >= waver->thnum){
        est_usleep(SLEEPUSEC);
      }
      if(pthread_create(&th, NULL, geturldoc, targs) == 0){
        pthread_detach(th);
        if(thid <= waver->thnum) est_usleep(SLEEPUSEC);
      } else {
        geturldoc(targs);
      }
    } else {
      geturldoc(targs);
    }
    if(thid % 256 == 0){
      locked = pthread_mutex_lock(&g_mutex) == 0;
      log_print(LL_INFO, "status: dnum=%d, wnum=%d, size=%.0f, queue=%d",
                est_mtdb_doc_num(waver->index), est_mtdb_word_num(waver->index),
                est_mtdb_size(waver->index), queue_rnum(waver->queue) + 1);
      if(locked) pthread_mutex_unlock(&g_mutex);
      est_usleep(SLEEPUSEC);
    }
  }
  est_usleep(SLEEPUSEC);
  if(waver->thnum > 1){
    log_print(LL_INFO, "waiting for threads: %d", accessthnum(0));
    t = time(NULL);
    while(accessthnum(0) > 0){
      if(time(NULL) > t + waver->timeout * 8 + 60){
        log_print(LL_WARN, "thread waiting timed out: %d", accessthnum(0));
        raise(3);
        est_usleep(1000 * 1000 * 5);
        break;
      }
      est_usleep(SLEEPUSEC);
    }
  }
  free(endurl);
  log_print(LL_INFO, "crawling finished");
  locked = pthread_mutex_lock(&g_mutex) == 0;
  g_sigterm = TRUE;
  if(!waver_close(waver)){
    log_print(LL_ERROR, "%s: closing failed", rootdir);
    err = TRUE;
  }
  if(locked) pthread_mutex_unlock(&g_mutex);
  if(!err) log_print(LL_INFO, "finished successfully");
  return err ? 1 : 0;
}


/* perform the unittest command */
static int procunittest(const char *rootdir){
  WAVER *waver;
  QUEUE *queue;
  CBMAP *seeds, *kwords;
  char uri[URIBUFSIZ], *vbuf;
  int i, err;
  log_print(LL_INFO, "initializing the waver handle");
  if(!waver_init(rootdir, 0)){
    log_print(LL_ERROR, "%s: initializing failed", rootdir);
    return FALSE;
  }
  log_print(LL_INFO, "opening the waver handle");
  if(!(waver = waver_open(rootdir))){
    log_print(LL_ERROR, "%s: opening failed", rootdir);
    return FALSE;
  }
  err = FALSE;
  log_print(LL_INFO, "checking seeding");
  seeds = waver->seeds;
  for(i = 0; i < 100; i++){
    sprintf(uri, "http://%05d/%x/%x.html",
            i + 1, (int)(est_random() * 0x1000000), (int)(est_random() * 0x1000000));
    cbmapput(seeds, uri, -1, "", 0, TRUE);
  }
  log_print(LL_INFO, "checking priority queue");
  queue = waver->queue;
  for(i = 0; i < 100; i++){
    sprintf(uri, "0:0:0.0\thttp://%05d/%x/%x.html",
            i + 1, (int)(est_random() * 0x1000000), (int)(est_random() * 0x1000000));
    if(!queue_enqueue(queue, uri, est_random())){
      err = TRUE;
      break;
    }
    if(i % 10 == 0) queue_set_range(queue, i);
  }
  if(queue_rnum(queue) != 100) err = TRUE;
  if(err) log_print(LL_ERROR, "%s: enqueue failed", rootdir);
  if(!queue_slim(queue, 60)) err = TRUE;
  if(err) log_print(LL_ERROR, "%s: slim failed", rootdir);
  for(i = 0; (vbuf = queue_dequeue(queue)) != NULL; i++){
    free(vbuf);
  }
  if(i != 60){
    err = TRUE;
    log_print(LL_ERROR, "%s: dequeue failed", rootdir);
  }
  log_print(LL_INFO, "checking keyword map");
  kwords = waver->kwords;
  for(i = 0; i < 10000; i++){
    sprintf(uri, "%d", (int)(est_random() * 1000));
    kwords_add(kwords, uri, (int)(est_random() * 1000));
  }
  kwords_reduce(kwords, 100, TRUE);
  log_print(LL_INFO, "closing the waver handle");
  if(!waver_close(waver)){
    log_print(LL_ERROR, "%s: closing failed", rootdir);
    err = TRUE;
  }
  if(!err) log_print(LL_INFO, "finished successfully");
  return err ? 1 : 0;
}


/* perform the fetch command */
static int procfetch(const char *url){
  CBMAP *heads;
  CBLIST *links;
  CBDATUM *raw;
  ESTDOC *doc;
  const char *border, *vbuf;
  char *str;
  int i, code, vsiz;
  raw = cbdatumopen(NULL, -1);
  heads = cbmapopen();
  links = cblistopen();
  doc = est_doc_new();
  if(!fetch_document(url, g_pxhost, g_pxport, g_timeout, -1, NULL, NULL, &code, raw, heads,
                     links, NULL, doc, g_inputlang)){
    log_print(LL_WARN, "could not get: %d: %s", code, url);
    est_doc_delete(doc);
    cblistclose(links);
    cbmapclose(heads);
    cbdatumclose(raw);
    return 1;
  }
  border = est_border_str();
  printf("URL: %s\r\n", url);
  str = cbdatestrhttp(time(NULL), 0);
  printf("Date: %s\r\n", str);
  free(str);
  str = cbmimeencode((vbuf = est_doc_attr(doc, ESTDATTRTITLE)) ? vbuf : url, "UTF-8", TRUE);
  printf("Subject: [estwaver] %s\r\n", str);
  free(str);
  printf("Content-Type: multipart/mixed; boundary=%s\r\n", border);
  printf("\r\n");
  printf("This is a multi-part message in MIME format.\n");
  printf("\r\n");
  printf("--%s\r\n", border);
  printf("Content-Type: text/x-estraier-draft\r\n");
  printf("X-Estwaver-Role: draft\r\n");
  printf("\r\n");
  str = est_doc_dump_draft(doc);
  printf("%s", str);
  free(str);
  printf("\r\n");
  printf("--%s\r\n", border);
  printf("Content-Type: text/plain\r\n");
  printf("X-Estwaver-Role: links\r\n");
  printf("\r\n");
  for(i = 0; i < cblistnum(links); i++){
    printf("%s\n", cblistval(links, i, NULL));
  }
  printf("\r\n");
  printf("--%s\r\n", border);
  cbmapiterinit(heads);
  while((vbuf = cbmapiternext(heads, &vsiz)) != NULL){
    if(vsiz < 1){
      printf("X-Original-HTTP-Response: %s\r\n", cbmapget(heads, vbuf, vsiz, NULL));
    } else if(!strcmp(vbuf, "content-encoding")){
      printf("X-Original-Content-Encoding: %s\r\n", cbmapget(heads, vbuf, vsiz, NULL));
    } else {
      str = capitalize(vbuf);
      printf("%s: %s\r\n", str, cbmapget(heads, vbuf, vsiz, NULL));
      free(str);
    }
  }
  printf("X-Estwaver-Role: raw\r\n");
  printf("\r\n");
  fwrite(cbdatumptr(raw), 1, cbdatumsize(raw), stdout);
  printf("\r\n");
  printf("--%s--\r\n", border);
  est_doc_delete(doc);
  cblistclose(links);
  cbmapclose(heads);
  cbdatumclose(raw);
  return 0;
}


/* get the language value */
static int strtolang(const char *str){
  if(!cbstricmp(str, "en")) return ESTLANGEN;
  if(!cbstricmp(str, "ja")) return ESTLANGJA;
  if(!cbstricmp(str, "zh")) return ESTLANGZH;
  if(!cbstricmp(str, "ko")) return ESTLANGKO;
  return ESTLANGMISC;
}


/* make a capitalized string */
static char *capitalize(const char *str){
  char *buf;
  int i, cap;
  buf = cbmemdup(str, -1);
  cap = TRUE;
  for(i = 0; buf[i] != '\0'; i++){
    if(cap && buf[i] >= 'a' && buf[i] <= 'z'){
      buf[i] -= 'a' - 'A';
    }
    cap = buf[i] == ' ' || buf[i] == '-';
  }
  return buf;
}


/* get keywords of a seed document */
static void seedurldocs(WAVER *waver, const char *url, int depth, double bias, CBMAP *ulinks){
  PMRULE *pmrule;
  ESTDOC *doc;
  CBMAP *kwords;
  CBLIST *links;
  const char *kbuf;
  char *ubuf, *pv;
  int i, j, code, ksiz, num, len, allow;
  double lnumtune;
  if(g_sigterm) return;
  ubuf = cbsprintf("%d:0:1.0\t%s", depth, url);
  queue_enqueue(waver->queue, ubuf, depth * MINPRIOR);
  free(ubuf);
  links = cblistopen();
  doc = est_doc_new();
  log_print(LL_INFO, "fetching: %d: %s", depth, url);
  fetch_document(url, waver->pxhost, waver->pxport, waver->timeout * 2, 0,
                 waver->urlrules, waver->mtrules, &code, NULL, NULL, links,
                 waver->unrules, doc, waver->language);
  if(code == 200 && !est_doc_is_empty(doc)){
    log_print(LL_INFO, "seeding: %.3f: %s", bias, url);
    kwords = est_morph_etch_doc(doc, waver->seedkeynum);
    cbmapiterinit(kwords);
    while((kbuf = cbmapiternext(kwords, &ksiz)) != NULL){
      num = atoi(cbmapget(kwords, kbuf, ksiz, NULL)) * bias;
      if(num > 0) kwords_add(waver->kwords, kbuf, num);
    }
    if(depth < waver->seeddepth && bias > 0.0){
      lnumtune = pow(cblistnum(links) + 2, 0.5);
      for(i = 0; i < cblistnum(links); i++){
        ubuf = cbmemdup(cblistval(links, i, NULL), -1);
        if((pv = strchr(ubuf, '#')) != NULL) *pv = '\0';
        len = strlen(ubuf);
        if(len > 1024 || cbmapget(ulinks, ubuf, len, NULL) ||
           cbmapget(waver->seeds, ubuf, len, NULL)){
          free(ubuf);
          continue;
        }
        cbmapput(ulinks, ubuf, -1, "", 0, FALSE);
        allow = FALSE;
        for(j = 0; j < cblistnum(waver->pmrules); j++){
          pmrule = (PMRULE *)cblistval(waver->pmrules, j, NULL);
          switch(pmrule->visit){
          case 1:
            if(est_regex_match(pmrule->regex, ubuf)) allow = TRUE;
            break;
          case -1:
            if(est_regex_match(pmrule->regex, ubuf)) allow = FALSE;
            break;
          }
        }
        if(allow) seedurldocs(waver, ubuf, depth + 1, bias / lnumtune, ulinks);
        free(ubuf);
      }
    }
    cbmapclose(kwords);
  } else {
    log_print(LL_INFO, "ignored: %d: %s", code, url);
  }
  est_doc_delete(doc);
  cblistclose(links);
}


/* access the number of threads */
static int accessthnum(int inc){
  static pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
  int thnum;
  if(pthread_mutex_lock(&mymutex) != 0){
    log_print(LL_ERROR, "could not get mutex");
    return INT_MAX;
  }
  g_thnum += inc;
  thnum = g_thnum;
  pthread_mutex_unlock(&mymutex);
  return thnum;
}


/* put the current task */
static int puttask(const char *url){
  if(pthread_mutex_lock(&g_mutex) != 0){
    log_print(LL_ERROR, "could not get mutex");
    return FALSE;
  }
  if(!cbmapput(g_tasks, url, -1, "", 0, FALSE)){
    pthread_mutex_unlock(&g_mutex);
    return FALSE;
  }
  pthread_mutex_unlock(&g_mutex);
  return TRUE;
}


/* put the current task */
static void outtask(const char *url){
  if(pthread_mutex_lock(&g_mutex) != 0){
    log_print(LL_ERROR, "could not get mutex");
    return;
  }
  cbmapout(g_tasks, url, -1);
  pthread_mutex_unlock(&g_mutex);
  return;
}


/* get a document of URL */
static void *geturldoc(void *args){
  TARGSURL *myargs;
  PMRULE *pmrule;
  WAVER *waver;
  CBMAP *heads, *kwords;
  CBLIST *links;
  CBDATUM *raw;
  ESTDOC *doc;
  char *url, *rec, *dpath, *epath;
  char numbuf[NUMBUFSIZ], *tmp;
  int i, thid, id, alive, code, allow;
  double now;
  accessthnum(1);
  myargs = args;
  thid = myargs->thid;
  waver = myargs->waver;
  url = myargs->url;
  if(!puttask(url)){
    log_print(LL_DEBUG, "[%d]: early collision: %s", thid, url);
    free(url);
    free(myargs);
    accessthnum(-1);
    return NULL;
  }
  if(waver->interval > 0) est_usleep(waver->interval * 1000);
  alive = FALSE;
  raw = cbdatumopen(NULL, -1);
  heads = cbmapopen();
  links = cblistopen();
  doc = est_doc_new();
  log_print(LL_INFO, "[%d]: fetching: %d: %s", thid, myargs->depth, url);
  fetch_document(url, waver->pxhost, waver->pxport, waver->timeout, myargs->mdate,
                 waver->urlrules, waver->mtrules, &code, raw, heads, links,
                 waver->unrules, doc, waver->language);
  if(g_sigterm){
    log_print(LL_WARN, "[%d]: terminated: %s", thid, url);
    if(pthread_mutex_lock(&g_mutex) == 0){
      tmp = cbsprintf("%d:%d:%.5f\t%s", myargs->depth, myargs->pid, myargs->psim, url);
      queue_enqueue(waver->queue, tmp, MINPRIOR);
      free(tmp);
      pthread_mutex_unlock(&g_mutex);
    }
    est_doc_delete(doc);
    cblistclose(links);
    cbmapclose(heads);
    cbdatumclose(raw);
    outtask(url);
    free(url);
    free(myargs);
    accessthnum(-1);
    return NULL;
  }
  if(pthread_mutex_lock(&g_mutex) == 0){
    now = time(NULL);
    if((rec = crget(waver->trace, url, -1, 0, -1, NULL)) != NULL){
      if(strtod(rec, NULL) + waver->revisit >= now){
        log_print(LL_DEBUG, "[%d]: late collision: %s", thid, url);
        free(rec);
        pthread_mutex_unlock(&g_mutex);
        est_doc_delete(doc);
        cblistclose(links);
        cbmapclose(heads);
        cbdatumclose(raw);
        outtask(url);
        free(url);
        free(myargs);
        accessthnum(-1);
        return NULL;
      }
      free(rec);
    }
    sprintf(numbuf, "%.0f:%d:%d:%.5f#%d",
            now, myargs->depth, myargs->pid, myargs->psim, waver->curnode);
    crput(waver->trace, url, -1, numbuf, -1, CR_DOVER);
    if(code == 200 && est_doc_attr(doc, ESTDATTRURI)){
      alive = TRUE;
      est_doc_slim(doc, waver->textlimit);
      kwords = est_morph_etch_doc(doc, waver->seedkeynum);
      id = 0;
      allow = FALSE;
      for(i = 0; i < cblistnum(waver->pmrules); i++){
        pmrule = (PMRULE *)cblistval(waver->pmrules, i, NULL);
        switch(pmrule->index){
        case 1:
          if(est_regex_match(pmrule->regex, url)) allow = TRUE;
          break;
        case -1:
          if(est_regex_match(pmrule->regex, url)) allow = FALSE;
          break;
        }
      }
      if(allow && !est_doc_is_empty(doc)){
        est_doc_set_keywords(doc, kwords);
        kwords_reduce(est_doc_keywords(doc), waver->savekeynum, FALSE);
        if(waver->curnode > 0){
          if(waver_node_put_doc(waver, doc, &code)){
            log_print(LL_DEBUG, "[%d]: registered: %s", thid, url);
            waver->curnum++;
          } else {
            log_print(LL_ERROR, "[%d]: registration failed: %s: %d", thid, url, code);
          }
        } else {
          if(est_mtdb_put_doc(waver->index, doc, ESTPDCLEAN)){
            log_print(LL_DEBUG, "[%d]: registered: %s", thid, url);
            id = est_doc_id(doc);
            waver->curnum++;
          } else {
            log_print(LL_ERROR, "[%d]: registration failed: %s: %s",
                      thid, url, dperrmsg(dpecode));
          }
        }
      } else {
        log_print(LL_DEBUG, "[%d]: not to be indexed: %s", thid, url);
      }
      enqueuelinks(waver, url, links, kwords, myargs->depth, id, myargs->pid, myargs->psim);
      cbmapclose(kwords);
    } else if(cblistnum(links) > 0){
      enqueuelinks(waver, url, links, NULL,
                   myargs->depth, myargs->pid, myargs->pid, myargs->psim);
      log_print(LL_INFO, "[%d]: redirected: %d: %s", thid, code, url);
    } else {
      log_print(LL_INFO, "[%d]: ignored: %d: %s", thid, code, url);
    }
    if(!alive && code != 304){
      if(cbmaprnum(waver->nodes) > 0){
        if(waver_node_out_doc(waver, url, &code)){
          log_print(LL_DEBUG, "[%d]: deleted: %s", thid, url);
        } else {
          if(code != 400) log_print(LL_ERROR, "[%d]: deletion failed: %s: %d", thid, url, code);
        }
      } else if((id = est_mtdb_uri_to_id(waver->index, url)) > 0){
        if(est_mtdb_out_doc(waver->index, id, ESTODCLEAN)){
          log_print(LL_DEBUG, "[%d]: deleted: %s", thid, url);
        } else {
          log_print(LL_ERROR, "[%d]: deletion failed: %s: %s",
                    thid, url, dperrmsg(dpecode));
        }
      }
    }
    pthread_mutex_unlock(&g_mutex);
  } else {
    log_print(LL_ERROR, "[%d]: could not get mutex", thid);
  }
  if(code == 200){
    dpath = NULL;
    epath = NULL;
    if(waver->postproc){
      dpath = cbsprintf("%s%c%s%c%08d.est",
                        waver->rootdir, ESTPATHCHR, MYTMPDIR, ESTPATHCHR, thid);
      epath = cbsprintf("%s%c%s%c%08d.dat",
                        waver->rootdir, ESTPATHCHR, MYTMPDIR, ESTPATHCHR, thid);
    }
    if(waver->draftdir){
      free(dpath);
      dpath = urltosavepath(waver->draftdir, url);
    }
    if(waver->entitydir){
      free(epath);
      epath = urltosavepath(waver->entitydir, url);
    }
    if(dpath){
      log_print(LL_DEBUG, "[%d]: saving: %s", thid, dpath);
      tmp = est_doc_dump_draft(doc);
      if(!cbwritefile(dpath, tmp, -1))
        log_print(LL_ERROR, "[%d]: saving failed: %s", thid, dpath);
      free(tmp);
    }
    if(epath){
      log_print(LL_DEBUG, "[%d]: saving: %s", thid, epath);
      if(!cbwritefile(epath, cbdatumptr(raw), cbdatumsize(raw)))
        log_print(LL_ERROR, "[%d]: saving failed: %s", thid, epath);
    }
    if(waver->postproc){
      tmp = cbsprintf("%s \"%s\" \"%s\"", waver->postproc, dpath, epath);
      system(tmp);
      free(tmp);
    }
    if(epath && !waver->entitydir) unlink(epath);
    if(dpath && !waver->draftdir) unlink(dpath);
    free(dpath);
    free(epath);
  }
  est_doc_delete(doc);
  cblistclose(links);
  cbmapclose(heads);
  cbdatumclose(raw);
  outtask(url);
  free(url);
  free(myargs);
  accessthnum(-1);
  return NULL;
}


/* get the saving path of a URL */
static char *urltosavepath(const char *savedir, const char *url){
  CBDATUM *buf;
  CBLIST *elems;
  const char *rp;
  int i;
  if((rp = strstr(url, "://")) != NULL) url = rp + 3;
  buf = cbdatumopen(NULL, -1);
  elems = cbsplit(url, -1, "/");
  if(cbstrbwmatch(url, "/")) cblistpush(elems, "index.html", -1);
  cbdatumprintf(buf, "%s", savedir);
  for(i = 0; i < cblistnum(elems); i++){
    rp = cblistval(elems, i, NULL);
    if(rp[0] == '\0') continue;
    est_mkdir(cbdatumptr(buf));
    unlink(cbdatumptr(buf));
    cbdatumprintf(buf, "%c%?", ESTPATHCHR, rp);
  }
  cblistclose(elems);
  return cbdatumtomalloc(buf, NULL);
}



/* END OF FILE */
