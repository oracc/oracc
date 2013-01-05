/*************************************************************************************************
 * The MT-safe API of Hyper Estraier
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


#ifndef _ESTMTDB_H                       /* duplication check */
#define _ESTMTDB_H

#if defined(__cplusplus)                 /* export for C++ */
extern "C" {
#endif


#include <estraier.h>
#include <cabin.h>
#include <pthread.h>
#include <stdlib.h>



/*************************************************************************************************
 * API for MT-safe database
 *************************************************************************************************/


typedef struct {                         /* type of structure for a MT-safe database */
  ESTDB *db;                             /* database connection */
  char *path;                            /* real path of the directory */
  pthread_mutex_t mutex;                 /* mutex for each connection */
} ESTMTDB;


/* Open a database. */
ESTMTDB *est_mtdb_open(const char *name, int omode, int *ecp);


/* Close a database. */
int est_mtdb_close(ESTMTDB *db, int *ecp);


/* Get the last happended error code of a database. */
int est_mtdb_error(ESTMTDB *db);


/* Check whether a database has a fatal error. */
int est_mtdb_fatal(ESTMTDB *db);


/* Add an index for narrowing or sorting with document attributes. */
int est_mtdb_add_attr_index(ESTMTDB *db, const char *name, int type);


/* Flush index words in the cache of a database. */
int est_mtdb_flush(ESTMTDB *db, int max);


/* Synchronize updating contents of a database. */
int est_mtdb_sync(ESTMTDB *db);


/* Optimize a database. */
int est_mtdb_optimize(ESTMTDB *db, int options);


/* Merge another database. */
int est_mtdb_merge(ESTMTDB *db, const char *name, int options);


/* Add a document to a database. */
int est_mtdb_put_doc(ESTMTDB *db, ESTDOC *doc, int options);


/* Remove a document from a database. */
int est_mtdb_out_doc(ESTMTDB *db, int id, int options);


/* Edit attributes of a document object in a database. */
int est_mtdb_edit_doc(ESTMTDB *db, ESTDOC *doc);


/* Retrieve a document in a database. */
ESTDOC *est_mtdb_get_doc(ESTMTDB *db, int id, int options);


/* Retrieve the value of an attribute of a document in a database. */
char *est_mtdb_get_doc_attr(ESTMTDB *db, int id, const char *name);


/* Get the ID of a document spacified by URI. */
int est_mtdb_uri_to_id(ESTMTDB *db, const char *uri);


/* Get the name of a database. */
const char *est_mtdb_name(ESTMTDB *db);


/* Get the number of documents in a database. */
int est_mtdb_doc_num(ESTMTDB *db);


/* Get the number of unique words in a database. */
int est_mtdb_word_num(ESTMTDB *db);


/* Get the size of a database. */
double est_mtdb_size(ESTMTDB *db);


/* Search a database for documents corresponding a condition. */
int *est_mtdb_search(ESTMTDB *db, ESTCOND *cond, int *nump, CBMAP *hints);


/* Search plural databases for documents corresponding a condition. */
int *est_mtdb_search_meta(ESTMTDB **dbs, int dbnum, ESTCOND *cond, int *nump, CBMAP *hints);


/* Check whether a document object matches the phrase of a search condition object definitely. */
int est_mtdb_scan_doc(ESTMTDB *db, ESTDOC *doc, ESTCOND *cond);


/* Set the maximum size of the cache memory of a database. */
void est_mtdb_set_cache_size(ESTMTDB *db, size_t size, int anum, int tnum, int rnum);


/* Add a pseudo index directory to a database. */
int est_mtdb_add_pseudo_index(ESTMTDB *db, const char *path);



/*************************************************************************************************
 * features for experts
 *************************************************************************************************/


/* Set the error code of a database. */
void est_mtdb_set_ecode(ESTMTDB *db, int ecode);


/* Check whether an option is set. */
int est_mtdb_check_option(ESTMTDB *db, int option);


/* Get the inode number of a database. */
int est_mtdb_inode(ESTMTDB *db);


/* Set the entity data of a document in a database. */
int est_mtdb_set_doc_entity(ESTMTDB *db, int id, const char *ptr, int size);


/* Get the entity data of a document in a database. */
char *est_mtdb_get_doc_entity(ESTMTDB *db, int id, int *sp);


/* Set the maximum number of extension of wild cards. */
void est_mtdb_set_wildmax(ESTMTDB *db, int num);


/* Add a piece of meta data to a database. */
void est_mtdb_add_meta(ESTMTDB *db, const char *name, const char *value);


/* Get a list of names of meta data of a database. */
CBLIST *est_mtdb_meta_names(ESTMTDB *db);


/* Get the value of a piece of meta data of a database. */
char *est_mtdb_meta(ESTMTDB *db, const char *name);


/* Extract keywords of a document object. */
CBMAP *est_mtdb_etch_doc(ESTMTDB *db, ESTDOC *doc, int max);


/* Store a map object of keywords. */
int est_mtdb_put_keywords(ESTMTDB *db, int id, CBMAP *kwords, double weight);


/* Remove keywords of a document. */
int est_mtdb_out_keywords(ESTMTDB *db, int id);


/* Retrieve a map object of keywords. */
CBMAP *est_mtdb_get_keywords(ESTMTDB *db, int id);


/* Mesure the total size of each inner records of a stored document. */
int est_mtdb_measure_doc(ESTMTDB *db, int id, int parts);


/* Initialize the iterator of a database. */
int est_mtdb_iter_init(ESTMTDB *db, const char *prev);


/* Get the next ID of the iterator of a database. */
int est_mtdb_iter_next(ESTMTDB *db);


/* Initialize the word iterator of a database. */
int est_mtdb_word_iter_init(ESTMTDB *db);


/* Get the next word of the word iterator of a database. */
char *est_mtdb_word_iter_next(ESTMTDB *db);


/* Get the size of the record of a word. */
int est_mtdb_word_rec_size(ESTMTDB *db, const char *word);


/* Get the number of unique keywords in a database. */
int est_mtdb_keyword_num(ESTMTDB *db);


/* Initialize the keyword iterator of a database. */
int est_mtdb_keyword_iter_init(ESTMTDB *db);


/* Get the next keyword of the word iterator of a database. */
char *est_mtdb_keyword_iter_next(ESTMTDB *db);


/* Get the size of the record of a keyword. */
int est_mtdb_keyword_rec_size(ESTMTDB *db, const char *word);


/* Search documents corresponding a keyword for a database. */
int *est_mtdb_keyword_search(ESTMTDB *db, const char *word, int *nump);


/* Get the number of records in the cache memory of a database. */
int est_mtdb_cache_num(ESTMTDB *db);


/* Get the size of used cache region. */
int est_mtdb_used_cache_size(ESTMTDB *db);


/* Set the special cache for narrowing and sorting with document attributes. */
void est_mtdb_set_special_cache(ESTMTDB *db, const char *name, int num);


/* Set the callback function to inform of database events. */
void est_mtdb_set_informer(ESTMTDB *db, void (*func)(const char *, void *), void *opaque);


/* Fill the cache for keys for TF-IDF. */
void est_mtdb_fill_key_cache(ESTMTDB *db);


/* Set the database of document frequency. */
void est_mtdb_set_dfdb(ESTMTDB *db, DEPOT *dfdb);


/* Clear the result cache. */
void est_mtdb_refresh_rescc(ESTMTDB *db);


/* Charge the result cache. */
void est_mtdb_charge_rescc(ESTMTDB *db, int max);


/* Get a list of words in the result cache. */
CBLIST *est_mtdb_list_rescc(ESTMTDB *db);


/* Get the number of pseudo documents in a database. */
int est_mtdb_pseudo_doc_num(ESTMTDB *db);


/* Get a list of expressions of attribute indexes of a database. */
CBLIST *est_mtdb_attr_index_exprs(ESTMTDB *db);


/* Interrupt long time processing. */
void est_mtdb_interrupt(ESTMTDB *db);



/*************************************************************************************************
 * features for hyper experts
 *************************************************************************************************/


/* Get the usage ratio of the cache.
   The return value is the usage ratio of the cache.  While flushing, the return value is 1.0.
   Though this function is never blocked, the accuracy of the return value is inferior to
   `est_mtdb_used_cache_size'. */
double est_mtdb_cache_usage(ESTMTDB *db);



#if defined(__cplusplus)                 /* export for C++ */
}
#endif

#endif                                   /* duplication check */


/* END OF FILE */
