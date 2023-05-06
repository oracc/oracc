/*************************************************************************************************
 * Implementation of the MT-safe API
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

#define ESTMINIBNUM    31                /* bucket number of map for attributes */


/* private function prototypes */
static int est_global_lock(void);
static void est_global_unlock(void);
static int est_mtdb_lock(ESTMTDB *db);
static void est_mtdb_unlock(ESTMTDB *db);



/*************************************************************************************************
 * API for MT-safe database
 *************************************************************************************************/


/* Global mutex. */
pthread_mutex_t est_global_mutex = PTHREAD_MUTEX_INITIALIZER;
CBMAP *est_global_db_names = NULL;


/* Open a database. */
ESTMTDB *est_mtdb_open(const char *name, int omode, int *ecp){
  ESTMTDB *mtdb;
  ESTDB *db;
  char *path;
  assert(name && ecp);
  if(!est_global_lock()){
    *ecp = ESTELOCK;
    return NULL;
  }
  if(!est_global_db_names){
    est_global_db_names = cbmapopenex(ESTMINIBNUM);
    cbglobalgc(est_global_db_names, (void (*)(void *))cbmapclose);
  }
  path = est_realpath(name);
  if(cbmapget(est_global_db_names, path, -1, NULL)){
    free(path);
    *ecp = ESTEACCES;
    est_global_unlock();
    return NULL;
  }
  mtdb = cbmalloc(sizeof(ESTMTDB));
  if(!(db = est_db_open(name, omode, ecp))){
    free(mtdb);
    free(path);
    est_global_unlock();
    return NULL;
  }
  free(path);
  path = est_realpath(name);
  cbmapput(est_global_db_names, path, -1, "", 0, FALSE);
  mtdb->db = db;
  mtdb->path = path;
  pthread_mutex_init(&(mtdb->mutex), NULL);
  est_global_unlock();
  return mtdb;
}


/* Close a database. */
int est_mtdb_close(ESTMTDB *db, int *ecp){
  int err;
  assert(db && ecp);
  if(!est_global_lock()){
    *ecp = ESTELOCK;
    return FALSE;
  }
  err = FALSE;
  cbmapout(est_global_db_names, db->path, -1);
  pthread_mutex_destroy(&(db->mutex));
  free(db->path);
  if(!est_db_close(db->db, ecp)) err = TRUE;
  free(db);
  est_global_unlock();
  return err ? FALSE : TRUE;
}


/* Get the last happended error code of a database. */
int est_mtdb_error(ESTMTDB *db){
  int rv;
  assert(db);
  if(!est_mtdb_lock(db)) return ESTELOCK;
  rv = est_db_error(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Check whether a database has a fatal error. */
int est_mtdb_fatal(ESTMTDB *db){
  int rv;
  assert(db);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_fatal(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Add an index for narrowing or sorting with document attributes. */
int est_mtdb_add_attr_index(ESTMTDB *db, const char *name, int type){
  int rv;
  assert(db && name);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_add_attr_index(db->db, name, type);
  est_mtdb_unlock(db);
  return rv;
}


/* Flush index words in the cache of a database. */
int est_mtdb_flush(ESTMTDB *db, int max){
  int rv;
  assert(db);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_flush(db->db, max);
  est_mtdb_unlock(db);
  return rv;
}


/* Synchronize updating contents of a database. */
int est_mtdb_sync(ESTMTDB *db){
  int rv;
  assert(db);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_sync(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Optimize a database. */
int est_mtdb_optimize(ESTMTDB *db, int options){
  int rv;
  assert(db);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_optimize(db->db, options);
  est_mtdb_unlock(db);
  return rv;
}


/* Merge another database. */
int est_mtdb_merge(ESTMTDB *db, const char *name, int options){
  int rv;
  assert(db && name);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_merge(db->db, name, options);
  est_mtdb_unlock(db);
  return rv;
}


/* Add a document to a database. */
int est_mtdb_put_doc(ESTMTDB *db, ESTDOC *doc, int options){
  int rv;
  assert(db && doc);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_put_doc(db->db, doc, options);
  est_mtdb_unlock(db);
  return rv;
}


/* Remove a document from a database. */
int est_mtdb_out_doc(ESTMTDB *db, int id, int options){
  int rv;
  assert(db && id > 0);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_out_doc(db->db, id, options);
  est_mtdb_unlock(db);
  return rv;
}


/* Edit attributes of a document object in a database. */
int est_mtdb_edit_doc(ESTMTDB *db, ESTDOC *doc){
  int rv;
  assert(db && doc);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_edit_doc(db->db, doc);
  est_mtdb_unlock(db);
  return rv;
}


/* Retrieve a document in a database. */
ESTDOC *est_mtdb_get_doc(ESTMTDB *db, int id, int options){
  ESTDOC *rv;
  assert(db && id > 0);
  if(!est_mtdb_lock(db)) return NULL;
  rv = est_db_get_doc(db->db, id, options);
  est_mtdb_unlock(db);
  return rv;
}


/* Retrieve the value of an attribute of a document in a database. */
char *est_mtdb_get_doc_attr(ESTMTDB *db, int id, const char *name){
  char *rv;
  assert(db && id > 0 && name);
  if(!est_mtdb_lock(db)) return NULL;
  rv = est_db_get_doc_attr(db->db, id, name);
  est_mtdb_unlock(db);
  return rv;
}


/* Get the ID of a document spacified by URI. */
int est_mtdb_uri_to_id(ESTMTDB *db, const char *uri){
  int rv;
  assert(db && uri);
  if(!est_mtdb_lock(db)) return -1;
  rv = est_db_uri_to_id(db->db, uri);
  est_mtdb_unlock(db);
  return rv;
}


/* Get the name of a database. */
const char *est_mtdb_name(ESTMTDB *db){
  const char *rv;
  assert(db);
  if(!est_mtdb_lock(db)) return "";
  rv = est_db_name(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Get the number of documents in a database. */
int est_mtdb_doc_num(ESTMTDB *db){
  int rv;
  assert(db);
  if(!est_mtdb_lock(db)) return 0;
  rv = est_db_doc_num(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Get the number of unique words in a database. */
int est_mtdb_word_num(ESTMTDB *db){
  int rv;
  assert(db);
  if(!est_mtdb_lock(db)) return 0;
  rv = est_db_word_num(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Get the size of a database. */
double est_mtdb_size(ESTMTDB *db){
  double rv;
  assert(db);
  if(!est_mtdb_lock(db)) return 0.0;
  rv = est_db_size(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Search documents corresponding a condition for a database. */
int *est_mtdb_search(ESTMTDB *db, ESTCOND *cond, int *nump, CBMAP *hints){
  int *rv;
  assert(db && cond && nump);
  if(!est_mtdb_lock(db)){
    est_db_set_ecode(db->db, ESTELOCK);
    cbmapput(hints, "", 0, "0", -1, TRUE);
    *nump = 0;
    return cbmalloc(1);
  }
  rv = est_db_search(db->db, cond, nump, hints);
  est_mtdb_unlock(db);
  return rv;
}


/* Search plural databases for documents corresponding a condition. */
int *est_mtdb_search_meta(ESTMTDB **dbs, int dbnum, ESTCOND *cond, int *nump, CBMAP *hints){
  ESTDB **cdbs;
  CBMAP *umap;
  const char *kbuf;
  int i, *rv;
  assert(dbs && dbnum >= 0 && cond && nump);
  cdbs = cbmalloc(dbnum * sizeof(ESTDB *) + 1);
  if(dpisreentrant){
    umap = cbmapopenex(dbnum + 1);
    for(i = 0; i < dbnum; i++){
      if(!cbmapget(umap, (char *)&(dbs[i]), sizeof(ESTDB *), NULL) && !est_mtdb_lock(dbs[i])){
        while(--i >= 0){
          est_mtdb_unlock(dbs[i]);
        }
        free(cdbs);
        est_db_set_ecode(cdbs[i], ESTELOCK);
        cbmapput(hints, "", 0, "0", -1, TRUE);
        *nump = 0;
        return cbmalloc(1);
      }
      cbmapput(umap, (char *)&(dbs[i]), sizeof(ESTDB *), "", 0, FALSE);
      cdbs[i] = dbs[i]->db;
    }
    rv = est_db_search_meta(cdbs, dbnum, cond, nump, hints);
    cbmapiterinit(umap);
    while((kbuf = cbmapiternext(umap, NULL)) != NULL){
      est_mtdb_unlock(*(ESTMTDB **)kbuf);
    }
    cbmapclose(umap);
  } else {
    if(pthread_mutex_lock(&est_global_mutex) != 0){
      *nump = 0;
      return cbmalloc(1);
    }
    for(i = 0; i < dbnum; i++){
      cdbs[i] = dbs[i]->db;
    }
    rv = est_db_search_meta(cdbs, dbnum, cond, nump, hints);
    pthread_mutex_unlock(&est_global_mutex);
  }
  free(cdbs);
  return rv;
}


/* Check whether a document object matches the phrase of a search condition object definitely. */
int est_mtdb_scan_doc(ESTMTDB *db, ESTDOC *doc, ESTCOND *cond){
  int rv;
  assert(db && doc && cond);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_scan_doc(db->db, doc, cond);
  est_mtdb_unlock(db);
  return rv;
}


/* Set the maximum size of the cache memory of a database. */
void est_mtdb_set_cache_size(ESTMTDB *db, size_t size, int anum, int tnum, int rnum){
  assert(db);
  if(!est_mtdb_lock(db)) return;
  est_db_set_cache_size(db->db, size, anum, tnum, rnum);
  est_mtdb_unlock(db);
}


/* Add a pseudo index directory to a database. */
int est_mtdb_add_pseudo_index(ESTMTDB *db, const char *path){
  int rv;
  assert(db && path);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_add_pseudo_index(db->db, path);
  est_mtdb_unlock(db);
  return rv;
}



/*************************************************************************************************
 * features for experts
 *************************************************************************************************/


/* Set the error code of a database. */
void est_mtdb_set_ecode(ESTMTDB *db, int ecode){
  assert(db);
  if(!est_mtdb_lock(db)) return;
  est_db_set_ecode(db->db, ecode);
  est_mtdb_unlock(db);
}


/* Check whether an option is set. */
int est_mtdb_check_option(ESTMTDB *db, int option){
  int rv;
  assert(db);
  if(!est_mtdb_lock(db)) return -1;
  rv = est_db_check_option(db->db, option);
  est_mtdb_unlock(db);
  return rv;
}


/* Get the inode number of a database. */
int est_mtdb_inode(ESTMTDB *db){
  int rv;
  assert(db);
  if(!est_mtdb_lock(db)) return -1;
  rv = est_db_inode(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Set the entity data of a document in a database. */
int est_mtdb_set_doc_entity(ESTMTDB *db, int id, const char *ptr, int size){
  int rv;
  assert(db && id > 0);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_set_doc_entity(db->db, id, ptr, size);
  est_mtdb_unlock(db);
  return rv;
}


/* Get the entity data of a document in a database. */
char *est_mtdb_get_doc_entity(ESTMTDB *db, int id, int *sp){
  char *rv;
  assert(db && id > 0 && sp);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_get_doc_entity(db->db, id, sp);
  est_mtdb_unlock(db);
  return rv;
}


/* Set the maximum number of extension of wild cards. */
void est_mtdb_set_wildmax(ESTMTDB *db, int num){
  assert(db && num >= 0);
  if(!est_mtdb_lock(db)) return;
  est_db_set_wildmax(db->db, num);
  est_mtdb_unlock(db);
}


/* Add a piece of meta data to a database. */
void est_mtdb_add_meta(ESTMTDB *db, const char *name, const char *value){
  assert(db && name);
  if(!est_mtdb_lock(db)) return;
  est_db_add_meta(db->db, name, value);
  est_mtdb_unlock(db);
}


/* Get a list of names of meta data of a database. */
CBLIST *est_mtdb_meta_names(ESTMTDB *db){
  CBLIST *rv;
  assert(db);
  if(!est_mtdb_lock(db)) return NULL;
  rv = est_db_meta_names(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Get the value of a piece of meta data of a database. */
char *est_mtdb_meta(ESTMTDB *db, const char *name){
  char *rv;
  assert(db && name);
  if(!est_mtdb_lock(db)) return NULL;
  rv = est_db_meta(db->db, name);
  est_mtdb_unlock(db);
  return rv;
}


/* Extract keywords of a document object. */
CBMAP *est_mtdb_etch_doc(ESTMTDB *db, ESTDOC *doc, int max){
  CBMAP *rv;
  assert(doc && max >= 0);
  if(!db) return est_db_etch_doc(NULL, doc, max);
  if(!est_mtdb_lock(db)) return cbmapopenex(1);
  rv = est_db_etch_doc(db->db, doc, max);
  est_mtdb_unlock(db);
  return rv;
}


/* Store a map object of keywords. */
int est_mtdb_put_keywords(ESTMTDB *db, int id, CBMAP *kwords, double weight){
  int rv;
  assert(db && id > 0 && kwords);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_put_keywords(db->db, id, kwords, weight);
  est_mtdb_unlock(db);
  return rv;
}


/* Remove keywords of a document. */
int est_mtdb_out_keywords(ESTMTDB *db, int id){
  int rv;
  assert(db && id > 0);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_out_keywords(db->db, id);
  est_mtdb_unlock(db);
  return rv;
}


/* Retrieve a map object of keywords. */
CBMAP *est_mtdb_get_keywords(ESTMTDB *db, int id){
  CBMAP *rv;
  assert(db && id > 0);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_get_keywords(db->db, id);
  est_mtdb_unlock(db);
  return rv;
}


/* Mesure the total size of each inner records of a stored document. */
int est_mtdb_measure_doc(ESTMTDB *db, int id, int parts){
  int rv;
  assert(db && id > 0);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_measure_doc(db->db, id, parts);
  est_mtdb_unlock(db);
  return rv;
}


/* Initialize the iterator of a database. */
int est_mtdb_iter_init(ESTMTDB *db, const char *prev){
  int rv;
  assert(db);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_iter_init(db->db, prev);
  est_mtdb_unlock(db);
  return rv;
}


/* Get the next ID of the iterator of a database. */
int est_mtdb_iter_next(ESTMTDB *db){
  int rv;
  assert(db);
  if(!est_mtdb_lock(db)) return -1;
  rv = est_db_iter_next(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Initialize the word iterator of a database. */
int est_mtdb_word_iter_init(ESTMTDB *db){
  int rv;
  assert(db);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_word_iter_init(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Get the next word of the word iterator of a database. */
char *est_mtdb_word_iter_next(ESTMTDB *db){
  char *rv;
  assert(db);
  if(!est_mtdb_lock(db)) return NULL;
  rv = est_db_word_iter_next(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Get the size of the record of a word. */
int est_mtdb_word_rec_size(ESTMTDB *db, const char *word){
  int rv;
  assert(db && word);
  if(!est_mtdb_lock(db)) return 0;
  rv = est_db_word_rec_size(db->db, word);
  est_mtdb_unlock(db);
  return rv;
}


/* Get the number of unique keywords in a database. */
int est_mtdb_keyword_num(ESTMTDB *db){
  int rv;
  assert(db);
  if(!est_mtdb_lock(db)) return 0;
  rv = est_db_keyword_num(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Initialize the keyword iterator of a database. */
int est_mtdb_keyword_iter_init(ESTMTDB *db){
  int rv;
  assert(db);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_keyword_iter_init(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Get the next keyword of the word iterator of a database. */
char *est_mtdb_keyword_iter_next(ESTMTDB *db){
  char *rv;
  assert(db);
  if(!est_mtdb_lock(db)) return NULL;
  rv = est_db_keyword_iter_next(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Get the size of the record of a keyword. */
int est_mtdb_keyword_rec_size(ESTMTDB *db, const char *word){
  int rv;
  assert(db && word);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_keyword_rec_size(db->db, word);
  est_mtdb_unlock(db);
  return rv;
}


/* Search documents corresponding a keyword for a database. */
int *est_mtdb_keyword_search(ESTMTDB *db, const char *word, int *nump){
  int *rv;
  assert(db && word && nump);
  if(!est_mtdb_lock(db)) return NULL;
  rv = est_db_keyword_search(db->db, word, nump);
  est_mtdb_unlock(db);
  return rv;
}


/* Get the number of records in the cache memory of a database. */
int est_mtdb_cache_num(ESTMTDB *db){
  int rv;
  assert(db);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_cache_num(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Get the size of used cache region. */
int est_mtdb_used_cache_size(ESTMTDB *db){
  int rv;
  assert(db);
  if(!est_mtdb_lock(db)) return FALSE;
  rv = est_db_used_cache_size(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Set the special cache for narrowing and sorting with document attributes. */
void est_mtdb_set_special_cache(ESTMTDB *db, const char *name, int num){
  assert(db && name && num >= 0);
  if(!est_mtdb_lock(db)) return;
  est_db_set_special_cache(db->db, name, num);
  est_mtdb_unlock(db);
}


/* Set the callback function to inform of database events. */
void est_mtdb_set_informer(ESTMTDB *db, void (*func)(const char *, void *), void *opaque){
  assert(db && func);
  if(!est_mtdb_lock(db)) return;
  est_db_set_informer(db->db, func, opaque);
  est_mtdb_unlock(db);
}


/* Fill the cache for keys for TF-IDF. */
void est_mtdb_fill_key_cache(ESTMTDB *db){
  assert(db);
  if(!est_mtdb_lock(db)) return;
  est_db_fill_key_cache(db->db);
  est_mtdb_unlock(db);
}


/* Set the database of document frequency. */
void est_mtdb_set_dfdb(ESTMTDB *db, DEPOT *dfdb){
  assert(db && dfdb);
  if(!est_mtdb_lock(db)) return;
  est_db_set_dfdb(db->db, dfdb);
  est_mtdb_unlock(db);
}


/* Clear the result cache. */
void est_mtdb_refresh_rescc(ESTMTDB *db){
  assert(db);
  if(!est_mtdb_lock(db)) return;
  est_db_refresh_rescc(db->db);
  est_mtdb_unlock(db);
}


/* Charge the result cache. */
void est_mtdb_charge_rescc(ESTMTDB *db, int max){
  assert(db);
  if(!est_mtdb_lock(db)) return;
  est_db_charge_rescc(db->db, max);
  est_mtdb_unlock(db);
}


/* Get a list of words in the result cache. */
CBLIST *est_mtdb_list_rescc(ESTMTDB *db){
  CBLIST *rv;
  assert(db);
  if(!est_mtdb_lock(db)) return cblistopen();
  rv = est_db_list_rescc(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Get the number of pseudo documents in a database. */
int est_mtdb_pseudo_doc_num(ESTMTDB *db){
  int rv;
  assert(db);
  if(!est_mtdb_lock(db)) return 0;
  rv = est_db_pseudo_doc_num(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Get a list of expressions of attribute indexes of a database. */
CBLIST *est_mtdb_attr_index_exprs(ESTMTDB *db){
  CBLIST *rv;
  assert(db);
  if(!est_mtdb_lock(db)) return 0;
  rv = est_db_attr_index_exprs(db->db);
  est_mtdb_unlock(db);
  return rv;
}


/* Interrupt long time processing. */
void est_mtdb_interrupt(ESTMTDB *db){
  assert(db);
  est_db_interrupt(db->db);
}



/*************************************************************************************************
 * features for hyper experts
 *************************************************************************************************/


/* Get the usage ratio of the cache. */
double est_mtdb_cache_usage(ESTMTDB *db){
  if(db->db->flsflag) return 1.0;
  return (double)db->db->icsiz / db->db->icmax;
}



/*************************************************************************************************
 * private objects
 *************************************************************************************************/


/* Lock the global environment.
   The return value is true if success, else it is false. */
static int est_global_lock(void){
  return pthread_mutex_lock(&est_global_mutex) == 0;
}


/* Unlock the global environment. */
static void est_global_unlock(void){
  pthread_mutex_unlock(&est_global_mutex);
}


/* Lock a database object.
   `db' specifies a database object.
   The return value is true if success, else it is false. */
static int est_mtdb_lock(ESTMTDB *db){
  assert(db);
  if(dpisreentrant){
    if(pthread_mutex_lock(&(db->mutex)) != 0){
      est_db_set_ecode(db->db, ESTELOCK);
      return FALSE;
    }
    return TRUE;
  }
  if(pthread_mutex_lock(&est_global_mutex) != 0){
    est_db_set_ecode(db->db, ESTELOCK);
    return FALSE;
  }
  return TRUE;
}


/* Unlock a database object.
   `db' specifies a database object. */
static void est_mtdb_unlock(ESTMTDB *db){
  assert(db);
  if(dpisreentrant){
    pthread_mutex_unlock(&(db->mutex));
  } else {
    pthread_mutex_unlock(&est_global_mutex);
  }
}



/* END OF FILE */
