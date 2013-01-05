/*************************************************************************************************
 * The core API of Hyper Estraier
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


#ifndef _ESTRAIER_H                      /* duplication check */
#define _ESTRAIER_H

#if defined(__cplusplus)                 /* export for C++ */
extern "C" {
#endif



/*************************************************************************************************
 * common settings
 *************************************************************************************************/


/* version of Hyper Estraier */
extern const char *est_version;



/*************************************************************************************************
 * underlying headers
 *************************************************************************************************/


#include <depot.h>
#include <curia.h>
#include <cabin.h>
#include <villa.h>
#include <stdlib.h>



/*************************************************************************************************
 * API for document
 *************************************************************************************************/


#define ESTDATTRID     "@id"             /* name of the attribute of the ID number */
#define ESTDATTRURI    "@uri"            /* name of the attribute of the URI */
#define ESTDATTRDIGEST "@digest"         /* name of the attribute of message digest */
#define ESTDATTRCDATE  "@cdate"          /* name of the attribute of creation date */
#define ESTDATTRMDATE  "@mdate"          /* name of the attribute of modification date */
#define ESTDATTRADATE  "@adate"          /* name of the attribute of access date */
#define ESTDATTRTITLE  "@title"          /* name of the attribute of title */
#define ESTDATTRAUTHOR "@author"         /* name of the attribute of author */
#define ESTDATTRTYPE   "@type"           /* name of the attribute of content type */
#define ESTDATTRLANG   "@lang"           /* name of the attribute of language */
#define ESTDATTRGENRE  "@genre"          /* name of the attribute of genre */
#define ESTDATTRSIZE   "@size"           /* name of the attribute of entity size */
#define ESTDATTRWEIGHT "@weight"         /* name of the attribute of scoring weight */
#define ESTDATTRMISC   "@misc"           /* name of the attribute of miscellaneous information */
#define ESTDCNTLVECTOR "%VECTOR"         /* name of the control code for keyword vector */
#define ESTDCNTLSCORE  "%SCORE"          /* name of the control code for substitute score */
#define ESTDCNTLSHADOW "%SHADOW"         /* name of the control code for shadow document */

typedef struct {                         /* type of structure for a document */
  int id;                                /* identification number */
  CBMAP *attrs;                          /* map of attributes */
  CBLIST *dtexts;                        /* list of shown text */
  CBMAP *kwords;                         /* map of keywords */
} ESTDOC;


/* Create a document object.
   The return value is an object of a document. */
ESTDOC *est_doc_new(void);


/* Create a document object made from draft data.
   `draft' specifies a string of draft data.
   The return value is an object of a document. */
ESTDOC *est_doc_new_from_draft(const char *draft);


/* Destroy a document object.
   `doc' specifies a document object. */
void est_doc_delete(ESTDOC *doc);


/* Add an attribute to a document object.
   `doc' specifies a document object.
   `name' specifies the name of an attribute.
   `value' specifies the value of the attribute.  If it is `NULL', the attribute is removed. */
void est_doc_add_attr(ESTDOC *doc, const char *name, const char *value);


/* Add a sentence of text to a document object.
   `doc' specifies a document object.
   `text' specifies a sentence of text. */
void est_doc_add_text(ESTDOC *doc, const char *text);


/* Add a hidden sentence to a document object.
   `doc' specifies a document object.
   `text' specifies a hidden sentence. */
void est_doc_add_hidden_text(ESTDOC *doc, const char *text);


/* Attach keywords to a document object.
   `doc' specifies a document object.
   `kwords' specifies a map object of keywords.  Keys of the map should be keywords of the
   document and values should be their scores in decimal string.  The map object is copied
   internally. */
void est_doc_set_keywords(ESTDOC *doc, CBMAP *kwords);


/* Set the substitute score of a document object.
   `doc' specifies a document object.
   `score' specifies the substitute score.  It it is negative, the substitute score setting is
   nullified. */
void est_doc_set_score(ESTDOC *doc, int score);


/* Get the ID number of a document object.
   `doc' specifies a document object.
   The return value is the ID number of the document object.  If the object has not been
   registered, -1 is returned. */
int est_doc_id(ESTDOC *doc);


/* Get a list of attribute names of a document object.
   `doc' specifies a document object.
   The return value is a new list object of attribute names of the document object.  Because
   the object of the return value is opened with the function `cblistopen', it should be closed
   with the function `cblistclose' if it is no longer in use. */
CBLIST *est_doc_attr_names(ESTDOC *doc);


/* Get the value of an attribute of a document object.
   `doc' specifies a document object.
   `name' specifies the name of an attribute.
   The return value is the value of the attribute or `NULL' if it does not exist.  The life
   duration of the returned string is synchronous with the one of the document object. */
const char *est_doc_attr(ESTDOC *doc, const char *name);


/* Get a list of sentences of the text of a document object.
   `doc' specifies a document object.
   The return value is a list object of sentences of the text of the document object.  The life
   duration of the returned object is synchronous with the one of the document object. */
const CBLIST *est_doc_texts(ESTDOC *doc);


/* Concatenate sentences of the text of a document object.
   `doc' specifies a document object.
   The return value is concatenated sentences of the document object.  Because the region of the
   return value is allocated with the `malloc' call, it should be released with the `free' call
   if it is no longer in use. */
char *est_doc_cat_texts(ESTDOC *doc);


/* Get attached keywords of a document object.
   `doc' specifies a document object.
   The return value is a map object of keywords and their scores in decimal string.  If no
   keyword is attached, `NULL' is returned.  The life duration of the returned object is
   synchronous with the one of the document object. */
CBMAP *est_doc_keywords(ESTDOC *doc);


/* Get the substitute score of a document object.
   `doc' specifies a document object.
   The return value is the substitute score or -1 if it is not set. */
int est_doc_score(ESTDOC *doc);


/* Dump draft data of a document object.
   `doc' specifies a document object.
   The return value is draft data of the document object.  Because the region of the return value
   is allocated with the `malloc' call, it should be released with the `free' call if it is no
   longer in use. */
char *est_doc_dump_draft(ESTDOC *doc);


/* Make a snippet of the body text of a document object.
   `doc' specifies a document object.
   `word' specifies a list object of words to be highlight.
   `wwidth' specifies whole width of the result.
   `hwidth' specifies width of strings picked up from the beginning of the text.
   `awidth' specifies width of strings picked up around each highlighted word.
   The return value is a snippet string of the body text of the document object.  There are tab
   separated values.  Each line is a string to be shown.  Though most lines have only one field,
   some lines have two fields.  If the second field exists, the first field is to be shown with
   highlighted, and the second field means its normalized form.  Because the region of the
   return value is allocated with the `malloc' call, it should be released with the `free' call
   if it is no longer in use. */
char *est_doc_make_snippet(ESTDOC *doc, const CBLIST *words, int wwidth, int hwidth, int awidth);



/*************************************************************************************************
 * API for search conditions
 *************************************************************************************************/


#define ESTOPUVSET     "[UVSET]"         /* universal set */
#define ESTOPID        "[ID]"            /* ID matching search */
#define ESTOPURI       "[URI]"           /* URI matching search */
#define ESTOPSIMILAR   "[SIMILAR]"       /* similarity search */
#define ESTOPRANK      "[RANK]"          /* ranking search */

#define ESTOPUNION     "OR"              /* union (conjunction) */
#define ESTOPISECT     "AND"             /* intersection (disjunction) */
#define ESTOPDIFF      "ANDNOT"          /* difference (intersection with negation) */
#define ESTOPWCBW      "[BW]"            /* wild card for words beginning with a string */
#define ESTOPWCEW      "[EW]"            /* wild card for words ending with a string */
#define ESTOPWCRX      "[RX]"            /* wild card for words matching regular expressions */
#define ESTOPWITH      "WITH"            /* delimiter for elements */

#define ESTOPSTREQ     "STREQ"           /* string is equal */
#define ESTOPSTRNE     "STRNE"           /* string is not equal */
#define ESTOPSTRINC    "STRINC"          /* string is included in */
#define ESTOPSTRBW     "STRBW"           /* string begins with */
#define ESTOPSTREW     "STREW"           /* string ends with */
#define ESTOPSTRAND    "STRAND"          /* string includes all tokens in */
#define ESTOPSTROR     "STROR"           /* string includes at least one token in */
#define ESTOPSTROREQ   "STROREQ"         /* string is equal at least one token in */
#define ESTOPSTRRX     "STRRX"           /* string matches regular expressions of */
#define ESTOPNUMEQ     "NUMEQ"           /* number or date is equal */
#define ESTOPNUMNE     "NUMNE"           /* number or date is not equal */
#define ESTOPNUMGT     "NUMGT"           /* number or date is greater than */
#define ESTOPNUMGE     "NUMGE"           /* number or date is greater than or equal to */
#define ESTOPNUMLT     "NUMLT"           /* number or date is less than */
#define ESTOPNUMLE     "NUMLE"           /* number or date is less than or equal to */
#define ESTOPNUMBT     "NUMBT"           /* number or date is between two tokens of */

#define ESTORDIDA      "[IDA]"           /* ID numbers in ascending order */
#define ESTORDIDD      "[IDD]"           /* ID numbers in descending order */
#define ESTORDSCA      "[SCA]"           /* scores in ascending order */
#define ESTORDSCD      "[SCD]"           /* scores in descending order */
#define ESTORDSTRA     "STRA"            /* strings in ascending order */
#define ESTORDSTRD     "STRD"            /* strings in descending order */
#define ESTORDNUMA     "NUMA"            /* numbers in ascending order */
#define ESTORDNUMD     "NUMD"            /* numbers in descending order */

#define ESTECLSIMURL   10.0              /* eclipse considering similarity and URL */
#define ESTECLSERV     100.0             /* eclipse on server basis */
#define ESTECLDIR      101.0             /* eclipse on directory basis */
#define ESTECLFILE     102.0             /* eclipse on file basis */

typedef struct {                         /* type of structure for search conditions */
  char *phrase;                          /* search phrase */
  int gstep;                             /* step of N-gram */
  int tfidf;                             /* whether with TF-IDF tuning */
  int pmode;                             /* mode of phrase form */
  void (*cbxpn)(const char *, CBLIST *); /* callback function for query expansion */
  CBLIST *attrs;                         /* conditions with attributes */
  char *order;                           /* sorting order */
  int max;                               /* maximum number of retrieval */
  int skip;                              /* number of documents to be skipped */
  int auxmin;                            /* minimum hits to adopt the auxiliary index */
  CBMAP *auxwords;                       /* words which the auxiliary index has been used */
  int scfb;                              /* whether to feed back scores */
  int *scores;                           /* array of scores */
  int snum;                              /* number of elemnts of the score array */
  const int *nscores;                    /* array of narrowing scores */
  int nsnum;                             /* number of elemnts of the narrowing score array */
  int opts;                              /* options for preservation */
  double ecllim;                         /* lower limit of similarity eclipse */
  CBMAP *shadows;                        /* map of eclipsed documents */
  char *distinct;                        /* distinct attribute */
  int mask;                              /* mask for meta search */
} ESTCOND;

enum {                                   /* enumeration for options */
  ESTCONDSURE = 1 << 0,                  /* check every N-gram key */
  ESTCONDUSUAL = 1 << 1,                 /* check N-gram keys skipping by one */
  ESTCONDFAST = 1 << 2,                  /* check N-gram keys skipping by two */
  ESTCONDAGITO = 1 << 3,                 /* check N-gram keys skipping by three */
  ESTCONDNOIDF = 1 << 4,                 /* without TF-IDF tuning */
  ESTCONDSIMPLE = 1 << 10,               /* with the simplified phrase */
  ESTCONDROUGH = 1 << 11,                /* with the rough phrase */
  ESTCONDUNION = 1 << 15,                /* with the union phrase */
  ESTCONDISECT = 1 << 16,                /* with the intersection phrase */
  ESTCONDSCFB = 1 << 30                  /* feed back scores (for debug) */
};


/* Create a condition object.
   The return value is an object of search conditions. */
ESTCOND *est_cond_new(void);


/* Destroy a condition object.
   `cond' specifies a condition object. */
void est_cond_delete(ESTCOND *cond);


/* Set the search phrase to a condition object.
   `cond' specifies a condition object.
   `phrase' specifies a search phrase. */
void est_cond_set_phrase(ESTCOND *cond, const char *phrase);


/* Add an expression for an attribute to a condition object.
   `cond' specifies a condition object.
   `expr' specifies an expression for an attribute. */
void est_cond_add_attr(ESTCOND *cond, const char *expr);


/* Set the order of a condition object.
   `cond' specifies a condition object.
   `expr' specifies an expression for the order.  By default, the order is by score descending. */
void est_cond_set_order(ESTCOND *cond, const char *expr);


/* Set the maximum number of retrieval of a condition object.
   `cond' specifies a condition object.
   `max' specifies the maximum number of retrieval.  By default, the number of retrieval is not
   limited. */
void est_cond_set_max(ESTCOND *cond, int max);


/* Set the number of skipped documents of a condition object.
   `cond' specifies a condition object.
   `skip' specifies the number of documents to be skipped in the search result. */
void est_cond_set_skip(ESTCOND *cond, int skip);


/* Set options of retrieval of a condition object.
   `cond' specifies a condition object.
   `options' specifies options: `ESTCONDSURE' specifies that it checks every N-gram key,
   `ESTCONDUSUAL', which is the default, specifies that it checks N-gram keys with skipping one
   key, `ESTCONDFAST' skips two keys, `ESTCONDAGITO' skips three keys, `ESTCONDNOIDF' specifies
   not to perform TF-IDF tuning, `ESTCONDSIMPLE' specifies to use simplified phrase,
   `ESTCONDROUGH' specifies to use rough phrase, `ESTCONDUNION' specifies to use union phrase,
   `ESTCONDISECT' specifies to use intersection phrase, `ESTCONDSCFB' specifies to feed back
   scores (only for debugging).  Each option can be specified at the same time by bitwise or.  If
   keys are skipped, though search speed is improved, the relevance ratio grows less. */
void est_cond_set_options(ESTCOND *cond, int options);


/* Set permission to adopt result of the auxiliary index.
   `cond' specifies a condition object.
   `min' specifies the minimum hits to adopt result of the auxiliary index.  If it is not more
   than 0, the auxiliary index is not used.  By default, it is 32. */
void est_cond_set_auxiliary(ESTCOND *cond, int min);


/* Set the lower limit of similarity eclipse.
   `cond' specifies a condition object.
   `limit' specifies the lower limit of similarity for documents to be eclipsed.  Similarity is
   between 0.0 and 1.0.  If the limit is added by `ESTECLSIMURL', similarity is weighted by URL.
   If the limit is `ESTECLSERV', similarity is ignored and documents in the same server are
   eclipsed.  If the limit is `ESTECLDIR', similarity is ignored and documents in the same
   directory are eclipsed.  If the limit is `ESTECLFILE', similarity is ignored and documents of
   the same file are eclipsed. */
void est_cond_set_eclipse(ESTCOND *cond, double limit);


/* Set the attribute distinction filter.
   `cond' specifies a condition object.
   `name' specifies the name of an attribute to be distinct.
   If this filter is set, candidates which have same value of the attribute is omitted. */
void est_cond_set_distinct(ESTCOND *cond, const char *name);


/* Set the mask of targets of meta search.
   `cond' specifies a condition object.
   `mask' specifies a masking number.  1 means the first target, 2 means the second target, 4
   means the third target, and power values of 2 and their summation compose the mask. */
void est_cond_set_mask(ESTCOND *cond, int mask);



/*************************************************************************************************
 * API for database
 *************************************************************************************************/


#define ESTIDXDMAX     256               /* max number of the inverted index */
#define ESTIDXDSTD     16                /* standard number of the inverted index */
#define ESTPDOCIDMIN   2000000001        /* minimum ID number of pseudo documents */

typedef struct {                         /* type of structure for the inverted index */
  char *name;                            /* name of the database */
  int omode;                             /* open mode */
  VILLA *dbs[ESTIDXDMAX];                /* database handles */
  int dnum;                              /* number of division */
  VILLA *cdb;                            /* current database handle */
} ESTIDX;

typedef struct {                         /* type of structure for a database object */
  char *name;                            /* name of the database */
  int inode;                             /* inode of the database */
  DEPOT *metadb;                         /* handle of the meta database */
  ESTIDX *idxdb;                         /* handles of the inverted indexs */
  VILLA *fwmdb;                          /* handle of the database for forward matching */
  VILLA *auxdb;                          /* handle of the auxiliary index */
  VILLA *xfmdb;                          /* handle of the database for aux forward matching */
  CURIA *attrdb;                         /* handle of the database for attrutes */
  CURIA *textdb;                         /* handle of the database for texts */
  CURIA *kwddb;                          /* handle of the database for keywords */
  VILLA *listdb;                         /* handle of the database for document list */
  CBMAP *aidxs;                          /* map of attribute indexes */
  CBLIST *pdocs;                         /* list of pseudo documents */
  CBMAP *puris;                          /* map of URIs of pseudo documents */
  int ecode;                             /* last happened error code */
  int fatal;                             /* whether to have a fatal error */
  int dseq;                              /* sequence for document IDs */
  int dnum;                              /* number of the documents */
  int amode;                             /* mode of text analyzer */
  int zmode;                             /* mode of data compression */
  int smode;                             /* mode of score type */
  CBMAP *idxcc;                          /* cache for the inverted index */
  CBMAP *auxcc;                          /* cache for the auxiliary index */
  size_t icsiz;                          /* power of the cache */
  size_t icmax;                          /* max size of the cache */
  CBMAP *outcc;                          /* cache for deleted documents */
  CBMAP *keycc;                          /* cache for keys for TF-IDF */
  int  kcmnum;                           /* max number of the key cache */
  CBMAP *attrcc;                         /* cache for attributes */
  int acmnum;                            /* max number of the attribute cache */
  CBMAP *textcc;                         /* cache for texts */
  int tcmnum;                            /* max number of the text cache */
  CBMAP *veccc;                          /* cache for keyword vectors */
  int vcmnum;                            /* max number of the vector cache */
  CBMAP *rescc;                          /* cache for results */
  int rcmnum;                            /* max number of the result cache */
  CBMAP *spacc;                          /* special cache for attributes */
  int scmnum;                            /* max number of the special cache */
  char *scname;                          /* name of the attribute for the special cache */
  void (*infocb)(const char *, void *);  /* callback function to inform of events */
  void *infoop;                          /* opaque for the informing callback */
  DEPOT *dfdb;                           /* handle of the database for document frequency */
  int wildmax;                           /* maximum number of expansion of wild cards */
  CBMAP *metacc;                         /* cache for meta data */
  int flsflag;                           /* flag of flushing */
  int intflag;                           /* flag of thread interruption */
} ESTDB;

enum {                                   /* enumeration for error codes */
  ESTENOERR,                             /* no error */
  ESTEINVAL,                             /* invalid argument */
  ESTEACCES,                             /* access forbidden */
  ESTELOCK,                              /* lock failure */
  ESTEDB,                                /* database problem */
  ESTEIO,                                /* I/O problem */
  ESTENOITEM,                            /* no item */
  ESTEMISC = 9999                        /* miscellaneous */
};

enum {                                   /* enumeration for open modes */
  ESTDBREADER = 1 << 0,                  /* open as a reader */
  ESTDBWRITER = 1 << 1,                  /* open as a writer */
  ESTDBCREAT = 1 << 2,                   /* a writer creating */
  ESTDBTRUNC = 1 << 3,                   /* a writer truncating */
  ESTDBNOLCK = 1 << 4,                   /* open without locking */
  ESTDBLCKNB = 1 << 5,                   /* lock without blocking */
  ESTDBPERFNG = 1 << 10,                 /* use perfect N-gram analyzer */
  ESTDBCHRCAT = 1 << 11,                 /* use character category analyzer */
  ESTDBSMALL = 1 << 20,                  /* small tuning */
  ESTDBLARGE = 1 << 21,                  /* large tuning */
  ESTDBHUGE = 1 << 22,                   /* huge tuning */
  ESTDBHUGE2 = 1 << 23,                  /* huge tuning second */
  ESTDBHUGE3 = 1 << 24,                  /* huge tuning third */
  ESTDBSCVOID = 1 << 25,                 /* store scores as void */
  ESTDBSCINT = 1 << 26,                  /* store scores as integer */
  ESTDBSCASIS = 1 << 27                  /* refrain from adjustment of scores */
};

enum {                                   /* enumeration for data types of attribute index */
  ESTIDXATTRSEQ,                         /* for multipurpose sequencial access method */
  ESTIDXATTRSTR,                         /* for narrowing with attributes as strings */
  ESTIDXATTRNUM                          /* for narrowing with attributes as numbers */
};

enum {                                   /* enumeration for options of optimization */
  ESTOPTNOPURGE = 1 << 0,                /* omit purging dispensable region of deleted */
  ESTOPTNODBOPT = 1 << 1                 /* omit optimization of the database files */
};

enum {                                   /* enumeration for options of document merger */
  ESTMGCLEAN = 1 << 0                    /* clean up dispensable regions */
};

enum {                                   /* enumeration for options of document registration */
  ESTPDCLEAN = 1 << 0,                   /* clean up dispensable regions */
  ESTPDWEIGHT = 1 << 1                   /* weight scores statically when indexing */
};

enum {                                   /* enumeration for options of document deletion */
  ESTODCLEAN = 1 << 0                    /* clean up dispensable regions */
};

enum {                                   /* enumeration for options of document retrieval */
  ESTGDNOATTR = 1 << 0,                  /* no attributes */
  ESTGDNOTEXT = 1 << 1,                  /* no text */
  ESTGDNOKWD = 1 << 2                    /* no keywords */
};


/* Get the string of an error code.
   `ecode' specifies an error code.
   The return value is the string of the error code. */
const char *est_err_msg(int ecode);


/* Open a database.
   `name' specifies the name of a database directory.
   `omode' specifies open modes: `ESTDBWRITER' as a writer, `ESTDBREADER' as a reader.  If the
   mode is `ESTDBWRITER', the following may be added by bitwise or: `ESTDBCREAT', which means it
   creates a new database if not exist, `ESTDBTRUNC', which means it creates a new database
   regardless if one exists.  Both of `ESTDBREADER' and  `ESTDBWRITER' can be added to by
   bitwise or: `ESTDBNOLCK', which means it opens a database file without file locking, or
   `ESTDBLCKNB', which means locking is performed without blocking.  If `ESTDBNOLCK' is used,
   the application is responsible for exclusion control.  `ESTDBCREAT' can be added to by bitwise
   or: `ESTDBPERFNG', which means N-gram analysis is performed against European text also,
   `ESTDBCHRCAT', which means character category analysis is performed instead of N-gram analysis,
   `ESTDBSMALL', which means the index is tuned to register less than 50000 documents,
   `ESTDBLARGE', which means the index is tuned to register more than 300000 documents,
   `ESTDBHUGE', which means the index is tuned to register more than 1000000 documents,
   `ESTDBHUGE2', which means the index is tuned to register more than 5000000 documents,
   `ESTDBHUGE3', which means the index is tuned to register more than 10000000 documents,
   `ESTDBSCVOID', which means scores are stored as void, `ESTDBSCINT', which means scores are
   stored as 32-bit integer, `ESTDBSCASIS', which means scores are stored as-is and marked not
   to be tuned when search.
   `ecp' specifies the pointer to a variable to which the error code is assigned.
   The return value is a database object of the database or `NULL' if failure. */
ESTDB *est_db_open(const char *name, int omode, int *ecp);


/* Close a database.
   `db' specifies a database object.
   `ecp' specifies the pointer to a variable to which the error code is assigned.
   The return value is true if success, else it is false. */
int est_db_close(ESTDB *db, int *ecp);


/* Get the last happened error code of a database.
   `db' specifies a database object.
   The return value is the last happened error code of the database. */
int est_db_error(ESTDB *db);


/* Check whether a database has a fatal error.
   `db' specifies a database object.
   The return value is true if the database has fatal erroor, else it is false. */
int est_db_fatal(ESTDB *db);


/* Add an index for narrowing or sorting with document attributes.
   `db' specifies a database object connected as a writer.
   `name' specifies the name of an attribute.
   `type' specifies the data type of attribute index; `ESTIDXATTRSEQ' for multipurpose sequencial
   access method, `ESTIDXATTRSTR' for narrowing with attributes as strings, `ESTIDXATTRNUM' for
   narrowing with attributes as numbers.
   The return value is true if success, else it is false.
   Note that this function should be called before the first document is registered. */
int est_db_add_attr_index(ESTDB *db, const char *name, int type);


/* Flush index words in the cache of a database.
   `db' specifies a database object connected as a writer.
   `max' specifies the maximum number of words to be flushed.  If it not more than zero, all
   words are flushed.
   The return value is true if success, else it is false. */
int est_db_flush(ESTDB *db, int max);


/* Synchronize updating contents of a database.
   `db' specifies a database object connected as a writer.
   The return value is true if success, else it is false. */
int est_db_sync(ESTDB *db);


/* Optimize a database.
   `db' specifies a database object connected as a writer.
   `options' specifies options: `ESTOPTNOPURGE' to omit purging dispensable region of deleted
   documents, `ESTOPTNODBOPT' to omit optimization of the database files.  The two can be
   specified at the same time by bitwise or.
   The return value is true if success, else it is false. */
int est_db_optimize(ESTDB *db, int options);


/* Merge another database.
   `db' specifies a database object connected as a writer.
   `name' specifies the name of another database directory.
   `options' specifies options: `ESTMGCLEAN' to clean up dispensable regions of the deleted
   document.
   The return value is true if success, else it is false.
   Creation options of the two databases should be same entirely.  ID numbers of imported
   documents are changed within the sequence of the desitination database.  If URIs of imported
   documents conflict ones of exsisting documents, existing documents are removed. */
int est_db_merge(ESTDB *db, const char *name, int options);


/* Add a document to a database.
   `db' specifies a database object connected as a writer.
   `doc' specifies a document object.  The document object should have the URI attribute.
   `options' specifies options: `ESTPDCLEAN' to clean up dispensable regions of the overwritten
   document, `ESTPDWEIGHT' to weight scores statically with score weighting attribute.
   The return value is true if success, else it is false.
   If the URI attribute is same with an existing document in the database, the existing one is
   deleted. */
int est_db_put_doc(ESTDB *db, ESTDOC *doc, int options);


/* Remove a document from a database.
   `db' specifies a database object connected as a writer.
   `id' specifies the ID number of a registered document.
   `options' specifies options: `ESTODCLEAN' to clean up dispensable regions of the deleted
   document.
   The return value is true if success, else it is false. */
int est_db_out_doc(ESTDB *db, int id, int options);


/* Edit attributes of a document in a database.
   `db' specifies a database object connected as a writer.
   `doc' specifies a document object.
   The return value is true if success, else it is false.
   The ID can not be changed.  If the URI is changed and it overlaps the URI of another
   registered document, this function fails. */
int est_db_edit_doc(ESTDB *db, ESTDOC *doc);


/* Retrieve a document in a database.
   `db' specifies a database object.
   `id' specifies the ID number of a registered document.
   `options' specifies options: `ESTGDNOATTR' to ignore attributes, `ESTGDNOTEXT' to ignore
   the body text, `ESTGDNOKWD' to ignore keywords.  The three can be specified at the same time
   by bitwise or.
   The return value is a document object.  It should be deleted with `est_doc_delete' if it is
   no longer in use.  On error, `NULL' is returned. */
ESTDOC *est_db_get_doc(ESTDB *db, int id, int options);


/* Retrieve the value of an attribute of a document in a database.
   `db' specifies a database object.
   `id' specifies the ID number of a registered document.
   `name' specifies the name of an attribute.
   The return value is the value of the attribute or `NULL' if it does not exist.  Because the
   region of the return value is allocated with the `malloc' call, it should be released with
   the `free' call if it is no longer in use. */
char *est_db_get_doc_attr(ESTDB *db, int id, const char *name);


/* Get the ID of a document specified by URI.
   `db' specifies a database object.
   `uri' specifies the URI of a registered document.
   The return value is the ID of the document.  On error, -1 is returned. */
int est_db_uri_to_id(ESTDB *db, const char *uri);


/* Get the name of a database.
   `db' specifies a database object.
   The return value is the name of the database.  The life duration of the returned string is
   synchronous with the one of the database object. */
const char *est_db_name(ESTDB *db);


/* Get the number of documents in a database.
   `db' specifies a database object.
   The return value is the number of documents in the database. */
int est_db_doc_num(ESTDB *db);


/* Get the number of unique words in a database.
   `db' specifies a database object.
   The return value is the number of unique words in the database. */
int est_db_word_num(ESTDB *db);


/* Get the size of a database.
   `db' specifies a database object.
   The return value is the size of the database. */
double est_db_size(ESTDB *db);


/* Search a database for documents corresponding a condition.
   `db' specifies a database object.
   `cond' specifies a condition object.
   `nump' specifies the pointer to a variable to which the number of elements in the result is
   assigned.
   `hints' specifies a map object into which the number of documents corresponding to each word
   is stored.  If a word is in a negative condition, the number is negative.  The element whose
   key is an empty string specifies the number of whole result.  If it is `NULL', it is not used.
   The return value is an array whose elements are ID numbers of corresponding documents.
   This function does never fail.  Even if no document corresponds or an error occurs, an empty
   array is returned.  Because the region of the return value is allocated with the `malloc'
   call, it should be released with the `free' call if it is no longer in use. */
int *est_db_search(ESTDB *db, ESTCOND *cond, int *nump, CBMAP *hints);


/* Search plural databases for documents corresponding a condition.
   `dbs' specifies an array whose elements are database objects.
   `dbnum' specifies the number of elements of the array.
   `cond' specifies a condition object.
   `nump' specifies the pointer to a variable to which the number of elements in the result is
   assigned.
   `hints' specifies a map object into which the number of documents corresponding to each word
   is stored.  If a word is in a negative condition, the number is negative.  The element whose
   key is an empty string specifies the number of whole result.  If it is `NULL', it is not used.
   The return value is an array whose elements are indexes of container databases and ID numbers
   of in each database alternately.
   This function does never fail.  Even if no document corresponds or an error occurs, an empty
   array is returned.  Because the region of the return value is allocated with the `malloc'
   call, it should be released with the `free' call if it is no longer in use. */
int *est_db_search_meta(ESTDB **dbs, int dbnum, ESTCOND *cond, int *nump, CBMAP *hints);


/* Check whether a document object matches the phrase of a search condition object definitely.
   `db' specifies a database object.
   `doc' specifies a document object.
   `cond' specifies a search condition object.
   The return value is true if the document matches the phrase of the condition object
   definitely, else it is false. */
int est_db_scan_doc(ESTDB *db, ESTDOC *doc, ESTCOND *cond);


/* Set the maximum size of the cache memory of a database.
   `db' specifies a database object.
   `size' specifies the maximum size of the index cache.  By default, it is 64MB.  If it is
   negative, the current size is not changed.
   `anum' specifies the maximum number of cached records for document attributes.  By default, it
   is 8192.  If it is negative, the current size is not changed.
   `tnum' specifies the maximum number of cached records for document texts.  By default, it is
   1024.  If it is negative, the current size is not changed.
   `rnum' specifies the maximum number of cached records for occurrence results.  By default, it
   is 256.  If it is negative, the current size is not changed. */
void est_db_set_cache_size(ESTDB *db, size_t size, int anum, int tnum, int rnum);


/* Add a pseudo index directory to a database.
   `db' specifies a database object.
   `path' specifies the path of a pseudo index directory.
   The return value is true if success, else it is false. */
int est_db_add_pseudo_index(ESTDB *db, const char *path);



/*************************************************************************************************
 * features for experts
 *************************************************************************************************/


#define _EST_VERSION   "1.4.13"
#define _EST_LIBVER    838
#define _EST_PROTVER   "1.0"

#define _EST_PROJURL   "http://fallabs.com/hyperestraier/"
#define _EST_XNSEARCH  "http://fallabs.com/hyperestraier/xmlns/search"
#define _EST_XNNODE    "http://fallabs.com/hyperestraier/xmlns/node"

enum {                                   /* enumeration for languages */
  ESTLANGEN,                             /* English */
  ESTLANGJA,                             /* Japanese */
  ESTLANGZH,                             /* Chinese */
  ESTLANGKO,                             /* Korean */
  ESTLANGMISC                            /* miscellaneous */
};

enum {                                   /* enumeration for document parts */
  ESTMDATTR = 1 << 0,                    /* attributes */
  ESTMDTEXT = 1 << 1,                    /* texts */
  ESTMDKWD = 1 << 2                      /* keywords */
};

enum {                                   /* enumeration for database repair */
  ESTRPSTRICT = 1 << 0,                  /* perform strict consistency check */
  ESTRPSHODDY = 1 << 1                   /* omit consistency check */
};

typedef struct {                         /* type of structure for an element of result map */
  const char *key;                       /* pointer to the key string */
  int score;                             /* total score */
} ESTRESMAPELEM;

enum {                                   /* enumeration for scoring for result map */
  ESTRMLOSUM,                            /* summation */
  ESTRMLOMAX,                            /* maximum */
  ESTRMLOMIN,                            /* minimum */
  ESTRMLOAVG                             /* average */
};


/* Break a sentence of text and extract words.
   `text' specifies a sentence of text.
   `list' specifies a list object to which extract words are added.
   `norm' specifies whether to normalize the text.
   `tail' specifies whether to pick up oddness N-gram at the end. */
void est_break_text(const char *text, CBLIST *list, int norm, int tail);


/* Break a sentence of text and extract words using perfect N-gram analyzer.
   `text' specifies a sentence of text.
   `list' specifies a list object to which extract words are added.
   `norm' specifies whether to normalize the text.
   `tail' specifies whether to pick up oddness N-gram at the end. */
void est_break_text_perfng(const char *text, CBLIST *list, int norm, int tail);


/* Break a sentence of text and extract words, using character category analyzer.
   `text' specifies a sentence of text.
   `list' specifies a list object to which extract words are added.
   `norm' specifies whether to normalize the text. */
void est_break_text_chrcat(const char *text, CBLIST *list, int norm);


/* Make a snippet of an arbitrary string.
   `word' specifies a list object of words to be highlight.
   `wwidth' specifies whole width of the result.
   `hwidth' specifies width of strings picked up from the beginning of the text.
   `awidth' specifies width of strings picked up around each highlighted word.
   The return value is a snippet string of the string.  Because the region of the return value is
   allocated with the `malloc' call, it should be released with the `free' call if it is no
   longer in use. */
char *est_str_make_snippet(const char *str, const CBLIST *words,
                           int wwidth, int hwidth, int awidth);


/* Convert the character encoding of a string.
   `ptr' specifies the pointer to a region.
   `size' specifies the size of the region.  If it is negative, the size is assigned with
   `strlen(ptr)'.
   `icode' specifies the name of encoding of the input string.
   `ocode' specifies the name of encoding of the output string.
   `sp' specifies the pointer to a variable to which the size of the region of the return
   value is assigned.  If it is `NULL', it is not used.
   `mp' specifies the pointer to a variable to which the number of missing characters by failure
   of conversion is assigned.  If it is `NULL', it is not used.
   If successful, the return value is the pointer to the result object, else, it is `NULL'.
   Because an additional zero code is appended at the end of the region of the return value,
   the return value can be treated as a character string.  Because the region of the return
   value is allocated with the `malloc' call, it should be released with the `free' call if it
   is no longer in use. */
char *est_iconv(const char *ptr, int size, const char *icode, const char *ocode,
                int *sp, int *mp);


/* Detect the encoding of a string automatically.
   `ptr' specifies the pointer to a region.
   `size' specifies the size of the region.  If it is negative, the size is assigned with
   `strlen(ptr)'.
   `plang' specifies a preferred language.  As for now, `ESTLANGEN', `ESTLANGJA', `ESTLANGZH',
   and `ESTLANGKO' are supported.
   The return value is the string of the encoding name of the string. */
const char *est_enc_name(const char *ptr, int size, int plang);


/* Convert a UTF-8 string into UTF-16BE.
   `ptr' specifies the pointer to a region.
   `size' specifies the size of the region.
   `sp' specifies the pointer to a variable to which the size of the region of the return
   value is assigned.
   The return value is the pointer to the result object.  Because an additional zero code is
   appended at the end of the region of the return value, the return value can be treated as a
   character string.  Because the region of the return value is allocated with the `malloc' call,
   it should be released with the `free' call if it is no longer in use. */
char *est_uconv_in(const char *ptr, int size, int *sp);


/* Convert a UTF-16BE string into UTF-8.
   `ptr' specifies the pointer to a region.
   `size' specifies the size of the region.
   `sp' specifies the pointer to a variable to which the size of the region of the return
   value is assigned.  If it is `NULL', it is not used.
   The return value is the pointer to the result object.  Because an additional zero code is
   appended at the end of the region of the return value, the return value can be treated as a
   character string.  Because the region of the return value is allocated with the `malloc' call,
   it should be released with the `free' call if it is no longer in use. */
char *est_uconv_out(const char *ptr, int size, int *sp);


/* Compress a serial object with ZLIB.
   `ptr' specifies the pointer to a region.
   `size' specifies the size of the region.  If it is negative, the size is assigned with
   `strlen(ptr)'.
   `sp' specifies the pointer to a variable to which the size of the region of the return
   value is assigned.
   `mode' specifies detail behavior.  0 specifies using the standard deflate encoding, -1
   specifies the raw deflate encoding, and 1 specifies the GZIP encoding.
   If successful, the return value is the pointer to the result object, else, it is `NULL'.
   Because the region of the return value is allocated with the `malloc' call, it should be
   released with the `free' call if it is no longer in use. */
char *est_deflate(const char *ptr, int size, int *sp, int mode);


/* Decompress a serial object compressed with ZLIB.
   `ptr' specifies the pointer to a region.
   `size' specifies the size of the region.
   `sp' specifies the pointer to a variable to which the size of the region of the return
   value is assigned.  If it is `NULL', it is not used.
   `mode' specifies detail behavior.  0 specifies using the standard deflate encoding, -1
   specifies the raw deflate encoding, and 1 specifies the GZIP encoding.
   If successful, the return value is the pointer to the result object, else, it is `NULL'.
   Because an additional zero code is appended at the end of the region of the return value,
   the return value can be treated as a character string.  Because the region of the return
   value is allocated with the `malloc' call, it should be released with the `free' call if it
   is no longer in use. */
char *est_inflate(const char *ptr, int size, int *sp, int mode);


/* Compress a serial object with LZO.
   `ptr' specifies the pointer to a region.
   `size' specifies the size of the region.  If it is negative, the size is assigned with
   `strlen(ptr)'.
   `sp' specifies the pointer to a variable to which the size of the region of the return
   value is assigned.
   If successful, the return value is the pointer to the result object, else, it is `NULL'.
   Because the region of the return value is allocated with the `malloc' call, it should be
   released with the `free' call if it is no longer in use. */
char *est_lzoencode(const char *ptr, int size, int *sp);


/* Decompress a serial object compressed with LZO.
   `ptr' specifies the pointer to a region.
   `size' specifies the size of the region.
   `sp' specifies the pointer to a variable to which the size of the region of the return
   value is assigned.  If it is `NULL', it is not used.
   If successful, the return value is the pointer to the result object, else, it is `NULL'.
   Because an additional zero code is appended at the end of the region of the return value,
   the return value can be treated as a character string.  Because the region of the return
   value is allocated with the `malloc' call, it should be released with the `free' call if it
   is no longer in use. */
char *est_lzodecode(const char *ptr, int size, int *sp);


/* Compress a serial object with BZIP2.
   `ptr' specifies the pointer to a region.
   `size' specifies the size of the region.  If it is negative, the size is assigned with
   `strlen(ptr)'.
   `sp' specifies the pointer to a variable to which the size of the region of the return
   value is assigned.
   If successful, the return value is the pointer to the result object, else, it is `NULL'.
   Because the region of the return value is allocated with the `malloc' call, it should be
   released with the `free' call if it is no longer in use. */
char *est_bzencode(const char *ptr, int size, int *sp);


/* Decompress a serial object compressed with BZIP2.
   `ptr' specifies the pointer to a region.
   `size' specifies the size of the region.
   `sp' specifies the pointer to a variable to which the size of the region of the return
   value is assigned.  If it is `NULL', it is not used.
   If successful, the return value is the pointer to the result object, else, it is `NULL'.
   Because an additional zero code is appended at the end of the region of the return value,
   the return value can be treated as a character string.  Because the region of the return
   value is allocated with the `malloc' call, it should be released with the `free' call if it
   is no longer in use. */
char *est_bzdecode(const char *ptr, int size, int *sp);


/* Get the border string for draft data of documents.
   The return value is the border string for draft data of documents. */
const char *est_border_str(void);


/* Get the real random number.
   The return value is the real random number between 0.0 and 1.0. */
double est_random(void);


/* Get the random number in normal distribution.
   The return value is the random number in normal distribution between 0.0 and 1.0. */
double est_random_nd(void);


/* Get an MD5 hash string of a key string.
   `key' specifies a string to be encrypted.
   The return value is an MD5 hash string of the key string.  Because the region of the return
   value is allocated with the `malloc' call, it should be released with the `free' call if it
   is no longer in use. */
char *est_make_crypt(const char *key);


/* Check whether a key matches an MD5 hash string.
   `key' specifies a string to be checked.
   `hash' specifies an MD5 hash string.
   The return value is true if the key matches the hash string, else it is false. */
int est_match_crypt(const char *key, const char *hash);


/* Create a regular expression object.
   `str' specifies a string of regular expressions.
   The return value is a regular expression object or `NULL' if failure.
   If the expression is leaded by "*I:", the pattern is case insensitive. */
void *est_regex_new(const char *str);


/* Delete a regular expression object.
   `regex' specifies a regular expression object. */
void est_regex_delete(void *regex);


/* Check whether a regular expression matches a string.
   `regex' specifies a regular expression object.
   `str' specifies a string.
   The return value is true if the regular expression object matchs the string. */
int est_regex_match(const void *regex, const char *str);


/* Check whether a regular expression matches a string.
   `rstr' specifies a regular expression string.
   `tstr' specifies a target string.
   The return value is true if the regular expression string matchs the target string. */
int est_regex_match_str(const char *rstr, const char *tstr);


/* Replace each substring matching a regular expression string.
   `str' specifies a target string.
   `bef' specifies a string of regular expressions for substrings.
   `aft' specifies a string with which each substrings are replaced.  Each "&" in the string is
   replaced with the matched substring.  Each "\" in the string escapes the following character.
   Special escapes "\1" through "\9" referring to the corresponding matching sub-expressions in
   the regular expression string are supported.
   The return value is a new converted string.  Even if the regular expression is invalid, a copy
   of the original string is returned.  Because the region of the return value is allocated with
   the `malloc' call, it should be released with the `free' call if it is no longer in use. */
char *est_regex_replace(const char *str, const char *bef, const char *aft);


/* Duplicate a document object.
   `doc' specifies a document object.
   The return value is a duplicated document object. */
ESTDOC *est_doc_dup(ESTDOC *doc);


/* Set the ID number of a document object.
   `doc' specifies a document object.
   `id' specifies the ID number to set. */
void est_doc_set_id(ESTDOC *doc, int id);


/* Get the hidden texts of a document object.
   `doc' specifies a document object.
   The return value is concatenated sentences of the hidden text of the document object.  The
   life duration of the returned string is synchronous with the one of the document object. */
const char *est_doc_hidden_texts(ESTDOC *doc);


/* Reduce the texts to fit to the specified size.
   `doc' specifies a document object.
   `len' specifies the total size of the texts. */
void est_doc_slim(ESTDOC *doc, int size);


/* Check whether a docuemnt object is empty.
   `doc' specifies a document object.
   The return value is true the document is empty, else it is false. */
int est_doc_is_empty(ESTDOC *doc);


/* Duplicate a condition object.
   `cond' specifies a condition object.
   The return value is a duplicated condition object. */
ESTCOND *est_cond_dup(ESTCOND *cond);


/* Get the phrase of a condition object.
   `cond' specifies a condition object.
   The return value is the phrase of the condition object or `NULL' if it is not specified.  The
   life duration of the returned string is synchronous with the one of the condition object. */
const char *est_cond_phrase(ESTCOND *cond);


/* Get a list object of attribute expressions of a condition object.
   `cond' specifies a condition object.
   The return value is a list object of attribute expressions of the condition object or `NULL' if
   it is not specified.  The life duration of the returned object is synchronous with the one of
   the condition object. */
const CBLIST *est_cond_attrs(ESTCOND *cond);


/* Get the order expression of a condition object.
   `cond' specifies a condition object.
   The return value is the order expression of the condition object or `NULL' if it is not
   specified.  The life duration of the returned string is synchronous with the one of the
   condition object. */
const char *est_cond_order(ESTCOND *cond);


/* Get the maximum number of retrieval of a condition object.
   `cond' specifies a condition object.
   The return value is the maximum number of retrieval of the condition object or -1 if it is not
   specified. */
int est_cond_max(ESTCOND *cond);


/* Get the number of skipped documents of a condition object.
   `cond' specifies a condition object.
   The return value is the number of documents to be skipped in the search result. */
int est_cond_skip(ESTCOND *cond);


/* Get the options of a condition object.
   `cond' specifies a condition object.
   The return value is the options of the condition object. */
int est_cond_options(ESTCOND *cond);


/* Get permission to adopt result of the auxiliary index.
   `cond' specifies a condition object.
   The return value is permission to adopt result of the auxiliary index. */
int est_cond_auxiliary(ESTCOND *cond);


/* Get the attribute distinction filter.
   `cond' specifies a condition object.
   The return value is the name of the distinct attribute or `NULL' if it is not specified.  The
   life duration of the returned string is synchronous with the one of the condition object. */
const char *est_cond_distinct(ESTCOND *cond);


/* Get the mask of targets of meta search.
   `cond' specifies a condition object.
   The return value is the mask of targets of meta search. */
int est_cond_mask(ESTCOND *cond);


/* Get the score of a document corresponding to a condition object.
   `cond' specifies a condition object.
   `index' specifies the index of an element of the result array of `est_db_search'.
   The return value is the score of the element or -1 if the index is out of bounds. */
int est_cond_score(ESTCOND *cond, int index);


/* Get the score array of corresponding documents of a condition object.
   `cond' specifies a condition object.
   `nump' specifies the pointer to a variable to which the number of elements in the score array
   is assigned.
   The return value is the score array of corresponding documents. */
const int *est_cond_scores(ESTCOND *cond, int *nump);


/* Set the narrowing scores of a condition object.
   `cond' specifies a condition object.
   `scores' specifies the pointer to an array of narrowing scores.  The life duration of the
   array should be equal to or longer than the condition object itself.
   `num' specifies the number of the array. */
void est_cond_set_narrowing_scores(ESTCOND *cond, const int *scores, int num);


/* Check whether a condition object has used the auxiliary index.
   `cond' specifies a condition object.
   `word' specifies a keyword to be checked.  If it is an empty string, whether at least one
   keyword is used is checked.
   The return value is true if the condition object has used the auxiliary index, else it is
   false */
int est_cond_auxiliary_word(ESTCOND *cond, const char *word);


/* Get an array of ID numbers of eclipsed docuemnts of a document in a condition object.
   `cond' specifies a condition object.
   `id' specifies the ID number of a parent document.
   `np' specifies the pointer to a variable to which the number of elements of the return value
   is assigned.
   The return value is an array whose elements expresse the ID numbers and their scores
   alternately. */
const int *est_cond_shadows(ESTCOND *cond, int id, int *np);


/* Set the callback function for query expansion.
   `cond' specifies a condition object.
   `func' specifies the pointer to a function.  The first argument of the callback specifies a
   word to be expand.  The second argument speciifes a list object into which renewed words to
   be stored. */
void est_cond_set_expander(ESTCOND *cond, void (*func)(const char *, CBLIST *));


/* Set the error code of a database.
   `db' specifies a database object.
   `ecode' specifies a error code to set. */
void est_db_set_ecode(ESTDB *db, int ecode);


/* Check whether an option is set.
   `db' specifies a database object.
   `option' specifies an option used when opening the database.
   The return value is 1 if the option is set, 0 if the option is not set, or -1 if it is
   unknown. */
int est_db_check_option(ESTDB *db, int option);


/* Get the inode number of a database.
   `db' specifies a database object.
   The return value is the inode number of the database. */
int est_db_inode(ESTDB *db);


/* Set the entity data of a document in a database.
   `db' specifies a database object connected as a writer.
   `id' specifies the ID number of a registered document.
   `ptr' specifies the pointer to a region of entity data.  If it is `NULL', the entity data is
   removed.
   `size' specifies the size of the region.
   The return value is true if success, else it is false. */
int est_db_set_doc_entity(ESTDB *db, int id, const char *ptr, int size);


/* Get the entity data of a document in a database.
   `db' specifies a database object.
   `id' specifies the ID number of a registered document.
   `sp' specifies the pointer to a variable to which the size of the region of the return value
   is assigned.
   The return value is the value of the entity data or `NULL' if it does not exist.  Because the
   region of the return value is allocated with the `malloc' call, it should be released with
   the `free' call if it is no longer in use. */
char *est_db_get_doc_entity(ESTDB *db, int id, int *sp);


/* Set the maximum number of expansion of wild cards.
   `db' specifies a database object.
   `num' specifies the maximum number of expansion of wild cards. */
void est_db_set_wildmax(ESTDB *db, int num);


/* Add a piece of meta data to a database.
   `db' specifies a database object connected as a writer.
   `name' specifies the name of a piece of meta data.
   `value' specifies the value of the meta data.  If it is `NULL', the meta data is removed. */
void est_db_add_meta(ESTDB *db, const char *name, const char *value);


/* Get a list of names of meta data of a database.
   `db' specifies a database object.
   The return value is a new list object of meta data names of the document object.  Because the
   object of the return value is opened with the function `cblistopen', it should be closed with
   the function `cblistclose' if it is no longer in use. */
CBLIST *est_db_meta_names(ESTDB *db);


/* Get the value of a piece of meta data of a database.
   `db' specifies a database object.
   `name' specifies the name of a piece of meta data.
   The return value is the value of the meta data or `NULL' if it does not exist.  Because the
   region of the return value is allocated with the `malloc' call, it should be released with
   the `free' call if it is no longer in use. */
char *est_db_meta(ESTDB *db, const char *name);


/* Extract keywords of a document object.
   `db' specifies a database object for TF-IDF tuning.  If it is `NULL', it is not used.
   `doc' specifies a document object.
   `max' specifies the maximum number of keywords to be extracted.
   The return value is a new map object of keywords and their scores in decimal string.  Because
   the object of the return value is opened with the function `cbmapopen', it should be closed
   with the function `cbmapclose' if it is no longer in use. */
CBMAP *est_db_etch_doc(ESTDB *db, ESTDOC *doc, int max);


/* Store a map object of keywords.
   `db' specifies a database object connected as a writer.
   `id' specifies the ID number of a document.
   `kwords' specifies a map object of keywords of the document.
   `weight' specifies weighting bias of scores.
   The return value is true if success, else it is false. */
int est_db_put_keywords(ESTDB *db, int id, CBMAP *kwords, double weight);


/* Remove keywords of a document.
   `db' specifies a database object connected as a writer.
   `id' specifies the ID number of a document.
   The return value is true if success, else it is false. */
int est_db_out_keywords(ESTDB *db, int id);


/* Retrieve a map object of keywords.
   `db' specifies a database object.
   `id' specifies the ID number of a document.
   The return value is a new map object of keywords and their scores in decimal string.  If
   keywords of the document is not stored, `NULL' is returned.  Because the object of the return
   value is opened with the function `cbmapopen', it should be closed with the function
   `cbmapclose' if it is no longer in use. */
CBMAP *est_db_get_keywords(ESTDB *db, int id);


/* Mesure the total size of each inner records of a stored document.
   `db' specifies a database object.
   `id' specifies the ID number of a document.
   `parts' specifies document parts: `ESTMDATTR' for attributes, `ESTMDTEXT' for texts, and
   `ESTMDKWD' for keywords.  They can be specified at the same time by bitwise or.
   The return value is the total size of each inner records of a stored document. */
int est_db_measure_doc(ESTDB *db, int id, int parts);


/* Initialize the document iterator of a database.
   `db' specifies a database object.
   `prev' specifies the URI of the previous element of iteration.  If it is `NULL', it is not used.
   The return value is true if success, else it is false. */
int est_db_iter_init(ESTDB *db, const char *prev);


/* Get the next ID of the document iterator of a database.
   `db' specifies a database object.
   The return value is the next ID.  If there is no more document, 0 is returned.  On error,
   -1 is returned. */
int est_db_iter_next(ESTDB *db);


/* Initialize the word iterator of a database.
   `db' specifies a database object.
   The return value is true if success, else it is false. */
int est_db_word_iter_init(ESTDB *db);


/* Get the next word of the word iterator of a database.
   `db' specifies a database object.
   The return value is the next word.  If there is no more word, `NULL' is returned.  Because
   the region of the return value is allocated with the `malloc' call, it should be released
   with the `free' call if it is no longer in use. */
char *est_db_word_iter_next(ESTDB *db);


/* Get the size of the record of a word.
   `db' specifies a database object.
   `word' specifies a word.
   The return value is the size of the record of the word.  If there is no corresponding record,
   0 is returned. */
int est_db_word_rec_size(ESTDB *db, const char *word);


/* Get the number of unique keywords in a database.
   `db' specifies a database object.
   The return value is the number of unique keywords in the database. */
int est_db_keyword_num(ESTDB *db);


/* Initialize the keyword iterator of a database.
   `db' specifies a database object.
   The return value is true if success, else it is false. */
int est_db_keyword_iter_init(ESTDB *db);


/* Get the next keyword of the word iterator of a database.
   `db' specifies a database object.
   The return value is the next word.  If there is no more keyword, `NULL' is returned.  Because
   the region of the return value is allocated with the `malloc' call, it should be released
   with the `free' call if it is no longer in use. */
char *est_db_keyword_iter_next(ESTDB *db);


/* Get the size of the record of a keyword.
   `db' specifies a database object.
   `word' specifies a keyword.
   The return value is the size of the record of the keyword.  If there is no corresponding
   record, 0 is returned. */
int est_db_keyword_rec_size(ESTDB *db, const char *word);


/* Search documents corresponding a keyword for a database.
   `db' specifies a database object.
   `word' specifies a keyword.
   `nump' specifies the pointer to a variable to which the number of elements in the result is
   assigned.
   The return value is an array whose elements are ID numbers of corresponding documents.
   This function does never fail.  Even if no document corresponds or an error occurs, an empty
   array is returned.  Because the region of the return value is allocated with the `malloc'
   call, it should be released with the `free' call if it is no longer in use. */
int *est_db_keyword_search(ESTDB *db, const char *word, int *nump);


/* Get the number of records in the cache memory of a database.
   `db' specifies a database object.
   The return value is the cache memory of a database. */
int est_db_cache_num(ESTDB *db);


/* Get the size of used cache region.
   `db' specifies a database object.
   The return value is the size of used cache region. */
int est_db_used_cache_size(ESTDB *db);


/* Set the special cache for narrowing and sorting with document attributes.
   `db' specifies a database object.
   `name' specifies the name of a document.
   `num' specifies the maximum number of cached records. */
void est_db_set_special_cache(ESTDB *db, const char *name, int num);


/* Set the callback function to inform of database events.
   `db' specifies a database object.
   `func' specifies the pointer to a function.  The first argument of the callback specifies a
   message of each event.  The second argument specifies an arbitrary pointer of a opaque data.
   `opaque' specifies the pointer of the second argument of the callback. */
void est_db_set_informer(ESTDB *db, void (*func)(const char *, void *), void *opaque);


/* Fill the cache for keys for TF-IDF.
   `db' specifies a database object. */
void est_db_fill_key_cache(ESTDB *db);


/* Set the database of document frequency.
   `db' specifies a database object.
   `dfdb' specifies a database object of `DEPOT'.  If it is `NULL', the setting is cleared. */
void est_db_set_dfdb(ESTDB *db, DEPOT *dfdb);


/* Clear the result cache.
   `db' specifies a database object. */
void est_db_refresh_rescc(ESTDB *db);


/* Charge the result cache.
   `db' specifies a database object.
   `max' specifies the maximum number of words to be charged.  If it not more than zero, all
   words are charged. */
void est_db_charge_rescc(ESTDB *db, int max);


/* Get a list of words in the result cache.
   `db' specifies a database object.
   The return value is a new list object of words in the result cache.  Because the object of the
   return value is opened with the function `cblistopen', it should be closed with the function
   `cblistclose' if it is no longer in use. */
CBLIST *est_db_list_rescc(ESTDB *db);


/* Get the number of pseudo documents in a database.
   `db' specifies a database object.
   The return value is the number of pseudo documents in the database. */
int est_db_pseudo_doc_num(ESTDB *db);


/* Get a list of expressions of attribute indexes of a database.
   `db' specifies a database object.
   The return value is a new list object of expressions of attribute indexes.  Because the object
   of the return value is opened with the function `cblistopen', it should be closed with the
   function `cblistclose' if it is no longer in use. */
CBLIST *est_db_attr_index_exprs(ESTDB *db);


/* Interrupt long time processing.
   `db' specifies a database object. */
void est_db_interrupt(ESTDB *db);


/* Repair a broken database directory.
   `name' specifies the name of a database directory.
   `options' specifies options: `ESTRPSTRICT' to perform strict consistency check, `ESTRPSHODDY'
   to omit consistency check.
   `ecp' specifies the pointer to a variable to which the error code is assigned.
   The return value is true if success, else it is false. */
int est_db_repair(const char *name, int options, int *ecp);


/* Extract words for snippet from hints of search.
   `hints' specifies a map object whose records were set by `est_db_search'.
   The return value is a new list object of words to be highlighted.  Because the object of the
   return value is opened with the function `cblistopen', it should be closed with the function
   `cblistclose' if it is no longer in use. */
CBLIST *est_hints_to_words(CBMAP *hints);


/* Add a record into a result map for logical operation.
   `map' specifies a map object.
   `key' specifies the key of a record.
   `score' specifies the score of the record.
   `method' specifies a scoring method when logical operation.  As for now, `ESTRMLOSUM',
   `ESTRMLOMAX', `ESTRMLOMIN', and `ESTRMLOAVG'. */
void est_resmap_add(CBMAP *map, const char *key, int score, int method);


/* Dump a result list of a result map for logical operation.
   `map' specifies a map object.
   `min' specifies the minimum number of times for which each element of the result occurs.
   `nump' specifies the pointer to a variable to which the number of elements in the result is
   assigned.
   The return value is an array whose elements are structures of keys and scores.  Because the
   region of the return value is allocated with the `malloc' call, it should be released with the
   `free' call if it is no longer in use. */
ESTRESMAPELEM *est_resmap_dump(CBMAP *map, int min, int *nump);


/* Reset the environment of the process.
   This function sets the standard streams as binary mode and resets environment variables for
   locale. */
void est_proc_env_reset(void);


/* Make a directory.
   `path' specifies the path of a new directory.
   The return value is true if success, else it is false. */
int est_mkdir(const char *path);


/* Remove a directory and its contents recursively.
   `path' specifies the path of a directory.
   The return value is true if success, else it is false. */
int est_rmdir_rec(const char *path);


/* Get the canonicalized absolute pathname of a file.
   `path' specifies the path of a file.
   The return value is the canonicalized absolute pathname of a file.  Because the region of the
   return value is allocated with the `malloc' call, it should be released with the `free' call
   if it is no longer in use. */
char *est_realpath(const char *path);


/* Get the inode number of a file.
   `path' specifies the path of a file.
   The return value is the inode number of a file or -1 on error. */
int est_inode(const char *path);


/* Change modification time of a file.
   `path' specifies the path of a file.
   `mtime' specifies modification time.  If it is negative, the current time is set.
   The return value is true if success, else it is false. */
int est_utime(const char *path, time_t mtime);


/* Get the time of day in milliseconds.
   The return value is the time of day in milliseconds. */
double est_gettimeofday(void);


/* Suspend execution for microsecond intervals.
   `usec' specifies microseconds to sleep for. */
void est_usleep(unsigned long usec);


/* Set a signal handler.
   `signum' specifies the number of a target signal.
   `sighandler' specifies the pointer to a function.  The argument of the handler specifies the
   number of the catched signal.  If it is `SIG_IGN', the signal is ignored. */
void est_signal(int signum, void (*sighandler)(int));


/* Send a signal to a process.
   `pid' specifies the PID of a target process.
   `sig' specifies a signal code.
   The return value is true if success, else it is false. */
int est_kill(int pid, int sig);


/* Get the load ratio of the physical memory.
   The return value is the load ratio of the physical memory.
   As for now, this function returns 0.0 on platforms except for Windows. */
double est_memory_usage(void);


/* Get the media type of an extention.
   `ext' specifies the extension of a file path.
   The return value is the media time of the extension. */
const char *est_ext_type(const char *ext);


/* Set a seed vector from a map object.
   `svmap' specifies a map object of a seed vector.
   `svec' specifies a vector object.
   `vnum' specifies the number of dimensions of the vector. */
void est_vector_set_seed(CBMAP *svmap, int *svec, int vnum);


/* Set a target vector from a map object.
   `svmap' specifies a map object of a seed vector.
   `tvmap' specifies a map object of a target vector.
   `tvec' specifies a vector object.
   `vnum' specifies the number of dimensions of the vector. */
void est_vector_set_target(CBMAP *svmap, CBMAP *tvmap, int *tvec, int vnum);


/* Get the cosine of the angle of two vectors.
   `avec' specifies a vector object.
   `bvec' specifies the other vector object.
   `vnum' specifies the number of dimensions of the vector.
   The return value is the cosine of the angle of two vectors. */
double est_vector_cosine(const int *avec, const int *bvec, int vnum);



#if defined(__cplusplus)                 /* export for C++ */
}
#endif

#endif                                   /* duplication check */


/* END OF FILE */
