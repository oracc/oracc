/*************************************************************************************************
 * Common modules related to estwaver
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


#ifndef _WAVERMOD_H                      /* duplication check */
#define _WAVERMOD_H

#include "estraier.h"
#include "estmtdb.h"
#include "estnode.h"
#include "myconf.h"
#include "mymorph.h"



/*************************************************************************************************
 * pseudo API
 *************************************************************************************************/


#define NUMBUFSIZ      32                /* size of a buffer for a number */
#define URIBUFSIZ      8192              /* size of a buffer for an URI */
#define IOBUFSIZ       8192              /* size of a buffer for I/O */
#define MINIBNUM       31                /* bucket number of a small map */
#define RESLIMSIZE     (1048576*32)      /* limitation of the entity body of response */

#define CONFFILE       "_conf"           /* name of the configuration file */
#define METAFILE       "_meta"           /* name of the meta database */
#define LOGFILE        "_log"            /* name of the log file */
#define INDEXDIR       "_index"          /* name of the index directory */
#define MYTMPDIR       "_tmp"            /* name of the temporary directory */
#define MMKMAGIC       "magic"           /* meta key of the magic number of meta DB */
#define MMKMAGVAL      "[ESTWAVER]"      /* value of the magic number of meta DB */

#define QUEUEFILE      "_queue"          /* name of the queue file */
#define QUEUELRM       77                /* records in a leaf node of the queue */
#define QUEUENIM       192               /* records in a non-leaf node of the queue */
#define QUEUELCN       2048              /* number of leaf cache of the queue */
#define QUEUENCN       512               /* number of non-leaf cache of the queue */

#define TRACEFILE      "_trace"          /* name of the trace file */
#define TRACEBNUM      425977            /* bucket number of the trace database */
#define TRACEDNUM      3                 /* division number of the trace database */

#define DATTRDEPTH     "_depth"          /* name of the attribute of the depth */
#define DRAFTCMD       "[DRAFT]"         /* built-in command for document draft */
#define TEXTCMD        "[TEXT]"          /* built-in command for plain text */
#define HTMLCMD        "[HTML]"          /* built-in command for HTML */
#define MIMECMD        "[MIME]"          /* built-in command for MIME */

enum {                                   /* enumeration for initializing options */
  WI_PERFNG = 1 << 10,                   /* use perfect N-gram analyzer */
  WI_CHRCAT = 1 << 11,                   /* use character category analyzer */
  WI_SMALL = 1 << 20,                    /* small tuning */
  WI_LARGE = 1 << 21,                    /* large tuning */
  WI_HUGE = 1 << 22,                     /* huge tuning */
  WI_SCVOID = 1 << 25,                   /* store scores as void */
  WI_SCINT = 1 << 26,                    /* store scores as integer */
  WI_SCASIS = 1 << 27                    /* refrain from adjustment of scores */
};

enum {                                   /* enumeration for running modes */
  LL_DEBUG = 1,                          /* debug */
  LL_INFO = 2,                           /* information */
  LL_WARN = 3,                           /* warning */
  LL_ERROR = 4,                          /* error */
  LL_NONE = 5,                           /* none */
  LL_CHECK = 6                           /* check to open */
};

enum {                                   /* enumeration for crawling strategy */
  CS_BALANCED,                           /* balanced of depth, width, and similarity */
  CS_SIMILARITY,                         /* similarity first */
  CS_DEPTH,                              /* depth first */
  CS_WIDTH,                              /* width first */
  CS_RANDOM                              /* at random */
};

typedef struct {                         /* type of structure for priority queue */
  VILLA *db;                             /* internal database */
  double max;                            /* maximum point */
} QUEUE;

typedef struct {                         /* type of structure for a keyword */
  const char *word;                      /* face of keyword */
  int wsiz;                              /* size of the keyword */
  int pt;                                /* score tuned by TF-IDF */
} KEYSC;

typedef struct {                         /* type of structure for a URL normalization */
  void *regex;                           /* regular expressions */
  char *before;                          /* before substring */
  char *after;                           /* after substring */
} UNRULE;

typedef struct {                         /* type of structure for a permission */
  void *regex;                           /* regular expressions */
  int visit;                             /* to be visited */
  int index;                             /* to be indexed */
} PMRULE;

typedef struct {                         /* type of structure for a URL rule */
  void *regex;                           /* regular expressions */
  char *type;                            /* media type */
} URLRULE;

typedef struct {                         /* type of structure for a media type rule */
  void *regex;                           /* regular expressions */
  char *filter;                          /* filter command */
} MTRULE;

typedef struct {                         /* type of structure for waver */
  char *rootdir;                         /* root directory */
  DEPOT *metadb;                         /* meta database */
  QUEUE *queue;                          /* priority queue */
  CURIA *trace;                          /* tracking records */
  ESTMTDB *index;                        /* document index */
  CBMAP *seeds;                          /* seed map */
  CBMAP *kwords;                         /* keyword map */
  CBMAP *sites;                          /* site map */
  char *pxhost;                          /* host name of the proxy */
  int pxport;                            /* port number of the proxy */
  int interval;                          /* interval time */
  int timeout;                           /* timeout of each request */
  int strategy;                          /* crawling strategy */
  double inherit;                        /* inheritance ratio of similarity */
  int seeddepth;                         /* maximum depth of seed documents */
  int maxdepth;                          /* maximum depth of recursion */
  int masscheck;                         /* standard value for checking mass sites */
  int queuesize;                         /* size of the priority queue */
  CBLIST *unrules;                       /* rules of URL normalization */
  CBLIST *pmrules;                       /* rules of permission */
  CBLIST *urlrules;                      /* rules of URL */
  CBLIST *mtrules;                       /* rules of media types */
  int language;                          /* preferred language */
  int textlimit;                         /* text size limitation */
  int seedkeynum;                        /* total number of keywords for seed documents */
  int savekeynum;                        /* number of keywords saved for each document */
  int thnum;                             /* number of threads */
  int docnum;                            /* total number of documents */
  int period;                            /* running time period */
  int revisit;                           /* revisit span */
  size_t cachesize;                      /* maximum size of the index cache */
  CBMAP *nodes;                          /* remote nodes for alternative indexes */
  char *draftdir;                        /* path of the draft directory */
  char *entitydir;                       /* path of the entity directory */
  char *postproc;                        /* postprocessor for retrieved files */
  time_t stime;                          /* start time */
  int curnum;                            /* current number of documents */
  int curnode;                           /* current using node */
  double minload;                        /* minimum load of nodes */
} WAVER;


/* The handles of the log file. */
extern FILE *log_fp;


/* The level of logging. */
extern int log_level;


/* Open the log file.
   `rootdir' specifies the path of the root directory.
   `path' specifies the path of the log file.
   `level' specifies the leve of logging.
   `trunc' specifies whether to truncate the log file.
   The return value is true if success, else it is false. */
int log_open(const char *rootdir, const char *path, int level, int trunc);


/* Print formatted string into the log file. */
void log_print(int level, const char *format, ...);


/* Initialize the root directory.
   `rootdir' specifies the path of the root directory.
   `options' specifies the options: `WI_PERFNG', `WI_CHRCAT', `WI_SMALL', `WI_LARGE', `WI_HUGE',
   `WI_SCVOID', `WI_SCINT', and `WI_ACASIS'.
   The return value is true if success, else it is false. */
int waver_init(const char *rootdir, int options);


/* Open a waver handle.
   `rootdir' specifies the path of the root directory.
   The return value is the waver handle or `NULL' on failure. */
WAVER *waver_open(const char *rootdir);


/* Close a waver handle.
   `waver' specifies a waver handle.
   The return value is true if success, else it is false. */
int waver_close(WAVER *waver);


/* Set the current node.
   `waver' specifies a waver handle. */
void waver_set_current_node(WAVER *waver);


/* Get the load of the current node.
   `waver' specifies a waver handle.
   the return value is the load of the current node. */
double waver_current_node_load(WAVER *waver);


/* Add a document to a node.
   `waver' specifies a waver handle.
   `doc' specifies a document object.
   `codep' specifies the pointer to a variable to which the status code of respnese is
   assigned.  If it is `NULL', it is not used.
   The return value is true if success, else it is false. */
int waver_node_put_doc(WAVER *waver, ESTDOC *doc, int *codep);


/* Remove a document from a node.
   `waver' specifies a waver handle.
   `url' specifies the URL of a document.
   `codep' specifies the pointer to a variable to which the status code of respnese is
   assigned.  If it is `NULL', it is not used.
   The return value is true if success, else it is false. */
int waver_node_out_doc(WAVER *waver, const char *url, int *codep);


/* Open a priority queue.
   `name' specifies the name of a database file.
   The return value is the queue handle or `NULL' on failure. */
QUEUE *queue_open(const char *name);


/* Close a priority queue.
   `queue' specifies a queue handle.
   The return value is true if successful, or false on failure. */
int queue_close(QUEUE *queue);


/* Set the range of the priority space of a priority queue.
   `queue' specifies a queue handle.
   `range' specifies the range of the priority space. */
void queue_set_range(QUEUE *queue, double range);


/* Enqueue a record into a priority queue.
   `queue' specifies a queue handle.
   `str' specifies a string.
   `priority' specifies the priority between 0.0 and 1.0.
   The return value is true if successful, or false on failure. */
int queue_enqueue(QUEUE *queue, const char *str, double priority);


/* Dequeue a record from a priority queue.
   `queue' specifies a queue handle.
   The return value is the pointer to a record or `NULL' if no record exists.
   Because the region of the return value is allocated with the `malloc' call, it should be
   released with the `free' call if it is no longer in use. */
char *queue_dequeue(QUEUE *queue);


/* Get the number of records in a priority queue.
   `queue' specifies a queue handle.
   The return value is the number of records. */
int queue_rnum(QUEUE *queue);


/* Discard inferior records in a priority queue.
   `queue' specifies a queue handle.
   `num' specifies the number of records to be kept.
   The return value is true if successful, or false on failure. */
int queue_slim(QUEUE *queue, int num);


/* Add a word to a keyword map.
   `kwords' specifies a keyword map handle.
   `word' specifies the string of a word.
   `frequency' specifies the frequency of the word. */
void kwords_add(CBMAP *kwords, const char *word, int frequency);


/* Reduce elements of a keyword map.
   `kwords' specifies a keyword map handle.
   `num' specifies the number of elements after reduction of the keyword map.
   `fadeout' specifies whether scores do fade-out. */
void kwords_reduce(CBMAP *kwords, int num, int fadeout);


/* Fetch a document of a URL.
   `url' specifies the URL of a document.
   `pxhost' specifies the host name of a proxy.  If it is `NULL', it is not used.
   `pxport' specifies the port number of the proxy.
   `outsec' specifies timeout in seconds.  If it is negative, it is not used.
   `mdate' specifies the last-modified date.  If it is not more than 0, it is not used.
   `urlrules' specifies a list object conteining type rules of URLs.  If it is `NULL', the
   default rule is applied.
   `mtrules' specifies a list object conteining filter rules of media types.  If it is `NULL',
   the default rule is applied.
   `codep' specifies the pointer to a variable to which the status code of respnese is
   assigned.  If it is `NULL', it is not used.
   `raw' specifies a datum handle to store raw data.  If it is `NULL', it is not used.
   `heads' specifies a map handle to store HTTP headers.  If it is `NULL', it is not used.
   `links' specifies a list handle to store links.  If it is `NULL', it is not used.
   `unrules' specifies a list object conteining URL normalization rules.  If it is `NULL', it is
   not used.
   `doc' specifies a document handle to store attributes and texts.  If it is `NULL', it is not
   used.
   `lang' specifies the code of preferred language.
   The return value is true if success, else it is false. */
int fetch_document(const char *url, const char *pxhost, int pxport, int outsec, time_t mdate,
                   const CBLIST *urlrules, const CBLIST *mtrules,
                   int *codep, CBDATUM *raw, CBMAP *heads,
                   CBLIST *links, const CBLIST *unrules, ESTDOC *doc, int lang);



#endif                                   /* duplication check */


/* END OF FILE */
