/*************************************************************************************************
 * Implementation of mastermod
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


/* private function prototypes */
static void log_close(void);
static void db_informer(const char *message, void *opaque);
static int resdoc_compare_by_score(const void *ap, const void *bp);
static int resdoc_compare_by_str_asc(const void *ap, const void *bp);
static int resdoc_compare_by_str_desc(const void *ap, const void *bp);
static int resdoc_compare_by_num_asc(const void *ap, const void *bp);
static int resdoc_compare_by_num_desc(const void *ap, const void *bp);



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
int master_init(const char *rootdir){
  DEPOT *depot;
  FILE *ofp;
  char path[URIBUFSIZ];
  int err;
  assert(rootdir);
  if(est_mkdir(rootdir) == -1 && errno != EEXIST) return FALSE;
  err = FALSE;
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, METAFILE);
  if((depot = dpopen(path, DP_OWRITER | DP_OCREAT | DP_OTRUNC, MINIBNUM))){
    if(!dpput(depot, MMKMAGIC, -1, MMKMAGVAL, -1, DP_DKEEP)) err = TRUE;
    if(!dpclose(depot)) err = TRUE;
  } else {
    err = TRUE;
  }
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, CONFFILE);
  if((ofp = fopen(path, "wb")) != NULL){
    fprintf(ofp, "# binding address of TCP (0.0.0.0 means every address)\n");
    fprintf(ofp, "bindaddr: 0.0.0.0\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# port number of TCP\n");
    fprintf(ofp, "portnum: 1978\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# public URL (absolute URL)\n");
    fprintf(ofp, "publicurl:\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# running mode (1:normal, 2:readonly)\n");
    fprintf(ofp, "runmode: 1\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# authorization mode (1:none, 2:admin, 3:all)\n");
    fprintf(ofp, "authmode: 2\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# maximum length of data to receive (in kilobytes)\n");
    fprintf(ofp, "recvmax: 1024\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# maximum number of connections at the same time\n");
    fprintf(ofp, "maxconn: 30\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# idle time to start flushing (in seconds)\n");
    fprintf(ofp, "idleflush: 20\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# idle time to start synchronizing (in seconds)\n");
    fprintf(ofp, "idlesync: 300\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# timeout of a session (in seconds)\n");
    fprintf(ofp, "sessiontimeout: 600\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# timeout of search (in seconds)\n");
    fprintf(ofp, "searchtimeout: 15\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# maximum number of documents to send\n");
    fprintf(ofp, "searchmax: 1000\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# maximum depth of meta search\n");
    fprintf(ofp, "searchdepth: 5\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# whether to rate URI for scoring (0:no, 1:yes)\n");
    fprintf(ofp, "rateuri: 1\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# merge method of meta search (1:score, 2:score and rank, 3:rank)\n");
    fprintf(ofp, "mergemethod: 2\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# host name of the proxy\n");
    fprintf(ofp, "proxyhost:\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# port number of the proxy\n");
    fprintf(ofp, "proxyport:\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# path of the log file (relative path or absolute path)\n");
    fprintf(ofp, "logfile: %s\n", LOGFILE);
    fprintf(ofp, "\n");
    fprintf(ofp, "# logging level (1:debug, 2:information, 3:warning, 4:error, 5:none)\n");
    fprintf(ofp, "loglevel: 2\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# command for backup (absolute path of a command)\n");
    fprintf(ofp, "backupcmd:\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# scale prediction (1:small, 2:medium, 3:large, 4:huge)\n");
    fprintf(ofp, "scalepred: 2\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# score expression (1:void, 2:char, 3:int, 4:asis)\n");
    fprintf(ofp, "scoreexpr: 2\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# attribute indexes (attribute name and data type)\n");
    fprintf(ofp, "attrindex: @mdate{{!}}seq\n");
    fprintf(ofp, "attrindex: @title{{!}}str\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# document root directory (absolute path of a directory to be public)\n");
    fprintf(ofp, "docroot:\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# index file (name of directory index files)\n");
    fprintf(ofp, "indexfile:\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# decimal IP addresses of trusted nodes\n");
    fprintf(ofp, "trustednode:\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# whether to deny all nodes except for trusted nodes (0:no, 1:yes)\n");
    fprintf(ofp, "denyuntrusted: 0\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# maximum size of the index cache (in megabytes)\n");
    fprintf(ofp, "cachesize: 64\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# maximum number of cached records for document attributes\n");
    fprintf(ofp, "cacheanum: 8192\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# maximum number of cached records for document texts\n");
    fprintf(ofp, "cachetnum: 1024\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# maximum number of cached records for occurrence results\n");
    fprintf(ofp, "cachernum: 256\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# name of the attribute of the special cache\n");
    fprintf(ofp, "specialcache:\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# lower limit of cache usage to use the helper\n");
    fprintf(ofp, "helpershift: 0.9\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# maximum number of expansion of wild cards\n");
    fprintf(ofp, "wildmax: 256\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# text size limitation of indexing documents (in kilobytes)\n");
    fprintf(ofp, "limittextsize: 128\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# whole width of the snippet of each shown document\n");
    fprintf(ofp, "snipwwidth: 480\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# width of strings picked up from the beginning of the text\n");
    fprintf(ofp, "sniphwidth: 96\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# width of strings picked up around each highlighted word\n");
    fprintf(ofp, "snipawidth: 96\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# whether to check documents by scanning (0:no, 1:yes)\n");
    fprintf(ofp, "scancheck: 1\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# number of keywords for similarity search (0 means disabled)\n");
    fprintf(ofp, "smlrvnum: 32\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# number of documents for delay of keyword extraction\n");
    fprintf(ofp, "extdelay: 4096\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# e-mail address of the administrator\n");
    fprintf(ofp, "adminemail: magnus@hyperestraier.gov\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# expressions to replace the URI of each document\n");
    fprintf(ofp, "uireplace: ^file:///home/mikio/public_html/{{!}}http://localhost/\n");
    fprintf(ofp, "uireplace: /index\\.html?${{!}}/\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# extra attributes to be shown\n");
    fprintf(ofp, "uiextattr: @author|Author\n");
    fprintf(ofp, "uiextattr: @mdate|Modification Date\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# mode of phrase form"
            " (1:usual, 2:simplified, 3:rough, 4:union: 5:intersection)\n");
    fprintf(ofp, "uiphraseform: 2\n");
    fprintf(ofp, "\n");
    fprintf(ofp, "# tuning parameters for similarity search\n");
    fprintf(ofp, "uismlrtune: 16 1024 4096\n");
    fprintf(ofp, "\n");
    if(fclose(ofp) == EOF) err = TRUE;
  } else {
    err = TRUE;
  }
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, PIDFILE);
  unlink(path);
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, STOPFILE);
  unlink(path);
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, USERFILE);
  if((ofp = fopen(path, "wb")) != NULL){
    if(fclose(ofp) == EOF) err = TRUE;
  } else {
    err = TRUE;
  }
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, LOGFILE);
  if((ofp = fopen(path, "wb")) != NULL){
    if(fclose(ofp) == EOF) err = TRUE;
  } else {
    err = TRUE;
  }
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, NODEDIR);
  est_rmdir_rec(path);
  if(est_mkdir(path) == -1) err = TRUE;
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, SESSDIR);
  est_rmdir_rec(path);
  if(est_mkdir(path) == -1) err = TRUE;
  return err ? FALSE : TRUE;
}


/* Get the PID of the process locking the root directory. */
int lockerpid(const char *rootdir){
  char path[URIBUFSIZ], *vbuf;
  int pid;
  pid = -1;
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, PIDFILE);
  if((vbuf = cbreadfile(path, NULL)) != NULL){
    pid = atoi(vbuf);
    free(vbuf);
  }
  return pid;
}


/* Check whether a name includes alpha numeric characters only. */
int check_alnum_name(const char *name){
  while(*name != '\0'){
    if(!(*name >= 'a' && *name <= 'z') && !(*name >= '0' && *name <= '9') &&
       *name != '-' && *name != '_' && *name != '.'){
      return FALSE;
    }
    name++;
  }
  return TRUE;
}


/* Create a user manager object. */
UMGR *umgr_new(const char *rootdir){
  UMGR *umgr;
  assert(rootdir);
  log_print(LL_INFO, "starting the user manager");
  umgr = cbmalloc(sizeof(UMGR));
  umgr->rootdir = cbmemdup(rootdir, -1);
  umgr->users = cbmapopen();
  return umgr;
}


/* Destroy a user manager object. */
int umgr_delete(UMGR *umgr){
  USER *user;
  const char *kbuf, *vbuf;
  int err;
  assert(umgr);
  log_print(LL_INFO, "finishing the user manager");
  err = FALSE;
  if(!umgr_sync(umgr)) err = TRUE;
  cbmapiterinit(umgr->users);
  while((kbuf = cbmapiternext(umgr->users, NULL)) != NULL){
    vbuf = cbmapiterval(kbuf, NULL);
    user = (USER *)vbuf;
    pthread_mutex_destroy(&(user->mutex));
    if(user->sess) cbmapclose(user->sess);
    free(user->misc);
    free(user->fname);
    free(user->flags);
    free(user->passwd);
    free(user->name);
  }
  cbmapclose(umgr->users);
  free(umgr->rootdir);
  free(umgr);
  return err ? FALSE : TRUE;
}


/* Load all users from the user file. */
int umgr_load(UMGR *umgr){
  CBLIST *lines, *elems;
  const char *line;
  char path[URIBUFSIZ];
  int i, size;
  assert(umgr);
  log_print(LL_INFO, "loading the user list");
  sprintf(path, "%s%c%s", umgr->rootdir, ESTPATHCHR, USERFILE);
  if(!(lines = cbreadlines(path))){
    log_print(LL_ERROR, "loading the user list failed");
    return FALSE;
  }
  for(i = 0; i < cblistnum(lines); i++){
    line = cblistval(lines, i, &size);
    if(size < 1) continue;
    elems = cbsplit(line, size, "\t");
    if(cblistnum(elems) >= 5){
      umgr_put(umgr, cblistval(elems, 0, NULL), cblistval(elems, 1, NULL),
               cblistval(elems, 2, NULL), cblistval(elems, 3, NULL), cblistval(elems, 4, NULL));
    } else {
      log_print(LL_WARN, "invalid line: %d", i + 1);
    }
    cblistclose(elems);
  }
  cblistclose(lines);
  return TRUE;
}


/* Synchronize all users into the user file. */
int umgr_sync(UMGR *umgr){
  FILE *ofp;
  USER *user;
  const char *kbuf, *vbuf;
  char path[URIBUFSIZ];
  int err;
  assert(umgr);
  log_print(LL_INFO, "saving the user list");
  sprintf(path, "%s%c%s", umgr->rootdir, ESTPATHCHR, USERFILE);
  if(!(ofp = fopen(path, "wb"))){
    log_print(LL_ERROR, "synchronizing the user list failed");
    return FALSE;
  }
  err = FALSE;
  cbmapiterinit(umgr->users);
  while((kbuf = cbmapiternext(umgr->users, NULL)) != NULL){
    vbuf = cbmapiterval(kbuf, NULL);
    user = (USER *)vbuf;
    fprintf(ofp, "%s\t%s\t%s\t%s\t%s\n",
            user->name, user->passwd, user->flags, user->fname, user->misc);
  }
  if(fclose(ofp) == EOF){
    log_print(LL_ERROR, "saving the user list failed");
    err = TRUE;
  }
  return err ? FALSE : TRUE;
}


/* Add a user to a user manager object. */
int umgr_put(UMGR *umgr, const char *name, const char *passwd, const char *flags,
             const char *fname, const char *misc){
  USER user;
  assert(umgr && name && passwd && flags && fname && misc);
  log_print(LL_DEBUG, "umgr_put: %s:%s:%s:%s:%s", name, passwd, flags, fname, misc);
  if(name[0] == '\0' || cbmapget(umgr->users, name, -1, NULL)){
    log_print(LL_WARN, "duplicated or empty user name: %s", name);
    return FALSE;
  }
  if(!check_alnum_name(name)){
    log_print(LL_WARN, "invalid user name: %s", name);
    return FALSE;
  }
  user.name = cbmemdup(name, -1);
  user.passwd = cbmemdup(passwd, -1);
  user.flags = cbmemdup(flags, -1);
  user.fname = cbmemdup(fname, -1);
  user.misc = cbmemdup(misc, -1);
  user.atime = 0;
  user.sess = NULL;
  pthread_mutex_init(&(user.mutex), NULL);
  cbmapput(umgr->users, name, -1, (char *)&user, sizeof(USER), FALSE);
  return TRUE;
}


/* Remove a user from a user manager object. */
int umgr_out(UMGR *umgr, const char *name){
  USER *user;
  const char *vbuf;
  assert(umgr && name);
  log_print(LL_DEBUG, "umgr_out: %s", name);
  if(!(vbuf = cbmapget(umgr->users, name, -1, NULL))) return FALSE;
  user = (USER *)vbuf;
  pthread_mutex_destroy(&(user->mutex));
  if(user->sess) cbmapclose(user->sess);
  free(user->misc);
  free(user->fname);
  free(user->flags);
  free(user->passwd);
  free(user->name);
  cbmapout(umgr->users, name, -1);
  return TRUE;
}


/* Get a list of names of users in a user manager object. */
CBLIST *umgr_names(UMGR *umgr){
  CBLIST *names;
  assert(umgr);
  names = cbmapkeys(umgr->users);
  cblistsort(names);
  return names;
}


/* Get a user object in a user manager object. */
USER *umgr_get(UMGR *umgr, const char *name){
  const char *vbuf;
  assert(umgr && name);
  if(!(vbuf = cbmapget(umgr->users, name, -1, NULL))) return NULL;
  return (USER *)vbuf;
}


/* Make the session of a user object. */
void user_make_sess(USER *user){
  assert(user);
  if(pthread_mutex_lock(&(user->mutex)) != 0) return;
  if(user->sess) cbmapclose(user->sess);
  user->sess = cbmapopenex(MINIBNUM);
  pthread_mutex_unlock(&(user->mutex));
}


/* Clear the session of a user object. */
void user_clear_sess(USER *user){
  assert(user);
  if(pthread_mutex_lock(&(user->mutex)) != 0) return;
  if(user->sess) cbmapclose(user->sess);
  user->sess = NULL;
  pthread_mutex_unlock(&(user->mutex));
}


/* Set a session variable of a user object. */
void user_set_sess_val(USER *user, const char *name, const char *value){
  assert(user && name);
  if(pthread_mutex_lock(&(user->mutex)) != 0) return;
  if(user->sess){
    if(value){
      cbmapput(user->sess, name, -1, value, -1, TRUE);
    } else {
      cbmapout(user->sess, name, -1);
    }
  }
  pthread_mutex_unlock(&(user->mutex));
}


/* Get the value of a session variable of a user object. */
char *user_sess_val(USER *user, const char *name){
  const char *value;
  char *rv;
  assert(user && name);
  if(pthread_mutex_lock(&(user->mutex)) != 0) return NULL;
  value = user->sess ? cbmapget(user->sess, name, -1, NULL) : NULL;
  rv = value ? cbmemdup(value, -1) : NULL;
  pthread_mutex_unlock(&(user->mutex));
  return rv;
}


/* Create a node manager object. */
NMGR *nmgr_new(const char *rootdir){
  NMGR *nmgr;
  assert(rootdir);
  log_print(LL_INFO, "starting the node manager");
  nmgr = cbmalloc(sizeof(NMGR));
  nmgr->rootdir = cbmemdup(rootdir, -1);
  nmgr->nodes = cbmapopenex(MINIBNUM);
  nmgr->aidxs = cbmapopenex(MINIBNUM);
  return nmgr;
}


/* Destroy a node manager object. */
int nmgr_delete(NMGR *nmgr){
  NODE *node;
  const char *kbuf, *vbuf;
  int err, ecode;
  assert(nmgr);
  log_print(LL_INFO, "finishing the node manager");
  err = FALSE;
  if(!nmgr_sync(nmgr, FALSE)) err = TRUE;
  cbmapclose(nmgr->aidxs);
  cbmapiterinit(nmgr->nodes);
  while((kbuf = cbmapiternext(nmgr->nodes, NULL)) != NULL){
    vbuf = cbmapiterval(kbuf, NULL);
    node = (NODE *)vbuf;
    pthread_mutex_destroy(&(node->mutex));
    cbmapclose(node->links);
    cbmapclose(node->users);
    cbmapclose(node->admins);
    free(node->label);
    free(node->name);
    est_mtdb_close(node->db, &ecode);
  }
  cbmapclose(nmgr->nodes);
  free(nmgr->rootdir);
  free(nmgr);
  return err ? FALSE : TRUE;
}


/* Load all nodes from the node directory. */
int nmgr_load(NMGR *nmgr, int wmode){
  CBLIST *list;
  const char *file;
  char path[URIBUFSIZ];
  int i, err;
  assert(nmgr);
  sprintf(path, "%s%c%s", nmgr->rootdir, ESTPATHCHR, NODEDIR);
  if(!(list = cbdirlist(path))){
    log_print(LL_ERROR, "loading the node directory failed");
    return FALSE;
  }
  err = FALSE;
  for(i = 0; i < cblistnum(list); i++){
    file = cblistval(list, i, NULL);
    if(!strcmp(file, ESTCDIRSTR) || !strcmp(file, ESTPDIRSTR)) continue;
    if(!nmgr_put(nmgr, file, wmode, 0)) err = TRUE;
  }
  cblistclose(list);
  return err ? FALSE : TRUE;
}


/* Synchronize all nodes into the node directory. */
int nmgr_sync(NMGR *nmgr, int phis){
  NODE *node;
  CBDATUM *datum;
  const char *kbuf, *vbuf;
  int err, ksiz, vsiz;
  assert(nmgr);
  log_print(LL_INFO, "synchronizing the node manager");
  err = FALSE;
  cbmapiterinit(nmgr->nodes);
  while((kbuf = cbmapiternext(nmgr->nodes, NULL)) != NULL){
    vbuf = cbmapiterval(kbuf, NULL);
    node = (NODE *)vbuf;
    est_mtdb_add_meta(node->db, NMKNAME, node->name);
    est_mtdb_add_meta(node->db, NMKLABEL, node->label);
    datum = cbdatumopen(NULL, -1);
    cbmapiterinit(node->admins);
    while((kbuf = cbmapiternext(node->admins, &ksiz)) != NULL){
      cbdatumcat(datum, kbuf, ksiz);
      cbdatumcat(datum, "\n", 1);
    }
    est_mtdb_add_meta(node->db, NMKADMINS, cbdatumptr(datum));
    cbdatumclose(datum);
    datum = cbdatumopen(NULL, -1);
    cbmapiterinit(node->users);
    while((kbuf = cbmapiternext(node->users, &ksiz)) != NULL){
      cbdatumcat(datum, kbuf, ksiz);
      cbdatumcat(datum, "\n", 1);
    }
    est_mtdb_add_meta(node->db, NMKUSERS, cbdatumptr(datum));
    cbdatumclose(datum);
    datum = cbdatumopen(NULL, -1);
    cbmapiterinit(node->links);
    while((kbuf = cbmapiternext(node->links, &ksiz)) != NULL){
      vbuf = cbmapiterval(kbuf, &vsiz);
      cbdatumcat(datum, kbuf, ksiz);
      cbdatumcat(datum, "\t", 1);
      cbdatumcat(datum, vbuf, vsiz);
      cbdatumcat(datum, "\n", 1);
    }
    est_mtdb_add_meta(node->db, NMKLINKS, cbdatumptr(datum));
    cbdatumclose(datum);
    if(phis && !est_mtdb_sync(node->db)){
      log_print(LL_ERROR, "DB-ERROR: %s", est_err_msg(est_mtdb_error(node->db)));
      err = TRUE;
    }
  }
  return err ? FALSE : TRUE;
}


/* Add an attribute index to a node manager object. */
void nmgr_add_aidx(NMGR *nmgr, const char *name, const char *type){
  int tnum;
  assert(nmgr && name && type);
  if(!cbstricmp(type, "str")){
    tnum = ESTIDXATTRSTR;
  } else if(!cbstricmp(type, "num")){
    tnum = ESTIDXATTRNUM;
  } else {
    tnum = ESTIDXATTRSEQ;
  }
  cbmapput(nmgr->aidxs, name, -1, (char *)&tnum, sizeof(int), TRUE);
}


/* Add a node to a node manager object. */
int nmgr_put(NMGR *nmgr, const char *name, int wmode, int options){
  NODE node;
  ESTMTDB *db;
  CBLIST *list;
  const char *cbuf, *pv;
  char pbuf[URIBUFSIZ], *vbuf;
  int i, ecode, csiz;
  assert(nmgr && name);
  log_print(LL_DEBUG, "nmgr_put: %s", name);
  if(name[0] == '\0' || cbmapget(nmgr->nodes, name, -1, NULL)){
    log_print(LL_WARN, "duplicated or empty node name: %s", name);
    return FALSE;
  }
  if(strlen(name) >= NODENAMEMAX || !check_alnum_name(name)){
    log_print(LL_WARN, "invalid node name: %s", name);
    return FALSE;
  }
  log_print(LL_INFO, "opening a node (%s): %s", wmode ? "WRITER" : "READER", name);
  sprintf(pbuf, "%s%c%s%c%s", nmgr->rootdir, ESTPATHCHR, NODEDIR, ESTPATHCHR, name);
  if(!(db = est_mtdb_open(pbuf, wmode ? ESTDBWRITER | ESTDBCREAT | options : ESTDBREADER,
                          &ecode))){
    log_print(LL_ERROR, "DB-ERROR: %s", est_err_msg(ecode));
    return FALSE;
  }
  est_mtdb_set_informer(db, db_informer, NULL);
  cbmapiterinit(nmgr->aidxs);
  while((cbuf = cbmapiternext(nmgr->aidxs, NULL)) != NULL){
    est_mtdb_add_attr_index(db, cbuf, *(int *)cbmapiterval(cbuf, NULL));
  }
  node.db = db;
  est_mtdb_add_meta(db, NMKNAME, name);
  node.name = cbmemdup(name, -1);
  vbuf = est_mtdb_meta(db, NMKLABEL);
  node.label = vbuf ? vbuf : cbmemdup(name, -1);
  if((vbuf = est_mtdb_meta(db, NMKADMINS)) != NULL){
    list = cbsplit(vbuf, -1, "\n");
    node.admins = cbmapopenex(cblistnum(list) + MINIBNUM);
    for(i = 0; i < cblistnum(list); i++){
      cbuf = cblistval(list, i, &csiz);
      if(csiz < 1) continue;
      cbmapput(node.admins, cbuf, csiz, "", 0, FALSE);
    }
    cblistclose(list);
    free(vbuf);
  } else {
    node.admins = cbmapopenex(MINIBNUM);
  }
  if((vbuf = est_mtdb_meta(db, NMKUSERS)) != NULL){
    list = cbsplit(vbuf, -1, "\n");
    node.users = cbmapopenex(cblistnum(list) + MINIBNUM);
    for(i = 0; i < cblistnum(list); i++){
      cbuf = cblistval(list, i, &csiz);
      if(csiz < 1) continue;
      cbmapput(node.users, cbuf, csiz, "", 0, FALSE);
    }
    cblistclose(list);
    free(vbuf);
  } else {
    node.users = cbmapopenex(MINIBNUM);
  }
  if((vbuf = est_mtdb_meta(db, NMKLINKS)) != NULL){
    list = cbsplit(vbuf, -1, "\n");
    node.links = cbmapopenex(cblistnum(list) + MINIBNUM);
    for(i = 0; i < cblistnum(list); i++){
      cbuf = cblistval(list, i, NULL);
      if(!(pv = strchr(cbuf, '\t'))) continue;
      cbmapput(node.links, cbuf, pv - cbuf, pv + 1, -1, FALSE);
    }
    cblistclose(list);
    free(vbuf);
  } else {
    node.links = cbmapopenex(MINIBNUM);
  }
  node.mtime = time(NULL);
  node.dirty = FALSE;
  pthread_mutex_init(&(node.mutex), NULL);
  cbmapput(nmgr->nodes, name, -1, (char *)&node, sizeof(NODE), FALSE);
  return TRUE;
}


/* Remove a node from a node manager object. */
int nmgr_out(NMGR *nmgr, const char *name){
  NODE *node;
  const char *vbuf;
  char pbuf[URIBUFSIZ];
  int err, ecode;
  assert(nmgr && name);
  log_print(LL_DEBUG, "nmgr_out: %s", name);
  if(!(vbuf = cbmapget(nmgr->nodes, name, -1, NULL))) return FALSE;
  err = FALSE;
  node = (NODE *)vbuf;
  pthread_mutex_destroy(&(node->mutex));
  cbmapclose(node->links);
  cbmapclose(node->users);
  cbmapclose(node->admins);
  free(node->label);
  free(node->name);
  if(!est_mtdb_close(node->db, &ecode)){
    log_print(LL_ERROR, "DB-ERROR: %s", est_err_msg(ecode));
    err = TRUE;
  }
  sprintf(pbuf, "%s%c%s%c%s", nmgr->rootdir, ESTPATHCHR, NODEDIR, ESTPATHCHR, name);
  if(!est_rmdir_rec(pbuf)){
    log_print(LL_ERROR, "could not remove a directory");
    err = TRUE;
  }
  cbmapout(nmgr->nodes, name, -1);
  return TRUE;
}


/* Clear registered documents in a node in a node manager object. */
int nmgr_clear(NMGR *nmgr, const char *name, int options){
  NODE *node;
  CBMAP *admins, *users, *links;
  const char *vbuf;
  char *label;
  if(!(vbuf = cbmapget(nmgr->nodes, name, -1, NULL))) return FALSE;
  node = (NODE *)vbuf;
  label = cbmemdup(node->label, -1);
  admins = cbmapdup(node->admins);
  users = cbmapdup(node->users);
  links = cbmapdup(node->links);
  if(!nmgr_out(nmgr, name) || !nmgr_put(nmgr, name, TRUE, options)){
    cbmapclose(links);
    cbmapclose(users);
    cbmapclose(admins);
    free(label);
    return FALSE;
  }
  if(!(vbuf = cbmapget(nmgr->nodes, name, -1, NULL))) return FALSE;
  node = (NODE *)vbuf;
  cbmapclose(node->links);
  cbmapclose(node->users);
  cbmapclose(node->admins);
  free(node->label);
  node->label = label;
  node->admins = admins;
  node->users = users;
  node->links = links;
  return TRUE;
}


/* Get a list of names of nodes in a noder manager object. */
CBLIST *nmgr_names(NMGR *nmgr){
  CBLIST *names;
  assert(nmgr);
  names = cbmapkeys(nmgr->nodes);
  cblistsort(names);
  return names;
}


/* Get a node object in a node manager object. */
NODE *nmgr_get(NMGR *nmgr, const char *name){
  const char *vbuf;
  assert(nmgr && name);
  if(!(vbuf = cbmapget(nmgr->nodes, name, -1, NULL))) return NULL;
  return (NODE *)vbuf;
}


/* Set a link object of a node. */
void node_set_link(NODE *node, const char *url, const char *label, int credit){
  char *vbuf;
  assert(node && url);
  if(!label || credit < 0){
    cbmapout(node->links, url, -1);
    return;
  }
  vbuf = cbsprintf("%s\t%d", label, credit);
  cbmapput(node->links, url, -1, vbuf, -1, TRUE);
  free(vbuf);
}


/* Create a read-write lock object. */
RWLOCK *rwlock_new(void){
  RWLOCK *rwlock;
  rwlock = cbmalloc(sizeof(RWLOCK));
  rwlock->readers = 0;
  rwlock->writers = 0;
  pthread_mutex_init(&(rwlock->mutex), NULL);
  pthread_cond_init(&(rwlock->cond), NULL);
  return rwlock;
}


/* Destroy a read-write lock object. */
void rwlock_delete(RWLOCK *rwlock){
  assert(rwlock);
  pthread_cond_destroy(&(rwlock->cond));
  pthread_mutex_destroy(&(rwlock->mutex));
  free(rwlock);
}


/* Lock a read-write lock object. */
int rwlock_lock(RWLOCK *rwlock, int wmode){
  assert(rwlock);
  if(pthread_mutex_lock(&(rwlock->mutex)) != 0) return FALSE;
  if(wmode){
    while(rwlock->writers > 0 || rwlock->readers > 0){
      pthread_cond_wait(&(rwlock->cond), &(rwlock->mutex));
    }
    rwlock->writers++;
  } else {
    while(rwlock->writers > 0){
      pthread_cond_wait(&(rwlock->cond), &(rwlock->mutex));
    }
    rwlock->readers++;
  }
  pthread_mutex_unlock(&(rwlock->mutex));
  return TRUE;
}


/* Unlock a read-write lock object. */
int rwlock_unlock(RWLOCK *rwlock){
  assert(rwlock);
  if(pthread_mutex_lock(&(rwlock->mutex)) != 0) return FALSE;
  if(rwlock->writers > 0){
    rwlock->writers--;
    pthread_cond_broadcast(&(rwlock->cond));
    pthread_mutex_unlock(&(rwlock->mutex));
  } else {
    rwlock->readers--;
    if(rwlock->readers < 1) pthread_cond_signal(&(rwlock->cond));
    pthread_mutex_unlock(&(rwlock->mutex));
  }
  return TRUE;
}


/* Get the number of readers locking a read-write lock object. */
int rwlock_rnum(RWLOCK *rwlock){
  assert(rwlock);
  return rwlock->readers;
}


/* Create a result map object. */
RESMAP *resmap_new(void){
  RESMAP *resmap;
  resmap = cbmalloc(sizeof(RESMAP));
  resmap->uris = cbmapopen();
  pthread_mutex_init(&(resmap->mutex), NULL);
  return resmap;
}


/* Destroy a result map object. */
void resmap_delete(RESMAP *resmap){
  RESDOC *resdoc;
  const char *kbuf, *vbuf;
  assert(resmap);
  cbmapiterinit(resmap->uris);
  while((kbuf = cbmapiternext(resmap->uris, NULL)) != NULL){
    vbuf = cbmapiterval(kbuf, NULL);
    resdoc = (RESDOC *)vbuf;
    if(resdoc->doc) est_doc_delete(resdoc->doc);
    if(resdoc->attrs) cbmapclose(resdoc->attrs);
    if(resdoc->body) free(resdoc->body);
  }
  pthread_mutex_destroy(&(resmap->mutex));
  cbmapclose(resmap->uris);
  free(resmap);
}


/* Add a result document data to a result map object. */
void resmap_put(RESMAP *resmap, int score, ESTDOC *doc, CBMAP *attrs, char *body){
  RESDOC resdoc;
  const char *uri, *vbuf;
  assert(resmap);
  uri = NULL;
  if(doc) uri = est_doc_attr(doc, ESTDATTRURI);
  if(attrs) uri = cbmapget(attrs, ESTDATTRURI, -1, NULL);
  if(!uri || pthread_mutex_lock(&(resmap->mutex)) != 0){
    if(doc) est_doc_delete(doc);
    if(attrs) cbmapclose(attrs);
    if(body) free(body);
    return;
  }
  if((vbuf = cbmapget(resmap->uris, uri, -1, NULL)) != NULL){
    if(((RESDOC *)vbuf)->score >= score){
      if(doc) est_doc_delete(doc);
      if(attrs) cbmapclose(attrs);
      if(body) free(body);
    } else {
      if(((RESDOC *)vbuf)->doc) est_doc_delete(((RESDOC *)vbuf)->doc);
      if(((RESDOC *)vbuf)->attrs) cbmapclose(((RESDOC *)vbuf)->attrs);
      if(((RESDOC *)vbuf)->body) free(((RESDOC *)vbuf)->body);
      resdoc.score = score;
      resdoc.doc = doc;
      resdoc.attrs = attrs;
      resdoc.body = body;
      resdoc.value = NULL;
      cbmapput(resmap->uris, uri, -1, (char *)&resdoc, sizeof(RESDOC), TRUE);
    }
  } else {
    resdoc.score = score;
    resdoc.doc = doc;
    resdoc.attrs = attrs;
    resdoc.body = body;
    cbmapput(resmap->uris, uri, -1, (char *)&resdoc, sizeof(RESDOC), FALSE);
  }
  pthread_mutex_unlock(&(resmap->mutex));
}


/* Get a list object of result objects in a result map objects. */
RESDOC **resmap_list(RESMAP *resmap, int *nump, const char *order, const char *distinct){
  RESDOC **resdocs, *resdoc;
  CBMAP *umap;
  const char *kbuf, *vbuf, *otype, *rp;
  char *oname, *wp;
  int i, onlen, dnlen, nnum;
  time_t tval;
  assert(resmap && nump);
  if(pthread_mutex_lock(&(resmap->mutex)) != 0){
    *nump = 0;
    return cbmalloc(1);
  }
  *nump = cbmaprnum(resmap->uris);
  resdocs = cbmalloc(*nump * sizeof(RESDOC) + 1);
  cbmapiterinit(resmap->uris);
  for(i = 0; i < *nump; i++){
    kbuf = cbmapiternext(resmap->uris, NULL);
    vbuf = cbmapiterval(kbuf, NULL);
    resdocs[i] = (RESDOC *)vbuf;
  }
  if(order){
    oname = cbmemdup(order, -1);
    cbstrtrim(oname);
    otype = ESTORDSTRA;
    if((wp = strchr(oname, ' ')) != NULL){
      *wp = '\0';
      rp = wp + 1;
      while(*rp == ' '){
        rp++;
      }
      otype = rp;
    }
    onlen = strlen(oname);
    for(i = 0; i < *nump; i++){
      if(resdocs[i]->doc){
        resdocs[i]->value = est_doc_attr(resdocs[i]->doc, oname);
      } else {
        resdocs[i]->value = cbmapget(resdocs[i]->attrs, oname, onlen, NULL);
      }
      if(!resdocs[i]->value) resdocs[i]->value = "";
    }
    if(!cbstricmp(otype, ESTORDSTRA)){
      qsort(resdocs, *nump, sizeof(RESDOC *), resdoc_compare_by_str_asc);
    } else if(!cbstricmp(otype, ESTORDSTRD)){
      qsort(resdocs, *nump, sizeof(RESDOC *), resdoc_compare_by_str_desc);
    } else if(!cbstricmp(otype, ESTORDNUMA)){
      for(i = 0; i < *nump; i++){
        tval = cbstrmktime(resdocs[i]->value);
        resdocs[i]->value = (void *)tval;
      }
      qsort(resdocs, *nump, sizeof(RESDOC *), resdoc_compare_by_num_asc);
    } else if(!cbstricmp(otype, ESTORDNUMD)){
      for(i = 0; i < *nump; i++){
        tval = cbstrmktime(resdocs[i]->value);
        resdocs[i]->value = (void *)tval;
      }
      qsort(resdocs, *nump, sizeof(RESDOC *), resdoc_compare_by_num_desc);
    }
    free(oname);
  } else {
    qsort(resdocs, *nump, sizeof(RESDOC *), resdoc_compare_by_score);
  }
  if(distinct){
    dnlen = strlen(distinct);
    umap = cbmapopenex(*nump + 1);
    nnum = 0;
    for(i = 0; i < *nump; i++){
      resdoc = resdocs[i];
      if(resdoc->doc){
        vbuf = est_doc_attr(resdoc->doc, distinct);
        printf("*");
      } else {
        vbuf = cbmapget(resdoc->attrs, distinct, dnlen, NULL);
      }
      if(!vbuf) vbuf = "";
      if(cbmapput(umap, vbuf, -1, "", 0, FALSE)) resdocs[nnum++] = resdoc;
    }
    *nump = nnum;
    cbmapclose(umap);
  }
  pthread_mutex_unlock(&(resmap->mutex));
  return resdocs;
}


/* Be a daemon process. */
int be_daemon(const char *curdir){
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  PROCESS_INFORMATION pi;
  STARTUPINFO si;
  assert(curdir);
  FreeConsole();
  if(getenv("ESTDAEMON")){
    Sleep(1000);
    if(chdir(curdir) == -1) return FALSE;
  } else {
    putenv("ESTDAEMON=1");
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    if(!CreateProcess(NULL, GetCommandLine(), NULL, NULL, FALSE,
                      BELOW_NORMAL_PRIORITY_CLASS | CREATE_NEW_PROCESS_GROUP |
                      CREATE_NO_WINDOW | DETACHED_PROCESS, NULL, NULL, &si, &pi))
      return FALSE;
    CloseHandle(pi.hProcess);
    exit(0);
  }
  return TRUE;
#else
  int fd;
  assert(curdir);
  switch(fork()){
  case -1:
    return FALSE;
  case 0:
    break;
  default:
    exit(0);
  }
  if(setsid() == -1) return FALSE;
  switch(fork()){
  case -1:
    return FALSE;
  case 0:
    break;
  default:
    exit(0);
  }
  umask(0);
  if(chdir(curdir) == -1) return FALSE;
  close(0);
  close(1);
  close(2);
  if((fd = open(NULLDEV, O_RDWR, 0)) != -1){
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);
    if(fd > 2) close(fd);
  }
  nice(5);
  return TRUE;
#endif
}



/*************************************************************************************************
 * private objects
 *************************************************************************************************/


/* Close the log file. */
static void log_close(void){
  if(log_fp) fclose(log_fp);
}


/* Output the log message of a DB event.
   `msg' specifies the log message of a DB event.
   `opaque' is simply ignored. */
static void db_informer(const char *message, void *opaque){
  assert(message);
  log_print(LL_INFO, "DB-EVENT: %s", message);
}


/* Compare two result document objects by score.
   `ap' specifies the pointer to one object.
   `ap' specifies the pointer to the other object.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int resdoc_compare_by_score(const void *ap, const void *bp){
  assert(ap && bp);
  return (*(RESDOC **)bp)->score - (*(RESDOC **)ap)->score;
}


/* Compare two result document objects by attributes of strings for ascending order.
   `ap' specifies the pointer to one score.
   `bp' specifies the pointer to the other score.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int resdoc_compare_by_str_asc(const void *ap, const void *bp){
  assert(ap && bp);
  return strcmp((*(RESDOC **)ap)->value, (*(RESDOC **)bp)->value);
}


/* Compare two result document objects by attributes of strings for descending order.
   `ap' specifies the pointer to one score.
   `bp' specifies the pointer to the other score.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int resdoc_compare_by_str_desc(const void *ap, const void *bp){
  assert(ap && bp);
  return strcmp((*(RESDOC **)bp)->value, (*(RESDOC **)ap)->value);
}


/* Compare two result document objects by attributes of numbers for ascending order.
   `ap' specifies the pointer to one score.
   `bp' specifies the pointer to the other score.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int resdoc_compare_by_num_asc(const void *ap, const void *bp){
  assert(ap && bp);
  return (time_t)(*(RESDOC **)ap)->value - (time_t)(*(RESDOC **)bp)->value;
}


/* Compare two result document objects by attributes of numbers for descending order.
   `ap' specifies the pointer to one score.
   `bp' specifies the pointer to the other score.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int resdoc_compare_by_num_desc(const void *ap, const void *bp){
  assert(ap && bp);
  return (time_t)(*(RESDOC **)bp)->value - (time_t)(*(RESDOC **)ap)->value;
}



/* END OF FILE */
