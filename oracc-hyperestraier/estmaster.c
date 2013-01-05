/*************************************************************************************************
 * The master of node servers
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
#include "myimage.dat"

#define SERVNAME       "estmaster"       /* name of the server */
#define IOBUFSIZ       8192              /* size of a buffer for I/O */
#define SPCACHEMNUM    131072            /* max number of the special cache */
#define IREFRESHSEC    3                 /* wait time for refreshing and charging */
#define ICHARGENUM     16                /* number of keys per charging */
#define IFLUSHNUM      32768             /* number of keys per flushing */
#define LINKMASKMAX    9                 /* maximum number of link masks */
#define LCSRCHRESMIN   31                /* minimum number of result to get from local */
#define RMSRCHRESMIN   16                /* minimum number of result to get from local */
#define FCLOSECONNNUM  10                /* number of over connections for forced closing */
#define FCLOSEWAITMAX  10                /* wait time for forced closing */
#define FTERMWAITMAX   60                /* wait time for forced termination */
#define HELPERTRYNUM   3                 /* number of trys of helper command */
#define UICACHELIFE    1800              /* maximum age of image cache data */
#define IMGCACHELIFE   (3600 * 24)       /* maximum age of image cache data */

#define MASTERLOC      "/master"         /* location of the URL of the master */
#define NODEPREFIX     "/node/"          /* prefix of URLs of nodes */
#define MASTERUILOC    "/master_ui"      /* location of administration user intarfaces */
#define FAVICONLOC     "/favicon.ico"    /* location of the favorite icon */
#define IMAGEPREFIX    "/image/"         /* prefix of URLs of images */
#define SEARCHUICMD    "search_ui"       /* name of search user interface */
#define SEARCHATOMCMD  "search_atom"     /* name of Atom feed interface */
#define SEARCHRSSCMD   "search_rss"      /* name of RSS feed interface */
#define OPENSEARCHCMD  "opensearch"      /* name of OpenSearch Description */
#define BIGICONNAME    "bigicon.png"     /* name of the big icon */
#define CANVASNAME     "canvas.png"      /* name of the backgroud pattern */
#define DELIMSTR       "{{!}}"           /* delimiter string */

#define UIMIMETYPE     "text/html; charset=UTF-8"  /* media type of search_ui */
#define ATOMMIMETYPE   "application/atom+xml"      /* media type of search_atom */
#define RSSMIMETYPE    "application/rdf+xml"       /* media type of search_rss */
#define OSRCHMIMETYPE  "application/opensearchdescription+xml"  /* media type of opensearch */
#define DIRMIMETYPE    "text/html"                 /* media type of directory index */
#define MENUMIMETYPE   "text/html"                 /* media type of the menu page */
#define DEFMIMETYPE    "application/octet-stream"  /* media type by default */

typedef struct {                         /* type of structure for a communication */
  int clsock;                            /* socket between client */
  char claddr[ADDRBUFSIZ];               /* address of the client */
  int clport;                            /* port number of the client */
} TARGCOMM;

enum {                                   /* enumeration for HTTP method */
  HM_HEAD,                               /* HEAD */
  HM_GET,                                /* GET */
  HM_POST,                               /* POST */
  HM_OPTIONS,                            /* OPTIONS */
  HM_UNKNOWN                             /* unknown */
};

typedef struct {                         /* type of structure for a request */
  const char *claddr;                    /* address of the client */
  int clport;                            /* port number of the client */
  char prefix[HOSTBUFSIZ];               /* prefix of the server */
  int method;                            /* method */
  char *target;                          /* target path */
  char *name;                            /* user name */
  char *passwd;                          /* password */
  time_t ims;                            /* if-modified-since header */
  int gzip;                              /* whether to accept gzip encoding */
  int deflate;                           /* whether to accept deflate encoding */
  char *ctype;                           /* content-type header */
  char *referer;                         /* referer header */
  char *estvia;                          /* x-estraier-via header */
  char *body;                            /* entity body */
  CBMAP *params;                         /* parameters */
  time_t now;                            /* current time */
  int reload;                            /* whether to be reloaded */
} REQUEST;

typedef struct {                         /* type of structure for a local search */
  REQUEST *req;                          /* request object */
  int alive;                             /* whether to be alive */
  ESTCOND *cond;                         /* search condition */
  CBMAP *hints;                          /* hints about result */
  int max;                               /* max number of retrieved documents */
  NODE *node;                            /* self node object */
  RESMAP *resmap;                        /* documents in result */
  CBLIST *words;                         /* words in the search phrase */
  int hnum;                              /* number of hits */
  int mhnum;                             /* number of mishits */
  double itime;                          /* index elapsed time */
  double etime;                          /* total elapsed time */
} TARGLCSRCH;

typedef struct {                         /* type of structure for a remote search */
  REQUEST *req;                          /* request object */
  int alive;                             /* whether to be alive */
  const char *myurl;                     /* url of the local node */
  ESTCOND *cond;                         /* search condition */
  CBMAP *hints;                          /* hints about result */
  int max;                               /* max number of retrieved documents */
  NODE *node;                            /* self node object */
  RESMAP *resmap;                        /* documents in result */
  const char *url;                       /* URL of the link */
  int credit;                            /* credit of the link */
  char *label;                           /* label of the link */
  int depth;                             /* depth of meta search */
  int wwidth;                            /* while width of a snippet */
  int hwidth;                            /* top width of a snippet */
  int awidth;                            /* around width of a snippet */
  int hnum;                              /* number of hits */
  int dnum;                              /* number of documents */
  int wnum;                              /* number of words */
  double etime;                          /* elapsed time */
  double size;                           /* size of the database */
  time_t mtime;                          /* modification time */
} TARGRMSRCH;

enum {                                   /* enumeration for UI operations */
  UI_VIEWMASTER,                         /* view master operations */
  UI_SHUTDOWN,                           /* shutdown the master server */
  UI_SYNCNODES,                          /* synchronize all nodes */
  UI_BACKUPDB,                           /* backup the database */
  UI_VIEWUSERS,                          /* view user operations */
  UI_NEWUSER,                            /* create a user */
  UI_DELUSER,                            /* delete a user */
  UI_VIEWNODES,                          /* view node operations */
  UI_NEWNODE,                            /* create a node */
  UI_DELENODE,                           /* delete a node */
  UI_EDITNODE,                           /* edit a node */
  UI_STATNODE,                           /* view search history of a node */
  UI_NONE                                /* none */
};


/* global variables */
const char *g_progname;                  /* program name */
int g_sigterm = FALSE;                   /* flag for termination signal */
int g_sigrestart = FALSE;                /* flag for restarting signal */
int g_sigsync = FALSE;                   /* flag for synchronus signal */
int g_sigback = FALSE;                   /* flag for backup signal */
const char *g_rootdir = NULL;            /* path of the root directory */
const char *g_bindaddr = NULL;           /* binding address of TCP */
int g_portnum = 0;                       /* port number of TCP */
const char *g_publicurl = NULL;          /* public URL */
int g_runmode = 0;                       /* runnning mode */
int g_authmode = 0;                      /* authorization mode */
int g_recvmax = 0;                       /* maximum length of data to receive */
int g_maxconn = 0;                       /* maximum number of connections */
int g_idleflush = 0;                     /* idle time to start flushing */
int g_idlesync = 0;                      /* idle time to start synchronizing */
int g_sessiontimeout = 0;                /* timeout of a session, in seconds */
int g_searchtimeout = 0;                 /* timeout of search, in seconds */
int g_searchmax = 0;                     /* maximum number of documents to send */
int g_searchdepth = 0;                   /* depth of meta search */
int g_rateuri = FALSE;                   /* whether to rate URI for scoring */
int g_mergemethod = 0;                   /* merge method */
const char *g_proxyhost = NULL;          /* host name of the proxy */
int g_proxyport = 0;                     /* port number of the proxy */
const char *g_logfile = NULL;            /* path of the log file */
int g_loglevel = 0;                      /* logging level */
const char *g_backupcmd = NULL;          /* path of the backup command */
int g_scalepred = 0;                     /* scale prediction */
int g_scoreexpr = 0;                     /* score expression */
CBLIST *g_attrindexes = NULL;            /* expressions of attribute indexes */
const char *g_docroot = NULL;            /* path of the document root directory */
const char *g_indexfile = NULL;          /* name of the directory index file */
CBMAP *g_trustednodes = NULL;            /* addresses of trusted nodes */
int g_denyuntrusted = FALSE;             /* whether to deny untrusted nodes */
double g_cachesize = 0.0;                /* maximum size of the index cache */
int g_cacheanum = 0;                     /* max of cached records for document attributes */
int g_cachetnum = 0;                     /* max of cached records for document texts */
int g_cachernum = 0;                     /* max of cached records for occurrence results */
const char *g_specialcache = NULL;       /* name of the attribute of special cache */
double g_helpershift = 0.0;              /* lower limit of cache usage size to use the helper */
int g_wildmax = -1;                      /* maximum number of extension of wild cards */
int g_limittextsize = 0;                 /* text size limitation */
int g_snipwwidth = 0;                    /* whole width of the snippet */
int g_sniphwidth = 0;                    /* width of beginning of the text */
int g_snipawidth = 0;                    /* width around each highlighted word */
int g_scancheck = FALSE;                 /* whether to check documents by scanning */
int g_smlrvnum = 0;                      /* number of keywords for similarity search */
int g_extdelay = 0;                      /* number of documents for delay of extraction */
const char *g_adminemail = NULL;         /* e-mail address of the administrator */
CBLIST *g_uireplaces = NULL;             /* expressions to replace the URI of each document */
CBLIST *g_uiextattrs = NULL;             /* extra attributes to be shown */
int g_uiphraseform = 0;                  /* mode of phrase form */
const char *g_uismlrtune = NULL;         /* tuning parameters for similarity search */
int g_bgmode = FALSE;                    /* whether to be foreground mode */
int g_romode = FALSE;                    /* whether to be read only mode */
int g_stmode = FALSE;                    /* whether to be single thread mode */
char g_hostname[HOSTBUFSIZ];             /* host name of the server */
DEPOT *g_metadb = NULL;                  /* meta database */
DEPOT *g_dfdb = NULL;                    /* document frequency database */
UMGR *g_umgr = NULL;                     /* user manager */
NMGR *g_nmgr = NULL;                     /* node manager */
RWLOCK *g_runlock = NULL;                /* read-write lock for running threads */
RWLOCK *g_mgrlock = NULL;                /* read-write lock for handling managers */
pthread_mutex_t g_backlock = PTHREAD_MUTEX_INITIALIZER;  /* mutex for background tasks */
const char *g_bordstr = NULL;            /* border string */
int g_svsock = -1;                       /* server socket */
int g_accesscount = 0;                   /* count of all access */
time_t g_startdate = 0;                  /* start date of the server */
double g_cacheratio = 0.0;               /* worst usage ratio of the index cache */


/* function prototypes */
int main(int argc, char **argv);
static int realmain(int argc, char **argv);
static void setsignals(void);
static void sigtermhandler(int num);
static void usage(void);
static int runinit(int argc, char **argv);
static int runstart(int argc, char **argv);
static int runstop(int argc, char **argv);
static int rununittest(int argc, char **argv);
static int runcrypt(int argc, char **argv);
static void die(const char *format, ...);
static void startup(void);
static const char *skiplabel(const char *str);
static void cleanup(void);
static int procinit(const char *rootdir, int ex);
static int procstart(const char *rootdir);
static int procstop(const char *rootdir);
static int procunittest(const char *rootdir);
static int proccrypt(const char *key, const char *hash);
static void dispatch(void);
static void *refreshnode(void *targ);
static void *flushnode(void *targ);
static void *syncnodes(void *targ);
static void backupdb(void);
static void *communicate(void *targ);
static void setparams(CBMAP *params, const char *src);
static void addservinfo(CBDATUM *datum, time_t now, int ccage);
static void senderror(int clsock, REQUEST *req, int code, const char *message);
static void sendautherror(int clsock, REQUEST *req, const char *realm);
static void sendnotmoderror(int clsock, REQUEST *req);
static void sendheadonly(int clsock, REQUEST *req, const char *type);
static int isbanned(USER *user);
static int ismasteradmin(REQUEST *req, USER *user);
static int isnodeadmin(NODE *node, REQUEST *req, USER *user);
static int isnodeuser(NODE *node, REQUEST *req, USER *user);
static int getnodeoptions(void);
static void sendoptions(int clsock, REQUEST *req, USER *user);
static void sendmasterdata(int clsock, REQUEST *req, USER *user);
static void sendnodedata(int clsock, REQUEST *req, USER *user, const char *path);
static void setdocorigin(ESTDOC *doc, REQUEST *req, NODE *node, int score);
static void mergehints(CBMAP *total, CBMAP *local);
static int islooproute(const char *url, REQUEST *req);
static void *searchlocal(void *targ);
static void *searchremote(void *targ);
static void setsimilarphrase(ESTCOND *cond, const char *url, int id);
static void catdocdata(CBDATUM *datum, NODE *node, RESDOC *resdoc, CBLIST *words,
                       int wwidth, int hwidth, int awidth);
static void catdocdataui(CBDATUM *datum, int num, RESDOC *resdoc, CBLIST *words,
                         REQUEST *req, NODE *node, const char *condstr, const char *simcondstr);
static void catdocdataatom(CBDATUM *datum, RESDOC *resdoc, CBLIST *words, NODE *node);
static void catdocdatarss(CBDATUM *datum, RESDOC *resdoc, CBLIST *words, NODE *node);
static char *makeshownuri(const char *uri);
static void sendnodecmdinform(int clsock, REQUEST *req, NODE *node);
static int sendnodecmdinformhelper(int clsock, REQUEST *req, NODE *node);
static void sendnodecmdcacheusage(int clsock, REQUEST *req, NODE *node);
static void sendnodecmdsearch(int clsock, REQUEST *req, NODE *node);
static int sendnodecmdsearchhelper(int clsock, REQUEST *req, NODE *node, const char *myurl);
static void sendnodecmdlist(int clsock, REQUEST *req, NODE *node);
static void sendnodecmdgetdoc(int clsock, REQUEST *req, NODE *node);
static int sendnodecmdgetdochelper(int clsock, REQUEST *req, NODE *node,
                                   int id, const char *uri);
static void sendnodecmdgetdocattr(int clsock, REQUEST *req, NODE *node);
static int sendnodecmdgetdocattrhelper(int clsock, REQUEST *req, NODE *node,
                                       int id, const char *uri, const char *attr);
static void sendnodecmdetchdoc(int clsock, REQUEST *req, NODE *node);
static int sendnodecmdetchdochelper(int clsock, REQUEST *req, NODE *node,
                                    int id, const char *uri);
static void sendnodecmduritoid(int clsock, REQUEST *req, NODE *node);
static int sendnodecmduritoidhelper(int clsock, REQUEST *req, NODE *node, const char *uri);
static void sendnodecmdputdoc(int clsock, REQUEST *req, NODE *node);
static void sendnodecmdoutdoc(int clsock, REQUEST *req, NODE *node);
static void sendnodecmdeditdoc(int clsock, REQUEST *req, NODE *node);
static void sendnodecmdsync(int clsock, REQUEST *req, NODE *node);
static void sendnodecmdoptimize(int clsock, REQUEST *req, NODE *node);
static void sendnodecmdsetuser(int clsock, REQUEST *req, NODE *node);
static void sendnodecmdsetlink(int clsock, REQUEST *req, NODE *node);
static void sendnodecmdsearchui(int clsock, REQUEST *req, NODE *node);
static void sendnodecmdsearchatom(int clsock, REQUEST *req, NODE *node);
static void sendnodecmdsearchrss(int clsock, REQUEST *req, NODE *node);
static void sendnodecmdopensearch(int clsock, REQUEST *req, NODE *node);
static void sendmasteruidata(int clsock, REQUEST *req, USER *user);
static void sendfavicondata(int clsock, REQUEST *req);
static void sendimagedata(int clsock, REQUEST *req, const char *name);
static void sendfiledata(int clsock, REQUEST *req);
static char *makelocalpath(const char *target);
static void sendmenudata(int clsock, REQUEST *req);


/* for windows service */
#if defined(MYWINSERV)
SERVICE_STATUS_HANDLE g_svstat = 0;
static void svmain(int argc, char **argv);
static int sendstatus(int curstate, int exitcode, int svexitcode, int check, int hint);
static void svfinish(int code);
static void svhandle(int code);
static void svmain(int argc, char **argv){
  if(!(g_svstat = RegisterServiceCtrlHandler(SERVNAME, (LPHANDLER_FUNCTION)svhandle))){
    svfinish(GetLastError());
    return;
  }
  if(!sendstatus(SERVICE_START_PENDING, 0, 0, 1, 5000)){
    svfinish(GetLastError());
    return;
  }
  est_init_net_env();
  startup();
  if(!sendstatus(SERVICE_RUNNING, 0, 0, 0, 0)){
    svfinish(GetLastError());
    return;
  }
  dispatch();
  cleanup();
  svfinish(0);
}
static int sendstatus(int curstate, int exitcode, int svexitcode, int check, int hint){
  SERVICE_STATUS st;
  st.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  st.dwCurrentState = curstate;
  if(curstate == SERVICE_START_PENDING){
    st.dwControlsAccepted = 0;
  } else {
    st.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
  }
  if(svexitcode == 0){
    st.dwWin32ExitCode = exitcode;
  } else {
    st.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
  }
  st.dwServiceSpecificExitCode = svexitcode;
  st.dwCheckPoint = check;
  st.dwWaitHint = hint;
  if(!SetServiceStatus(g_svstat, &st)){
    g_sigterm = TRUE;
    return FALSE;
  }
  return TRUE;
}
static void svfinish(int code){
  if(g_svstat) sendstatus(SERVICE_STOPPED, code, 0, 0, 0);
}
static void svhandle(int code){
  switch(code){
  case SERVICE_CONTROL_STOP:
  case SERVICE_CONTROL_SHUTDOWN:
    sendstatus(SERVICE_STOP_PENDING, 0, 0, 1, 5000);
    g_sigterm = TRUE;
    break;
  case SERVICE_CONTROL_INTERROGATE:
    sendstatus(0, 0, 0, 0, 0);
    break;
  }
}
#endif


/* main routine */
int main(int argc, char **argv){
#if defined(MYWINSERV)
  SERVICE_TABLE_ENTRY table[] = {
    { SERVNAME, (LPSERVICE_MAIN_FUNCTION)svmain }, { NULL, NULL }
  };
  est_proc_env_reset();
  g_progname = argv[0];
  g_sigterm = FALSE;
  g_rootdir = argc >= 2 ? argv[1] : "casket";
  if(!StartServiceCtrlDispatcher(table)) die("not called by service controll manager");
  return 0;
#else
  return realmain(argc, argv);
#endif
}


/* real main routine */
static int realmain(int argc, char **argv){
  const char *tmp;
  int rv;
  if((tmp = getenv("ESTDBGFD")) != NULL) dpdbgfd = atoi(tmp);
  est_proc_env_reset();
  g_progname = argv[0];
  g_sigterm = FALSE;
  if(argc < 2) usage();
  rv = 0;
  if(!strcmp(argv[1], "init")){
    rv = runinit(argc, argv);
  } else if(!strcmp(argv[1], "start")){
    rv = runstart(argc, argv);
  } else if(!strcmp(argv[1], "stop")){
    rv = runstop(argc, argv);
  } else if(!strcmp(argv[1], "unittest")){
    rv = rununittest(argc, argv);
  } else if(!strcmp(argv[1], "crypt")){
    rv = runcrypt(argc, argv);
  } else {
    usage();
  }
  return rv;
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
  g_sigrestart = FALSE;
  g_sigsync = FALSE;
  g_sigback = FALSE;
}


/* handler of termination signal */
static void sigtermhandler(int num){
  g_sigterm = TRUE;
  if(num == 1 && g_bgmode) g_sigrestart = TRUE;
}


/* print the usage and exit */
static void usage(void){
  fprintf(stderr, "%s: the master of node servers\n", g_progname);
  fprintf(stderr, "\n");
  fprintf(stderr, "usage:\n");
  fprintf(stderr, "  %s init [-ex] rootdir\n", g_progname);
  fprintf(stderr, "  %s start [-bg] [-ro] [-st] rootdir\n", g_progname);
  fprintf(stderr, "  %s stop rootdir\n", g_progname);
  fprintf(stderr, "  %s unittest rootdir\n", g_progname);
  fprintf(stderr, "  %s crypt key [hash]\n", g_progname);
  fprintf(stderr, "\n");
  exit(1);
}


/* parse arguments of the init command */
static int runinit(int argc, char **argv){
  char *rootdir;
  int i, ex, rv;
  rootdir = NULL;
  ex = FALSE;
  for(i = 2; i < argc; i++){
    if(!rootdir && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-ex")){
        ex = TRUE;
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
  rv = procinit(rootdir, ex);
  return rv;
}


/* parse arguments of the start command */
static int runstart(int argc, char **argv){
  char *rootdir;
  int i, rv;
  rootdir = NULL;
  for(i = 2; i < argc; i++){
    if(!rootdir && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-bg")){
        g_bgmode = TRUE;
      } else if(!strcmp(argv[i], "-ro")){
        g_romode = TRUE;
      } else if(!strcmp(argv[i], "-st")){
        g_stmode = TRUE;
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
  rv = procstart(rootdir);
  return rv;
}


/* parse arguments of the stop command */
static int runstop(int argc, char **argv){
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
  rv = procstop(rootdir);
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


/* parse arguments of the crypt command */
static int runcrypt(int argc, char **argv){
  char *key, *hash;
  int i, rv;
  key = NULL;
  hash = NULL;
  for(i = 2; i < argc; i++){
    if(!key && argv[i][0] == '-'){
      usage();
    } else if(!key){
      key = argv[i];
    } else if(!hash){
      hash = argv[i];
    } else {
      usage();
    }
  }
  if(!key) usage();
  rv = proccrypt(key, hash);
  return rv;
}


/* print formatted error string and exit */
static void die(const char *format, ...){
  va_list ap;
  char msgbuf[URIBUFSIZ];
  va_start(ap, format);
  vsprintf(msgbuf, format, ap);
  if(log_open(g_rootdir, g_logfile ? g_logfile : LOGFILE, LL_INFO, FALSE)){
    log_print(LL_ERROR, "startup failed: %s", msgbuf);
  } else {
    fprintf(stderr, "%s: %s\n", g_progname, msgbuf);
    fflush(stderr);
  }
  va_end(ap);
  exit(1);
}


/* initialize the global variables */
static void startup(void){
  NODE *node;
  ESTMTDB *db;
  CBLIST *lines, *elems;
  struct stat sbuf;
  const char *rp, *name;
  char path[URIBUFSIZ], numbuf[NUMBUFSIZ];
  int i, ecode, pid, omode;
  if(stat(g_rootdir, &sbuf) == -1)
    die("the server root directory (%s) could not open", g_rootdir);
  sprintf(path, "%s%c%s", g_rootdir, ESTPATHCHR, CONFFILE);
  if(!(lines = cbreadlines(path))) die("the configuration file (%s) could not open", path);
  cbglobalgc(lines, (void (*)(void *))cblistclose);
  g_attrindexes = cblistopen();
  cbglobalgc(g_attrindexes, (void (*)(void *))cblistclose);
  g_trustednodes = cbmapopenex(MINIBNUM);
  cbglobalgc(g_trustednodes, (void (*)(void *))cbmapclose);
  g_uireplaces = cblistopen();
  cbglobalgc(g_uireplaces, (void (*)(void *))cblistclose);
  g_uiextattrs = cblistopen();
  cbglobalgc(g_uiextattrs, (void (*)(void *))cblistclose);
  for(i = 0; i < cblistnum(lines); i++){
    rp = cblistval(lines, i, NULL);
    if(cbstrfwimatch(rp, "bindaddr:")){
      g_bindaddr = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "portnum:")){
      g_portnum = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "publicurl:")){
      g_publicurl = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "runmode:")){
      g_runmode = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "authmode:")){
      g_authmode = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "recvmax:")){
      g_recvmax = atoi(skiplabel(rp)) * 1024;
    } else if(cbstrfwimatch(rp, "maxconn:")){
      g_maxconn = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "idleflush:")){
      g_idleflush = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "idlesync:")){
      g_idlesync = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "sessiontimeout:")){
      g_sessiontimeout = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "searchtimeout:")){
      g_searchtimeout = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "searchmax:")){
      g_searchmax = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "searchdepth:")){
      g_searchdepth = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "rateuri:")){
      g_rateuri = atoi(skiplabel(rp)) > 0;
    } else if(cbstrfwimatch(rp, "mergemethod:")){
      g_mergemethod = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "proxyhost:")){
      g_proxyhost = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "proxyport:")){
      g_proxyport = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "logfile:")){
      g_logfile = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "loglevel:")){
      g_loglevel = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "backupcmd:")){
      g_backupcmd = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "scalepred:")){
      g_scalepred = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "scoreexpr:")){
      g_scoreexpr = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "attrindex:")){
      cblistpush(g_attrindexes, skiplabel(rp), -1);
    } else if(cbstrfwimatch(rp, "docroot:")){
      g_docroot = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "indexfile:")){
      g_indexfile = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "trustednode:")){
      cbmapput(g_trustednodes, skiplabel(rp), -1, "", 0, TRUE);
    } else if(cbstrfwimatch(rp, "denyuntrusted:")){
      g_denyuntrusted = atoi(skiplabel(rp)) > 0;
    } else if(cbstrfwimatch(rp, "cachesize:")){
      g_cachesize = strtod(skiplabel(rp), NULL) * 1024 * 1024;
    } else if(cbstrfwimatch(rp, "cacheanum:")){
      g_cacheanum = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "cachetnum:")){
      g_cachetnum = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "cachernum:")){
      g_cachernum = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "specialcache:")){
      g_specialcache = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "helpershift:")){
      g_helpershift = strtod(skiplabel(rp), NULL);
    } else if(cbstrfwimatch(rp, "wildmax:")){
      g_wildmax = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "limittextsize:")){
      g_limittextsize = atoi(skiplabel(rp)) * 1024;
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
    } else if(cbstrfwimatch(rp, "adminemail:")){
      g_adminemail = skiplabel(rp);
    } else if(cbstrfwimatch(rp, "uireplace:")){
      cblistpush(g_uireplaces, skiplabel(rp), -1);
    } else if(cbstrfwimatch(rp, "uiextattr:")){
      cblistpush(g_uiextattrs, skiplabel(rp), -1);
    } else if(cbstrfwimatch(rp, "uiphraseform:")){
      g_uiphraseform = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "uismlrtune:")){
      g_uismlrtune = skiplabel(rp);
    }
  }
  if(!g_bindaddr) die("bindaddr is undefined");
  if(g_portnum < 1) die("portnum is undefined");
  if(!g_publicurl) die("publicurl is undefined");
  if(g_runmode < RM_NORMAL || g_runmode > RM_RDONLY) die("runmode is undefined");
  if(g_authmode < AM_NONE || g_authmode > AM_ALL) die("authmode is undefined");
  if(g_recvmax < 1) die("recvmax is undefined");
  if(g_maxconn < 1) die("maxconn is undefined");
  if(g_idleflush < 1) die("idleflush is undefined");
  if(g_idlesync < 1) die("idlesync is undefined");
  if(g_sessiontimeout < 1) die("sessiontimeout is undefined");
  if(g_searchtimeout < 1) die("searchtimeout is undefined");
  if(g_searchmax < 1) die("searchmax is undefined");
  if(g_searchdepth < 0) die("searchdepth is undefined");
  if(g_mergemethod < MM_SCORE || g_mergemethod > MM_RANK) die("mergemethod is undefined");
  if(!g_proxyhost) die("proxyhost is undefined");
  if(g_proxyhost[0] != '\0' && g_proxyport < 1) die("proxyport is undefined");
  if(!g_logfile) die("logfile is undefined");
  if(g_loglevel < LL_DEBUG || g_loglevel > LL_NONE) die("loglevel is undefined");
  if(!g_docroot) die("docroot is undefined");
  if(!g_indexfile) die("indexfile is undefined");
  if(!g_backupcmd) die("backupcmd is undefined");
  if(g_scalepred < SP_SMALL || g_scalepred > SP_HUGE) die("scalepred is undefined");
  if(g_scoreexpr < SE_VOID || g_scoreexpr > SE_ASIS) die("scoreexpr is undefined");
  if(g_cachesize < 0.0) die("cachesize is undefined");
  if(g_cacheanum < 0) die("cacheanum is undefined");
  if(g_cachetnum < 0) die("cachetnum is undefined");
  if(g_cachernum < 0) die("cachernum is undefined");
  if(!g_specialcache) die("indexfile is undefined");
  if(g_helpershift < 0) die("helpershift is undefined");
  if(g_wildmax < 0) die("wildmax is undefined");
  if(g_limittextsize < 1) die("limittextsize is undefined");
  if(g_snipwwidth < 0) die("snipwwidth is undefined");
  if(g_sniphwidth < 0) die("sniphwidth is undefined");
  if(g_snipawidth < 0) die("snipawidth is undefined");
  if(g_smlrvnum < 0) die("smlrvnum is undefined");
  if(!g_adminemail) die("adminemail is undefined");
  if(g_uiphraseform < 1) die("uiphraseform is undefined");
  if(!g_uismlrtune) die("uismlrtune is undefined");
  if(g_romode) g_runmode = RM_RDONLY;
  if((pid = lockerpid(g_rootdir)) > 0)
    die("another process (pid:%d) has opened the database", pid);
  if(!log_open(g_rootdir, g_logfile, LL_CHECK, FALSE))
    die("the log file (%s) could not open", g_logfile);
  sprintf(path, "%s%c%s", g_rootdir, ESTPATHCHR, METAFILE);
  if(!(g_metadb = dpopen(path, DP_OREADER, -1))){
    if(dpecode == DP_ELOCK){
      die("other process (pid:%d) has opened the database", lockerpid(g_rootdir));
    } else {
      die("the meta database file (%s) could not open", path);
    }
  }
  sprintf(path, "%s%c%s", g_rootdir, ESTPATHCHR, NODEDIR);
  if(!(elems = cbdirlist(path))){
    dpclose(g_metadb);
    die("the node directory (%s) could not open", path);
  }
  for(i = 0; i < cblistnum(elems); i++){
    name = cblistval(elems, i, NULL);
    if(!strcmp(name, ESTCDIRSTR) || !strcmp(name, ESTPDIRSTR)) continue;
    sprintf(path, "%s%c%s%c%s", g_rootdir, ESTPATHCHR, NODEDIR, ESTPATHCHR, name);
    if((db = est_mtdb_open(path, ESTDBREADER, &ecode)) != NULL){
      if(!est_mtdb_close(db, &ecode)){
        cblistclose(elems);
        dpclose(g_metadb);
        die("the database (%s) could not close", path);
      }
    } else {
      cblistclose(elems);
      dpclose(g_metadb);
      die("the database (%s) could not open", path);
    }
  }
  cblistclose(elems);
  dpclose(g_metadb);
  if(g_bgmode && !be_daemon(g_rootdir)) die("the process could not be a daemon");
  sprintf(g_hostname, "%s", est_get_host_name());
  sprintf(path, "%s%c%s", g_rootdir, ESTPATHCHR, METAFILE);
  omode = (g_runmode == RM_RDONLY ? DP_OREADER : DP_OWRITER) | DP_OLCKNB;
  if(!(g_metadb = dpopen(path, omode, -1))){
    if(dpecode == DP_ELOCK){
      die("other process (pid:%d) has opened the database", lockerpid(g_rootdir));
    } else {
      die("the meta database file (%s) could not open", path);
    }
  }
  pid = (int)getpid();
  sprintf(path, "%s%c%s", g_rootdir, ESTPATHCHR, PIDFILE);
  sprintf(numbuf, "%d\n", pid);
  cbwritefile(path, numbuf, -1);
  if(!log_open(g_rootdir, g_logfile, g_loglevel, FALSE)){
    unlink(path);
    dpclose(g_metadb);
    die("starting logging failed");
  }
  log_print(LL_INFO, "starting the master process (pid:%d)", pid);
  g_umgr = umgr_new(g_rootdir);
  g_nmgr = nmgr_new(g_rootdir);
  if(!umgr_load(g_umgr) || !nmgr_load(g_nmgr, g_runmode != RM_RDONLY)){
    unlink(path);
    nmgr_delete(g_nmgr);
    umgr_delete(g_umgr);
    dpclose(g_metadb);
    log_print(LL_ERROR, "loading users and nodes failed");
    die("loading users and nodes failed");
  }
  for(i = 0; i < cblistnum(g_attrindexes); i++){
    name = cblistval(g_attrindexes, i, NULL);
    if(!(rp = strstr(name, "{{!}}"))) continue;
    memcpy(path, name, rp - name);
    path[rp-name] = '\0';
    nmgr_add_aidx(g_nmgr, path, rp + 5);
  }
  g_runlock = rwlock_new();
  g_mgrlock = rwlock_new();
  if((g_svsock = est_get_server_sock(g_bindaddr, g_portnum)) == -1){
    unlink(path);
    rwlock_delete(g_mgrlock);
    rwlock_delete(g_runlock);
    nmgr_delete(g_nmgr);
    umgr_delete(g_umgr);
    dpclose(g_metadb);
    log_print(LL_ERROR, "initializing network failed");
    die("initializing network failed");
  }
  g_accesscount = 0;
  g_startdate = time(NULL);
  if(g_docroot[0] != '\0'){
    if(stat(g_docroot, &sbuf) == 0){
      log_print(LL_INFO, "letting the directory (%s) be public", g_docroot);
    } else {
      log_print(LL_WARN, "missing the directory (%s)", g_docroot);
    }
  }
  sprintf(path, "%s%c%s", g_rootdir, ESTPATHCHR, DFDBFILE);
  g_dfdb = dpopen(path, DP_OREADER, -1);
  g_bordstr = est_border_str();
  elems = nmgr_names(g_nmgr);
  for(i = 0; i < cblistnum(elems); i++){
    if(!(name = cblistval(elems, i, NULL)) || !(node = nmgr_get(g_nmgr, name))) continue;
    est_mtdb_set_cache_size(node->db, g_cachesize, g_cacheanum, g_cachetnum, g_cachernum);
    if(g_specialcache[0] != '\0')
      est_mtdb_set_special_cache(node->db, g_specialcache, SPCACHEMNUM);
    est_mtdb_set_wildmax(node->db, g_wildmax);
    if(g_dfdb) est_mtdb_set_dfdb(node->db, g_dfdb);
  }
  cblistclose(elems);
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


/* clean up resources */
static void cleanup(void){
  char path[URIBUFSIZ];
  if(g_svsock != -1) est_sock_down(g_svsock);
  rwlock_delete(g_mgrlock);
  rwlock_delete(g_runlock);
  nmgr_delete(g_nmgr);
  umgr_delete(g_umgr);
  sprintf(path, "%s%c%s", g_rootdir, ESTPATHCHR, STOPFILE);
  unlink(path);
  sprintf(path, "%s%c%s", g_rootdir, ESTPATHCHR, PIDFILE);
  unlink(path);
  dpclose(g_metadb);
  if(g_dfdb) dpclose(g_dfdb);
  log_print(LL_INFO, "the master process finished");
}


/* perform the init command */
static int procinit(const char *rootdir, int ex){
  UMGR *umgr;
  NMGR *nmgr;
  NODE *node;
  ESTDOC *doc;
  char *tmp;
  int err;
  setsignals();
  if(!master_init(rootdir)){
    log_print(LL_ERROR, "initializing the root directory failed");
    return 1;
  }
  log_open(rootdir, LOGFILE, LL_INFO, FALSE);
  log_print(LL_INFO, "the root directory created");
  umgr = umgr_new(rootdir);
  tmp = est_make_crypt("admin");
  umgr_put(umgr, "admin", tmp, "s", "Carolus Magnus", "Administrator");
  free(tmp);
  if(ex){
    tmp = est_make_crypt("john");
    umgr_put(umgr, "john", tmp, "", "John Doe", "Normal User");
    free(tmp);
    tmp = est_make_crypt("dick");
    umgr_put(umgr, "dick", tmp, "", "Richard Roe", "Normal User");
    free(tmp);
    tmp = est_make_crypt("lupin");
    umgr_put(umgr, "lupin", tmp, "b", "Arsene Lupin", "Banned User");
    free(tmp);
  }
  err = FALSE;
  if(!umgr_delete(umgr)) err = TRUE;
  nmgr = nmgr_new(rootdir);
  if(ex){
    nmgr_put(nmgr, "sample1", TRUE, 0);
    if((node = nmgr_get(nmgr, "sample1")) != NULL){
      free(node->label);
      node->label = cbmemdup("Sample Node One", -1);
      cbmapput(node->admins, "john", -1, "", 0, TRUE);
      cbmapput(node->users, "john", -1, "", 0, TRUE);
      cbmapput(node->users, "dick", -1, "", 0, TRUE);
      cbmapput(node->users, "lupin", -1, "", 0, TRUE);
      node_set_link(node, "http://localhost:1978/node/sample2",
                    "Sample-Node-Two", 4000);
      doc = est_doc_new();
      est_doc_add_attr(doc, ESTDATTRURI, "http://localhost/foo.html");
      est_doc_add_text(doc, "You may my glories and my state dispose,");
      est_doc_add_text(doc, "But not my griefs; still am I king of those.");
      est_doc_add_text(doc, "(Give it up, Yo!  Give it up, Yo!)");
      est_mtdb_put_doc(node->db, doc, 0);
      est_doc_delete(doc);
      doc = est_doc_new();
      est_doc_add_attr(doc, ESTDATTRURI, "http://localhost/bar.html");
      est_doc_add_text(doc, "The faster I go, the behinder I get.");
      est_doc_add_text(doc, "(Give it up, Yo!  Give it up, Yo!)");
      est_mtdb_put_doc(node->db, doc, 0);
      est_doc_delete(doc);
    } else {
      err = TRUE;
    }
    nmgr_put(nmgr, "sample2", TRUE, 0);
    if((node = nmgr_get(nmgr, "sample2")) != NULL){
      free(node->label);
      node->label = cbmemdup("Sample Node Two", -1);
      cbmapput(node->users, "john", -1, "", 0, TRUE);
      node_set_link(node, "http://localhost:1978/node/sample1",
                    "Sample-Node-One", 8000);
      doc = est_doc_new();
      est_doc_add_attr(doc, ESTDATTRURI, "http://localhost/foo.html");
      est_doc_add_text(doc, "He that is giddy thinks the world turns round.");
      est_doc_add_text(doc, "(Give it up, Yo!  Give it up, Yo!)");
      est_mtdb_put_doc(node->db, doc, 0);
      est_doc_delete(doc);
    } else {
      err = TRUE;
    }
  }
  if(!nmgr_delete(nmgr)) err = TRUE;
  return err ? 1 : 0;
}


/* perform the start command */
static int procstart(const char *rootdir){
  char *path;
  path = est_realpath(rootdir);
  cbglobalgc(path, free);
  g_rootdir = path;
  if(!est_init_net_env()) die("could not initialize network environment");
  atexit(est_free_net_env);
  do {
    setsignals();
    startup();
    dispatch();
    cleanup();
  } while(g_sigrestart);
  return 0;
}


/* perform the stop command */
static int procstop(const char *rootdir){
  struct stat sbuf;
  char path[URIBUFSIZ], *buf;
  int pid;
  sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, PIDFILE);
  if(!(buf = cbreadfile(path, NULL))) die("not running");
  pid = atoi(buf);
  free(buf);
  if(pid < 1) die("not running");
  if(est_kill(pid, 15)){
    do {
      est_usleep(1000 * 500);
      if(!est_kill(pid, 13) && errno == ESRCH) break;
    } while(TRUE);
  } else {
    sprintf(path, "%s%c%s", rootdir, ESTPATHCHR, STOPFILE);
    if(!cbwritefile(path, "OH, MY GOODNESS!\n", -1)) die("killing failed");
    do {
      est_usleep(1000 * 500);
      if(stat(path, &sbuf) == -1 && errno == ENOENT) break;
    } while(TRUE);
  }
  return 0;
}


/* perform the unittest command */
static int procunittest(const char *rootdir){
  UMGR *umgr;
  USER *user;
  NMGR *nmgr;
  NODE *node;
  RWLOCK *rwlock;
  RESMAP *resmap;
  RESDOC **resdocs;
  ESTNODERES *nres;
  ESTRESDOC *rdoc;
  ESTDOC *doc;
  CBLIST *list;
  CBMAP *attrs;
  const char *name, *path;
  char *str;
  int i, err, rnum;
  setsignals();
  if(procinit(rootdir, TRUE) != 0) return 1;
  log_open(rootdir, LOGFILE, LL_DEBUG, FALSE);
  err = FALSE;
  umgr = umgr_new(rootdir);
  umgr_load(umgr);
  umgr_put(umgr, "mikio", "x", "", "Mikio Hirabayashi", "Test");
  umgr_put(umgr, "mikio", "x", "", "Mikio Hirabayashi", "Test");
  list = umgr_names(umgr);
  for(i = 0; i < cblistnum(list); i++){
    name = cblistval(list, i, NULL);
    if(!(user = umgr_get(umgr, name))){
      err = TRUE;
      continue;
    }
    if(i % 2 == 0) user_make_sess(user);
    if(i % 3 == 0){
      user_set_sess_val(user, "oda", "nobunaga");
      user_set_sess_val(user, "hashiba", "hideyoshi");
      user_set_sess_val(user, "ieyasu", "tokugawa");
      user_set_sess_val(user, "oda", NULL);
      free(user_sess_val(user, "oda"));
      free(user_sess_val(user, "hashiba"));
      user_clear_sess(user);
      free(user_sess_val(user, "ieyasu"));
    }
  }
  cblistclose(list);
  umgr_out(umgr, "mikio");
  umgr_out(umgr, "hoge");
  if(!umgr_delete(umgr)) err = TRUE;
  nmgr = nmgr_new(rootdir);
  nmgr_load(nmgr, TRUE);
  nmgr_put(nmgr, "eagle", TRUE, 0);
  nmgr_put(nmgr, "shark", TRUE, 0);
  nmgr_put(nmgr, "panther", TRUE, 0);
  nmgr_put(nmgr, "i n v a l i d", TRUE, 0);
  list = nmgr_names(nmgr);
  for(i = 0; i < cblistnum(list); i++){
    path = cblistval(list, i, NULL);
    if(!(node = nmgr_get(nmgr, path))){
      err = TRUE;
      continue;
    }
    doc = est_doc_new();
    est_doc_add_attr(doc, ESTDATTRURI, "file:///home/mikio/sample1.html");
    est_doc_add_text(doc, "Happy Hacking!");
    if(!est_mtdb_put_doc(node->db, doc, 0)) err = TRUE;
    est_doc_delete(doc);
    doc = est_doc_new();
    est_doc_add_attr(doc, ESTDATTRURI, "file:///home/mikio/sample2.txt");
    est_doc_add_text(doc, "The savior becomes the victim.");
    if(!est_mtdb_put_doc(node->db, doc, 0)) err = TRUE;
    est_doc_delete(doc);
    node_set_link(node, "http://hoge.com/node/hoge1", "a", 100);
    node_set_link(node, "http://hoge.com/node/hoge2", "b", 200);
    node_set_link(node, "http://hoge.com/node/hoge2", "c", 300);
  }
  cblistclose(list);
  nmgr_out(nmgr, "eagle");
  nmgr_out(nmgr, "hoge");
  nmgr_delete(nmgr);
  rwlock = rwlock_new();
  if(!rwlock_lock(rwlock, TRUE)) err = TRUE;
  if(!rwlock_unlock(rwlock)) err = TRUE;
  if(!rwlock_lock(rwlock, FALSE)) err = TRUE;
  if(!rwlock_unlock(rwlock)) err = TRUE;
  rwlock_delete(rwlock);
  resmap = resmap_new();
  doc = est_doc_new();
  est_doc_add_attr(doc, ESTDATTRURI, "http://tako.ika/one.html");
  est_doc_add_text(doc, "This is a pen.");
  resmap_put(resmap, 100, doc, NULL, NULL);
  doc = est_doc_new();
  est_doc_add_attr(doc, ESTDATTRURI, "http://tako.ika/two.html");
  est_doc_add_text(doc, "Love is stranger.");
  resmap_put(resmap, 200, doc, NULL, NULL);
  doc = est_doc_new();
  est_doc_add_attr(doc, ESTDATTRURI, "http://tako.ika/001.html");
  resmap_put(resmap, 1, doc, NULL, cbmemdup("one", -1));
  doc = est_doc_new();
  est_doc_add_attr(doc, ESTDATTRURI, "http://tako.ika/006.html");
  resmap_put(resmap, 6, doc, NULL, cbmemdup("six", -1));
  doc = est_doc_new();
  est_doc_add_attr(doc, ESTDATTRURI, "http://tako.ika/003.html");
  resmap_put(resmap, 3, doc, NULL, cbmemdup("three", -1));
  doc = est_doc_new();
  est_doc_add_attr(doc, ESTDATTRURI, "http://tako.ika/004.html");
  resmap_put(resmap, 4, doc, NULL, cbmemdup("four", -1));
  doc = est_doc_new();
  est_doc_add_attr(doc, ESTDATTRURI, "http://tako.ika/002.html");
  resmap_put(resmap, 2, doc, NULL, cbmemdup("two", -1));
  doc = est_doc_new();
  est_doc_add_attr(doc, ESTDATTRURI, "http://tako.ika/005.html");
  resmap_put(resmap, 1, doc, NULL, cbmemdup("five", -1));
  resdocs = resmap_list(resmap, &rnum, NULL, NULL);
  for(i = 0; i < rnum; i++){
    log_print(LL_DEBUG, "result: uri=%s score=%d body=%s",
              est_doc_attr(resdocs[i]->doc, ESTDATTRURI), resdocs[i]->score,
              resdocs[i]->body ? resdocs[i]->body : "(null)");
  }
  free(resdocs);
  resmap_delete(resmap);
  nres = est_noderes_new();
  for(i = 0; i < 128; i++){
    attrs = cbmapopenex(MINIBNUM);
    str = cbsprintf("http://big/bigger/biggest/%d", i + 1);
    if(i % 10 != 0) cbmapput(attrs, ESTDATTRURI, -1, str, -1, FALSE);
    free(str);
    str = cbsprintf("This is %d\n", i + 1);
    est_noderes_add_doc(nres, attrs, str);
  }
  for(i = 0; i < 100 && est_noderes_shift_doc(nres, &attrs, &str); i++){
    free(str);
    cbmapclose(attrs);
  }
  for(i = 0; i < est_noderes_doc_num(nres); i++){
    rdoc = est_noderes_get_doc(nres, i);
    log_print(LL_DEBUG, "%s", est_resdoc_uri(rdoc));
  }
  for(i = 0; i < 2048; i++){
    attrs = cbmapopenex(MINIBNUM);
    str = cbsprintf("http://big/bigger/biggest/%d", i + 1);
    if(i % 10 != 0) cbmapput(attrs, ESTDATTRURI, -1, str, -1, FALSE);
    free(str);
    str = cbsprintf("This is %d\n", i + 1);
    est_noderes_add_doc(nres, attrs, str);
  }
  est_noderes_delete(nres);
  if(!err) log_print(LL_INFO, "finished successfully");
  return err ? 1 : 0;
}


/* perform the crypt command */
static int proccrypt(const char *key, const char *hash){
  char *tmp;
  if(hash){
    if(!est_match_crypt(key, hash)){
      fprintf(stderr, "%s and %s do not match", key, hash);
      return 1;
    }
  } else {
    tmp = est_make_crypt(key);
    printf("%s\n", tmp);
    free(tmp);
  }
  return 0;
}


/* listen the socket and dispatch processes */
static void dispatch(void){
  pthread_t th;
  fd_set rfds;
  struct timeval tv;
  struct stat sbuf;
  char *stfile, claddr[ADDRBUFSIZ];
  int clsock, clport, isec, cnum;
  time_t start;
  TARGCOMM *targ;
  log_print(LL_INFO, "waiting for requests");
  stfile = cbsprintf("%s%c%s", g_rootdir, ESTPATHCHR, STOPFILE);
  isec = 0;
  start = -1;
  while(TRUE){
    FD_ZERO(&rfds);
    FD_SET(g_svsock, &rfds);
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    errno = 0;
    if(select(g_svsock + 1, &rfds, NULL, NULL, &tv) > 0 && FD_ISSET(g_svsock, &rfds)){
      clsock = est_accept_conn(g_svsock, claddr, &clport);
      switch(clsock){
      case -1:
        log_print(LL_ERROR, "accepting connection failed");
        break;
      case 0:
        log_print(LL_WARN, "accepting connection interrupted by a signal");
        break;
      default:
        if(rwlock_rnum(g_runlock) > g_maxconn + FCLOSECONNNUM){
          log_print(LL_WARN, "doing forced shutdown due to jam-up");
          est_sock_down(clsock);
        } else {
          g_accesscount++;
          targ = cbmalloc(sizeof(TARGCOMM));
          targ->clsock = clsock;
          sprintf(targ->claddr, "%s", claddr);
          targ->clport = clport;
          if(g_stmode){
            communicate(targ);
          } else {
            if(pthread_create(&th, NULL, communicate, targ) == 0){
              if(pthread_detach(th) != 0) log_print(LL_ERROR, "detachting thread failed");
            } else {
              log_print(LL_WARN, "creating thread failed");
              est_sock_down(clsock);
              free(targ);
            }
          }
        }
        break;
      }
      isec = 0;
    } else {
      if(errno != EINTR) isec++;
      if(stat(stfile, &sbuf) == 0){
        log_print(LL_INFO, "the stop file detected");
        g_sigterm = TRUE;
      }
    }
    if(g_sigterm){
      if(rwlock_rnum(g_runlock) < 1){
        break;
      } else if(start == -1){
        start = time(NULL);
      } else if(time(NULL) - start > 1){
        break;
      }
    }
    if(isec >= IREFRESHSEC && g_runmode != RM_RDONLY){
      if(g_stmode){
        refreshnode(NULL);
      } else {
        if(pthread_create(&th, NULL, refreshnode, NULL) == 0){
          if(pthread_detach(th) != 0) log_print(LL_ERROR, "detachting thread failed");
        } else {
          log_print(LL_WARN, "creating thread failed");
        }
      }
      est_usleep(1000 * 100);
    }
    if(isec >= g_idleflush * (1.0 - g_cacheratio) &&
       (g_cacheratio > 0.0 || isec % (g_idleflush / 2 + 1) == 0)){
      if(g_stmode){
        flushnode(NULL);
      } else {
        if(pthread_create(&th, NULL, flushnode, NULL) == 0){
          if(pthread_detach(th) != 0) log_print(LL_ERROR, "detachting thread failed");
        } else {
          log_print(LL_WARN, "creating thread failed");
        }
      }
      est_usleep(1000 * 100);
    }
    if(g_sigsync || g_sigback || isec >= g_idlesync){
      if(g_cacheratio > 0.0){
        if(g_stmode){
          flushnode(NULL);
        } else {
          if(pthread_create(&th, NULL, flushnode, NULL) == 0){
            if(pthread_detach(th) != 0) log_print(LL_ERROR, "detachting thread failed");
          } else {
            log_print(LL_WARN, "creating thread failed");
          }
        }
      } else {
        if(g_stmode){
          syncnodes(NULL);
        } else {
          if(pthread_create(&th, NULL, syncnodes, NULL) == 0){
            if(pthread_detach(th) != 0) log_print(LL_ERROR, "detachting thread failed");
          } else {
            log_print(LL_WARN, "creating thread failed");
          }
        }
        g_sigsync = FALSE;
        isec = 0;
        if(g_sigback){
          backupdb();
          g_sigback = FALSE;
        }
      }
      est_usleep(1000 * 100);
    }
  }
  free(stfile);
  log_print(LL_INFO, "shutting down");
  start = time(NULL);
  while((cnum = rwlock_rnum(g_runlock)) > 0){
    log_print(LL_INFO, "waiting for children: %d", cnum);
    est_usleep(1000 * 1000);
    if(time(NULL) - start > FCLOSEWAITMAX && g_svsock != -1){
      est_sock_down(g_svsock);
      g_svsock = -1;
      log_print(LL_WARN, "doing forced shutdown of the server socket");
    }
    if(time(NULL) - start > FTERMWAITMAX){
      log_print(LL_WARN, "doing forced termination of the server process");
      break;
    }
  }
  est_usleep(1000 * 100);
}


/* refresh or charge result cache of one of nodes */
static void *refreshnode(void *targ){
  static int cnt = 0;
  CBLIST *list;
  NODE *node;
  if(pthread_mutex_trylock(&g_backlock) != 0) return NULL;
  if(!rwlock_lock(g_runlock, FALSE)){
    log_print(LL_ERROR, "locking failed");
    pthread_mutex_unlock(&g_backlock);
    return NULL;
  }
  if(!rwlock_lock(g_mgrlock, FALSE)){
    rwlock_unlock(g_runlock);
    pthread_mutex_unlock(&g_backlock);
    log_print(LL_ERROR, "locking failed");
    return NULL;
  }
  list = nmgr_names(g_nmgr);
  if(cblistnum(list) > 0){
    if((node = nmgr_get(g_nmgr, cblistval(list, cnt++ % cblistnum(list), NULL))) != NULL){
      if(node->dirty){
        log_print(LL_INFO, "refreshing a node: %s", node->name);
        est_mtdb_refresh_rescc(node->db);
        node->dirty = FALSE;
      } else {
        est_mtdb_charge_rescc(node->db, ICHARGENUM);
      }
    }
  }
  cblistclose(list);
  if(!rwlock_unlock(g_mgrlock)) log_print(LL_ERROR, "unlocking failed");
  if(!rwlock_unlock(g_runlock)) log_print(LL_ERROR, "unlocking failed");
  if(pthread_mutex_unlock(&g_backlock) != 0) log_print(LL_ERROR, "unlocking failed");
  return NULL;
}


/* flush one of nodes */
static void *flushnode(void *targ){
  static int cnt = 0;
  CBLIST *list;
  NODE *node;
  int size;
  double msiz;
  if(g_runmode == RM_RDONLY) return NULL;
  if(pthread_mutex_trylock(&g_backlock) != 0) return NULL;
  if(!rwlock_lock(g_runlock, FALSE)){
    log_print(LL_ERROR, "locking failed");
    pthread_mutex_unlock(&g_backlock);
    return NULL;
  }
  if(!rwlock_lock(g_mgrlock, FALSE)){
    rwlock_unlock(g_runlock);
    pthread_mutex_unlock(&g_backlock);
    log_print(LL_ERROR, "locking failed");
    return NULL;
  }
  list = nmgr_names(g_nmgr);
  msiz = 0.0;
  if(cblistnum(list) > 0){
    if((node = nmgr_get(g_nmgr, cblistval(list, cnt++ % cblistnum(list), NULL))) != NULL){
      if(!est_mtdb_flush(node->db, est_mtdb_cache_num(node->db) > 0 ? IFLUSHNUM : -1))
        log_print(LL_ERROR, "flushing failed");
      if((size = est_mtdb_used_cache_size(node->db)) > msiz) msiz = size;
    }
  }
  g_cacheratio = msiz / g_cachesize;
  cblistclose(list);
  if(!rwlock_unlock(g_mgrlock)) log_print(LL_ERROR, "unlocking failed");
  if(!rwlock_unlock(g_runlock)) log_print(LL_ERROR, "unlocking failed");
  if(pthread_mutex_unlock(&g_backlock) != 0) log_print(LL_ERROR, "unlocking failed");
  return NULL;
}


/* synchronize all nodes */
static void *syncnodes(void *targ){
  if(g_runmode == RM_RDONLY) return NULL;
  if(pthread_mutex_lock(&g_backlock) != 0) return NULL;
  if(!rwlock_lock(g_runlock, FALSE)){
    log_print(LL_ERROR, "locking failed");
    pthread_mutex_unlock(&g_backlock);
    return NULL;
  }
  if(!rwlock_lock(g_mgrlock, TRUE)){
    rwlock_unlock(g_runlock);
    pthread_mutex_unlock(&g_backlock);
    log_print(LL_ERROR, "locking failed");
    return NULL;
  }
  umgr_sync(g_umgr);
  nmgr_sync(g_nmgr, TRUE);
  if(!dpsync(g_metadb)) log_print(LL_ERROR, "synchronizing the meda database failed");
  if(!rwlock_unlock(g_mgrlock)) log_print(LL_ERROR, "unlocking failed");
  if(!rwlock_unlock(g_runlock)) log_print(LL_ERROR, "unlocking failed");
  if(pthread_mutex_unlock(&g_backlock) != 0) log_print(LL_ERROR, "unlocking failed");
  return NULL;
}


/* backup the database */
static void backupdb(void){
  char cmd[URIBUFSIZ], *tmp;
  if(g_backupcmd[0] == '\0') return;
  syncnodes(NULL);
  if(!rwlock_lock(g_runlock, FALSE)){
    log_print(LL_ERROR, "locking failed");
    return;
  }
  if(!rwlock_lock(g_mgrlock, TRUE)){
    rwlock_unlock(g_runlock);
    log_print(LL_ERROR, "locking failed");
    return;
  }
  log_print(LL_INFO, "running the backup command");
  tmp = est_realpath(g_indexfile);
  sprintf(cmd, "%s \"%s\"", g_backupcmd, g_rootdir);
  free(tmp);
  system(cmd);
  if(!rwlock_unlock(g_mgrlock)) log_print(LL_ERROR, "unlocking failed");
  if(!rwlock_unlock(g_runlock)) log_print(LL_ERROR, "unlocking failed");
}


/* communicate with a client */
static void *communicate(void *targ){
  TARGCOMM *myarg;
  REQUEST req;
  USER *user;
  const char *cstr;
  char ibuf[IOBUFSIZ], *tmp, *bp, *ep;
  int clsock, len, clen;
  myarg = (TARGCOMM *)targ;
  clsock = myarg->clsock;
  if(!rwlock_lock(g_runlock, FALSE)){
    est_sock_down(myarg->clsock);
    free(myarg);
    log_print(LL_ERROR, "locking failed");
    return NULL;
  }
  est_sock_recv_line(clsock, ibuf, IOBUFSIZ - 1);
  log_print(LL_INFO, "[%s:%d]: %s", myarg->claddr, myarg->clport, ibuf);
  if(cbstrfwmatch(ibuf, "HEAD ")){
    req.method = HM_HEAD;
  } else if(cbstrfwmatch(ibuf, "GET ")){
    req.method = HM_GET;
  } else if(cbstrfwmatch(ibuf, "POST ")){
    req.method = HM_POST;
  } else if(cbstrfwmatch(ibuf, "OPTIONS ")){
    req.method = HM_OPTIONS;
  } else {
    req.method = HM_UNKNOWN;
  }
  req.claddr = myarg->claddr;
  req.clport = myarg->clport;
  sprintf(req.prefix, "http://%s:%d", g_hostname, g_portnum);
  req.target = NULL;
  req.name = NULL;
  req.passwd = NULL;
  req.ims = -1;
  req.gzip = FALSE;
  req.deflate = FALSE;
  req.ctype = NULL;
  req.referer = NULL;
  req.estvia = NULL;
  req.body = NULL;
  req.params = cbmapopenex(MINIBNUM);
  req.now = time(NULL);
  req.reload = FALSE;
  if((bp = strchr(ibuf, ' ')) != NULL){
    while(*bp == ' '){
      bp++;
    }
    while(bp[0] == '/' && bp[1] == '/'){
      bp++;
    }
    if((ep = strchr(bp, ' ')) != NULL){
      req.target = cbmemdup(bp, ep - bp);
    } else {
      req.target = cbmemdup(bp, -1);
    }
    if((bp = strchr(req.target, '?')) != NULL){
      *(bp++) = '\0';
      setparams(req.params, bp);
    }
  }
  if(!req.target) req.target = cbmemdup("/", 1);
  clen = -1;
  do {
    len = est_sock_recv_line(clsock, ibuf, IOBUFSIZ - 1);
    if(len > 0) log_print(LL_DEBUG, "[%s:%d]: + %s", myarg->claddr, myarg->clport, ibuf);
    if(cbstrfwimatch(ibuf, "Host:")){
      ibuf[HOSTBUFSIZ-1] = '\0';
      sprintf(req.prefix, "http://%s", skiplabel(ibuf));
    } else if(cbstrfwimatch(ibuf, "Content-Length:")){
      clen = atoi(skiplabel(ibuf));
    } else if(cbstrfwimatch(ibuf, "Content-Type:")){
      req.ctype = cbmemdup(skiplabel(ibuf), -1);
    } else if(cbstrfwimatch(ibuf, "Referer:")){
      req.referer = cbmemdup(skiplabel(ibuf), -1);
    } else if(cbstrfwimatch(ibuf, "X-Estraier-Via:")){
      if(req.estvia){
        tmp = cbsprintf("%s, %s", req.estvia, skiplabel(ibuf));
        free(req.estvia);
        req.estvia = tmp;
      } else {
        req.estvia = cbmemdup(skiplabel(ibuf), -1);
      }
    } else if(cbstrfwimatch(ibuf, "Authorization:")){
      cstr = skiplabel(ibuf);
      if(cbstrfwimatch(cstr, "basic") && (cstr = strchr(cstr, ' ')) != NULL){
        while(*cstr == ' '){
          cstr++;
        }
        bp = cbbasedecode(cstr, NULL);
        if((ep = strchr(bp, ':')) != NULL) *(ep++) = '\0';
        req.name = cbmemdup(bp, -1);
        req.passwd = cbmemdup(ep ? ep : "", -1);
        free(bp);
      }
    } else if(cbstrfwimatch(ibuf, "If-Modified-Since:")){
      req.ims = cbstrmktime(skiplabel(ibuf));
    } else if(cbstrfwimatch(ibuf, "Accept-Encoding:")){
      cstr = skiplabel(ibuf);
      if(ESTUSEZLIB && strstr(cstr, "gzip")) req.gzip = TRUE;
      if(ESTUSEZLIB && strstr(cstr, "deflate")) req.deflate = TRUE;
    }
  } while(len > 0);
  if(g_publicurl[0] != '\0') sprintf(req.prefix, "%s", g_publicurl);
  if(clen > g_recvmax) clen = g_recvmax;
  if(req.method == HM_POST && clen > 0){
    req.body = est_sock_recv_all(clsock, clen);
    if(req.ctype && cbstrfwimatch(req.ctype, ESTFORMTYPE)) setparams(req.params, req.body);
    est_sock_recv_void(clsock);
  }
  if(req.ims > 0 && req.ims < req.now - UICACHELIFE) req.reload = TRUE;
  user = NULL;
  if(rwlock_lock(g_mgrlock, FALSE)){
    if(req.name) user = umgr_get(g_umgr, req.name);
    if(user && !est_match_crypt(req.passwd, user->passwd)) user = NULL;
    if(!rwlock_unlock(g_mgrlock)) log_print(LL_ERROR, "unlocking failed");
  } else {
    log_print(LL_ERROR, "locking failed");
  }
  if(g_sigterm){
    senderror(clsock, &req, 503, "Service Unavailable (shutting down)");
  } else if(rwlock_rnum(g_runlock) > g_maxconn){
    senderror(clsock, &req, 503, "Service Unavailable (too busy)");
  } else if(req.method == HM_UNKNOWN){
    senderror(clsock, &req, 501, "Not Implemented");
  } else if(req.method == HM_POST && clen < 0){
    senderror(clsock, &req, 411, "Length Required");
  } else if(req.method == HM_POST && clen >= g_recvmax){
    senderror(clsock, &req, 413, "Request Entity Too Large");
  } else if(req.target[0] != '/' || strstr(req.target, "/../")){
    senderror(clsock, &req, 400, "Bad Request (invalid URL)");
  } else if(g_denyuntrusted && !cbmapget(g_trustednodes, req.claddr, -1, NULL)){
    senderror(clsock, &req, 403, "Forbidden (untrusted node)");
  } else if(req.method == HM_OPTIONS){
    sendoptions(clsock, &req, user);
  } else if(!strcmp(req.target, MASTERLOC)){
    sendmasterdata(clsock, &req, user);
  } else if(cbstrfwmatch(req.target, NODEPREFIX)){
    sendnodedata(clsock, &req, user, req.target + strlen(NODEPREFIX));
  } else if(!strcmp(req.target, MASTERUILOC)){
    sendmasteruidata(clsock, &req, user);
  } else if(!strcmp(req.target, FAVICONLOC)){
    sendfavicondata(clsock, &req);
  } else if(cbstrfwmatch(req.target, IMAGEPREFIX)){
    sendimagedata(clsock, &req, req.target + strlen(IMAGEPREFIX));
  } else if(g_docroot[0] != '\0'){
    sendfiledata(clsock, &req);
  } else if(!strcmp(req.target, "/")){
    sendmenudata(clsock, &req);
  } else {
    senderror(clsock, &req, 404, "Not Found");
  }
  cbmapclose(req.params);
  free(req.body);
  free(req.estvia);
  free(req.referer);
  free(req.ctype);
  free(req.name);
  free(req.passwd);
  free(req.target);
  est_sock_down(myarg->clsock);
  free(myarg);
  if(!rwlock_unlock(g_runlock)) log_print(LL_ERROR, "unlocking failed");
  return NULL;
}


/* set parameters into a map object */
static void setparams(CBMAP *params, const char *src){
  CBLIST *pairs;
  char *key, *val, *dkey, *dval;
  int i;
  pairs = cbsplit(src, -1, "&");
  for(i = 0; i < cblistnum(pairs); i++){
    key = cbmemdup(cblistval(pairs, i, NULL), -1);
    if((val = strchr(key, '=')) != NULL){
      *(val++) = '\0';
      dkey = cburldecode(key, NULL);
      dval = cburldecode(val, NULL);
      cbmapput(params, dkey, -1, dval, -1, FALSE);
      free(dval);
      free(dkey);
    }
    free(key);
  }
  cblistclose(pairs);
}


/* add the server information to a datum object */
static void addservinfo(CBDATUM *datum, time_t now, int ccage){
  char *tmp;
  tmp = cbdatestrhttp(now, 0);
  cbdatumprintf(datum, "Date: %s\r\n", tmp);
  free(tmp);
  cbdatumprintf(datum, "Server: %s/%s\r\n", SERVNAME, est_version);
  if(ccage < 1){
    cbdatumprintf(datum, "Cache-Control: no-cache, must-revalidate, no-transform\r\n");
    cbdatumprintf(datum, "Pragma: no-cache\r\n");
  } else {
    cbdatumprintf(datum, "Cache-Control: max-age=%d, no-transform\r\n", ccage);
    tmp = cbdatestrhttp(now + ccage, 0);
    cbdatumprintf(datum, "Expires: %s\r\n", tmp);
    free(tmp);
  }
  cbdatumprintf(datum, "Connection: close\r\n");
}


/* send error message */
static void senderror(int clsock, REQUEST *req, int code, const char *message){
  CBDATUM *datum;
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 %d %s\r\n", code, message);
  addservinfo(datum, req->now, 0);
  cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
  cbdatumprintf(datum, "\r\n");
  if(req->method != HM_HEAD) cbdatumprintf(datum, "%s\n", message);
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: %d %s", req->claddr, req->clport, code, message);
}


/* send authenticate error message */
static void sendautherror(int clsock, REQUEST *req, const char *realm){
  CBDATUM *datum;
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 401 Unauthorized\r\n");
  addservinfo(datum, req->now, 0);
  cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
  cbdatumprintf(datum, "WWW-Authenticate: Basic realm=\"%s\"\r\n", realm);
  cbdatumprintf(datum, "\r\n");
  if(req->method != HM_HEAD) cbdatumprintf(datum, "Unauthorized\n");
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: 401 Unauthorized", req->claddr, req->clport);
}


/* send not modified error message */
static void sendnotmoderror(int clsock, REQUEST *req){
  CBDATUM *datum;
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 304 Not Modified\r\n");
  addservinfo(datum, req->now, 0);
  cbdatumprintf(datum, "\r\n");
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: 304 Not Modified", req->claddr, req->clport);
}


/* send header only message */
static void sendheadonly(int clsock, REQUEST *req, const char *type){
  CBDATUM *datum;
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
  addservinfo(datum, req->now, 0);
  cbdatumprintf(datum, "Content-Type: %s\r\n", type);
  cbdatumprintf(datum, "\r\n");
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: 200 OK (head)", req->claddr, req->clport);
}


/* check whether a user is banned */
static int isbanned(USER *user){
  if(user && strchr(user->flags, 'b')) return TRUE;
  return FALSE;
}


/* check whether a user is an administrator of the master */
static int ismasteradmin(REQUEST *req, USER *user){
  if(isbanned(user)) return FALSE;
  if(cbmapget(g_trustednodes, req->claddr, -1, NULL)) return TRUE;
  if(user && strchr(user->flags, 's')) return TRUE;
  return FALSE;
}


/* check whether a user is an administrator of a node */
static int isnodeadmin(NODE *node, REQUEST *req, USER *user){
  if(isbanned(user)) return FALSE;
  if(ismasteradmin(req, user)) return TRUE;
  if(g_authmode == AM_NONE) return TRUE;
  if(user && cbmapget(node->admins, user->name, -1, NULL)) return TRUE;
  return FALSE;
}


/* check whether a user is a user of a node */
static int isnodeuser(NODE *node, REQUEST *req, USER *user){
  if(isbanned(user)) return FALSE;
  if(isnodeadmin(node, req, user)) return TRUE;
  if(g_authmode == AM_NONE || g_authmode == AM_ADMIN) return TRUE;
  if(user && cbmapget(node->users, user->name, -1, NULL)) return TRUE;
  return FALSE;
}


/* get the options for node database */
static int getnodeoptions(void){
  int options;
  options = 0;
  switch(g_scalepred){
  case SP_SMALL:
    options |= ESTDBSMALL;
    break;
  default:
    break;
  case SP_LARGE:
    options |= ESTDBLARGE;
    break;
  case SP_HUGE:
    options |= ESTDBHUGE;
    break;
  }
  switch(g_scoreexpr){
  case SE_VOID:
    options |= ESTDBSCVOID;
    break;
  default:
    break;
  case SE_INT:
    options |= ESTDBSCINT;
    break;
  case SE_ASIS:
    options |= ESTDBSCASIS;
    break;
  }
  return options;
}


/* send the option messages */
static void sendoptions(int clsock, REQUEST *req, USER *user){
  CBDATUM *datum;
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
  addservinfo(datum, req->now, 0);
  cbdatumprintf(datum, "Allow: GET,HEAD,POST,OPTIONS\r\n");
  cbdatumprintf(datum, "\r\n");
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: 200 OK (options)", req->claddr, req->clport);
}


/* send the master data */
static void sendmasterdata(int clsock, REQUEST *req, USER *user){
  CBDATUM *datum;
  CBLIST *list;
  USER *tuser;
  NODE *tnode;
  const char *act, *name, *passwd, *flags, *fname, *misc, *label;
  char *tmp;
  int i;
  if(!rwlock_lock(g_mgrlock, TRUE)){
    log_print(LL_ERROR, "locking failed");
    senderror(clsock, req, 500, "Internal Server Error (locking failed)");
    return;
  }
  if(req->referer && !cbstrfwmatch(req->referer, req->prefix)){
    senderror(clsock, req, 403, "Forbidden (invalid referrer)");
    if(!rwlock_unlock(g_mgrlock)) log_print(LL_ERROR, "unlocking failed");
    return;
  }
  if(!ismasteradmin(req, user)){
    sendautherror(clsock, req, "Super User");
    if(!rwlock_unlock(g_mgrlock)) log_print(LL_ERROR, "unlocking failed");
    return;
  }
  if(!(act = cbmapget(req->params, "action", -1, NULL))) act = "";
  if(!(name = cbmapget(req->params, "name", -1, NULL))) name = "";
  if(!(passwd = cbmapget(req->params, "passwd", -1, NULL))) passwd = "";
  if(!(flags = cbmapget(req->params, "flags", -1, NULL))) flags = "";
  if(!(fname = cbmapget(req->params, "fname", -1, NULL))) fname = "";
  if(!(misc = cbmapget(req->params, "misc", -1, NULL))) misc = "";
  if(!(label = cbmapget(req->params, "label", -1, NULL))) label = "";
  if(!strcmp(act, "shutdown")){
    datum = cbdatumopen(NULL, -1);
    cbdatumprintf(datum, "HTTP/1.0 202 Accepted\r\n");
    addservinfo(datum, req->now, 0);
    cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
    cbdatumprintf(datum, "\r\n");
    cbdatumprintf(datum, "Accepted\n");
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    g_sigterm = TRUE;
    cbdatumclose(datum);
    log_print(LL_DEBUG, "[%s:%d]: 202 (accepted - shutdown)", req->claddr, req->clport);
  } else if(!strcmp(act, "sync")){
    datum = cbdatumopen(NULL, -1);
    cbdatumprintf(datum, "HTTP/1.0 202 Accepted\r\n");
    addservinfo(datum, req->now, 0);
    cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
    cbdatumprintf(datum, "\r\n");
    cbdatumprintf(datum, "Accepted\n");
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    g_sigsync = TRUE;
    cbdatumclose(datum);
    log_print(LL_DEBUG, "[%s:%d]: 202 (accepted - sync)", req->claddr, req->clport);
  } else if(!strcmp(act, "backup")){
    datum = cbdatumopen(NULL, -1);
    cbdatumprintf(datum, "HTTP/1.0 202 Accepted\r\n");
    addservinfo(datum, req->now, 0);
    cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
    cbdatumprintf(datum, "\r\n");
    cbdatumprintf(datum, "Accepted\n");
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    g_sigback = TRUE;
    cbdatumclose(datum);
    log_print(LL_DEBUG, "[%s:%d]: 202 (accepted - backup)", req->claddr, req->clport);
  } else if(!strcmp(act, "userlist")){
    datum = cbdatumopen(NULL, -1);
    cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
    addservinfo(datum, req->now, 0);
    cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
    cbdatumprintf(datum, "\r\n");
    list = umgr_names(g_umgr);
    for(i = 0; i < cblistnum(list); i++){
      if(!(name = cblistval(list, i, NULL)) || !(tuser = umgr_get(g_umgr, name))) continue;
      cbdatumprintf(datum, "%s\t%s\t%s\t%s\t%s\n",
                    tuser->name, tuser->passwd, tuser->flags, tuser->fname, tuser->misc);
    }
    cblistclose(list);
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    cbdatumclose(datum);
    log_print(LL_DEBUG, "[%s:%d]: 200 OK (userlist)", req->claddr, req->clport);
  } else if(!strcmp(act, "useradd") && name[0] != '\0' && passwd[0] != '\0'){
    tmp = est_make_crypt(passwd);
    if(g_runmode == RM_RDONLY){
      senderror(clsock, req, 503, "Service Unavailable (read only)");
    } else if(umgr_put(g_umgr, name, tmp, flags, fname, misc)){
      datum = cbdatumopen(NULL, -1);
      cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
      addservinfo(datum, req->now, 0);
      cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
      cbdatumprintf(datum, "\r\n");
      cbdatumprintf(datum, "OK\n");
      est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
      cbdatumclose(datum);
      log_print(LL_DEBUG, "[%s:%d]: 200 OK (useradd)", req->claddr, req->clport);
    } else {
      senderror(clsock, req, 400, "Bad Request (maybe, the user already exists)");
    }
    free(tmp);
  } else if(!strcmp(act, "userdel") && name[0] != '\0'){
    if(g_runmode == RM_RDONLY){
      senderror(clsock, req, 503, "Service Unavailable (read only)");
    } else if(umgr_out(g_umgr, name)){
      datum = cbdatumopen(NULL, -1);
      cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
      addservinfo(datum, req->now, 0);
      cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
      cbdatumprintf(datum, "\r\n");
      cbdatumprintf(datum, "OK\n");
      est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
      cbdatumclose(datum);
      log_print(LL_DEBUG, "[%s:%d]: 200 OK (userdel)", req->claddr, req->clport);
    } else {
      senderror(clsock, req, 400, "Bad Request (maybe, the user does not exist)");
    }
  } else if(!strcmp(act, "nodelist")){
    datum = cbdatumopen(NULL, -1);
    cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
    addservinfo(datum, req->now, 0);
    cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
    cbdatumprintf(datum, "\r\n");
    list = nmgr_names(g_nmgr);
    for(i = 0; i < cblistnum(list); i++){
      if(!(name = cblistval(list, i, NULL)) || !(tnode = nmgr_get(g_nmgr, name))) continue;
      cbdatumprintf(datum, "%s\t%s\t%d\t%d\t%.0f\n", tnode->name, tnode->label,
                    est_mtdb_doc_num(tnode->db), est_mtdb_word_num(tnode->db),
                    est_mtdb_size(tnode->db));
    }
    cblistclose(list);
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    cbdatumclose(datum);
    log_print(LL_DEBUG, "[%s:%d]: 200 OK (nodelist)", req->claddr, req->clport);
  } else if(!strcmp(act, "nodeadd") && name[0] != '\0'){
    if(g_runmode == RM_RDONLY){
      senderror(clsock, req, 503, "Service Unavailable (read only)");
    } else if(nmgr_put(g_nmgr, name, TRUE, getnodeoptions())){
      if((tnode = nmgr_get(g_nmgr, name)) != NULL){
        free(tnode->label);
        tnode->label = cbmemdup(label[0] != '\0' ? label : name, -1);
      }
      nmgr_sync(g_nmgr, FALSE);
      datum = cbdatumopen(NULL, -1);
      cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
      addservinfo(datum, req->now, 0);
      cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
      cbdatumprintf(datum, "\r\n");
      cbdatumprintf(datum, "OK\n");
      est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
      cbdatumclose(datum);
      log_print(LL_DEBUG, "[%s:%d]: 200 OK (nodeadd)", req->claddr, req->clport);
    } else {
      senderror(clsock, req, 400, "Bad Request (maybe, the node already exists)");
    }
  } else if(!strcmp(act, "nodedel") && name[0] != '\0'){
    if(g_runmode == RM_RDONLY){
      senderror(clsock, req, 503, "Service Unavailable (read only)");
    } else if(nmgr_out(g_nmgr, name)){
      datum = cbdatumopen(NULL, -1);
      cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
      addservinfo(datum, req->now, 0);
      cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
      cbdatumprintf(datum, "\r\n");
      cbdatumprintf(datum, "OK\n");
      est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
      cbdatumclose(datum);
      log_print(LL_DEBUG, "[%s:%d]: 200 OK (nodedel)", req->claddr, req->clport);
    } else {
      senderror(clsock, req, 400, "Bad Request (maybe, the node does not exist)");
    }
  } else if(!strcmp(act, "nodeclr") && name[0] != '\0'){
    if(g_runmode == RM_RDONLY){
      senderror(clsock, req, 503, "Service Unavailable (read only)");
    } else if(nmgr_clear(g_nmgr, name, getnodeoptions())){
      datum = cbdatumopen(NULL, -1);
      cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
      addservinfo(datum, req->now, 0);
      cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
      cbdatumprintf(datum, "\r\n");
      cbdatumprintf(datum, "OK\n");
      est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
      cbdatumclose(datum);
      log_print(LL_DEBUG, "[%s:%d]: 200 OK (nodeclr)", req->claddr, req->clport);
    } else {
      senderror(clsock, req, 400, "Bad Request (maybe, the node does not exist)");
    }
  } else if(!strcmp(act, "logrtt")){
    if(log_rotate(g_rootdir, g_logfile)){
      datum = cbdatumopen(NULL, -1);
      cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
      addservinfo(datum, req->now, 0);
      cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
      cbdatumprintf(datum, "\r\n");
      cbdatumprintf(datum, "OK\n");
      est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
      cbdatumclose(datum);
      log_print(LL_DEBUG, "[%s:%d]: 200 OK (nodeclr)", req->claddr, req->clport);
    } else {
      senderror(clsock, req, 500, "Internal Server Error (log rotation failed)");
    }
  } else {
    senderror(clsock, req, 400, "Bad Request (the action is invalid or lack of parameters)");
  }
  if(!rwlock_unlock(g_mgrlock)) log_print(LL_ERROR, "unlocking failed");
}


/* send a node data */
static void sendnodedata(int clsock, REQUEST *req, USER *user, const char *path){
  CBDATUM *datum;
  CBLIST *list;
  NODE *node;
  const char *cmd;
  char *pbuf, *pv;
  int i;
  pbuf = cbmemdup(path, -1);
  if((pv = strchr(path, '/')) != NULL){
    *pv = '\0';
    cmd = pv + 1;
  } else {
    cmd = "";
  }
  while(*cmd == '/'){
    cmd++;
  }
  if(!rwlock_lock(g_mgrlock, cmd[0] == '_')){
    log_print(LL_ERROR, "locking failed");
    senderror(clsock, req, 500, "Internal Server Error (locking failed)");
    free(pbuf);
    return;
  }
  if(!(node = nmgr_get(g_nmgr, path))){
    if(!strcmp(req->target, NODEPREFIX)){
      datum = cbdatumopen(NULL, -1);
      cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
      addservinfo(datum, req->now, 0);
      cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
      cbdatumprintf(datum, "\r\n");
      if(req->method != HM_HEAD){
        list = nmgr_names(g_nmgr);
        for(i = 0; i < cblistnum(list); i++){
          cbdatumprintf(datum, "%s\n", cblistval(list, i, NULL));
        }
        cblistclose(list);
      }
      est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
      cbdatumclose(datum);
    } else {
      senderror(clsock, req, 404, "Not Found (the node does not exist)");
    }
  } else if(!strcmp(cmd, "inform")){
    if(!isnodeuser(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmdinform(clsock, req, node);
    }
  } else if(!strcmp(cmd, "cacheusage")){
    if(!isnodeuser(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else {
      sendnodecmdcacheusage(clsock, req, node);
    }
  } else if(!strcmp(cmd, "search")){
    if(!isnodeuser(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmdsearch(clsock, req, node);
    }
  } else if(!strcmp(cmd, "list")){
    if(!isnodeuser(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmdlist(clsock, req, node);
    }
  } else if(!strcmp(cmd, "get_doc")){
    if(!isnodeuser(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmdgetdoc(clsock, req, node);
    }
  } else if(!strcmp(cmd, "get_doc_attr")){
    if(!isnodeuser(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmdgetdocattr(clsock, req, node);
    }
  } else if(!strcmp(cmd, "etch_doc")){
    if(!isnodeuser(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmdetchdoc(clsock, req, node);
    }
  } else if(!strcmp(cmd, "uri_to_id")){
    if(!isnodeuser(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmduritoid(clsock, req, node);
    }
  } else if(!strcmp(cmd, "put_doc")){
    if(g_runmode == RM_RDONLY){
      senderror(clsock, req, 503, "Service Unavailable (read only)");
    } else if(req->referer && !cbstrfwmatch(req->referer, req->prefix)){
      senderror(clsock, req, 403, "Forbidden (invalid referrer)");
    } else if(!isnodeadmin(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmdputdoc(clsock, req, node);
    }
  } else if(!strcmp(cmd, "out_doc")){
    if(g_runmode == RM_RDONLY){
      senderror(clsock, req, 503, "Service Unavailable (read only)");
    } else if(req->referer && !cbstrfwmatch(req->referer, req->prefix)){
      senderror(clsock, req, 403, "Forbidden (invalid referrer)");
    } else if(!isnodeadmin(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmdoutdoc(clsock, req, node);
    }
  } else if(!strcmp(cmd, "edit_doc")){
    if(g_runmode == RM_RDONLY){
      senderror(clsock, req, 503, "Service Unavailable (read only)");
    } else if(req->referer && !cbstrfwmatch(req->referer, req->prefix)){
      senderror(clsock, req, 403, "Forbidden (invalid referrer)");
    } else if(!isnodeadmin(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmdeditdoc(clsock, req, node);
    }
  } else if(!strcmp(cmd, "sync")){
    if(g_runmode == RM_RDONLY){
      senderror(clsock, req, 503, "Service Unavailable (read only)");
    } else if(req->referer && !cbstrfwmatch(req->referer, req->prefix)){
      senderror(clsock, req, 403, "Forbidden (invalid referrer)");
    } else if(!isnodeadmin(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmdsync(clsock, req, node);
    }
  } else if(!strcmp(cmd, "optimize")){
    if(g_runmode == RM_RDONLY){
      senderror(clsock, req, 503, "Service Unavailable (read only)");
    } else if(req->referer && !cbstrfwmatch(req->referer, req->prefix)){
      senderror(clsock, req, 403, "Forbidden (invalid referrer)");
    } else if(!isnodeadmin(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmdoptimize(clsock, req, node);
    }
  } else if(!strcmp(cmd, "_set_user")){
    if(g_runmode == RM_RDONLY){
      senderror(clsock, req, 503, "Service Unavailable (read only)");
    } else if(req->referer && !cbstrfwmatch(req->referer, req->prefix)){
      senderror(clsock, req, 403, "Forbidden (invalid referrer)");
    } else if(!isnodeadmin(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmdsetuser(clsock, req, node);
    }
  } else if(!strcmp(cmd, "_set_link")){
    if(g_runmode == RM_RDONLY){
      senderror(clsock, req, 503, "Service Unavailable (read only)");
    } else if(req->referer && !cbstrfwmatch(req->referer, req->prefix)){
      senderror(clsock, req, 403, "Forbidden (invalid referrer)");
    } else if(!isnodeadmin(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmdsetlink(clsock, req, node);
    }
  } else if(!strcmp(cmd, SEARCHUICMD)){
    if(req->ims > 0 && node->mtime <= req->ims && !req->reload){
      sendnotmoderror(clsock, req);
    } else if(!isnodeuser(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else if(req->method == HM_HEAD){
      sendheadonly(clsock, req, UIMIMETYPE);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmdsearchui(clsock, req, node);
    }
  } else if(!strcmp(cmd, SEARCHATOMCMD)){
    if(req->ims > 0 && node->mtime <= req->ims && !req->reload){
      sendnotmoderror(clsock, req);
    } else if(!isnodeuser(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else if(req->method == HM_HEAD){
      sendheadonly(clsock, req, ATOMMIMETYPE);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmdsearchatom(clsock, req, node);
    }
  } else if(!strcmp(cmd, SEARCHRSSCMD)){
    if(req->ims > 0 && node->mtime <= req->ims && !req->reload){
      sendnotmoderror(clsock, req);
    } else if(!isnodeuser(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else if(req->method == HM_HEAD){
      sendheadonly(clsock, req, RSSMIMETYPE);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmdsearchrss(clsock, req, node);
    }
  } else if(!strcmp(cmd, OPENSEARCHCMD)){
    if(req->ims > 0 && node->mtime <= req->ims && !req->reload){
      sendnotmoderror(clsock, req);
    } else if(!isnodeuser(node, req, user)){
      sendautherror(clsock, req, node->name);
    } else if(req->method == HM_HEAD){
      sendheadonly(clsock, req, OSRCHMIMETYPE);
    } else {
      est_mtdb_interrupt(node->db);
      sendnodecmdopensearch(clsock, req, node);
    }
  } else {
    senderror(clsock, req, 400, "Bad Request (the command is invalid)");
  }
  free(pbuf);
  if(!rwlock_unlock(g_mgrlock)) log_print(LL_ERROR, "unlocking failed");
}


/* set the pseudo-attribute of the original node to a document */
static void setdocorigin(ESTDOC *doc, REQUEST *req, NODE *node, int score){
  char pbuf[URIBUFSIZ];
  sprintf(pbuf, "%s%s%s", req->prefix, NODEPREFIX, node->name);
  est_doc_add_attr(doc, DATTRNDURL, pbuf);
  est_doc_add_attr(doc, DATTRNDLABEL, node->label);
  if(score >= 0){
    sprintf(pbuf, "%d", score);
    est_doc_add_attr(doc, DATTRNDSCORE, pbuf);
  }
}


/* merge local hints to total hints */
static void mergehints(CBMAP *total, CBMAP *local){
  static pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
  const char *kbuf, *vbuf;
  char numbuf[NUMBUFSIZ];
  int ksiz, num, vsiz;
  if(pthread_mutex_lock(&mymutex) != 0) return;
  cbmapiterinit(local);
  while((kbuf = cbmapiternext(local, &ksiz)) != NULL){
    num = atoi(cbmapiterval(kbuf, NULL));
    if((vbuf = cbmapget(total, kbuf, ksiz, NULL)) != NULL) num += atoi(vbuf);
    vsiz = sprintf(numbuf, "%d", num);
    cbmapput(total, kbuf, ksiz,  numbuf, vsiz, TRUE);
  }
  pthread_mutex_unlock(&mymutex);
}


/* check whether the request is looping */
static int islooproute(const char *url, REQUEST *req){
  CBLIST *list;
  const char *rp;
  int i;
  assert(url && req);
  if(!req->estvia) return FALSE;
  list = cbsplit(req->estvia, -1, ",");
  for(i = 0; i < cblistnum(list); i++){
    rp = cblistval(list, i, NULL);
    while(*rp == ' ' || *rp == '\t'){
      rp++;
    }
    if(!strcmp(url, rp)){
      cblistclose(list);
      return TRUE;
    }
  }
  cblistclose(list);
  return FALSE;
}


/* search a local index */
static void *searchlocal(void *targ){
  REQUEST *req;
  ESTCOND *cond;
  ESTDOC *doc;
  NODE *node;
  RESMAP *resmap;
  CBMAP *hints, *myhints;
  CBLIST *words, *mywords;
  const char *phrase, *vbuf;
  int i, max, *res, rnum, hits, miss, cnt, rateuri, score, down, vsiz;
  double curtime, weight;
  req = ((TARGLCSRCH *)targ)->req;
  cond = ((TARGLCSRCH *)targ)->cond;
  node = ((TARGLCSRCH *)targ)->node;
  hints = ((TARGLCSRCH *)targ)->hints;
  max = ((TARGLCSRCH *)targ)->max;
  if(max < LCSRCHRESMIN) max = LCSRCHRESMIN;
  est_cond_set_max(cond, max);
  resmap = ((TARGLCSRCH *)targ)->resmap;
  words = ((TARGLCSRCH *)targ)->words;
  log_print(LL_DEBUG, "[%s:%d]: local search: %s",
            req->claddr, req->clport, est_mtdb_name(node->db));
  curtime = est_gettimeofday();
  myhints = cbmapopenex(MINIBNUM);
  est_cond_set_options(cond, ESTCONDSCFB);
  res = est_mtdb_search(node->db, cond, &rnum, myhints);
  hits = (vbuf = cbmapget(myhints, "", 0, NULL)) ? atoi(vbuf) : rnum;
  if(max >= 0 && hits < max + 1 && est_cond_auxiliary_word(cond, "")){
    free(res);
    est_cond_set_auxiliary(cond, -1);
    res = est_mtdb_search(node->db, cond, &rnum, myhints);
    hits = (vbuf = cbmapget(myhints, "", 0, NULL)) ? atoi(vbuf) : rnum;
  }
  miss = 0;
  mergehints(hints, myhints);
  ((TARGLCSRCH *)targ)->itime = est_gettimeofday() - curtime;
  cnt = 0;
  phrase = est_cond_phrase(cond);
  rateuri = g_rateuri && !(phrase && cbstrfwmatch(phrase, ESTOPSIMILAR));
  for(i = 0; i < rnum && cnt < max; i++){
    if(!(doc = est_mtdb_get_doc(node->db, res[i], 0))) continue;
    if(g_scancheck && phrase && phrase[0] != '[' && phrase[0] != '*' &&
       !est_mtdb_scan_doc(node->db, doc, cond)){
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
    setdocorigin(doc, req, node, score);
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
  ((TARGLCSRCH *)targ)->hnum = atoi(cbmapget(myhints, "", 0, NULL));
  ((TARGLCSRCH *)targ)->mhnum = miss;
  mywords = est_hints_to_words(myhints);
  cbmapiterinit(myhints);
  for(i = 0; i < cblistnum(mywords); i++){
    vbuf = cblistval(mywords, i, &vsiz);
    if(vsiz > 0) cblistpush(words, vbuf, vsiz);
  }
  cblistclose(mywords);
  cbmapclose(myhints);
  ((TARGLCSRCH *)targ)->etime = est_gettimeofday() - curtime;
  return NULL;
}


/* search a remote index */
static void *searchremote(void *targ){
  REQUEST *req;
  ESTCOND *cond;
  NODE *node;
  RESMAP *resmap;
  ESTNODE *tnode;
  ESTNODERES *nres;
  CBMAP *hints, *attrs, *myhints, *whints;
  CBLIST *list;
  const char *myurl, *url, *label, *vbuf, *pv;
  char numbuf[NUMBUFSIZ], *snippet;
  int i, max, credit, depth, wwidth, hwidth, awidth, len, cnt, score;
  double curtime;
  req = ((TARGRMSRCH *)targ)->req;
  myurl = ((TARGRMSRCH *)targ)->myurl;
  cond = ((TARGRMSRCH *)targ)->cond;
  node = ((TARGRMSRCH *)targ)->node;
  hints = ((TARGRMSRCH *)targ)->hints;
  max = ((TARGRMSRCH *)targ)->max;
  if(max < RMSRCHRESMIN) max = RMSRCHRESMIN;
  est_cond_set_max(cond, max);
  resmap = ((TARGRMSRCH *)targ)->resmap;
  url = ((TARGRMSRCH *)targ)->url;
  label = ((TARGRMSRCH *)targ)->label;
  credit = ((TARGRMSRCH *)targ)->credit;
  depth = ((TARGRMSRCH *)targ)->depth;
  wwidth = ((TARGRMSRCH *)targ)->wwidth;
  hwidth = ((TARGRMSRCH *)targ)->hwidth;
  awidth = ((TARGRMSRCH *)targ)->awidth;
  if(!strcmp(myurl, url) || islooproute(url, req)){
    log_print(LL_DEBUG, "[%s:%d]: omitting request loop (link): %s",
              req->claddr, req->clport, url);
    return NULL;
  }
  log_print(LL_DEBUG, "[%s:%d]: remote search: %s", req->claddr, req->clport, url);
  curtime = est_gettimeofday();
  tnode = est_node_new(url);
  if(g_proxyhost[0] != '\0') est_node_set_proxy(tnode, g_proxyhost, g_proxyport);
  est_node_set_timeout(tnode, g_searchtimeout);
  if(req->estvia) est_node_add_header(tnode, ESTHTHVIA, req->estvia);
  est_node_add_header(tnode, ESTHTHVIA, myurl);
  est_node_set_snippet_width(tnode, wwidth, hwidth, awidth);
  if((nres = est_node_search(tnode, cond, depth - 1)) != NULL){
    cnt = 0;
    while(est_noderes_shift_doc(nres, &attrs, &snippet)){
      switch(g_mergemethod){
      case MM_SCORE:
        score = (vbuf = cbmapget(attrs, DATTRNDSCORE, -1, NULL)) != NULL ? atoi(vbuf) : 0;
        score = score * (credit / (double)SELFCREDIT);
        resmap_put(resmap, score, NULL, attrs, snippet);
        break;
      case MM_SCRK:
        score = (vbuf = cbmapget(attrs, DATTRNDSCORE, -1, NULL)) != NULL ? atoi(vbuf) : 0;
        score = score * (credit / (double)SELFCREDIT) * (max * 2 - cnt);
        resmap_put(resmap, score, NULL, attrs, snippet);
        break;
      case MM_RANK:
        resmap_put(resmap, credit * (max - cnt), NULL, attrs, snippet);
        break;
      }
      cnt++;
    }
    whints = cbmapopenex(MINIBNUM);
    myhints = est_noderes_hints(nres);
    if((vbuf = cbmapget(myhints, "HIT", -1, NULL)) != NULL){
      cbmapput(whints, "", 0, vbuf, -1, FALSE);
      ((TARGRMSRCH *)targ)->hnum = atoi(vbuf);
    } else {
      cbmapput(whints, "", 0, "0", -1, FALSE);
    }
    if((vbuf = cbmapget(myhints, "DOCNUM", -1, NULL)) != NULL)
      ((TARGRMSRCH *)targ)->dnum = atoi(vbuf);
    if((vbuf = cbmapget(myhints, "WORDNUM", -1, NULL)) != NULL)
      ((TARGRMSRCH *)targ)->wnum = atoi(vbuf);
    if((vbuf = cbmapget(myhints, "LINK#0", -1, NULL)) != NULL){
      list = cbsplit(vbuf, -1, "\t");
      if(cblistnum(list) == 7)
        ((TARGRMSRCH *)targ)->size = strtod(cblistval(list, 5, NULL), NULL);
      cblistclose(list);
    }
    for(i = 0; i < cbmaprnum(myhints); i++){
      len = sprintf(numbuf, "HINT#%d", i + 1);
      if((vbuf = cbmapget(myhints, numbuf, len, NULL)) != NULL &&
         (pv = strchr(vbuf, '\t')) != NULL && pv > vbuf){
        cbmapput(whints, vbuf, pv - vbuf, pv + 1, -1, FALSE);
      } else {
        break;
      }
    }
    mergehints(hints, whints);
    cbmapclose(whints);
    ((TARGRMSRCH *)targ)->mtime = est_noderes_mdate(nres);
    est_noderes_delete(nres);
  } else {
    log_print(LL_WARN, "[%s:%d]: connection failed: %s", req->claddr, req->clport, url);
  }
  est_node_delete(tnode);
  ((TARGRMSRCH *)targ)->etime = est_gettimeofday() - curtime;
  return NULL;
}


/* set the phrase for similar search of a document */
static void setsimilarphrase(ESTCOND *cond, const char *url, int id){
  ESTNODE *tnode;
  CBMAP *kwords;
  CBDATUM *pbuf;
  const char *kbuf, *vbuf;
  int ksiz, vsiz;
  tnode = est_node_new(url);
  if(g_proxyhost[0] != '\0') est_node_set_proxy(tnode, g_proxyhost, g_proxyport);
  est_node_set_timeout(tnode, g_searchtimeout);
  if((kwords = est_node_etch_doc(tnode, id)) != NULL){
    pbuf = cbdatumopen(ESTOPSIMILAR " ", -1);
    cbdatumcat(pbuf, g_uismlrtune, -1);
    cbmapiterinit(kwords);
    while((kbuf = cbmapiternext(kwords, &ksiz)) != NULL){
      vbuf = cbmapiterval(kbuf, &vsiz);
      cbdatumcat(pbuf, " WITH ", -1);
      cbdatumcat(pbuf, vbuf, vsiz);
      cbdatumcat(pbuf, " ", 1);
      cbdatumcat(pbuf, kbuf, ksiz);
    }
    est_cond_set_phrase(cond, cbdatumptr(pbuf));
    cbdatumclose(pbuf);
    cbmapclose(kwords);
  }
  est_node_delete(tnode);
}


/* concatenate a document data to an output buffer */
static void catdocdata(CBDATUM *datum, NODE *node, RESDOC *resdoc, CBLIST *words,
                       int wwidth, int hwidth, int awidth){
  CBMAP *kwords;
  CBLIST *list;
  const CBLIST *texts;
  const char *kbuf, *vbuf;
  char *snippet;
  int i, id, ksiz, vsiz;
  if(resdoc->doc){
    list = est_doc_attr_names(resdoc->doc);
    for(i = 0; i < cblistnum(list); i++){
      vbuf = cblistval(list, i, NULL);
      cbdatumprintf(datum, "%s=%s\n", vbuf, est_doc_attr(resdoc->doc, vbuf));
    }
    cblistclose(list);
    if(g_smlrvnum > 0){
      cbdatumprintf(datum, "%s", ESTDCNTLVECTOR);
      id = est_doc_id(resdoc->doc);
      kwords = id > 0 ? est_mtdb_get_keywords(node->db, id) : NULL;
      if(!kwords) kwords = est_mtdb_etch_doc(node->db, resdoc->doc, g_smlrvnum);
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
  } else if(resdoc->attrs){
    list = cbmapkeys(resdoc->attrs);
    cblistsort(list);
    for(i = 0; i < cblistnum(list); i++){
      vbuf = cblistval(list, i, NULL);
      if(vbuf[0] == '%') continue;
      cbdatumprintf(datum, "%s=%s\n", vbuf, cbmapget(resdoc->attrs, vbuf, -1, NULL));
    }
    cblistclose(list);
    if((vbuf = cbmapget(resdoc->attrs, ESTDCNTLVECTOR, -1, NULL)) != NULL)
      cbdatumprintf(datum, "%s\t%s\n", ESTDCNTLVECTOR, vbuf);
  }
  cbdatumcat(datum, "\n", 1);
  if(resdoc->body){
    cbdatumcat(datum, resdoc->body, -1);
  } else if(resdoc->doc){
    if(wwidth < 0){
      texts = est_doc_texts(resdoc->doc);
      for(i = 0; i < cblistnum(texts); i++){
        vbuf = cblistval(texts, i, &vsiz);
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
}


/* concatenate a document data to an output buffer for search user interface */
static void catdocdataui(CBDATUM *datum, int num, RESDOC *resdoc, CBLIST *words,
                         REQUEST *req, NODE *node, const char *condstr, const char *simcondstr){
  CBLIST *lines;
  const char *uri, *file, *title, *type, *ndurl, *ndlabel, *vbuf;
  char *turi, buf[URIBUFSIZ], *pv, *snippet, *word;
  int i, id, vsiz, ld;
  cbdatumprintf(datum, "<dl class=\"doc\" id=\"doc%d\">\n", num);
  if(!(uri = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRURI) :
       cbmapget(resdoc->attrs, ESTDATTRURI, -1, NULL))) uri = ".";
  turi = makeshownuri(uri);
  if((file = strrchr(turi, '/')) != NULL) file++;
  title = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRTITLE) :
    cbmapget(resdoc->attrs, ESTDATTRTITLE, -1, NULL);
  if(!title) title = resdoc->doc ? est_doc_attr(resdoc->doc, DATTRLFILE) :
    cbmapget(resdoc->attrs, DATTRLFILE, -1, NULL);
  if(!title) title = file;
  if(!title || title[0] == '\0') title = "(no title)";
  type = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRTYPE) :
    cbmapget(resdoc->attrs, ESTDATTRTYPE, -1, NULL);
  if(!type || type[0] == '\0') type = DEFMIMETYPE;
  ndurl = resdoc->doc ? est_doc_attr(resdoc->doc, DATTRNDURL) :
    cbmapget(resdoc->attrs, DATTRNDURL, -1, NULL);
  if(!ndurl) ndurl = ".";
  ndlabel = resdoc->doc ? est_doc_attr(resdoc->doc, DATTRNDLABEL) :
    cbmapget(resdoc->attrs, DATTRNDLABEL, -1, NULL);
  vbuf = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRID) :
    cbmapget(resdoc->attrs, ESTDATTRID, -1, NULL);
  id = vbuf ? atoi(vbuf) : -1;
  if(!ndlabel) ndlabel = "(unknown)";
  cbdatumprintf(datum, "<dt class=\"title\" id=\"doc%d_title\">", num);
  cbdatumprintf(datum, "<a href=\"%@\" type=\"%@\" class=\"title\""
                " id=\"doc%d_title_v\" name=\"doc%d_title_v\""
                " title=\"Get this document\">%@</a>", turi, type, num, num, title);
  cbdatumprintf(datum, " <a href=\"%@/%@?%s\" type=\"%@\" class=\"ndlabel\""
                " id=\"doc%d_node\" name=\"doc%d_node\" title=\"Jump to the node\">(%@)</a>",
                ndurl, SEARCHUICMD, condstr, UIMIMETYPE, num, num, ndlabel);
  cbdatumprintf(datum, "</dt>\n");
  for(i = 0; i < cblistnum(g_uiextattrs); i++){
    sprintf(buf, "%s", cblistval(g_uiextattrs, i, NULL));
    if(!(pv = strchr(buf, '|'))) continue;
    *(pv++) = '\0';
    vbuf = resdoc->doc ? est_doc_attr(resdoc->doc, buf) : cbmapget(resdoc->attrs, buf, -1, NULL);
    if(!vbuf) continue;
    cbdatumprintf(datum, "<dd class=\"attr\" id=\"doc%d_attr%d\">", num, i + 1);
    cbdatumprintf(datum, "<span class=\"label\" id=\"doc%d_attr%d_l\">%@</span>: ",
                  num, i + 1, pv);
    cbdatumprintf(datum, "<span class=\"value\" id=\"doc%d_attr%d_v\">%@</span>",
                  num, i + 1, vbuf);
    cbdatumprintf(datum, "</dd>\n");
  }
  if(g_snipwwidth > 0){
    cbdatumprintf(datum, "<dd class=\"snippet\" id=\"doc%d_snippet\">", num);
    snippet = resdoc->doc ? est_doc_make_snippet(resdoc->doc, words, g_snipwwidth,
                                                 g_sniphwidth, g_snipawidth) : NULL;
    lines = cbsplit(snippet ? snippet: resdoc->body, -1, "\n");
    ld = TRUE;
    for(i = 0; i < cblistnum(lines); i++){
      vbuf = cblistval(lines, i, &vsiz);
      word = cbmemdup(vbuf, vsiz);
      if(vsiz < 1){
        if(!ld) cbdatumprintf(datum, " <code>...</code> ");
        ld = TRUE;
      } else if((pv = strchr(word, '\t')) != NULL){
        *pv = '\0';
        cbdatumprintf(datum, "<strong>%@</strong>", word);
        ld = FALSE;
      } else {
        cbdatumprintf(datum, "%@", word);
        ld = FALSE;
      }
      free(word);
    }
    cblistclose(lines);
    free(snippet);
    cbdatumprintf(datum, "</dd>\n");
  }
  cbdatumprintf(datum, "<dd class=\"guide\" id=\"doc%d_guide\">", num);
  cbdatumprintf(datum, "<span class=\"uri\" id=\"doc%d_uri\">%@</span>", num, turi);
  if(g_smlrvnum > 0 && id > 0)
    cbdatumprintf(datum, " - <a href=\"%@%@%@/%@?%s&amp;simid=%d&amp;simnode=%?\""
                  " class=\"navi\" title=\"Perform similarity search\">[similar]</a>",
                  req->prefix, NODEPREFIX, node->name, SEARCHUICMD, simcondstr, id, ndurl);
  cbdatumprintf(datum, "</dd>\n");
  free(turi);
  cbdatumprintf(datum, "</dl>\n");
}


/* concatenate a document data to an output buffer for Atom feed interface */
static void catdocdataatom(CBDATUM *datum, RESDOC *resdoc, CBLIST *words, NODE *node){
  CBLIST *lines;
  const char *uri, *file, *title, *author, *type, *vbuf;
  char *turi, *dstr, *pv, *snippet, *word;
  int i, vsiz, ld;
  time_t mtime, ctime;
  if(!(uri = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRURI) :
       cbmapget(resdoc->attrs, ESTDATTRURI, -1, NULL))) uri = ".";
  turi = makeshownuri(uri);
  if((file = strrchr(turi, '/')) != NULL) file++;
  title = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRTITLE) :
    cbmapget(resdoc->attrs, ESTDATTRTITLE, -1, NULL);
  if(!title) title = resdoc->doc ? est_doc_attr(resdoc->doc, DATTRLFILE) :
    cbmapget(resdoc->attrs, DATTRLFILE, -1, NULL);
  if(!title) title = file;
  if(!title || title[0] == '\0') title = "(no title)";
  author = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRAUTHOR) :
    cbmapget(resdoc->attrs, ESTDATTRAUTHOR, -1, NULL);
  if(!author || author[0] == '\0') author = "(anonymous)";
  type = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRTYPE) :
    cbmapget(resdoc->attrs, ESTDATTRTYPE, -1, NULL);
  if(!type || type[0] == '\0') type = DEFMIMETYPE;
  vbuf = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRMDATE) :
    cbmapget(resdoc->attrs, ESTDATTRMDATE, -1, NULL);
  mtime = vbuf && vbuf[0] != '\0' ? cbstrmktime(vbuf) : node->mtime;
  vbuf = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRCDATE) :
    cbmapget(resdoc->attrs, ESTDATTRCDATE, -1, NULL);
  ctime = vbuf && vbuf[0] != '\0' ? cbstrmktime(vbuf) : -1;
  cbdatumprintf(datum, "<entry>\n");
  cbdatumprintf(datum, "<id>%@</id>\n", turi);
  cbdatumprintf(datum, "<title>%@</title>\n", title);
  cbdatumprintf(datum, "<author><name>%@</name></author>\n", author);
  dstr = cbdatestrwww(mtime, 0);
  cbdatumprintf(datum, "<updated>%@</updated>\n", dstr);
  free(dstr);
  if(ctime > 0){
    dstr = cbdatestrwww(ctime, 0);
    cbdatumprintf(datum, "<published>%@</published>\n", dstr);
    free(dstr);
  }
  vbuf = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRGENRE) :
    cbmapget(resdoc->attrs, ESTDATTRGENRE, -1, NULL);
  if(vbuf && vbuf[0] != '\0') cbdatumprintf(datum, "<category term=\"%@\"/>\n", vbuf);
  cbdatumprintf(datum, "<link rel=\"alternate\" type=\"%@\" href=\"%@\"/>\n", type, turi);
  if(g_snipwwidth > 0){
    cbdatumprintf(datum, "<summary type=\"xhtml\">");
    cbdatumprintf(datum, "<xh:div>");
    snippet = resdoc->doc ? est_doc_make_snippet(resdoc->doc, words, g_snipwwidth,
                                                 g_sniphwidth, g_snipawidth) : NULL;
    lines = cbsplit(snippet ? snippet: resdoc->body, -1, "\n");
    ld = TRUE;
    for(i = 0; i < cblistnum(lines); i++){
      vbuf = cblistval(lines, i, &vsiz);
      word = cbmemdup(vbuf, vsiz);
      if(vsiz < 1){
        if(!ld) cbdatumprintf(datum, " <xh:code>...</xh:code> ");
        ld = TRUE;
      } else if((pv = strchr(word, '\t')) != NULL){
        *pv = '\0';
        cbdatumprintf(datum, "<xh:strong>%@</xh:strong>", word);
        ld = FALSE;
      } else {
        cbdatumprintf(datum, "%@", word);
        ld = FALSE;
      }
      free(word);
    }
    cblistclose(lines);
    free(snippet);
    cbdatumprintf(datum, "</xh:div>");
    cbdatumprintf(datum, "</summary>\n");
  }
  vbuf = resdoc->doc ? est_doc_attr(resdoc->doc, DATTRNDURL) :
    cbmapget(resdoc->attrs, DATTRNDURL, -1, NULL);
  if(vbuf && vbuf[0] != '\0')
    cbdatumprintf(datum, "<est:nodeurl>%@</est:nodeurl>\n", vbuf);
  vbuf = resdoc->doc ? est_doc_attr(resdoc->doc, DATTRNDLABEL) :
    cbmapget(resdoc->attrs, DATTRNDLABEL, -1, NULL);
  if(vbuf && vbuf[0] != '\0')
    cbdatumprintf(datum, "<est:nodelabel>%@</est:nodelabel>\n", vbuf);
  vbuf = resdoc->doc ? est_doc_attr(resdoc->doc, DATTRNDSCORE) :
    cbmapget(resdoc->attrs, DATTRNDSCORE, -1, NULL);
  if(vbuf && vbuf[0] != '\0')
    cbdatumprintf(datum, "<est:nodescore>%@</est:nodescore>\n", vbuf);
  vbuf = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRID) :
    cbmapget(resdoc->attrs, ESTDATTRID, -1, NULL);
  if(vbuf && vbuf[0] != '\0')
    cbdatumprintf(datum, "<est:id>%@</est:id>\n", vbuf);
  cbdatumprintf(datum, "</entry>\n");
  free(turi);
}


/* concatenate a document data to an output buffer for RSS feed interface */
static void catdocdatarss(CBDATUM *datum, RESDOC *resdoc, CBLIST *words, NODE *node){
  CBLIST *lines;
  const char *uri, *file, *title, *author, *type, *vbuf;
  char *turi, *dstr, *pv, *snippet, *word;
  int i, vsiz, ld;
  time_t mtime;
  if(!(uri = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRURI) :
       cbmapget(resdoc->attrs, ESTDATTRURI, -1, NULL))) uri = ".";
  turi = makeshownuri(uri);
  if((file = strrchr(turi, '/')) != NULL) file++;
  title = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRTITLE) :
    cbmapget(resdoc->attrs, ESTDATTRTITLE, -1, NULL);
  if(!title) title = resdoc->doc ? est_doc_attr(resdoc->doc, DATTRLFILE) :
    cbmapget(resdoc->attrs, DATTRLFILE, -1, NULL);
  if(!title) title = file;
  if(!title || title[0] == '\0') title = "(no title)";
  author = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRAUTHOR) :
    cbmapget(resdoc->attrs, ESTDATTRAUTHOR, -1, NULL);
  if(!author || author[0] == '\0') author = "(anonymous)";
  type = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRTYPE) :
    cbmapget(resdoc->attrs, ESTDATTRTYPE, -1, NULL);
  if(!type || type[0] == '\0') type = DEFMIMETYPE;
  vbuf = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRMDATE) :
    cbmapget(resdoc->attrs, ESTDATTRMDATE, -1, NULL);
  mtime = vbuf && vbuf[0] != '\0' ? cbstrmktime(vbuf) : node->mtime;
  cbdatumprintf(datum, "<item rdf:about=\"%@\">\n", turi);
  cbdatumprintf(datum, "<title>%@</title>\n", title);
  cbdatumprintf(datum, "<link>%@</link>\n", turi);
  cbdatumprintf(datum, "<dc:title>%@</dc:title>\n", title);
  vbuf = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRGENRE) :
    cbmapget(resdoc->attrs, ESTDATTRGENRE, -1, NULL);
  if(vbuf && vbuf[0] != '\0') cbdatumprintf(datum, "<dc:subject>%@</dc:subject>\n", vbuf);
  cbdatumprintf(datum, "<dc:creator>%@</dc:creator>\n", author);
  dstr = cbdatestrwww(mtime, 0);
  cbdatumprintf(datum, "<dc:date>%@</dc:date>\n", dstr);
  free(dstr);
  cbdatumprintf(datum, "<dc:format>%@</dc:format>\n", type);
  vbuf = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRLANG) :
    cbmapget(resdoc->attrs, ESTDATTRLANG, -1, NULL);
  if(vbuf && vbuf[0] != '\0') cbdatumprintf(datum, "<dc:language>%@</dc:language>\n", vbuf);
  if(g_snipwwidth > 0){
    cbdatumprintf(datum, "<description>");
    snippet = resdoc->doc ? est_doc_make_snippet(resdoc->doc, words, g_snipwwidth,
                                                 g_sniphwidth, g_snipawidth) : NULL;
    lines = cbsplit(snippet ? snippet: resdoc->body, -1, "\n");
    ld = TRUE;
    for(i = 0; i < cblistnum(lines); i++){
      vbuf = cblistval(lines, i, &vsiz);
      word = cbmemdup(vbuf, vsiz);
      if(vsiz < 1){
        if(!ld) cbdatumprintf(datum, " ... ");
        ld = TRUE;
      } else if((pv = strchr(word, '\t')) != NULL){
        *pv = '\0';
        cbdatumprintf(datum, "%@", word);
        ld = FALSE;
      } else {
        cbdatumprintf(datum, "%@", word);
        ld = FALSE;
      }
      free(word);
    }
    cblistclose(lines);
    free(snippet);
    cbdatumprintf(datum, "</description>\n");
  }
  vbuf = resdoc->doc ? est_doc_attr(resdoc->doc, DATTRNDURL) :
    cbmapget(resdoc->attrs, DATTRNDURL, -1, NULL);
  if(vbuf && vbuf[0] != '\0')
    cbdatumprintf(datum, "<est:nodeurl>%@</est:nodeurl>\n", vbuf);
  vbuf = resdoc->doc ? est_doc_attr(resdoc->doc, DATTRNDLABEL) :
    cbmapget(resdoc->attrs, DATTRNDLABEL, -1, NULL);
  if(vbuf && vbuf[0] != '\0')
    cbdatumprintf(datum, "<est:nodelabel>%@</est:nodelabel>\n", vbuf);
  vbuf = resdoc->doc ? est_doc_attr(resdoc->doc, DATTRNDSCORE) :
    cbmapget(resdoc->attrs, DATTRNDSCORE, -1, NULL);
  if(vbuf && vbuf[0] != '\0')
    cbdatumprintf(datum, "<est:nodescore>%@</est:nodescore>\n", vbuf);
  vbuf = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRID) :
    cbmapget(resdoc->attrs, ESTDATTRID, -1, NULL);
  if(vbuf && vbuf[0] != '\0')
    cbdatumprintf(datum, "<est:id>%@</est:id>\n", vbuf);
  cbdatumprintf(datum, "</item>\n");
  free(turi);
}


/* make a URI to be shown */
static char *makeshownuri(const char *uri){
  char *turi, *bef, *aft, *pv, *nuri;
  int i;
  turi = cbmemdup(uri, -1);
  for(i = 0; i < cblistnum(g_uireplaces); i++){
    bef = cbmemdup(cblistval(g_uireplaces, i, NULL), -1);
    if((pv = strstr(bef, "{{!}}")) != NULL){
      *pv = '\0';
      aft = pv + 5;
    } else {
      aft = "";
    }
    nuri = est_regex_replace(turi, bef, aft);
    free(turi);
    turi = nuri;
    free(bef);
  }
  return turi;
}


/* send the result of the search command */
static void sendnodecmdinform(int clsock, REQUEST *req, NODE *node){
  CBDATUM *datum;
  const char *kbuf;
  if(est_mtdb_cache_usage(node->db) >= g_helpershift &&
     sendnodecmdinformhelper(clsock, req, node)) return;
  if(pthread_mutex_lock(&(node->mutex)) != 0){
    log_print(LL_ERROR, "locking failed");
    senderror(clsock, req, 500, "Internal Server Error (locking failed)");
    return;
  }
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
  addservinfo(datum, req->now, 0);
  cbdatumprintf(datum, "Content-Type: %s; charset=UTF-8\r\n", ESTINFORMTYPE);
  cbdatumprintf(datum, "\r\n");
  cbdatumprintf(datum, "%s\t%s\t%d\t%d\t%.0f\n", node->name, node->label,
                est_mtdb_doc_num(node->db), est_mtdb_word_num(node->db),
                est_mtdb_size(node->db));
  cbdatumprintf(datum, "\n");
  cbmapiterinit(node->admins);
  while((kbuf = cbmapiternext(node->admins, NULL)) != NULL){
    if(kbuf[0] != '\0') cbdatumprintf(datum, "%s\n", kbuf);
  }
  cbdatumprintf(datum, "\n");
  cbmapiterinit(node->users);
  while((kbuf = cbmapiternext(node->users, NULL)) != NULL){
    if(kbuf[0] != '\0') cbdatumprintf(datum, "%s\n", kbuf);
  }
  cbdatumprintf(datum, "\n");
  cbmapiterinit(node->links);
  while((kbuf = cbmapiternext(node->links, NULL)) != NULL){
    if(kbuf[0] != '\0') cbdatumprintf(datum, "%s\t%s\n", kbuf, cbmapiterval(kbuf, NULL));
  }
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: 200 OK (inform)", req->claddr, req->clport);
  if(pthread_mutex_unlock(&(node->mutex)) != 0) log_print(LL_ERROR, "unlocking failed");
}


/* send the result of the inform helper command */
static int sendnodecmdinformhelper(int clsock, REQUEST *req, NODE *node){
  CBDATUM *datum;
  char *dbuf, *ebuf, dbpath[URIBUFSIZ], opath[URIBUFSIZ];
  const char *kbuf;
  int i, dsiz, err, dnum, wnum;
  double fsiz;
  if(pthread_mutex_lock(&(node->mutex)) != 0){
    log_print(LL_ERROR, "locking failed");
    return FALSE;
  }
  datum = cbdatumopen(NULL, -1);
  sprintf(dbpath, "%s%c%s%c%s", g_rootdir, ESTPATHCHR, NODEDIR, ESTPATHCHR, node->name);
  cbdatumprintf(datum, "%s \"%s\"", INFORMHELPER, dbpath);
  sprintf(opath, "%s%chelper-%d", dbpath, ESTPATHCHR, g_accesscount);
  cbdatumprintf(datum, " \"%s\"", opath);
  log_print(LL_DEBUG, "[%s:%d]: calling the inform helper: %s",
            req->claddr, req->clport, cbdatumptr(datum));
  for(i = 0; i < HELPERTRYNUM; i++){
    system(cbdatumptr(datum));
    if((dbuf = cbreadfile(opath, &dsiz)) != NULL) break;
    est_usleep(1000 * 100);
  }
  cbdatumclose(datum);
  err = FALSE;
  if(dbuf){
    datum = cbdatumopen(NULL, -1);
    cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
    addservinfo(datum, req->now, 0);
    cbdatumprintf(datum, "Content-Type: %s; charset=UTF-8\r\n", ESTINFORMTYPE);
    ebuf = cbdatestrhttp(node->mtime, 0);
    cbdatumprintf(datum, "Last-Modified: %s\r\n", ebuf);
    free(ebuf);
    cbdatumprintf(datum, "\r\n");
    kbuf = dbuf;
    dnum = atoi(kbuf);
    wnum = 0;
    fsiz = 0.0;
    if((kbuf = strchr(kbuf, '\t')) != NULL){
      kbuf++;
      wnum = atoi(kbuf);
      if((kbuf = strchr(kbuf, '\t')) != NULL){
        kbuf++;
        fsiz = strtod(kbuf, NULL);
      }
    }
    cbdatumprintf(datum, "%s\t%s\t%d\t%d\t%.0f\n", node->name, node->label, dnum, wnum, fsiz);
    cbdatumprintf(datum, "\n");
    cbmapiterinit(node->admins);
    while((kbuf = cbmapiternext(node->admins, NULL)) != NULL){
      if(kbuf[0] != '\0') cbdatumprintf(datum, "%s\n", kbuf);
    }
    cbdatumprintf(datum, "\n");
    cbmapiterinit(node->users);
    while((kbuf = cbmapiternext(node->users, NULL)) != NULL){
      if(kbuf[0] != '\0') cbdatumprintf(datum, "%s\n", kbuf);
    }
    cbdatumprintf(datum, "\n");
    cbmapiterinit(node->links);
    while((kbuf = cbmapiternext(node->links, NULL)) != NULL){
      if(kbuf[0] != '\0') cbdatumprintf(datum, "%s\t%s\n", kbuf, cbmapiterval(kbuf, NULL));
    }
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    cbdatumclose(datum);
    free(dbuf);
    log_print(LL_DEBUG, "[%s:%d]: 200 OK (inform-helper)", req->claddr, req->clport);
  } else {
    log_print(LL_WARN, "[%s:%d]: the inform helper failed", req->claddr, req->clport);
    err = TRUE;
  }
  unlink(opath);
  if(pthread_mutex_unlock(&(node->mutex)) != 0) log_print(LL_ERROR, "unlocking failed");
  return err ? FALSE : TRUE;
}


/* send the result of the cacheusage command */
static void sendnodecmdcacheusage(int clsock, REQUEST *req, NODE *node){
  CBDATUM *datum;
  if(pthread_mutex_lock(&(node->mutex)) != 0){
    log_print(LL_ERROR, "locking failed");
    senderror(clsock, req, 500, "Internal Server Error (locking failed)");
    return;
  }
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
  addservinfo(datum, req->now, 0);
  cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
  cbdatumprintf(datum, "\r\n");
  cbdatumprintf(datum, "%.6f\n", est_mtdb_cache_usage(node->db));
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: 200 OK (cacheusage)", req->claddr, req->clport);
  if(pthread_mutex_unlock(&(node->mutex)) != 0) log_print(LL_ERROR, "unlocking failed");
}


/* send the result of the search command */
static void sendnodecmdsearch(int clsock, REQUEST *req, NODE *node){
  pthread_t lth, *rths;
  TARGLCSRCH ltarg;
  TARGRMSRCH *rtargs;
  RESMAP *resmap;
  RESDOC **resdocs;
  ESTCOND *cond;
  CBMAP *hints;
  CBDATUM *datum;
  const char *tmp, *url, *label, *kbuf, *phrase, *order, *distinct;
  char *myurl, *dstr, name[NUMBUFSIZ], masks[LINKMASKMAX+1], *zbuf;
  int i, num, max, depth, wwidth, hwidth, awidth, skip, dnum, wnum, hnum, end, ksiz, rnum, zsiz;
  double curtime;
  time_t mtime;
  myurl = cbsprintf("%s%s%s", req->prefix, NODEPREFIX, node->name);
  if(islooproute(myurl, req)){
    log_print(LL_DEBUG, "[%s:%d]: omitting request loop (self): %s",
              req->claddr, req->clport, myurl);
    free(myurl);
    senderror(clsock, req, 400, "Bad Request (the request loops)");
    return;
  }
  if(est_mtdb_cache_usage(node->db) >= g_helpershift &&
     (!(tmp = cbmapget(req->params, "depth", -1, NULL)) || atoi(tmp) < 1 ||
      cbmaprnum(node->links) < 1) &&
     sendnodecmdsearchhelper(clsock, req, node, myurl)){
    free(myurl);
    return;
  }
  if(pthread_mutex_lock(&(node->mutex)) != 0){
    log_print(LL_ERROR, "locking failed");
    senderror(clsock, req, 500, "Internal Server Error (locking failed)");
    return;
  }
  cond = est_cond_new();
  max = DEFMAXSRCH;
  depth = 0;
  wwidth = g_snipwwidth;
  hwidth = g_sniphwidth;
  awidth = g_snipawidth;
  skip = 0;
  if((tmp = cbmapget(req->params, "skip", -1, NULL)) != NULL && (num = atoi(tmp)) > 0)
    skip = num;
  if((tmp = cbmapget(req->params, "phrase", -1, NULL)) != NULL && tmp[0] != '\0')
    est_cond_set_phrase(cond, tmp);
  if((tmp = cbmapget(req->params, "attr", -1, NULL)) != NULL && tmp[0] != '\0')
    est_cond_add_attr(cond, tmp);
  for(i = 0; i <= CONDATTRMAX; i++){
    num = sprintf(name, "attr%d", i);
    if((tmp = cbmapget(req->params, name, num, NULL)) != NULL && tmp[0] != '\0')
      est_cond_add_attr(cond, tmp);
  }
  if((tmp = cbmapget(req->params, "order", -1, NULL)) != NULL && tmp[0] != '\0')
    est_cond_set_order(cond, tmp);
  if((tmp = cbmapget(req->params, "max", -1, NULL)) != NULL && (num = atoi(tmp)) >= 0)
    max = num;
  max = max > g_searchmax ? g_searchmax : max;
  if((tmp = cbmapget(req->params, "options", -1, NULL)) != NULL && (num = atoi(tmp)) > 0)
    est_cond_set_options(cond, num);
  if((tmp = cbmapget(req->params, "auxiliary", -1, NULL)) != NULL)
    est_cond_set_auxiliary(cond, atoi(tmp));
  if((tmp = cbmapget(req->params, "distinct", -1, NULL)) != NULL && tmp[0] != '\0')
    est_cond_set_distinct(cond, tmp);
  if((tmp = cbmapget(req->params, "depth", -1, NULL)) != NULL && (num = atoi(tmp)) > 0)
    depth = num;
  if(depth >= g_searchdepth) depth = g_searchdepth;
  if((tmp = cbmapget(req->params, "wwidth", -1, NULL)) != NULL) wwidth = atoi(tmp);
  if((tmp = cbmapget(req->params, "hwidth", -1, NULL)) != NULL && (num = atoi(tmp)) >= 0)
    hwidth = num;
  if((tmp = cbmapget(req->params, "awidth", -1, NULL)) != NULL && (num = atoi(tmp)) >= 0)
    awidth = num;
  if((tmp = cbmapget(req->params, "mask", -1, NULL)) != NULL && tmp[0] != '\0'){
    num = atoi(tmp);
    for(i = 0; i <= LINKMASKMAX; i++){
      masks[i] = num & (1 << i);
    }
  } else if((tmp = cbmapget(req->params, "allmask", -1, NULL)) != NULL && tmp[0] != '\0'){
    memset(masks, 1, LINKMASKMAX+1);
    for(i = 0; i <= LINKMASKMAX; i++){
      num = sprintf(name, "nomask%d", i);
      if((tmp = cbmapget(req->params, name, num, NULL)) != NULL && tmp[0] != '\0') masks[i] = 0;
    }
  } else {
    memset(masks, 0, LINKMASKMAX+1);
    if(!(tmp = cbmapget(req->params, "nomask", -1, NULL)) || tmp[0] == '\0'){
      for(i = 0; i <= LINKMASKMAX; i++){
        num = sprintf(name, "mask%d", i);
        if((tmp = cbmapget(req->params, name, num, NULL)) != NULL && tmp[0] != '\0') masks[i] = 1;
      }
    }
  }
  resmap = resmap_new();
  hints = cbmapopenex(MINIBNUM);
  curtime = est_gettimeofday();
  ltarg.req = req;
  ltarg.alive = TRUE;
  ltarg.cond = est_cond_dup(cond);
  ltarg.hints = hints;
  ltarg.max = max + skip + 1;
  ltarg.node = node;
  ltarg.resmap = resmap;
  ltarg.words = cblistopen();
  ltarg.hnum = 0;
  ltarg.mhnum = 0;
  ltarg.itime = 0.0;
  ltarg.etime = 0.0;
  if(masks[0]){
    ltarg.alive = FALSE;
  } else if(g_stmode){
    searchlocal(&ltarg);
    ltarg.alive = FALSE;
  } else if(pthread_create(&lth, NULL, searchlocal, &ltarg) != 0){
    log_print(LL_WARN, "creating thread failed");
    ltarg.alive = FALSE;
  }
  rths = cbmalloc(cbmaprnum(node->links) * sizeof(pthread_t) + 1);
  rtargs = cbmalloc(cbmaprnum(node->links) * sizeof(TARGRMSRCH) + 1);
  cbmapiterinit(node->links);
  for(i = 0; (url = cbmapiternext(node->links, NULL)) != NULL; i++){
    rtargs[i].req = req;
    rtargs[i].alive = TRUE;
    rtargs[i].myurl = myurl;
    rtargs[i].cond = est_cond_dup(cond);
    rtargs[i].hints = hints;
    rtargs[i].max = max + skip + 1;
    rtargs[i].node = node;
    rtargs[i].resmap = resmap;
    rtargs[i].url = url;
    label = cbmapiterval(url, NULL);
    if((tmp = strchr(label, '\t')) != NULL){
      rtargs[i].label = cbmemdup(label, tmp - label);
      rtargs[i].credit = atoi(tmp + 1);
    } else {
      rtargs[i].label = cbmemdup(label, -1);
      rtargs[i].credit = 0;
    }
    rtargs[i].depth = depth;
    rtargs[i].wwidth = wwidth;
    rtargs[i].hwidth = hwidth;
    rtargs[i].awidth = awidth;
    rtargs[i].hnum = 0;
    rtargs[i].dnum = 0;
    rtargs[i].wnum = 0;
    rtargs[i].etime = 0.0;
    rtargs[i].size = 0.0;
    rtargs[i].mtime = 0;
    if(depth < 1 || (i < LINKMASKMAX && masks[i+1])){
      rtargs[i].alive = FALSE;
      rtargs[i].hnum = -1;
      rtargs[i].dnum = -1;
      rtargs[i].wnum = -1;
      rtargs[i].etime = -1.0;
      rtargs[i].size = -1.0;
    } else if(g_stmode){
      searchremote(&rtargs[i]);
      rtargs[i].alive = FALSE;
    } else if(pthread_create(rths + i, NULL, searchremote, rtargs + i) != 0){
      log_print(LL_WARN, "creating thread failed");
      rtargs[i].alive = FALSE;
    }
  }
  if(pthread_mutex_unlock(&(node->mutex)) != 0) log_print(LL_ERROR, "unlocking failed");
  dnum = 0;
  wnum = 0;
  mtime = node->mtime;
  if(ltarg.alive){
    if(pthread_join(lth, NULL) == 0){
      dnum = est_mtdb_doc_num(node->db);
      wnum = est_mtdb_word_num(node->db);
    } else {
      log_print(LL_ERROR, "joining thread failed");
    }
  } else if(g_stmode){
    dnum = est_mtdb_doc_num(node->db);
    wnum = est_mtdb_word_num(node->db);
  }
  for(i = 0; i < cbmaprnum(node->links); i++){
    if(rtargs[i].alive){
      if(pthread_join(rths[i], NULL) == 0){
        dnum += rtargs[i].dnum;
        wnum += rtargs[i].wnum;
        if(rtargs[i].mtime > mtime) mtime = rtargs[i].mtime;
      } else {
        log_print(LL_ERROR, "joining thread failed");
      }
    } else if(g_stmode && depth > 0){
      dnum += rtargs[i].dnum;
      wnum += rtargs[i].wnum;
      if(rtargs[i].mtime > mtime) mtime = rtargs[i].mtime;
    }
  }
  hnum = (tmp = cbmapget(hints, "", 0, NULL)) ? atoi(tmp) : 0;
  hnum -= ltarg.mhnum;
  if(hnum < 0) hnum = 0;
  end = max + skip;
  curtime = est_gettimeofday() - curtime;
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
  addservinfo(datum, req->now, 0);
  cbdatumprintf(datum, "Content-Type: %s; charset=UTF-8\r\n", ESTSEARCHTYPE);
  if(req->gzip){
    cbdatumprintf(datum, "Content-Encoding: gzip\r\n");
  } else if(req->deflate){
    cbdatumprintf(datum, "Content-Encoding: deflate\r\n");
  }
  dstr = cbdatestrhttp(mtime, 0);
  cbdatumprintf(datum, "Last-Modified: %s\r\n", dstr);
  free(dstr);
  cbdatumprintf(datum, "\r\n");
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  cbdatumsetsize(datum, 0);
  cbdatumprintf(datum, "%s\n", g_bordstr);
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
  if(ltarg.itime >= 0.0) cbdatumprintf(datum, "TIME#i\t%.6f\n", ltarg.itime / 1000.0);
  if(ltarg.etime >= 0.0) cbdatumprintf(datum, "TIME#0\t%.6f\n", ltarg.etime / 1000.0);
  for(i = 0; i < cbmaprnum(node->links); i++){
    if(rtargs[i].etime >= 0.0)
      cbdatumprintf(datum, "TIME#%d\t%.6f\n", i + 1, rtargs[i].etime / 1000.0);
  }
  cbdatumprintf(datum, "LINK#0\t%s\t", myurl);
  cbdatumprintf(datum, "%s\t%d\t%d\t%d\t%.0f\t%d\n",
                node->label, SELFCREDIT, est_mtdb_doc_num(node->db),
                est_mtdb_word_num(node->db), est_mtdb_size(node->db),
                ltarg.hnum - ltarg.mhnum);
  for(i = 0; i < cbmaprnum(node->links); i++){
    cbdatumprintf(datum, "LINK#%d\t%s\t%s\t%d\t%d\t%d\t%.0f\t%d\n",
                  i + 1, rtargs[i].url, rtargs[i].label, rtargs[i].credit,
                  rtargs[i].dnum, rtargs[i].wnum, rtargs[i].size, rtargs[i].hnum);
  }
  cbdatumprintf(datum, "VIEW\tSNIPPET\n");
  cbdatumprintf(datum, "\n");
  phrase = est_cond_phrase(cond);
  order = est_cond_order(cond);
  if(!order && phrase && cbstrfwmatch(phrase, ESTOPSIMILAR))
    order = DATTRNDSCORE " " ESTORDNUMD;
  distinct = depth > 0 && cbmaprnum(node->links) > 0 ? est_cond_distinct(cond) : NULL;
  resdocs = resmap_list(resmap, &rnum, order, distinct);
  for(i = skip; i < rnum && i < end; i++){
    cbdatumprintf(datum, "%s\n", g_bordstr);
    catdocdata(datum, node, resdocs[i], ltarg.words, wwidth, hwidth, awidth);
  }
  free(resdocs);
  cbdatumprintf(datum, "%s:END\n", g_bordstr);
  if(req->gzip){
    if((zbuf = est_deflate(cbdatumptr(datum), cbdatumsize(datum), &zsiz, 1)) != NULL){
      est_sock_send_all(clsock, zbuf, zsiz);
      free(zbuf);
    }
  } else if(req->deflate){
    if((zbuf = est_deflate(cbdatumptr(datum), cbdatumsize(datum), &zsiz, 0)) != NULL){
      est_sock_send_all(clsock, zbuf, zsiz);
      free(zbuf);
    }
  } else {
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  }
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: 200 OK (search)", req->claddr, req->clport);
  cblistclose(ltarg.words);
  est_cond_delete(ltarg.cond);
  for(i = 0; i < cbmaprnum(node->links); i++){
    free(rtargs[i].label);
    est_cond_delete(rtargs[i].cond);
  }
  free(rtargs);
  free(rths);
  cbmapclose(hints);
  resmap_delete(resmap);
  est_cond_delete(cond);
  free(myurl);
}


/* send the result of the search helper command */
static int sendnodecmdsearchhelper(int clsock, REQUEST *req, NODE *node, const char *myurl){
  CBDATUM *datum;
  char *dbuf, *ebuf, dbpath[URIBUFSIZ], opath[URIBUFSIZ];
  int i, dsiz, err;
  if(pthread_mutex_lock(&(node->mutex)) != 0){
    log_print(LL_ERROR, "locking failed");
    return FALSE;
  }
  datum = cbdatumopen(NULL, -1);
  sprintf(dbpath, "%s%c%s%c%s", g_rootdir, ESTPATHCHR, NODEDIR, ESTPATHCHR, node->name);
  cbdatumprintf(datum, "%s \"%s\"", SEARCHHELPER, dbpath);
  dbuf = cbmapdump(req->params, &dsiz);
  ebuf = cbbaseencode(dbuf, dsiz);
  sprintf(opath, "%s%chelper-%d", dbpath, ESTPATHCHR, g_accesscount);
  cbdatumprintf(datum, " \"%s\" \"%s\"", opath, ebuf);
  free(ebuf);
  free(dbuf);
  ebuf = cbbaseencode(myurl, -1);
  cbdatumprintf(datum, " \"%s\"", ebuf);
  free(ebuf);
  ebuf = cbbaseencode(node->label, -1);
  cbdatumprintf(datum, " \"%s\"", ebuf);
  free(ebuf);
  cbdatumprintf(datum, " \"%d\" \"%d\" \"%d\" \"%d\" \"%d\"",
                g_rateuri, g_mergemethod, g_scoreexpr, g_searchmax, g_wildmax);
  cbdatumprintf(datum, " \"%d\" \"%d\" \"%d\" \"%d\" \"%d\"",
                g_snipwwidth, g_sniphwidth, g_snipawidth, g_scancheck, g_smlrvnum);
  log_print(LL_DEBUG, "[%s:%d]: calling the search helper: %s",
            req->claddr, req->clport, cbdatumptr(datum));
  for(i = 0; i < HELPERTRYNUM; i++){
    system(cbdatumptr(datum));
    if((dbuf = cbreadfile(opath, &dsiz)) != NULL) break;
    est_usleep(1000 * 100);
  }
  cbdatumclose(datum);
  err = FALSE;
  if(dbuf){
    datum = cbdatumopen(NULL, -1);
    cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
    addservinfo(datum, req->now, 0);
    cbdatumprintf(datum, "Content-Type: %s; charset=UTF-8\r\n", ESTSEARCHTYPE);
    ebuf = cbdatestrhttp(node->mtime, 0);
    cbdatumprintf(datum, "Last-Modified: %s\r\n", ebuf);
    free(ebuf);
    cbdatumprintf(datum, "\r\n");
    cbdatumcat(datum, dbuf, dsiz);
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    cbdatumclose(datum);
    free(dbuf);
    log_print(LL_DEBUG, "[%s:%d]: 200 OK (search-helper)", req->claddr, req->clport);
  } else {
    log_print(LL_WARN, "[%s:%d]: the search helper failed", req->claddr, req->clport);
    err = TRUE;
  }
  unlink(opath);
  if(pthread_mutex_unlock(&(node->mutex)) != 0) log_print(LL_ERROR, "unlocking failed");
  return err ? FALSE : TRUE;
}


/* send the result of the list command */
static void sendnodecmdlist(int clsock, REQUEST *req, NODE *node){
  ESTDOC *doc;
  CBDATUM *head, *body;
  const char *tmp, *prev;
  char *zbuf;
  int max, num, cnt, id, zsiz;
  max = DEFMAXSRCH;
  if((tmp = cbmapget(req->params, "max", -1, NULL)) != NULL && (num = atoi(tmp)) >= 0)
    max = num;
  max = max > g_searchmax ? g_searchmax : max;
  prev = cbmapget(req->params, "prev", -1, NULL);
  if(pthread_mutex_lock(&(node->mutex)) != 0){
    log_print(LL_ERROR, "locking failed");
    senderror(clsock, req, 500, "Internal Server Error (locking failed)");
    return;
  }
  body = cbdatumopen(NULL, -1);
  est_mtdb_iter_init(node->db, prev);
  cnt = 0;
  while(cnt < max && (id = est_mtdb_iter_next(node->db)) > 0){
    if(!(doc = est_mtdb_get_doc(node->db, id, ESTGDNOTEXT))) continue;
    tmp = est_doc_attr(doc, ESTDATTRID);
    cbdatumcat(body, tmp ? tmp : "", -1);
    cbdatumcat(body, "\t", 1);
    tmp = est_doc_attr(doc, ESTDATTRURI);
    cbdatumcat(body, tmp ? tmp : "", -1);
    cbdatumcat(body, "\t", 1);
    tmp = est_doc_attr(doc, ESTDATTRDIGEST);
    cbdatumcat(body, tmp ? tmp : "", -1);
    cbdatumcat(body, "\t", 1);
    tmp = est_doc_attr(doc, ESTDATTRCDATE);
    cbdatumcat(body, tmp ? tmp : "", -1);
    cbdatumcat(body, "\t", 1);
    tmp = est_doc_attr(doc, ESTDATTRMDATE);
    cbdatumcat(body, tmp ? tmp : "", -1);
    cbdatumcat(body, "\t", 1);
    tmp = est_doc_attr(doc, ESTDATTRADATE);
    cbdatumcat(body, tmp ? tmp : "", -1);
    cbdatumcat(body, "\t", 1);
    tmp = est_doc_attr(doc, ESTDATTRTITLE);
    cbdatumcat(body, tmp ? tmp : "", -1);
    cbdatumcat(body, "\t", 1);
    tmp = est_doc_attr(doc, ESTDATTRAUTHOR);
    cbdatumcat(body, tmp ? tmp : "", -1);
    cbdatumcat(body, "\t", 1);
    tmp = est_doc_attr(doc, ESTDATTRTYPE);
    cbdatumcat(body, tmp ? tmp : "", -1);
    cbdatumcat(body, "\t", 1);
    tmp = est_doc_attr(doc, ESTDATTRLANG);
    cbdatumcat(body, tmp ? tmp : "", -1);
    cbdatumcat(body, "\t", 1);
    tmp = est_doc_attr(doc, ESTDATTRGENRE);
    cbdatumcat(body, tmp ? tmp : "", -1);
    cbdatumcat(body, "\t", 1);
    tmp = est_doc_attr(doc, ESTDATTRSIZE);
    cbdatumcat(body, tmp ? tmp : "", -1);
    cbdatumcat(body, "\t", 1);
    tmp = est_doc_attr(doc, ESTDATTRWEIGHT);
    cbdatumcat(body, tmp ? tmp : "", -1);
    cbdatumcat(body, "\t", 1);
    tmp = est_doc_attr(doc, ESTDATTRMISC);
    cbdatumcat(body, tmp ? tmp : "", -1);
    cbdatumcat(body, "\n", 1);
    est_doc_delete(doc);
    cnt++;
  }
  if(pthread_mutex_unlock(&(node->mutex)) != 0) log_print(LL_ERROR, "unlocking failed");
  head = cbdatumopen(NULL, -1);
  cbdatumprintf(head, "HTTP/1.0 200 OK\r\n");
  addservinfo(head, req->now, 0);
  cbdatumprintf(head, "Content-Type: %s; charset=UTF-8\r\n", ESTLISTTYPE);
  if(req->gzip){
    cbdatumprintf(head, "Content-Encoding: gzip\r\n");
  } else if(req->deflate){
    cbdatumprintf(head, "Content-Encoding: deflate\r\n");
  }
  cbdatumprintf(head, "\r\n");
  est_sock_send_all(clsock, cbdatumptr(head), cbdatumsize(head));
  cbdatumclose(head);
  if(req->gzip){
    if((zbuf = est_deflate(cbdatumptr(body), cbdatumsize(body), &zsiz, 1)) != NULL){
      est_sock_send_all(clsock, zbuf, zsiz);
      free(zbuf);
    }
  } else if(req->deflate){
    if((zbuf = est_deflate(cbdatumptr(body), cbdatumsize(body), &zsiz, 0)) != NULL){
      est_sock_send_all(clsock, zbuf, zsiz);
      free(zbuf);
    }
  } else {
    est_sock_send_all(clsock, cbdatumptr(body), cbdatumsize(body));
  }
  cbdatumclose(body);
  log_print(LL_DEBUG, "[%s:%d]: 200 OK (list)", req->claddr, req->clport);
}


/* send the result of the get_doc command */
static void sendnodecmdgetdoc(int clsock, REQUEST *req, NODE *node){
  ESTDOC *doc;
  CBDATUM *datum;
  const char *tmp, *uri;
  char *draft, *zbuf;
  int id, zsiz;
  id = (tmp = cbmapget(req->params, "id", -1, NULL)) ? atoi(tmp) : 0;
  if(!(uri = cbmapget(req->params, "uri", -1, NULL))) uri = "";
  if(id < 1 && uri[0] == '\0'){
    senderror(clsock, req, 400, "Bad Request (the parameters lack)");
    return;
  }
  if(est_mtdb_cache_usage(node->db) >= g_helpershift &&
     sendnodecmdgetdochelper(clsock, req, node, id, uri)) return;
  if(id < 1) id = est_mtdb_uri_to_id(node->db, uri);
  if(id > 0 && (doc = est_mtdb_get_doc(node->db, id, 0)) != NULL){
    setdocorigin(doc, req, node, -1);
    draft = est_doc_dump_draft(doc);
    datum = cbdatumopen(NULL, -1);
    cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
    addservinfo(datum, req->now, 0);
    cbdatumprintf(datum, "Content-Type: %s; charset=UTF-8\r\n", ESTDRAFTTYPE);
    if(req->gzip){
      cbdatumprintf(datum, "Content-Encoding: gzip\r\n");
    } else if(req->deflate){
      cbdatumprintf(datum, "Content-Encoding: deflate\r\n");
    }
    cbdatumprintf(datum, "\r\n");
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    cbdatumsetsize(datum, 0);
    cbdatumcat(datum, draft, -1);
    if(req->gzip){
      if((zbuf = est_deflate(cbdatumptr(datum), cbdatumsize(datum), &zsiz, 1)) != NULL){
        est_sock_send_all(clsock, zbuf, zsiz);
        free(zbuf);
      }
    } else if(req->deflate){
      if((zbuf = est_deflate(cbdatumptr(datum), cbdatumsize(datum), &zsiz, 0)) != NULL){
        est_sock_send_all(clsock, zbuf, zsiz);
        free(zbuf);
      }
    } else {
      est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    }
    cbdatumclose(datum);
    log_print(LL_DEBUG, "[%s:%d]: 200 OK (get_doc)", req->claddr, req->clport);
    free(draft);
    est_doc_delete(doc);
  } else {
    if(est_mtdb_fatal(node->db))
      log_print(LL_ERROR, "DB-ERROR: %s", est_err_msg(est_mtdb_error(node->db)));
    senderror(clsock, req, 400, "Bad Request (maybe, the document does not exist)");
  }
}


/* send the result of the get_doc helper command */
static int sendnodecmdgetdochelper(int clsock, REQUEST *req, NODE *node,
                                   int id, const char *uri){
  ESTDOC *doc;
  CBDATUM *datum;
  char *dbuf, *ebuf, dbpath[URIBUFSIZ], opath[URIBUFSIZ], *draft;
  int i, dsiz, err;
  if(pthread_mutex_lock(&(node->mutex)) != 0){
    log_print(LL_ERROR, "locking failed");
    return FALSE;
  }
  datum = cbdatumopen(NULL, -1);
  sprintf(dbpath, "%s%c%s%c%s", g_rootdir, ESTPATHCHR, NODEDIR, ESTPATHCHR, node->name);
  cbdatumprintf(datum, "%s \"%s\"", GETDOCHELPER, dbpath);
  sprintf(opath, "%s%chelper-%d", dbpath, ESTPATHCHR, g_accesscount);
  cbdatumprintf(datum, " \"%s\"", opath);
  cbdatumprintf(datum, " \"%d\"", id);
  ebuf = cbbaseencode(uri, -1);
  cbdatumprintf(datum, " \"%s\"", ebuf);
  free(ebuf);
  log_print(LL_DEBUG, "[%s:%d]: calling the get_doc helper: %s",
            req->claddr, req->clport, cbdatumptr(datum));
  for(i = 0; i < HELPERTRYNUM; i++){
    system(cbdatumptr(datum));
    if((dbuf = cbreadfile(opath, &dsiz)) != NULL) break;
    est_usleep(1000 * 100);
  }
  cbdatumclose(datum);
  err = FALSE;
  if(dbuf){
    if(dsiz > 0){
      datum = cbdatumopen(NULL, -1);
      cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
      addservinfo(datum, req->now, 0);
      cbdatumprintf(datum, "Content-Type: %s; charset=UTF-8\r\n", ESTDRAFTTYPE);
      ebuf = cbdatestrhttp(node->mtime, 0);
      cbdatumprintf(datum, "Last-Modified: %s\r\n", ebuf);
      free(ebuf);
      cbdatumprintf(datum, "\r\n");
      doc = est_doc_new_from_draft(dbuf);
      setdocorigin(doc, req, node, -1);
      draft = est_doc_dump_draft(doc);
      cbdatumcat(datum, draft, -1);
      free(draft);
      free(doc);
      est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
      cbdatumclose(datum);
      log_print(LL_DEBUG, "[%s:%d]: 200 OK (get_doc-helper)", req->claddr, req->clport);
    } else {
      senderror(clsock, req, 400, "Bad Request (maybe, the document does not exist)");
    }
    free(dbuf);
  } else {
    log_print(LL_WARN, "[%s:%d]: the get_doc helper failed", req->claddr, req->clport);
    err = TRUE;
  }
  unlink(opath);
  if(pthread_mutex_unlock(&(node->mutex)) != 0) log_print(LL_ERROR, "unlocking failed");
  return err ? FALSE : TRUE;
}


/* send the result of the get_doc_attr command */
static void sendnodecmdgetdocattr(int clsock, REQUEST *req, NODE *node){
  CBDATUM *datum;
  const char *tmp, *uri, *attr;
  char *value;
  int id;
  id = (tmp = cbmapget(req->params, "id", -1, NULL)) ? atoi(tmp) : 0;
  if(!(uri = cbmapget(req->params, "uri", -1, NULL))) uri = "";
  if(!(attr = cbmapget(req->params, "attr", -1, NULL))) attr = "";
  if((id < 1 && uri[0] == '\0') || attr[0] == '\0'){
    senderror(clsock, req, 400, "Bad Request (the parameters lack)");
    return;
  }
  if(est_mtdb_cache_usage(node->db) >= g_helpershift &&
     sendnodecmdgetdocattrhelper(clsock, req, node, id, uri, attr)) return;
  if(id < 1) id = est_mtdb_uri_to_id(node->db, uri);
  if(id > 0 && (value = est_mtdb_get_doc_attr(node->db, id, attr)) != NULL){
    datum = cbdatumopen(NULL, -1);
    cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
    addservinfo(datum, req->now, 0);
    cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
    cbdatumprintf(datum, "\r\n");
    cbdatumprintf(datum, "%s\n", value);
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    cbdatumclose(datum);
    log_print(LL_DEBUG, "[%s:%d]: 200 OK (get_doc_attr)", req->claddr, req->clport);
    free(value);
  } else {
    if(est_mtdb_fatal(node->db))
      log_print(LL_ERROR, "DB-ERROR: %s", est_err_msg(est_mtdb_error(node->db)));
    senderror(clsock, req, 400,
              "Bad Request (maybe, the document or the attribute does not exist)");
  }
}


/* send the result of the get_doc_attr helper command */
static int sendnodecmdgetdocattrhelper(int clsock, REQUEST *req, NODE *node,
                                       int id, const char *uri, const char *attr){
  CBDATUM *datum;
  char *dbuf, *ebuf, dbpath[URIBUFSIZ], opath[URIBUFSIZ];
  int i, dsiz, err;
  if(pthread_mutex_lock(&(node->mutex)) != 0){
    log_print(LL_ERROR, "locking failed");
    return FALSE;
  }
  datum = cbdatumopen(NULL, -1);
  sprintf(dbpath, "%s%c%s%c%s", g_rootdir, ESTPATHCHR, NODEDIR, ESTPATHCHR, node->name);
  cbdatumprintf(datum, "%s \"%s\"", GETDOCATTRHELPER, dbpath);
  sprintf(opath, "%s%chelper-%d", dbpath, ESTPATHCHR, g_accesscount);
  cbdatumprintf(datum, " \"%s\"", opath);
  cbdatumprintf(datum, " \"%d\"", id);
  ebuf = cbbaseencode(uri, -1);
  cbdatumprintf(datum, " \"%s\"", ebuf);
  free(ebuf);
  ebuf = cbbaseencode(attr, -1);
  cbdatumprintf(datum, " \"%s\"", ebuf);
  free(ebuf);
  log_print(LL_DEBUG, "[%s:%d]: calling the get_doc_attr helper: %s",
            req->claddr, req->clport, cbdatumptr(datum));
  for(i = 0; i < HELPERTRYNUM; i++){
    system(cbdatumptr(datum));
    if((dbuf = cbreadfile(opath, &dsiz)) != NULL) break;
    est_usleep(1000 * 100);
  }
  cbdatumclose(datum);
  err = FALSE;
  if(dbuf){
    if(strchr(dbuf, '\n')){
      datum = cbdatumopen(NULL, -1);
      cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
      addservinfo(datum, req->now, 0);
      cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
      ebuf = cbdatestrhttp(node->mtime, 0);
      cbdatumprintf(datum, "Last-Modified: %s\r\n", ebuf);
      free(ebuf);
      cbdatumprintf(datum, "\r\n");
      cbdatumcat(datum, dbuf, dsiz);
      est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
      cbdatumclose(datum);
      log_print(LL_DEBUG, "[%s:%d]: 200 OK (get_doc_attr-helper)", req->claddr, req->clport);
    } else {
      senderror(clsock, req, 400,
                "Bad Request (maybe, the document or the attribute does not exist)");
    }
    free(dbuf);
  } else {
    log_print(LL_WARN, "[%s:%d]: the get_doc_attr helper failed", req->claddr, req->clport);
    err = TRUE;
  }
  unlink(opath);
  if(pthread_mutex_unlock(&(node->mutex)) != 0) log_print(LL_ERROR, "unlocking failed");
  return err ? FALSE : TRUE;
}


/* send the result of the etch_doc command */
static void sendnodecmdetchdoc(int clsock, REQUEST *req, NODE *node){
  ESTDOC *doc;
  CBMAP *kwords;
  CBDATUM *datum;
  const char *tmp, *uri, *kbuf;
  int id, ksiz;
  id = (tmp = cbmapget(req->params, "id", -1, NULL)) ? atoi(tmp) : 0;
  if(!(uri = cbmapget(req->params, "uri", -1, NULL))) uri = "";
  if(id < 1 && uri[0] == '\0'){
    senderror(clsock, req, 400, "Bad Request (the parameters lack)");
    return;
  }
  if(est_mtdb_cache_usage(node->db) >= g_helpershift &&
     sendnodecmdetchdochelper(clsock, req, node, id, uri)) return;
  if(id < 1) id = est_mtdb_uri_to_id(node->db, uri);
  kwords = NULL;
  if(id > 0){
    kwords = est_mtdb_get_keywords(node->db, id);
    if(!kwords && (doc = est_mtdb_get_doc(node->db, id, 0)) != NULL){
      kwords = est_mtdb_etch_doc(node->db, doc, g_smlrvnum > 0 ? g_smlrvnum : KWORDNUM);
      if(g_runmode == RM_NORMAL) est_mtdb_put_keywords(node->db, id, kwords, 1.0);
      est_doc_delete(doc);
    }
  }
  if(kwords){
    datum = cbdatumopen(NULL, -1);
    cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
    addservinfo(datum, req->now, 0);
    cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
    cbdatumprintf(datum, "\r\n");
    cbmapiterinit(kwords);
    while((kbuf = cbmapiternext(kwords, &ksiz)) != NULL){
      cbdatumprintf(datum, "%s\t%s\n", kbuf, cbmapiterval(kbuf, NULL));
    }
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    cbdatumclose(datum);
    log_print(LL_DEBUG, "[%s:%d]: 200 OK (etch_doc)", req->claddr, req->clport);
    cbmapclose(kwords);
  } else {
    if(est_mtdb_fatal(node->db))
      log_print(LL_ERROR, "DB-ERROR: %s", est_err_msg(est_mtdb_error(node->db)));
    senderror(clsock, req, 400, "Bad Request (maybe, the document does not exist)");
  }
}


/* send the result of the etch_doc helper command */
static int sendnodecmdetchdochelper(int clsock, REQUEST *req, NODE *node,
                                    int id, const char *uri){
  CBDATUM *datum;
  char *dbuf, *ebuf, dbpath[URIBUFSIZ], opath[URIBUFSIZ];
  int i, dsiz, err;
  if(pthread_mutex_lock(&(node->mutex)) != 0){
    log_print(LL_ERROR, "locking failed");
    return FALSE;
  }
  datum = cbdatumopen(NULL, -1);
  sprintf(dbpath, "%s%c%s%c%s", g_rootdir, ESTPATHCHR, NODEDIR, ESTPATHCHR, node->name);
  cbdatumprintf(datum, "%s \"%s\"", ETCHDOCHELPER, dbpath);
  sprintf(opath, "%s%chelper-%d", dbpath, ESTPATHCHR, g_accesscount);
  cbdatumprintf(datum, " \"%s\"", opath);
  cbdatumprintf(datum, " \"%d\"", id);
  ebuf = cbbaseencode(uri, -1);
  cbdatumprintf(datum, " \"%s\"", ebuf);
  free(ebuf);
  cbdatumprintf(datum, " \"%d\"", g_smlrvnum > 0 ? g_smlrvnum : KWORDNUM);
  log_print(LL_DEBUG, "[%s:%d]: calling the etch_doc helper: %s",
            req->claddr, req->clport, cbdatumptr(datum));
  for(i = 0; i < HELPERTRYNUM; i++){
    system(cbdatumptr(datum));
    if((dbuf = cbreadfile(opath, &dsiz)) != NULL) break;
    est_usleep(1000 * 100);
  }
  cbdatumclose(datum);
  err = FALSE;
  if(dbuf){
    if(strchr(dbuf, '\n')){
      datum = cbdatumopen(NULL, -1);
      cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
      addservinfo(datum, req->now, 0);
      cbdatumprintf(datum, "Content-Type: %s; charset=UTF-8\r\n", ESTDRAFTTYPE);
      ebuf = cbdatestrhttp(node->mtime, 0);
      cbdatumprintf(datum, "Last-Modified: %s\r\n", ebuf);
      free(ebuf);
      cbdatumprintf(datum, "\r\n");
      cbdatumcat(datum, dbuf, dsiz);
      est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
      cbdatumclose(datum);
      log_print(LL_DEBUG, "[%s:%d]: 200 OK (etch_doc-helper)", req->claddr, req->clport);
    } else {
      senderror(clsock, req, 400, "Bad Request (maybe, the document does not exist)");
    }
    free(dbuf);
  } else {
    log_print(LL_WARN, "[%s:%d]: the etch_doc helper failed", req->claddr, req->clport);
    err = TRUE;
  }
  unlink(opath);
  if(pthread_mutex_unlock(&(node->mutex)) != 0) log_print(LL_ERROR, "unlocking failed");
  return err ? FALSE : TRUE;
}


/* send the result of the uri_to_id command */
static void sendnodecmduritoid(int clsock, REQUEST *req, NODE *node){
  CBDATUM *datum;
  const char *uri;
  int id;
  uri = cbmapget(req->params, "uri", -1, NULL);
  if(!uri){
    senderror(clsock, req, 400, "Bad Request (the parameters lack)");
    return;
  }
  if(est_mtdb_cache_usage(node->db) >= g_helpershift &&
     sendnodecmduritoidhelper(clsock, req, node, uri)) return;
  if((id = est_mtdb_uri_to_id(node->db, uri)) > 0){
    datum = cbdatumopen(NULL, -1);
    cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
    addservinfo(datum, req->now, 0);
    cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
    cbdatumprintf(datum, "\r\n");
    cbdatumprintf(datum, "%d\n", id);
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    cbdatumclose(datum);
    log_print(LL_DEBUG, "[%s:%d]: 200 OK (uri_to_id)", req->claddr, req->clport);
  } else {
    if(est_mtdb_fatal(node->db))
      log_print(LL_ERROR, "DB-ERROR: %s", est_err_msg(est_mtdb_error(node->db)));
    senderror(clsock, req, 400, "Bad Request (maybe, the URI is not registered)");
  }
}


/* send the result of the inform helper command */
static int sendnodecmduritoidhelper(int clsock, REQUEST *req, NODE *node, const char *uri){
  CBDATUM *datum;
  char *dbuf, *ebuf, dbpath[URIBUFSIZ], opath[URIBUFSIZ];
  int i, dsiz, err;
  if(pthread_mutex_lock(&(node->mutex)) != 0){
    log_print(LL_ERROR, "locking failed");
    return FALSE;
  }
  datum = cbdatumopen(NULL, -1);
  sprintf(dbpath, "%s%c%s%c%s", g_rootdir, ESTPATHCHR, NODEDIR, ESTPATHCHR, node->name);
  cbdatumprintf(datum, "%s \"%s\"", URITOIDHELPER, dbpath);
  sprintf(opath, "%s%chelper-%d", dbpath, ESTPATHCHR, g_accesscount);
  cbdatumprintf(datum, " \"%s\"", opath);
  ebuf = cbbaseencode(uri, -1);
  cbdatumprintf(datum, " \"%s\"", ebuf);
  free(ebuf);
  log_print(LL_DEBUG, "[%s:%d]: calling the uri_to_id helper: %s",
            req->claddr, req->clport, cbdatumptr(datum));
  for(i = 0; i < HELPERTRYNUM; i++){
    system(cbdatumptr(datum));
    if((dbuf = cbreadfile(opath, &dsiz)) != NULL) break;
    est_usleep(1000 * 100);
  }
  cbdatumclose(datum);
  err = FALSE;
  if(dbuf){
    if(dsiz > 0){
      datum = cbdatumopen(NULL, -1);
      cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
      addservinfo(datum, req->now, 0);
      cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
      ebuf = cbdatestrhttp(node->mtime, 0);
      cbdatumprintf(datum, "Last-Modified: %s\r\n", ebuf);
      free(ebuf);
      cbdatumprintf(datum, "\r\n");
      cbdatumcat(datum, dbuf, dsiz);
      est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
      cbdatumclose(datum);
      log_print(LL_DEBUG, "[%s:%d]: 200 OK (uri_to_id-helper)", req->claddr, req->clport);
    } else {
      senderror(clsock, req, 400, "Bad Request (maybe, the URI is not registered)");
    }
    free(dbuf);
  } else {
    log_print(LL_WARN, "[%s:%d]: the uri_to_id helper failed", req->claddr, req->clport);
    err = TRUE;
  }
  unlink(opath);
  if(pthread_mutex_unlock(&(node->mutex)) != 0) log_print(LL_ERROR, "unlocking failed");
  return err ? FALSE : TRUE;
}


/* send the result of the pub_doc command */
static void sendnodecmdputdoc(int clsock, REQUEST *req, NODE *node){
  ESTDOC *doc;
  CBDATUM *datum;
  CBMAP *kwords;
  const char *draft;
  double ratio;
  if(req->ctype && cbstrfwimatch(req->ctype, ESTDRAFTTYPE)){
    draft = req->body;
  } else {
    draft = cbmapget(req->params, "draft", -1, NULL);
  }
  if(!draft){
    senderror(clsock, req, 400, "Bad Request (the parameters lack)");
    return;
  }
  doc = est_doc_new_from_draft(draft);
  est_doc_slim(doc, g_limittextsize);
  if(est_mtdb_put_doc(node->db, doc, ESTPDCLEAN)){
    if(!est_doc_keywords(doc) && g_smlrvnum > 0){
      if(g_extdelay < 0){
        kwords = est_morph_etch_doc(doc, g_smlrvnum);
        est_mtdb_put_keywords(node->db, est_doc_id(doc), kwords, 1.0);
        cbmapclose(kwords);
      } else if(est_mtdb_doc_num(node->db) > g_extdelay){
        kwords = est_mtdb_etch_doc(node->db, doc, g_smlrvnum);
        est_mtdb_put_keywords(node->db, est_doc_id(doc), kwords, 1.0);
        cbmapclose(kwords);
      }
    }
    node->mtime = req->now;
    node->dirty = TRUE;
    ratio = est_mtdb_used_cache_size(node->db) / g_cachesize;
    g_cacheratio = ratio > g_cacheratio ? ratio : g_cacheratio;
    datum = cbdatumopen(NULL, -1);
    cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
    addservinfo(datum, req->now, 0);
    cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
    cbdatumprintf(datum, "\r\n");
    cbdatumprintf(datum, "OK\n");
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    cbdatumclose(datum);
    log_print(LL_DEBUG, "[%s:%d]: 200 OK (put_doc)", req->claddr, req->clport);
  } else {
    if(est_mtdb_fatal(node->db))
      log_print(LL_ERROR, "DB-ERROR: %s", est_err_msg(est_mtdb_error(node->db)));
    if(!est_doc_attr(doc, ESTDATTRURI)){
      senderror(clsock, req, 400, "Bad Request (maybe, the document is invalid)");
    } else {
      senderror(clsock, req, 400, "Bad Request (maybe, the database has a fatal error)");
    }
  }
  est_doc_delete(doc);
}


/* send the result of the out_doc command */
static void sendnodecmdoutdoc(int clsock, REQUEST *req, NODE *node){
  CBDATUM *datum;
  const char *tmp, *uri;
  int id;
  id = (tmp = cbmapget(req->params, "id", -1, NULL)) ? atoi(tmp) : 0;
  if(!(uri = cbmapget(req->params, "uri", -1, NULL))) uri = "";
  if(id < 1 && uri[0] == '\0'){
    senderror(clsock, req, 400, "Bad Request (the parameters lack)");
    return;
  }
  if(id < 1) id = est_mtdb_uri_to_id(node->db, uri);
  if(id > 0 && est_mtdb_out_doc(node->db, id, ESTODCLEAN)){
    node->mtime = req->now;
    node->dirty = TRUE;
    datum = cbdatumopen(NULL, -1);
    cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
    addservinfo(datum, req->now, 0);
    cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
    cbdatumprintf(datum, "\r\n");
    cbdatumprintf(datum, "OK\n");
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    cbdatumclose(datum);
    log_print(LL_DEBUG, "[%s:%d]: 200 OK (out_doc)", req->claddr, req->clport);
  } else {
    if(est_mtdb_fatal(node->db))
      log_print(LL_ERROR, "DB-ERROR: %s", est_err_msg(est_mtdb_error(node->db)));
    senderror(clsock, req, 400, "Bad Request (maybe, the document does not exist)");
  }
}


/* send the result of the edit_doc command */
static void sendnodecmdeditdoc(int clsock, REQUEST *req, NODE *node){
  ESTDOC *doc;
  CBDATUM *datum;
  const char *draft;
  if(req->ctype && cbstrfwimatch(req->ctype, ESTDRAFTTYPE)){
    draft = req->body;
  } else {
    draft = cbmapget(req->params, "draft", -1, NULL);
  }
  if(!draft){
    senderror(clsock, req, 400, "Bad Request (the parameters lack)");
    return;
  }
  doc = est_doc_new_from_draft(draft);
  if(est_mtdb_edit_doc(node->db, doc)){
    node->mtime = req->now;
    datum = cbdatumopen(NULL, -1);
    cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
    addservinfo(datum, req->now, 0);
    cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
    cbdatumprintf(datum, "\r\n");
    cbdatumprintf(datum, "OK\n");
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    cbdatumclose(datum);
    log_print(LL_DEBUG, "[%s:%d]: 200 OK (edit_doc)", req->claddr, req->clport);
  } else {
    if(est_mtdb_fatal(node->db))
      log_print(LL_ERROR, "DB-ERROR: %s", est_err_msg(est_mtdb_error(node->db)));
    if(!est_doc_attr(doc, ESTDATTRID) || !est_doc_attr(doc, ESTDATTRURI)){
      senderror(clsock, req, 400, "Bad Request (maybe, the document is invalid)");
    } else {
      senderror(clsock, req, 400, "Bad Request (maybe, the database has a fatal error)");
    }
  }
  est_doc_delete(doc);
}


/* send the result of the sync command */
static void sendnodecmdsync(int clsock, REQUEST *req, NODE *node){
  CBDATUM *datum;
  if(est_mtdb_sync(node->db)){
    node->mtime = req->now;
    datum = cbdatumopen(NULL, -1);
    cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
    addservinfo(datum, req->now, 0);
    cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
    cbdatumprintf(datum, "\r\n");
    cbdatumprintf(datum, "OK\n");
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    cbdatumclose(datum);
    log_print(LL_DEBUG, "[%s:%d]: 200 OK (sync)", req->claddr, req->clport);
  } else {
    if(est_mtdb_fatal(node->db))
      log_print(LL_ERROR, "DB-ERROR: %s", est_err_msg(est_mtdb_error(node->db)));
    senderror(clsock, req, 400, "Bad Request (maybe, the database has a fatal error)");
  }
}


/* send the result of the optimize command */
static void sendnodecmdoptimize(int clsock, REQUEST *req, NODE *node){
  CBDATUM *datum;
  if(est_mtdb_optimize(node->db, ESTOPTNOPURGE)){
    node->mtime = req->now;
    datum = cbdatumopen(NULL, -1);
    cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
    addservinfo(datum, req->now, 0);
    cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
    cbdatumprintf(datum, "\r\n");
    cbdatumprintf(datum, "OK\n");
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    cbdatumclose(datum);
    log_print(LL_DEBUG, "[%s:%d]: 200 OK (optimize)", req->claddr, req->clport);
  } else {
    if(est_mtdb_fatal(node->db))
      log_print(LL_ERROR, "DB-ERROR: %s", est_err_msg(est_mtdb_error(node->db)));
    senderror(clsock, req, 400, "Bad Request (maybe, the database has a fatal error)");
  }
}


/* send the result of the _set_user command */
static void sendnodecmdsetuser(int clsock, REQUEST *req, NODE *node){
  CBDATUM *datum;
  const char *name, *tmp;
  int mode;
  name = cbmapget(req->params, "name", -1, NULL);
  mode = (tmp = cbmapget(req->params, "mode", -1, NULL)) != NULL &&
    tmp[0] != '\0' ? atoi(tmp) : -1;
  if(!name || name[0] == '\0' || mode < 0 || mode > 2){
    senderror(clsock, req, 400, "Bad Request (the parameters lack)");
    return;
  }
  if(!check_alnum_name(name)){
    senderror(clsock, req, 400, "Bad Request (the name is invalid)");
    return;
  }
  switch(mode){
  case 1:
    cbmapput(node->admins, name, -1, "", 0, FALSE);
    break;
  case 2:
    cbmapput(node->users, name, -1, "", 0, FALSE);
    break;
  default:
    cbmapout(node->admins, name, -1);
    cbmapout(node->users, name, -1);
    break;
  }
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
  addservinfo(datum, req->now, 0);
  cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
  cbdatumprintf(datum, "\r\n");
  cbdatumprintf(datum, "OK\n");
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: 200 OK (_set_user)", req->claddr, req->clport);
}


/* send the result of the _set_link command */
static void sendnodecmdsetlink(int clsock, REQUEST *req, NODE *node){
  CBDATUM *datum;
  const char *url, *label, *tmp;
  int credit;
  url = cbmapget(req->params, "url", -1, NULL);
  label = cbmapget(req->params, "label", -1, NULL);
  credit = (tmp = cbmapget(req->params, "credit", -1, NULL)) ? atoi(tmp) : -1;
  if(!url || url[0] == '\0'){
    senderror(clsock, req, 400, "Bad Request (the parameters lack)");
    return;
  }
  node_set_link(node, url, label, credit);
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
  addservinfo(datum, req->now, 0);
  cbdatumprintf(datum, "Content-Type: text/plain; charset=UTF-8\r\n");
  cbdatumprintf(datum, "\r\n");
  cbdatumprintf(datum, "OK\n");
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: 200 OK (_set_link)", req->claddr, req->clport);
}


/* send the result of search user interface */
static void sendnodecmdsearchui(int clsock, REQUEST *req, NODE *node){
  pthread_t lth, *rths;
  TARGLCSRCH ltarg;
  TARGRMSRCH *rtargs;
  RESMAP *resmap;
  RESDOC **resdocs;
  ESTCOND *cond;
  CBMAP *hints;
  const CBLIST *attrs;
  CBDATUM *condbuf, *simcondbuf, *datum;
  const char *tmp, *url, *label, *kbuf, *phrase, *order, *simnode;
  char *myurl, *dstr, name[NUMBUFSIZ], masks[LINKMASKMAX+1], *zbuf;
  int i, num, max, depth, simid, page, top, dnum, wnum, hnum, end, tab, axk, ksiz, rnum, zsiz;
  double curtime;
  time_t mtime;
  myurl = cbsprintf("%s%s%s", req->prefix, NODEPREFIX, node->name);
  if(islooproute(myurl, req)){
    log_print(LL_DEBUG, "[%s:%d]: omitting request loop (self): %s",
              req->claddr, req->clport, myurl);
    free(myurl);
    senderror(clsock, req, 400, "Bad Request (the request loops)");
    return;
  }
  if(pthread_mutex_lock(&(node->mutex)) != 0){
    log_print(LL_ERROR, "locking failed");
    senderror(clsock, req, 500, "Internal Server Error (locking failed)");
    return;
  }
  cond = est_cond_new();
  max = DEFMAXSRCH;
  depth = 0;
  page = 0;
  simid = 0;
  simnode = NULL;
  if((tmp = cbmapget(req->params, "page", -1, NULL)) != NULL && (num = atoi(tmp)) > 0)
    page = num;
  if((tmp = cbmapget(req->params, "pageone", -1, NULL)) != NULL && (num = atoi(tmp)) > 1)
    page = num - 1;
  if((tmp = cbmapget(req->params, "phrase", -1, NULL)) != NULL && tmp[0] != '\0')
    est_cond_set_phrase(cond, tmp);
  if((tmp = cbmapget(req->params, "attr", -1, NULL)) != NULL && tmp[0] != '\0')
    est_cond_add_attr(cond, tmp);
  for(i = 0; i <= CONDATTRMAX; i++){
    num = sprintf(name, "attr%d", i);
    if((tmp = cbmapget(req->params, name, num, NULL)) != NULL && tmp[0] != '\0')
      est_cond_add_attr(cond, tmp);
  }
  if((tmp = cbmapget(req->params, "order", -1, NULL)) != NULL && tmp[0] != '\0')
    est_cond_set_order(cond, tmp);
  if((tmp = cbmapget(req->params, "max", -1, NULL)) != NULL && (num = atoi(tmp)) >= 0)
    max = num;
  max = max > g_searchmax ? g_searchmax : max;
  if((tmp = cbmapget(req->params, "depth", -1, NULL)) != NULL && (num = atoi(tmp)) > 0)
    depth = num;
  if((tmp = cbmapget(req->params, "simid", -1, NULL)) != NULL && (num = atoi(tmp)) > 0)
    simid = num;
  if((tmp = cbmapget(req->params, "simnode", -1, NULL)) != NULL && tmp[0] != '\0')
    simnode = tmp;
  if((tmp = cbmapget(req->params, "mask", -1, NULL)) != NULL && tmp[0] != '\0'){
    num = atoi(tmp);
    for(i = 0; i <= LINKMASKMAX; i++){
      masks[i] = num & (1 << i);
    }
  } else if((tmp = cbmapget(req->params, "allmask", -1, NULL)) != NULL && tmp[0] != '\0'){
    memset(masks, 1, LINKMASKMAX+1);
    for(i = 0; i <= LINKMASKMAX; i++){
      num = sprintf(name, "nomask%d", i);
      if((tmp = cbmapget(req->params, name, num, NULL)) != NULL && tmp[0] != '\0') masks[i] = 0;
    }
  } else {
    memset(masks, 0, LINKMASKMAX+1);
    if(!(tmp = cbmapget(req->params, "nomask", -1, NULL)) || tmp[0] == '\0'){
      for(i = 0; i <= LINKMASKMAX; i++){
        num = sprintf(name, "mask%d", i);
        if((tmp = cbmapget(req->params, name, num, NULL)) != NULL && tmp[0] != '\0') masks[i] = 1;
      }
    }
  }
  if(depth >= g_searchdepth) depth = g_searchdepth;
  switch(g_uiphraseform){
  case PM_SIMPLE:
    est_cond_set_options(cond, ESTCONDSIMPLE);
    break;
  case PM_ROUGH:
    est_cond_set_options(cond, ESTCONDROUGH);
    break;
  case PM_UNION:
    est_cond_set_options(cond, ESTCONDUNION);
    break;
  case PM_ISECT:
    est_cond_set_options(cond, ESTCONDISECT);
    break;
  }
  top = !est_cond_phrase(cond) && !est_cond_attrs(cond) && simid < 1;
  phrase = est_cond_phrase(cond);
  attrs = est_cond_attrs(cond);
  order = est_cond_order(cond);
  condbuf = cbdatumopen(NULL, -1);
  simcondbuf = cbdatumopen(NULL, -1);
  cbdatumprintf(condbuf, "phrase=%?", phrase ? phrase : "");
  cbdatumprintf(simcondbuf, "phrase=");
  if(attrs){
    for(i = 0; i < cblistnum(attrs); i++){
      tmp = cblistval(attrs, i, NULL);
      if(tmp[0] != '\0'){
        cbdatumprintf(condbuf, "&amp;attr%d=%?", i + 1, tmp);
        cbdatumprintf(simcondbuf, "&amp;attr%d=%?", i + 1, tmp);
      }
    }
  }
  if(order && order[0] != '\0'){
    cbdatumprintf(condbuf, "&amp;order=%?", order);
    cbdatumprintf(simcondbuf, "&amp;order=%?", order);
  }
  if(max != DEFMAXSRCH){
    cbdatumprintf(condbuf, "&amp;max=%d", max);
    cbdatumprintf(simcondbuf, "&amp;max=%d", max);
  }
  if(depth > 0){
    cbdatumprintf(condbuf, "&amp;depth=%d", depth);
    cbdatumprintf(simcondbuf, "&amp;depth=%d", depth);
  }
  if(simid > 0 && simnode && simnode[0] != '\0'){
    cbdatumprintf(condbuf, "&amp;simid=%d", simid);
    cbdatumprintf(condbuf, "&amp;simnode=%?", simnode);
    setsimilarphrase(cond, simnode, simid);
    phrase = est_cond_phrase(cond);
  }
  for(i = 0; i <= LINKMASKMAX; i++){
    if(masks[i]) cbdatumprintf(condbuf, "&amp;mask%d=1", i);
  }
  resmap = resmap_new();
  hints = cbmapopenex(MINIBNUM);
  curtime = est_gettimeofday();
  ltarg.req = req;
  ltarg.alive = TRUE;
  ltarg.cond = est_cond_dup(cond);
  ltarg.hints = hints;
  ltarg.max = max + page * max + 1;
  ltarg.node = node;
  ltarg.resmap = resmap;
  ltarg.words = cblistopen();
  ltarg.hnum = 0;
  ltarg.mhnum = 0;
  ltarg.itime = 0.0;
  ltarg.etime = 0.0;
  if(top || masks[0]){
    ltarg.alive = FALSE;
  } else if(g_stmode){
    searchlocal(&ltarg);
    ltarg.alive = FALSE;
  } else if(pthread_create(&lth, NULL, searchlocal, &ltarg) != 0){
    log_print(LL_WARN, "creating thread failed");
    ltarg.alive = FALSE;
  }
  rths = cbmalloc(cbmaprnum(node->links) * sizeof(pthread_t) + 1);
  rtargs = cbmalloc(cbmaprnum(node->links) * sizeof(TARGRMSRCH) + 1);
  cbmapiterinit(node->links);
  for(i = 0; (url = cbmapiternext(node->links, NULL)) != NULL; i++){
    rtargs[i].req = req;
    rtargs[i].alive = TRUE;
    rtargs[i].myurl = myurl;
    rtargs[i].cond = est_cond_dup(cond);
    rtargs[i].hints = hints;
    rtargs[i].max = max + page * max + 1;
    rtargs[i].node = node;
    rtargs[i].resmap = resmap;
    rtargs[i].url = url;
    label = cbmapiterval(url, NULL);
    if((tmp = strchr(label, '\t')) != NULL){
      rtargs[i].label = cbmemdup(label, tmp - label);
      rtargs[i].credit = atoi(tmp + 1);
    } else {
      rtargs[i].label = cbmemdup(label, -1);
      rtargs[i].credit = 0;
    }
    rtargs[i].depth = depth;
    rtargs[i].wwidth = g_snipwwidth;
    rtargs[i].hwidth = g_sniphwidth;
    rtargs[i].awidth = g_snipawidth;
    rtargs[i].hnum = 0;
    rtargs[i].dnum = 0;
    rtargs[i].wnum = 0;
    rtargs[i].etime = 0.0;
    rtargs[i].size = 0.0;
    rtargs[i].mtime = 0;
    if(top || depth < 1 || (i < LINKMASKMAX && masks[i+1])){
      rtargs[i].alive = FALSE;
      rtargs[i].hnum = -1;
      rtargs[i].dnum = -1;
      rtargs[i].wnum = -1;
      rtargs[i].etime = -1.0;
      rtargs[i].size = -1.0;
    } else if(g_stmode){
      searchremote(&rtargs[i]);
      rtargs[i].alive = FALSE;
    } else if(pthread_create(rths + i, NULL, searchremote, rtargs + i) != 0){
      log_print(LL_WARN, "creating thread failed");
      rtargs[i].alive = FALSE;
    }
  }
  if(pthread_mutex_unlock(&(node->mutex)) != 0) log_print(LL_ERROR, "unlocking failed");
  dnum = 0;
  wnum = 0;
  mtime = node->mtime;
  if(ltarg.alive){
    if(pthread_join(lth, NULL) == 0){
      dnum = est_mtdb_doc_num(node->db);
      wnum = est_mtdb_word_num(node->db);
    } else {
      log_print(LL_ERROR, "joining thread failed");
    }
  } else if(!top && g_stmode){
    dnum = est_mtdb_doc_num(node->db);
    wnum = est_mtdb_word_num(node->db);
  }
  for(i = 0; i < cbmaprnum(node->links); i++){
    if(rtargs[i].alive){
      if(pthread_join(rths[i], NULL) == 0){
        dnum += rtargs[i].dnum;
        wnum += rtargs[i].wnum;
        if(rtargs[i].mtime > mtime) mtime = rtargs[i].mtime;
      } else {
        log_print(LL_ERROR, "joining thread failed");
      }
    } else if(!top && g_stmode && depth > 0){
      dnum += rtargs[i].dnum;
      wnum += rtargs[i].wnum;
      if(rtargs[i].mtime > mtime) mtime = rtargs[i].mtime;
    }
  }
  hnum = (tmp = cbmapget(hints, "", 0, NULL)) ? atoi(tmp) : 0;
  hnum -= ltarg.mhnum;
  if(hnum < 0) hnum = 0;
  end = page * max + max;
  curtime = est_gettimeofday() - curtime;
  tab = 1;
  axk = 'a';
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
  addservinfo(datum, req->now, UICACHELIFE);
  cbdatumprintf(datum, "Content-Type: %s\r\n", UIMIMETYPE);
  if(req->gzip){
    cbdatumprintf(datum, "Content-Encoding: gzip\r\n");
  } else if(req->deflate){
    cbdatumprintf(datum, "Content-Encoding: deflate\r\n");
  }
  cbdatumprintf(datum, "Content-Disposition: inline; filename=%s.html\r\n", node->name);
  dstr = cbdatestrhttp(req->reload ? req->now : mtime, 0);
  cbdatumprintf(datum, "Last-Modified: %s\r\n", dstr);
  free(dstr);
  cbdatumprintf(datum, "\r\n");
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  cbdatumsetsize(datum, 0);
  cbdatumprintf(datum, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  cbdatumprintf(datum, "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\""
                " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
  cbdatumprintf(datum, "<html xmlns=\"http://www.w3.org/1999/xhtml\""
                " xml:lang=\"en\" lang=\"en\">\n");
  cbdatumprintf(datum, "<head profile=\"http://a9.com/-/spec/opensearch/1.1/\">\n");
  cbdatumprintf(datum, "<meta http-equiv=\"Content-Language\" content=\"en\" />\n");
  cbdatumprintf(datum, "<meta http-equiv=\"Content-Type\" content=\"%@\" />\n", UIMIMETYPE);
  cbdatumprintf(datum, "<meta http-equiv=\"Content-Style-Type\" content=\"text/css\" />\n");
  cbdatumprintf(datum, "<meta http-equiv=\"Content-Script-Type\""
                " content=\"text/javascript\" />\n");
  dstr = cbdatestrhttp(mtime, 0);
  cbdatumprintf(datum, "<meta http-equiv=\"Last-Modified\" content=\"%@\" />\n", dstr);
  free(dstr);
  cbdatumprintf(datum, "<meta name=\"generator\" content=\"%@/%@\" />\n",
                SERVNAME, est_version);
  cbdatumprintf(datum, "<meta name=\"robots\" content=\"NOFOLLOW\" />\n");
  cbdatumprintf(datum, "<link rel=\"contents\" type=\"%@\" href=\"%@%@%@/%@\""
                " title=\"Go to the top page\" />\n",
                UIMIMETYPE, req->prefix, NODEPREFIX, node->name, SEARCHUICMD);
  if(page > 0)
    cbdatumprintf(datum, "<link rel=\"prev\" type=\"%@\""
                  " href=\"%@%@%@/%@?%s&amp;page=%d\" title=\"Go back one page\" />\n",
                  UIMIMETYPE, req->prefix, NODEPREFIX, node->name, SEARCHUICMD,
                  cbdatumptr(condbuf), page - 1);
  if(hnum > end)
    cbdatumprintf(datum, "<link rel=\"next\" type=\"%@\""
                  " href=\"%@%@%@/%@?%s&amp;page=%d\" title=\"Go forward one page\" />\n",
                  UIMIMETYPE, req->prefix, NODEPREFIX, node->name, SEARCHUICMD,
                  cbdatumptr(condbuf), page + 1);
  cbdatumprintf(datum, "<link rel=\"search\" type=\"%@\" href=\"%@%@%@/%@\""
                " title=\"OpenSearch Description 1.1\" />\n",
                OSRCHMIMETYPE, req->prefix, NODEPREFIX, node->name, OPENSEARCHCMD);
  if(!top){
    cbdatumprintf(datum, "<link rel=\"alternate\" type=\"%@\""
                  " href=\"%@%@%@/%@?%s\" title=\"Atom 1.0\" />\n",
                  ATOMMIMETYPE, req->prefix, NODEPREFIX, node->name,
                  SEARCHATOMCMD, cbdatumptr(condbuf));
    cbdatumprintf(datum, "<link rel=\"alternate\" type=\"%@\""
                  " href=\"%@%@%@/%@?%s\" title=\"RSS 1.0\" />\n",
                  RSSMIMETYPE, req->prefix, NODEPREFIX, node->name,
                  SEARCHRSSCMD, cbdatumptr(condbuf));
  }
  cbdatumprintf(datum, "<link rel=\"icon\" href=\"%@%@\" />\n", req->prefix, FAVICONLOC);
  cbdatumprintf(datum, "<link rev=\"made\" href=\"mailto:%@\" />\n", g_adminemail);
  if(phrase && phrase[0] != '\0'){
    cbdatumprintf(datum, "<title>Search %@: %@</title>\n",
                  node->label, simid > 0 ? ESTOPSIMILAR : phrase);
  } else {
    cbdatumprintf(datum, "<title>Search Interface of %@</title>\n", node->label);
  }
  cbdatumprintf(datum, "<style type=\"text/css\">"
                "html { margin: 0em 0em; padding 0em 0em;"
                " background: #eeeeee url(%@%@%@) repeat; }\n",
                req->prefix, IMAGEPREFIX, CANVASNAME);
  cbdatumprintf(datum, "body { margin: 0em 0em; padding: 0.5em 0.8em;"
                " font-style: normal; font-weight: normal; color: #111111; }\n");
  cbdatumprintf(datum, "h1 { font-size: medium; }\n");
  cbdatumprintf(datum, "div.controller { margin: 0.5em 0.3em; padding: 0em 0em; }\n");
  cbdatumprintf(datum, "div.result { margin: 0.5em 0.5em; padding: 0em 0em; }\n");
  cbdatumprintf(datum, "p { color: #555555; }\n");
  cbdatumprintf(datum, "a { color: #0022aa; text-decoration: none; }\n");
  cbdatumprintf(datum, "a:hover,a:focus { color: #0033ee; text-decoration: underline; }\n");
  cbdatumprintf(datum, "table { padding: 1pt 2pt 1pt 2pt; border: none;"
                " margin: 0.3em 0.3em; border-collapse: collapse; }\n");
  cbdatumprintf(datum, "th { padding: 1pt 4pt 1pt 4pt; border-style: none;"
                " text-align: right; vertical-align: top;"
                " font-size: smaller; font-weight: normal; color: #444444; }\n");
  cbdatumprintf(datum, "td { padding: 1pt 4pt 1pt 4pt;"
                " text-align: left; vertical-align: top; }\n");
  cbdatumprintf(datum, "td.conditions { padding-right: 1.5em; vertical-align:top; }\n");
  cbdatumprintf(datum, "td.hints { padding-left: .5em; vertical-align:top; }\n");
  cbdatumprintf(datum, "td.nodeinfo span.hit { font-size: smaller; color: #555555; }\n");
  cbdatumprintf(datum, "dl.doc { margin: 1.3em 0em; }\n");
  cbdatumprintf(datum, "dt.title { margin: 0em 0.5em; }\n");
  cbdatumprintf(datum, "dd.snippet { margin: 0em 1.5em; font-size: smaller;"
                " color: #333333; }\n");
  cbdatumprintf(datum, "dd.attr { margin: 0em 1.3em; font-size: smaller; color: #333333; }\n");
  cbdatumprintf(datum, "dd.guide { margin: 0em 1.2em; font-size: smaller; }\n");
  cbdatumprintf(datum, "a.title { text-decoration: underline; }\n");
  cbdatumprintf(datum, "a.ndlabel { font-size: smaller; }\n");
  cbdatumprintf(datum, "dd.attr span.value { color: #001111; }\n");
  cbdatumprintf(datum, "dd.snippet strong { color: #000000; }\n");
  cbdatumprintf(datum, "dd.snippet code { color: #555555; letter-spacing: -0.1em; }\n");
  cbdatumprintf(datum, "span.uri { color: #007744; }\n");
  cbdatumprintf(datum, "div.paging { margin: 1.5em 1.0em; text-align: right; }\n");
  cbdatumprintf(datum, "a.navi { margin: 0em 0.2em; }\n");
  cbdatumprintf(datum, "span.void { margin: 0em 0.2em; color: #888888; }\n");
  cbdatumprintf(datum, "address.logo { text-align: right;"
                " font-size: smaller; font-style: normal; color: #777777; }\n");
  cbdatumprintf(datum, "address.logo a { color: #777777; }\n");
  cbdatumprintf(datum, "hr { display: none; }\n");
  cbdatumprintf(datum, "</style>\n");
  cbdatumprintf(datum, "<script type=\"text/javascript\">function startup(){\n");
  cbdatumprintf(datum, "  var elem = document.getElementById(\"phrase\");\n");
  cbdatumprintf(datum, "  if(elem) elem.focus();\n");
  cbdatumprintf(datum, "}\n");
  cbdatumprintf(datum, "</script>\n");
  cbdatumprintf(datum, "</head>\n");
  cbdatumprintf(datum, "<body onload=\"startup();\">\n");
  cbdatumprintf(datum, "<h1>Search Interface of %@</h1>\n", node->label);
  cbdatumprintf(datum, "<form method=\"get\" action=\"%@%@%@/%@\" id=\"searchform\">\n",
                req->prefix, NODEPREFIX, node->name, SEARCHUICMD);
  cbdatumprintf(datum, "<div class=\"controller\">\n");
  cbdatumprintf(datum, "<table summary=\"controller\" class=\"controller\">\n");
  cbdatumprintf(datum, "<tr>\n");
  cbdatumprintf(datum, "<td class=\"conditions\">\n");
  cbdatumprintf(datum, "<table summary=\"conditions\">\n");
  cbdatumprintf(datum, "<tr>");
  cbdatumprintf(datum, "<th abbr=\"phrase\">phrase:</th>");
  cbdatumprintf(datum, "<td>");
  cbdatumprintf(datum, "<input type=\"text\" name=\"phrase\" value=\"%@\" size=\"32\""
                " id=\"phrase\" title=\"Input some words for full-text search\""
                " tabindex=\"%d\" accesskey=\"%c\" />", phrase ? phrase : "", tab++, axk++);
  cbdatumprintf(datum, "</td>");
  cbdatumprintf(datum, "</tr>\n");
  for(i = 0; i < 3; i++){
    cbdatumprintf(datum, "<tr>");
    tmp = attrs ? cblistval(attrs, i, NULL) : NULL;
    cbdatumprintf(datum, "<th abbr=\"attr%d\">attribute:</th>", i + 1);
    cbdatumprintf(datum, "<td>");
    cbdatumprintf(datum, "<input type=\"text\" name=\"attr%d\" value=\"%@\" size=\"32\""
                  " id=\"attr%d\" title=\"Input an expression for attribute search\""
                  " tabindex=\"%d\" accesskey=\"%c\" />",
                  i + 1, tmp ? tmp : "", i + 1, tab++, axk++);
    cbdatumprintf(datum, "</td>");
    cbdatumprintf(datum, "</tr>\n");
  }
  cbdatumprintf(datum, "<tr>");
  cbdatumprintf(datum, "<th abbr=\"order\">order:</th>");
  cbdatumprintf(datum, "<td>");
  cbdatumprintf(datum, "<input type=\"text\" name=\"order\" value=\"%@\" size=\"24\""
                " id=\"order\" title=\"Input an ordering expression\""
                " tabindex=\"%d\" accesskey=\"%c\" />", order ? order : "", tab++, axk++);
  cbdatumprintf(datum, "</td>");
  cbdatumprintf(datum, "</tr>\n");
  cbdatumprintf(datum, "<tr>");
  cbdatumprintf(datum, "<th abbr=\"max\">max:</th>");
  cbdatumprintf(datum, "<td>");
  cbdatumprintf(datum, "<select name=\"max\" id=\"max\""
                " title=\"Set the maximum number of shown documents\""
                " tabindex=\"%d\">", tab++);
  for(i = 10; i <= 100; i += 10){
    cbdatumprintf(datum, "<option value=\"%d\"%s>%d</option>",
                  i, max == i ? " selected=\"selected\"" : "", i);
  }
  cbdatumprintf(datum, "</select>");
  cbdatumprintf(datum, "</td>");
  cbdatumprintf(datum, "</tr>\n");
  cbdatumprintf(datum, "<tr>");
  cbdatumprintf(datum, "<th abbr=\"depth\">depth:</th>");
  cbdatumprintf(datum, "<td>");
  cbdatumprintf(datum, "<select name=\"depth\" id=\"depth\""
                " title=\"Set the depth of meta search\" tabindex=\"%d\">", tab++);
  for(i = 0; i <= 3; i++){
    cbdatumprintf(datum, "<option value=\"%d\"%s>%d</option>",
                  i, depth == i ? " selected=\"selected\"" : "", i);
  }
  cbdatumprintf(datum, "</select>");
  cbdatumprintf(datum, "</td>");
  cbdatumprintf(datum, "</tr>\n");
  cbdatumprintf(datum, "<tr>");
  cbdatumprintf(datum, "<th abbr=\"submit\"></th>");
  cbdatumprintf(datum, "<td>");
  cbdatumprintf(datum, "<input type=\"submit\" value=\"search\""
                " title=\"Perform the search\" tabindex=\"%d\" accesskey=\"%c\" />",
                tab++, axk++);
  cbdatumprintf(datum, "</td>");
  cbdatumprintf(datum, "</tr>\n");
  cbdatumprintf(datum, "</table>\n");
  cbdatumprintf(datum, "</td>\n");
  cbdatumprintf(datum, "<td class=\"hints\">\n");
  cbdatumprintf(datum, "<table summary=\"hints\">\n");
  cbdatumprintf(datum, "<tr>");
  cbdatumprintf(datum, "<th abbr=\"hit\">hit:</th>");
  cbdatumprintf(datum, "<td id=\"hit\">%d</td>", hnum);
  cbdatumprintf(datum, "</tr>\n");
  cbmapiterinit(hints);
  num = 1;
  while((kbuf = cbmapiternext(hints, &ksiz)) != NULL){
    if(ksiz < 1) continue;
    cbdatumprintf(datum, "<tr>");
    cbdatumprintf(datum, "<th abbr=\"hint#%d\">hint#%d:</th>", num, num);
    cbdatumprintf(datum, "<td id=\"hint%d\">%@ (%@)</td>",
                  num, kbuf, cbmapiterval(kbuf, NULL));
    cbdatumprintf(datum, "</tr>\n");
    num++;
  }
  cbdatumprintf(datum, "<tr>");
  cbdatumprintf(datum, "<th abbr=\"docnum\">docnum:</th>");
  cbdatumprintf(datum, "<td id=\"docnum\">%d</td>",
                dnum > 0 ? dnum : est_mtdb_doc_num(node->db));
  cbdatumprintf(datum, "</tr>\n");
  cbdatumprintf(datum, "<tr>");
  cbdatumprintf(datum, "<th abbr=\"wordnum\">wordnum:</th>");
  cbdatumprintf(datum, "<td id=\"wordnum\">%d</td>",
                wnum > 0 ? wnum : est_mtdb_word_num(node->db));
  cbdatumprintf(datum, "</tr>\n");
  cbdatumprintf(datum, "<tr>");
  cbdatumprintf(datum, "<th abbr=\"time\">time:</th>");
  cbdatumprintf(datum, "<td id=\"time\">%.6f</td>", curtime / 1000.0);
  cbdatumprintf(datum, "</tr>\n");
  cbdatumprintf(datum, "<tr>");
  cbdatumprintf(datum, "<th abbr=\"link#0\">link#0:</th>");
  cbdatumprintf(datum, "<td id=\"link0\" class=\"nodeinfo\">");
  cbdatumprintf(datum, "<input type=\"checkbox\" name=\"nomask0\" value=\"on\"%s"
                " title=\"Enable Search of Link#0\" />",
                masks[0] ? "" : " checked=\"checked\"");
  cbdatumprintf(datum, " %@", node->label);
  cbdatumprintf(datum, " <span class=\"hit\" title=\"time=%.6f, itime=%.6f\">(%d)</span>",
                ltarg.etime / 1000.0, ltarg.itime / 1000.0, ltarg.hnum - ltarg.mhnum);
  cbdatumprintf(datum, "</td>");
  cbdatumprintf(datum, "</tr>\n");
  for(i = 0; i < cbmaprnum(node->links); i++){
    cbdatumprintf(datum, "<tr>");
    cbdatumprintf(datum, "<th abbr=\"link#%d\">link#%d:</th>", i + 1, i + 1);
    cbdatumprintf(datum, "<td id=\"link%d\" class=\"nodeinfo\">", i + 1);
    cbdatumprintf(datum, "<input type=\"checkbox\" name=\"nomask%d\" value=\"on\"%s"
                  " title=\"Enable Search of Link#%d\" />",
                  i + 1, i < LINKMASKMAX && masks[i+1] ? "" : " checked=\"checked\"", i + 1);
    cbdatumprintf(datum, " <a href=\"%@/%@?%s&amp;nomask=on\" type=\"%@\""
                  " title=\"Jump to the node\">%@</a>",
                  rtargs[i].url, SEARCHUICMD, cbdatumptr(condbuf),
                  UIMIMETYPE, rtargs[i].label);
    if(rtargs[i].hnum >= 0){
      cbdatumprintf(datum, " <span class=\"hit\" title=\"time=%.6f\">(%d)</span>",
                    rtargs[i].etime / 1000.0, rtargs[i].hnum);
    }
    cbdatumprintf(datum, "</td>");
    cbdatumprintf(datum, "</tr>\n");
  }
  cbdatumprintf(datum, "<tr>");
  cbdatumprintf(datum, "<th abbr=\"feed\">feed:</th>");
  if(top){
    cbdatumprintf(datum, "<td id=\"feed\">");
    cbdatumprintf(datum, "<a href=\"%@%@%@/%@\" rel=\"search\""
                  " type=\"%@\" title=\"OpenSearch Description 1.1\">OpenSearch</a>\n",
                  req->prefix, NODEPREFIX, node->name, OPENSEARCHCMD, OSRCHMIMETYPE);
    cbdatumprintf(datum, "</td>");
  } else {
    cbdatumprintf(datum, "<td id=\"feed\">");
    cbdatumprintf(datum, "<a href=\"%@%@%@/%@?%s\" rel=\"alternate\""
                  " type=\"%@\" title=\"Atom 1.0 feed\">Atom</a>", req->prefix, NODEPREFIX,
                  node->name, SEARCHATOMCMD, cbdatumptr(condbuf), ATOMMIMETYPE);
    cbdatumprintf(datum, " <a href=\"%@%@%@/%@?%s\" rel=\"alternate\""
                  " type=\"%@\" title=\"RSS 1.0 feed\">RSS</a>", req->prefix, NODEPREFIX,
                  node->name, SEARCHRSSCMD, cbdatumptr(condbuf), RSSMIMETYPE);
    cbdatumprintf(datum, "</td>");
  }
  cbdatumprintf(datum, "</tr>\n");
  cbdatumprintf(datum, "</table>\n");
  cbdatumprintf(datum, "</td>\n");
  cbdatumprintf(datum, "</tr>\n");
  cbdatumprintf(datum, "</table>\n");
  cbdatumprintf(datum, "<input type=\"hidden\" name=\"allmask\" value=\"on\" />\n");
  cbdatumprintf(datum, "</div>\n");
  cbdatumprintf(datum, "</form>\n");
  cbdatumprintf(datum, "<hr />\n");
  if(!top){
    cbdatumprintf(datum, "<div class=\"result\">\n");
    if(!order && phrase && cbstrfwmatch(phrase, ESTOPSIMILAR))
      order = DATTRNDSCORE " " ESTORDNUMD;
    resdocs = resmap_list(resmap, &rnum, order, NULL);
    for(i = page * max; i < rnum && i < end; i++){
      catdocdataui(datum, i + 1, resdocs[i], ltarg.words, req, node,
                   cbdatumptr(condbuf), cbdatumptr(simcondbuf));
    }
    free(resdocs);
    if(rnum < 1) cbdatumprintf(datum, "<p>The conditions did not match any documents.</p>\n");
    cbdatumprintf(datum, "<div class=\"paging\">");
    if(page > 0){
      cbdatumprintf(datum, "<a href=\"%@%@%@/%@?%s&amp;page=%d\" type=\"%@\" class=\"navi\""
                    " title=\"Go back one page\">PREV</a>",
                    req->prefix, NODEPREFIX, node->name, SEARCHUICMD,
                    cbdatumptr(condbuf), page - 1, UIMIMETYPE);
    } else {
      cbdatumprintf(datum, "<span class=\"void\">PREV</span>");
    }
    if(hnum > end){
      cbdatumprintf(datum, " <a href=\"%@%@%@/%@?%s&amp;page=%d\" type=\"%@\" class=\"navi\""
                    " title=\"Go forward one page\">NEXT</a>",
                    req->prefix, NODEPREFIX, node->name, SEARCHUICMD,
                    cbdatumptr(condbuf), page + 1, UIMIMETYPE);
    } else {
      cbdatumprintf(datum, " <span class=\"void\">NEXT</span>");
    }
    cbdatumprintf(datum, "</div>\n");
    cbdatumprintf(datum, "</div>\n");
    cbdatumprintf(datum, "<hr />\n");
  }
  cbdatumprintf(datum, "<address class=\"logo\">Powered by <a href=\"%@\""
                " title=\"Go to the home page of Hyper Estraier\">Hyper Estraier</a>"
                " %@.</address>\n", _EST_PROJURL, est_version);
  cbdatumprintf(datum, "</body>\n");
  cbdatumprintf(datum, "</html>\n");
  if(req->gzip){
    if((zbuf = est_deflate(cbdatumptr(datum), cbdatumsize(datum), &zsiz, 1)) != NULL){
      est_sock_send_all(clsock, zbuf, zsiz);
      free(zbuf);
    }
  } else if(req->deflate){
    if((zbuf = est_deflate(cbdatumptr(datum), cbdatumsize(datum), &zsiz, 0)) != NULL){
      est_sock_send_all(clsock, zbuf, zsiz);
      free(zbuf);
    }
  } else {
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  }
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: 200 OK (%s)", req->claddr, req->clport, SEARCHUICMD);
  cblistclose(ltarg.words);
  est_cond_delete(ltarg.cond);
  for(i = 0; i < cbmaprnum(node->links); i++){
    free(rtargs[i].label);
    est_cond_delete(rtargs[i].cond);
  }
  free(rtargs);
  free(rths);
  cbmapclose(hints);
  resmap_delete(resmap);
  cbdatumclose(simcondbuf);
  cbdatumclose(condbuf);
  est_cond_delete(cond);
  free(myurl);
}


/* send the result of Atom feed interface */
static void sendnodecmdsearchatom(int clsock, REQUEST *req, NODE *node){
  pthread_t lth, *rths;
  TARGLCSRCH ltarg;
  TARGRMSRCH *rtargs;
  RESMAP *resmap;
  RESDOC **resdocs;
  ESTCOND *cond;
  CBMAP *hints;
  const CBLIST *attrs;
  CBDATUM *condbuf, *datum;
  const char *tmp, *url, *label, *kbuf, *phrase, *order, *simnode;
  char *myurl, *dstr, name[NUMBUFSIZ], masks[LINKMASKMAX+1], *zbuf;
  int i, num, max, depth, simid, page, top, dnum, wnum, hnum, end, ksiz, rnum, zsiz;
  double curtime;
  time_t mtime;
  myurl = cbsprintf("%s%s%s", req->prefix, NODEPREFIX, node->name);
  if(islooproute(myurl, req)){
    log_print(LL_DEBUG, "[%s:%d]: omitting request loop (self): %s",
              req->claddr, req->clport, myurl);
    free(myurl);
    senderror(clsock, req, 400, "Bad Request (the request loops)");
    return;
  }
  if(pthread_mutex_lock(&(node->mutex)) != 0){
    log_print(LL_ERROR, "locking failed");
    senderror(clsock, req, 500, "Internal Server Error (locking failed)");
    return;
  }
  cond = est_cond_new();
  max = DEFMAXSRCH;
  depth = 0;
  page = 0;
  simid = 0;
  simnode = NULL;
  if((tmp = cbmapget(req->params, "page", -1, NULL)) != NULL && (num = atoi(tmp)) > 0)
    page = num;
  if((tmp = cbmapget(req->params, "pageone", -1, NULL)) != NULL && (num = atoi(tmp)) > 1)
    page = num - 1;
  if((tmp = cbmapget(req->params, "phrase", -1, NULL)) != NULL && tmp[0] != '\0')
    est_cond_set_phrase(cond, tmp);
  if((tmp = cbmapget(req->params, "attr", -1, NULL)) != NULL && tmp[0] != '\0')
    est_cond_add_attr(cond, tmp);
  for(i = 0; i <= CONDATTRMAX; i++){
    num = sprintf(name, "attr%d", i);
    if((tmp = cbmapget(req->params, name, num, NULL)) != NULL && tmp[0] != '\0')
      est_cond_add_attr(cond, tmp);
  }
  if((tmp = cbmapget(req->params, "order", -1, NULL)) != NULL && tmp[0] != '\0')
    est_cond_set_order(cond, tmp);
  if((tmp = cbmapget(req->params, "max", -1, NULL)) != NULL && (num = atoi(tmp)) >= 0)
    max = num;
  max = max > g_searchmax ? g_searchmax : max;
  if((tmp = cbmapget(req->params, "depth", -1, NULL)) != NULL && (num = atoi(tmp)) > 0)
    depth = num;
  if((tmp = cbmapget(req->params, "simid", -1, NULL)) != NULL && (num = atoi(tmp)) > 0)
    simid = num;
  if((tmp = cbmapget(req->params, "simnode", -1, NULL)) != NULL && tmp[0] != '\0')
    simnode = tmp;
  if((tmp = cbmapget(req->params, "mask", -1, NULL)) != NULL && tmp[0] != '\0'){
    num = atoi(tmp);
    for(i = 0; i <= LINKMASKMAX; i++){
      masks[i] = num & (1 << i);
    }
  } else if((tmp = cbmapget(req->params, "allmask", -1, NULL)) != NULL && tmp[0] != '\0'){
    memset(masks, 1, LINKMASKMAX+1);
    for(i = 0; i <= LINKMASKMAX; i++){
      num = sprintf(name, "nomask%d", i);
      if((tmp = cbmapget(req->params, name, num, NULL)) != NULL && tmp[0] != '\0') masks[i] = 0;
    }
  } else {
    memset(masks, 0, LINKMASKMAX+1);
    if(!(tmp = cbmapget(req->params, "nomask", -1, NULL)) || tmp[0] == '\0'){
      for(i = 0; i <= LINKMASKMAX; i++){
        num = sprintf(name, "mask%d", i);
        if((tmp = cbmapget(req->params, name, num, NULL)) != NULL && tmp[0] != '\0') masks[i] = 1;
      }
    }
  }
  if(depth >= g_searchdepth) depth = g_searchdepth;
  switch(g_uiphraseform){
  case PM_SIMPLE:
    est_cond_set_options(cond, ESTCONDSIMPLE);
    break;
  case PM_ROUGH:
    est_cond_set_options(cond, ESTCONDROUGH);
    break;
  case PM_UNION:
    est_cond_set_options(cond, ESTCONDUNION);
    break;
  case PM_ISECT:
    est_cond_set_options(cond, ESTCONDISECT);
    break;
  }
  top = !est_cond_phrase(cond) && !est_cond_attrs(cond) && simid < 1;
  phrase = est_cond_phrase(cond);
  attrs = est_cond_attrs(cond);
  order = est_cond_order(cond);
  condbuf = cbdatumopen(NULL, -1);
  cbdatumprintf(condbuf, "phrase=%?", phrase ? phrase : "");
  if(attrs){
    for(i = 0; i < cblistnum(attrs); i++){
      tmp = cblistval(attrs, i, NULL);
      if(tmp[0] != '\0') cbdatumprintf(condbuf, "&amp;attr%d=%?", i + 1, tmp);
    }
  }
  if(order && order[0] != '\0') cbdatumprintf(condbuf, "&amp;order=%?", order);
  if(max != DEFMAXSRCH) cbdatumprintf(condbuf, "&amp;max=%d", max);
  if(depth > 0) cbdatumprintf(condbuf, "&amp;depth=%d", depth);
  if(simid > 0 && simnode && simnode[0] != '\0'){
    cbdatumprintf(condbuf, "&amp;simid=%d", simid);
    cbdatumprintf(condbuf, "&amp;simnode=%?", simnode);
    setsimilarphrase(cond, simnode, simid);
    phrase = est_cond_phrase(cond);
  }
  for(i = 0; i <= LINKMASKMAX; i++){
    if(masks[i]) cbdatumprintf(condbuf, "&amp;mask%d=1", i);
  }
  if(page > 0) cbdatumprintf(condbuf, "&amp;page=%d", page);
  resmap = resmap_new();
  hints = cbmapopenex(MINIBNUM);
  curtime = est_gettimeofday();
  ltarg.req = req;
  ltarg.alive = TRUE;
  ltarg.cond = est_cond_dup(cond);
  ltarg.hints = hints;
  ltarg.max = max + page * max + 1;
  ltarg.node = node;
  ltarg.resmap = resmap;
  ltarg.words = cblistopen();
  ltarg.hnum = 0;
  ltarg.mhnum = 0;
  ltarg.itime = 0.0;
  ltarg.etime = 0.0;
  if(top || masks[0]){
    ltarg.alive = FALSE;
  } else if(g_stmode){
    searchlocal(&ltarg);
    ltarg.alive = FALSE;
  } else if(pthread_create(&lth, NULL, searchlocal, &ltarg) != 0){
    log_print(LL_WARN, "creating thread failed");
    ltarg.alive = FALSE;
  }
  rths = cbmalloc(cbmaprnum(node->links) * sizeof(pthread_t) + 1);
  rtargs = cbmalloc(cbmaprnum(node->links) * sizeof(TARGRMSRCH) + 1);
  cbmapiterinit(node->links);
  for(i = 0; (url = cbmapiternext(node->links, NULL)) != NULL; i++){
    rtargs[i].req = req;
    rtargs[i].alive = TRUE;
    rtargs[i].myurl = myurl;
    rtargs[i].cond = est_cond_dup(cond);
    rtargs[i].hints = hints;
    rtargs[i].max = max + page * max + 1;
    rtargs[i].node = node;
    rtargs[i].resmap = resmap;
    rtargs[i].url = url;
    label = cbmapiterval(url, NULL);
    if((tmp = strchr(label, '\t')) != NULL){
      rtargs[i].label = cbmemdup(label, tmp - label);
      rtargs[i].credit = atoi(tmp + 1);
    } else {
      rtargs[i].label = cbmemdup(label, -1);
      rtargs[i].credit = 0;
    }
    rtargs[i].depth = depth;
    rtargs[i].wwidth = g_snipwwidth;
    rtargs[i].hwidth = g_sniphwidth;
    rtargs[i].awidth = g_snipawidth;
    rtargs[i].hnum = 0;
    rtargs[i].dnum = 0;
    rtargs[i].wnum = 0;
    rtargs[i].etime = 0.0;
    rtargs[i].size = 0.0;
    rtargs[i].mtime = 0;
    if(top || depth < 1 || (i < LINKMASKMAX && masks[i+1])){
      rtargs[i].alive = FALSE;
      rtargs[i].hnum = -1;
      rtargs[i].dnum = -1;
      rtargs[i].wnum = -1;
      rtargs[i].etime = -1.0;
      rtargs[i].size = -1.0;
    } else if(g_stmode){
      searchremote(&rtargs[i]);
      rtargs[i].alive = FALSE;
    } else if(pthread_create(rths + i, NULL, searchremote, rtargs + i) != 0){
      log_print(LL_WARN, "creating thread failed");
      rtargs[i].alive = FALSE;
    }
  }
  if(pthread_mutex_unlock(&(node->mutex)) != 0) log_print(LL_ERROR, "unlocking failed");
  dnum = 0;
  wnum = 0;
  mtime = node->mtime;
  if(ltarg.alive){
    if(pthread_join(lth, NULL) == 0){
      dnum = est_mtdb_doc_num(node->db);
      wnum = est_mtdb_word_num(node->db);
    } else {
      log_print(LL_ERROR, "joining thread failed");
    }
  } else if(!top && g_stmode){
    dnum = est_mtdb_doc_num(node->db);
    wnum = est_mtdb_word_num(node->db);
  }
  for(i = 0; i < cbmaprnum(node->links); i++){
    if(rtargs[i].alive){
      if(pthread_join(rths[i], NULL) == 0){
        dnum += rtargs[i].dnum;
        wnum += rtargs[i].wnum;
        if(rtargs[i].mtime > mtime) mtime = rtargs[i].mtime;
      } else {
        log_print(LL_ERROR, "joining thread failed");
      }
    } else if(!top && g_stmode && depth > 0){
      dnum += rtargs[i].dnum;
      wnum += rtargs[i].wnum;
      if(rtargs[i].mtime > mtime) mtime = rtargs[i].mtime;
    }
  }
  hnum = (tmp = cbmapget(hints, "", 0, NULL)) ? atoi(tmp) : 0;
  hnum -= ltarg.mhnum;
  if(hnum < 0) hnum = 0;
  end = page * max + max;
  curtime = est_gettimeofday() - curtime;
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
  addservinfo(datum, req->now, UICACHELIFE);
  cbdatumprintf(datum, "Content-Type: %s\r\n", ATOMMIMETYPE);
  if(req->gzip){
    cbdatumprintf(datum, "Content-Encoding: gzip\r\n");
  } else if(req->deflate){
    cbdatumprintf(datum, "Content-Encoding: deflate\r\n");
  }
  cbdatumprintf(datum, "Content-Disposition: inline; filename=%s.atom\r\n", node->name);
  dstr = cbdatestrhttp(req->reload ? req->now : mtime, 0);
  cbdatumprintf(datum, "Last-Modified: %s\r\n", dstr);
  free(dstr);
  cbdatumprintf(datum, "\r\n");
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  cbdatumsetsize(datum, 0);
  cbdatumprintf(datum, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  cbdatumprintf(datum, "<feed xmlns=\"http://www.w3.org/2005/Atom\""
                " xmlns:xh=\"http://www.w3.org/1999/xhtml\""
                " xmlns:est=\"%@\""
                " xmlns:os=\"http://a9.com/-/spec/opensearch/1.1/\">\n",
                _EST_XNSEARCH);
  cbdatumprintf(datum, "<id>%@%@%@/%@?%s</id>\n",
                req->prefix, NODEPREFIX, node->name, SEARCHATOMCMD, cbdatumptr(condbuf));
  if(phrase && phrase[0] != '\0'){
    cbdatumprintf(datum, "<title>Search %@: %@</title>\n",
                  node->label, simid > 0 ? ESTOPSIMILAR : phrase);
  } else {
    cbdatumprintf(datum, "<title>Search Interface of %@</title>\n", node->label);
  }
  cbdatumprintf(datum, "<subtitle>Search Result of Hyper Estraier %@</subtitle>\n",
                est_version);
  dstr = cbdatestrwww(mtime, 0);
  cbdatumprintf(datum, "<updated>%@</updated>\n", dstr);
  free(dstr);
  cbdatumprintf(datum, "<link href=\"%@%@%@/%@?%s\""
                " rel=\"self\" type=\"%@\"/>\n",
                req->prefix, NODEPREFIX, node->name, SEARCHATOMCMD,
                cbdatumptr(condbuf), ATOMMIMETYPE);
  if(page > 0)
    cbdatumprintf(datum, "<link href=\"%@%@%@/%@?%s&amp;page=%d\""
                  " rel=\"previous\" type=\"%@\"/>\n",
                  req->prefix, NODEPREFIX, node->name, SEARCHATOMCMD,
                  cbdatumptr(condbuf), page - 1, ATOMMIMETYPE);
  if(hnum > end)
    cbdatumprintf(datum, "<link href=\"%@%@%@/%@?%s&amp;page=%d\""
                  " rel=\"next\" type=\"%@\"/>\n",
                  req->prefix, NODEPREFIX, node->name, SEARCHATOMCMD,
                  cbdatumptr(condbuf), page + 1, ATOMMIMETYPE);
  cbdatumprintf(datum, "<link href=\"%@%@%@/%@?%s\""
                " rel=\"alternate\" type=\"%@\"/>\n",
                req->prefix, NODEPREFIX, node->name, SEARCHUICMD,
                cbdatumptr(condbuf), UIMIMETYPE);
  cbdatumprintf(datum, "<link href=\"%@%@%@/%@?%s\""
                " rel=\"alternate\" type=\"%@\"/>\n",
                req->prefix, NODEPREFIX, node->name, SEARCHRSSCMD,
                cbdatumptr(condbuf), RSSMIMETYPE);
  cbdatumprintf(datum, "<generator uri=\"%@\" version=\"%@\">"
                "Hyper Estraier (%@)</generator>\n", _EST_PROJURL, est_version, SERVNAME);
  cbdatumprintf(datum, "<icon>%@%@%@</icon>\n", req->prefix, IMAGEPREFIX, BIGICONNAME);
  cbdatumprintf(datum, "<est:hit number=\"%d\"/>\n", hnum);
  cbmapiterinit(hints);
  while((kbuf = cbmapiternext(hints, &ksiz)) != NULL){
    if(ksiz < 1) continue;
    cbdatumprintf(datum, "<est:hit key=\"%@\" number=\"%@\"/>\n",
                  kbuf, cbmapiterval(kbuf, NULL));
  }
  cbdatumprintf(datum, "<est:time value=\"%.6f\"/>\n", curtime / 1000.0);
  cbdatumprintf(datum, "<est:total docnum=\"%d\" wordnum=\"%d\"/>\n",
                dnum > 0 ? dnum : est_mtdb_doc_num(node->db),
                wnum > 0 ? wnum : est_mtdb_word_num(node->db));
  cbdatumprintf(datum, "<est:link url=\"%@%@%@\" label=\"%@\""
                " credit=\"%d\" docnum=\"%d\" wordnum=\"%d\" size=\"%.0f\""
                " hit=\"%d\" time=\"%.6f\"/>\n",
                req->prefix, NODEPREFIX, node->name,
                node->label, SELFCREDIT, est_mtdb_doc_num(node->db),
                est_mtdb_word_num(node->db), est_mtdb_size(node->db),
                ltarg.hnum - ltarg.mhnum, ltarg.etime >= 0.0 ? ltarg.etime / 1000.0 : 0.0);
  for(i = 0; i < cbmaprnum(node->links); i++){
    cbdatumprintf(datum, "<est:link url=\"%@\" label=\"%@\""
                  " credit=\"%d\" docnum=\"%d\" wordnum=\"%d\" size=\"%.0f\""
                  " hit=\"%d\" time=\"%.6f\"/>\n",
                  rtargs[i].url, rtargs[i].label, rtargs[i].credit,
                  rtargs[i].dnum, rtargs[i].wnum, rtargs[i].size,
                  rtargs[i].hnum, rtargs[i].etime >= 0.0 ? rtargs[i].etime / 1000.0 : 0.0);
  }
  cbdatumprintf(datum, "<os:Query role=\"request\" searchTerms=\"%@\"/>\n",
                phrase ? phrase : "");
  cbdatumprintf(datum, "<os:totalResults>%d</os:totalResults>\n", hnum);
  cbdatumprintf(datum, "<os:startIndex>%d</os:startIndex>\n", page * max + 1);
  cbdatumprintf(datum, "<os:itemsPerPage>%d</os:itemsPerPage>\n", max);
  cbdatumprintf(datum, "<os:link href=\"%@%@%@/%@\" rel=\"search\" type=\"%@\"/>\n",
                req->prefix, NODEPREFIX, node->name, OPENSEARCHCMD, OSRCHMIMETYPE);
  if(!top){
    if(!order && phrase && cbstrfwmatch(phrase, ESTOPSIMILAR))
      order = DATTRNDSCORE " " ESTORDNUMD;
    resdocs = resmap_list(resmap, &rnum, order, NULL);
    for(i = page * max; i < rnum && i < end; i++){
      catdocdataatom(datum, resdocs[i], ltarg.words, node);
    }
    free(resdocs);
  }
  cbdatumprintf(datum, "</feed>\n");
  if(req->gzip){
    if((zbuf = est_deflate(cbdatumptr(datum), cbdatumsize(datum), &zsiz, 1)) != NULL){
      est_sock_send_all(clsock, zbuf, zsiz);
      free(zbuf);
    }
  } else if(req->deflate){
    if((zbuf = est_deflate(cbdatumptr(datum), cbdatumsize(datum), &zsiz, 0)) != NULL){
      est_sock_send_all(clsock, zbuf, zsiz);
      free(zbuf);
    }
  } else {
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  }
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: 200 OK (%s)", req->claddr, req->clport, SEARCHATOMCMD);
  cblistclose(ltarg.words);
  est_cond_delete(ltarg.cond);
  for(i = 0; i < cbmaprnum(node->links); i++){
    free(rtargs[i].label);
    est_cond_delete(rtargs[i].cond);
  }
  free(rtargs);
  free(rths);
  cbmapclose(hints);
  resmap_delete(resmap);
  cbdatumclose(condbuf);
  est_cond_delete(cond);
  free(myurl);
}


/* send the result of RSS feed interface */
static void sendnodecmdsearchrss(int clsock, REQUEST *req, NODE *node){
  pthread_t lth, *rths;
  TARGLCSRCH ltarg;
  TARGRMSRCH *rtargs;
  RESMAP *resmap;
  RESDOC **resdocs, *resdoc;
  ESTCOND *cond;
  CBMAP *hints;
  const CBLIST *attrs;
  CBDATUM *condbuf, *datum;
  const char *tmp, *url, *label, *kbuf, *phrase, *order, *simnode;
  char *myurl, name[NUMBUFSIZ], masks[LINKMASKMAX+1], *dstr, *turi, *zbuf;
  int i, num, max, depth, simid, page, top, dnum, wnum, hnum, end, ksiz, rnum, zsiz;
  double curtime;
  time_t mtime;
  myurl = cbsprintf("%s%s%s", req->prefix, NODEPREFIX, node->name);
  if(islooproute(myurl, req)){
    log_print(LL_DEBUG, "[%s:%d]: omitting request loop (self): %s",
              req->claddr, req->clport, myurl);
    free(myurl);
    senderror(clsock, req, 400, "Bad Request (the request loops)");
    return;
  }
  if(pthread_mutex_lock(&(node->mutex)) != 0){
    log_print(LL_ERROR, "locking failed");
    senderror(clsock, req, 500, "Internal Server Error (locking failed)");
    return;
  }
  cond = est_cond_new();
  max = DEFMAXSRCH;
  depth = 0;
  page = 0;
  simid = 0;
  simnode = NULL;
  if((tmp = cbmapget(req->params, "page", -1, NULL)) != NULL && (num = atoi(tmp)) > 0)
    page = num;
  if((tmp = cbmapget(req->params, "pageone", -1, NULL)) != NULL && (num = atoi(tmp)) > 1)
    page = num - 1;
  if((tmp = cbmapget(req->params, "phrase", -1, NULL)) != NULL && tmp[0] != '\0')
    est_cond_set_phrase(cond, tmp);
  if((tmp = cbmapget(req->params, "attr", -1, NULL)) != NULL && tmp[0] != '\0')
    est_cond_add_attr(cond, tmp);
  for(i = 0; i <= CONDATTRMAX; i++){
    num = sprintf(name, "attr%d", i);
    if((tmp = cbmapget(req->params, name, num, NULL)) != NULL && tmp[0] != '\0')
      est_cond_add_attr(cond, tmp);
  }
  if((tmp = cbmapget(req->params, "order", -1, NULL)) != NULL && tmp[0] != '\0')
    est_cond_set_order(cond, tmp);
  if((tmp = cbmapget(req->params, "max", -1, NULL)) != NULL && (num = atoi(tmp)) >= 0)
    max = num;
  max = max > g_searchmax ? g_searchmax : max;
  if((tmp = cbmapget(req->params, "depth", -1, NULL)) != NULL && (num = atoi(tmp)) > 0)
    depth = num;
  if((tmp = cbmapget(req->params, "simid", -1, NULL)) != NULL && (num = atoi(tmp)) > 0)
    simid = num;
  if((tmp = cbmapget(req->params, "simnode", -1, NULL)) != NULL && tmp[0] != '\0')
    simnode = tmp;
  if((tmp = cbmapget(req->params, "mask", -1, NULL)) != NULL && tmp[0] != '\0'){
    num = atoi(tmp);
    for(i = 0; i <= LINKMASKMAX; i++){
      masks[i] = num & (1 << i);
    }
  } else if((tmp = cbmapget(req->params, "allmask", -1, NULL)) != NULL && tmp[0] != '\0'){
    memset(masks, 1, LINKMASKMAX+1);
    for(i = 0; i <= LINKMASKMAX; i++){
      num = sprintf(name, "nomask%d", i);
      if((tmp = cbmapget(req->params, name, num, NULL)) != NULL && tmp[0] != '\0') masks[i] = 0;
    }
  } else {
    memset(masks, 0, LINKMASKMAX+1);
    if(!(tmp = cbmapget(req->params, "nomask", -1, NULL)) || tmp[0] == '\0'){
      for(i = 0; i <= LINKMASKMAX; i++){
        num = sprintf(name, "mask%d", i);
        if((tmp = cbmapget(req->params, name, num, NULL)) != NULL && tmp[0] != '\0') masks[i] = 1;
      }
    }
  }
  if(depth >= g_searchdepth) depth = g_searchdepth;
  switch(g_uiphraseform){
  case PM_SIMPLE:
    est_cond_set_options(cond, ESTCONDSIMPLE);
    break;
  case PM_ROUGH:
    est_cond_set_options(cond, ESTCONDROUGH);
    break;
  case PM_UNION:
    est_cond_set_options(cond, ESTCONDUNION);
    break;
  case PM_ISECT:
    est_cond_set_options(cond, ESTCONDISECT);
    break;
  }
  top = !est_cond_phrase(cond) && !est_cond_attrs(cond) && simid < 1;
  phrase = est_cond_phrase(cond);
  attrs = est_cond_attrs(cond);
  order = est_cond_order(cond);
  condbuf = cbdatumopen(NULL, -1);
  cbdatumprintf(condbuf, "phrase=%?", phrase ? phrase : "");
  if(attrs){
    for(i = 0; i < cblistnum(attrs); i++){
      tmp = cblistval(attrs, i, NULL);
      if(tmp[0] != '\0') cbdatumprintf(condbuf, "&amp;attr%d=%?", i + 1, tmp);
    }
  }
  if(order && order[0] != '\0') cbdatumprintf(condbuf, "&amp;order=%?", order);
  if(max != DEFMAXSRCH) cbdatumprintf(condbuf, "&amp;max=%d", max);
  if(depth > 0) cbdatumprintf(condbuf, "&amp;depth=%d", depth);
  if(simid > 0 && simnode && simnode[0] != '\0'){
    cbdatumprintf(condbuf, "&amp;simid=%d", simid);
    cbdatumprintf(condbuf, "&amp;simnode=%?", simnode);
    setsimilarphrase(cond, simnode, simid);
    phrase = est_cond_phrase(cond);
  }
  for(i = 0; i <= LINKMASKMAX; i++){
    if(masks[i]) cbdatumprintf(condbuf, "&amp;mask%d=1", i);
  }
  if(page > 0) cbdatumprintf(condbuf, "&amp;page=%d", page);
  resmap = resmap_new();
  hints = cbmapopenex(MINIBNUM);
  curtime = est_gettimeofday();
  ltarg.req = req;
  ltarg.alive = TRUE;
  ltarg.cond = est_cond_dup(cond);
  ltarg.hints = hints;
  ltarg.max = max + page * max + 1;
  ltarg.node = node;
  ltarg.resmap = resmap;
  ltarg.words = cblistopen();
  ltarg.hnum = 0;
  ltarg.mhnum = 0;
  ltarg.itime = 0.0;
  ltarg.etime = 0.0;
  if(top || masks[0]){
    ltarg.alive = FALSE;
  } else if(g_stmode){
    searchlocal(&ltarg);
    ltarg.alive = FALSE;
  } else if(pthread_create(&lth, NULL, searchlocal, &ltarg) != 0){
    log_print(LL_WARN, "creating thread failed");
    ltarg.alive = FALSE;
  }
  rths = cbmalloc(cbmaprnum(node->links) * sizeof(pthread_t) + 1);
  rtargs = cbmalloc(cbmaprnum(node->links) * sizeof(TARGRMSRCH) + 1);
  cbmapiterinit(node->links);
  for(i = 0; (url = cbmapiternext(node->links, NULL)) != NULL; i++){
    rtargs[i].req = req;
    rtargs[i].alive = TRUE;
    rtargs[i].myurl = myurl;
    rtargs[i].cond = est_cond_dup(cond);
    rtargs[i].hints = hints;
    rtargs[i].max = max + page * max + 1;
    rtargs[i].node = node;
    rtargs[i].resmap = resmap;
    rtargs[i].url = url;
    label = cbmapiterval(url, NULL);
    if((tmp = strchr(label, '\t')) != NULL){
      rtargs[i].label = cbmemdup(label, tmp - label);
      rtargs[i].credit = atoi(tmp + 1);
    } else {
      rtargs[i].label = cbmemdup(label, -1);
      rtargs[i].credit = 0;
    }
    rtargs[i].depth = depth;
    rtargs[i].wwidth = g_snipwwidth;
    rtargs[i].hwidth = g_sniphwidth;
    rtargs[i].awidth = g_snipawidth;
    rtargs[i].hnum = 0;
    rtargs[i].dnum = 0;
    rtargs[i].wnum = 0;
    rtargs[i].etime = 0.0;
    rtargs[i].size = 0.0;
    rtargs[i].mtime = 0;
    if(top || depth < 1 || (i < LINKMASKMAX && masks[i+1])){
      rtargs[i].alive = FALSE;
      rtargs[i].hnum = -1;
      rtargs[i].dnum = -1;
      rtargs[i].wnum = -1;
      rtargs[i].etime = -1.0;
      rtargs[i].size = -1.0;
    } else if(g_stmode){
      searchremote(&rtargs[i]);
      rtargs[i].alive = FALSE;
    } else if(pthread_create(rths + i, NULL, searchremote, rtargs + i) != 0){
      log_print(LL_WARN, "creating thread failed");
      rtargs[i].alive = FALSE;
    }
  }
  if(pthread_mutex_unlock(&(node->mutex)) != 0) log_print(LL_ERROR, "unlocking failed");
  dnum = 0;
  wnum = 0;
  mtime = node->mtime;
  if(ltarg.alive){
    if(pthread_join(lth, NULL) == 0){
      dnum = est_mtdb_doc_num(node->db);
      wnum = est_mtdb_word_num(node->db);
    } else {
      log_print(LL_ERROR, "joining thread failed");
    }
  } else if(!top && g_stmode){
    dnum = est_mtdb_doc_num(node->db);
    wnum = est_mtdb_word_num(node->db);
  }
  for(i = 0; i < cbmaprnum(node->links); i++){
    if(rtargs[i].alive){
      if(pthread_join(rths[i], NULL) == 0){
        dnum += rtargs[i].dnum;
        wnum += rtargs[i].wnum;
        if(rtargs[i].mtime > mtime) mtime = rtargs[i].mtime;
      } else {
        log_print(LL_ERROR, "joining thread failed");
      }
    } else if(!top && g_stmode && depth > 0){
      dnum += rtargs[i].dnum;
      wnum += rtargs[i].wnum;
      if(rtargs[i].mtime > mtime) mtime = rtargs[i].mtime;
    }
  }
  hnum = (tmp = cbmapget(hints, "", 0, NULL)) ? atoi(tmp) : 0;
  hnum -= ltarg.mhnum;
  if(hnum < 0) hnum = 0;
  end = page * max + max;
  curtime = est_gettimeofday() - curtime;
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
  addservinfo(datum, req->now, UICACHELIFE);
  cbdatumprintf(datum, "Content-Type: %s\r\n", RSSMIMETYPE);
  if(req->gzip){
    cbdatumprintf(datum, "Content-Encoding: gzip\r\n");
  } else if(req->deflate){
    cbdatumprintf(datum, "Content-Encoding: deflate\r\n");
  }
  cbdatumprintf(datum, "Content-Disposition: inline; filename=%s.rdf\r\n", node->name);
  dstr = cbdatestrhttp(req->reload ? req->now : mtime, 0);
  cbdatumprintf(datum, "Last-Modified: %s\r\n", dstr);
  free(dstr);
  cbdatumprintf(datum, "\r\n");
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  cbdatumsetsize(datum, 0);
  cbdatumprintf(datum, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  cbdatumprintf(datum, "<rdf:RDF xmlns=\"http://purl.org/rss/1.0/\""
                " xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\""
                " xmlns:dc=\"http://purl.org/dc/elements/1.1/\""
                " xmlns:atom=\"http://www.w3.org/2005/Atom\""
                " xmlns:est=\"%@\""
                " xmlns:os=\"http://a9.com/-/spec/opensearch/1.1/\">\n",
                _EST_XNSEARCH);
  cbdatumprintf(datum, "<channel rdf:about=\"%@%@%@/%@?%s\">\n",
                req->prefix, NODEPREFIX, node->name, SEARCHRSSCMD, cbdatumptr(condbuf));
  if(phrase && phrase[0] != '\0'){
    cbdatumprintf(datum, "<title>Search %@: %@</title>\n",
                  node->label, simid > 0 ? ESTOPSIMILAR : phrase);
  } else {
    cbdatumprintf(datum, "<title>Search Interface of %@</title>\n", node->label);
  }
  cbdatumprintf(datum, "<description>Search Result of Hyper Estraier %@</description>\n",
                est_version);
  cbdatumprintf(datum, "<link>%@%@%@/%@?%s</link>\n",
                req->prefix, NODEPREFIX, node->name, SEARCHUICMD, cbdatumptr(condbuf));
  if(!top){
    if(!order && phrase && cbstrfwmatch(phrase, ESTOPSIMILAR))
      order = DATTRNDSCORE " " ESTORDNUMD;
    resdocs = resmap_list(resmap, &rnum, order, NULL);
    cbdatumprintf(datum, "<items>\n");
    cbdatumprintf(datum, "<rdf:Seq>\n");
    for(i = page * max; i < rnum && i < end; i++){
      resdoc = resdocs[i];

      if(!(tmp = resdoc->doc ? est_doc_attr(resdoc->doc, ESTDATTRURI) :
           cbmapget(resdoc->attrs, ESTDATTRURI, -1, NULL))) tmp = ".";
      turi = makeshownuri(tmp);
      cbdatumprintf(datum, "<rdf:li rdf:resource=\"%@\"/>\n", turi);
      free(turi);
    }
    cbdatumprintf(datum, "</rdf:Seq>\n");
    cbdatumprintf(datum, "</items>\n");
    free(resdocs);
  }
  dstr = cbdatestrwww(mtime, 0);
  cbdatumprintf(datum, "<dc:date>%@</dc:date>\n", dstr);
  free(dstr);
  cbdatumprintf(datum, "<atom:link href=\"%@%@%@/%@?%s\""
                " rel=\"self\" type=\"%@\"/>\n",
                req->prefix, NODEPREFIX, node->name, SEARCHRSSCMD,
                cbdatumptr(condbuf), RSSMIMETYPE);
  if(page > 0)
    cbdatumprintf(datum, "<atom:link href=\"%@%@%@/%@?%s&amp;page=%d\""
                  " rel=\"previous\" type=\"%@\"/>\n",
                  req->prefix, NODEPREFIX, node->name, SEARCHATOMCMD,
                  cbdatumptr(condbuf), page - 1, ATOMMIMETYPE);
  if(hnum > end)
    cbdatumprintf(datum, "<atom:link href=\"%@%@%@/%@?%s&amp;page=%d\""
                  " rel=\"next\" type=\"%@\"/>\n",
                  req->prefix, NODEPREFIX, node->name, SEARCHATOMCMD,
                  cbdatumptr(condbuf), page + 1, ATOMMIMETYPE);
  cbdatumprintf(datum, "<atom:link href=\"%@%@%@/%@?%s\""
                " rel=\"alternate\" type=\"%@\"/>\n",
                req->prefix, NODEPREFIX, node->name, SEARCHUICMD,
                cbdatumptr(condbuf), UIMIMETYPE);
  cbdatumprintf(datum, "<atom:link href=\"%@%@%@/%@?%s\""
                " rel=\"alternate\" type=\"%@\"/>\n",
                req->prefix, NODEPREFIX, node->name, SEARCHATOMCMD,
                cbdatumptr(condbuf), ATOMMIMETYPE);
  cbdatumprintf(datum, "<est:hit number=\"%d\"/>\n", hnum);
  cbmapiterinit(hints);
  while((kbuf = cbmapiternext(hints, &ksiz)) != NULL){
    if(ksiz < 1) continue;
    cbdatumprintf(datum, "<est:hit key=\"%@\" number=\"%@\"/>\n",
                  kbuf, cbmapiterval(kbuf, NULL));
  }
  cbdatumprintf(datum, "<est:time value=\"%.6f\"/>\n", curtime / 1000.0);
  cbdatumprintf(datum, "<est:total docnum=\"%d\" wordnum=\"%d\"/>\n",
                dnum > 0 ? dnum : est_mtdb_doc_num(node->db),
                wnum > 0 ? wnum : est_mtdb_word_num(node->db));
  cbdatumprintf(datum, "<est:link url=\"%@%@%@\" label=\"%@\""
                " credit=\"%d\" docnum=\"%d\" wordnum=\"%d\" size=\"%.0f\""
                " hit=\"%d\" time=\"%.6f\"/>\n",
                req->prefix, NODEPREFIX, node->name,
                node->label, SELFCREDIT, est_mtdb_doc_num(node->db),
                est_mtdb_word_num(node->db), est_mtdb_size(node->db),
                ltarg.hnum - ltarg.mhnum, ltarg.etime >= 0.0 ? ltarg.etime / 1000.0 : 0.0);
  for(i = 0; i < cbmaprnum(node->links); i++){
    cbdatumprintf(datum, "<est:link url=\"%@\" label=\"%@\""
                  " credit=\"%d\" docnum=\"%d\" wordnum=\"%d\" size=\"%.0f\""
                  " hit=\"%d\" time=\"%.6f\"/>\n",
                  rtargs[i].url, rtargs[i].label, rtargs[i].credit,
                  rtargs[i].dnum, rtargs[i].wnum, rtargs[i].size,
                  rtargs[i].hnum, rtargs[i].etime >= 0.0 ? rtargs[i].etime / 1000.0 : 0.0);
  }
  cbdatumprintf(datum, "<os:Query role=\"request\" searchTerms=\"%@\"/>\n",
                phrase ? phrase : "");
  cbdatumprintf(datum, "<os:totalResults>%d</os:totalResults>\n", hnum);
  cbdatumprintf(datum, "<os:startIndex>%d</os:startIndex>\n", page * max + 1);
  cbdatumprintf(datum, "<os:itemsPerPage>%d</os:itemsPerPage>\n", max);
  cbdatumprintf(datum, "<os:link href=\"%@%@%@/%@\" rel=\"search\" type=\"%@\"/>\n",
                req->prefix, NODEPREFIX, node->name, OPENSEARCHCMD, OSRCHMIMETYPE);
  cbdatumprintf(datum, "</channel>\n");
  if(!top){
    if(!order && phrase && cbstrfwmatch(phrase, ESTOPSIMILAR))
      order = DATTRNDSCORE " " ESTORDNUMD;
    resdocs = resmap_list(resmap, &rnum, order, NULL);
    for(i = page * max; i < rnum && i < end; i++){
      catdocdatarss(datum, resdocs[i], ltarg.words, node);
    }
    free(resdocs);
  }
  cbdatumprintf(datum, "</rdf:RDF>\n");
  if(req->gzip){
    if((zbuf = est_deflate(cbdatumptr(datum), cbdatumsize(datum), &zsiz, 1)) != NULL){
      est_sock_send_all(clsock, zbuf, zsiz);
      free(zbuf);
    }
  } else if(req->deflate){
    if((zbuf = est_deflate(cbdatumptr(datum), cbdatumsize(datum), &zsiz, 0)) != NULL){
      est_sock_send_all(clsock, zbuf, zsiz);
      free(zbuf);
    }
  } else {
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  }
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: 200 OK (%s)", req->claddr, req->clport, SEARCHRSSCMD);
  cblistclose(ltarg.words);
  est_cond_delete(ltarg.cond);
  for(i = 0; i < cbmaprnum(node->links); i++){
    free(rtargs[i].label);
    est_cond_delete(rtargs[i].cond);
  }
  free(rtargs);
  free(rths);
  cbmapclose(hints);
  resmap_delete(resmap);
  cbdatumclose(condbuf);
  est_cond_delete(cond);
  free(myurl);
}


/* send the result of OpenSearch description */
static void sendnodecmdopensearch(int clsock, REQUEST *req, NODE *node){
  CBDATUM *datum;
  char *dstr, *zbuf;
  int zsiz;
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
  addservinfo(datum, req->now, UICACHELIFE);
  cbdatumprintf(datum, "Content-Type: %s\r\n", OSRCHMIMETYPE);
  if(req->gzip){
    cbdatumprintf(datum, "Content-Encoding: gzip\r\n");
  } else if(req->deflate){
    cbdatumprintf(datum, "Content-Encoding: deflate\r\n");
  }
  cbdatumprintf(datum, "Content-Disposition: inline; filename=%s.xml\r\n", node->name);
  dstr = cbdatestrhttp(node->mtime, 0);
  cbdatumprintf(datum, "Last-Modified: %s\r\n", dstr);
  free(dstr);
  cbdatumprintf(datum, "\r\n");
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  cbdatumsetsize(datum, 0);
  cbdatumprintf(datum, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  cbdatumprintf(datum, "<OpenSearchDescription"
                " xmlns=\"http://a9.com/-/spec/opensearch/1.1/\""
                " xmlns:est=\"%@\">\n", _EST_XNNODE);
  cbdatumprintf(datum, "<ShortName>%@</ShortName>\n", node->name);
  cbdatumprintf(datum, "<Description>%@</Description>\n", node->label);
  cbdatumprintf(datum, "<Tags>Hyper Estraier %@ %@ %@</Tags>\n",
                SERVNAME, node->name, node->label);
  cbdatumprintf(datum, "<Contact>%@</Contact>\n", g_adminemail);
  cbdatumprintf(datum, "<Url type=\"%@\" template=\"%@%@%@/%@"
                "?phrase={searchTerms}&amp;max={count}&amp;pageone={startPage}\"/>\n",
                ATOMMIMETYPE, req->prefix, NODEPREFIX, node->name, SEARCHATOMCMD);
  cbdatumprintf(datum, "<Url type=\"%@\" template=\"%@%@%@/%@"
                "?phrase={searchTerms}&amp;max={count}&amp;pageone={startPage}\"/>\n",
                UIMIMETYPE, req->prefix, NODEPREFIX, node->name, SEARCHUICMD);
  cbdatumprintf(datum, "<Image height=\"16\" width=\"16\" type=\"image/png\">"
                "%@%@</Image>\n", req->prefix, FAVICONLOC);
  cbdatumprintf(datum, "<Image height=\"64\" width=\"64\" type=\"image/png\">"
                "%@%@%@</Image>\n", req->prefix, IMAGEPREFIX, BIGICONNAME);
  switch(g_uiphraseform){
  default:
    cbdatumprintf(datum, "<Query role=\"example\" searchTerms=\"%@\"/>\n",
                  "search AND hyperestraier OR hyperestraier");
    break;
  case PM_SIMPLE:
  case PM_ROUGH:
    cbdatumprintf(datum, "<Query role=\"example\" searchTerms=\"%@\"/>\n",
                  "search \"hyper estraier\" | hyperestraier");
    break;
  case PM_UNION:
  case PM_ISECT:
    cbdatumprintf(datum, "<Query role=\"example\" searchTerms=\"%@\"/>\n",
                  "hop step jump");
    break;
  }
  cbdatumprintf(datum, "<est:docnum>%d</est:docnum>\n", est_mtdb_doc_num(node->db));
  cbdatumprintf(datum, "<est:wordnum>%d</est:wordnum>\n", est_mtdb_word_num(node->db));
  cbdatumprintf(datum, "<est:size>%.0f</est:size>\n", est_mtdb_size(node->db));
  cbdatumprintf(datum, "</OpenSearchDescription>\n");
  if(req->gzip){
    if((zbuf = est_deflate(cbdatumptr(datum), cbdatumsize(datum), &zsiz, 1)) != NULL){
      est_sock_send_all(clsock, zbuf, zsiz);
      free(zbuf);
    }
  } else if(req->deflate){
    if((zbuf = est_deflate(cbdatumptr(datum), cbdatumsize(datum), &zsiz, 0)) != NULL){
      est_sock_send_all(clsock, zbuf, zsiz);
      free(zbuf);
    }
  } else {
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  }
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: 200 OK (opensearch)", req->claddr, req->clport);
}


/* send administration interface data */
static void sendmasteruidata(int clsock, REQUEST *req, USER *user){
  CBDATUM *datum;
  CBLIST *list, *words;
  USER *tuser;
  NODE *tnode;
  const char *tmp, *name, *passwd, *flags, *fname, *misc, *label, *admins, *users, *links;
  char *pbuf, *pv, *nv;
  int i, action, sure, vml, vul, vnl, vne, vns, size, dnum, msiz;
  double fsiz;
  time_t rsec;
  action = UI_NONE;
  if((tmp = cbmapget(req->params, "action", -1, NULL)) != NULL)
    action = atoi(tmp);
  sure = FALSE;
  if((tmp = cbmapget(req->params, "sure", -1, NULL)) != NULL)
    sure = atoi(tmp) > 0;
  if(!(name = cbmapget(req->params, "name", -1, NULL))) name = "";
  if(!(passwd = cbmapget(req->params, "passwd", -1, NULL))) passwd = "";
  if(!(flags = cbmapget(req->params, "flags", -1, NULL))) flags = "";
  if(!(fname = cbmapget(req->params, "fname", -1, NULL))) fname = "";
  if(!(misc = cbmapget(req->params, "misc", -1, NULL))) misc = "";
  if(!(label = cbmapget(req->params, "label", -1, NULL))) label = "";
  admins = cbmapget(req->params, "admins", -1, NULL);
  users = cbmapget(req->params, "users", -1, NULL);
  links = cbmapget(req->params, "links", -1, NULL);
  if(!rwlock_lock(g_mgrlock, TRUE)){
    log_print(LL_ERROR, "locking failed");
    senderror(clsock, req, 500, "Internal Server Error (locking failed)");
    return;
  }
  if(req->referer && !cbstrfwmatch(req->referer, req->prefix) &&
     action != UI_VIEWMASTER && action != UI_VIEWUSERS &&
     action != UI_VIEWNODES && action != UI_STATNODE && action != UI_NONE){
    senderror(clsock, req, 403, "Forbidden (invalid referrer)");
    if(!rwlock_unlock(g_mgrlock)) log_print(LL_ERROR, "unlocking failed");
    return;
  }
  if(!ismasteradmin(req, user)){
    sendautherror(clsock, req, "Super User");
    if(!rwlock_unlock(g_mgrlock)) log_print(LL_ERROR, "unlocking failed");
    return;
  }
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
  addservinfo(datum, req->now, 0);
  cbdatumprintf(datum, "Content-Type: %s\r\n", UIMIMETYPE);
  cbdatumprintf(datum, "\r\n");
  cbdatumprintf(datum, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  cbdatumprintf(datum, "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\""
                " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
  cbdatumprintf(datum, "<html xmlns=\"http://www.w3.org/1999/xhtml\""
                " xml:lang=\"en\" lang=\"en\">\n");
  cbdatumprintf(datum, "<head>\n");
  cbdatumprintf(datum, "<meta http-equiv=\"Content-Language\" content=\"en\" />\n");
  cbdatumprintf(datum, "<meta http-equiv=\"Content-Type\" content=\"%s\" />\n", UIMIMETYPE);
  cbdatumprintf(datum, "<meta http-equiv=\"Content-Style-Type\" content=\"text/css\" />\n");
  cbdatumprintf(datum, "<meta name=\"generator\" content=\"%@/%@\" />\n",
                SERVNAME, est_version);
  cbdatumprintf(datum, "<meta name=\"robots\" content=\"NOFOLLOW\" />\n");
  cbdatumprintf(datum, "<link rel=\"contents\" href=\"%@%@\" />\n", req->prefix, MASTERUILOC);
  cbdatumprintf(datum, "<link rel=\"icon\" href=\"%@%@\" />\n", req->prefix, FAVICONLOC);
  cbdatumprintf(datum, "<link rev=\"made\" href=\"mailto:%@\" />\n", g_adminemail);
  cbdatumprintf(datum, "<title>Administration Interface of Hyper Estraier</title>\n");
  cbdatumprintf(datum, "<style type=\"text/css\">"
                "html { margin: 0em 0em; padding 0em 0em;"
                " background: #eeeeee none; }\n");
  cbdatumprintf(datum, "body { margin: 0em 0em; padding: 1em 1em;"
                " font-style: normal; font-weight: normal; color: #111111; }\n");
  cbdatumprintf(datum, "h1 { font-size: large; }\n");
  cbdatumprintf(datum, "a { color: #0022aa; text-decoration: none; }\n");
  cbdatumprintf(datum, "a:hover,a:focus { color: #0033ee; text-decoration: underline; }\n");
  cbdatumprintf(datum, "table { padding: 1pt 2pt 1pt 2pt; border: none;"
                " margin: 1.5em 1.5em; border-collapse: collapse; }\n");
  cbdatumprintf(datum, "th { padding: 1pt 4pt 1pt 4pt; border-style: none;"
                " text-align: left; vertical-align: bottom; font-size: smaller; }\n");
  cbdatumprintf(datum, "td { padding: 1pt 4pt 1pt 4pt; border: 1pt solid #555555;"
                " text-align: left; vertical-align: top; }\n");
  cbdatumprintf(datum, "div.form { margin: 1.0em 1.5em; }\n");
  cbdatumprintf(datum, "address.logo { text-align: right;"
                " font-size: smaller; font-style: normal; color: #777777; }\n");
  cbdatumprintf(datum, "address.logo a { color: #777777; }\n");
  cbdatumprintf(datum, "div.hidden { display: none; }\n");
  cbdatumprintf(datum, "dl.tlist { margin: 1em 1.5em; }\n");
  cbdatumprintf(datum, "dl.tlist dt { font-size: smaller; font-weight: bold; }\n");
  cbdatumprintf(datum, "hr { margin-top: 1.2em; margin-bottom: 1.2em; height: 1pt;"
                " color: #bbbbbb; background-color: #bbbbbb; border: none; }\n");
  cbdatumprintf(datum, "</style>\n");
  cbdatumprintf(datum, "</head>\n");
  cbdatumprintf(datum, "<body>\n");
  cbdatumprintf(datum, "<h1>Administration Interface</h1>\n");
  cbdatumprintf(datum, "<ul>\n");
  vml = FALSE;
  vul = FALSE;
  vnl = FALSE;
  vne = FALSE;
  vns = FALSE;
  switch(action){
  case UI_VIEWMASTER:
  case UI_SHUTDOWN:
  case UI_SYNCNODES:
  case UI_BACKUPDB:
    vml = TRUE;
    break;
  case UI_VIEWUSERS:
  case UI_NEWUSER:
  case UI_DELUSER:
    vul = TRUE;
    break;
  case UI_VIEWNODES:
  case UI_NEWNODE:
  case UI_DELENODE:
    vnl = TRUE;
    break;
  case UI_EDITNODE:
    vne = TRUE;
    break;
  case UI_STATNODE:
    vns = TRUE;
    break;
  default:
    break;
  }
  if(vml){
    cbdatumprintf(datum, "<li><strong><a href=\"%@%@?action=%d\">Manage Master"
                  "</a></strong></li>\n", req->prefix, MASTERUILOC, UI_VIEWMASTER);
  } else {
    cbdatumprintf(datum, "<li><a href=\"%@%@?action=%d\">Manage Master</a></li>\n",
                  req->prefix, MASTERUILOC, UI_VIEWMASTER);
  }
  if(vul){
    cbdatumprintf(datum, "<li><strong><a href=\"%@%@?action=%d\">Manage Users"
                  "</a></strong></li>\n", req->prefix, MASTERUILOC, UI_VIEWUSERS);
  } else {
    cbdatumprintf(datum, "<li><a href=\"%@%@?action=%d\">Manage Users</a></li>\n",
                  req->prefix, MASTERUILOC, UI_VIEWUSERS);
  }
  if(vnl || vne || vns){
    cbdatumprintf(datum, "<li><strong><a href=\"%@%@?action=%d\">Manage Nodes"
                  "</a></strong></li>\n", req->prefix, MASTERUILOC, UI_VIEWNODES);
  } else {
    cbdatumprintf(datum, "<li><a href=\"%@%@?action=%d\">Manage Nodes</a></li>\n",
                  req->prefix, MASTERUILOC, UI_VIEWNODES);
  }
  cbdatumprintf(datum, "</ul>\n");
  cbdatumprintf(datum, "<hr />\n");
  switch(action){
  case UI_SHUTDOWN:
    if(sure){
      g_sigterm = TRUE;
      cbdatumprintf(datum, "<p>Shutdown operation has been ordered.</p>\n");
    } else {
      cbdatumprintf(datum, "<form method=\"post\" action=\"%@%@\">\n",
                    req->prefix, MASTERUILOC);
      cbdatumprintf(datum, "<div class=\"form\">\n");
      cbdatumprintf(datum, "confirmation to shutdown the master server:\n");
      cbdatumprintf(datum, "<input type=\"submit\" value=\"sure\" />\n");
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"action\" value=\"%d\" />\n",
                    UI_SHUTDOWN);
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"sure\" value=\"1\" />\n");
      cbdatumprintf(datum, "</div>\n");
      cbdatumprintf(datum, "</form>\n");
    }
    cbdatumprintf(datum, "<hr />\n");
    break;
  case UI_SYNCNODES:
    if(sure){
      g_sigsync = TRUE;
      cbdatumprintf(datum, "<p>Synchronus operation has been ordered.</p>\n");
    } else {
      cbdatumprintf(datum, "<form method=\"post\" action=\"%@%@\">\n",
                    req->prefix, MASTERUILOC);
      cbdatumprintf(datum, "<div class=\"form\">\n");
      cbdatumprintf(datum, "confirmation to synchronize all nodes:\n");
      cbdatumprintf(datum, "<input type=\"submit\" value=\"sure\" />\n");
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"action\" value=\"%d\" />\n",
                    UI_SYNCNODES);
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"sure\" value=\"1\" />\n");
      cbdatumprintf(datum, "</div>\n");
      cbdatumprintf(datum, "</form>\n");
    }
    cbdatumprintf(datum, "<hr />\n");
    break;
  case UI_BACKUPDB:
    if(sure){
      g_sigback = TRUE;
      cbdatumprintf(datum, "<p>Backup operation has been ordered.</p>\n");
    } else {
      cbdatumprintf(datum, "<form method=\"post\" action=\"%@%@\">\n",
                    req->prefix, MASTERUILOC);
      cbdatumprintf(datum, "<div class=\"form\">\n");
      cbdatumprintf(datum, "confirmation to backup the database:\n");
      cbdatumprintf(datum, "<input type=\"submit\" value=\"sure\" />\n");
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"action\" value=\"%d\" />\n",
                    UI_BACKUPDB);
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"sure\" value=\"1\" />\n");
      cbdatumprintf(datum, "</div>\n");
      cbdatumprintf(datum, "</form>\n");
    }
    cbdatumprintf(datum, "<hr />\n");
    break;
  case UI_NEWUSER:
    pbuf = est_make_crypt(passwd);
    if(g_runmode == RM_RDONLY){
      cbdatumprintf(datum, "<p>The server is in read only mode.</p>\n");
    } else if(name[0] == '\0' || passwd[0] == '\0'){
      cbdatumprintf(datum, "<p>The name and the password sould not be empty.</p>\n");
    } else if((tuser = umgr_get(g_umgr, name)) != NULL){
      cbdatumprintf(datum, "<p>The name should not be duplicated.</p>\n");
    } else if(!check_alnum_name(name)){
      cbdatumprintf(datum, "<p>The name can include alphanumeric characters only.</p>\n");
    } else if(umgr_put(g_umgr, name, pbuf, flags, fname, misc)){
      cbdatumprintf(datum, "<p><strong>%@</strong> was created successfully.</p>\n", name);
    } else {
      cbdatumprintf(datum, "<p>Some errors occurred.</p>\n");
    }
    free(pbuf);
    cbdatumprintf(datum, "<hr />\n");
    break;
  case UI_DELUSER:
    if(g_runmode == RM_RDONLY){
      cbdatumprintf(datum, "<p>The server is in read only mode.</p>\n");
    } else if(sure){
      if(umgr_out(g_umgr, name)){
        cbdatumprintf(datum, "<p><strong>%@</strong> was deleted successfully.</p>\n", name);
      } else {
        cbdatumprintf(datum, "<p>Some errors occurred.</p>\n");
      }
    } else {
      cbdatumprintf(datum, "<form method=\"post\" action=\"%@%@\">\n",
                    req->prefix, MASTERUILOC);
      cbdatumprintf(datum, "<div class=\"form\">\n");
      cbdatumprintf(datum, "confirmation to delete <strong>%@</strong>:\n", name);
      cbdatumprintf(datum, "<input type=\"submit\" value=\"sure\" />\n");
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"action\" value=\"%d\" />\n",
                    UI_DELUSER);
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"name\" value=\"%@\" />\n", name);
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"sure\" value=\"1\" />\n");
      cbdatumprintf(datum, "</div>\n");
      cbdatumprintf(datum, "</form>\n");
    }
    cbdatumprintf(datum, "<hr />\n");
    break;
  case UI_NEWNODE:
    if(g_runmode == RM_RDONLY){
      cbdatumprintf(datum, "<p>The server is in read only mode.</p>\n");
    } else if(name[0] == '\0' || label[0] == '\0'){
      cbdatumprintf(datum, "<p>The name and the label sould not be empty.</p>\n");
    } else if((tnode = nmgr_get(g_nmgr, name)) != NULL){
      cbdatumprintf(datum, "<p>The name should not be duplicated.</p>\n");
    } else if(!check_alnum_name(name)){
      cbdatumprintf(datum, "<p>The name can include alphanumeric characters only.</p>\n");
    } else if(nmgr_put(g_nmgr, name, TRUE, getnodeoptions())){
      if((tnode = nmgr_get(g_nmgr, name)) != NULL){
        free(tnode->label);
        tnode->label = cbmemdup(label, -1);
      }
      nmgr_sync(g_nmgr, FALSE);
      cbdatumprintf(datum, "<p><strong>%@</strong> was created successfully.</p>\n", name);
    } else {
      cbdatumprintf(datum, "<p>Some errors occurred.</p>\n");
    }
    cbdatumprintf(datum, "<hr />\n");
    break;
  case UI_DELENODE:
    if(g_runmode == RM_RDONLY){
      cbdatumprintf(datum, "<p>The server is in read only mode.</p>\n");
    } else if(sure){
      if(nmgr_out(g_nmgr, name)){
        cbdatumprintf(datum, "<p><strong>%@</strong> was deleted successfully.</p>\n", name);
      } else {
        cbdatumprintf(datum, "<p>Some errors occurred.</p>\n");
      }
    } else {
      cbdatumprintf(datum, "<form method=\"post\" action=\"%@%@\">\n",
                    req->prefix, MASTERUILOC);
      cbdatumprintf(datum, "<div class=\"form\">\n");
      cbdatumprintf(datum, "confirmation to delete <strong>%@</strong>:\n", name);
      cbdatumprintf(datum, "<input type=\"submit\" value=\"sure\" />\n");
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"action\" value=\"%d\" />\n",
                    UI_DELENODE);
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"name\" value=\"%@\" />\n", name);
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"sure\" value=\"1\" />\n");
      cbdatumprintf(datum, "</div>\n");
      cbdatumprintf(datum, "</form>\n");
    }
    cbdatumprintf(datum, "<hr />\n");
    break;
  case UI_EDITNODE:
    if(!(tnode = nmgr_get(g_nmgr, name))){
      cbdatumprintf(datum, "<p>Some errors occurred.</p>\n");
      cbdatumprintf(datum, "<hr />\n");
    } else if(label[0] != '\0'){
      free(tnode->label);
      tnode->label = cbmemdup(label, -1);
      cbdatumprintf(datum, "<p>The label was updated.</p>\n");
      cbdatumprintf(datum, "<hr />\n");
    } else if(admins){
      cbmapclose(tnode->admins);
      tnode->admins = cbmapopenex(MINIBNUM);
      list = cbsplit(admins, -1, "\r\n");
      for(i = 0; i < cblistnum(list); i++){
        pbuf = cbmemdup(cblistval(list, i, NULL), -1);
        cbstrtrim(pbuf);
        if(pbuf[0] != '\0' && check_alnum_name(pbuf))
          cbmapput(tnode->admins, pbuf, -1, "", 0, TRUE);
        free(pbuf);
      }
      cblistclose(list);
      cbdatumprintf(datum, "<p>The list of administrators was updated.</p>\n");
      cbdatumprintf(datum, "<hr />\n");
    } else if(users){
      cbmapclose(tnode->users);
      tnode->users = cbmapopenex(MINIBNUM);
      list = cbsplit(users, -1, "\r\n");
      for(i = 0; i < cblistnum(list); i++){
        pbuf = cbmemdup(cblistval(list, i, NULL), -1);
        cbstrtrim(pbuf);
        if(pbuf[0] != '\0' && check_alnum_name(pbuf))
          cbmapput(tnode->users, pbuf, -1, "", 0, TRUE);
        free(pbuf);
      }
      cblistclose(list);
      cbdatumprintf(datum, "<p>The list of normal users was updated.</p>\n");
      cbdatumprintf(datum, "<hr />\n");
    } else if(links){
      cbmapclose(tnode->links);
      tnode->links = cbmapopenex(MINIBNUM);
      list = cbsplit(links, -1, "\r\n");
      for(i = 0; i < cblistnum(list); i++){
        pbuf = cbmemdup(cblistval(list, i, NULL), -1);
        cbstrtrim(pbuf);
        if((pv = strstr(pbuf, DELIMSTR)) != NULL && (nv = strstr(pv + 1, DELIMSTR)) != NULL){
          *pv = '\0';
          pv += strlen(DELIMSTR);
          *nv = '\0';
          nv += strlen(DELIMSTR);
          node_set_link(tnode, pbuf, pv, atoi(nv));
        }
        free(pbuf);
      }
      cblistclose(list);

      cbdatumprintf(datum, "<p>The list of links was updated.</p>\n");
      cbdatumprintf(datum, "<hr />\n");
    }
    break;
  default:
    break;
  }
  if(vml){
    cbdatumprintf(datum, "<ul>\n");
    cbdatumprintf(datum, "<li>");
    cbdatumprintf(datum, "<a href=\"%@%@?action=%d\">SHUTDOWN</a>",
                  req->prefix, MASTERUILOC, UI_SHUTDOWN);
    cbdatumprintf(datum, "</li>\n");
    cbdatumprintf(datum, "<li>");
    cbdatumprintf(datum, "<a href=\"%@%@?action=%d\">SYNCHRONIZE</a>",
                  req->prefix, MASTERUILOC, UI_SYNCNODES);
    cbdatumprintf(datum, "</li>\n");
    if(g_backupcmd[0] != '\0'){
      cbdatumprintf(datum, "<li>");
      cbdatumprintf(datum, "<a href=\"%@%@?action=%d\">BACKUP</a>",
                    req->prefix, MASTERUILOC, UI_BACKUPDB);
      cbdatumprintf(datum, "</li>\n");
    }
    cbdatumprintf(datum, "</ul>\n");
    cbdatumprintf(datum, "<hr />\n");
    list = nmgr_names(g_nmgr);
    dnum = 0;
    fsiz = 0.0;
    msiz = 0;
    for(i = 0; i < cblistnum(list); i++){
      if(!(tmp = cblistval(list, i, NULL)) || !(tnode = nmgr_get(g_nmgr, tmp))) continue;
      dnum += est_mtdb_doc_num(tnode->db);
      fsiz += est_mtdb_size(tnode->db);
      if((size = est_mtdb_used_cache_size(tnode->db)) > msiz) msiz = size;
    }
    g_cacheratio = msiz / g_cachesize;
    cblistclose(list);
    rsec = req->now - g_startdate;
    cbdatumprintf(datum, "<dl class=\"tlist\">\n");
    cbdatumprintf(datum, "<dt>total number of documents</dt>\n");
    cbdatumprintf(datum, "<dd>%d</dd>\n", dnum);
    cbdatumprintf(datum, "<dt>total size of database files</dt>\n");
    cbdatumprintf(datum, "<dd>%0.0fMB</dd>\n", fsiz / 1024.0 / 1024.0);
    cbdatumprintf(datum, "<dt>worst usage ratio of the writing cache</dt>\n");
    cbdatumprintf(datum, "<dd>%0.1f%%</dd>\n", g_cacheratio * 100.0);
    cbdatumprintf(datum, "<dt>total access count</dt>\n");
    cbdatumprintf(datum, "<dd>%d</dd>\n", g_accesscount);
    cbdatumprintf(datum, "<dt>running time</dt>\n");
    pbuf = cbdatestrwww(g_startdate, 0);
    cbdatumprintf(datum, "<dd>%dh %dm %ds (since %@)</dd>\n",
                  (int)(rsec / 3600), (int)((rsec / 60) % 60), (int)(rsec % 60), pbuf);
    free(pbuf);
    cbdatumprintf(datum, "</dl>\n");
    cbdatumprintf(datum, "<hr />\n");
  }
  if(vul){
    list = umgr_names(g_umgr);
    cbdatumprintf(datum, "<table summary=\"nodes\">\n");
    cbdatumprintf(datum, "<tr>\n");
    cbdatumprintf(datum, "<th abbr=\"name\">name</th>\n");
    cbdatumprintf(datum, "<th abbr=\"passwd\">password</th>\n");
    cbdatumprintf(datum, "<th abbr=\"flags\">flags</th>\n");
    cbdatumprintf(datum, "<th abbr=\"fname\">full name</th>\n");
    cbdatumprintf(datum, "<th abbr=\"misc\">misc</th>\n");
    cbdatumprintf(datum, "<th abbr=\"actions\">actions</th>\n");
    cbdatumprintf(datum, "</tr>\n");
    for(i = 0; i < cblistnum(list); i++){
      if(!(tmp = cblistval(list, i, NULL)) || !(tuser = umgr_get(g_umgr, tmp))) continue;
      pbuf = cbmemdup(tuser->passwd, -1);
      if(strlen(pbuf) > 8) sprintf(pbuf + 4, "...");
      cbdatumprintf(datum, "<tr>\n");
      cbdatumprintf(datum, "<td>%@</td>\n", tuser->name);
      cbdatumprintf(datum, "<td>%@</td>\n", pbuf);
      cbdatumprintf(datum, "<td>%@</td>\n", tuser->flags);
      cbdatumprintf(datum, "<td>%@</td>\n", tuser->fname);
      cbdatumprintf(datum, "<td>%@</td>\n", tuser->misc);
      cbdatumprintf(datum, "<td>");
      cbdatumprintf(datum, "<a href=\"%@%@?action=%d&amp;name=%?\">DELE</a>",
                    req->prefix, MASTERUILOC, UI_DELUSER, tuser->name);
      cbdatumprintf(datum, "</td>\n");
      cbdatumprintf(datum, "</tr>\n");
      free(pbuf);
    }
    cbdatumprintf(datum, "</table>\n");
    cblistclose(list);
    cbdatumprintf(datum, "<form method=\"post\" action=\"%@%@\">\n", req->prefix, MASTERUILOC);
    cbdatumprintf(datum, "<div class=\"form\">\n");
    cbdatumprintf(datum, "<input type=\"text\" name=\"name\" value=\"\" size=\"12\" />\n");
    cbdatumprintf(datum, "<input type=\"password\" name=\"passwd\" value=\"\""
                  " size=\"12\" />\n");
    cbdatumprintf(datum, "<input type=\"text\" name=\"flags\" value=\"\" size=\"4\" />\n");
    cbdatumprintf(datum, "<input type=\"text\" name=\"fname\" value=\"\" size=\"12\" />\n");
    cbdatumprintf(datum, "<input type=\"text\" name=\"misc\" value=\"\" size=\"12\" />\n");
    cbdatumprintf(datum, "<input type=\"submit\" value=\"create\" />\n");
    cbdatumprintf(datum, "<input type=\"hidden\" name=\"action\" value=\"%d\" />\n",
                  UI_NEWUSER);
    cbdatumprintf(datum, "</div>\n");
    cbdatumprintf(datum, "</form>\n");
    cbdatumprintf(datum, "<hr />\n");
  }
  if(vnl){
    list = nmgr_names(g_nmgr);
    cbdatumprintf(datum, "<table summary=\"nodes\">\n");
    cbdatumprintf(datum, "<tr>\n");
    cbdatumprintf(datum, "<th abbr=\"name\">name</th>\n");
    cbdatumprintf(datum, "<th abbr=\"label\">label</th>\n");
    cbdatumprintf(datum, "<th abbr=\"docnum\">#docs</th>\n");
    cbdatumprintf(datum, "<th abbr=\"wordnum\">#words</th>\n");
    cbdatumprintf(datum, "<th abbr=\"size\">size</th>\n");
    cbdatumprintf(datum, "<th abbr=\"adminnum\">#a</th>\n");
    cbdatumprintf(datum, "<th abbr=\"usernum\">#u</th>\n");
    cbdatumprintf(datum, "<th abbr=\"linknum\">#l</th>\n");
    cbdatumprintf(datum, "<th abbr=\"actions\">actions</th>\n");
    cbdatumprintf(datum, "</tr>\n");
    for(i = 0; i < cblistnum(list); i++){
      if(!(tmp = cblistval(list, i, NULL)) || !(tnode = nmgr_get(g_nmgr, tmp))) continue;
      cbdatumprintf(datum, "<tr>\n");
      cbdatumprintf(datum, "<td><a href=\"%@%@%@/%@\">%@</a></td>\n",
                    req->prefix, NODEPREFIX, tnode->name, SEARCHUICMD, tnode->name);
      cbdatumprintf(datum, "<td>%@</td>\n", tnode->label);
      cbdatumprintf(datum, "<td>%d</td>\n", est_mtdb_doc_num(tnode->db));
      cbdatumprintf(datum, "<td>%d</td>\n", est_mtdb_word_num(tnode->db));
      cbdatumprintf(datum, "<td>%.1f</td>\n", est_mtdb_size(tnode->db) / 1024.0 / 1024.0);
      cbdatumprintf(datum, "<td>%d</td>\n", cbmaprnum(tnode->admins));
      cbdatumprintf(datum, "<td>%d</td>\n", cbmaprnum(tnode->users));
      cbdatumprintf(datum, "<td>%d</td>\n", cbmaprnum(tnode->links));
      cbdatumprintf(datum, "<td>");
      cbdatumprintf(datum, "<a href=\"%@%@?action=%d&amp;name=%?\">STAT</a>",
                    req->prefix, MASTERUILOC, UI_STATNODE, tnode->name);
      cbdatumprintf(datum, " / <a href=\"%@%@?action=%d&amp;name=%?\">EDIT</a>",
                    req->prefix, MASTERUILOC, UI_EDITNODE, tnode->name);
      cbdatumprintf(datum, " / <a href=\"%@%@?action=%d&amp;name=%?\">DELE</a>",
                    req->prefix, MASTERUILOC, UI_DELENODE, tnode->name);
      cbdatumprintf(datum, "</td>\n");
      cbdatumprintf(datum, "</tr>\n");
    }
    cbdatumprintf(datum, "</table>\n");
    cblistclose(list);
    cbdatumprintf(datum, "<form method=\"post\" action=\"%@%@\">\n", req->prefix, MASTERUILOC);
    cbdatumprintf(datum, "<div class=\"form\">\n");
    cbdatumprintf(datum, "<input type=\"text\" name=\"name\" value=\"\" size=\"12\" />\n");
    cbdatumprintf(datum, "<input type=\"text\" name=\"label\" value=\"\" size=\"24\" />\n");
    cbdatumprintf(datum, "<input type=\"submit\" value=\"create\" />\n");
    cbdatumprintf(datum, "<input type=\"hidden\" name=\"action\" value=\"%d\" />\n",
                  UI_NEWNODE);
    cbdatumprintf(datum, "</div>\n");
    cbdatumprintf(datum, "</form>\n");
    cbdatumprintf(datum, "<hr />\n");
  }
  if(vne){
    if((tnode = nmgr_get(g_nmgr, name)) != NULL){
      cbdatumprintf(datum, "<dl class=\"tlist\">\n");
      cbdatumprintf(datum, "<dt>name</dt>\n");
      cbdatumprintf(datum, "<dd>%@</dd>\n", tnode->name);
      cbdatumprintf(datum, "</dl>\n");
      cbdatumprintf(datum, "<form method=\"post\" action=\"%@%@\">\n",
                    req->prefix, MASTERUILOC);
      cbdatumprintf(datum, "<dl class=\"tlist\">\n");
      cbdatumprintf(datum, "<dt>label</dt>\n");
      cbdatumprintf(datum, "<dd>");
      cbdatumprintf(datum, "<input type=\"text\" name=\"label\" value=\"%@\" size=\"64\" />",
                    tnode->label);
      cbdatumprintf(datum, " <input type=\"submit\" value=\"change\" />");
      cbdatumprintf(datum, "</dd>\n");
      cbdatumprintf(datum, "</dl>\n");
      cbdatumprintf(datum, "<div class=\"hidden\">\n");
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"action\" value=\"%d\" />\n",
                    UI_EDITNODE);
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"name\" value=\"%@\" />\n", name);
      cbdatumprintf(datum, "</div>\n");
      cbdatumprintf(datum, "</form>\n");
      cbdatumprintf(datum, "<form method=\"post\" action=\"%@%@\">\n",
                    req->prefix, MASTERUILOC);
      cbdatumprintf(datum, "<dl class=\"tlist\">\n");
      cbdatumprintf(datum, "<dt>list of administrators</dt>\n");
      cbdatumprintf(datum, "<dd>");
      cbdatumprintf(datum, "<textarea name=\"admins\" cols=\"80\" rows=\"5\">");
      cbmapiterinit(tnode->admins);
      while((tmp = cbmapiternext(tnode->admins, NULL)) != NULL){
        cbdatumprintf(datum, "%@\n", tmp);
      }
      cbdatumprintf(datum, "</textarea>");
      cbdatumprintf(datum, "</dd>\n");
      cbdatumprintf(datum, "<dd>");
      cbdatumprintf(datum, "<input type=\"submit\" value=\"change\" />");
      cbdatumprintf(datum, "</dd>\n");
      cbdatumprintf(datum, "</dl>\n");
      cbdatumprintf(datum, "<div class=\"hidden\">\n");
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"action\" value=\"%d\" />\n",
                    UI_EDITNODE);
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"name\" value=\"%@\" />\n", name);
      cbdatumprintf(datum, "</div>\n");
      cbdatumprintf(datum, "</form>\n");
      cbdatumprintf(datum, "<form method=\"post\" action=\"%@%@\">\n",
                    req->prefix, MASTERUILOC);
      cbdatumprintf(datum, "<dl class=\"tlist\">\n");
      cbdatumprintf(datum, "<dt>list of normal users</dt>\n");
      cbdatumprintf(datum, "<dd>");
      cbdatumprintf(datum, "<textarea name=\"users\" cols=\"80\" rows=\"5\">");
      cbmapiterinit(tnode->users);
      while((tmp = cbmapiternext(tnode->users, NULL)) != NULL){
        cbdatumprintf(datum, "%@\n", tmp);
      }
      cbdatumprintf(datum, "</textarea>");
      cbdatumprintf(datum, "</dd>\n");
      cbdatumprintf(datum, "<dd>");
      cbdatumprintf(datum, "<input type=\"submit\" value=\"change\" />");
      cbdatumprintf(datum, "</dd>\n");
      cbdatumprintf(datum, "</dl>\n");
      cbdatumprintf(datum, "<div class=\"hidden\">\n");
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"action\" value=\"%d\" />\n",
                    UI_EDITNODE);
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"name\" value=\"%@\" />\n", name);
      cbdatumprintf(datum, "</div>\n");
      cbdatumprintf(datum, "</form>\n");
      cbdatumprintf(datum, "<form method=\"post\" action=\"%@%@\">\n",
                    req->prefix, MASTERUILOC);
      cbdatumprintf(datum, "<dl class=\"tlist\">\n");
      cbdatumprintf(datum, "<dt>list of links</dt>\n");
      cbdatumprintf(datum, "<dd>");
      cbdatumprintf(datum, "<textarea name=\"links\" cols=\"80\" rows=\"5\">");
      cbmapiterinit(tnode->links);
      while((tmp = cbmapiternext(tnode->links, NULL)) != NULL){
        pbuf = cbmemdup(cbmapiterval(tmp, NULL), -1);
        if((pv = strchr(pbuf, '\t')) != NULL){
          *(pv++) = '\0';
          cbdatumprintf(datum, "%@%@%@%@%@\n", tmp, DELIMSTR, pbuf, DELIMSTR, pv);
        }
        free(pbuf);
      }
      cbdatumprintf(datum, "</textarea>");
      cbdatumprintf(datum, "</dd>\n");
      cbdatumprintf(datum, "<dd>");
      cbdatumprintf(datum, "<input type=\"submit\" value=\"change\" />");
      cbdatumprintf(datum, "</dd>\n");
      cbdatumprintf(datum, "</dl>\n");
      cbdatumprintf(datum, "<div class=\"hidden\">\n");
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"action\" value=\"%d\" />\n",
                    UI_EDITNODE);
      cbdatumprintf(datum, "<input type=\"hidden\" name=\"name\" value=\"%@\" />\n", name);
      cbdatumprintf(datum, "</div>\n");
      cbdatumprintf(datum, "</form>\n");
    } else {
      cbdatumprintf(datum, "<p>Some errors occured.</p>\n");
    }
    cbdatumprintf(datum, "<hr />\n");
  }
  if(vns){
    if((tnode = nmgr_get(g_nmgr, name)) != NULL){
      cbdatumprintf(datum, "<dl class=\"tlist\">\n");
      cbdatumprintf(datum, "<dt>name</dt>\n");
      cbdatumprintf(datum, "<dd>%@</dd>\n", tnode->name);
      cbdatumprintf(datum, "</dl>\n");
      cbdatumprintf(datum, "<dl class=\"tlist\">\n");
      cbdatumprintf(datum, "<dt>label</dt>\n");
      cbdatumprintf(datum, "<dd>%@</dd>\n", tnode->label);
      cbdatumprintf(datum, "</dl>\n");
      cbdatumprintf(datum, "<dl class=\"tlist\">\n");
      cbdatumprintf(datum, "<dt>number of documents</dt>\n");
      cbdatumprintf(datum, "<dd>%d</dd>\n", est_mtdb_doc_num(tnode->db));
      cbdatumprintf(datum, "</dl>\n");
      cbdatumprintf(datum, "<dl class=\"tlist\">\n");
      cbdatumprintf(datum, "<dt>number of unique words</dt>\n");
      cbdatumprintf(datum, "<dd>%d</dd>\n", est_mtdb_word_num(tnode->db));
      cbdatumprintf(datum, "</dl>\n");
      cbdatumprintf(datum, "<dl class=\"tlist\">\n");
      cbdatumprintf(datum, "<dt>size of the database</dt>\n");
      cbdatumprintf(datum, "<dd>%.1fMB</dd>\n", est_mtdb_size(tnode->db) / 1024.0 / 1024.0);
      cbdatumprintf(datum, "</dl>\n");
      cbdatumprintf(datum, "<dl class=\"tlist\">\n");
      cbdatumprintf(datum, "<dt>words in the reading cache</dt>\n");
      cbdatumprintf(datum, "<dd>");
      words = est_mtdb_list_rescc(tnode->db);
      for(i = 0; i < cblistnum(words); i++){
        tmp = cblistval(words, i, NULL);
        if(i > 0) cbdatumprintf(datum, ", ");
        cbdatumprintf(datum, "<a href=\"%@%@%@/%@?phrase=%?\">%@</a>",
                      req->prefix, NODEPREFIX, tnode->name, SEARCHUICMD, tmp, tmp);
      }
      cblistclose(words);
      cbdatumprintf(datum, "...</dd>\n");
      cbdatumprintf(datum, "</dl>\n");
      cbdatumprintf(datum, "<dl class=\"tlist\">\n");
      cbdatumprintf(datum, "<dt>usage ratio of the writing cache</dt>\n");
      size = est_mtdb_used_cache_size(tnode->db);
      cbdatumprintf(datum, "<dd>%.1f%% (%.1fMB / %.1fMB), %d unique words</dd>",
                    size / g_cachesize * 100, size / 1024.0 / 1024.0,
                    g_cachesize / 1024 / 1024, est_mtdb_cache_num(tnode->db));
      cbdatumprintf(datum, "</dl>\n");
      cbdatumprintf(datum, "<dl class=\"tlist\">\n");
      cbdatumprintf(datum, "<dt>modification date</dt>\n");
      pbuf = cbdatestrhttp(tnode->mtime, 0);
      cbdatumprintf(datum, "<dd>%@</dd>", pbuf);
      free(pbuf);
      cbdatumprintf(datum, "</dl>\n");
    } else {
      cbdatumprintf(datum, "<p>Some errors occured.</p>\n");
    }
    cbdatumprintf(datum, "<hr />\n");
  }
  cbdatumprintf(datum, "<address class=\"logo\">Powered by <a href=\"%@\">Hyper Estraier</a>"
                " %@.</address>\n", _EST_PROJURL, est_version);
  cbdatumprintf(datum, "</body>\n");
  cbdatumprintf(datum, "</html>\n");
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: 200 OK (ui:%d)", req->claddr, req->clport, action);
  if(!rwlock_unlock(g_mgrlock)) log_print(LL_ERROR, "unlocking failed");
}


/* send the favorite icon data */
static void sendfavicondata(int clsock, REQUEST *req){
  CBDATUM *datum;
  if(req->ims > 0){
    sendnotmoderror(clsock, req);
    return;
  }
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
  addservinfo(datum, req->now, IMGCACHELIFE);
  cbdatumprintf(datum, "Content-Length: %d\r\n", sizeof(g_favicon));
  cbdatumprintf(datum, "Content-Type: image/png\r\n");
  cbdatumprintf(datum, "Last-Modified: Sat, 01 Jan 2000 00:00:00 GMT\r\n");
  cbdatumprintf(datum, "\r\n");
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  est_sock_send_all(clsock, (char *)g_favicon, sizeof(g_favicon));
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: 200 OK (favicon)", req->claddr, req->clport);
}


/* send a image data */
static void sendimagedata(int clsock, REQUEST *req, const char *name){
  CBDATUM *datum;
  const char *ptr, *type;
  int size;
  if(req->ims > 0){
    sendnotmoderror(clsock, req);
    return;
  }
  if(!strcmp(name, BIGICONNAME)){
    ptr = (char *)g_bigicon;
    size = sizeof(g_bigicon);
    type = "image/png";
  } else if(!strcmp(name, CANVASNAME)){
    ptr = (char *)g_canvas;
    size = sizeof(g_canvas);
    type = "image/png";
  } else {
    senderror(clsock, req, 404, "File Not Found");
    return;
  }
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
  addservinfo(datum, req->now, IMGCACHELIFE);
  cbdatumprintf(datum, "Content-Length: %d\r\n", size);
  cbdatumprintf(datum, "Content-Type: %s\r\n", type);
  cbdatumprintf(datum, "Last-Modified: Sat, 01 Jan 2000 00:00:00 GMT\r\n");
  cbdatumprintf(datum, "\r\n");
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  est_sock_send_all(clsock, ptr, size);
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: 200 OK (%s)", req->claddr, req->clport, name);
}


/* send a file data */
static void sendfiledata(int clsock, REQUEST *req){
  struct stat sbuf;
  CBLIST *list;
  CBDATUM *datum;
  const char *elem;
  char *path, *index, pbuf[URIBUFSIZ], ibuf[IOBUFSIZ], *ext, *tmp;
  int i, fd, len, fdir;
  time_t ims;
  double size;
  path = makelocalpath(req->target);
  if(g_indexfile[0] != '\0'){
    index = cbsprintf("%s%c%s", path, ESTPATHCHR, g_indexfile);
    if(stat(index, &sbuf) == 0 && cbstrbwmatch(req->target, "/")){
      free(path);
      path = index;
      index = NULL;
    }
    free(index);
  }
  if((list = cbdirlist(path)) != NULL){
    datum = cbdatumopen(NULL, -1);
    if(cbstrbwmatch(req->target, "/")){
      cblistsort(list);
      cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
      addservinfo(datum, req->now, 0);
      cbdatumprintf(datum, "Content-Type: %s\r\n", DIRMIMETYPE);
      cbdatumprintf(datum, "\r\n");
      if(req->method != HM_HEAD){
        cbdatumprintf(datum, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        cbdatumprintf(datum, "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\""
                      " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
        cbdatumprintf(datum, "<html xmlns=\"http://www.w3.org/1999/xhtml\""
                      " xml:lang=\"en\" lang=\"en\">\n");
        cbdatumprintf(datum, "<head>\n");
        cbdatumprintf(datum, "<meta http-equiv=\"Content-Type\" content=\"%@\" />\n",
                      DIRMIMETYPE);
        cbdatumprintf(datum, "<meta http-equiv=\"Content-Style-Type\""
                      " content=\"text/css\" />\n");
        cbdatumprintf(datum, "<meta name=\"generator\" content=\"%@/%@\" />\n",
                      SERVNAME, est_version);
        cbdatumprintf(datum, "<meta name=\"robots\" content=\"NOFOLLOW\" />\n");
        cbdatumprintf(datum, "<link rel=\"contents\" href=\"./\" />\n");
        cbdatumprintf(datum, "<link rel=\"icon\" href=\"%@%@\" />\n", req->prefix, FAVICONLOC);
        cbdatumprintf(datum, "<link rev=\"made\" href=\"mailto:%@\" />\n", g_adminemail);
        cbdatumprintf(datum, "<title>%@</title>\n", req->target);
        cbdatumprintf(datum, "<style type=\"text/css\">"
                      "html { margin: 0em 0em; padding 0em 0em;"
                      " background: #eeeeee none; }\n");
        cbdatumprintf(datum, "body { margin: 0em 0em; padding: 1em 1em;"
                      " font-style: normal; font-weight: normal; color: #111111; }\n");
        cbdatumprintf(datum, "a { color: #0022aa; text-decoration: none; }\n");
        cbdatumprintf(datum, "a:hover,a:focus { color: #0033ee;"
                      " text-decoration: underline; }\n");
        cbdatumprintf(datum, "</style>\n");
        cbdatumprintf(datum, "</head>\n");
        cbdatumprintf(datum, "<body>\n");
        cbdatumprintf(datum, "<h1>Index of %@</h1>\n", req->target);
        cbdatumprintf(datum, "<hr />\n");
        cbdatumprintf(datum, "<ul>\n");
        if(strcmp(req->target, "/"))
          cbdatumprintf(datum, "<li><a href=\"../\">../</a></li>\n");
        for(i = 0; i < cblistnum(list); i++){
          elem = cblistval(list, i, NULL);
          if(!strcmp(elem, ESTCDIRSTR) || !strcmp(elem, ESTPDIRSTR) || elem[0] == ESTEXTCHR)
            continue;
          sprintf(pbuf, "%s%c%s", path, ESTPATHCHR, elem);
          if(!cbfilestat(pbuf, &fdir, NULL, NULL)) continue;
          cbdatumprintf(datum, "<li><a href=\"%?%s\">%@%s</a></li>\n",
                        elem, fdir ? "/" : "", elem, fdir ? "/" : "");
        }
        cbdatumprintf(datum, "</ul>\n");
        cbdatumprintf(datum, "<hr />\n");
        cbdatumprintf(datum, "<address>%@/%@ at %@/</address>\n",
                      SERVNAME, est_version, req->prefix);
        cbdatumprintf(datum, "</body>\n");
        cbdatumprintf(datum, "</html>\n");
      }
      log_print(LL_DEBUG, "[%s:%d]: 200 OK (directory)", req->claddr, req->clport);
    } else {
      cblistsort(list);
      cbdatumprintf(datum, "HTTP/1.0 301 Moved Parmanently\r\n");
      addservinfo(datum, req->now, 0);
      cbdatumprintf(datum, "Location: %s%s/\r\n", req->prefix, req->target);
      cbdatumprintf(datum, "Content-Type: text/plain\r\n");
      cbdatumprintf(datum, "\r\n");
      if(req->method != HM_HEAD) cbdatumprintf(datum, "Go to %s/\n", req->target);
      log_print(LL_DEBUG, "[%s:%d]: 301 (moved parmanently)", req->claddr, req->clport);
    }
    est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
    cbdatumclose(datum);
    cblistclose(list);
  } else if(strstr(path, ESTPATHSTR ESTEXTSTR)){
    senderror(clsock, req, 403, "Forbidden");
  } else if((fd = open(path, O_RDONLY, 0)) != -1){
    if(fstat(fd, &sbuf) == 0){
      ims = sbuf.st_mtime;
      size = sbuf.st_size;
    } else {
      ims = -1;
      size = -1.0;
    }
    if(req->ims > 0 && ims <= req->ims){
      sendnotmoderror(clsock, req);
    } else {
      datum = cbdatumopen(NULL, -1);
      cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
      addservinfo(datum, req->now, 0);
      if(ims > 0){
        tmp = cbdatestrhttp(ims, 0);
        cbdatumprintf(datum, "Last-Modified: %s\r\n", tmp);
        free(tmp);
      }
      if(size >= 0.0) cbdatumprintf(datum, "Content-Length: %.0f\r\n", size);
      ext = strrchr(path, ESTPATHCHR);
      ext = strrchr(ext ? ext : path, '.');
      cbdatumprintf(datum, "Content-Type: %s\r\n", est_ext_type(ext ? ext : ""));
      cbdatumprintf(datum, "\r\n");
      est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
      if(req->method != HM_HEAD){
        while((len = read(fd, ibuf, IOBUFSIZ)) > 0 && !g_sigterm){
          send(clsock, ibuf, len, 0);
        }
      }
      cbdatumclose(datum);
      log_print(LL_DEBUG, "[%s:%d]: 200 OK (file)", req->claddr, req->clport);
    }
    close(fd);
  } else {
    senderror(clsock, req, 404, "Not Found");
  }
  free(path);
}


/* make the local path of a target */
static char *makelocalpath(const char *target){
  CBLIST *list;
  CBDATUM *datum;
  const char *elem;
  char *tmp;
  int i;
  datum = cbdatumopen(g_docroot, -1);
  list = cbsplit(target, -1, "/");
  for(i = 0; i < cblistnum(list); i++){
    elem = cblistval(list, i, NULL);
    if(elem[0] == '\0') continue;
    tmp = cburldecode(elem, NULL);
    cbdatumprintf(datum, "%c%s", ESTPATHCHR, tmp);
    free(tmp);
  }
  cblistclose(list);
  return cbdatumtomalloc(datum, NULL);
}


/* send the menu data */
static void sendmenudata(int clsock, REQUEST *req){
  NODE *tnode;
  CBDATUM *datum;
  CBLIST *list;
  const char *tmp;
  int i;
  if(!rwlock_lock(g_mgrlock, FALSE)){
    log_print(LL_ERROR, "locking failed");
    senderror(clsock, req, 500, "Internal Server Error (locking failed)");
    return;
  }
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "HTTP/1.0 200 OK\r\n");
  addservinfo(datum, req->now, 0);
  cbdatumprintf(datum, "Content-Type: %s\r\n", DIRMIMETYPE);
  cbdatumprintf(datum, "\r\n");
  if(req->method != HM_HEAD){
    cbdatumprintf(datum, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    cbdatumprintf(datum, "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\""
                  " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
    cbdatumprintf(datum, "<html xmlns=\"http://www.w3.org/1999/xhtml\""
                  " xml:lang=\"en\" lang=\"en\">\n");
    cbdatumprintf(datum, "<head>\n");
    cbdatumprintf(datum, "<meta http-equiv=\"Content-Type\" content=\"%@\" />\n",
                  MENUMIMETYPE);
    cbdatumprintf(datum, "<meta http-equiv=\"Content-Style-Type\""
                  " content=\"text/css\" />\n");
    cbdatumprintf(datum, "<meta name=\"generator\" content=\"%@/%@\" />\n",
                  SERVNAME, est_version);
    cbdatumprintf(datum, "<meta name=\"robots\" content=\"NOFOLLOW\" />\n");
    cbdatumprintf(datum, "<link rel=\"contents\" href=\"./\" />\n");
    cbdatumprintf(datum, "<link rel=\"icon\" href=\"%@%@\" />\n", req->prefix, FAVICONLOC);
    cbdatumprintf(datum, "<link rev=\"made\" href=\"mailto:%@\" />\n", g_adminemail);
    cbdatumprintf(datum, "<title>%@</title>\n", SERVNAME);
    cbdatumprintf(datum, "<style type=\"text/css\">"
                  "html { margin: 0em 0em; padding 0em 0em;"
                  " background: #eeeeee none; }\n");
    cbdatumprintf(datum, "body { margin: 0em 0em; padding: 1em 1em;"
                  " font-style: normal; font-weight: normal; color: #111111; }\n");
    cbdatumprintf(datum, "a { color: #0022aa; text-decoration: none; }\n");
    cbdatumprintf(datum, "a:hover,a:focus { color: #0033ee;"
                  " text-decoration: underline; }\n");
    cbdatumprintf(datum, "</style>\n");
    cbdatumprintf(datum, "</head>\n");
    cbdatumprintf(datum, "<body>\n");
    cbdatumprintf(datum, "<h1>Hyper Estraier</h1>\n");
    list = nmgr_names(g_nmgr);
    if(cblistnum(list) > 0){
      cbdatumprintf(datum, "<ul>\n");
      for(i = 0; i < cblistnum(list); i++){
        if(!(tmp = cblistval(list, i, NULL)) || !(tnode = nmgr_get(g_nmgr, tmp))) continue;
        cbdatumprintf(datum, "<li><a href=\"%@%@%@/%@\">%@</a></li>\n",
                      req->prefix, NODEPREFIX, tnode->name, SEARCHUICMD, tnode->name);
      }
      cbdatumprintf(datum, "</ul>\n");
    }
    cblistclose(list);
    cbdatumprintf(datum, "<ul>\n");
    cbdatumprintf(datum, "<li><a href=\"%@%@\">administration</a></li>\n",
                  req->prefix, MASTERUILOC);
    cbdatumprintf(datum, "</ul>\n");
    cbdatumprintf(datum, "<hr />\n");
    cbdatumprintf(datum, "<address>%@/%@ at %@/</address>\n",
                  SERVNAME, est_version, req->prefix);
    cbdatumprintf(datum, "</body>\n");
    cbdatumprintf(datum, "</html>\n");
  }
  est_sock_send_all(clsock, cbdatumptr(datum), cbdatumsize(datum));
  cbdatumclose(datum);
  log_print(LL_DEBUG, "[%s:%d]: 200 OK (menu)", req->claddr, req->clport);
  if(!rwlock_unlock(g_mgrlock)) log_print(LL_ERROR, "unlocking failed");
}



/* END OF FILE */
