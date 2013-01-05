/*************************************************************************************************
 * Common modules related to estmaster
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


#ifndef _MASTERMOD_H                     /* duplication check */
#define _MASTERMOD_H

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
#define HOSTBUFSIZ     256               /* size of a buffer for a host name */
#define ADDRBUFSIZ     48                /* size of a buffer for an address */
#define NODENAMEMAX    128               /* maximum length of node name  */
#define MINIBNUM       31                /* bucket number of a small map */
#define KWORDNUM       32                /* number of shown keywords */
#define CONDATTRMAX    9                 /* maximum number of attribute conditions */
#define DEFMAXSRCH     10                /* default max number of retrieved documents */
#define SELFCREDIT     10000             /* credit of the node itself */

#define CONFFILE       "_conf"           /* name of the configuration file */
#define USERFILE       "_user"           /* name of the user list file */
#define LOGFILE        "_log"            /* name of the log file */
#define METAFILE       "_meta"           /* name of the meta database */
#define PIDFILE        "_pid"            /* name of the process ID file */
#define STOPFILE       "_stop"           /* name of the process ID file */
#define DFDBFILE       "_dfdb"           /* name of the document frequency database */
#define NODEDIR        "_node"           /* name of the node directory */
#define SESSDIR        "_sess"           /* name of the session directory */
#define NULLDEV        "/dev/null"       /* path of null device */

#define INFORMHELPER   "estbutler inform"    /* command name of the inform helper */
#define SEARCHHELPER   "estbutler search"    /* command name of the search helper */
#define GETDOCHELPER   "estbutler getdoc"    /* command name of the get_doc helper */
#define GETDOCATTRHELPER  "estbutler getdocattr"  /* command name of the get_doc helper */
#define ETCHDOCHELPER  "estbutler etchdoc"   /* command name of the etch_doc helper */
#define URITOIDHELPER  "estbutler uritoid"   /* command name of the uri_to_id helper */

#define MMKMAGIC       "magic"           /* meta key of the magic number of meta DB */
#define MMKMAGVAL      "[ESTMASTER]"     /* value of the magic number of meta DB */
#define NMKNAME        "name"            /* meta key of the name of node DB */
#define NMKLABEL       "label"           /* meta key of the label of node DB */
#define NMKADMINS      "admins"          /* meta key of the administrators of node DB */
#define NMKUSERS       "users"           /* meta key of the users of node DB */
#define NMKLINKS       "links"           /* meta key of the links of node DB */

#define DATTRNDURL     "#nodeurl"        /* name of the pseudo-attribute of the node URL */
#define DATTRNDLABEL   "#nodelabel"      /* name of the pseudo-attribute of the node label */
#define DATTRNDSCORE   "#nodescore"      /* name of the pseudo-attribute of the node score */
#define DATTRLFILE     "_lfile"          /* name of the attribute of the file name */

enum {                                   /* enumeration for running modes */
  RM_NORMAL = 1,                         /* normal */
  RM_RDONLY = 2                          /* read only */
};

enum {                                   /* enumeration for running modes */
  AM_NONE = 1,                           /* none */
  AM_ADMIN = 2,                          /* admin */
  AM_ALL = 3                             /* all */
};

enum {                                   /* enumeration for running modes */
  LL_DEBUG = 1,                          /* debug */
  LL_INFO = 2,                           /* information */
  LL_WARN = 3,                           /* warning */
  LL_ERROR = 4,                          /* error */
  LL_NONE = 5,                           /* none */
  LL_CHECK = 6                           /* check to open */
};

enum {                                   /* enumeration for scale prediction */
  SP_SMALL = 1,                          /* small */
  SP_MEDIUM = 2,                         /* medium */
  SP_LARGE = 3,                          /* large */
  SP_HUGE = 4                            /* huge */
};

enum {                                   /* enumeration for scoring expression modes */
  SE_VOID = 1,                           /* void */
  SE_CHAR = 2,                           /* char */
  SE_INT = 3,                            /* int */
  SE_ASIS = 4                            /* asis */
};

enum {                                   /* enumeration for UI operations */
  MM_SCORE = 1,                          /* score */
  MM_SCRK = 2,                           /* score and rank */
  MM_RANK = 3                            /* rank */
};

enum {                                   /* enumeration for phrase mode */
  PM_USUAL = 1,                          /* usual phrase */
  PM_SIMPLE = 2,                         /* simplified phrase */
  PM_ROUGH = 3,                          /* rough phrase */
  PM_UNION = 4,                          /* union phrase */
  PM_ISECT = 5                           /* intersection phrase */
};

typedef struct {                         /* type of structure for a user object */
  char *name;                            /* unique name */
  char *passwd;                          /* encrypted password */
  char *flags;                           /* flags */
  char *fname;                           /* full name */
  char *misc;                            /* miscellaneous */
  time_t atime;                          /* last access time */
  CBMAP *sess;                           /* session variables */
  pthread_mutex_t mutex;                 /* mutex per user */
} USER;

typedef struct {                         /* type of structure for a user manager object */
  char *rootdir;                         /* path of the root directory */
  CBMAP *users;                          /* map of names and entities */
} UMGR;

typedef struct {                         /* type of structure for a node object */
  ESTMTDB *db;                           /* index database */
  char *name;                            /* unique name of the index */
  char *label;                           /* label for display */
  CBMAP *admins;                         /* set of ID numbers of administrators */
  CBMAP *users;                          /* set of ID numbers of users */
  CBMAP *links;                          /* list of expressions of links */
  time_t mtime;                          /* modification date */
  int dirty;                             /* whether to have dirty cache */
  pthread_mutex_t mutex;                 /* mutex per node */
} NODE;

typedef struct {                         /* type of structure for a node manager object */
  char *rootdir;                         /* path of the root directory */
  CBMAP *nodes;                          /* map of names and entities */
  CBMAP *aidxs;                          /* map of attribute indexes */
} NMGR;

typedef struct {                         /* type of structure for a read/write lock object */
  int readers;                           /* number of readers */
  int writers;                           /* number of writers */
  pthread_mutex_t mutex;                 /* mutex per lock */
  pthread_cond_t cond;                   /* conditional variable */
} RWLOCK;

typedef struct {                         /* type of structure for a document in result */
  int score;                             /* score */
  ESTDOC *doc;                           /* document object */
  CBMAP *attrs;                          /* map object for attributes */
  char *body;                            /* body data */
  const char *value;                     /* value of an attribute for sorting */
} RESDOC;

typedef struct {                         /* type of structure for documents in result */
  CBMAP *uris;                           /* map of URIs and entities */
  pthread_mutex_t mutex;                 /* mutex per result */
} RESMAP;


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


/* Rotete the log file.
   `rootdir' specifies the path of the root directory.
   `path' specifies the path of the log file.
   The return value is true if success, else it is false. */
int log_rotate(const char *rootdir, const char *path);


/* Initialize the root directory.
   `rootdir' specifies the path of the root directory.
   The return value is true if success, else it is false. */
int master_init(const char *rootdir);


/* Get the PID of the process locking the root directory.
   `rootdir' specifies the path of the root directory.
   The return value is the PID of the process. */
int lockerpid(const char *rootdir);


/* Check whether a name includes alpha numeric characters only.
   `name' specifies a name.
   The return value is true if so or false if not. */
int check_alnum_name(const char *name);


/* Create a user manager object.
   `rootdir' specifies the path of the root directory.
   The return value is a user manager object. */
UMGR *umgr_new(const char *rootdir);


/* Destroy a user manager object.
   `umgr' specifies a user manager object.
   The return value is true if success, else it is false. */
int umgr_delete(UMGR *umgr);


/* Load all users from the user file.
   `umgr' specifies a user manager object.
   The return value is true if success, else it is false. */
int umgr_load(UMGR *umgr);


/* Synchronize all users into the user file.
   `umgr' specifies a user manager object.
   The return value is true if success, else it is false. */
int umgr_sync(UMGR *umgr);


/* Add a user to a user manager object.
   `umgr' specifies a user manager object.
   `name' specifies the unique name of a user.
   `passwd' specifies the encrypted password of the user.
   `flags' specifies the flags of the user.
   `fname' specifies the full name of the user.
   `misc' specifies the miscellaneous information of the user.
   The return value is true if success, else it is false. */
int umgr_put(UMGR *umgr, const char *name, const char *passwd, const char *flags,
             const char *fname, const char *misc);


/* Remove a user from a user manager object.
   `umgr' specifies a user manager object.
   `name' specifies the unique name of a user.
   The return value is true if success, else it is false. */
int umgr_out(UMGR *umgr, const char *name);


/* Get a list of names of users in a user manager object.
   `umgr' specifies a user manager object.
   The return value is a list object of names of users in a user manager object.
   The returned object should be destroyed with `cblistclose'. */
CBLIST *umgr_names(UMGR *umgr);


/* Get a user object in a user manager object.
   `umgr' specifies a user manager object.
   `name' specifies the unique name of a user.
   The return value is a user object or `NULL' on failure. */
USER *umgr_get(UMGR *umgr, const char *name);


/* Make the session of a user object.
   `user' specifies a user object. */
void user_make_sess(USER *user);


/* Clear the session of a user object.
   `user' specifies a user object. */
void user_clear_sess(USER *user);


/* Set a session variable of a user object.
   `user' specifies a user object.
   `name' specifies the name of a variable.
   `value' specifies the value of the variable.  If it is `NULL', the variable is deleted. */
void user_set_sess_val(USER *user, const char *name, const char *value);


/* Get the value of a session variable of a user object.
   `user' specifies a user object.
   `name' specifies the name of a variable.
   The return value is the value of the session variable or `NULL' if it does not exist.
   Because the region of the return value is  allocated with the `malloc' call, it should be
   released with the `free' call if it is no longer in use. */
char *user_sess_val(USER *user, const char *name);


/* Create a node manager object.
   `rootdir' specifies the path of the root directory.
   The return value is a node manager object. */
NMGR *nmgr_new(const char *rootdir);


/* Destroy a node manager object.
   `nmgr' specifies a node manager object.
   The return value is true if success, else it is false. */
int nmgr_delete(NMGR *nmgr);


/* Load all nodes from the node directory.
   `nmgr' specifies a node manager object.
   `wmode' specifies whether the node is a writer or a reader.
   The return value is true if success, else it is false. */
int nmgr_load(NMGR *nmgr, int wmode);


/* Synchronize all nodes into the node directory.
   `nmgr' specifies a node manager object.
   `phis' specifies whether to synchronize phisically.
   The return value is true if success, else it is false. */
int nmgr_sync(NMGR *nmgr, int phis);


/* Add an attribute index to a node manager object.
   `nmgr' specifies a node manager object.
   `name' specifies the name of a target attribute.
   `type' specifies the expression of a data type. */
void nmgr_add_aidx(NMGR *nmgr, const char *name, const char *type);


/* Add a node to a node manager object.
   `nmgr' specifies a node manager object.
   `name' specifies the unique name of a node.
   `wmode' specifies whether the node is a writer or a reader.
   `options' specifies options for the database of the node.
   The return value is true if success, else it is false. */
int nmgr_put(NMGR *nmgr, const char *name, int wmode, int options);


/* Remove a node from a node manager object.
   `nmgr' specifies a node manager object.
   `name' specifies the unique name of a node.
   The return value is true if success, else it is false. */
int nmgr_out(NMGR *nmgr, const char *name);


/* Clear registered documents in a node in a node manager object.
   `nmgr' specifies a node manager object.
   `name' specifies the unique name of a node.
   `options' specifies options for the database of the node.
   The return value is true if success, else it is false. */
int nmgr_clear(NMGR *nmgr, const char *name, int options);


/* Get a list of names of nodes in a noder manager object.
   `nmgr' specifies a node manager object.
   The return value is a list object of names of nodes in a node manager object.
   The returned object should be destroyed with `cblistclose'. */
CBLIST *nmgr_names(NMGR *nmgr);


/* Get a node object in a node manager object.
   `nmgr' specifies a node manager object.
   `name' specifies the unique name of a node.
   The return value is a node object or `NULL' on failure. */
NODE *nmgr_get(NMGR *nmgr, const char *name);


/* Set a link object of a node.
   `node' specifies a node object.
   `url' specifies the URL of a link object.
   `label' specifies the label of the link object.  If it is `NULL', the link is removed.
   `credit' specifies the credit of the link object. */
void node_set_link(NODE *node, const char *url, const char *label, int credit);


/* Get a vector of keywords.
   `node' specifies a node object.
   `id' specifies the ID number of a document.
   The return value is a map object of keywords.
   The returned object should be destroyed with `cbmapclose'. */
CBMAP *node_etch_doc(NODE *node, int id);


/* Create a read-write lock object.
   The return value is a read-write lock object. */
RWLOCK *rwlock_new(void);


/* Destroy a read-write lock object.
   `rwlock' specifies a read-write lock object. */
void rwlock_delete(RWLOCK *rwlock);


/* Lock a read-write lock object.
   `rwlock' specifies a read-write lock object.
   `wmode' specifies whether the lock is writer.
   The return value is true if success, else it is false. */
int rwlock_lock(RWLOCK *rwlock, int wmode);


/* Unlock a read-write lock object.
   `rwlock' specifies a read-write lock object.
   The return value is true if success, else it is false. */
int rwlock_unlock(RWLOCK *rwlock);


/* Get the number of readers locking a read-write lock object.
   `rwlock' specifies a read-write lock object.
   The return value is the number of readers locking a read-write lock object. */
int rwlock_rnum(RWLOCK *rwlock);


/* Create a result map object.
   The return value is a result map object. */
RESMAP *resmap_new(void);


/* Destroy a result map object.
   `resmap' specifies a result map object. */
void resmap_delete(RESMAP *resmap);


/* Add a result document data to a result map object.
   `resmap' specifies a result map object.
   `score' specifies the score of the document.
   `doc' specifies a document object of the document.  It is closed internally.
   `attrs' specifies the attributes of the document.  It can be `NULL'.  It is deleted internally.
   `body' specifies the body data of the document.  It can be `NULL'.  It is freed internally. */
void resmap_put(RESMAP *resmap, int score, ESTDOC *doc, CBMAP *attrs, char *body);


/* Get a list object of result objects in a result map objects.
   `resmap' specifies a result map object.
   `nump' specifies the pointer to a variable to which the number of elements in the result is
   assigned.
   `order' specifies an expression for the order.  If it is `NULL', the order is by score
   descending.
   `distinct' specifies the name of the distinct attribute.  If it is `NULL', no filter is used.
   The return value is an array whose elements are result document objects.  Because the region
   of the return value is allocated with the `malloc' call, it should be released with the `free'
   call if it is no longer in use. */
RESDOC **resmap_list(RESMAP *resmap, int *nump, const char *order, const char *distinct);


/* Be a daemon process.
   `curdir' specifies the path of the current directory of the daemon.
   The return value is true if success, else it is false. */
int be_daemon(const char *curdir);



#endif                                   /* duplication check */


/* END OF FILE */
