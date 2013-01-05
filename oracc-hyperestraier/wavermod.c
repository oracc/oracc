/*************************************************************************************************
 * Implementation of wavermod
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


/* private function prototypes */
static void log_close(void);
static void db_informer(const char *message, void *opaque);
static const char *skiplabel(const char *str);
static char *makeabspath(const char *rootdir, const char *path);
static int queue_compare(const char *aptr, int asiz, const char *bptr, int bsiz);
static int keysc_compare(const void *ap, const void *bp);
static void make_doc_from_draft(const char *buf, int size, ESTDOC *doc, CBLIST *links);
static void make_doc_from_text(const char *buf, int size, const char *penc, int plang,
                               ESTDOC *doc, CBLIST *links);
static int check_binary(const char *buf, int size);
static void make_doc_from_html(const char *buf, int size, const char *penc, int plang,
                               ESTDOC *doc, CBLIST *links);
static char *html_enc(const char *str);
static char *html_raw_text(const char *html);
static void make_doc_from_mime(const char *buf, int size, const char *penc, int plang,
                               ESTDOC *doc, CBLIST *links);
static void doc_add_attr_mime(ESTDOC *doc, const char *name, const char *value);
static void make_doc_with_xcmd(const char *xcmd, const char *url, const char *buf, int size,
                               const char *penc, int plang, ESTDOC *doc, CBLIST *links);



/*************************************************************************************************
 * pseudo API
 *************************************************************************************************/


/* The handles of the log file. */
FILE *log_fp = NULL;


/* Level of logging. */
int log_level = LL_INFO;


/* Open the log file. */
int log_open(const char *rootdir, const char *path, int level, int trunc){
  char mypath[URIBUFSIZ];
  assert(rootdir && path);
  log_level = level;
  if(log_fp) return TRUE;
  if((ESTPATHCHR == '/' && path[0] == ESTPATHCHR) ||
     (ESTPATHCHR == '\\' && ((path[0] >= 'A' && path[0] <= 'Z') ||
                             (path[0] >= 'a' && path[0] <= 'z')) && path[1] == ':' &&
      path[2] == '\\')){
    sprintf(mypath, "%s", path);
  } else {
    sprintf(mypath, "%s%c%s", rootdir, ESTPATHCHR, path);
  }
  if(!(log_fp = fopen(mypath, trunc ? "wb" : "ab"))) return FALSE;
  if(level == LL_CHECK){
    fclose(log_fp);
    log_fp = NULL;
    return TRUE;
  }
  atexit(log_close);
  return TRUE;
}


/* Print formatted string into the log file. */
void log_print(int level, const char *format, ...){
  static pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
  va_list ap, aq;
  const char *lvstr;
  char *date;
  if(level < log_level) return;
  if(pthread_mutex_lock(&mymutex) != 0) return;
  va_start(ap, format);
  est_va_copy(aq, ap);
  switch(level){
  case LL_DEBUG: lvstr = "DEBUG"; break;
  case LL_INFO: lvstr = "INFO"; break;
  case LL_WARN: lvstr = "WARN"; break;
  default: lvstr = "ERROR"; break;
  }
  date = cbdatestrwww(time(NULL), 0);
  printf("%s\t%s\t", date, lvstr);
  vprintf(format, ap);
  putchar('\n');
  fflush(stdout);
  if(log_fp){
    fprintf(log_fp, "%s\t%s\t", date, lvstr);
    vfprintf(log_fp, format, aq);
    fputc('\n', log_fp);
    fflush(log_fp);
  }
  free(date);
  va_end(aq);
  va_end(ap);
  pthread_mutex_unlock(&mymutex);
}


/* Rotete the log file. */
int log_rotate(const char *rootdir, const char *path){
  FILE *ifp, *ofp;
  char mypath[URIBUFSIZ], *wp, iobuf[IOBUFSIZ];
  int err, year, month, day, hour, minute, second, len;
  assert(rootdir && path);
  if(!log_fp || fflush(log_fp) == -1) return FALSE;
  err = FALSE;
  wp = mypath;
  if((ESTPATHCHR == '/' && path[0] == ESTPATHCHR) ||
     (ESTPATHCHR == '\\' && ((path[0] >= 'A' && path[0] <= 'Z') ||
                             (path[0] >= 'a' && path[0] <= 'z')) && path[1] == ':' &&
      path[2] == '\\')){
    wp += sprintf(wp, "%s", path);
  } else {
    wp += sprintf(wp, "%s%c%s", rootdir, ESTPATHCHR, path);
  }
  if(!(ifp = fopen(mypath, "rb"))) return FALSE;
  cbcalendar(-1, 0, &year, &month, &day, &hour, &minute, &second);
  sprintf(wp, "-%04d%02d%02d%02d%02d%02d", year, month, day, hour, minute, second);
  if(!(ofp = fopen(mypath, "wb"))){
    fclose(ifp);
    return FALSE;
  }
  while((len = fread(iobuf, 1, IOBUFSIZ, ifp)) > 0){
    fwrite(iobuf, 1, len, ofp);
  }
  if(fclose(ofp) == -1) err = TRUE;
  if(fclose(ifp) == -1) err = TRUE;
  if(fseek(log_fp, 0, SEEK_SET) == -1 || fflush(log_fp) == -1) err = TRUE;
  if(ftruncate(fileno(log_fp), 0) == -1) err = TRUE;
  return err ? FALSE : TRUE;
}


/* Initialize the root directory. */
int waver_init(const char *rootdir, int options){
  DEPOT *metadb;
  CURIA *trace;
  QUEUE *queue;
  ESTMTDB *index;
  FILE *ofp;
  char path[URIBUFSIZ];
  int err, tracebnum, estopts, ecode;
  assert(rootdir);
  if(est_mkdir(rootdir) == -1 && errno != EEXIST) return FALSE;
  err = FALSE;
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, METAFILE);
  if((metadb = dpopen(path, DP_OWRITER | DP_OCREAT | DP_OTRUNC, MINIBNUM))){
    if(!dpput(metadb, MMKMAGIC, -1, MMKMAGVAL, -1, DP_DKEEP)) err = TRUE;
    if(!dpclose(metadb)) err = TRUE;
  } else {
    err = TRUE;
  }
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, CONFFILE);
  if((ofp = fopen(path, "wb")) != NULL){
    fprintf(ofp, "# seed documents (weight and URL)\n");
    fprintf(ofp, "seed: 1.5|http://fallabs.com/hyperstraier/uguide-en.html\n");
    fprintf(ofp, "seed: 1.0|http://fallabs.com/hyperestraier/pguide-en.html\n");
    fprintf(ofp, "seed: 1.0|http://fallabs.com/hyperestraier/nguide-en.html\n");
    fprintf(ofp, "seed: 0.0|http://fallabs.com/qdbm/\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# host name of the proxy\n");
    fprintf(ofp, "proxyhost:\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# port number of the proxy\n");
    fprintf(ofp, "proxyport:\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# waiting interval of each request (in milliseconds)\n");
    fprintf(ofp, "interval: 500\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# timeout of each request (in seconds)\n");
    fprintf(ofp, "timeout: 30\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# strategy of crawling path"
            " (0:balanced, 1:similarity, 2:depth, 3:width, 4:random)\n");
    fprintf(ofp, "strategy: %d\n", CS_BALANCED);
    fprintf(ofp, "\n");
    fprintf(ofp, "# inheritance ratio of similarity from the parent\n");
    fprintf(ofp, "inherit: 0.4\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# maximum depth of seed documents\n");
    fprintf(ofp, "seeddepth: 0\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# maximum depth of recursion\n");
    fprintf(ofp, "maxdepth: 20\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# standard value for checking mass sites\n");
    fprintf(ofp, "masscheck: 500\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# maximum number of records of the priority queue\n");
    fprintf(ofp, "queuesize: 50000\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# regular expressions and replacement strings to normalize URLs\n");
    fprintf(ofp, "replace: ^http://127.0.0.1/{{!}}http://localhost/\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# allowing regular expressions of URLs to be visited\n");
    fprintf(ofp, "allowrx: ^http://\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# denying regular expressions of URLs to be visited\n");
    fprintf(ofp, "denyrx: \\.(css|js|csv|tsv|log|md5|crc|conf|ini|inf|lnk|sys|tmp|bak)$\n");
    fprintf(ofp, "denyrx: \\.(xml|xsl|xslt|rdf|rss|dtd|sgml|sgm)$\n");
    fprintf(ofp, "denyrx: \\.(pgp|sig|cer|csr|pem|key|b64|uu|uue|[0-9])$\n");
    fprintf(ofp, "denyrx: \\.(rtf|pdf|ps|eps|ai|doc|xls|ppt|sxw|sxc|sxi|xdw|jtd|oas|swf)$\n");
    fprintf(ofp, "denyrx: \\.(zip|tar|tgz|gz|bz2|tbz2|z|lha|lzh)(\\?.*)?$\n");
    fprintf(ofp, "denyrx: \\.(7z|lzo|lzma|cpio|shar|cab|rar|sit|ace|hqx)(\\?.*)?$\n");
    fprintf(ofp, "denyrx: \\.(bin|o|a|so|exe|dll|lib|obj|ocx|class|jar|war)(\\?.*)?$\n");
    fprintf(ofp, "denyrx: \\.(rpm|deb|qdb|qdb|dbx|dbf|dat|msi|bat|com|iso)(\\?.*)?$\n");
    fprintf(ofp, "denyrx: \\.(png|gif|jpg|jpeg|tif|tiff|bmp|ico|pbm|pgm|ppm|xbm|xpm|dvi)$\n");
    fprintf(ofp, "denyrx: \\.(au|snd|mid|midi|kar|smf|mp2|mp3|m3u|wav|wma|wmp|asx|at3|aif)$\n");
    fprintf(ofp, "denyrx: \\.(mpg|mpeg|qt|mov|avi|wmv|wvx|asf|ram|rm)$\n");
    fprintf(ofp, "denyrx: (/core$|/core\\.[0-9]*$|/casket/)\n");
    fprintf(ofp, "denyrx: ://(localhost|[a-z]*\\.localdomain|127\\.0\\.0\\.1)/\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# denying regular expressions of URLs to be indexed\n");
    fprintf(ofp, "noidxrx: /\\?[a-z]=[a-z](;|$)\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# URL rules (regular expressions and media types)\n");
    fprintf(ofp, "urlrule: \\.est${{!}}text/x-estraier-draft\n");
    fprintf(ofp, "urlrule: \\.(eml|mime|mht|mhtml)${{!}}message/rfc822\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# media type rules (regular expressions and filter commands)\n");
    fprintf(ofp, "typerule: ^text/x-estraier-draft${{!}}%s\n", DRAFTCMD);
    fprintf(ofp, "typerule: ^text/plain${{!}}%s\n", TEXTCMD);
    fprintf(ofp, "typerule: ^(text/html|application/xhtml+xml)${{!}}%s\n", HTMLCMD);
    fprintf(ofp, "typerule: ^message/rfc822${{!}}%s\n", MIMECMD);
    fprintf(ofp, "\n");
    fprintf(ofp, "# preferred language (0:English, 1:Japanese, 2:Chinese, 3:Korean, 4:misc)\n");
    fprintf(ofp, "language: 0\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# text size limitation (in kilobytes)\n");
    fprintf(ofp, "textlimit: 128\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# total number of keywords for seed documents\n");
    fprintf(ofp, "seedkeynum: 256\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# number of keywords saved for each document\n");
    fprintf(ofp, "savekeynum: 32\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# number of threads running in parallel\n");
    fprintf(ofp, "threadnum: 10\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# number of documents to collect\n");
    fprintf(ofp, "docnum: 10000\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# running time period (in s:seconds, m:minutes, h:hours, d:days)\n");
    fprintf(ofp, "period: 10000s\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# revisit span (in s:seconds, m:minutes, h:hours, d:days)\n");
    fprintf(ofp, "revisit: 7d\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# maximum size of the index cache (in megabytes)\n");
    fprintf(ofp, "cachesize: 256\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# remote nodes for alternative indexes (ID number and URL)\n");
    fprintf(ofp, "#nodeserv: 1|http://admin:admin@localhost:1978/node/node1\n");
    fprintf(ofp, "#nodeserv: 2|http://admin:admin@localhost:1978/node/node2\n");
    fprintf(ofp, "#nodeserv: 3|http://admin:admin@localhost:1978/node/node3\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# path of the log file (relative path or absolute path)\n");
    fprintf(ofp, "logfile: %s\n", LOGFILE);
    fprintf(ofp, "\n");
    fprintf(ofp, "# logging level (1:debug, 2:information, 3:warning, 4:error, 5:none)\n");
    fprintf(ofp, "loglevel: 2\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# path of the draft directory (relative path or absolute path)\n");
    fprintf(ofp, "draftdir:\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# path of the entity directory (relative path or absolute path)\n");
    fprintf(ofp, "entitydir:\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# postprocessor for retrieved files\n");
    fprintf(ofp, "postproc:\n");
    fprintf(ofp, "\n");
    if(fclose(ofp) == EOF) err = TRUE;
  } else {
    err = TRUE;
  }
  tracebnum = TRACEBNUM;
  estopts = 0;
  if(options & WI_PERFNG){
    estopts |= ESTDBPERFNG;
  } else if(options & WI_CHRCAT){
    estopts |= ESTDBCHRCAT;
  }
  if(options & WI_SMALL){
    tracebnum = TRACEBNUM / 2;
    estopts |= ESTDBSMALL;
  } else if(options & WI_LARGE){
    tracebnum = TRACEBNUM * 2;
    estopts |= ESTDBLARGE;
  } else if(options & WI_HUGE){
    tracebnum = TRACEBNUM * 4;
    estopts |= ESTDBHUGE;
  }
  if(options & WI_SCVOID){
    estopts |= ESTDBSCVOID;
  } else if(options & WI_SCINT){
    estopts |= ESTDBSCINT;
  } else if(options & WI_SCASIS){
    estopts |= ESTDBSCASIS;
  }
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, QUEUEFILE);
  unlink(path);
  if((queue = queue_open(path)) != NULL){
    if(!queue_close(queue)) err = TRUE;
  } else {
    err = TRUE;
  }
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, TRACEFILE);
  if((trace = cropen(path, CR_OWRITER | CR_OCREAT | CR_OTRUNC, tracebnum, TRACEDNUM)) != NULL){
    if(!crclose(trace)) err = TRUE;
  } else {
    err = TRUE;
  }
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, INDEXDIR);
  if((index = est_mtdb_open(path, ESTDBWRITER | ESTDBCREAT | ESTDBTRUNC | estopts,
                            &ecode)) != NULL){
    if(!est_mtdb_add_attr_index(index, ESTDATTRURI, ESTIDXATTRSTR)) err = TRUE;
    if(!est_mtdb_add_attr_index(index, ESTDATTRTITLE, ESTIDXATTRSTR)) err = TRUE;
    if(!est_mtdb_add_attr_index(index, ESTDATTRMDATE, ESTIDXATTRSEQ)) err = TRUE;
    if(!est_mtdb_close(index, &ecode)) err = TRUE;
  } else {
    err = TRUE;
  }
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, MYTMPDIR);
  est_mkdir(path);
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, LOGFILE);
  if((ofp = fopen(path, "wb")) != NULL){
    if(fclose(ofp) == EOF) err = TRUE;
  } else {
    err = TRUE;
  }
  return err ? FALSE : TRUE;
}


/* Open a waver handle. */
WAVER *waver_open(const char *rootdir){
  WAVER *waver;
  UNRULE unrule;
  PMRULE pmrule;
  URLRULE urlrule;
  MTRULE mtrule;
  DEPOT *metadb;
  CURIA *trace;
  QUEUE *queue;
  ESTMTDB *index;
  ESTNODE *node;
  CBLIST *lines;
  const char *rp, *pv, *logfile;
  char path[URIBUFSIZ], *tmp;
  int i, ecode, loglevel, num;
  struct stat sbuf;
  assert(rootdir);
  if(stat(rootdir, &sbuf) == -1) return NULL;
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, CONFFILE);
  lines = cbreadlines(path);
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, METAFILE);
  metadb = dpopen(path, DP_OWRITER, -1);
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, QUEUEFILE);
  queue = queue_open(path);
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, TRACEFILE);
  trace = cropen(path, CR_OWRITER, -1, -1);
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, INDEXDIR);
  index = est_mtdb_open(path, ESTDBWRITER, &ecode);
  if(!lines || !metadb || !queue || !trace || !index){
    if(index) est_mtdb_close(index, &ecode);
    if(trace) crclose(trace);
    if(queue) queue_close(queue);
    if(metadb) dpclose(metadb);
    if(lines) cblistclose(lines);
    return NULL;
  }
  waver = cbmalloc(sizeof(WAVER));
  waver->rootdir = cbmemdup(rootdir, -1);
  waver->metadb = metadb;
  waver->queue = queue;
  waver->trace = trace;
  waver->index = index;
  waver->seeds = cbmapopen();
  waver->kwords = cbmapopen();
  waver->sites = cbmapopen();
  waver->pxhost = NULL;
  waver->pxport = 80;
  waver->timeout = -1;
  waver->interval = 0;
  waver->strategy = CS_BALANCED;
  waver->inherit = 0.0;
  waver->seeddepth = 0;
  waver->maxdepth = INT_MAX;
  waver->masscheck = INT_MAX;
  waver->queuesize = INT_MAX;
  waver->unrules = cblistopen();
  waver->pmrules = cblistopen();
  waver->urlrules = cblistopen();
  waver->mtrules = cblistopen();
  waver->language = ESTLANGEN;
  waver->textlimit = 0;
  waver->seedkeynum = 0;
  waver->savekeynum = 0;
  waver->thnum = 1;
  waver->docnum = 0;
  waver->period = 0;
  waver->revisit = 0;
  waver->cachesize = 0;
  waver->nodes = cbmapopenex(MINIBNUM);
  waver->draftdir = NULL;
  waver->entitydir = NULL;
  waver->postproc = NULL;
  waver->stime = time(NULL);
  waver->curnum = 0;
  waver->curnode = 0;
  waver->minload = 1.0;
  logfile = LOGFILE;
  loglevel = LL_INFO;
  for(i = 0; i < cblistnum(lines); i++){
    rp = cblistval(lines, i, NULL);
    if(cbstrfwimatch(rp, "seed:")){
      rp = skiplabel(rp);
      if((pv = strchr(rp, '|')) != NULL){
        tmp = cburlresolve(pv + 1, "");
        cbmapput(waver->seeds, tmp, -1, rp, pv - rp, FALSE);
        free(tmp);
      }
    } else if(cbstrfwimatch(rp, "proxyhost:")){
      rp = skiplabel(rp);
      if(rp[0] != '\0') waver->pxhost = cbmemdup(rp, -1);
    } else if(cbstrfwimatch(rp, "proxyport:")){
      rp = skiplabel(rp);
      if(rp[0] != '\0') waver->pxport = atoi(rp);
    } else if(cbstrfwimatch(rp, "interval:")){
      waver->interval = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "timeout:")){
      waver->timeout = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "strategy:")){
      waver->strategy = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "inherit:")){
      waver->inherit = strtod(skiplabel(rp), NULL);
    } else if(cbstrfwimatch(rp, "seeddepth:")){
      waver->seeddepth = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "maxdepth:")){
      waver->maxdepth = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "masscheck:")){
      waver->masscheck = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "queuesize:")){
      waver->queuesize = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "replace:")){
      rp = skiplabel(rp);
      if((pv = strstr(rp, "{{!}}")) != NULL){
        tmp = cbmemdup(rp, pv - rp);
        if((unrule.regex = est_regex_new(tmp)) != NULL){
          unrule.before = tmp;
          unrule.after = cbmemdup(pv + 5, -1);
          cblistpush(waver->unrules, (char *)&unrule, sizeof(UNRULE));
        } else {
          free(tmp);
        }
      }
    } else if(cbstrfwimatch(rp, "allowrx:")){
      tmp = cbsprintf("*I:%s", skiplabel(rp));
      if((pmrule.regex = est_regex_new(tmp)) != NULL){
        pmrule.visit = 1;
        pmrule.index = 1;
        cblistpush(waver->pmrules, (char *)&pmrule, sizeof(PMRULE));
      }
      free(tmp);
    } else if(cbstrfwimatch(rp, "denyrx:")){
      tmp = cbsprintf("*I:%s", skiplabel(rp));
      if((pmrule.regex = est_regex_new(tmp)) != NULL){
        pmrule.visit = -1;
        pmrule.index = 0;
        cblistpush(waver->pmrules, (char *)&pmrule, sizeof(PMRULE));
      }
      free(tmp);
    } else if(cbstrfwimatch(rp, "noidxrx:")){
      tmp = cbsprintf("*I:%s", skiplabel(rp));
      if((pmrule.regex = est_regex_new(tmp)) != NULL){
        pmrule.visit = 0;
        pmrule.index = -1;
        cblistpush(waver->pmrules, (char *)&pmrule, sizeof(PMRULE));
      }
      free(tmp);
    } else if(cbstrfwimatch(rp, "urlrule:")){
      rp = skiplabel(rp);
      if((pv = strstr(rp, "{{!}}")) != NULL){
        tmp = cbmemdup(rp, pv - rp);
        if((urlrule.regex = est_regex_new(tmp)) != NULL){
          urlrule.type = cbmemdup(pv + 5, -1);
          cblistpush(waver->urlrules, (char *)&urlrule, sizeof(URLRULE));
        }
        free(tmp);
      }
    } else if(cbstrfwimatch(rp, "typerule:")){
      rp = skiplabel(rp);
      if((pv = strstr(rp, "{{!}}")) != NULL){
        tmp = cbmemdup(rp, pv - rp);
        if((mtrule.regex = est_regex_new(tmp)) != NULL){
          mtrule.filter = cbmemdup(pv + 5, -1);
          cblistpush(waver->mtrules, (char *)&mtrule, sizeof(MTRULE));
        }
        free(tmp);
      }
    } else if(cbstrfwimatch(rp, "language:")){
      waver->language = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "textlimit:")){
      waver->textlimit = atoi(skiplabel(rp)) * 1024;
    } else if(cbstrfwimatch(rp, "seedkeynum:")){
      waver->seedkeynum = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "savekeynum:")){
      waver->savekeynum = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "threadnum:")){
      waver->thnum = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "docnum:")){
      waver->docnum = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "period:")){
      rp = skiplabel(rp);
      waver->period = atoi(rp);
      if(cbstrbwimatch(rp, "m")){
        waver->period *= 60;
      } else if(cbstrbwimatch(rp, "h")){
        waver->period *= 3600;
      } else if(cbstrbwimatch(rp, "d")){
        waver->period *= 86400;
      }
    } else if(cbstrfwimatch(rp, "revisit:")){
      rp = skiplabel(rp);
      waver->revisit = atoi(rp);
      if(cbstrbwimatch(rp, "m")){
        waver->revisit *= 60;
      } else if(cbstrbwimatch(rp, "h")){
        waver->revisit *= 3600;
      } else if(cbstrbwimatch(rp, "d")){
        waver->revisit *= 86400;
      }
    } else if(cbstrfwimatch(rp, "cachesize:")){
      waver->cachesize = atoi(skiplabel(rp)) * 1024 * 1024;
    } else if(cbstrfwimatch(rp, "nodeserv:")){
      rp = skiplabel(rp);
      if((pv = strchr(rp, '|')) != NULL){
        pv++;
        num = atoi(rp);
        if(num > 0 && !cbmapget(waver->nodes, (char *)&num, sizeof(int), NULL)){
          node = est_node_new(pv);
          cbmapput(waver->nodes, (char *)&num, sizeof(int),
                   (char *)&node, sizeof(ESTNODE *), FALSE);
        }
      }
    } else if(cbstrfwimatch(rp, "logfile:")){
      logfile = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "loglevel:")){
      loglevel = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "draftdir:")){
      rp = skiplabel(rp);
      if(rp[0] != '\0' && !waver->draftdir) waver->draftdir = makeabspath(rootdir, rp);
    } else if(cbstrfwimatch(rp, "entitydir:")){
      rp = skiplabel(rp);
      if(rp[0] != '\0' && !waver->entitydir) waver->entitydir = makeabspath(rootdir, rp);
    } else if(cbstrfwimatch(rp, "postproc:")){
      rp = skiplabel(rp);
      if(rp[0] != '\0' && !waver->postproc) waver->postproc = cbmemdup(rp, -1);
    }
  }
  if(!log_open(rootdir, logfile, loglevel, FALSE)){
    cblistclose(lines);
    waver_close(waver);
    return NULL;
  }
  if(waver->pxport < 1) waver->pxport = 80;
  if(waver->seeddepth < 0) waver->seeddepth = 0;
  if(waver->maxdepth < 0) waver->maxdepth = 0;
  if(waver->masscheck < 1) waver->masscheck = 1;
  if(waver->queuesize < 1) waver->queuesize = 1;
  if(waver->textlimit < 0) waver->textlimit = 0;
  if(waver->seedkeynum < 0) waver->seedkeynum = 0;
  if(waver->savekeynum < 0) waver->savekeynum = 0;
  if(waver->thnum < 1) waver->thnum = 1;
  if(waver->period < 1) waver->period = 1;
  if(waver->revisit < 1) waver->revisit = 1;
  if(waver->cachesize < 1) waver->cachesize = 1;
  cblistclose(lines);
  est_mtdb_set_informer(index, db_informer, NULL);
  est_mtdb_set_cache_size(index, waver->cachesize, -1, -1, -1);
  return waver;
}


/* Close a waver handle. */
int waver_close(WAVER *waver){
  UNRULE *unrule;
  PMRULE *pmrule;
  URLRULE *urlrule;
  MTRULE *mtrule;
  ESTNODE *node;
  const char *kbuf;
  int i, err, ecode;
  assert(waver);
  err = FALSE;
  free(waver->postproc);
  free(waver->entitydir);
  free(waver->draftdir);
  cbmapiterinit(waver->nodes);
  while((kbuf = cbmapiternext(waver->nodes, NULL)) != NULL){
    node = *(ESTNODE **)cbmapget(waver->nodes, kbuf, sizeof(int), NULL);
    est_node_delete(node);
  }
  cbmapclose(waver->nodes);
  for(i = 0; i < cblistnum(waver->mtrules); i++){
    mtrule = (MTRULE *)cblistval(waver->mtrules, i, NULL);
    est_regex_delete(mtrule->regex);
    free(mtrule->filter);
  }
  cblistclose(waver->mtrules);
  for(i = 0; i < cblistnum(waver->urlrules); i++){
    urlrule = (URLRULE *)cblistval(waver->urlrules, i, NULL);
    est_regex_delete(urlrule->regex);
    free(urlrule->type);
  }
  cblistclose(waver->urlrules);
  for(i = 0; i < cblistnum(waver->pmrules); i++){
    pmrule = (PMRULE *)cblistval(waver->pmrules, i, NULL);
    est_regex_delete(pmrule->regex);
  }
  cblistclose(waver->pmrules);
  for(i = 0; i < cblistnum(waver->unrules); i++){
    unrule = (UNRULE *)cblistval(waver->unrules, i, NULL);
    est_regex_delete(unrule->regex);
    free(unrule->before);
    free(unrule->after);
  }
  cblistclose(waver->unrules);
  free(waver->pxhost);
  cbmapclose(waver->sites);
  cbmapclose(waver->kwords);
  cbmapclose(waver->seeds);
  if(!est_mtdb_close(waver->index, &ecode)) err = TRUE;
  if(!crclose(waver->trace)) err = TRUE;
  if(!queue_close(waver->queue)) err = TRUE;
  if(!dpclose(waver->metadb)) err = TRUE;
  free(waver->rootdir);
  free(waver);
  return err ? FALSE : TRUE;
}


/* Set the current node. */
void waver_set_current_node(WAVER *waver){
  ESTNODE *node;
  const char *kbuf;
  int i;
  double ratio;
  assert(waver);
  waver->curnode = 0;
  cbmapiterinit(waver->nodes);
  waver->minload = 1.0;
  for(i = 0; i < 600; i++){
    while((kbuf = cbmapiternext(waver->nodes, NULL)) != NULL){
      node = *(ESTNODE **)cbmapget(waver->nodes, kbuf, sizeof(int), NULL);
      if((ratio = est_node_cache_usage(node)) >= 0.0 && ratio <= waver->minload){
        waver->curnode = *(int *)kbuf;
        waver->minload = ratio;
      }
    }
    if(waver->minload < 1.0){
      cbmapmove(waver->nodes, (char *)&(waver->curnode), sizeof(int), TRUE);
      break;
    }
    est_usleep(1000 * 1000);
  }
}


/* Get the load of the current node. */
double waver_current_node_load(WAVER *waver){
  ESTNODE *node;
  double ratio;
  if(!(node = *(ESTNODE **)cbmapget(waver->nodes, (char *)&(waver->curnode), sizeof(int), NULL)))
    return 1.0;
  ratio = est_node_cache_usage(node);
  return ratio > 0.0 ? ratio : 1.0;
}


/* Add a document to a node. */
int waver_node_put_doc(WAVER *waver, ESTDOC *doc, int *codep){
  ESTNODE *node;
  int rv;
  assert(waver && doc);
  if(!(node = *(ESTNODE **)cbmapget(waver->nodes, (char *)&(waver->curnode), sizeof(int), NULL))){
    if(codep) *codep = -1;
    return FALSE;
  }
  rv = est_node_put_doc(node, doc);
  if(codep) *codep = est_node_status(node);
  return rv;
}


/* Remove a document from a node. */
int waver_node_out_doc(WAVER *waver, const char *url, int *codep){
  ESTNODE *node;
  const char *rp;
  char *vbuf;
  int nid, rv;
  assert(waver && url);
  nid = 0;
  if((vbuf = crget(waver->trace, url, -1, 0, -1, NULL)) != NULL){
    if((rp = strchr(vbuf, '#')) != NULL) nid = atoi(rp + 1);
    free(vbuf);
  }
  if(nid < 1 || !(node = *(ESTNODE **)cbmapget(waver->nodes, (char *)&nid, sizeof(int), NULL))){
    if(codep) *codep = -1;
    return FALSE;
  }
  rv = est_node_out_doc_by_uri(node, url);
  if(codep) *codep = est_node_status(node);
  return rv;
}


/* Open a priority queue. */
QUEUE *queue_open(const char *name){
  QUEUE *queue;
  VILLA *db;
  const char *vbuf;
  int vomode, vsiz;
  double max;
  assert(name);
  vomode = VL_OWRITER | VL_OCREAT;
  if(ESTUSEBZIP){
    vomode |= VL_OXCOMP;
  } else if(ESTUSELZO){
    vomode |= VL_OYCOMP;
  } else if(ESTUSEZLIB){
    vomode |= VL_OZCOMP;
  }
  if(!(db = vlopen(name, vomode, queue_compare))) return NULL;
  vlsettuning(db, QUEUELRM, QUEUENIM, QUEUELCN, QUEUENCN);
  max = 1.0;
  vlcurlast(db);
  if((vbuf = vlcurkeycache(db, &vsiz)) != NULL && vsiz == sizeof(double))
    max = *(double *)vbuf;
  if(max < 1.0) max = 1.0;
  queue = cbmalloc(sizeof(QUEUE));
  queue->db = db;
  queue->max = max;
  return queue;
}


/* Close a priority queue. */
int queue_close(QUEUE *queue){
  int err;
  assert(queue);
  err = FALSE;
  if(!vlclose(queue->db)) err = TRUE;
  free(queue);
  return err ? FALSE : TRUE;
}


/* Set the range of the priority space of a priority queue. */
void queue_set_range(QUEUE *queue, double range){
  assert(queue);
  queue->max = range;
  if(queue->max < 1.0) queue->max = 1.0;
}


/* Enqueue a record into a priority queue. */
int queue_enqueue(QUEUE *queue, const char *str, double priority){
  int err;
  assert(queue && str);
  if(priority < 0.0) priority = 0.0;
  if(priority > 1.0) priority = 1.0;
  priority *= queue->max;
  err = FALSE;
  if(!vlput(queue->db, (char *)&priority, sizeof(double), str, -1, VL_DDUP)) err = TRUE;
  return err ? FALSE : TRUE;
}


/* Dequeue a record from a priority queue. */
char *queue_dequeue(QUEUE *queue){
  char *vbuf;
  assert(queue);
  vlcurfirst(queue->db);
  if(!(vbuf = vlcurval(queue->db, NULL))) return NULL;
  vlcurout(queue->db);
  return vbuf;
}


/* Get the number of records in a priority queue. */
int queue_rnum(QUEUE *queue){
  assert(queue);
  return vlrnum(queue->db);
}


/* Discard inferior records in a priority queue. */
int queue_slim(QUEUE *queue, int num){
  int i, diff;
  assert(queue && num >= 0);
  if((diff = vlrnum(queue->db) - num) < 1) return TRUE;
  vlcurlast(queue->db);
  for(i = 1; i < diff; i++){
    vlcurprev(queue->db);
  }
  while(TRUE){
    if(!vlcurout(queue->db)) break;
  }
  return vlrnum(queue->db) == num && !vlfatalerror(queue->db);
}


/* Add a word to a keyword map. */
void kwords_add(CBMAP *kwords, const char *word, int frequency){
  const char *vbuf;
  char numbuf[NUMBUFSIZ];
  int wlen, nlen;
  assert(kwords && word && frequency >= 0);
  wlen = strlen(word);
  if((vbuf = cbmapget(kwords, word, wlen, NULL)) != NULL) frequency += atoi(vbuf);
  nlen = sprintf(numbuf, "%d", frequency);
  cbmapput(kwords, word, wlen, numbuf, nlen, TRUE);
}


/* Reduce elements of a keyword map. */
void kwords_reduce(CBMAP *kwords, int num, int fadeout){
  KEYSC *scores;
  const char *vbuf;
  char numbuf[NUMBUFSIZ];
  int i, snum, vsiz;
  double basis;
  assert(kwords && num >= 0);
  if(num < 1) num = 1;
  snum = cbmaprnum(kwords);
  scores = cbmalloc(snum * sizeof(KEYSC) + 1);
  cbmapiterinit(kwords);
  for(i = 0; i < snum; i++){
    vbuf = cbmapiternext(kwords, &vsiz);
    scores[i].word = vbuf;
    scores[i].wsiz = vsiz;
    scores[i].pt = atoi(cbmapget(kwords, vbuf, vsiz, NULL));
  }
  qsort(scores, snum, sizeof(KEYSC), keysc_compare);
  basis = num * 1.1 + 1.0;
  for(i = 0; i < snum; i++){
    if(i < num){
      vsiz = sprintf(numbuf, "%d",
                     fadeout ? (int)(scores[i].pt * (basis - i) / basis) : scores[i].pt);
      cbmapput(kwords, scores[i].word, scores[i].wsiz, numbuf, vsiz, TRUE);
      cbmapmove(kwords, scores[i].word, scores[i].wsiz, FALSE);
    } else {
      cbmapout(kwords, scores[i].word, scores[i].wsiz);
    }
  }
  free(scores);
}


/* Fetch a document of a URL. */
int fetch_document(const char *url, const char *pxhost, int pxport, int outsec, time_t mdate,
                   const CBLIST *urlrules, const CBLIST *mtrules,
                   int *codep, CBDATUM *raw, CBMAP *heads,
                   CBLIST *links, const CBLIST *unrules, ESTDOC *doc, int lang){
  URLRULE *urlrule;
  MTRULE *mtrule;
  UNRULE *unrule;
  ESTDOC *rdoc;
  CBMAP *rheads;
  CBLIST *reqheads;
  CBDATUM *rbuf;
  const char *vbuf, *cmd;
  char *dstr, *tbuf, *type, *enc, *pv;
  int i, j, rescode;
  assert(url);
  rescode = -1;
  if(raw){
    rbuf = NULL;
  } else {
    rbuf = cbdatumopen(NULL, -1);
    raw = rbuf;
  }
  if(heads){
    rheads = NULL;
  } else {
    rheads = cbmapopenex(MINIBNUM);
    heads = rheads;
  }
  if(doc){
    rdoc = NULL;
  } else {
    rdoc = est_doc_new();
    doc = rdoc;
  }
  reqheads = cblistopen();
  if(mdate > 0){
    dstr = cbdatestrhttp(mdate, 0);
    tbuf = cbsprintf("If-Modified-Since: %s", dstr);
    cblistpush(reqheads, tbuf, -1);
    free(tbuf);
    free(dstr);
  }
  switch(lang){
  case ESTLANGEN:
    cblistpush(reqheads, "Accept-Language: en,ja", -1);
    break;
  case ESTLANGJA:
    cblistpush(reqheads, "Accept-Language: ja,en", -1);
    break;
  case ESTLANGZH:
    cblistpush(reqheads, "Accept-Language: zh,en", -1);
    break;
  case ESTLANGKO:
    cblistpush(reqheads, "Accept-Language: ko,en", -1);
    break;
  }
  if(!est_url_shuttle(url, pxhost, pxport, outsec, RESLIMSIZE, NULL, reqheads, NULL, 0,
                      &rescode, heads, raw) || rescode != 200){
    if(links && (rescode == 301 || rescode == 302) &&
       (vbuf = cbmapget(heads, "location", -1, NULL)) != NULL){
      tbuf = cburlresolve(url, vbuf);
      cblistpush(links, tbuf, -1);
      free(tbuf);
    }
    cblistclose(reqheads);
    if(rdoc) est_doc_delete(rdoc);
    if(rheads) cbmapclose(rheads);
    if(rbuf) cbdatumclose(rbuf);
    if(codep) *codep = rescode;
    return FALSE;
  }
  if(urlrules){
    for(i = 0; i < cblistnum(urlrules); i++){
      urlrule = (URLRULE *)cblistval(urlrules, i, NULL);
      if(est_regex_match(urlrule->regex, url)){
        cbmapput(heads, "content-type", -1, urlrule->type, -1, TRUE);
        break;
      }
    }
  }
  if(!(vbuf = cbmapget(heads, "content-type", -1, NULL))) vbuf = "text/plain";
  type = cbmemdup(vbuf, -1);
  if((pv = strchr(type, ';')) != NULL) *pv = '\0';
  cbstrtolower(type);
  enc = NULL;
  if((pv = strstr(vbuf, "charset=")) != NULL || (pv = strstr(vbuf, "CHARSET=")) != NULL){
    pv = strchr(pv, '=') + 1;
    if(*pv == '"') pv++;
    enc = cbmemdup(pv, -1);
    if((pv = strchr(enc, '"')) != NULL) *pv = '\0';
  }
  cmd = "";
  if(mtrules){
    for(i = 0; i < cblistnum(mtrules); i++){
      mtrule = (MTRULE *)cblistval(mtrules, i, NULL);
      if(est_regex_match(mtrule->regex, type)){
        cmd = mtrule->filter;
        break;
      }
    }
  } else if(!strcmp(type, "text/plain")){
    cmd = TEXTCMD;
  } else if(!strcmp(type, "text/html") || !strcmp(vbuf, "application/xhtml+xml")){
    cmd = HTMLCMD;
  } else if(!strcmp(type, "message/rfc822")){
    cmd = MIMECMD;
  }
  if(!strcmp(cmd, DRAFTCMD)){
    make_doc_from_draft(cbdatumptr(raw), cbdatumsize(raw), doc, links);
    if(!est_doc_attr(doc, ESTDATTRURI)) est_doc_add_attr(doc, ESTDATTRURI, url);
  } else if(!strcmp(cmd, TEXTCMD)){
    make_doc_from_text(cbdatumptr(raw), cbdatumsize(raw), enc, lang, doc, links);
    est_doc_add_attr(doc, ESTDATTRURI, url);
  } else if(!strcmp(cmd, HTMLCMD)){
    make_doc_from_html(cbdatumptr(raw), cbdatumsize(raw), enc, lang, doc, links);
    est_doc_add_attr(doc, ESTDATTRURI, url);
  } else if(!strcmp(cmd, MIMECMD)){
    make_doc_from_mime(cbdatumptr(raw), cbdatumsize(raw), enc, lang, doc, links);
    est_doc_add_attr(doc, ESTDATTRURI, url);
  } else if(cmd[0] != '\0'){
    make_doc_with_xcmd(cmd, url, cbdatumptr(raw), cbdatumsize(raw), enc, lang, doc, links);
    if(!est_doc_attr(doc, ESTDATTRURI)) est_doc_add_attr(doc, ESTDATTRURI, url);
  }
  free(enc);
  free(type);
  if((vbuf = cbmapget(heads, "last-modified", -1, NULL)) != NULL)
    est_doc_add_attr(doc, ESTDATTRMDATE, vbuf);
  if(links){
    for(i = 0; i < cblistnum(links); i++){
      tbuf = cburlresolve(url, cblistval(links, i, NULL));
      if(unrules){
        for(j = 0; j < cblistnum(unrules); j++){
          unrule = (UNRULE *)cblistval(unrules, j, NULL);
          if(!est_regex_match(unrule->regex, tbuf)) continue;
          dstr = est_regex_replace(tbuf, unrule->before, unrule->after);
          free(tbuf);
          tbuf = dstr;
        }
      }
      if(cbstrfwmatch(tbuf, "http://")){
        pv = tbuf + 7;
        if((pv = strchr(pv, '/')) != NULL && cbstrfwmatch(pv, "/%E2%80%BE")){
          pv[1] = '~';
          memmove(pv + 2, pv + 10, strlen(pv + 10) + 1);
        }
        cblistover(links, i, tbuf, -1);
      }
      free(tbuf);
    }
  }
  cblistclose(reqheads);
  if(rdoc) est_doc_delete(rdoc);
  if(rheads) cbmapclose(rheads);
  if(rbuf) cbdatumclose(rbuf);
  if(codep) *codep = rescode;
  return TRUE;
}



/*************************************************************************************************
 * private objects
 *************************************************************************************************/


/* Close the log file. */
static void log_close(void){
  if(log_fp) fclose(log_fp);
}


/* Output the log message of a DB event.
   `message' specifies the log message of a DB event.
   `opaque' is simply ignored. */
static void db_informer(const char *message, void *opaque){
  assert(message);
  log_print(LL_INFO, "DB-EVENT: %s", message);
}


/* Skip the label of a line.
   `str' specifies a string of a line.
   The return value is the pointer to the first character of the line. */
static const char *skiplabel(const char *str){
  assert(str);
  if(!(str = strchr(str, ':'))) return "";
  str++;
  while(*str != '\0' && (*str == ' ' || *str == '\t')){
    str++;
  }
  return str;
}


/* Make the absolute path of a relative path.
   `rootdir' specifies the path of the root directory.
   `path' specifies a ralative path of the a file. */
static char *makeabspath(const char *rootdir, const char *path){
  char mypath[URIBUFSIZ];
  assert(rootdir && path);
  if((ESTPATHCHR == '/' && path[0] == ESTPATHCHR) ||
     (ESTPATHCHR == '\\' && ((path[0] >= 'A' && path[0] <= 'Z') ||
                             (path[0] >= 'a' && path[0] <= 'z')) && path[1] == ':' &&
      path[2] == '\\')){
    sprintf(mypath, "%s", path);
  } else {
    sprintf(mypath, "%s%c%s", rootdir, ESTPATHCHR, path);
  }
  return est_realpath(mypath);
}


/* Compare keys of two records as double type objects.
   `aptr' specifies the pointer to the region of one key.
   `asiz' specifies the size of the region of one key.
   `bptr' specifies the pointer to the region of the other key.
   `bsiz' specifies the size of the region of the other key.
   The return value is positive if the former is big, negative if the latter is big, 0 if both
   are equivalent. */
static int queue_compare(const char *aptr, int asiz, const char *bptr, int bsiz){
  double anum, bnum;
  assert(aptr && asiz >= 0 && bptr && bsiz >= 0);
  if(asiz != bsiz) return asiz - bsiz;
  anum = (asiz == sizeof(double) ? *(double *)aptr : INT_MIN);
  bnum = (bsiz == sizeof(double) ? *(double *)bptr : INT_MIN);
  if(anum > bnum) return 1;
  if(anum < bnum) return -1;
  return 0;
}


/* Compare two keywords by scores in descending order.
   `ap' specifies the pointer to one keyword.
   `bp' specifies the pointer to the other keyword.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int keysc_compare(const void *ap, const void *bp){
  assert(ap && bp);
  return ((KEYSC *)bp)->pt - ((KEYSC *)ap)->pt;
}


/* Create a document object from docuemnt draft.
   `buf' specifies the pointer to a data buffer.  It should be trailed by zero code.
   `size' specifies the size of the buffer.
   `doc' specifies a document handle to store attributes and texts.
   `link' specifies a list handle to store links.  If it is `NULL', it is not used. */
static void make_doc_from_draft(const char *buf, int size, ESTDOC *doc, CBLIST *links){
  CBLIST *lines;
  const char *line;
  char *pv;
  int i;
  assert(buf && size >= 0 && doc);
  lines = cbsplit(buf, -1, "\n");
  for(i = 0; i < CB_LISTNUM(lines); i++){
    line = CB_LISTVAL(lines, i);
    while(*line > '\0' && *line <= ' '){
      line++;
    }
    if(*line == '\0'){
      i++;
      break;
    }
    if(*line != '%' && (pv = strchr(line, '=')) != NULL){
      *(pv++) = '\0';
      est_doc_add_attr(doc, line, pv);
    }
  }
  for(; i < CB_LISTNUM(lines); i++){
    line = CB_LISTVAL(lines, i);
    if(*line == '\t'){
      est_doc_add_hidden_text(doc, line + 1);
    } else {
      est_doc_add_text(doc, line);
    }
  }
  cblistclose(lines);
}


/* Create a document object from plain text.
   `buf' specifies the pointer to a data buffer.  It should be trailed by zero code.
   `size' specifies the size of the buffer.
   `penc' specifies the name of preferred encoding.  If it is `NULL', it is not used.
   `plang' specifies the code of preferred language.
   `doc' specifies a document handle to store attributes and texts.
   `link' specifies a list handle to store links.  If it is `NULL', it is not used. */
static void make_doc_from_text(const char *buf, int size, const char *penc, int plang,
                               ESTDOC *doc, CBLIST *links){
  CBLIST *lines;
  CBDATUM *datum;
  const char *enc, *text, *line;
  char *nbuf, numbuf[NUMBUFSIZ];
  int i;
  assert(buf && size >= 0 && doc);
  if(check_binary(buf, size)) return;
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
}


/* Check whether a buffer is binary.
   `buf' specifies the pointer to a data buffer.  It should be trailed by zero code.
   `size' specifies the size of the buffer. */
static int check_binary(const char *buf, int size){
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


/* Create a document object from HTML.
   `buf' specifies the pointer to a data buffer.  It should be trailed by zero code.
   `size' specifies the size of the buffer.
   `penc' specifies the name of preferred encoding.  If it is `NULL', it is not used.
   `plang' specifies the code of preferred language.
   `doc' specifies a document handle to store attributes and texts.
   `link' specifies a list handle to store links.  If it is `NULL', it is not used. */
static void make_doc_from_html(const char *buf, int size, const char *penc, int plang,
                               ESTDOC *doc, CBLIST *links){
  CBLIST *elems;
  CBMAP *attrs;
  CBDATUM *datum;
  const char *enc, *html, *elem, *next, *value, *name, *content, *rp;
  char *nbuf, *nenc, *rbuf, *lbuf, numbuf[NUMBUFSIZ];
  int i, esiz;
  assert(buf && size >= 0 && doc);
  enc = est_enc_name(buf, size, plang);
  html = NULL;
  nbuf = NULL;
  if(!strcmp(enc, "UTF-16") || !strcmp(enc, "UTF-16BE") || !strcmp(enc, "UTF-16LE")){
    nbuf = est_iconv(buf, size, enc, "UTF-8", NULL, NULL);
  } else if(!strcmp(enc, "US-ASCII")){
    nbuf = NULL;
  } else {
    if((nenc = penc ? cbmemdup(penc, -1) : html_enc(buf)) != NULL){
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
              rbuf = html_raw_text(content);
              est_doc_add_attr(doc, ESTDATTRAUTHOR, rbuf);
              free(rbuf);
            } else {
              est_doc_add_attr(doc, ESTDATTRAUTHOR, content);
            }
          } else if(!strcmp(lbuf, "refresh")){
            if(strchr(content, '&')){
              rbuf = html_raw_text(content);
              rp = rbuf;
              while((*rp >= '0' && *rp <= '9') || *rp == ' ' || *rp == '\t' || *rp == ';'){
                rp++;
              }
              if(cbstrfwmatch(rp, "url=")) rp += 4;
              if(*rp != '\0') cblistpush(links, rp, -1);
              free(rbuf);
            } else {
              rp = content;
              while((*rp >= '0' && *rp <= '9') || *rp == ' ' || *rp == '\t' || *rp == ';'){
                rp++;
              }
              if(cbstrfwmatch(rp, "url=")) rp += 4;
              if(*rp != '\0') cblistpush(links, rp, -1);
            }
          }
          if(name[0] != '@' && name[0] != '_'){
            if(strchr(content, '&')){
              rbuf = html_raw_text(content);
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
          rbuf = html_raw_text(next);
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
          rbuf = html_raw_text(CB_DATUMPTR(datum));
          est_doc_add_text(doc, rbuf);
          free(rbuf);
        } else {
          est_doc_add_text(doc, CB_DATUMPTR(datum));
        }
        CB_DATUMSETSIZE(datum, 0);
      } else if(links && (cbstrfwimatch(elem, "<a") || cbstrfwimatch(elem, "<link"))){
        attrs = cbxmlattrs(elem);
        value = cbmapget(attrs, "href", -1, NULL);
        if(!value) value = cbmapget(attrs, "HREF", -1, NULL);
        if(value && !cbstrfwimatch(value, "https:") && !cbstrfwimatch(value, "ftp:") &&
           !cbstrfwimatch(value, "mailto:") && !cbstrfwimatch(value, "javascript:"))
          cblistpush(links, value, -1);
        cbmapclose(attrs);
      } else if(links && cbstrfwimatch(elem, "<frame")){
        attrs = cbxmlattrs(elem);
        value = cbmapget(attrs, "src", -1, NULL);
        if(!value) value = cbmapget(attrs, "SRC", -1, NULL);
        if(value && !cbstrfwimatch(value, "https:") && !cbstrfwimatch(value, "ftp:") &&
           !cbstrfwimatch(value, "mailto:") && !cbstrfwimatch(value, "javascript:"))
          cblistpush(links, value, -1);
        cbmapclose(attrs);
      } else if(links && cbstrfwimatch(elem, "<object")){
        attrs = cbxmlattrs(elem);
        value = cbmapget(attrs, "data", -1, NULL);
        if(!value) value = cbmapget(attrs, "DATA", -1, NULL);
        if(value && !cbstrfwimatch(value, "https:") && !cbstrfwimatch(value, "ftp:") &&
           !cbstrfwimatch(value, "mailto:") && !cbstrfwimatch(value, "javascript:"))
          cblistpush(links, value, -1);
        cbmapclose(attrs);
      } else if(links && (cbstrfwimatch(elem, "<embed") || cbstrfwimatch(elem, "<iframe"))){
        attrs = cbxmlattrs(elem);
        value = cbmapget(attrs, "src", -1, NULL);
        if(!value) value = cbmapget(attrs, "SRC", -1, NULL);
        if(value && !cbstrfwimatch(value, "https:") && !cbstrfwimatch(value, "ftp:") &&
           !cbstrfwimatch(value, "mailto:") && !cbstrfwimatch(value, "javascript:"))
          cblistpush(links, value, -1);
        cbmapclose(attrs);
      }
    } else {
      CB_DATUMCAT(datum, " ", 1);
      CB_DATUMCAT(datum, elem, esiz);
    }
  }
  CB_LISTCLOSE(elems);
  if(strchr(CB_DATUMPTR(datum), '&')){
    rbuf = html_raw_text(CB_DATUMPTR(datum));
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
}


/* Get the encoding of an HTML string.
   `str' specifies string of HTML.
   The return value is the name of the character encoding.
   Because the region of the return value is allocated with the `malloc' call, it should be
   released with the `free' call if it is no longer in use. */
static char *html_enc(const char *str){
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


/* Unescape entity references of HTML.
   `str' specifies string of HTML.
   The return value is the result string.
   Because the region of the return value is allocated with the `malloc' call, it should be
   released with the `free' call if it is no longer in use. */
static char *html_raw_text(const char *html){
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


/* Create a document object from MIME.
   `buf' specifies the pointer to a data buffer.  It should be trailed by zero code.
   `size' specifies the size of the buffer.
   `penc' specifies the name of preferred encoding.  If it is `NULL', it is not used.
   `plang' specifies the code of preferred language.
   `doc' specifies a document handle to store attributes and texts.
   `link' specifies a list handle to store links.  If it is `NULL', it is not used. */
static void make_doc_from_mime(const char *buf, int size, const char *penc, int plang,
                               ESTDOC *doc, CBLIST *links){
  ESTDOC *tdoc;
  CBMAP *attrs;
  const CBLIST *texts;
  CBLIST *parts, *lines;
  CBDATUM *datum;
  const char *key, *val, *bound, *part, *text, *line;
  char *body, *swap, numbuf[NUMBUFSIZ];
  int i, j, bsiz, psiz, ssiz, mht;
  assert(buf && size >= 0 && doc);
  attrs = cbmapopenex(MINIBNUM);
  body = cbmimebreak(buf, size, attrs, &bsiz);
  if((val = cbmapget(attrs, "subject", -1, NULL)) != NULL){
    doc_add_attr_mime(doc, ESTDATTRTITLE, val);
    if((val = est_doc_attr(doc, ESTDATTRTITLE)) != NULL) est_doc_add_hidden_text(doc, val);
  }
  if((val = cbmapget(attrs, "from", -1, NULL)) != NULL)
    doc_add_attr_mime(doc, ESTDATTRAUTHOR, val);
  if((val = cbmapget(attrs, "date", -1, NULL)) != NULL){
    doc_add_attr_mime(doc, ESTDATTRCDATE, val);
    doc_add_attr_mime(doc, ESTDATTRMDATE, val);
  }
  est_doc_add_attr(doc, ESTDATTRTYPE, "message/rfc822");
  sprintf(numbuf, "%d", size);
  est_doc_add_attr(doc, ESTDATTRSIZE, numbuf);
  cbmapiterinit(attrs);
  while((key = cbmapiternext(attrs, NULL)) != NULL){
    if((key[0] >= 'A' && key[0] <= 'Z') || key[0] == '@' || key[0] == '_') continue;
    val = cbmapget(attrs, key, -1, NULL);
    doc_add_attr_mime(doc, key, val);
  }
  if((key = cbmapget(attrs, "TYPE", -1, NULL)) != NULL && cbstrfwimatch(key, "multipart/")){
    mht = cbstrfwimatch(key, "multipart/related");
    if((bound = cbmapget(attrs, "BOUNDARY", -1, NULL)) != NULL){
      parts = cbmimeparts(body, bsiz, bound);
      for(i = 0; i < CB_LISTNUM(parts) && i < 8; i++){
        part = CB_LISTVAL2(parts, i, psiz);
        tdoc = est_doc_new();
        make_doc_from_mime(part, psiz, penc, plang, tdoc, links);
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
      if(!check_binary(body, bsiz)){
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
      tdoc = est_doc_new();
      make_doc_from_html(body, bsiz, penc, plang, tdoc, links);
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
    } else if(cbstrfwimatch(key, "message/rfc822")){
      tdoc = est_doc_new();
      make_doc_from_mime(body, bsiz, penc, plang, tdoc, links);
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
    } else if(cbstrfwimatch(key, "text/")){
      tdoc = est_doc_new();
      make_doc_from_text(body, bsiz, penc, plang, tdoc, links);
      texts = est_doc_texts(tdoc);
      for(i = 0; i < CB_LISTNUM(texts); i++){
        text = CB_LISTVAL(texts, i);
        est_doc_add_text(doc, text);
      }
      est_doc_delete(tdoc);
    }
  }
  free(body);
  cbmapclose(attrs);
}


/* set mime value as an attribute of a document */
static void doc_add_attr_mime(ESTDOC *doc, const char *name, const char *value){
  char enc[64], *ebuf, *rbuf;
  assert(doc && name && value);
  ebuf = cbmimedecode(value, enc);
  if((rbuf = est_iconv(ebuf, -1, enc, "UTF-8", NULL, NULL)) != NULL){
    est_doc_add_attr(doc, name, rbuf);
    free(rbuf);
  }
  free(ebuf);
}


/* Create a document object with an outer command.
   `xcmd' specifies an outer command line.
   `buf' specifies the pointer to a data buffer.  It should be trailed by zero code.
   `size' specifies the size of the buffer.
   `url' specifies the URL of the target document.
   `penc' specifies the name of preferred encoding.  If it is `NULL', it is not used.
   `plang' specifies the code of preferred language.
   `doc' specifies a document handle to store attributes and texts.
   `link' specifies a list handle to store links.  If it is `NULL', it is not used. */
static void make_doc_with_xcmd(const char *xcmd, const char *url, const char *buf, int size,
                               const char *penc, int plang, ESTDOC *doc, CBLIST *links){
  const char *tmpdir, *pv, *ext, *fmt;
  char iname[URIBUFSIZ], oname[URIBUFSIZ], cmd[URIBUFSIZ];
  char *rbuf, numbuf[NUMBUFSIZ];
  int rnd, pid, rsiz;
  struct stat sbuf;
  assert(buf && size >= 0 && url && xcmd);
  if(ESTPATHCHR == '/' && stat("/tmp", &sbuf) == 0){
    tmpdir = "/tmp";
  } else if(ESTPATHCHR == '\\' &&
            ((pv = getenv("TMP")) != NULL || (pv = getenv("TEMP")) != NULL) &&
            stat(pv, &sbuf) == 0){
    tmpdir = pv;
  } else {
    tmpdir = ESTCDIRSTR;
  }
  ext = NULL;
  if((pv = strrchr(url, ESTPATHCHR)) != NULL) url = pv;
  if((pv = strrchr(url, ESTEXTCHR)) != NULL) ext = pv;
  if(!ext || strlen(ext) >= 32 || strchr(ext, '"') || strchr(ext, '\\')) ext = "";
  rnd = dpouterhash(url, -1) & 0xffff;
  pid = (int)getpid() & 0xffff;
  sprintf(iname, "%s%cxcmd-in-%04X%04X%s", tmpdir, ESTPATHCHR, pid, rnd, ext);
  sprintf(oname, "%s%cxcmd-out-%04X%04X%cest", tmpdir, ESTPATHCHR, pid, rnd, ESTEXTCHR);
  fmt = DRAFTCMD;
  if(cbstrfwmatch(xcmd, "T@")){
    fmt = TEXTCMD;
    xcmd += 2;
  } else if(cbstrfwmatch(xcmd, "H@")){
    fmt = HTMLCMD;
    xcmd += 2;
  } else if(cbstrfwmatch(xcmd, "M@")){
    fmt = MIMECMD;
    xcmd += 2;
  }
  cbwritefile(iname, buf, size);
  sprintf(cmd, "%s \"%s\" \"%s\"", xcmd, iname, oname);
  system(cmd);
  if((rbuf = cbreadfile(oname, &rsiz)) != NULL){
    if(fmt == DRAFTCMD){
      make_doc_from_draft(rbuf, rsiz, doc, links);
    } else if(fmt == TEXTCMD){
      make_doc_from_text(rbuf, rsiz, penc, plang, doc, links);
    } else if(fmt == HTMLCMD){
      make_doc_from_html(rbuf, rsiz, penc, plang, doc, links);
    } else if(fmt == MIMECMD){
      make_doc_from_mime(rbuf, rsiz, penc, plang, doc, links);
    }
    free(rbuf);
  }
  if(doc && fmt != NULL){
    sprintf(numbuf, "%d", size);
    est_doc_add_attr(doc, ESTDATTRSIZE, numbuf);
    est_doc_add_attr(doc, ESTDATTRTYPE, est_ext_type(ext));
  }
  unlink(oname);
  unlink(iname);
}



/* END OF FILE */
