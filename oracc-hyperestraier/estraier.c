/*************************************************************************************************
 * Implementation of the core API
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


#if defined(_MYVISTA)
#include <vista.h>
#endif

#include "estraier.h"
#include "myconf.h"

#define ESTNUMBUFSIZ   32                /* size of a buffer for a number */
#define ESTPATHBUFSIZ  4096              /* size of a buffer for a path */
#define ESTIOBUFSIZ    8192              /* size of a buffer for I/O */
#define ESTALLOCUNIT   1024              /* unit number of memory allocation */
#define ESTMINIBNUM    31                /* bucket number of map for attributes */
#define ESTSCANWNUM    256               /* number of words for scaning check */
#define ESTSIGNUM      64                /* number of signals */
#define ESTREGSUBMAX   32                /* maximum number of substrings for regex */

#define ESTMETADBNAME  "_meta"           /* name of the meta database */
#define ESTKEYIDXNUM   "_idxnum"         /* key for the number of inverted indexes */
#define ESTKEYDSEQ     "_dseq"           /* key for the sequence for document IDs */
#define ESTKEYDNUM     "_dnum"           /* key for the number of documents */
#define ESTKEYMETA     "_meta"           /* key for meta data */

#define ESTIDXDBNAME   "_idx"            /* name of the inverted index */
#define ESTIDXDBLRM    109               /* records in a leaf node of the inverted index */
#define ESTIDXDBLRMA   17                /* records in a leaf node of the index in APN mode */
#define ESTIDXDBNIM    160               /* records in a non-leaf node of the inverted index */
#define ESTIDXDBLCN    16                /* number of leaf cache of the inverted index */
#define ESTIDXDBNCN    16                /* number of non-leaf cache of the inverted index */
#define ESTIDXDBRLCN   128               /* number of leaf cache of the index reader */
#define ESTIDXDBRLCNA  32                /* number of leaf cache of the reader in APN mode */
#define ESTIDXDBRNCN   256               /* number of non-leaf cache of the index reader */
#define ESTIDXDBFBP    512               /* size of free block pool of the inverted index */
#define ESTIDXDBMIN    (1048576*512)     /* minimum size of a database file */
#define ESTIDXDBMAX    (1048576*1536)    /* maximum size of a database file */

#define ESTFWMDBNAME   "_fwm"            /* name of the database for forward matching */
#define ESTFWMDBLRM    251               /* records in a leaf node of forward matching DB */
#define ESTFWMDBNIM    110               /* records in a non-leaf node of forward matching DB */
#define ESTFWMDBLCN    32                /* number of leaf cache of forward matching DB */
#define ESTFWMDBNCN    16                /* number of non-leaf cache of forward matching DB */
#define ESTFWMDBFBP    128               /* size of free block pool of forward matching DB */

#define ESTAUXDBNAME   "_aux"            /* name of the auxiliary index */
#define ESTAUXDBLRM    23                /* records in a leaf node of the auxiliary index */
#define ESTAUXDBNIM    160               /* records in a non-leaf node of the auxiliary index */
#define ESTAUXDBLCN    16                /* number of leaf cache of the auxiliary index */
#define ESTAUXDBNCN    16                /* number of non-leaf cache of the auxiliary index */
#define ESTAUXDBRLCN   256               /* number of leaf cache of the auxiliary reader */
#define ESTAUXDBRNCN   64                /* number of non-leaf cache of the auxiliary reader */
#define ESTAUXDBFBP    256               /* size of free block pool of the auxiliary index */

#define ESTXFMDBNAME   "_xfm"            /* name of the database for auxiliary forward matching */
#define ESTXFMDBLRM    111               /* records in a leaf node of xfm DB */
#define ESTXFMDBNIM    110               /* records in a non-leaf node of xfm DB */
#define ESTXFMDBLCN    32                /* number of leaf cache of xfm DB */
#define ESTXFMDBNCN    16                /* number of non-leaf cache of xfm DB */
#define ESTXFMDBFBP    128               /* size of free block pool of xfm DB */

#define ESTATTRDBNAME  "_attr"           /* name of the database for attributes */
#define ESTATTRDBBNUM  212987            /* bucket number of the database for attributes */
#define ESTATTRDBDNUM  3                 /* division number of the database for attributes */
#define ESTATTRDBALN   -5                /* alignment of the database for attributes */
#define ESTATTRDBFBP   64                /* size of free block pool of the attribute DB */

#define ESTTEXTDBNAME  "_text"           /* name of the database of texts */
#define ESTTEXTDBBNUM  61417             /* bucket number of the database for texts */
#define ESTTEXTDBDNUM  7                 /* division number of the database for texts */
#define ESTTEXTDBALN   -5                /* alignment of the database for texts */
#define ESTTEXTDBFBP   128               /* size of free block pool of the text DB */

#define ESTKWDDBNAME   "_kwd"            /* name of the database of keywords */
#define ESTKWDDBBNUM   163819            /* bucket number of the database for keywords */
#define ESTKWDDBDNUM   3                 /* division number of the database for keywords */
#define ESTKWDDBALN    -5                /* alignment of the database for keywords */
#define ESTKWDDBFBP    64                /* size of free block pool of the keyword DB */

#define ESTLISTDBNAME  "_list"           /* name of the database of document list */
#define ESTLISTDBLRM   99                /* records in a leaf node of document list DB */
#define ESTLISTDBNIM   200               /* records in a non-leaf node of document list DB */
#define ESTLISTDBLCN   64                /* number of leaf cache of document list DB */
#define ESTLISTDBNCN   16                /* number of non-leaf cache of document list DB */
#define ESTLISTDBFBP   128               /* size of free block pool of document list DB */

#define ESTAISEQPREF   "__seq_"          /* prefix of the database for sequencial access */
#define ESTAISTRPREF   "__str_"          /* prefix of the database for string narrowing */
#define ESTAINUMPREF   "__num_"          /* prefix of the database for number narrowing */
#define ESTAIBDIAM     0.8               /* diameter of the bucket number */
#define ESTAIDXLRM     99                /* records in a leaf node of narrowing index */
#define ESTAIDXNIM     120               /* records in a non-leaf node of narrowing index */
#define ESTAIDXLCN     1024              /* number of leaf cache of narrowing index */
#define ESTAIDXNCN     256               /* number of non-leaf cache of narrowing index */
#define ESTAIDXDPFBP   32                /* size of free block pool of sequencial DB */
#define ESTAIDXVLFBP   128               /* size of free block pool of narrowing DB */
#define ESTAIKBUFSIZ   8192              /* size of a buffer for a key */
#define ESTAISNUMMIN   256               /* minimum number of scores to use narrowing index */
#define ESTOPDUMMY     "[DUMMY]"         /* dummy operator */

#define ESTDBSBRAT     0.3               /* ratio of bucket numbers of large mode */
#define ESTDBSDRAT     0.4               /* ratio of the division number of large mode */
#define ESTDBLBRAT     3.0               /* ratio of bucket numbers of large mode */
#define ESTDBLDRAT     1.0               /* ratio of the division number of large mode */
#define ESTDBHBRAT     5.0               /* ratio of bucket numbers of huge mode */
#define ESTDBHDRAT     2.0               /* ratio of the division number of huge mode */
#define ESTDBH2RAT     1.4               /* ratio of huge mode second */
#define ESTDBH3RAT     2.0               /* ratio of huge mode third */

#define ESTVLCRDNUM    2                 /* division number of usual Villa databases */
#define ESTVLCRDNAUX   7                 /* division number of the auxiliary index */

#define ESTIDXCCBNUM   524288            /* bucket number of cache for the inverted index */
#define ESTAUXCCBNUM   65521             /* bucket number of cache for the auxiliary index */
#define ESTIDXCCMAX    (1048576*64)      /* max size of the cache */
#define ESTOUTCCBNUM   131072            /* bucket number of cache for deleted documents */
#define ESTKEYCCMNUM   65536             /* bucket number of cache for keys for TF-IDF */
#define ESTATTRCCMNUM  8192              /* number of cache for attributes */
#define ESTTEXTCCMNUM  1024              /* number of cache for texts */
#define ESTRESCCMNUM   256               /* number of cache for results */
#define ESTCCIRSLOT    256               /* slot timing for interruption */
#define ESTCCCBFREQ    10000             /* frequency of callback for flushing words */

#define ESTDIRMODE     00755             /* permission of a creating directory */
#define ESTICCHECKSIZ  32768             /* size of checking character code */
#define ESTICMISSMAX   256               /* allowance number of missing characters */
#define ESTICALLWRAT   0.001             /* allowance ratio of missing characters */
#define ESTOCPOINT     16                /* point per occurrence */
#define ESTJHASHNUM    251               /* hash number for a junction */
#define ESTWORDMAXLEN  48                /* maximum length of a word */
#define ESTWORDAVGLEN  8                 /* average length of a word */
#define ESTATTRALW     1.5               /* allowance ratio of attribute narrowing */
#define ESTKEYSCALW    3                 /* allowance ratio of TF-IDF for keywords */
#define ESTMEMIRATIO   1.1               /* incremental ratio of memory allocation */

#define ESTSCOREUNIT   1000              /* unit of standard deviation of scoring */
#define ESTAUXMIN      32                /* minimum hits to adopt the auxiliary index */
#define ESTAUXEXRAT    16                /* ratio of hits of keywords expansion */
#define ESTWILDMAX     256               /* maximum number of expansion of wild cards */
#define ESTECLKNUM     32                /* number of keywords to eclipse candidates */
#define ESTSMLRKNUM    16                /* number of keywords to get candidates */
#define ESTSMLRUNUM    1024              /* number of adopted documents for a keyword */
#define ESTSMLRMNUM    4096              /* maximum number of candidates to be checked */
#define ESTSMLRNMIN    0.5               /* the minimum value for narrowing */

/* set a buffer for a variable length number */
#define EST_SET_VNUMBUF(EST_len, EST_buf, EST_num) \
  do { \
    int _EST_num = (EST_num); \
    div_t EST_d; \
    if(_EST_num == 0){ \
      ((signed char *)(EST_buf))[0] = 0; \
      (EST_len) = 1; \
    } else { \
      (EST_len) = 0; \
      while(_EST_num > 0){ \
        EST_d = div(_EST_num, 128); \
        _EST_num = EST_d.quot; \
        if(_EST_num > 0){ \
          ((signed char *)(EST_buf))[(EST_len)] = -EST_d.rem - 1; \
        } else { \
          ((signed char *)(EST_buf))[(EST_len)] = EST_d.rem; \
        } \
        (EST_len)++; \
      } \
    } \
  } while(FALSE)

/* read a variable length buffer */
#define EST_READ_VNUMBUF(EST_buf, EST_num, EST_step) \
  do { \
    int _EST_i, _EST_base; \
    (EST_num) = 0; \
    _EST_base = 1; \
    for(_EST_i = 0; TRUE; _EST_i++){ \
      if(((signed char *)(EST_buf))[_EST_i] >= 0){ \
        (EST_num) += ((signed char *)(EST_buf))[_EST_i] * _EST_base; \
        break; \
      } \
      (EST_num) += _EST_base * (((signed char *)(EST_buf))[_EST_i] + 1) * -1; \
      _EST_base *= 128; \
    } \
    EST_step = _EST_i + 1; \
  } while(FALSE)

typedef struct {                         /* type of structure for an attribute database */
  void *db;                              /* handle of the database */
  int type;                              /* data type */
} ESTATTRIDX;

enum {                                   /* enumeration for character categories */
  ESTSPACECHR,                           /* space characters */
  ESTDELIMCHR,                           /* delimiter characters */
  ESTWESTALPH,                           /* west alphabets */
  ESTEASTALPH,                           /* east alphabets */
  ESTHIRAGANA,                           /* east alphabets: hiragana */
  ESTKATAKANA,                           /* east alphabets: katakana */
  ESTHANGUL,                             /* east alphabets: hangul */
  ESTKANJI                               /* east alphabets: kanji */
};

enum {                                   /* enumeration for flags for databases */
  ESTDFPERFNG = 1 << 10,                 /* use perfect N-gram analizer */
  ESTDFCHRCAT = 1 << 11,                 /* use character category analizer */
  ESTDFZLIB = 1 << 15,                   /* compress records with ZLIB */
  ESTDFLZO = 1 << 16,                    /* compress records with LZO */
  ESTDFBZIP = 1 << 17,                   /* compress records with BZIP2 */
  ESTDFSCVOID = 1 << 20,                 /* store scores as void */
  ESTDFSCINT = 1 << 21,                  /* store scores as integer */
  ESTDFSCASIS = 1 << 22                  /* refrain from adjustment of scores */
};

enum {                                   /* enumration for phrase format */
  ESTPMUSUAL,                            /* usual phrase */
  ESTPMSIMPLE,                           /* simplified phrase */
  ESTPMROUGH,                            /* rough phrase */
  ESTPMUNION,                            /* union phrase */
  ESTPMISECT                             /* intersection phrase */
};

typedef struct {                         /* type of structure for a hitting object */
  int id;                                /* ID of a document */
  int score;                             /* score tuned by TF-IDF */
  char *value;                           /* value of an attribute for sorting */
} ESTSCORE;

typedef struct {                         /* type of structure for a conditional attribute */
  char *name;                            /* name */
  int nsiz;                              /* size of the name */
  CBLIST *nlist;                         /* list of plural names */
  char *oper;                            /* operator */
  char *val;                             /* value */
  int vsiz;                              /* size of the value */
  const char *cop;                       /* canonical operator */
  int sign;                              /* positive or negative */
  char *sval;                            /* value of small cases */
  int ssiz;                              /* size of the small value */
  void *regex;                           /* compiled regular expressions */
  time_t num;                            /* numeric value */
} ESTCATTR;

typedef struct {                         /* type of structure for a hitting object */
  const char *word;                      /* face of keyword */
  int wsiz;                              /* size of the keyword */
  int pt;                                /* score tuned by TF-IDF */
} ESTKEYSC;

typedef struct {                         /* type of structure for a meta hitting object */
  int db;                                /* index of a container database */
  int id;                                /* ID of a document */
  int score;                             /* score tuned by TF-IDF */
  char *value;                           /* value of an attribute for sorting */
} ESTMETASCORE;


/* private function prototypes */
static void est_set_ecode(int *ecp, int value, int line);
static char *est_hex_encode(const char *str);
static char *est_hex_decode(const char *str);
static int est_enc_miss(const char *ptr, int size, const char *icode, const char *ocode);
static void est_normalize_text(unsigned char *utext, int size, int *sp);
static void est_canonicalize_text(unsigned char *utext, int size, int funcspc);
static int est_char_category(int c);
static int est_char_category_perfng(int c);
static int est_char_category_chrcat(int c);
static char *est_make_snippet(const char *str, int len, const CBLIST *words,
                              int wwidth, int hwidth, int awidth);
static int est_check_cjk_only(const char *str);
static char *est_phrase_from_simple(const char *sphrase);
static char *est_phrase_from_rough(const char *rphrase);
static char *est_phrase_from_union(const char *uphrase);
static char *est_phrase_from_isect(const char *iphrase);
static void est_snippet_add_text(const unsigned char *rtext, const unsigned char *ctext,
                                 int size, int awsiz, CBDATUM *res, const CBLIST *rwords);
static int est_str_fwmatch_wide(const unsigned char *haystack, int hsiz,
                                const unsigned char *needle, int nsiz);
static char *est_strstr_sparse(const char *haystack, const char *needle);
static int est_idx_rec_last_id(const char *vbuf, int vsiz, int smode);
static void est_encode_idx_rec(CBDATUM *datum, const char *vbuf, int vsiz, int lid, int smode);
static void est_decode_idx_rec(CBDATUM *datum, const char *vbuf, int vsiz, int smode);
static ESTIDX *est_idx_open(const char *name, int omode, int dnum);
static int est_idx_close(ESTIDX *idx);
static void est_idx_set_tuning(ESTIDX *idx, int lrecmax, int nidxmax, int lcnum, int ncnum,
                               int fbpsiz);
static void est_idx_increment(ESTIDX *idx);
static int est_idx_dnum(ESTIDX *idx);
static int est_idx_add(ESTIDX *idx, const char *word, int wsiz,
                       const char *vbuf, int vsiz, int smode);
static int est_idx_put_one(ESTIDX *idx, int inum, const char *word, int wsiz,
                           const char *vbuf, int vsiz);
static int est_idx_out(ESTIDX *idx, const char *word, int wsiz);
static char *est_idx_scan(ESTIDX *idx, const char *word, int wsiz, int *sp, int smode);
static const char *est_idx_get_one(ESTIDX *idx, int inum, const char *word, int wsiz, int *sp);
static int est_idx_vsiz(ESTIDX *idx, const char *word, int wsiz);
static int est_idx_num(ESTIDX *idx);
static double est_idx_size(ESTIDX *idx);
static int est_idx_size_current(ESTIDX *idx);
static int est_idx_memflush(ESTIDX *idx);
static int est_idx_sync(ESTIDX *idx);
static int est_idx_optimize(ESTIDX *idx);
static void est_idx_set_current(ESTIDX *idx);
static int est_crput(CURIA *curia, int zmode, int id, const char *vbuf, int vsiz, int dmode);
static int est_crout(CURIA *curia, int id);
static char *est_crget(CURIA *curia, int flags, int id, int *sp);
static int est_aidx_seq_put(DEPOT *db, int id, const char *vbuf, int vsiz);
static int est_aidx_seq_out(DEPOT *db, int id);
static char *est_aidx_seq_get(DEPOT *db, int id, int *sp);
static int est_aidx_seq_narrow(DEPOT *db, const CBLIST *pdocs, const char *cop, int sign,
                               const char *oval, int osiz, const char *sval, int ssiz,
                               const void *regex, int onum, ESTSCORE *scores, int snum,
                               int limit, int *restp);
static int est_aidx_numcmp(const char *aptr, int asiz, const char *bptr, int bsiz);
static int est_aidx_attr_put(VILLA *db, int id, const char *vbuf, int vsiz);
static int est_aidx_attr_out(VILLA *db, int id, const char *vbuf, int vsiz);
static int est_aidx_attr_narrow(VILLA *db, const CBLIST *pdocs, const char *cop, int sign,
                                const char *oval, int osiz, const char *sval, int ssiz,
                                const void *regex, int onum, ESTSCORE *scores, int snum);
static int est_int_compare(const void *ap, const void *bp);
static int est_short_compare(const void *ap, const void *bp);
static void est_inodes_delete(void *arg);
static void est_inodes_delete_informer(const char *msg, void *opaque);
static int est_db_write_meta(ESTDB *db);
static void est_db_inform(ESTDB *db, const char *info);
static void est_db_prepare_meta(ESTDB *db);
static int est_db_score_doc(ESTDB *db, ESTDOC *doc, ESTCOND *cond, int *scp);
static int est_pidx_uri_to_id(ESTDB *db, const char *uri);
static CBLIST *est_phrase_terms(const char *phrase);
static int est_score_compare_by_id_asc(const void *ap, const void *bp);
static int est_score_compare_by_id_desc(const void *ap, const void *bp);
static int est_score_compare_by_score_asc(const void *ap, const void *bp);
static int est_score_compare_by_score_desc(const void *ap, const void *bp);
static int est_score_compare_by_str_asc(const void *ap, const void *bp);
static int est_score_compare_by_str_desc(const void *ap, const void *bp);
static int est_score_compare_by_num_asc(const void *ap, const void *bp);
static int est_score_compare_by_num_desc(const void *ap, const void *bp);
static int est_metascore_compare_by_id_asc(const void *ap, const void *bp);
static int est_metascore_compare_by_id_desc(const void *ap, const void *bp);
static int est_metascore_compare_by_score_asc(const void *ap, const void *bp);
static int est_metascore_compare_by_score_desc(const void *ap, const void *bp);
static int est_metascore_compare_by_str_asc(const void *ap, const void *bp);
static int est_metascore_compare_by_str_desc(const void *ap, const void *bp);
static int est_metascore_compare_by_num_asc(const void *ap, const void *bp);
static int est_metascore_compare_by_num_desc(const void *ap, const void *bp);
static ESTSCORE *est_search_uvset(ESTDB *db, int *nump, CBMAP *hints, int add);
static void est_expand_word_bw(ESTDB *db, const char *word, CBLIST *list);
static void est_expand_word_ew(ESTDB *db, const char *word, CBLIST *list);
static void est_expand_word_rx(ESTDB *db, const char *word, CBLIST *list);
static void est_expand_keyword_bw(ESTDB *db, const char *word, CBLIST *list);
static void est_expand_keyword_ew(ESTDB *db, const char *word, CBLIST *list);
static void est_expand_keyword_rx(ESTDB *db, const char *word, CBLIST *list);
static ESTSCORE *est_search_union(ESTDB *db, const char *term, int gstep,
                                  void (*xpn)(const char *, CBLIST *),
                                  int *nump, CBMAP *hints, int add, int auxmin, CBMAP *auxwords);
static const ESTSCORE *est_rescc_get(ESTDB *db, const char *word, int size, int *nump);
static void est_rescc_put(ESTDB *db, const char *word, int size, ESTSCORE *scores, int num);
static ESTSCORE *est_search_keywords(ESTDB *db, const char *word, int min, int *nump);
static void est_weight_keywords(ESTDB *db, const char *word, ESTSCORE *scores, int snum);
static ESTSCORE *est_search_rank(ESTDB *db, const char *name, int top, int *nump);
static ESTSCORE *est_search_aidx_attr(ESTDB *db, const char *expr, int *nump);
static ESTSCORE *est_search_pidxs(ESTDB *db, ESTCOND *cond, ESTSCORE *scores, int *nump,
                                  CBMAP *ordattrs);
static int est_narrow_scores(ESTDB *db, const CBLIST *attrs, int ign,
                             const char *order, const char *distinct, ESTSCORE *scores, int snum,
                             int limit, int *restp, CBMAP *ordattrs);
static ESTCATTR *est_make_cattr_list(const CBLIST *attrs, int *nump);
static void est_free_cattr_list(ESTCATTR *list, int anum);
static int est_eclipse_scores(ESTDB *db, ESTSCORE *scores, int snum, int num,
                              int vnum, int tfidf, double limit, CBMAP *shadows);
static int est_match_attr(const char *tval, int tsiz, const char *cop, int sign,
                          const char *oval, int osiz, const char *sval, int ssiz,
                          const void *regex, int onum);
static int est_check_strand(const char *tval, const char *oval);
static int est_check_stror(const char *tval, const char *oval);
static int est_check_stroreq(const char *tval, const char *oval);
static int est_check_numbt(const char *tval, const char *oval);
static int est_keysc_compare(const void *ap, const void *bp);
static ESTSCORE *est_search_similar(ESTDB *db, CBMAP *svmap, int *nump,
                                    int knum, int unum, int mnum, int tfidf,
                                    double nmin, int auxmin, CBMAP *auxwords);
static CBMAP *est_phrase_vector(const char *phrase);
static CBMAP *est_get_tvmap(ESTDB *db, int id, int vnum, int tfidf);
static int est_url_sameness(const char *aurl, const char *burl);
static void est_random_fclose(void);
static int est_signal_dispatch(int signum);



/*************************************************************************************************
 * common settings
 *************************************************************************************************/


/* version of Hyper Estraier */
const char *est_version = _EST_VERSION;



/*************************************************************************************************
 * API for document
 *************************************************************************************************/


/* Create a document object. */
ESTDOC *est_doc_new(void){
  ESTDOC *doc;
  CB_MALLOC(doc, sizeof(ESTDOC));
  doc->id = -1;
  doc->attrs = NULL;
  doc->dtexts = NULL;
  doc->kwords = NULL;
  return doc;
}


/* Create a document object made from draft data. */
ESTDOC *est_doc_new_from_draft(const char *draft){
  ESTDOC *doc;
  CBLIST *lines;
  const char *line;
  char *pv, *rp, *ep;
  int i;
  assert(draft);
  doc = est_doc_new();
  lines = cbsplit(draft, -1, "\n");
  for(i = 0; i < CB_LISTNUM(lines); i++){
    line = CB_LISTVAL(lines, i);
    while(*line > '\0' && *line <= ' '){
      line++;
    }
    if(*line == '\0'){
      i++;
      break;
    }
    if(*line == '%'){
      if(cbstrfwmatch(line, ESTDCNTLVECTOR)){
        if(!doc->kwords) doc->kwords = cbmapopenex(ESTMINIBNUM);
        if((rp = strchr(line, '\t')) != NULL) rp++;
        while(rp && (pv = strchr(rp, '\t')) != NULL){
          pv++;
          if((ep = strchr(pv, '\t')) != NULL){
            *ep = '\0';
            ep++;
          }
          if(rp[0] != '\0' && pv[0] != '\0') cbmapput(doc->kwords, rp, pv - rp - 1, pv, -1, TRUE);
          rp = ep;
        }
      } else if(cbstrfwmatch(line, ESTDCNTLSCORE)){
        if((rp = strchr(line, '\t')) != NULL) est_doc_set_score(doc, atoi(rp + 1));
      }
    } else if((pv = strchr(line, '=')) != NULL){
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
  CB_LISTCLOSE(lines);
  return doc;
}


/* Destroy a document object. */
void est_doc_delete(ESTDOC *doc){
  assert(doc);
  if(doc->kwords) cbmapclose(doc->kwords);
  if(doc->dtexts) CB_LISTCLOSE(doc->dtexts);
  if(doc->attrs) cbmapclose(doc->attrs);
  free(doc);
}


/* Add an attribute to a document object. */
void est_doc_add_attr(ESTDOC *doc, const char *name, const char *value){
  char *rbuf, *wp;
  int len;
  assert(doc && name);
  if(name[0] == '\0' || name[0] == '%') return;
  if(!doc->attrs) doc->attrs = cbmapopenex(ESTMINIBNUM);
  if(value){
    rbuf = cbmemdup(value, -1);
    for(wp = rbuf; *wp != '\0'; wp++){
      if(*wp > 0 && *wp < ' ') *wp = ' ';
    }
    cbstrsqzspc(rbuf);
    if((len = strlen(name)) > 0) cbmapput(doc->attrs, name, len, rbuf, -1, TRUE);
    free(rbuf);
  } else {
    cbmapout(doc->attrs, name, -1);
  }
}


/* Add a sentence of text to a document object. */
void est_doc_add_text(ESTDOC *doc, const char *text){
  unsigned char *utext;
  char *rtext, *wp;
  int size;
  assert(doc && text);
  while(*text > '\0' && *text <= ' '){
    text++;
  }
  if(text[0] == '\0') return;
  if(!doc->dtexts) CB_LISTOPEN(doc->dtexts);
  utext = (unsigned char *)est_uconv_in(text, strlen(text), &size);
  est_normalize_text(utext, size, &size);
  rtext = est_uconv_out((char *)utext, size, NULL);
  for(wp = rtext; *wp != '\0'; wp++){
    if(*wp > 0 && *wp < ' ') *wp = ' ';
  }
  cbstrsqzspc(rtext);
  if(rtext[0] != '\0'){
    CB_LISTPUSHBUF(doc->dtexts, rtext, strlen(rtext));
  } else {
    free(rtext);
  }
  free(utext);
}


/* Add a hidden sentence to a document object. */
void est_doc_add_hidden_text(ESTDOC *doc, const char *text){
  unsigned char *utext;
  char *rtext, *wp;
  int size;
  assert(doc && text);
  while(*text > '\0' && *text <= ' '){
    text++;
  }
  if(text[0] == '\0') return;
  utext = (unsigned char *)est_uconv_in(text, strlen(text), &size);
  est_normalize_text(utext, size, &size);
  rtext = est_uconv_out((char *)utext, size, NULL);
  for(wp = rtext; *wp != '\0'; wp++){
    if(*wp > 0 && *wp < ' ') *wp = ' ';
  }
  cbstrsqzspc(rtext);
  if(rtext[0] != '\0'){
    if(!doc->attrs) doc->attrs = cbmapopenex(ESTMINIBNUM);
    if(cbmapget(doc->attrs, "", 0, NULL)) cbmapputcat(doc->attrs, "", 0, " ", 1);
    cbmapputcat(doc->attrs, "", 0, rtext, -1);
  }
  free(rtext);
  free(utext);
}


/* Attach keywords to a document object. */
void est_doc_set_keywords(ESTDOC *doc, CBMAP *kwords){
  assert(doc && kwords);
  if(doc->kwords) cbmapclose(doc->kwords);
  doc->kwords = cbmapdup(kwords);
}


/* Set the substitute score of a document object. */
void est_doc_set_score(ESTDOC *doc, int score){
  char numbuf[ESTNUMBUFSIZ];
  assert(doc);
  if(!doc->attrs) doc->attrs = cbmapopenex(ESTMINIBNUM);
  if(score >= 0){
    sprintf(numbuf, "%d", score);
    cbmapput(doc->attrs, "\t", 1, numbuf, -1, TRUE);
  } else {
    cbmapout(doc->attrs, "\t", 1);
  }
}


/* Get the ID number of a document object. */
int est_doc_id(ESTDOC *doc){
  assert(doc);
  return doc->id;
}


/* Get a list of attribute names of a document object. */
CBLIST *est_doc_attr_names(ESTDOC *doc){
  CBLIST *names;
  const char *kbuf;
  int ksiz;
  assert(doc);
  if(!doc->attrs){
    CB_LISTOPEN(names);
    return names;
  }
  CB_LISTOPEN(names);
  cbmapiterinit(doc->attrs);
  while((kbuf = cbmapiternext(doc->attrs, &ksiz)) != NULL){
    if(ksiz > 0 && kbuf[0] != '\t') CB_LISTPUSH(names, kbuf, ksiz);
  }
  cblistsort(names);
  return names;
}


/* Get the value of an attribute of a document object. */
const char *est_doc_attr(ESTDOC *doc, const char *name){
  assert(doc && name);
  if(!doc->attrs || name[0] == '\0') return NULL;
  return cbmapget(doc->attrs, name, -1, NULL);
}


/* Get a list of sentences of the text of a document object. */
const CBLIST *est_doc_texts(ESTDOC *doc){
  assert(doc);
  if(!doc->dtexts) CB_LISTOPEN(doc->dtexts);
  return doc->dtexts;
}


/* Concatenate sentences of the text of a document object. */
char *est_doc_cat_texts(ESTDOC *doc){
  CBDATUM *datum;
  const char *elem;
  int i, size;
  if(!doc->dtexts) return cbmemdup("", 0);
  CB_DATUMOPEN(datum);
  for(i = 0; i < CB_LISTNUM(doc->dtexts); i++){
    elem = CB_LISTVAL2(doc->dtexts, i, size);
    if(i > 0) CB_DATUMCAT(datum, " ", 1);
    CB_DATUMCAT(datum, elem, size);
  }
  return cbdatumtomalloc(datum, NULL);
}


/* Get attached keywords of a document object. */
CBMAP *est_doc_keywords(ESTDOC *doc){
  assert(doc);
  return doc->kwords;
}


/* Get the substitute score of a document object. */
int est_doc_score(ESTDOC *doc){
  const char *vbuf;
  assert(doc);
  if(doc->attrs && (vbuf = cbmapget(doc->attrs, "\t", 1, NULL)) != NULL) return atoi(vbuf);
  return -1;
}


/* Dump draft data of a document object. */
char *est_doc_dump_draft(ESTDOC *doc){
  CBLIST *list;
  CBDATUM *datum;
  const char *kbuf, *vbuf;
  int i, ksiz, vsiz;
  assert(doc);
  CB_DATUMOPEN(datum);
  if(doc->attrs){
    list = est_doc_attr_names(doc);
    for(i = 0; i < CB_LISTNUM(list); i++){
      kbuf = CB_LISTVAL2(list, i, ksiz);
      vbuf = cbmapget(doc->attrs, kbuf, ksiz, &vsiz);
      CB_DATUMCAT(datum, kbuf, ksiz);
      CB_DATUMCAT(datum, "=", 1);
      CB_DATUMCAT(datum, vbuf, vsiz);
      CB_DATUMCAT(datum, "\n", 1);
    }
    CB_LISTCLOSE(list);
  }
  if(doc->kwords && cbmaprnum(doc->kwords) > 0){
    CB_DATUMCAT(datum, ESTDCNTLVECTOR, strlen(ESTDCNTLVECTOR));
    cbmapiterinit(doc->kwords);
    while((kbuf = cbmapiternext(doc->kwords, &ksiz)) != NULL){
      CB_MAPITERVAL(vbuf, kbuf, vsiz);
      CB_DATUMCAT(datum, "\t", 1);
      CB_DATUMCAT(datum, kbuf, ksiz);
      CB_DATUMCAT(datum, "\t", 1);
      CB_DATUMCAT(datum, vbuf, vsiz);
    }
    CB_DATUMCAT(datum, "\n", 1);
  }
  if(doc->attrs && (vbuf = cbmapget(doc->attrs, "\t", 1, &vsiz)) != NULL){
    CB_DATUMCAT(datum, ESTDCNTLSCORE, strlen(ESTDCNTLSCORE));
    CB_DATUMCAT(datum, "\t", 1);
    CB_DATUMCAT(datum, vbuf, vsiz);
    CB_DATUMCAT(datum, "\n", 1);
  }
  CB_DATUMCAT(datum, "\n", 1);
  if(doc->dtexts){
    for(i = 0; i < CB_LISTNUM(doc->dtexts); i++){
      kbuf = CB_LISTVAL2(doc->dtexts, i, ksiz);
      CB_DATUMCAT(datum, kbuf, ksiz);
      CB_DATUMCAT(datum, "\n", 1);
    }
  }
  if(doc->attrs && (vbuf = cbmapget(doc->attrs, "", 0, &vsiz)) != NULL){
    CB_DATUMCAT(datum, "\t", 1);
    CB_DATUMCAT(datum, vbuf, vsiz);
    CB_DATUMCAT(datum, "\n", 1);
  }
  return cbdatumtomalloc(datum, NULL);
}


/* Make a snippet of the body text of a document object. */
char *est_doc_make_snippet(ESTDOC *doc, const CBLIST *words, int wwidth, int hwidth, int awidth){
  CBDATUM *sbuf;
  const char *text;
  char *snippet;
  int i, size;
  assert(doc && words && wwidth >= 0 && hwidth >= 0 && awidth >= 0);
  if(!doc->dtexts) CB_LISTOPEN(doc->dtexts);
  CB_DATUMOPEN(sbuf);
  for(i = 0; i < CB_LISTNUM(doc->dtexts); i++){
    text = CB_LISTVAL2(doc->dtexts, i, size);
    if(i > 0) CB_DATUMCAT(sbuf, " ", 1);
    CB_DATUMCAT(sbuf, text, size);
  }
  snippet = est_make_snippet(CB_DATUMPTR(sbuf), CB_DATUMSIZE(sbuf),
                             words, wwidth, hwidth, awidth);
  CB_DATUMCLOSE(sbuf);
  return snippet;
}



/*************************************************************************************************
 * API for search conditions
 *************************************************************************************************/


/* Create a condition object. */
ESTCOND *est_cond_new(void){
  ESTCOND *cond;
  CB_MALLOC(cond, sizeof(ESTCOND));
  cond->phrase = NULL;
  cond->gstep = 2;
  cond->tfidf = TRUE;
  cond->pmode = ESTPMUSUAL;
  cond->cbxpn = NULL;
  cond->attrs = NULL;
  cond->order = NULL;
  cond->max = -1;
  cond->skip = 0;
  cond->auxmin = ESTAUXMIN;
  cond->auxwords = NULL;
  cond->scfb = FALSE;
  cond->scores = NULL;
  cond->snum = 0;
  cond->nscores = NULL;
  cond->nsnum = -1;
  cond->opts = 0;
  cond->ecllim = -1.0;
  cond->shadows = NULL;
  cond->distinct = NULL;
  cond->mask = 0;
  return cond;
}


/* Destroy a condition object. */
void est_cond_delete(ESTCOND *cond){
  assert(cond);
  if(cond->distinct) free(cond->distinct);
  if(cond->shadows) cbmapclose(cond->shadows);
  if(cond->auxwords) cbmapclose(cond->auxwords);
  if(cond->scores) free(cond->scores);
  if(cond->order) free(cond->order);
  if(cond->attrs) CB_LISTCLOSE(cond->attrs);
  if(cond->phrase) free(cond->phrase);
  free(cond);
}


/* Set a search phrase to a condition object. */
void est_cond_set_phrase(ESTCOND *cond, const char *phrase){
  assert(cond && phrase);
  if(cond->phrase) free(cond->phrase);
  while(*phrase > '\0' && *phrase <= ' '){
    phrase++;
  }
  cond->phrase = cbmemdup(phrase, -1);
}


/* Add a condition of an attribute fo a condition object. */
void est_cond_add_attr(ESTCOND *cond, const char *expr){
  assert(cond && expr);
  while(*expr > '\0' && *expr <= ' '){
    expr++;
  }
  if(*expr == '\0') return;
  if(!cond->attrs) CB_LISTOPEN(cond->attrs);
  CB_LISTPUSH(cond->attrs, expr, strlen(expr));
}


/* Set the order of a condition object. */
void est_cond_set_order(ESTCOND *cond, const char *expr){
  assert(cond && expr);
  while(*expr > '\0' && *expr <= ' '){
    expr++;
  }
  if(*expr == '\0') return;
  if(cond->order) free(cond->order);
  cond->order = cbmemdup(expr, -1);
}


/* Set the maximum number of retrieval of a condition object. */
void est_cond_set_max(ESTCOND *cond, int max){
  assert(cond && max >= 0);
  cond->max = max;
}


/* Set the number of skipped documents of a condition object. */
void est_cond_set_skip(ESTCOND *cond, int skip){
  assert(cond && skip >= 0);
  cond->skip = skip;
}


/* Set options of retrieval of a condition object. */
void est_cond_set_options(ESTCOND *cond, int options){
  assert(cond);
  if(options & ESTCONDSURE) cond->gstep = 1;
  if(options & ESTCONDUSUAL) cond->gstep = 2;
  if(options & ESTCONDFAST) cond->gstep = 3;
  if(options & ESTCONDAGITO) cond->gstep = 4;
  if(options & ESTCONDNOIDF) cond->tfidf = FALSE;
  if(options & ESTCONDSIMPLE) cond->pmode = ESTPMSIMPLE;
  if(options & ESTCONDROUGH) cond->pmode = ESTPMROUGH;
  if(options & ESTCONDUNION) cond->pmode = ESTPMUNION;
  if(options & ESTCONDISECT) cond->pmode = ESTPMISECT;
  if(options & ESTCONDSCFB) cond->scfb = TRUE;
  cond->opts |= options;
}


/* Set permission to adopt result of the auxiliary index. */
void est_cond_set_auxiliary(ESTCOND *cond, int min){
  assert(cond);
  cond->auxmin = min;
}


/* Set the upper limit of similarity for document eclipse. */
void est_cond_set_eclipse(ESTCOND *cond, double limit){
  assert(cond);
  if(limit > 0.0) cond->ecllim = limit;
}


/* Set the attribute distinction filter. */
void est_cond_set_distinct(ESTCOND *cond, const char *name){
  assert(cond && name);
  while(*name > '\0' && *name <= ' '){
    name++;
  }
  if(*name == '\0') return;
  if(cond->distinct) free(cond->distinct);
  cond->distinct = cbmemdup(name, -1);
}


/* Set the mask of targets of meta search. */
void est_cond_set_mask(ESTCOND *cond, int mask){
  assert(cond);
  cond->mask = mask & INT_MAX;
}



/*************************************************************************************************
 * API for database
 *************************************************************************************************/


/* Inode map for duplication check. */
CBMAP *est_inodes = NULL;


/* Get the string of an error code. */
const char *est_err_msg(int ecode){
  switch(ecode){
  case ESTENOERR: return "no error";
  case ESTEINVAL: return "invalid argument";
  case ESTEACCES: return "access forbidden";
  case ESTELOCK: return "lock failure";
  case ESTEDB: return "database problem";
  case ESTEIO: return "I/O problem";
  case ESTENOITEM: return "no such item";
  default: break;
  }
  return "miscellaneous";
}


/* Open a database. */
ESTDB *est_db_open(const char *name, int omode, int *ecp){
  ESTDB *db;
  DEPOT *metadb;
  ESTIDX *idxdb;
  CURIA *attrdb, *textdb, *kwddb;
  VILLA *fwmdb, *auxdb, *xfmdb, *listdb;
  CBMAP *aidxs;
  CBLIST *list;
  ESTATTRIDX attridx;
  void *aidxdb;
  const char *elem;
  char path[ESTPATHBUFSIZ], vbuf[ESTNUMBUFSIZ], *dec;
  int i, inode, domode, comode, vomode, flags, idxnum, dseq, dnum;
  int amode, zmode, smode, vsiz, type, crdnum;
  double bdiam, ddiam;
  assert(name && ecp);
  if(!est_inodes){
    est_inodes = cbmapopenex(ESTMINIBNUM);
    cbglobalgc(est_inodes, est_inodes_delete);
  }
  est_set_ecode(ecp, ESTENOERR, __LINE__);
  if((omode & ESTDBWRITER) && (omode & ESTDBCREAT) && !est_mkdir(name)){
    switch(errno){
    case EACCES:
      est_set_ecode(ecp, ESTEACCES, __LINE__);
      return NULL;
    case EEXIST:
      break;
    default:
      est_set_ecode(ecp, ESTEIO, __LINE__);
      return NULL;
    }
  }
  if((inode = est_inode(name)) < 1){
    est_set_ecode(ecp, ESTEIO, __LINE__);
    return NULL;
  }
  if(cbmapget(est_inodes, (char *)&inode, sizeof(int), NULL) && !(omode & ESTDBNOLCK)){
    est_set_ecode(ecp, ESTEACCES, __LINE__);
    return NULL;
  }
  domode = DP_OREADER;
  comode = CR_OREADER;
  vomode = VL_OREADER;
  if(omode & ESTDBWRITER){
    domode = DP_OWRITER;
    comode = CR_OWRITER;
    vomode = VL_OWRITER;
    if(ESTUSEBZIP){
      vomode |= VL_OXCOMP;
    } else if(ESTUSELZO){
      vomode |= VL_OYCOMP;
    } else if(ESTUSEZLIB){
      vomode |= VL_OZCOMP;
    }
    if(omode & ESTDBCREAT){
      domode |= DP_OCREAT;
      comode |= CR_OCREAT;
      vomode |= VL_OCREAT;
    }
    if(omode & ESTDBTRUNC){
      domode |= DP_OTRUNC;
      comode |= CR_OTRUNC;
      vomode |= VL_OTRUNC;
    }
  }
  if(omode & ESTDBNOLCK){
    domode |= DP_ONOLCK;
    comode |= CR_ONOLCK;
    vomode |= VL_ONOLCK;
  }
  if(omode & ESTDBLCKNB){
    domode |= DP_OLCKNB;
    comode |= CR_OLCKNB;
    vomode |= VL_OLCKNB;
  }
  flags = 0;
  idxnum = 0;
  dseq = 0;
  dnum = 0;
  amode = 0;
  zmode = 0;
  smode = 0;
  if(omode & ESTDBSMALL){
    bdiam = ESTDBSBRAT;
    ddiam = ESTDBSDRAT;
  } else if(omode & ESTDBLARGE){
    bdiam = ESTDBLBRAT;
    ddiam = ESTDBLDRAT;
  } else if(omode & ESTDBHUGE){
    bdiam = ESTDBHBRAT;
    ddiam = ESTDBHDRAT;
  } else if(omode & ESTDBHUGE2){
    bdiam = ESTDBHBRAT * ESTDBH2RAT;
    ddiam = ESTDBHDRAT * ESTDBH2RAT;
  } else if(omode & ESTDBHUGE3){
    bdiam = ESTDBHBRAT * ESTDBH3RAT;
    ddiam = ESTDBHDRAT * ESTDBH3RAT;
  } else {
    bdiam = 1.0;
    ddiam = 1.0;
  }
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTMETADBNAME);
  if((metadb = dpopen(path, domode, ESTMINIBNUM)) != NULL){
    flags = dpgetflags(metadb);
    if(dprnum(metadb) < 1){
      if(omode & ESTDBPERFNG){
        flags |= ESTDFPERFNG;
      } else if(omode & ESTDBCHRCAT){
        flags |= ESTDFCHRCAT;
      }
      if(ESTUSEBZIP){
        flags |= ESTDFBZIP;
      } else if(ESTUSELZO){
        flags |= ESTDFLZO;
      } else if(ESTUSEZLIB){
        flags |= ESTDFZLIB;
      }
      if(omode & ESTDBSCVOID){
        flags |= ESTDFSCVOID;
      } else if(omode & ESTDBSCINT){
        flags |= ESTDFSCINT;
      } else if(omode & ESTDBSCASIS){
        flags |= ESTDFSCASIS;
      }
      dpsetflags(metadb, flags);
    }
    if((vsiz = dpgetwb(metadb, ESTKEYIDXNUM, -1, 0, ESTNUMBUFSIZ - 1, vbuf)) > 0){
      vbuf[vsiz] = '\0';
      idxnum = atoi(vbuf);
    }
    if((vsiz = dpgetwb(metadb, ESTKEYDSEQ, -1, 0, ESTNUMBUFSIZ - 1, vbuf)) > 0){
      vbuf[vsiz] = '\0';
      dseq = atoi(vbuf);
    }
    if((vsiz = dpgetwb(metadb, ESTKEYDNUM, -1, 0, ESTNUMBUFSIZ - 1, vbuf)) > 0){
      vbuf[vsiz] = '\0';
      dnum = atoi(vbuf);
    }
    if(flags & ESTDFPERFNG){
      amode = ESTDFPERFNG;
    } else if(flags & ESTDFCHRCAT){
      amode = ESTDFCHRCAT;
    }
    if(flags & ESTDFZLIB){
      zmode = ESTDFZLIB;
    } else if(flags & ESTDFLZO){
      zmode = ESTDFLZO;
    } else if(flags & ESTDFBZIP){
      zmode = ESTDFBZIP;
    }
    if(flags & ESTDFSCVOID){
      smode = ESTDFSCVOID;
    } else if(flags & ESTDFSCINT){
      smode = ESTDFSCINT;
    } else if(flags & ESTDFSCASIS){
      smode = ESTDFSCASIS;
    }
  } else {
    est_set_ecode(ecp, dpecode == DP_ELOCK ? ESTELOCK : ESTEDB, __LINE__);
    return NULL;
  }
  if(idxnum < 1) idxnum = 1;
  if(dseq < 0) dseq = 0;
  if(dnum < 0) dnum = 0;
  crdnum = vlcrdnum;
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTIDXDBNAME);
  idxdb = est_idx_open(path, vomode, idxnum);
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTFWMDBNAME);
  vlcrdnum = ESTVLCRDNUM;
  fwmdb = vlopen(path, vomode, VL_CMPLEX);
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTAUXDBNAME);
  vlcrdnum = ESTVLCRDNAUX;
  auxdb = vlopen(path, vomode, VL_CMPLEX);
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTXFMDBNAME);
  vlcrdnum = ESTVLCRDNUM;
  xfmdb = vlopen(path, vomode, VL_CMPLEX);
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTATTRDBNAME);
  attrdb = cropen(path, comode, ESTATTRDBBNUM * bdiam, ESTATTRDBDNUM * ddiam);
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTTEXTDBNAME);
  textdb = cropen(path, comode, ESTTEXTDBBNUM * bdiam, ESTTEXTDBDNUM * ddiam);
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTKWDDBNAME);
  kwddb = cropen(path, comode,  ESTKWDDBBNUM * bdiam, ESTKWDDBDNUM * ddiam);
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTLISTDBNAME);
  vlcrdnum = ESTVLCRDNUM;
  listdb = vlopen(path, vomode, VL_CMPLEX);
  vlcrdnum = crdnum;
  if(!idxdb || !fwmdb || !auxdb || !xfmdb || !attrdb ||!textdb || !kwddb || !listdb){
    if(listdb) vlclose(listdb);
    if(kwddb) crclose(kwddb);
    if(textdb) crclose(textdb);
    if(attrdb) crclose(attrdb);
    if(xfmdb) vlclose(xfmdb);
    if(auxdb) vlclose(auxdb);
    if(fwmdb) vlclose(fwmdb);
    if(idxdb) est_idx_close(idxdb);
    dpclose(metadb);
    est_set_ecode(ecp, ESTEDB, __LINE__);
    return NULL;
  }
  if(omode & ESTDBWRITER){
    est_idx_set_tuning(idxdb, amode == ESTDFPERFNG ? ESTIDXDBLRMA : ESTIDXDBLRM, ESTIDXDBNIM,
                       ESTIDXDBLCN, ESTIDXDBNCN, ESTIDXDBFBP);
    est_idx_set_current(idxdb);
    vlsettuning(fwmdb, ESTFWMDBLRM, ESTFWMDBNIM, ESTFWMDBLCN, ESTFWMDBNCN);
    vlsetfbpsiz(fwmdb, ESTFWMDBFBP);
    vlsettuning(auxdb, ESTAUXDBLRM, ESTAUXDBNIM, ESTAUXDBLCN, ESTAUXDBNCN);
    vlsetfbpsiz(auxdb, ESTAUXDBFBP);
    vlsettuning(xfmdb, ESTXFMDBLRM, ESTXFMDBNIM, ESTXFMDBLCN, ESTXFMDBNCN);
    vlsetfbpsiz(xfmdb, ESTXFMDBFBP);
    crsetalign(attrdb, ESTATTRDBALN);
    crsetfbpsiz(attrdb, ESTATTRDBFBP);
    crsetalign(textdb, ESTTEXTDBALN);
    crsetfbpsiz(textdb, ESTTEXTDBFBP);
    crsetalign(kwddb, ESTKWDDBALN);
    crsetfbpsiz(kwddb, ESTKWDDBFBP);
    vlsettuning(listdb, ESTLISTDBLRM, ESTLISTDBNIM, ESTLISTDBLCN, ESTLISTDBNCN);
    vlsetfbpsiz(listdb, ESTLISTDBFBP);
  } else {
    est_idx_set_tuning(idxdb, -1, -1,
                       amode == ESTDFPERFNG ? ESTIDXDBRLCNA : ESTIDXDBRLCN, ESTIDXDBRNCN, -1);
    vlsettuning(fwmdb, -1, -1, ESTFWMDBLCN, ESTFWMDBNCN);
    vlsettuning(auxdb, -1, -1, ESTAUXDBRLCN, ESTAUXDBRNCN);
    vlsettuning(xfmdb, -1, -1, ESTXFMDBLCN, ESTXFMDBNCN);
    vlsettuning(listdb, -1, -1, ESTLISTDBLCN, ESTLISTDBNCN);
  }
  if((omode & ESTDBWRITER) && (omode & ESTDBTRUNC) && (list = cbdirlist(name)) != NULL){
    for(i = 0; i < CB_LISTNUM(list); i++){
      elem = CB_LISTVAL(list, i);
      if(cbstrfwmatch(elem, ESTAISEQPREF) || cbstrfwmatch(elem, ESTAISTRPREF) ||
         cbstrfwmatch(elem, ESTAINUMPREF)){
        sprintf(path, "%s%c%s", name, ESTPATHCHR, elem);
        if(unlink(path) == -1) est_rmdir_rec(path);
      }
    }
    CB_LISTCLOSE(list);
  }
  aidxs = cbmapopenex(ESTMINIBNUM);
  if((list = cbdirlist(name)) != NULL){
    for(i = 0; i < CB_LISTNUM(list); i++){
      elem = CB_LISTVAL(list, i);
      dec = NULL;
      type = -1;
      if(cbstrfwmatch(elem, ESTAISEQPREF)){
        dec = est_hex_decode(elem + strlen(ESTAISEQPREF));
        type = ESTIDXATTRSEQ;
      } else if(cbstrfwmatch(elem, ESTAISTRPREF)){
        dec = est_hex_decode(elem + strlen(ESTAISTRPREF));
        type = ESTIDXATTRSTR;
      } else if(cbstrfwmatch(elem, ESTAINUMPREF)){
        dec = est_hex_decode(elem + strlen(ESTAINUMPREF));
        type = ESTIDXATTRNUM;
      }
      if(dec){
        sprintf(path, "%s%c%s", name, ESTPATHCHR, elem);
        switch(type){
        case ESTIDXATTRSTR:
          if((aidxdb = vlopen(path, vomode, VL_CMPLEX)) != NULL){
            vlsettuning(aidxdb, ESTAIDXLRM, ESTAIDXNIM, ESTAIDXLCN, ESTAIDXNCN);
            vlsetfbpsiz(aidxdb, ESTAIDXVLFBP);
            attridx.db = aidxdb;
            attridx.type = type;
            cbmapput(aidxs, dec, -1, (char *)&attridx, sizeof(ESTATTRIDX), FALSE);
          }
          break;
        case ESTIDXATTRNUM:
          if((aidxdb = vlopen(path, vomode, est_aidx_numcmp)) != NULL){
            vlsettuning(aidxdb, ESTAIDXLRM, ESTAIDXNIM, ESTAIDXLCN, ESTAIDXNCN);
            vlsetfbpsiz(aidxdb, ESTAIDXVLFBP);
            attridx.db = aidxdb;
            attridx.type = type;
            cbmapput(aidxs, dec, -1, (char *)&attridx, sizeof(ESTATTRIDX), FALSE);
          }
          break;
        default:
          if((aidxdb = dpopen(path, domode, crbnum(attrdb) / ESTAIBDIAM)) != NULL){
            dpsetfbpsiz(aidxdb, ESTAIDXDPFBP);
            attridx.db = aidxdb;
            attridx.type = type;
            cbmapput(aidxs, dec, -1, (char *)&attridx, sizeof(ESTATTRIDX), FALSE);
          }
          break;
        }
        free(dec);
      }
    }
    CB_LISTCLOSE(list);
  }
  CB_MALLOC(db, sizeof(ESTDB));
  db->name = cbmemdup(name, -1);
  db->inode = inode;
  db->metadb = metadb;
  db->idxdb = idxdb;
  db->fwmdb = fwmdb;
  db->auxdb = auxdb;
  db->xfmdb = xfmdb;
  db->attrdb = attrdb;
  db->textdb = textdb;
  db->kwddb = kwddb;
  db->listdb = listdb;
  db->aidxs = aidxs;
  CB_LISTOPEN(db->pdocs);
  db->puris = NULL;
  est_set_ecode(&(db->ecode), ESTENOERR, __LINE__);
  db->fatal = FALSE;
  db->dseq = dseq;
  db->dnum = dnum;
  db->amode = amode;
  db->zmode = zmode;
  db->smode = smode;
  if(omode & ESTDBWRITER){
    db->idxcc = cbmapopenex(ESTIDXCCBNUM);
    db->auxcc = cbmapopenex(ESTAUXCCBNUM);
    db->icsiz = 0;
    db->icmax = ESTIDXCCMAX;
    db->outcc = cbmapopenex(ESTOUTCCBNUM);
  } else {
    db->idxcc = cbmapopenex(1);
    db->auxcc = cbmapopenex(1);
    db->icsiz = 0;
    db->icmax = 0;
    db->outcc = cbmapopenex(1);
  }
  db->keycc = cbmapopenex(ESTKEYCCMNUM + 1);
  db->kcmnum = ESTKEYCCMNUM;
  db->attrcc = cbmapopenex(ESTATTRCCMNUM + 1);
  db->acmnum = ESTATTRCCMNUM;
  db->textcc = cbmapopenex(ESTTEXTCCMNUM + 1);
  db->tcmnum = ESTTEXTCCMNUM;
  db->veccc = cbmapopenex(ESTATTRCCMNUM / 2 + 1);
  db->vcmnum = ESTATTRCCMNUM / 2;
  db->rescc = cbmapopenex(ESTRESCCMNUM * 2 + 1);
  db->rcmnum = ESTRESCCMNUM;
  db->spacc = NULL;
  db->scmnum = 0;
  db->scname = NULL;
  db->infocb = NULL;
  db->infoop = NULL;
  db->dfdb = NULL;
  db->metacc = NULL;
  db->wildmax = ESTWILDMAX;
  db->flsflag = FALSE;
  db->intflag = FALSE;
  cbmapput(est_inodes, (char *)&inode, sizeof(int), (char *)&db, sizeof(ESTDB *), FALSE);
  return db;
}


/* Close a database. */
int est_db_close(ESTDB *db, int *ecp){
  ESTATTRIDX *attridx;
  const char *kbuf;
  int err;
  assert(db && ecp);
  est_set_ecode(ecp, ESTENOERR, __LINE__);
  err = FALSE;
  cbmapout(est_inodes, (char *)&(db->inode), sizeof(int));
  if(dpwritable(db->metadb)){
    if(!est_db_flush(db, -1)) err = TRUE;
    if(!est_db_write_meta(db)) err = TRUE;
  }
  est_db_inform(db, "closing");
  if(db->metacc) cbmapclose(db->metacc);
  if(db->spacc){
    free(db->scname);
    cbmapclose(db->spacc);
  }
  cbmapclose(db->rescc);
  cbmapclose(db->veccc);
  cbmapclose(db->textcc);
  cbmapclose(db->attrcc);
  cbmapclose(db->keycc);
  cbmapclose(db->outcc);
  cbmapclose(db->auxcc);
  cbmapclose(db->idxcc);
  if(db->puris) cbmapclose(db->puris);
  CB_LISTCLOSE(db->pdocs);
  cbmapiterinit(db->aidxs);
  while((kbuf = cbmapiternext(db->aidxs, NULL)) != NULL){
    attridx = (ESTATTRIDX *)cbmapiterval(kbuf, NULL);
    switch(attridx->type){
    case ESTIDXATTRSTR:
    case ESTIDXATTRNUM:
      if(!vlclose(attridx->db)) err = TRUE;
      break;
    default:
      if(!dpclose(attridx->db)) err = TRUE;
      break;
    }
  }
  cbmapclose(db->aidxs);
  if(!vlclose(db->listdb)) err = TRUE;
  if(!crclose(db->kwddb)) err = TRUE;
  if(!crclose(db->textdb)) err = TRUE;
  if(!crclose(db->attrdb)) err = TRUE;
  if(!vlclose(db->xfmdb)) err = TRUE;
  if(!vlclose(db->auxdb)) err = TRUE;
  if(!vlclose(db->fwmdb)) err = TRUE;
  if(!est_idx_close(db->idxdb)) err = TRUE;
  if(!dpclose(db->metadb)) err = TRUE;
  free(db->name);
  if(db->fatal){
    est_set_ecode(ecp, db->ecode, __LINE__);
    err = TRUE;
  } else if(err){
    est_set_ecode(ecp, ESTEDB, __LINE__);
  }
  free(db);
  return err ? FALSE : TRUE;
}


/* Get the last happended error code of a database. */
int est_db_error(ESTDB *db){
  assert(db);
  return db->ecode;
}


/* Check whether a database has a fatal error. */
int est_db_fatal(ESTDB *db){
  assert(db);
  return db->fatal;
}


/* Add an index for narrowing or sorting with document attributes. */
int est_db_add_attr_index(ESTDB *db, const char *name, int type){
  ESTATTRIDX attridx;
  ESTSCORE *scores;
  void *aidxdb;
  char path[ESTPATHBUFSIZ], *enc, *vbuf;
  int i, domode, vomode, crdnum, err, snum;
  assert(db && name);
  if(!dpwritable(db->metadb)){
    est_set_ecode(&(db->ecode), ESTEACCES, __LINE__);
    return FALSE;
  }
  if(cbmapget(db->aidxs, name, -1, NULL)){
    est_set_ecode(&(db->ecode), ESTEMISC, __LINE__);
    return FALSE;
  }
  enc = est_hex_encode(name);
  switch(type){
  case ESTIDXATTRSEQ:
    sprintf(path, "%s%c%s%s", db->name, ESTPATHCHR, ESTAISEQPREF, enc);
    break;
  case ESTIDXATTRSTR:
    sprintf(path, "%s%c%s%s", db->name, ESTPATHCHR, ESTAISTRPREF, enc);
    break;
  case ESTIDXATTRNUM:
    sprintf(path, "%s%c%s%s", db->name, ESTPATHCHR, ESTAINUMPREF, enc);
    break;
  default:
    free(enc);
    est_set_ecode(&(db->ecode), ESTEINVAL, __LINE__);
    return FALSE;
  }
  free(enc);
  domode = DP_OWRITER | DP_OCREAT | DP_OTRUNC;
  vomode = VL_OWRITER | VL_OCREAT | VL_OTRUNC;
  if(ESTUSEBZIP){
    vomode |= VL_OXCOMP;
  } else if(ESTUSELZO){
    vomode |= VL_OYCOMP;
  } else if(ESTUSEZLIB){
    vomode |= VL_OZCOMP;
  }
  err = FALSE;
  crdnum = vlcrdnum;
  switch(type){
  case ESTIDXATTRSTR:
    vlcrdnum = ESTVLCRDNUM;
    if(!(aidxdb = vlopen(path, vomode, VL_CMPLEX))){
      est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
      vlcrdnum = crdnum;
      return FALSE;
    }
    vlsettuning(aidxdb, ESTAIDXLRM, ESTAIDXNIM, ESTAIDXLCN, ESTAIDXNCN);
    vlsetfbpsiz(aidxdb, ESTAIDXVLFBP);
    if(est_db_doc_num(db) > 0){
      scores = est_search_uvset(db, &snum, NULL, TRUE);
      for(i = 0; i < snum; i++){
        if((vbuf = est_db_get_doc_attr(db, scores[i].id, name)) != NULL){
          if(!est_aidx_attr_put(aidxdb, scores[i].id, vbuf, strlen(vbuf))){
            est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
            db->fatal = TRUE;
            err = TRUE;
          }
          free(vbuf);
        }
        if(i % (ESTCCCBFREQ / 10) == 0) est_db_inform(db, "entering existing attributes");
      }
      free(scores);
    }
    break;
  case ESTIDXATTRNUM:
    vlcrdnum = ESTVLCRDNUM;
    if(!(aidxdb = vlopen(path, vomode, est_aidx_numcmp))){
      est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
      vlcrdnum = crdnum;
      return FALSE;
    }
    vlsettuning(aidxdb, ESTAIDXLRM, ESTAIDXNIM, ESTAIDXLCN, ESTAIDXNCN);
    vlsetfbpsiz(aidxdb, ESTAIDXVLFBP);
    if(est_db_doc_num(db) > 0){
      scores = est_search_uvset(db, &snum, NULL, TRUE);
      for(i = 0; i < snum; i++){
        if((vbuf = est_db_get_doc_attr(db, scores[i].id, name)) != NULL){
          if(!est_aidx_attr_put(aidxdb, scores[i].id, vbuf, strlen(vbuf))){
            est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
            db->fatal = TRUE;
            err = TRUE;
          }
          free(vbuf);
        }
        if(i % (ESTCCCBFREQ / 10) == 0) est_db_inform(db, "entering existing attributes");
      }
      free(scores);
    }
    break;
  default:
    if(!(aidxdb = dpopen(path, domode, crbnum(db->attrdb) * ESTAIBDIAM))){
      est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
      vlcrdnum = crdnum;
      return FALSE;
    }
    dpsetfbpsiz(aidxdb, ESTAIDXDPFBP);
    if(est_db_doc_num(db) > 0){
      scores = est_search_uvset(db, &snum, NULL, TRUE);
      for(i = 0; i < snum; i++){
        if((vbuf = est_db_get_doc_attr(db, scores[i].id, name)) != NULL){
          if(!est_aidx_seq_put(aidxdb, scores[i].id, vbuf, strlen(vbuf))){
            est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
            db->fatal = TRUE;
            err = TRUE;
          }
          free(vbuf);
        }
        if(i % (ESTCCCBFREQ / 10) == 0) est_db_inform(db, "entering existing attributes");
      }
      free(scores);
    }
    break;
  }
  vlcrdnum = crdnum;
  attridx.db = aidxdb;
  attridx.type = type;
  cbmapput(db->aidxs, name, -1, (char *)&attridx, sizeof(ESTATTRIDX), FALSE);
  return err ? FALSE : TRUE;
}


/* Flush index words in the cache of a database. */
int est_db_flush(ESTDB *db, int max){
  ESTATTRIDX *attridx;
  CBMAP *ids;
  CBLIST *keys;
  CBDATUM *nval;
  const char *kbuf, *vbuf, *rp, *pv, *ep;
  char *tbuf, *wp, numbuf[ESTNUMBUFSIZ];
  int i, j, inc, err, ksiz, vsiz, rnum, len, id, sum, cid, vnum, lid, dnum, tsiz, vstep;
  assert(db);
  if(!dpwritable(db->metadb)){
    est_set_ecode(&(db->ecode), ESTEACCES, __LINE__);
    return FALSE;
  }
  if(max < 1 || max >= INT_MAX){
    if(!est_db_write_meta(db)) err = TRUE;
    if(!dpmemflush(db->metadb)) err = TRUE;
    if(!crmemflush(db->attrdb)) err = TRUE;
    if(!crmemflush(db->textdb)) err = TRUE;
    if(!crmemflush(db->kwddb)) err = TRUE;
    if(!vlmemflush(db->listdb)) err = TRUE;
    cbmapiterinit(db->aidxs);
    while((kbuf = cbmapiternext(db->aidxs, NULL)) != NULL){
      attridx = (ESTATTRIDX *)cbmapiterval(kbuf, NULL);
      switch(attridx->type){
      case ESTIDXATTRSTR:
      case ESTIDXATTRNUM:
        if(!vlmemflush(attridx->db)) err = TRUE;
        break;
      default:
        if(!dpmemflush(attridx->db)) err = TRUE;
        break;
      }
    }
  }
  if(cbmaprnum(db->idxcc) < 1 && cbmaprnum(db->auxcc) < 1 && cbmaprnum(db->outcc) < 1)
    return TRUE;
  db->flsflag = TRUE;
  db->intflag = FALSE;
  inc = est_db_used_cache_size(db) > db->icmax;
  err = FALSE;
  CB_LISTOPEN(keys);
  cbmapiterinit(db->idxcc);
  while((kbuf = cbmapiternext(db->idxcc, &ksiz)) != NULL){
    CB_LISTPUSH(keys, kbuf, ksiz);
  }
  rnum = CB_LISTNUM(keys);
  cblistsort(keys);
  if(max > 0){
    while(CB_LISTNUM(keys) > max){
      CB_LISTDROP(keys);
    }
  }
  for(i = 0; i < CB_LISTNUM(keys); i++){
    kbuf = CB_LISTVAL2(keys, i, ksiz);
    vbuf = cbmapget(db->idxcc, kbuf, ksiz, &vsiz);
    if(!est_idx_add(db->idxdb, kbuf, ksiz, vbuf, vsiz, db->smode) ||
       (!vlput(db->fwmdb, kbuf, ksiz, "", 0, VL_DKEEP) && dpecode != DP_EKEEP)){
      err = TRUE;
      break;
    }
    cbmapout(db->idxcc, kbuf, ksiz);
    db->icsiz -= vsiz;
    if(i % ESTCCCBFREQ == 0){
      est_db_inform(db, "flushing index words");
      if(est_idx_size_current(db->idxdb) >= ESTIDXDBMAX){
        est_db_inform(db, "adding a new database file");
        est_idx_increment(db->idxdb);
        inc = FALSE;
      }
    }
    if(max > 0 && db->intflag && i > 0 && i % ESTCCIRSLOT == 0) break;
  }
  CB_LISTCLOSE(keys);
  if(cbmaprnum(db->idxcc) < 1){
    cbmapclose(db->idxcc);
    db->idxcc = cbmapopenex(rnum > ESTIDXCCBNUM ? rnum * 1.5 : ESTIDXCCBNUM);
    if(cbmaprnum(db->auxcc) > 0){
      CB_LISTOPEN(keys);
      cbmapiterinit(db->auxcc);
      while((kbuf = cbmapiternext(db->auxcc, &ksiz)) != NULL){
        CB_LISTPUSH(keys, kbuf, ksiz);
      }
      cblistsort(keys);
      for(i = 0; i < CB_LISTNUM(keys); i++){
        kbuf = CB_LISTVAL2(keys, i, ksiz);
        vbuf = cbmapget(db->auxcc, kbuf, ksiz, &vsiz);
        if(!vlput(db->auxdb, kbuf, ksiz, vbuf, vsiz, VL_DCAT)){
          err = TRUE;
          break;
        }
        len = sprintf(numbuf, "%d", vlvsiz(db->auxdb, kbuf, ksiz) / (int)(sizeof(int) * 2));
        if(!vlput(db->xfmdb, kbuf, ksiz, numbuf, len, VL_DOVER)){
          err = TRUE;
          break;
        }
        cbmapout(db->auxcc, kbuf, ksiz);
        db->icsiz -= vsiz;
        if(i % ESTCCCBFREQ == 0) est_db_inform(db, "flushing auxiliary keywords");
        if(max > 0 && db->intflag && i > 0 && i % ESTCCIRSLOT == 0) break;
      }
      CB_LISTCLOSE(keys);
      if(cbmaprnum(db->auxcc) < 1){
        cbmapclose(db->auxcc);
        db->auxcc = cbmapopenex(ESTAUXCCBNUM);
      }
    }
  }
  if(max < 1 && cbmaprnum(db->outcc) > 0){
    ids = cbmapopen();
    CB_LISTOPEN(keys);
    cbmapiterinit(db->outcc);
    while((kbuf = cbmapiternext(db->outcc, &ksiz)) != NULL){
      if(*kbuf == '\t'){
        id = atoi(kbuf + 1);
        cbmapput(ids, (char *)&id, sizeof(int), "", 0, FALSE);
      } else {
        CB_LISTPUSH(keys, kbuf, ksiz);
      }
    }
    cblistsort(keys);
    dnum = est_idx_dnum(db->idxdb);
    for(i = 0; i < CB_LISTNUM(keys); i++){
      kbuf = CB_LISTVAL2(keys, i, ksiz);
      if(kbuf[0] == ' '){
        if((tbuf = vlget(db->auxdb, kbuf + 1, ksiz - 1, &tsiz)) != NULL){
          rp = tbuf;
          wp = tbuf;
          ep = tbuf + tsiz;
          while(rp < ep){
            if(!cbmapget(ids, rp, sizeof(int), NULL)){
              memmove(wp, rp, sizeof(int) * 2);
              wp += sizeof(int) * 2;
            }
            rp += sizeof(int) * 2;
          }
          if(wp > tbuf){
            if(!vlput(db->auxdb, kbuf + 1, ksiz - 1, tbuf, wp - tbuf, VL_DOVER)) err = TRUE;
            len = sprintf(numbuf, "%d", (int)((wp - tbuf) / (sizeof(int) * 2)));
            if(!vlput(db->xfmdb, kbuf + 1, ksiz - 1, numbuf, len, VL_DOVER)) err = TRUE;
          } else {
            if(!vlout(db->auxdb, kbuf + 1, ksiz - 1)) err = TRUE;
            if(!vlout(db->xfmdb, kbuf + 1, ksiz - 1) && dpecode != DP_ENOITEM) err = TRUE;
          }
          free(tbuf);
        }
      } else {
        sum = 0;
        for(j = 0; j < dnum; j++){
          if((vbuf = est_idx_get_one(db->idxdb, j, kbuf, ksiz, &tsiz)) != NULL){
            CB_DATUMOPEN(nval);
            rp = vbuf;
            ep = vbuf + tsiz;
            lid = 0;
            cid = 0;
            while(rp < ep){
              EST_READ_VNUMBUF(rp, vnum, vstep);
              cid += vnum + 1;
              rp += vstep;
              pv = rp;
              switch(db->smode){
              case ESTDFSCVOID:
                break;
              default:
                rp++;
                break;
              case ESTDFSCINT:
              case ESTDFSCASIS:
                rp += sizeof(int);
                break;
              }
              while(*rp != 0x0){
                rp += 2;
              }
              rp++;
              if(!cbmapget(ids, (char *)&cid, sizeof(int), NULL)){
                EST_SET_VNUMBUF(vstep, numbuf, cid - lid - 1);
                CB_DATUMCAT(nval, numbuf, vstep);
                CB_DATUMCAT(nval, pv, rp - pv);
                lid = cid;
              }
            }
            if(!est_idx_put_one(db->idxdb, j, kbuf, ksiz, CB_DATUMPTR(nval), CB_DATUMSIZE(nval)))
              err = TRUE;
            sum += CB_DATUMSIZE(nval);
            CB_DATUMCLOSE(nval);
          }
        }
        if(sum < 1 && !vlout(db->fwmdb, kbuf, ksiz) && dpecode != DP_ENOITEM) err = TRUE;
      }
      cbmapout(db->outcc, kbuf, ksiz);
      if(i % ESTCCCBFREQ == 0) est_db_inform(db, "cleaning dispensable keys");
      if(max > 0 && db->intflag && i > 0 && i % ESTCCIRSLOT == 0) break;
    }
    if(cbmaprnum(db->outcc) <= cbmaprnum(ids)){
      cbmapclose(db->outcc);
      db->outcc = cbmapopenex(ESTOUTCCBNUM);
    }
    CB_LISTCLOSE(keys);
    cbmapclose(ids);
  }
  cbmapclose(db->keycc);
  db->keycc = cbmapopenex(ESTKEYCCMNUM + 1);
  db->kcmnum = ESTKEYCCMNUM;
  if(!(max > 0 && db->intflag) && inc && est_idx_size_current(db->idxdb) >= ESTIDXDBMIN){
    est_db_inform(db, "adding a new database file");
    est_idx_increment(db->idxdb);
  }
  if(max < 1 || max >= INT_MAX){
    if(!vlmemflush(db->auxdb)) err = TRUE;
    if(!est_idx_memflush(db->idxdb)) err = TRUE;
  }
  if(max > 0 && db->intflag) est_db_inform(db, "flushing interrupted");
  db->flsflag = FALSE;
  db->intflag = FALSE;
  if(err){
    est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
    db->fatal = TRUE;
    return FALSE;
  }
  return TRUE;
}


/* Synchronize updating contents of a database. */
int est_db_sync(ESTDB *db){
  ESTATTRIDX *attridx;
  const char *kbuf;
  int err;
  assert(db);
  if(!dpwritable(db->metadb)){
    est_set_ecode(&(db->ecode), ESTEACCES, __LINE__);
    return FALSE;
  }
  err = FALSE;
  if(!est_db_flush(db, -1) || !est_db_write_meta(db)) err = TRUE;
  est_db_inform(db, "synchronizing the database for meta information");
  if(!dpsync(db->metadb)) err = TRUE;
  est_db_inform(db, "synchronizing the inverted index");
  if(!est_idx_sync(db->idxdb)) err = TRUE;
  est_db_inform(db, "synchronizing the database for forward matching");
  if(!vlsync(db->fwmdb)) err = TRUE;
  est_db_inform(db, "synchronizing the database for attributes");
  if(!crsync(db->attrdb)) err = TRUE;
  est_db_inform(db, "synchronizing the database for texts");
  if(!crsync(db->textdb)) err = TRUE;
  est_db_inform(db, "synchronizing the database for keywords");
  if(!crsync(db->kwddb)) err = TRUE;
  est_db_inform(db, "synchronizing the database for document list");
  if(!vlsync(db->listdb)) err = TRUE;
  if(cbmaprnum(db->aidxs) > 0){
    est_db_inform(db, "synchronizing the databases for attribute narrowing");
    cbmapiterinit(db->aidxs);
    while((kbuf = cbmapiternext(db->aidxs, NULL)) != NULL){
      attridx = (ESTATTRIDX *)cbmapiterval(kbuf, NULL);
      switch(attridx->type){
      case ESTIDXATTRSTR:
      case ESTIDXATTRNUM:
        if(!vlsync(attridx->db)) err = TRUE;
        break;
      default:
        if(!dpsync(attridx->db)) err = TRUE;
        break;
      }
    }
  }
  if(err){
    est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
    db->fatal = TRUE;
  }
  return err ? FALSE : TRUE;
}


/* Optimize a database. */
int est_db_optimize(ESTDB *db, int options){
  CBMAP *dmap;
  CBLIST *words;
  CBDATUM *nval;
  ESTATTRIDX *attridx;
  const char *word, *rp, *pv, *ep;
  char *kbuf, *vbuf, *wp, numbuf[ESTNUMBUFSIZ];
  int i, err, id, ksiz, vsiz, wsiz, len, vstep;
  assert(db);
  if(!dpwritable(db->metadb)){
    est_set_ecode(&(db->ecode), ESTEACCES, __LINE__);
    return FALSE;
  }
  if(!est_db_flush(db, -1)) return FALSE;
  err = FALSE;
  if(!(options & ESTOPTNOPURGE)){
    dmap = cbmapopenex(vlrnum(db->listdb) + 1);
    vlcurfirst(db->listdb);
    while((vbuf = vlcurval(db->listdb, NULL)) != NULL){
      id = atoi(vbuf);
      cbmapput(dmap, (char *)&id, sizeof(int), "", 0, FALSE);
      free(vbuf);
      vlcurnext(db->listdb);
    }
    CB_LISTOPEN(words);
    vlcurfirst(db->fwmdb);
    while((kbuf = vlcurkey(db->fwmdb, &ksiz)) != NULL){
      CB_LISTPUSHBUF(words, kbuf, ksiz);
      vlcurnext(db->fwmdb);
    }
    for(i = 0; i < CB_LISTNUM(words); i++){
      if(i % (ESTIDXDBLRM * 4) == 0) est_idx_set_current(db->idxdb);
      word = CB_LISTVAL2(words, i, wsiz);
      vbuf = est_idx_scan(db->idxdb, word, wsiz, &vsiz, db->smode);
      CB_DATUMOPEN(nval);
      rp = vbuf;
      ep = vbuf + vsiz;
      while(rp < ep){
        pv = rp;
        EST_READ_VNUMBUF(rp, id, vstep);
        rp += vstep;
        switch(db->smode){
        case ESTDFSCVOID:
          break;
        default:
          rp++;
          break;
        case ESTDFSCINT:
        case ESTDFSCASIS:
          rp += sizeof(int);
          break;
        }
        while(*rp != 0x00){
          rp += 2;
        }
        rp++;
        if(cbmapget(dmap, (char *)&id, sizeof(int), NULL)) CB_DATUMCAT(nval, pv, rp - pv);
      }
      if(!est_idx_out(db->idxdb, word, wsiz)) err = TRUE;
      if(CB_DATUMSIZE(nval) > 0){
        if(!est_idx_add(db->idxdb, word, wsiz, CB_DATUMPTR(nval), CB_DATUMSIZE(nval), db->smode))
          err = TRUE;
      } else {
        if(!vlout(db->fwmdb, word, wsiz)) err = TRUE;
      }
      CB_DATUMCLOSE(nval);
      free(vbuf);
      free(kbuf);
      if(i % ESTCCCBFREQ == 0) est_db_inform(db, "cleaning dispensable keys");
    }
    CB_LISTCLOSE(words);
    CB_LISTOPEN(words);
    vlcurfirst(db->auxdb);
    while((kbuf = vlcurkey(db->auxdb, &ksiz)) != NULL){
      CB_LISTPUSHBUF(words, kbuf, ksiz);
      vlcurnext(db->auxdb);
    }
    for(i = 0; i < CB_LISTNUM(words); i++){
      word = CB_LISTVAL2(words, i, wsiz);
      if(!(vbuf = vlget(db->auxdb, word, wsiz, &vsiz))) continue;
      rp = vbuf;
      wp = vbuf;
      ep = vbuf + vsiz;
      while(rp < ep){
        if(cbmapget(dmap, rp, sizeof(int), NULL)){
          memmove(wp, rp, sizeof(int) * 2);
          wp += sizeof(int) * 2;
        }
        rp += sizeof(int) * 2;
      }
      if(wp > vbuf){
        if(!vlput(db->auxdb, word, wsiz, vbuf, wp - vbuf, VL_DOVER)) err = TRUE;
        len = sprintf(numbuf, "%d", (int)((wp - vbuf) / (sizeof(int) * 2)));
        if(!vlput(db->xfmdb, word, wsiz, numbuf, len, VL_DOVER)) err = TRUE;
      } else {
        if(!vlout(db->auxdb, word, wsiz)) err = TRUE;
        if(!vlout(db->xfmdb, word, wsiz) && dpecode != DP_ENOITEM) err = TRUE;
      }
      free(vbuf);
      if(i % ESTCCCBFREQ == 0) est_db_inform(db, "cleaning dispensable auxiliary keys");
    }
    CB_LISTCLOSE(words);
    cbmapclose(dmap);
  }
  if(!(options & ESTOPTNODBOPT)){
    est_db_inform(db, "optimizing the inverted index");
    if(!est_idx_optimize(db->idxdb)) err = TRUE;
    est_db_inform(db, "optimizing the database for forward matching");
    if(!vloptimize(db->fwmdb)) err = TRUE;
    est_db_inform(db, "optimizing the auxiliary index");
    if(!vloptimize(db->auxdb)) err = TRUE;
    est_db_inform(db, "optimizing the database for auxiliary forward matching");
    if(!vloptimize(db->xfmdb)) err = TRUE;
    est_db_inform(db, "optimizing the database for attributes");
    if(!croptimize(db->attrdb, -1)) err = TRUE;
    est_db_inform(db, "optimizing the database for texts");
    if(!croptimize(db->textdb, -1)) err = TRUE;
    est_db_inform(db, "optimizing the database for keywords");
    if(!croptimize(db->kwddb, -1)) err = TRUE;
    est_db_inform(db, "optimizing the database for document list");
    if(!vloptimize(db->listdb)) err = TRUE;
    if(cbmaprnum(db->aidxs) > 0){
      est_db_inform(db, "optimizing the databases for attribute narrowing");
      cbmapiterinit(db->aidxs);
      while((rp = cbmapiternext(db->aidxs, NULL)) != NULL){
        attridx = (ESTATTRIDX *)cbmapiterval(rp, NULL);
        switch(attridx->type){
        case ESTIDXATTRSTR:
        case ESTIDXATTRNUM:
          if(!vloptimize(attridx->db)) err = TRUE;
          break;
        default:
          if(!dpoptimize(attridx->db, -1)) err = TRUE;
          break;
        }
      }
    }
  }
  cbmapclose(db->rescc);
  db->rescc = cbmapopenex(db->rcmnum * 2 + 1);
  if(err){
    est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
    db->fatal = TRUE;
  }
  return err ? FALSE : TRUE;
}


/* Merge another database. */
int est_db_merge(ESTDB *db, const char *name, int options){
  ESTDB *tgdb;
  ESTATTRIDX *attridx;
  CBMAP *idmap, *seqmap, *attrs;
  CBLIST *words;
  CBDATUM *rbuf;
  const char *kbuf, *vbuf, *rp, *ep, *sp;
  char *tbuf, numbuf[ESTNUMBUFSIZ];
  int i, j, ecode, err, ksiz, vsiz, tsiz, oid, nid, len, vstep, anum, *ary;
  assert(db && name);
  if(!dpwritable(db->metadb)){
    est_set_ecode(&(db->ecode), ESTEACCES, __LINE__);
    return FALSE;
  }
  est_db_inform(db, "opening the target database");
  if(!(tgdb = est_db_open(name, ESTDBREADER, &ecode))){
    est_set_ecode(&(db->ecode), ecode, __LINE__);
    return FALSE;
  }
  if(dpgetflags(db->metadb) != dpgetflags(tgdb->metadb)){
    est_db_close(tgdb, &ecode);
    est_set_ecode(&(db->ecode), ESTEMISC, __LINE__);
    return FALSE;
  }
  err = FALSE;
  idmap = cbmapopenex(est_db_doc_num(tgdb) + 1);
  vlcurfirst(tgdb->listdb);
  for(i = 0; (kbuf = vlcurkeycache(tgdb->listdb, &ksiz)) != NULL; i++){
    if((vbuf = vlgetcache(db->listdb, kbuf, ksiz, NULL)) != NULL &&
       !est_db_out_doc(db, atoi(vbuf), options & ESTMGCLEAN ? ESTODCLEAN : 0)) err = TRUE;
    oid = atoi(vlcurvalcache(tgdb->listdb, NULL));
    db->dseq++;
    db->dnum++;
    cbmapput(idmap, (char *)&oid, sizeof(int), (char *)&(db->dseq), sizeof(int), FALSE);
    vlcurnext(tgdb->listdb);
    if(i % (ESTCCCBFREQ / 10) == 0) est_db_inform(db, "calculating ID mapping");
  }
  if(!est_db_flush(db, -1)){
    cbmapclose(idmap);
    est_db_close(tgdb, &ecode);
    return FALSE;
  }
  cbmapiterinit(idmap);
  for(i = 0; (kbuf = cbmapiternext(idmap, &ksiz)) != NULL; i++){
    CB_MAPITERVAL(vbuf, kbuf, vsiz);
    oid = *(int *)kbuf;
    nid = *(int *)vbuf;
    if((tbuf = est_crget(tgdb->attrdb, tgdb->zmode, oid, &tsiz)) != NULL){
      attrs = cbmapload(tbuf, tsiz);
      len = sprintf(numbuf, "%d", nid);
      cbmapput(attrs, ESTDATTRID, -1, numbuf, len, TRUE);
      free(tbuf);
      tbuf = cbmapdump(attrs, &tsiz);
      if((vbuf = cbmapget(attrs, ESTDATTRURI, -1, &vsiz)) != NULL){
        if(!vlput(db->listdb, vbuf, vsiz, numbuf, len, VL_DKEEP)){
          est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
          db->fatal = TRUE;
          err = TRUE;
        }
      } else {
        est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
        db->fatal = TRUE;
        err = TRUE;
      }
      if(!est_crput(db->attrdb, db->zmode, nid, tbuf, tsiz, CR_DKEEP)){
        est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
        db->fatal = TRUE;
        err = TRUE;
      }
      if(cbmaprnum(db->aidxs) > 0){
        cbmapiterinit(db->aidxs);
        while((kbuf = cbmapiternext(db->aidxs, &ksiz)) != NULL){
          if(!(vbuf = cbmapget(attrs, kbuf, ksiz, &vsiz))) continue;
          attridx = (ESTATTRIDX *)cbmapiterval(kbuf, NULL);
          switch(attridx->type){
          case ESTIDXATTRSTR:
          case ESTIDXATTRNUM:
            if(!est_aidx_attr_put(attridx->db, nid, vbuf, vsiz)){
              est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
              db->fatal = TRUE;
              err = TRUE;
            }
            break;
          default:
            if(!est_aidx_seq_put(attridx->db, nid, vbuf, vsiz)){
              est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
              db->fatal = TRUE;
              err = TRUE;
            }
            break;
          }
        }
      }
      cbmapclose(attrs);
      free(tbuf);
    } else {
      est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
      err = TRUE;
    }
    if((tbuf = est_crget(tgdb->textdb, tgdb->zmode, oid, &tsiz)) != NULL){
      if(!est_crput(db->textdb, db->zmode, nid, tbuf, tsiz, CR_DKEEP)){
        est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
        db->fatal = TRUE;
        err = TRUE;
      }
      free(tbuf);
    } else {
      est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
      err = TRUE;
    }
    if((tbuf = est_crget(tgdb->kwddb, tgdb->zmode, oid, &tsiz)) != NULL){
      if(!est_crput(db->kwddb, db->zmode, nid, tbuf, tsiz, CR_DKEEP)){
        est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
        db->fatal = TRUE;
        err = TRUE;
      }
      free(tbuf);
    } else if(dpecode != DP_ENOITEM){
      est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
      db->fatal = TRUE;
      err = TRUE;
    }
    if(i % (ESTCCCBFREQ / 10) == 0) est_db_inform(db, "importing documents");
  }
  CB_LISTOPEN(words);
  vlcurfirst(tgdb->fwmdb);
  while((kbuf = vlcurkeycache(tgdb->fwmdb, &ksiz)) != NULL){
    CB_LISTPUSH(words, kbuf, ksiz);
    vlcurnext(tgdb->fwmdb);
  }
  for(i = 0; i < CB_LISTNUM(words); i++){
    kbuf = CB_LISTVAL2(words, i, ksiz);
    seqmap = cbmapopenex(tsiz / sizeof(int) + 1);
    tbuf = est_idx_scan(tgdb->idxdb, kbuf, ksiz, &tsiz, tgdb->smode);
    rp = tbuf;
    ep = tbuf + tsiz;
    while(rp < ep){
      EST_READ_VNUMBUF(rp, oid, vstep);
      rp += vstep;
      vbuf = cbmapget(idmap, (char *)&oid, sizeof(int), NULL);
      nid = vbuf ? *(int *)vbuf : -1;
      sp = rp;
      switch(tgdb->smode){
      case ESTDFSCVOID:
        break;
      default:
        rp++;
        break;
      case ESTDFSCINT:
      case ESTDFSCASIS:
        rp += sizeof(int);
        break;
      }
      while(*rp != 0x00){
        rp += 2;
      }
      rp++;
      if(nid > 0) cbmapputcat(seqmap, (char *)&nid, sizeof(int), sp, rp - sp);
    }
    anum = cbmaprnum(seqmap);
    CB_MALLOC(ary, anum * sizeof(int) + 1);
    cbmapiterinit(seqmap);
    for(j = 0; (rp = cbmapiternext(seqmap, NULL)) != NULL; j++){
      ary[j] = *(int *)rp;
    }
    qsort(ary, anum, sizeof(int), est_int_compare);
    CB_DATUMOPEN(rbuf);
    for(j = 0; j < anum; j++){
      EST_SET_VNUMBUF(vstep, numbuf, ary[j]);
      CB_DATUMCAT(rbuf, numbuf, vstep);
      vbuf = cbmapget(seqmap, (char *)(ary + j), sizeof(int), &vsiz);
      CB_DATUMCAT(rbuf, vbuf, vsiz);
    }
    if(!est_idx_add(db->idxdb, kbuf, ksiz, CB_DATUMPTR(rbuf), CB_DATUMSIZE(rbuf), db->smode)){
      est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
      db->fatal = TRUE;
      err = TRUE;
    }
    CB_DATUMCLOSE(rbuf);
    free(ary);
    cbmapclose(seqmap);
    free(tbuf);
    vlput(db->fwmdb, kbuf, ksiz, "", 0, VL_DKEEP);
    if(i % ESTCCCBFREQ == 0){
      est_db_inform(db, "importing words");
      if(est_idx_size_current(db->idxdb) >= ESTIDXDBMAX){
        est_db_inform(db, "adding a new database file");
        est_idx_increment(db->idxdb);
      }
    }
  }
  CB_LISTCLOSE(words);
  CB_LISTOPEN(words);
  vlcurfirst(tgdb->auxdb);
  while((kbuf = vlcurkeycache(tgdb->auxdb, &ksiz)) != NULL){
    CB_LISTPUSH(words, kbuf, ksiz);
    vlcurnext(tgdb->auxdb);
  }
  for(i = 0; i < CB_LISTNUM(words); i++){
    kbuf = CB_LISTVAL2(words, i, ksiz);
    vbuf = vlgetcache(tgdb->auxdb, kbuf, ksiz, &vsiz);
    CB_DATUMOPEN(rbuf);
    rp = vbuf;
    ep = vbuf + vsiz;
    while(rp < ep){
      oid = *(int *)rp;
      vbuf = cbmapget(idmap, rp, sizeof(int), NULL);
      nid = vbuf ? *(int *)vbuf : -1;
      if(nid > 0){
        CB_DATUMCAT(rbuf, (char *)&nid, sizeof(int));
        CB_DATUMCAT(rbuf, rp + sizeof(int), sizeof(int));
      }
      rp += sizeof(int) * 2;
    }
    if(!vlput(db->auxdb, kbuf, ksiz, CB_DATUMPTR(rbuf), CB_DATUMSIZE(rbuf), VL_DCAT)){
      est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
      db->fatal = TRUE;
      err = TRUE;
    }
    CB_DATUMCLOSE(rbuf);
    anum = 0;
    if((vbuf = vlgetcache(tgdb->xfmdb, kbuf, ksiz, NULL)) != NULL) anum += atoi(vbuf);
    len = sprintf(numbuf, "%d", anum);
    vlput(db->xfmdb, kbuf, ksiz, numbuf, len, VL_DOVER);
    if(i % ESTCCCBFREQ == 0) est_db_inform(db, "importing auxiliary words");
  }
  CB_LISTCLOSE(words);
  cbmapclose(idmap);
  est_db_inform(db, "closing the target database");
  if(!est_db_close(tgdb, &ecode)){
    est_set_ecode(&(db->ecode), ecode, __LINE__);
    return FALSE;
  }
  if(!est_db_flush(db, -1)) err = TRUE;
  return err ? FALSE : TRUE;
}


/* Add a document to a database. */
int est_db_put_doc(ESTDB *db, ESTDOC *doc, int options){
  CBMAP *ocmap, *fmap, *qmap;
  CBLIST *words;
  CBDATUM *ocbuf;
  ESTATTRIDX *attridx;
  md5_state_t ms;
  const char *uri, *ndig, *text, *word, *fnext, *snext, *kbuf, *vbuf;
  unsigned char junc[2], c;
  char dobuf[32], dsbuf[64], *wp, *odig, wbuf[ESTWORDMAXLEN+3], *sbuf, nbuf[ESTNUMBUFSIZ];
  int i, j, id, err, wnum, wsiz, fnsiz, snsiz, *np, score, num, ksiz, vsiz, ssiz;
  double tune, weight;
  assert(db && doc);
  if(!dpwritable(db->metadb)){
    est_set_ecode(&(db->ecode), ESTEACCES, __LINE__);
    return FALSE;
  }
  if(!doc->attrs || !(uri = cbmapget(doc->attrs, ESTDATTRURI, -1, NULL)) || uri[0] == '\0'){
    est_set_ecode(&(db->ecode), ESTEINVAL, __LINE__);
    return FALSE;
  }
  if(!doc->dtexts) CB_LISTOPEN(doc->dtexts);
  if(!(ndig = cbmapget(doc->attrs, ESTDATTRDIGEST, -1, NULL))){
    md5_init(&ms);
    for(i = 0; i < CB_LISTNUM(doc->dtexts); i++){
      vbuf = CB_LISTVAL2(doc->dtexts, i, vsiz);
      md5_append(&ms, (md5_byte_t *)vbuf, vsiz);
      md5_append(&ms, (md5_byte_t *)"\n", 1);
    }
    if((vbuf = cbmapget(doc->attrs, "", 0, &vsiz)) != NULL){
      md5_append(&ms, (md5_byte_t *)"\t", 1);
      md5_append(&ms, (md5_byte_t *)vbuf, vsiz);
      md5_append(&ms, (md5_byte_t *)"\n", 1);
    }
    md5_finish(&ms, (md5_byte_t *)dobuf);
    wp = dsbuf;
    for(i = 0; i < 16; i++){
      wp += sprintf(wp, "%02x", ((unsigned char *)dobuf)[i]);
    }
    ndig = dsbuf;
    cbmapput(doc->attrs, ESTDATTRDIGEST, -1, ndig, -1, FALSE);
  }
  if((id = est_db_uri_to_id(db, uri)) > 0){
    if((odig = est_db_get_doc_attr(db, id, ESTDATTRDIGEST)) != NULL){
      if(!strcmp(odig, ndig)){
        free(odig);
        doc->id = id;
        sprintf(nbuf, "%d", id);
        cbmapput(doc->attrs, ESTDATTRID, -1, nbuf, -1, TRUE);
        return est_db_edit_doc(db, doc);
      }
      free(odig);
    }
    if(!est_db_out_doc(db, id, (options & ESTPDCLEAN) ? ESTODCLEAN : 0)) return FALSE;
  }
  doc->id = ++(db->dseq);
  sprintf(nbuf, "%d", doc->id);
  cbmapput(doc->attrs, ESTDATTRID, -1, nbuf, -1, TRUE);
  ocmap = cbmapopen();
  fmap = cbmapopen();
  qmap = cbmapopen();
  wnum = 0;
  for(i = -1; i < CB_LISTNUM(doc->dtexts); i++){
    if(i < 0){
      if(!(text = cbmapget(doc->attrs, "", 0, NULL))) continue;
    } else {
      text = CB_LISTVAL(doc->dtexts, i);
    }
    CB_LISTOPEN(words);
    switch(db->amode){
    case ESTDFPERFNG:
      est_break_text_perfng(text, words, FALSE, TRUE);
      break;
    case ESTDFCHRCAT:
      est_break_text_chrcat(text, words, FALSE);
      break;
    default:
      est_break_text(text, words, FALSE, TRUE);
      break;
    }
    wnum += CB_LISTNUM(words);
    for(j = 0; j < CB_LISTNUM(words); j++){
      word = CB_LISTVAL2(words, j, wsiz);
      if(wsiz > ESTWORDMAXLEN) continue;
      fnext = cblistval(words, j + 1, &fnsiz);
      snext = cblistval(words, j + 2, &snsiz);
      junc[0] = fnext ? dpinnerhash(fnext, fnsiz) % ESTJHASHNUM + 1: 0xff;
      junc[1] = snext ? dpouterhash(snext, snsiz) % ESTJHASHNUM + 1: 0xff;
      memcpy(wbuf, word, wsiz);
      memcpy(wbuf + wsiz, "\t", 1);
      memcpy(wbuf + wsiz + 1, junc, 2);
      np = (int *)cbmapget(fmap, word, wsiz, NULL);
      num = np ? *(int *)np : 0;
      num += ESTOCPOINT;
      cbmapput(fmap, word, wsiz, (char *)&num, sizeof(int), TRUE);
      if(cbmapput(qmap, wbuf, wsiz + 3, "", 0, FALSE))
        cbmapputcat(ocmap, word, wsiz, (char *)junc, fnext ? 2 : 0);
    }
    CB_LISTCLOSE(words);
  }
  score = (vbuf = cbmapget(doc->attrs, "\t", 1, NULL)) ? atoi(vbuf) : -1;
  weight = 1.0;
  if(score < 0 && (options & ESTPDWEIGHT) &&
     (vbuf = cbmapget(doc->attrs, ESTDATTRWEIGHT, -1, NULL)) != NULL){
    weight = strtod(vbuf, NULL);
    weight = weight >= 0.01 ? weight : 0.01;
  }
  tune = sqrt(wnum + 128) / 16.0 / weight;
  cbmapiterinit(ocmap);
  while((kbuf = cbmapiternext(ocmap, &ksiz)) != NULL){
    CB_MAPITERVAL(vbuf, kbuf, vsiz);
    if(vsiz > 2) qsort((void *)vbuf, vsiz / 2, 2, est_short_compare);
    CB_DATUMOPEN(ocbuf);
    EST_SET_VNUMBUF(wsiz, wbuf, doc->id);
    CB_DATUMCAT(ocbuf, wbuf, wsiz);
    switch(db->smode){
    case ESTDFSCVOID:
      break;
    default:
      num = score < 0 ? *(int *)cbmapget(fmap, kbuf, ksiz, NULL) / tune : score;
      if(num >= 0x80) num += (0x80 - num) * 0.75;
      if(num >= 0xc0) num += (0xc0 - num) * 0.75;
      c = num < 0xff ? num : 0xff;
      CB_DATUMCAT(ocbuf, (char *)&c, 1);
      break;
    case ESTDFSCINT:
    case ESTDFSCASIS:
      num = score < 0 ? *(int *)cbmapget(fmap, kbuf, ksiz, NULL) * 10 / tune : score;
      CB_DATUMCAT(ocbuf, (char *)&num, sizeof(int));
      break;
    }
    CB_DATUMCAT(ocbuf, vbuf, vsiz);
    c = 0x00;
    CB_DATUMCAT(ocbuf, (char *)&c, 1);
    cbmapputcat(db->idxcc, kbuf, ksiz, CB_DATUMPTR(ocbuf), CB_DATUMSIZE(ocbuf));
    db->icsiz += CB_DATUMSIZE(ocbuf);
    CB_DATUMCLOSE(ocbuf);
  }
  cbmapclose(qmap);
  cbmapclose(fmap);
  cbmapclose(ocmap);
  err = FALSE;
  sbuf = cbmapdump(doc->attrs, &ssiz);
  if(!est_crput(db->attrdb, db->zmode, doc->id, sbuf, ssiz, CR_DKEEP)){
    est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
    db->fatal = TRUE;
    err = TRUE;
  }
  free(sbuf);
  sbuf = cblistdump(doc->dtexts, &ssiz);
  if(!est_crput(db->textdb, db->zmode, doc->id, sbuf, ssiz, CR_DKEEP)){
    est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
    db->fatal = TRUE;
    err = TRUE;
  }
  free(sbuf);
  if(doc->kwords && !est_db_put_keywords(db, doc->id, doc->kwords, weight)) err = TRUE;
  sprintf(nbuf, "%d", doc->id);
  if(!vlput(db->listdb, uri, -1, nbuf, -1, VL_DKEEP)){
    est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
    db->fatal = TRUE;
    err = TRUE;
  }
  if(cbmaprnum(db->aidxs) > 0){
    cbmapiterinit(db->aidxs);
    while((kbuf = cbmapiternext(db->aidxs, &ksiz)) != NULL){
      if(!(vbuf = cbmapget(doc->attrs, kbuf, ksiz, &vsiz))) continue;
      attridx = (ESTATTRIDX *)cbmapiterval(kbuf, NULL);
      switch(attridx->type){
      case ESTIDXATTRSTR:
      case ESTIDXATTRNUM:
        if(!est_aidx_attr_put(attridx->db, doc->id, vbuf, vsiz)){
          est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
          db->fatal = TRUE;
          err = TRUE;
        }
        break;
      default:
        if(!est_aidx_seq_put(attridx->db, doc->id, vbuf, vsiz)){
          est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
          db->fatal = TRUE;
          err = TRUE;
        }
        break;
      }
    }
  }
  db->dnum++;
  if(est_db_used_cache_size(db) > db->icmax && !est_db_flush(db, INT_MAX)) err = TRUE;
  return err ? FALSE : TRUE;
}


/* Remove a document from a database. */
int est_db_out_doc(ESTDB *db, int id, int options){
  ESTDOC *doc;
  CBLIST *words;
  ESTATTRIDX *attridx;
  const char *uri, *kbuf, *vbuf, *text, *word;
  char numbuf[ESTNUMBUFSIZ];
  int i, j, ksiz, vsiz, len, wsiz;
  assert(db && id > 0);
  if(!dpwritable(db->metadb)){
    est_set_ecode(&(db->ecode), ESTEACCES, __LINE__);
    return FALSE;
  }
  if(id >= ESTPDOCIDMIN){
    est_set_ecode(&(db->ecode), ESTEINVAL, __LINE__);
    return FALSE;
  }
  if(!(doc = est_db_get_doc(db, id, ESTGDNOKWD))) return FALSE;
  if(!doc->attrs || !(uri = cbmapget(doc->attrs, ESTDATTRURI, -1, NULL))){
    est_doc_delete(doc);
    est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
    db->fatal = TRUE;
    return FALSE;
  }
  if(!est_crout(db->attrdb, id) || !est_crout(db->textdb, id) || !vlout(db->listdb, uri, -1)){
    est_doc_delete(doc);
    est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
    db->fatal = TRUE;
    return FALSE;
  }
  cbmapout(db->attrcc, (char *)&id, sizeof(int));
  cbmapout(db->textcc, (char *)&id, sizeof(int));
  if(db->spacc) cbmapout(db->spacc, (char *)&id, sizeof(int));
  if(cbmaprnum(db->aidxs) > 0){
    cbmapiterinit(db->aidxs);
    while((kbuf = cbmapiternext(db->aidxs, &ksiz)) != NULL){
      if(!(vbuf = cbmapget(doc->attrs, kbuf, ksiz, &vsiz))) continue;
      attridx = (ESTATTRIDX *)cbmapiterval(kbuf, NULL);
      switch(attridx->type){
      case ESTIDXATTRSTR:
      case ESTIDXATTRNUM:
        if(!est_aidx_attr_out(attridx->db, doc->id, vbuf, vsiz)){
          est_doc_delete(doc);
          est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
          db->fatal = TRUE;
          return FALSE;
        }
        break;
      default:
        if(!est_aidx_seq_out(attridx->db, doc->id)){
          est_doc_delete(doc);
          est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
          db->fatal = TRUE;
          return FALSE;
        }
        break;
      }
    }
  }
  if(options & ESTODCLEAN){
    len = sprintf(numbuf, "\t%d", doc->id);
    cbmapput(db->outcc, numbuf, len, "", 0, FALSE);
    for(i = -1; i < CB_LISTNUM(doc->dtexts); i++){
      if(i < 0){
        if(!(text = cbmapget(doc->attrs, "", 0, NULL))) continue;
      } else {
        text = CB_LISTVAL(doc->dtexts, i);
      }
      CB_LISTOPEN(words);
      switch(db->amode){
      case ESTDFPERFNG:
        est_break_text_perfng(text, words, FALSE, TRUE);
        break;
      case ESTDFCHRCAT:
        est_break_text_chrcat(text, words, FALSE);
        break;
      default:
        est_break_text(text, words, FALSE, TRUE);
        break;
      }
      for(j = 0; j < CB_LISTNUM(words); j++){
        word = CB_LISTVAL2(words, j, wsiz);
        cbmapput(db->outcc, word, wsiz, "", 0, FALSE);
      }
      CB_LISTCLOSE(words);
    }
    if(!est_db_out_keywords(db, id) && db->ecode != ESTENOITEM){
      est_doc_delete(doc);
      est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
      db->fatal = TRUE;
      return FALSE;
    }
  } else {
    if(!est_crout(db->kwddb, id) && dpecode != DP_ENOITEM){
      est_doc_delete(doc);
      est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
      db->fatal = TRUE;
      return FALSE;
    }
    cbmapout(db->veccc, (char *)&id, sizeof(int));
  }
  est_doc_delete(doc);
  if(!est_db_set_doc_entity(db, id, NULL, -1) && db->ecode != ESTENOITEM) return FALSE;
  db->dnum--;
  return TRUE;
}


/* Edit attributes of a document object in a database. */
int est_db_edit_doc(ESTDB *db, ESTDOC *doc){
  ESTDOC *odoc;
  ESTATTRIDX *attridx;
  const char *uri, *tmp, *kbuf, *vbuf;
  char *ouri, numbuf[ESTNUMBUFSIZ], *text, *sbuf;
  int err, id, oid, ksiz, vsiz, ssiz;
  assert(db && doc);
  if(!dpwritable(db->metadb)){
    est_set_ecode(&(db->ecode), ESTEACCES, __LINE__);
    return FALSE;
  }
  id = -1;
  uri = NULL;
  if(doc->attrs){
    if((tmp = cbmapget(doc->attrs, ESTDATTRID, -1, NULL)) != NULL) id = atoi(tmp);
    if((tmp = cbmapget(doc->attrs, ESTDATTRURI, -1, NULL)) != NULL) uri = tmp;
  }
  if(id < 1 || id >= ESTPDOCIDMIN || (doc->id > 0 && doc->id != id) || !uri || uri[0] == '\0'){
    est_set_ecode(&(db->ecode), ESTEINVAL, __LINE__);
    return FALSE;
  }
  err = FALSE;
  if((oid = est_db_uri_to_id(db, uri)) == -1){
    if(!(ouri = est_db_get_doc_attr(db, id, ESTDATTRURI))){
      est_set_ecode(&(db->ecode), ESTEINVAL, __LINE__);
      return FALSE;
    }
    sprintf(numbuf, "%d", id);
    if(!vlout(db->listdb, ouri, -1) || !vlput(db->listdb, uri, -1, numbuf, -1, VL_DKEEP))
      err = TRUE;
    free(ouri);
  } else if(oid != id){
    est_set_ecode(&(db->ecode), ESTEINVAL, __LINE__);
    return FALSE;
  }
  doc->id = id;
  if(cbmaprnum(db->aidxs) > 0 && (odoc = est_db_get_doc(db, id, ESTGDNOTEXT))){
    if(!odoc->attrs) odoc->attrs = cbmapopenex(ESTMINIBNUM);
    cbmapiterinit(db->aidxs);
    while((kbuf = cbmapiternext(db->aidxs, &ksiz)) != NULL){
      if(!(vbuf = cbmapget(odoc->attrs, kbuf, ksiz, &vsiz))) continue;
      attridx = (ESTATTRIDX *)cbmapiterval(kbuf, NULL);
      switch(attridx->type){
      case ESTIDXATTRSTR:
      case ESTIDXATTRNUM:
        if(!est_aidx_attr_out(attridx->db, id, vbuf, vsiz)){
          est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
          db->fatal = TRUE;
          err = TRUE;
        }
        break;
      default:
        if(!est_aidx_seq_out(attridx->db, id)){
          est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
          db->fatal = TRUE;
          err = TRUE;
        }
        break;
      }
    }
    cbmapiterinit(db->aidxs);
    while((kbuf = cbmapiternext(db->aidxs, &ksiz)) != NULL){
      if(!(vbuf = cbmapget(doc->attrs, kbuf, ksiz, &vsiz))) continue;
      attridx = (ESTATTRIDX *)cbmapiterval(kbuf, NULL);
      switch(attridx->type){
      case ESTIDXATTRSTR:
      case ESTIDXATTRNUM:
        if(!est_aidx_attr_put(attridx->db, id, vbuf, vsiz)){
          est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
          db->fatal = TRUE;
          err = TRUE;
        }
        break;
      default:
        if(!est_aidx_seq_put(attridx->db, id, vbuf, vsiz)){
          est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
          db->fatal = TRUE;
          err = TRUE;
        }
        break;
      }
    }
    est_doc_delete(odoc);
  }
  if((text = est_db_get_doc_attr(db, id, "")) != NULL){
    cbmapput(doc->attrs, "", 0, text, -1, TRUE);
    free(text);
  }
  sbuf = cbmapdump(doc->attrs, &ssiz);
  if(!est_crput(db->attrdb, db->zmode, id, sbuf, ssiz, CR_DOVER)){
    est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
    db->fatal = TRUE;
    err = TRUE;
  }
  free(sbuf);
  cbmapout(db->attrcc, (char *)&id, sizeof(int));
  if(db->spacc) cbmapout(db->spacc, (char *)&id, sizeof(int));
  return err ? FALSE : TRUE;
}


/* Retrieve a document in a database. */
ESTDOC *est_db_get_doc(ESTDB *db, int id, int options){
  ESTDOC *doc;
  const char *cbuf;
  char *vbuf, numbuf[ESTNUMBUFSIZ];
  int i, csiz, vsiz, num;
  assert(db && id > 0);
  if(id >= ESTPDOCIDMIN){
    if((num = id - ESTPDOCIDMIN) >= CB_LISTNUM(db->pdocs)){
      est_set_ecode(&(db->ecode), ESTENOITEM, __LINE__);
      return NULL;
    }
    if((vbuf = cbreadfile(CB_LISTVAL(db->pdocs, num), NULL)) != NULL){
      doc = est_doc_new_from_draft(vbuf);
      free(vbuf);
    } else {
      doc = est_doc_new();
    }
    doc->id = id;
    sprintf(numbuf, "%d", id);
    est_doc_add_attr(doc, ESTDATTRID, numbuf);
    if(!est_doc_attr(doc, ESTDATTRURI))
      est_doc_add_attr(doc, ESTDATTRURI, CB_LISTVAL(db->pdocs, num));
    return doc;
  }
  cbuf = NULL;
  if(options & ESTGDNOATTR){
    if(crvsiz(db->attrdb, (char *)&id, sizeof(int)) == -1){
      if(dpecode == DP_ENOITEM){
        est_set_ecode(&(db->ecode), ESTENOITEM, __LINE__);
        return NULL;
      } else {
        est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
        db->fatal = TRUE;
        return NULL;
      }
    }
    vbuf = NULL;
  } else if((cbuf = cbmapget(db->attrcc, (char *)&id, sizeof(int), &csiz)) != NULL){
    cbmapmove(db->attrcc, (char *)&id, sizeof(int), FALSE);
    vbuf = NULL;
  } else if(!(vbuf = est_crget(db->attrdb, db->zmode, id, &vsiz))){
    if(dpecode == DP_ENOITEM){
      est_set_ecode(&(db->ecode), ESTENOITEM, __LINE__);
      return NULL;
    } else {
      est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
      db->fatal = TRUE;
      return NULL;
    }
  }
  doc = est_doc_new();
  doc->id = id;
  if(cbuf){
    doc->attrs = cbmapload(cbuf, csiz);
  } else if(vbuf){
    doc->attrs = cbmapload(vbuf, vsiz);
    if(db->acmnum > 0) cbmapput(db->attrcc, (char *)&id, sizeof(int), vbuf, vsiz, TRUE);
    free(vbuf);
    if(cbmaprnum(db->attrcc) > db->acmnum){
      num = cbmaprnum(db->attrcc) * 0.1 + 1;
      cbmapiterinit(db->attrcc);
      for(i = 0; i < num && (cbuf = cbmapiternext(db->attrcc, NULL)) != NULL; i++){
        cbmapout(db->attrcc, cbuf, sizeof(int));
      }
    }
  } else {
    doc->attrs = NULL;
  }
  if(!(options & ESTGDNOTEXT)){
    if((cbuf = cbmapget(db->textcc, (char *)&id, sizeof(int), &csiz)) != NULL){
      cbmapmove(db->textcc, (char *)&id, sizeof(int), FALSE);
      doc->dtexts = cblistload(cbuf, csiz);
    } else {
      if(!(vbuf = est_crget(db->textdb, db->zmode, id, &vsiz))){
        est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
        db->fatal = TRUE;
        est_doc_delete(doc);
        return NULL;
      }
      doc->dtexts = cblistload(vbuf, vsiz);
      if(db->tcmnum > 0) cbmapput(db->textcc, (char *)&id, sizeof(int), vbuf, vsiz, TRUE);
      free(vbuf);
      if(cbmaprnum(db->textcc) > db->tcmnum){
        num = cbmaprnum(db->textcc) * 0.1 + 1;
        cbmapiterinit(db->textcc);
        for(i = 0; i < num &&(cbuf = cbmapiternext(db->textcc, NULL)) != NULL; i++){
          cbmapout(db->textcc, cbuf, sizeof(int));
        }
      }
    }
  }
  if(!(options & ESTGDNOKWD)) doc->kwords = est_db_get_keywords(db, id);
  return doc;
}


/* Retrieve the value of an attribute of a document in a database. */
char *est_db_get_doc_attr(ESTDB *db, int id, const char *name){
  ESTATTRIDX *attridx;
  ESTDOC *doc;
  const char *cbuf;
  char *mbuf, *vbuf;
  int cb, csiz, msiz, vsiz;
  assert(db && id > 0 && name);
  if(id >= ESTPDOCIDMIN){
    if(!(doc = est_db_get_doc(db, id, 0))){
      est_set_ecode(&(db->ecode), ESTENOITEM, __LINE__);
      return NULL;
    }
    if(!(cbuf = est_doc_attr(doc, name))){
      est_set_ecode(&(db->ecode), ESTENOITEM, __LINE__);
      est_doc_delete(doc);
      return NULL;
    }
    vbuf = cbmemdup(cbuf, -1);
    est_doc_delete(doc);
    return vbuf;
  }
  cb = db->spacc && !strcmp(name, db->scname);
  if(cb && (cbuf = cbmapget(db->spacc, (char *)&id, sizeof(int), &csiz)) != NULL){
    cbmapmove(db->spacc, (char *)&id, sizeof(int), FALSE);
    return cbmemdup(cbuf, csiz);
  }
  if((attridx = (ESTATTRIDX *)cbmapget(db->aidxs, name, -1, NULL)) != NULL &&
     attridx->type ==  ESTIDXATTRSEQ){
    if(!(vbuf = est_aidx_seq_get(attridx->db, id, &vsiz))){
      est_set_ecode(&(db->ecode), ESTENOITEM, __LINE__);
      return NULL;
    }
    if(cb) cbmapput(db->spacc, (char *)&id, sizeof(int), vbuf, vsiz, FALSE);
    return vbuf;
  }
  if(!(mbuf = est_crget(db->attrdb, db->zmode, id, &msiz))){
    est_set_ecode(&(db->ecode), dpecode == DP_ENOITEM ? ESTENOITEM : ESTEDB, __LINE__);
    return NULL;
  }
  if(!(vbuf = cbmaploadone(mbuf, msiz, name, -1, &vsiz))){
    est_set_ecode(&(db->ecode), ESTENOITEM, __LINE__);
    free(mbuf);
    return NULL;
  }
  if(cb) cbmapput(db->spacc, (char *)&id, sizeof(int), vbuf, vsiz, FALSE);
  free(mbuf);
  return vbuf;
}


/* Get the ID of a document spacified by URI. */
int est_db_uri_to_id(ESTDB *db, const char *uri){
  const char *vbuf;
  int id;
  assert(db && uri);
  if(!(vbuf = vlgetcache(db->listdb, uri, -1, NULL))){
    if(CB_LISTNUM(db->pdocs) > 0 && (id = est_pidx_uri_to_id(db, uri)) > 0) return id;
    est_set_ecode(&(db->ecode), ESTENOITEM, __LINE__);
    return -1;
  }
  return atoi(vbuf);
}


/* Get the name of a database. */
const char *est_db_name(ESTDB *db){
  assert(db);
  return db->name;
}


/* Get the number of documents in a database. */
int est_db_doc_num(ESTDB *db){
  assert(db);
  return db->dnum;
}


/* Get the number of words in a database. */
int est_db_word_num(ESTDB *db){
  int wnum;
  assert(db);
  wnum = vlrnum(db->fwmdb);
  return wnum > 0 ? wnum : 0;
}


/* Get the size of a database. */
double est_db_size(ESTDB *db){
  ESTATTRIDX *attridx;
  const char *kbuf;
  double size;
  assert(db);
  size = (double)dpfsiz(db->metadb) + est_idx_size(db->idxdb) + vlfsiz(db->fwmdb) +
    vlfsiz(db->auxdb) + vlfsiz(db->xfmdb) + crfsizd(db->attrdb) + crfsizd(db->textdb) +
    crfsizd(db->kwddb) + vlfsiz(db->listdb);
  if(cbmaprnum(db->aidxs) > 0){
    cbmapiterinit(db->aidxs);
    while((kbuf = cbmapiternext(db->aidxs, NULL)) != NULL){
      attridx = (ESTATTRIDX *)cbmapiterval(kbuf, NULL);
      switch(attridx->type){
      case ESTIDXATTRSTR:
      case ESTIDXATTRNUM:
        size += vlfsiz(attridx->db);
        break;
      default:
        size += dpfsiz(attridx->db);
        break;
      }
    }
  }
  return size;
}


/* Search documents corresponding a condition for a database. */
int *est_db_search(ESTDB *db, ESTCOND *cond, int *nump, CBMAP *hints){
  ESTSCORE *scores, *tscores;
  CBMAP *svmap, *ordattrs;
  CBLIST *terms;
  const char *term, *rp;
  char *tmp, numbuf[ESTNUMBUFSIZ];
  const int *nscores;
  int i, j, snum, ign, nsnum, unum, knum, mnum, top, pcnum, ncnum, tsnum, add;
  int nnum, id, score, hnum, len, rest, *rval, rnum;
  double tune;
  assert(db && cond && nump);
  if(cond->auxwords) cbmapclose(cond->auxwords);
  cond->auxwords = cbmapopenex(ESTMINIBNUM);
  scores = NULL;
  snum = 0;
  ign = -1;
  nscores = cond->nscores;
  nsnum = cond->nsnum;
  ordattrs = cbmapopenex(cond->order ? (CB_LISTNUM(db->pdocs) + ESTMINIBNUM) : 1);
  if(cond->phrase){
    if(cbstrfwmatch(cond->phrase, ESTOPID)){
      if((id = atoi(cond->phrase + strlen(ESTOPID))) > 0){
        CB_MALLOC(scores, sizeof(ESTSCORE));
        scores[0].id = id;
        scores[0].score = 0;
        scores[0].value = NULL;
        snum = 1;
      } else {
        CB_MALLOC(scores, 1);
        snum = 0;
      }
    } else if(cbstrfwmatch(cond->phrase, ESTOPURI)){
      rp = cond->phrase + strlen(ESTOPURI);
      while(*rp > '\0' && *rp <= ' '){
        rp++;
      }
      if((id = est_db_uri_to_id(db, rp)) > 0){
        CB_MALLOC(scores, sizeof(ESTSCORE));
        scores[0].id = id;
        scores[0].score = 0;
        scores[0].value = NULL;
        snum = 1;
      } else {
        CB_MALLOC(scores, 1);
        snum = 0;
      }
    } else if(cbstrfwmatch(cond->phrase, ESTOPSIMILAR)){
      rp = cond->phrase + strlen(ESTOPSIMILAR);
      while(*rp > '\0' && *rp <= ' '){
        rp++;
      }
      knum = -1;
      unum = -1;
      mnum = -1;
      if(*rp >= '0' && *rp <= '9'){
        knum = atoi(rp);
        while(*rp >= '0' && *rp <= '9'){
          rp++;
        }
        while(*rp > '\0' && *rp <= ' '){
          rp++;
        }
        if(*rp >= '0' && *rp <= '9'){
          unum = atoi(rp);
          while(*rp >= '0' && *rp <= '9'){
            rp++;
          }
          while(*rp > '\0' && *rp <= ' '){
            rp++;
          }
          if(*rp >= '0' && *rp <= '9'){
            mnum = atoi(rp);
            while(*rp >= '0' && *rp <= '9'){
              rp++;
            }
            while(*rp > '\0' && *rp <= ' '){
              rp++;
            }
          }
        }
      }
      if(knum < 1) knum = ESTSMLRKNUM;
      if(unum < 1) unum = ESTSMLRUNUM;
      if(mnum < 1) mnum = ESTSMLRMNUM;
      svmap = est_phrase_vector(rp);
      scores = est_search_similar(db, svmap, &snum, knum, unum, mnum, cond->tfidf,
                                  cond->order ? ESTSMLRNMIN : 0.0, cond->auxmin, cond->auxwords);
      cbmapclose(svmap);
    } else if(cbstrfwmatch(cond->phrase, ESTOPRANK)){
      rp = cond->phrase + strlen(ESTOPRANK);
      while(*rp > '\0' && *rp <= ' '){
        rp++;
      }
      top = atoi(rp);
      while((*rp >= '0' && *rp <= '9') || *rp == '-'){
        rp++;
      }
      while(*rp > '\0' && *rp <= ' '){
        rp++;
      }
      scores = est_search_rank(db, rp, top, &snum);
    } else {
      switch(cond->pmode){
      default:
        terms = est_phrase_terms(cond->phrase);
        break;
      case ESTPMSIMPLE:
        tmp = est_phrase_from_simple(cond->phrase);
        terms = est_phrase_terms(tmp);
        free(tmp);
        break;
      case ESTPMROUGH:
        tmp = est_phrase_from_rough(cond->phrase);
        terms = est_phrase_terms(tmp);
        free(tmp);
        break;
      case ESTPMUNION:
        tmp = est_phrase_from_union(cond->phrase);
        terms = est_phrase_terms(tmp);
        free(tmp);
        break;
      case ESTPMISECT:
        tmp = est_phrase_from_isect(cond->phrase);
        terms = est_phrase_terms(tmp);
        free(tmp);
        break;
      }
      pcnum = 0;
      ncnum = 0;
      add = TRUE;
      for(i = 0; i < CB_LISTNUM(terms); i++){
        term = CB_LISTVAL(terms, i);
        if(!strcmp(term, ESTOPISECT)){
          add = TRUE;
        } else if(!strcmp(term, ESTOPDIFF)){
          add = FALSE;
        } else {
          if(!strcmp(term, ESTOPUVSET)){
            tscores = est_search_uvset(db, &tsnum, hints, add);
          } else {
            tscores = est_search_union(db, term, cond->gstep, cond->cbxpn, &tsnum, hints, add,
                                       add && !cond->order ? cond->auxmin : -1, cond->auxwords);
          }
          if(add){
            if(db->smode != ESTDFSCASIS){
              if(cond->tfidf){
                tune = pow(tsnum + 64, 0.4);
                for(j = 0; j < tsnum; j++){
                  tscores[j].score *= 100.0 / tune;
                }
              } else {
                for(j = 0; j < tsnum; j++){
                  tscores[j].score *= 10;
                }
              }
            }
            pcnum++;
          } else {
            ncnum++;
          }
          if(scores){
            CB_REALLOC(scores, (snum + tsnum) * sizeof(ESTSCORE) + 1);
            for(j = 0; j < tsnum; j++){
              scores[snum+j].id = tscores[j].id;
              scores[snum+j].score = add ? tscores[j].score : -1;
              scores[snum+j].value = NULL;
            }
            snum += tsnum;
            free(tscores);
          } else {
            scores = tscores;
            snum = tsnum;
          }
        }
      }
      if(scores){
        if(pcnum > 1 || ncnum > 0){
          qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_id_asc);
          nnum = 0;
          for(i = 0; i < snum; i++){
            id = scores[i].id;
            score = scores[i].score;
            hnum = score >= 0 ? 1 : 0;
            for(j = i + 1; j < snum && scores[j].id == id; j++){
              if(score >= 0 && scores[j].score >= 0){
                if(db->smode != ESTDFSCASIS) score += scores[j].score;
                hnum++;
              } else {
                score = -1;
              }
            }
            if(score >= 0 && hnum >= pcnum){
              scores[nnum].id = id;
              scores[nnum].score = score;
              scores[nnum].value = NULL;
              nnum++;
            }
            i = j - 1;
          }
          snum = nnum;
        }
      } else {
        CB_MALLOC(scores, 1);
        snum = 0;
      }
      CB_LISTCLOSE(terms);
    }
  } else if(cond->attrs){
    if(nscores && nsnum < ESTAISNUMMIN * 4){
      CB_MALLOC(scores, nsnum * sizeof(ESTSCORE) + 1);
      nnum = 0;
      for(i = 0; i < nsnum; i++){
        sprintf(numbuf, "%d", nscores[i]);
        if((id = est_db_uri_to_id(db, numbuf)) > 0){
          scores[nnum].id = id;
          scores[nnum].score = nscores[i];
          scores[nnum].value = NULL;
          nnum++;
        }
      }
      snum = nnum;
      nscores = NULL;
      nsnum = -1;
    } else {
      scores = NULL;
      for(i = 0; i < CB_LISTNUM(cond->attrs); i++){
        if((scores = est_search_aidx_attr(db, CB_LISTVAL(cond->attrs, i), &snum)) != NULL){
          ign = i;
          break;
        }
      }
      if(!scores) scores = est_search_uvset(db, &snum, hints, TRUE);
    }
  } else {
    CB_MALLOC(scores, 1);
    snum = 0;
  }
  if(CB_LISTNUM(db->pdocs) > 0) scores = est_search_pidxs(db, cond, scores, &snum, ordattrs);
  if(nscores && cond->phrase && cond->phrase[0] != '\0'){
    qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_score_desc);
    nnum = 0;
    j = 0;
    for(i = 0; i < snum; i++){
      id = scores[i].id;
      score = scores[i].score;
      while(j < nsnum && nscores[j] > score){
        j++;
      }
      if(j < nsnum && nscores[j] == score){
        scores[nnum].id = id;
        scores[nnum].score = score;
        scores[nnum].value = NULL;
        nnum++;
        j++;
      }
    }
    snum = nnum;
  }
  if(cbmaprnum(db->outcc) > 0){
    tsnum = 0;
    for(i = 0; i < snum; i++){
      len = sprintf(numbuf, "\t%d", scores[i].id);
      if(cbmapget(db->outcc, numbuf, len, NULL)) continue;
      scores[tsnum++] = scores[i];
    }
    snum = tsnum;
  }
  if(cond->max > 0 && cond->max * ESTATTRALW + 1 < snum && cond->attrs &&
     !cond->order && !cond->distinct){
    qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_score_desc);
    nnum = est_narrow_scores(db, cond->attrs, ign, cond->order, cond->distinct, scores, snum,
                             cond->max * ESTATTRALW + 1, &rest, ordattrs);
    if(hints){
      sprintf(numbuf, "%d",
              rest > cond->max / 2 ? (int)(snum * (nnum / (double)(snum - rest))) : nnum);
      cbmapput(hints, "", 0, numbuf, -1, TRUE);
    }
    snum = nnum;
  } else {
    if(cond->attrs || cond->order || cond->distinct)
      snum = est_narrow_scores(db, cond->attrs, ign, cond->order, cond->distinct, scores, snum,
                               INT_MAX, &rest, ordattrs);
    if(!cond->order) qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_score_desc);
    if(hints){
      sprintf(numbuf, "%d", snum);
      cbmapput(hints, "", 0, numbuf, -1, TRUE);
    }
  }
  if(cond->shadows) cbmapclose(cond->shadows);
  if(cond->ecllim >= 0.0){
    cond->shadows = cbmapopenex(snum + 1);
    snum = est_eclipse_scores(db, scores, snum, cond->max > 0 ? cond->max : snum,
                              ESTECLKNUM, cond->tfidf, cond->ecllim, cond->shadows);
  } else {
    cond->shadows = NULL;
  }
  rnum = snum - cond->skip;
  if(rnum < 0) rnum = 0;
  if(cond->max >= 0 && cond->max < rnum) rnum = cond->max;
  CB_MALLOC(rval, rnum * sizeof(int) + 1);
  tscores = scores + cond->skip;
  for(i = 0; i < rnum; i++){
    rval[i] = tscores[i].id;
  }
  if(cond->scfb){
    if(rnum > 0){
      CB_REALLOC(cond->scores, rnum * sizeof(int) + 1);
      for(i = 0; i < rnum; i++){
        cond->scores[i] = tscores[i].score;
      }
      cond->snum = rnum;
    } else {
      free(cond->scores);
      cond->scores = NULL;
      cond->snum = 0;
    }
  }
  *nump = rnum;
  if(*nump < 1) est_set_ecode(&(db->ecode), ESTENOITEM, __LINE__);
  cbmapclose(ordattrs);
  free(scores);
  return rval;
}


/* Search documents of plural databases. */
int *est_db_search_meta(ESTDB **dbs, int dbnum, ESTCOND *cond, int *nump, CBMAP *hints){
  ESTMETASCORE *scores, *tscores;
  ESTCOND *tcond;
  CBMAP *thints, *umap;
  const char *kbuf, *otype, *rp;
  char *distinct, numbuf[ESTNUMBUFSIZ], *oname, *wp, *vbuf;
  int i, j, max, skip, smax, snum, *res, rnum, ksiz, num;
  time_t tval;
  assert(dbs && dbnum >= 0 && cond && nump);
  max = cond->max;
  if(cond->distinct) cond->max = -1;
  skip = cond->skip;
  cond->skip = 0;
  distinct = cond->distinct;
  cond->distinct = NULL;
  smax = ESTALLOCUNIT;
  CB_MALLOC(scores, smax * sizeof(ESTMETASCORE));
  snum = 0;
  for(i = 0; i < dbnum; i++){
    if(cond->mask & (1 << i)) continue;
    tcond = est_cond_dup(cond);
    est_cond_set_options(tcond, ESTCONDSCFB);
    thints = cbmapopenex(ESTMINIBNUM);
    res = est_db_search(dbs[i], tcond, &rnum, thints);
    for(j = 0; j < rnum; j++){
      if(snum >= smax){
        smax *= 2;
        CB_REALLOC(scores, smax * sizeof(ESTMETASCORE));
      }
      scores[snum].db = i;
      scores[snum].id = res[j];
      scores[snum].score = est_cond_score(tcond, j);
      scores[snum].value = NULL;
      snum++;
    }
    if(hints){
      cbmapiterinit(thints);
      while((kbuf = cbmapiternext(thints, &ksiz)) != NULL){
        num = atoi(cbmapiterval(kbuf, NULL));
        if((rp = cbmapget(hints, kbuf, ksiz, NULL)) != NULL) num += atoi(rp);
        sprintf(numbuf, "%d", num);
        cbmapput(hints, kbuf, ksiz, numbuf, -1, TRUE);
      }
    }
    free(res);
    cbmapclose(thints);
    est_cond_delete(tcond);
  }
  oname = NULL;
  otype = NULL;
  if(cond->order){
    oname = cbmemdup(cond->order, -1);
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
  }
  if(oname){
    if(!cbstricmp(oname, ESTORDIDA)){
      qsort(scores, snum, sizeof(ESTMETASCORE), est_metascore_compare_by_id_asc);
    } else if(!cbstricmp(oname, ESTORDIDD)){
      qsort(scores, snum, sizeof(ESTMETASCORE), est_metascore_compare_by_id_desc);
    } else if(!cbstricmp(oname, ESTORDSCA)){
      qsort(scores, snum, sizeof(ESTMETASCORE), est_metascore_compare_by_score_asc);
    } else if(!cbstricmp(oname, ESTORDSCD)){
      qsort(scores, snum, sizeof(ESTMETASCORE), est_metascore_compare_by_score_desc);
    } else {
      for(i = 0; i < snum; i++){
        scores[i].value = est_db_get_doc_attr(dbs[scores[i].db], scores[i].id, oname);
        if(!scores[i].value) scores[i].value = cbmemdup("", 0);
      }
      if(!cbstricmp(otype, ESTORDSTRA)){
        qsort(scores, snum, sizeof(ESTMETASCORE), est_metascore_compare_by_str_asc);
      } else if(!cbstricmp(otype, ESTORDSTRD)){
        qsort(scores, snum, sizeof(ESTMETASCORE), est_metascore_compare_by_str_desc);
      } else if(!cbstricmp(otype, ESTORDNUMA)){
        for(i = 0; i < snum; i++){
          tval = cbstrmktime(scores[i].value);
          free(scores[i].value);
          scores[i].value = (void *)tval;
        }
        qsort(scores, snum, sizeof(ESTMETASCORE), est_metascore_compare_by_num_asc);
        for(i = 0; i < snum; i++){
          scores[i].value = NULL;
        }
      } else if(!cbstricmp(otype, ESTORDNUMD)){
        for(i = 0; i < snum; i++){
          tval = cbstrmktime(scores[i].value);
          free(scores[i].value);
          scores[i].value = (void *)tval;
        }
        qsort(scores, snum, sizeof(ESTMETASCORE), est_metascore_compare_by_num_desc);
        for(i = 0; i < snum; i++){
          scores[i].value = NULL;
        }
      }
      for(i = 0; i < snum; i++){
        free(scores[i].value);
      }
    }
    free(oname);
  } else {
    qsort(scores, snum, sizeof(ESTMETASCORE), est_metascore_compare_by_score_desc);
  }
  if(distinct){
    umap = cbmapopenex(snum + 1);
    rnum = 0;
    for(i = 0; i < snum; i++){
      vbuf = est_db_get_doc_attr(dbs[scores[i].db], scores[i].id, distinct);
      if(!vbuf) vbuf = cbmemdup("", 0);
      if(cbmapput(umap, vbuf, -1, "", 0, FALSE)) scores[rnum++] = scores[i];
      free(vbuf);
    }
    snum = rnum;
    cbmapclose(umap);
  }
  rnum = snum - skip;
  if(rnum < 0) rnum = 0;
  if(cond->max >= 0 && cond->max < rnum) rnum = cond->max;
  CB_MALLOC(res, rnum * sizeof(int) * 2 + 1);
  tscores = scores + skip;
  for(i = 0; i < rnum; i++){
    res[i*2] = tscores[i].db;
    res[i*2+1] = tscores[i].id;
  }
  if(cond->scfb){
    if(rnum > 0){
      CB_REALLOC(cond->scores, rnum * sizeof(int) + 1);
      for(i = 0; i < rnum; i++){
        cond->scores[i] = tscores[i].score;
      }
      cond->snum = rnum;
    } else {
      free(cond->scores);
      cond->scores = NULL;
      cond->snum = 0;
    }
  }
  *nump = rnum * 2;
  free(scores);
  cond->max = max;
  cond->skip = skip;
  cond->distinct = distinct;
  return res;
}


/* Check whether a document object matches the phrase of a search condition object definitely. */
int est_db_scan_doc(ESTDB *db, ESTDOC *doc, ESTCOND *cond){
  struct { char *word; int num; } wsets[ESTSCANWNUM];
  CBLIST *terms, *words;
  const char *term, *text;
  unsigned char *rbuf;
  char *tmp;
  int i, j, k, wsnum, add, rsiz, hit;
  assert(db && doc && cond);
  if(!cond->phrase || cbstrfwmatch(cond->phrase, ESTOPSIMILAR) ||
     cbstrfwmatch(cond->phrase, ESTOPID) || cbstrfwmatch(cond->phrase, ESTOPURI)) return FALSE;
  if(!doc->dtexts) CB_LISTOPEN(doc->dtexts);
  switch(cond->pmode){
  default:
    terms = est_phrase_terms(cond->phrase);
    break;
  case ESTPMSIMPLE:
    tmp = est_phrase_from_simple(cond->phrase);
    terms = est_phrase_terms(tmp);
    free(tmp);
    break;
  case ESTPMROUGH:
    tmp = est_phrase_from_rough(cond->phrase);
    terms = est_phrase_terms(tmp);
    free(tmp);
    break;
  case ESTPMUNION:
    tmp = est_phrase_from_union(cond->phrase);
    terms = est_phrase_terms(tmp);
    free(tmp);
    break;
  case ESTPMISECT:
    tmp = est_phrase_from_isect(cond->phrase);
    terms = est_phrase_terms(tmp);
    free(tmp);
    break;
  }
  wsnum = 0;
  add = TRUE;
  for(i = 0; i < CB_LISTNUM(terms); i++){
    term = CB_LISTVAL(terms, i);
    if(!strcmp(term, ESTOPISECT)){
      add = TRUE;
    } else if(!strcmp(term, ESTOPDIFF)){
      add = FALSE;
    } else if(add && strcmp(term, ESTOPUVSET)){
      if(term[0] == ' '){
        term++;
        if(term[0] == 'b'){
          term++;
        } else  if(term[0] == 'e'){
          term++;
        }
      }
      words = cbsplit(term, -1, "\t");
      while(wsnum < ESTSCANWNUM && CB_LISTNUM(words) > 0){
        wsets[wsnum].word = cblistshift(words, NULL);
        wsets[wsnum].num = i;
        wsnum++;
      }
      CB_LISTCLOSE(words);
    }
  }
  for(i = -1; i < CB_LISTNUM(doc->dtexts); i++){
    if(i < 0){
      if(!doc->attrs || !(text = cbmapget(doc->attrs, "", 0, NULL))) continue;
    } else {
      text = CB_LISTVAL(doc->dtexts, i);
    }
    rbuf = (unsigned char *)est_uconv_in(text, strlen(text), &rsiz);
    est_canonicalize_text(rbuf, rsiz, FALSE);
    tmp = est_uconv_out((char *)rbuf, rsiz, &rsiz);
    for(j = 0; j < wsnum; j++){
      if(!wsets[j].word) continue;
      if(est_strstr_sparse(tmp, wsets[j].word)){
        for(k = 0; k < wsnum; k++){
          if(!wsets[k].word) continue;
          if(wsets[k].num == wsets[j].num){
            free(wsets[k].word);
            wsets[k].word = NULL;
          }
        }
      }
    }
    free(tmp);
    free(rbuf);
  }
  hit = TRUE;
  for(i = 0; i < wsnum; i++){
    if(!wsets[i].word) continue;
    free(wsets[i].word);
    hit = FALSE;
  }
  CB_LISTCLOSE(terms);
  return hit;
}


/* Set the maximum size of the cache memory of a database. */
void est_db_set_cache_size(ESTDB *db, size_t size, int anum, int tnum, int rnum){
  assert(db);
  if(dpwritable(db->metadb) && size >= 0) db->icmax = size;
  if(anum >= 0) db->acmnum = anum;
  if(tnum >= 0) db->tcmnum = tnum;
  if(rnum >= 0) db->rcmnum = rnum;
  db->vcmnum = db->acmnum / 2;
}


/* Add a pseudo index directory to a database. */
int est_db_add_pseudo_index(ESTDB *db, const char *path){
  CBLIST *files;
  const char *file;
  char pbuf[ESTPATHBUFSIZ];
  int i, len;
  assert(db && path);
  if(!(files = cbdirlist(path))){
    est_set_ecode(&(db->ecode), ESTEINVAL, __LINE__);
    return FALSE;
  }
  cblistsort(files);
  for(i = 0; i < CB_LISTNUM(files); i++){
    file = CB_LISTVAL(files, i);
    if(!strcmp(file, ESTCDIRSTR) || !strcmp(file, ESTPDIRSTR)) continue;
    len = sprintf(pbuf, "%s%c%s", path, ESTPATHCHR, file);
    CB_LISTPUSH(db->pdocs, pbuf, len);
  }
  CB_LISTCLOSE(files);
  if(db->puris){
    cbmapclose(db->puris);
    db->puris = NULL;
  }
  return TRUE;
}



/*************************************************************************************************
 * features for experts
 *************************************************************************************************/


/* Handle to the file of random number generator. */
FILE *est_random_ifp = NULL;


/* POSIX signal handlers. */
void (*est_signal_handlers[ESTSIGNUM])(int);


/* Break a sentence of text and extract words. */
void est_break_text(const char *text, CBLIST *list, int norm, int tail){
  CBLIST *words;
  const unsigned char *word, *next;
  unsigned char *utext;
  char *tmp;
  int i, j, k, size, cc, wsiz, nsiz, tsiz;
  assert(text && list);
  utext = (unsigned char *)est_uconv_in(text, strlen(text), &size);
  if(norm) est_normalize_text(utext, size, &size);
  est_canonicalize_text(utext, size, FALSE);
  CB_LISTOPEN(words);
  for(i = 0; i < size; i += 2){
    cc = est_char_category(utext[i] * 0x100 + utext[i+1]);
    for(j = i + 2; j < size; j += 2){
      if(est_char_category(utext[j] * 0x100 + utext[j+1]) != cc) break;
    }
    switch(cc){
    case ESTDELIMCHR:
    case ESTWESTALPH:
      CB_LISTPUSH(words, (char *)(utext + i), j - i);
      break;
    case ESTEASTALPH:
      for(k = i; k < j; k += 2){
        if(j - k >= 4){
          CB_LISTPUSH(words, (char *)(utext + k), 4);
        } else {
          CB_LISTPUSH(words, (char *)(utext + k), 2);
        }
      }
      break;
    default:
      break;
    }
    i = j - 2;
  }
  for(i = 0; i < CB_LISTNUM(words); i++){
    word = (unsigned char *)CB_LISTVAL2(words, i, wsiz);
    if(est_char_category(word[0] * 0x100 + word[1]) == ESTEASTALPH && wsiz == 2 &&
       i < CB_LISTNUM(words) - 1){
      next = (unsigned char *)cblistval(words, i + 1, &nsiz);
      if(nsiz > 4) nsiz = 4;
      if(est_char_category(next[0] * 0x100 + next[1]) == ESTEASTALPH && nsiz > 2) nsiz = 2;
      CB_MALLOC(tmp, wsiz + nsiz + 1);
      memcpy(tmp, word, wsiz);
      memcpy(tmp + wsiz, next, nsiz);
      cblistover(words, i, tmp, wsiz + nsiz);
      free(tmp);
    }
  }
  for(i = 0; i < CB_LISTNUM(words); i++){
    word = (unsigned char *)CB_LISTVAL2(words, i, wsiz);
    if(!tail && wsiz == 2 && i == CB_LISTNUM(words) - 1){
      if(est_char_category(word[0] * 0x100 + word[1]) == ESTEASTALPH) continue;
    }
    tmp = est_uconv_out((char *)word, wsiz, &tsiz);
    CB_LISTPUSHBUF(list, tmp, tsiz);
  }
  CB_LISTCLOSE(words);
  free(utext);
}


/* Break a sentence of text and extract words using perfect N-gram analyzer. */
void est_break_text_perfng(const char *text, CBLIST *list, int norm, int tail){
  CBLIST *words;
  const unsigned char *word, *next;
  unsigned char *utext;
  char *tmp;
  int i, j, k, size, cc, wsiz, nsiz, tsiz;
  assert(text && list);
  utext = (unsigned char *)est_uconv_in(text, strlen(text), &size);
  if(norm) est_normalize_text(utext, size, &size);
  est_canonicalize_text(utext, size, FALSE);
  CB_LISTOPEN(words);
  for(i = 0; i < size; i += 2){
    cc = est_char_category_perfng(utext[i] * 0x100 + utext[i+1]);
    for(j = i + 2; j < size; j += 2){
      if(est_char_category_perfng(utext[j] * 0x100 + utext[j+1]) != cc) break;
    }
    switch(cc){
    case ESTEASTALPH:
      for(k = i; k < j; k += 2){
        if(j - k >= 4){
          CB_LISTPUSH(words, (char *)(utext + k), 4);
        } else {
          CB_LISTPUSH(words, (char *)(utext + k), 2);
        }
      }
      break;
    default:
      break;
    }
    i = j - 2;
  }
  for(i = 0; i < CB_LISTNUM(words); i++){
    word = (unsigned char *)CB_LISTVAL2(words, i, wsiz);
    if(est_char_category_perfng(word[0] * 0x100 + word[1]) == ESTEASTALPH && wsiz == 2 &&
       i < CB_LISTNUM(words) - 1){
      next = (unsigned char *)cblistval(words, i + 1, &nsiz);
      if(nsiz > 4) nsiz = 4;
      if(est_char_category_perfng(next[0] * 0x100 + next[1]) == ESTEASTALPH && nsiz > 2) nsiz = 2;
      CB_MALLOC(tmp, wsiz + nsiz + 1);
      memcpy(tmp, word, wsiz);
      memcpy(tmp + wsiz, next, nsiz);
      cblistover(words, i, tmp, wsiz + nsiz);
      free(tmp);
    }
  }
  for(i = 0; i < CB_LISTNUM(words); i++){
    word = (unsigned char *)CB_LISTVAL2(words, i, wsiz);
    if(!tail && wsiz == 2 && i == CB_LISTNUM(words) - 1){
      if(est_char_category_perfng(word[0] * 0x100 + word[1]) == ESTEASTALPH) continue;
    }
    tmp = est_uconv_out((char *)word, wsiz, &tsiz);
    CB_LISTPUSHBUF(list, tmp, tsiz);
  }
  CB_LISTCLOSE(words);
  free(utext);
}


/* Make a snippet of an arbitrary string. */
char *est_str_make_snippet(const char *str, const CBLIST *words,
                           int wwidth, int hwidth, int awidth){
  assert(str && words && wwidth >= 0 && hwidth >= 0 && awidth >= 0);
  return est_make_snippet(str, strlen(str), words, wwidth, hwidth, awidth);
}


/* Break a sentence of text and extract words, using character category analyzer.
   `text' specifies a sentence of text.
   `list' specifies a list object to which extract words are added.
   `norm' specifies whether to normalize the text. */
void est_break_text_chrcat(const char *text, CBLIST *list, int norm){
  unsigned char *utext;
  char *tmp;
  int i, j, size, cc, tsiz;
  assert(text && list);
  utext = (unsigned char *)est_uconv_in(text, strlen(text), &size);
  if(norm) est_normalize_text(utext, size, &size);
  est_canonicalize_text(utext, size, FALSE);
  for(i = 0; i < size; i += 2){
    cc = est_char_category_chrcat(utext[i] * 0x100 + utext[i+1]);
    for(j = i + 2; j < size; j += 2){
      if(est_char_category_chrcat(utext[j] * 0x100 + utext[j+1]) != cc &&
         (cc != ESTWESTALPH || utext[j] != 0x00 || utext[j+1] != 0x2d) &&
         (cc != ESTHIRAGANA || utext[j] != 0x30 || utext[j+1] != 0xfc)) break;
    }
    if(cc != ESTSPACECHR){
      tmp = est_uconv_out((char *)(utext + i), j - i, &tsiz);
      CB_LISTPUSHBUF(list, tmp, tsiz);
    }
    i = j - 2;
  }
  free(utext);
}


/* Convert the character encoding of a string. */
char *est_iconv(const char *ptr, int size,
                const char *icode, const char *ocode, int *sp, int *mp){
  iconv_t ic;
  char *obuf, *wp, *rp;
  size_t isiz, osiz;
  int miss;
  assert(ptr && icode && ocode);
  if(size < 0) size = strlen(ptr);
  if(icode[0] == 'x' && icode[1] == '-'){
    if(!cbstricmp(icode, "x-sjis")){
      icode = "Shift_JIS";
    } else if(!cbstricmp(icode, "x-ujis")){
      icode = "EUC-JP";
    } else if(!cbstricmp(icode, "x-euc-jp")){
      icode = "EUC-JP";
    }
  } else if(icode[0] == 'w' || icode[0] == 'W'){
    if(!cbstricmp(icode, "windows-31j")){
      icode = "CP932";
    }
  }
  if(ocode[0] == 'x' && ocode[1] == '-'){
    if(!cbstricmp(ocode, "x-sjis")){
      ocode = "Shift_JIS";
    } else if(!cbstricmp(ocode, "x-ujis")){
      ocode = "EUC-JP";
    } else if(!cbstricmp(ocode, "x-euc-jp")){
      ocode = "EUC-JP";
    }
  } else if(ocode[0] == 'w' || ocode[0] == 'W'){
    if(!cbstricmp(ocode, "windows-31j")){
      ocode = "CP932";
    }
  }
  if((ic = iconv_open(ocode, icode)) == (iconv_t)-1) return NULL;
  isiz = size;
  osiz = isiz * 5;
  CB_MALLOC(obuf, osiz + 1);
  wp = obuf;
  rp = (char *)ptr;
  miss = 0;
  while(isiz > 0){
    if(iconv(ic, (void *)&rp, &isiz, &wp, &osiz) == -1){
      if(errno == EILSEQ && (*rp == 0x5c || *rp == 0x7e)){
        *wp = *rp;
        wp++;
        rp++;
        isiz--;
      } else if(errno == EILSEQ || errno == EINVAL){
        rp++;
        isiz--;
        miss++;
      } else {
        break;
      }
    }
  }
  *wp = '\0';
  if(sp) *sp = wp - obuf;
  if(mp) *mp = miss;
  if(iconv_close(ic) == -1){
    free(obuf);
    return NULL;
  }
  return obuf;
}


/* Detect the encoding of a string automatically. */
const char *est_enc_name(const char *ptr, int size, int plang){
  const char *hypo;
  int i, lim, miss, ascii, cr;
  assert(ptr);
  if(size < 0) size = strlen(ptr);
  if(size > ESTICCHECKSIZ) size = ESTICCHECKSIZ;
  if(size >= 2 && (!memcmp(ptr, "\xfe\xff", 2) || !memcmp(ptr, "\xff\xfe", 2))) return "UTF-16";
  ascii = TRUE;
  cr = FALSE;
  lim = size - 1;
  for(i = 0; i < lim; i += 2){
    if(ptr[i] == 0x0) return "UTF-16BE";
    if(ptr[i+1] == 0x0) return "UTF-16LE";
    if(ptr[i] < 0x0 || ptr[i] == 0x1b){
      ascii = FALSE;
    } else if(ptr[i] == 0xd){
      cr = TRUE;
    }
  }
  if(ascii) return "US-ASCII";
  switch(plang){
  case ESTLANGEN:
    if(est_enc_miss(ptr, size, "UTF-8", "UTF-16BE") < 1) return "UTF-8";
    return "ISO-8859-1";
  case ESTLANGJA:
    lim = size - 3;
    for(i = 0; i < lim; i++){
      if(ptr[i] == 0x1b){
        i++;
        if(ptr[i] == '(' && strchr("BJHI", ptr[i+1])) return "ISO-2022-JP";
        if(ptr[i] == '$' && strchr("@B(", ptr[i+1])) return "ISO-2022-JP";
      }
    }
    if(est_enc_miss(ptr, size, "UTF-8", "UTF-16BE") < 1) return "UTF-8";
    hypo = NULL;
    if(cr){
      if((miss = est_enc_miss(ptr, size, "Shift_JIS", "EUC-JP")) < 1) return "Shift_JIS";
      if(!hypo && miss / (double)size <= ESTICALLWRAT) hypo = "Shift_JIS";
      if((miss = est_enc_miss(ptr, size, "EUC-JP", "UTF-16BE")) < 1) return "EUC-JP";
      if(!hypo && miss / (double)size <= ESTICALLWRAT) hypo = "EUC-JP";
    } else {
      if((miss = est_enc_miss(ptr, size, "EUC-JP", "UTF-16BE")) < 1) return "EUC-JP";
      if(!hypo && miss / (double)size <= ESTICALLWRAT) hypo = "EUC-JP";
      if((miss = est_enc_miss(ptr, size, "Shift_JIS", "EUC-JP")) < 1) return "Shift_JIS";
      if(!hypo && miss / (double)size <= ESTICALLWRAT) hypo = "Shift_JIS";
    }
    if((miss = est_enc_miss(ptr, size, "UTF-8", "UTF-16BE")) < 1) return "UTF-8";
    if(!hypo && miss / (double)size <= ESTICALLWRAT) hypo = "UTF-8";
    if((miss = est_enc_miss(ptr, size, "CP932", "UTF-16BE")) < 1) return "CP932";
    if(!hypo && miss / (double)size <= ESTICALLWRAT) hypo = "CP932";
    return hypo ? hypo : "ISO-8859-1";
  case ESTLANGZH:
    if(est_enc_miss(ptr, size, "UTF-8", "UTF-16BE") < 1) return "UTF-8";
    if(est_enc_miss(ptr, size, "EUC-CN", "UTF-16BE") < 1) return "EUC-CN";
    if(est_enc_miss(ptr, size, "BIG5", "UTF-16BE") < 1) return "BIG5";
    return "ISO-8859-1";
  case ESTLANGKO:
    if(est_enc_miss(ptr, size, "UTF-8", "UTF-16BE") < 1) return "UTF-8";
    if(est_enc_miss(ptr, size, "EUC-KR", "UTF-16BE") < 1) return "EUC-KR";
    return "ISO-8859-1";
  default:
    break;
  }
  return "ISO-8859-1";
}


/* Convert a UTF-8 string into UTF-16BE. */
char *est_uconv_in(const char *ptr, int size, int *sp){
  const unsigned char *rp;
  char *rbuf, *wp;
  assert(ptr && size >= 0 && sp);
  rp = (unsigned char *)ptr;
  CB_MALLOC(rbuf, size * 2 + 1);
  wp = rbuf;
  while(rp < (unsigned char *)ptr + size){
    if(*rp < 0x7f){
      *(wp++) = 0x00;
      *(wp++) = *rp;
      rp += 1;
    } else if(*rp < 0xdf){
      if(rp >= (unsigned char *)ptr + size - 1) break;
      *(wp++) = (rp[0] & 0x1f) >> 2;
      *(wp++) = (rp[0] << 6) | (rp[1] & 0x3f);
      rp += 2;
    } else if(*rp < 0xf0){
      if(rp >= (unsigned char *)ptr + size - 2) break;
      *(wp++) = (rp[0] << 4) | ((rp[1] & 0x3f) >> 2);
      *(wp++) = (rp[1] << 6) | (rp[2] & 0x3f);
      rp += 3;
    } else if(*rp < 0xf8){
      if(rp >= (unsigned char *)ptr + size - 3) break;
      *(wp++) = 0x00;
      *(wp++) = '?';
      rp += 4;
    } else if(*rp < 0xfb){
      if(rp >= (unsigned char *)ptr + size - 4) break;
      *(wp++) = 0x00;
      *(wp++) = '?';
      rp += 5;
    } else if(*rp < 0xfd){
      if(rp >= (unsigned char *)ptr + size - 5) break;
      *(wp++) = 0x00;
      *(wp++) = '?';
      rp += 6;
    } else {
      break;
    }
  }
  *wp = '\0';
  *sp = wp - rbuf;
  return rbuf;
}


/* Convert a UTF-16BE string into UTF-8. */
char *est_uconv_out(const char *ptr, int size, int *sp){
  const unsigned char *rp;
  char *rbuf, *wp;
  int c;
  assert(ptr && size >= 0);
  if(size % 2 != 0) size--;
  rp = (unsigned char *)ptr;
  CB_MALLOC(rbuf, size * 2 + 1);
  wp = rbuf;
  while(rp < (unsigned char *)ptr + size){
    c = rp[0] * 0x100 + rp[1];
    if(c < 0x0080){
      *(wp++) = rp[1];
    } else if(c < 0x0900){
      *(wp++) = 0xc0 | (rp[0] << 2) | ((rp[1] >> 6) & 0x03);
      *(wp++) = 0x80 | (rp[1] & 0x3f);
    } else {
      *(wp++) = 0xe0 | ((rp[0] >> 4) & 0x0f);
      *(wp++) = 0x80 | ((rp[0] & 0x0f) << 2) | ((rp[1] >> 6) & 0x03);
      *(wp++) = 0x80 | (rp[1] & 0x3f);
    }
    rp += 2;
  }
  *wp = '\0';
  if(sp) *sp = wp - rbuf;
  return rbuf;
}


/* Compress a serial object with ZLIB. */
char *est_deflate(const char *ptr, int size, int *sp, int mode){
#if ESTUSEZLIB
  z_stream zs;
  char *buf;
  unsigned char obuf[ESTIOBUFSIZ];
  int rv, asiz, bsiz, osiz;
  if(size < 0) size = strlen(ptr);
  zs.zalloc = Z_NULL;
  zs.zfree = Z_NULL;
  zs.opaque = Z_NULL;
  switch(mode){
  case -1:
    if(deflateInit2(&zs, 5, Z_DEFLATED, -15, 7, Z_DEFAULT_STRATEGY) != Z_OK)
      return NULL;
    break;
  case 1:
    if(deflateInit2(&zs, 6, Z_DEFLATED, 15 + 16, 9, Z_DEFAULT_STRATEGY) != Z_OK)
      return NULL;
    break;
  default:
    if(deflateInit2(&zs, 6, Z_DEFLATED, 15, 8, Z_DEFAULT_STRATEGY) != Z_OK)
      return NULL;
    break;
  }
  asiz = size + 16;
  if(asiz < ESTIOBUFSIZ) asiz = ESTIOBUFSIZ;
  CB_MALLOC(buf, asiz);
  bsiz = 0;
  zs.next_in = (unsigned char *)ptr;
  zs.avail_in = size;
  zs.next_out = obuf;
  zs.avail_out = ESTIOBUFSIZ;
  while((rv = deflate(&zs, Z_FINISH)) == Z_OK){
    osiz = ESTIOBUFSIZ - zs.avail_out;
    if(bsiz + osiz > asiz){
      asiz = asiz * 2 + osiz;
      CB_REALLOC(buf, asiz);
    }
    memcpy(buf + bsiz, obuf, osiz);
    bsiz += osiz;
    zs.next_out = obuf;
    zs.avail_out = ESTIOBUFSIZ;
  }
  if(rv != Z_STREAM_END){
    free(buf);
    deflateEnd(&zs);
    return NULL;
  }
  osiz = ESTIOBUFSIZ - zs.avail_out;
  if(bsiz + osiz + 1 > asiz){
    asiz = asiz * 2 + osiz;
    CB_REALLOC(buf, asiz);
  }
  memcpy(buf + bsiz, obuf, osiz);
  bsiz += osiz;
  buf[bsiz] = '\0';
  if(mode == -1) bsiz++;
  *sp = bsiz;
  deflateEnd(&zs);
  return buf;
#else
  if(size < 0) size = strlen(ptr);
  *sp = size;
  return cbmemdup(ptr, size);
#endif
}


/* Decompress a serial object compressed with ZLIB. */
char *est_inflate(const char *ptr, int size, int *sp, int mode){
#if ESTUSEZLIB
  z_stream zs;
  char *buf;
  unsigned char obuf[ESTIOBUFSIZ];
  int rv, asiz, bsiz, osiz;
  zs.zalloc = Z_NULL;
  zs.zfree = Z_NULL;
  zs.opaque = Z_NULL;
  switch(mode){
  case -1:
    if(inflateInit2(&zs, -15) != Z_OK) return NULL;
    break;
  case 1:
    if(inflateInit2(&zs, 15 + 16) != Z_OK) return NULL;
    break;
  default:
    if(inflateInit2(&zs, 15) != Z_OK) return NULL;
    break;
  }
  asiz = size * 2 + 16;
  if(asiz < ESTIOBUFSIZ) asiz = ESTIOBUFSIZ;
  CB_MALLOC(buf, asiz);
  bsiz = 0;
  zs.next_in = (unsigned char *)ptr;
  zs.avail_in = size;
  zs.next_out = obuf;
  zs.avail_out = ESTIOBUFSIZ;
  while((rv = inflate(&zs, Z_NO_FLUSH)) == Z_OK){
    osiz = ESTIOBUFSIZ - zs.avail_out;
    if(bsiz + osiz >= asiz){
      asiz = asiz * 2 + osiz;
      CB_REALLOC(buf, asiz);
    }
    memcpy(buf + bsiz, obuf, osiz);
    bsiz += osiz;
    zs.next_out = obuf;
    zs.avail_out = ESTIOBUFSIZ;
  }
  if(rv != Z_STREAM_END){
    free(buf);
    inflateEnd(&zs);
    return NULL;
  }
  osiz = ESTIOBUFSIZ - zs.avail_out;
  if(bsiz + osiz >= asiz){
    asiz = asiz * 2 + osiz;
    CB_REALLOC(buf, asiz);
  }
  memcpy(buf + bsiz, obuf, osiz);
  bsiz += osiz;
  buf[bsiz] = '\0';
  if(sp) *sp = bsiz;
  inflateEnd(&zs);
  return buf;
#else
  if(sp) *sp = size;
  return cbmemdup(ptr, size);
#endif
}


/* Compress a serial object with LZO. */
char *est_lzoencode(const char *ptr, int size, int *sp){
#if ESTUSELZO
  char wrkmem[LZO1X_1_MEM_COMPRESS];
  lzo_bytep buf;
  lzo_uint bsiz;
  if(size < 0) size = strlen(ptr);
  CB_MALLOC(buf, size + size / 16 + 80);
  if(lzo1x_1_compress((lzo_bytep)ptr, size, buf, &bsiz, wrkmem) != LZO_E_OK){
    free(buf);
    return NULL;
  }
  buf[bsiz] = '\0';
  *sp = bsiz;
  return (char *)buf;
#else
  if(size < 0) size = strlen(ptr);
  *sp = size;
  return cbmemdup(ptr, size);
#endif
}


/* Decompress a serial object compressed with LZO. */
char *est_lzodecode(const char *ptr, int size, int *sp){
#if ESTUSELZO
  lzo_bytep buf;
  lzo_uint bsiz;
  int rat, rv;
  rat = 4;
  while(TRUE){
    bsiz = (size + 256) * rat + 3;
    CB_MALLOC(buf, bsiz + 1);
    rv = lzo1x_decompress_safe((lzo_bytep)(ptr), size, buf, &bsiz, NULL);
    if(rv == LZO_E_OK){
      break;
    } else if(rv == LZO_E_OUTPUT_OVERRUN){
      free(buf);
      rat *= 2;
    } else {
      free(buf);
      return NULL;
    }
  }
  buf[bsiz] = '\0';
  if(sp) *sp = bsiz;
  return (char *)buf;
#else
  if(sp) *sp = size;
  return cbmemdup(ptr, size);
#endif
}


/* Compress a serial object with BZIP2. */
char *est_bzencode(const char *ptr, int size, int *sp){
#if ESTUSEBZIP
  bz_stream zs;
  char *buf, obuf[ESTIOBUFSIZ];
  int rv, asiz, bsiz, osiz;
  if(size < 0) size = strlen(ptr);
  zs.bzalloc = NULL;
  zs.bzfree = NULL;
  zs.opaque = NULL;
  if(BZ2_bzCompressInit(&zs, 9, 0, 30) != BZ_OK) return NULL;
  asiz = size + 16;
  if(asiz < ESTIOBUFSIZ) asiz = ESTIOBUFSIZ;
  CB_MALLOC(buf, asiz);
  bsiz = 0;
  zs.next_in = (char *)ptr;
  zs.avail_in = size;
  zs.next_out = obuf;
  zs.avail_out = ESTIOBUFSIZ;
  while((rv = BZ2_bzCompress(&zs, BZ_FINISH)) == BZ_FINISH_OK){
    osiz = ESTIOBUFSIZ - zs.avail_out;
    if(bsiz + osiz > asiz){
      asiz = asiz * 2 + osiz;
      CB_REALLOC(buf, asiz);
    }
    memcpy(buf + bsiz, obuf, osiz);
    bsiz += osiz;
    zs.next_out = obuf;
    zs.avail_out = ESTIOBUFSIZ;
  }
  if(rv != BZ_STREAM_END){
    free(buf);
    BZ2_bzCompressEnd(&zs);
    return NULL;
  }
  osiz = ESTIOBUFSIZ - zs.avail_out;
  if(bsiz + osiz + 1 > asiz){
    asiz = asiz * 2 + osiz;
    CB_REALLOC(buf, asiz);
  }
  memcpy(buf + bsiz, obuf, osiz);
  bsiz += osiz;
  buf[bsiz] = '\0';
  *sp = bsiz;
  BZ2_bzCompressEnd(&zs);
  return buf;
#else
  if(size < 0) size = strlen(ptr);
  *sp = size;
  return cbmemdup(ptr, size);
#endif
}


/* Decompress a serial object compressed with BZIP2. */
char *est_bzdecode(const char *ptr, int size, int *sp){
#if ESTUSEBZIP
  bz_stream zs;
  char *buf, obuf[ESTIOBUFSIZ];
  int rv, asiz, bsiz, osiz;
  zs.bzalloc = NULL;
  zs.bzfree = NULL;
  zs.opaque = NULL;
  if(BZ2_bzDecompressInit(&zs, 0, 0) != BZ_OK) return NULL;
  asiz = size * 2 + 16;
  if(asiz < ESTIOBUFSIZ) asiz = ESTIOBUFSIZ;
  CB_MALLOC(buf, asiz);
  bsiz = 0;
  zs.next_in = (char *)ptr;
  zs.avail_in = size;
  zs.next_out = obuf;
  zs.avail_out = ESTIOBUFSIZ;
  while((rv = BZ2_bzDecompress(&zs)) == BZ_OK){
    osiz = ESTIOBUFSIZ - zs.avail_out;
    if(bsiz + osiz >= asiz){
      asiz = asiz * 2 + osiz;
      CB_REALLOC(buf, asiz);
    }
    memcpy(buf + bsiz, obuf, osiz);
    bsiz += osiz;
    zs.next_out = obuf;
    zs.avail_out = ESTIOBUFSIZ;
  }
  if(rv != BZ_STREAM_END){
    free(buf);
    BZ2_bzDecompressEnd(&zs);
    return NULL;
  }
  osiz = ESTIOBUFSIZ - zs.avail_out;
  if(bsiz + osiz >= asiz){
    asiz = asiz * 2 + osiz;
    CB_REALLOC(buf, asiz);
  }
  memcpy(buf + bsiz, obuf, osiz);
  bsiz += osiz;
  buf[bsiz] = '\0';
  if(sp) *sp = bsiz;
  BZ2_bzDecompressEnd(&zs);
  return buf;
#else
  if(sp) *sp = size;
  return cbmemdup(ptr, size);
#endif
}


/* Get the border string for draft data of documents. */
const char *est_border_str(void){
  static int first = TRUE;
  static char border[ESTPATHBUFSIZ];
  int t, p;
  if(first){
    t = (int)(time(NULL) + est_random() * INT_MAX);
    p = (int)(getpid() + est_random() * INT_MAX);
    sprintf(border, "--------[%08X%08X]--------",
            dpouterhash((char *)&t, sizeof(int)), dpouterhash((char *)&p, sizeof(int)));
    first = FALSE;
  }
  return border;
}


/* Get the real random number. */
double est_random(void){
  static int first = TRUE;
  int num;
  if(first && !est_random_ifp){
    if((est_random_ifp = fopen("/dev/urandom", "rb")) != NULL){
      atexit(est_random_fclose);
    } else {
      srand(getpid());
    }
    first = FALSE;
  }
  if(est_random_ifp){
    fread(&num, sizeof(int), 1, est_random_ifp);
    return (num & INT_MAX) / (double)INT_MAX;
  }
  return rand() / (double)RAND_MAX;
}


/* Get the random number in normal distribution. */
double est_random_nd(void){
  double d;
  d = (sqrt(-2 * log(1.0 - est_random())) * cos(3.1415926535 * 2 * est_random()) + 6.0) / 12.0;
  if(d > 1.0) d = 1.0;
  if(d < 0.0) d = 0.0;
  return d;
}


/* Get an MD5 hash string of a key string. */
char *est_make_crypt(const char *key){
  md5_state_t ms;
  char digest[32], str[64], *wp;
  int i;
  assert(key);
  md5_init(&ms);
  md5_append(&ms, (md5_byte_t *)key, strlen(key));
  md5_finish(&ms, (md5_byte_t *)digest);
  wp = str;
  for(i = 0; i < 16; i++){
    wp += sprintf(wp, "%02x", ((unsigned char *)digest)[i]);
  }
  return cbmemdup(str, -1);
}


/* Check whether a key matches an MD5 hash string. */
int est_match_crypt(const char *key, const char *hash){
  char *khash;
  int rv;
  assert(key && hash);
  khash = est_make_crypt(key);
  rv = !strcmp(khash, hash);
  free(khash);
  return rv;
}


/* Create a regular expression object. */
void *est_regex_new(const char *str){
  regex_t regex;
  int options;
  assert(str);
  options = REG_EXTENDED | REG_NOSUB;
  if(str[0] == '*' && str[1] == 'I' && str[2] == ':'){
    options |= REG_ICASE;
    str += 3;
  }
  if(regcomp(&regex, str, options) != 0) return NULL;
  return cbmemdup((char *)&regex, sizeof(regex_t));
}


/* Delete a regular expression object. */
void est_regex_delete(void *regex){
  assert(regex);
  regfree(regex);
  free(regex);
}


/* Check whether a regular expression matches a string. */
int est_regex_match(const void *regex, const char *str){
  assert(regex && str);
  return regexec(regex, str, 0, NULL, 0) == 0;
}


/* Check whether a regular expression matches a string. */
int est_regex_match_str(const char *rstr, const char *tstr){
  void *regex;
  int rv;
  assert(rstr && tstr);
  if(!(regex = est_regex_new(rstr))) return FALSE;
  rv = est_regex_match(regex, tstr);
  est_regex_delete(regex);
  return rv;
}


/* Replace each substring matching a regular expression string. */
char *est_regex_replace(const char *str, const char *bef, const char *aft){
  regex_t regex;
  regmatch_t subs[256];
  CBDATUM *datum;
  const char *sp, *rp;
  int options, first, num;
  assert(str && bef && aft);
  options = REG_EXTENDED;
  if(bef[0] == '*' && bef[1] == 'I' && bef[2] == ':'){
    options |= REG_ICASE;
    bef += 3;
  }
  if(bef[0] == '\0' || regcomp(&regex, bef, options) != 0) return cbmemdup(str, -1);
  if(regexec(&regex, str, ESTREGSUBMAX, subs, 0) != 0){
    regfree(&regex);
    return cbmemdup(str, -1);
  }
  sp = str;
  CB_DATUMOPEN(datum);
  first = TRUE;
  while(sp[0] != '\0' && regexec(&regex, sp, 10, subs, first ? 0 : REG_NOTBOL) == 0){
    first = FALSE;
    if(subs[0].rm_so == -1) break;
    CB_DATUMCAT(datum, sp, subs[0].rm_so);
    for(rp = aft; *rp != '\0'; rp++){
      if(*rp == '\\'){
        if(rp[1] >= '0' && rp[1] <= '9'){
          num = rp[1] - '0';
          if(subs[num].rm_so != -1 && subs[num].rm_eo != -1)
            CB_DATUMCAT(datum, sp + subs[num].rm_so, subs[num].rm_eo - subs[num].rm_so);
          ++rp;
        } else if(rp[1] != '\0'){
          CB_DATUMCAT(datum, ++rp, 1);
        }
      } else if(*rp == '&'){
        CB_DATUMCAT(datum, sp + subs[0].rm_so, subs[0].rm_eo - subs[0].rm_so);
      } else {
        CB_DATUMCAT(datum, rp, 1);
      }
    }
    sp += subs[0].rm_eo;
    if(subs[0].rm_eo < 1) break;
  }
  CB_DATUMCAT(datum, sp, strlen(sp));
  regfree(&regex);
  return cbdatumtomalloc(datum, NULL);
}


/* Duplicate a document object. */
ESTDOC *est_doc_dup(ESTDOC *doc){
  ESTDOC *ndoc;
  assert(doc);
  CB_MALLOC(ndoc, sizeof(ESTDOC));
  ndoc->id = doc->id;
  ndoc->attrs = doc->attrs ? cbmapdup(doc->attrs) : NULL;
  ndoc->dtexts = doc->dtexts ? cblistdup(doc->dtexts) : NULL;
  ndoc->kwords = doc->kwords ? cbmapdup(doc->kwords) : NULL;
  return ndoc;
}


/* Set the ID number of a document object. */
void est_doc_set_id(ESTDOC *doc, int id){
  assert(doc);
  doc->id = id;
}


/* Get the hidden texts of a document object. */
const char *est_doc_hidden_texts(ESTDOC *doc){
  const char *rv;
  assert(doc);
  rv = doc->attrs ? cbmapget(doc->attrs, "", 0, NULL) : NULL;
  return rv ? rv : "";
}


/* Reduce the texts to fit to the specified size. */
void est_doc_slim(ESTDOC *doc, int len){
  const char *vbuf;
  unsigned char *tbuf;
  int i, vsiz, tsiz;
  assert(doc && len >= 0);
  if(!doc->dtexts) return;
  if(doc->attrs && cbmapget(doc->attrs, "", 0, &vsiz)) len -= vsiz;
  for(i = 0; i < CB_LISTNUM(doc->dtexts); i++){
    vbuf = CB_LISTVAL2(doc->dtexts, i, vsiz);
    len -= vsiz;
    if(len < 0){
      tbuf = (unsigned char *)cbmemdup(vbuf, vsiz);
      tsiz = vsiz > -len ? vsiz + len : 0;
      if(tsiz > 0){
        while(tsiz < vsiz){
          if(tbuf[tsiz] <= ' ' || (tbuf[tsiz] & 0xf0) == 0xe0) break;
          tsiz++;
        }
      }
      while(CB_LISTNUM(doc->dtexts) > i){
        CB_LISTDROP(doc->dtexts);
      }
      CB_LISTPUSHBUF(doc->dtexts, (char *)tbuf, tsiz);
      break;
    }
  }
}


/* Check whether a docuemnt object is empty. */
int est_doc_is_empty(ESTDOC *doc){
  assert(doc);
  if((!doc->dtexts || CB_LISTNUM(doc->dtexts) < 1) &&
     (!doc->attrs || !cbmapget(doc->attrs, "", 0, NULL))) return TRUE;
  return FALSE;
}


/* Duplicate a condition object. */
ESTCOND *est_cond_dup(ESTCOND *cond){
  ESTCOND *ncond;
  assert(cond);
  CB_MALLOC(ncond, sizeof(ESTCOND));
  ncond->phrase = cond->phrase ? cbmemdup(cond->phrase, -1) : NULL;
  ncond->gstep = cond->gstep;
  ncond->tfidf = cond->tfidf;
  ncond->pmode = cond->pmode;
  ncond->cbxpn = cond->cbxpn;
  ncond->attrs = cond->attrs ? cblistdup(cond->attrs) : NULL;
  ncond->order = cond->order ? cbmemdup(cond->order, -1) : NULL;
  ncond->max = cond->max;
  ncond->skip = cond->skip;
  ncond->auxmin = cond->auxmin;
  ncond->auxwords = cond->auxwords ? cbmapdup(cond->auxwords) : NULL;
  ncond->scfb = cond->scfb;
  ncond->scores = cond->scores ?
    (int *)cbmemdup((char *)cond->scores, cond->snum * sizeof(int)) : NULL;
  ncond->snum = cond->snum;
  ncond->nscores = cond->nscores;
  ncond->nsnum = cond->nsnum;
  ncond->opts = cond->opts;
  ncond->ecllim = cond->ecllim;
  ncond->shadows = cond->shadows ? cbmapdup(cond->shadows) : NULL;
  ncond->distinct = cond->distinct ? cbmemdup(cond->distinct, -1) : NULL;
  ncond->mask = cond->mask;
  return ncond;
}


/* Get the phrase of a condition object. */
const char *est_cond_phrase(ESTCOND *cond){
  assert(cond);
  return cond->phrase;
}


/* Get a list object of attribute expressions of a condition object. */
const CBLIST *est_cond_attrs(ESTCOND *cond){
  assert(cond);
  return cond->attrs;
}


/* Get the order expression of a condition object. */
const char *est_cond_order(ESTCOND *cond){
  assert(cond);
  return cond->order;
}


/* Get the maximum number of retrieval of a condition object. */
int est_cond_max(ESTCOND *cond){
  assert(cond);
  return cond->max;
}


/* Get the number of skipped documents of a condition object. */
int est_cond_skip(ESTCOND *cond){
  assert(cond);
  return cond->skip;
}


/* Get the options of a condition object. */
int est_cond_options(ESTCOND *cond){
  assert(cond);
  return cond->opts;
}


/* Get permission to adopt result of the auxiliary index. */
int est_cond_auxiliary(ESTCOND *cond){
  assert(cond);
  return cond->auxmin;
}


/* Get the attribute distinction filter. */
const char *est_cond_distinct(ESTCOND *cond){
  assert(cond);
  return cond->distinct;
}


/* Get the mask of targets of meta search. */
int est_cond_mask(ESTCOND *cond){
  assert(cond);
  return cond->mask;
}


/* Get the score of a document corresponding to a condition object. */
int est_cond_score(ESTCOND *cond, int index){
  assert(cond);
  if(!cond->scores || index < 0 || index >= cond->snum) return -1;
  return cond->scores[index];
}


/* Get the score array of corresponding documents of a condition object. */
const int *est_cond_scores(ESTCOND *cond, int *nump){
  assert(cond && nump);
  *nump = cond->snum;
  return cond->scores;
}


/* Set the narrowing scores of a condition object. */
void est_cond_set_narrowing_scores(ESTCOND *cond, const int *scores, int num){
  assert(cond && scores && num >= 0);
  cond->nscores = scores;
  cond->nsnum = num;
}


/* Check whether a condition object has used the auxiliary index. */
int est_cond_auxiliary_word(ESTCOND *cond, const char *word){
  assert(cond && word);
  if(!cond->auxwords) return FALSE;
  if(word[0] != '\0') return cbmapget(cond->auxwords, word, -1, NULL) != NULL;
  return cbmaprnum(cond->auxwords) > 0;
}


/* Get an array of ID numbers of eclipsed docuemnts of a document in a condition object. */
const int *est_cond_shadows(ESTCOND *cond, int id, int *np){
  const char *vbuf;
  int vsiz;
  assert(cond && id > 0 && np);
  if(!cond->shadows || !(vbuf = cbmapget(cond->shadows, (char *)&id, sizeof(int), &vsiz))){
    *np = 0;
    return (int *)"";
  }
  *np = vsiz / sizeof(int);
  return (int *)vbuf;
}


/* Set the callback function for query expansion. */
void est_cond_set_expander(ESTCOND *cond, void (*func)(const char *, CBLIST *)){
  assert(cond && func);
  cond->cbxpn = func;
}


/* Set the error code of a database. */
void est_db_set_ecode(ESTDB *db, int ecode){
  assert(db);
  est_set_ecode(&(db->ecode), ecode, __LINE__);
}


/* Check whether an option is set. */
int est_db_check_option(ESTDB *db, int option){
  assert(db);
  switch(option){
  case ESTDBREADER:
    return !dpwritable(db->metadb);
  case ESTDBWRITER:
    return dpwritable(db->metadb);
  case ESTDBCREAT:
    return -1;
  case ESTDBTRUNC:
    return -1;
  case ESTDBNOLCK:
    return -1;
  case ESTDBLCKNB:
    return -1;
  case ESTDBPERFNG:
    return db->amode == ESTDFPERFNG;
  case ESTDBCHRCAT:
    return db->amode == ESTDFCHRCAT;
  case ESTDBSMALL:
    return -1;
  case ESTDBLARGE:
    return -1;
  case ESTDBHUGE:
    return -1;
  case ESTDBHUGE2:
    return -1;
  case ESTDBHUGE3:
    return -1;
  case ESTDBSCVOID:
    return db->smode == ESTDFSCVOID;
  case ESTDBSCINT:
    return db->smode == ESTDFSCINT;
  case ESTDBSCASIS:
    return db->smode == ESTDFSCASIS;
  default:
    break;
  }
  return -1;
}


/* Get the inode number of a database. */
int est_db_inode(ESTDB *db){
  assert(db);
  return db->inode;
}


/* Set the entity data of a document in a database. */
int est_db_set_doc_entity(ESTDB *db, int id, const char *ptr, int size){
  int err;
  assert(db && id > 0);
  if(!dpwritable(db->metadb)){
    est_set_ecode(&(db->ecode), ESTEACCES, __LINE__);
    return FALSE;
  }
  err = FALSE;
  if(ptr){
    if(!crputlob(db->textdb, (char *)&id, sizeof(int), ptr, size, CR_DOVER)){
      est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
      err = TRUE;
    }
  } else {
    if(!croutlob(db->textdb, (char *)&id, sizeof(int))){
      est_set_ecode(&(db->ecode), dpecode == DP_ENOITEM ? ESTENOITEM : ESTEDB, __LINE__);
      err = TRUE;
    }
  }
  return err ? FALSE : TRUE;
}


/* Set the maximum number of expansion of wild cards. */
void est_db_set_wildmax(ESTDB *db, int num){
  assert(db && num >= 0);
  db->wildmax = num;
}


/* Get the entity data of a document in a database. */
char *est_db_get_doc_entity(ESTDB *db, int id, int *sp){
  char *ptr;
  assert(db && id > 0 && sp);
  if(!(ptr = crgetlob(db->textdb, (char *)&id, sizeof(int), 0, -1, sp))){
    est_set_ecode(&(db->ecode), dpecode == DP_ENOITEM ? ESTENOITEM : ESTEDB, __LINE__);
    return NULL;
  }
  return ptr;
}


/* Add a piece of meta data to a database. */
void est_db_add_meta(ESTDB *db, const char *name, const char *value){
  assert(db && name);
  if(!dpwritable(db->metadb)){
    est_set_ecode(&(db->ecode), ESTEACCES, __LINE__);
    return;
  }
  if(!db->metacc) est_db_prepare_meta(db);
  if(value){
    cbmapput(db->metacc, name, -1, value, -1, TRUE);
  } else {
    cbmapout(db->metacc, name, -1);
  }
}


/* Get a list of names of meta data of a database. */
CBLIST *est_db_meta_names(ESTDB *db){
  assert(db);
  if(!db->metacc) est_db_prepare_meta(db);
  return cbmapkeys(db->metacc);
}


/* Get the value of a piece of meta data of a database. */
char *est_db_meta(ESTDB *db, const char *name){
  const char *vbuf;
  int vsiz;
  assert(db && name);
  if(!db->metacc) est_db_prepare_meta(db);
  if(!(vbuf = cbmapget(db->metacc, name, -1, &vsiz))) return NULL;
  return cbmemdup(vbuf, vsiz);
}


/* Extract keywords of a document object. */
CBMAP *est_db_etch_doc(ESTDB *db, ESTDOC *doc, int max){
  ESTKEYSC *scores;
  CBMAP *keys, *umap;
  CBLIST *words;
  const char *text, *word, *vbuf;
  const unsigned char *uword;
  char numbuf[ESTNUMBUFSIZ];
  int i, wsiz, num, smax, snum, vsiz;
  assert(doc && max >= 0);
  if(!doc->dtexts) return cbmapopenex(1);
  keys = cbmapopenex(max * 2 + 1);
  CB_LISTOPEN(words);
  for(i = -1; i < CB_LISTNUM(doc->dtexts); i++){
    if(i < 0){
      if(!doc->attrs || !(text = cbmapget(doc->attrs, "", 0, NULL))) continue;
    } else {
      text = CB_LISTVAL(doc->dtexts, i);
    }
    if(db){
      switch(db->amode){
      case ESTDFPERFNG:
        est_break_text_perfng(text, words, FALSE, FALSE);
        break;
      case ESTDFCHRCAT:
        est_break_text_chrcat(text, words, FALSE);
        break;
      default:
        est_break_text(text, words, FALSE, FALSE);
        break;
      }
    } else {
      est_break_text(text, words, FALSE, FALSE);
    }
  }
  umap = cbmapopenex(CB_LISTNUM(words) + 1);
  for(i = 0; i < CB_LISTNUM(words); i++){
    word = CB_LISTVAL2(words, i, wsiz);
    if(wsiz > ESTWORDMAXLEN) continue;
    num = (vbuf = cbmapget(umap, word, wsiz, NULL)) ? *(int *)vbuf + 1 : 1;
    cbmapput(umap, word, wsiz, (char *)&num, sizeof(int), TRUE);
  }
  CB_MALLOC(scores, cbmaprnum(umap) * sizeof(ESTKEYSC) + 1);
  snum = 0;
  cbmapiterinit(umap);
  while((uword = (unsigned char *)cbmapiternext(umap, &wsiz)) != NULL){
    scores[snum].word = (char *)uword;
    scores[snum].wsiz = wsiz;
    scores[snum].pt = (vbuf = cbmapiterval((char *)uword, NULL)) ? *(int *)vbuf : 0;
    if(uword[0] >= 0xe3){
      if(wsiz <= 3){
        scores[snum].pt /= 2;
        if((uword[0] == 0xe3 && (uword[1] == 0x80 || uword[1] == 0x81 ||
                                 (uword[1] == 0x82 && uword[2] <= 0x9f))) ||
           (uword[0] == 0xef && uword[1] >= 0xbc)) scores[snum].pt /= 2;
      } else {
        if((uword[0] == 0xe3 && (uword[1] == 0x80 || uword[1] == 0x81 ||
                                 (uword[1] == 0x82 && uword[2] <= 0x9f))) ||
           (uword[0] == 0xef && uword[1] >= 0xbc)) scores[snum].pt /= 2;
        if((uword[3] == 0xe3 && (uword[4] == 0x80 || uword[4] == 0x81 ||
                                 (uword[4] == 0x82 && uword[5] <= 0x9f))) ||
           (uword[3] == 0xef && uword[4] >= 0xbc)) scores[snum].pt /= 2;
      }
    } else if((uword[0] > '\0' && uword[0] <= '/') || (uword[0] >= ':' && uword[0] <= '@') ||
              (uword[0] >= '[' && uword[0] <= '`') || (uword[0] >= '{' && uword[0] <= '~')){
      scores[snum].pt /= 25;
      if(wsiz <= 1) scores[snum].pt /= 2;
    } else {
      switch(wsiz){
      case 1:
        scores[snum].pt /= 9;
        break;
      case 2:
        scores[snum].pt /= 5;
        break;
      case 3:
        scores[snum].pt /= 3;
        break;
      case 4:
        scores[snum].pt /= 2;
        break;
      case 5:
        scores[snum].pt /= 1.5;
        break;
      case 6:
        scores[snum].pt /= 1.25;
        break;
      }
    }
    snum++;
  }
  qsort(scores, snum, sizeof(ESTKEYSC), est_keysc_compare);
  smax = max * (db ? ESTKEYSCALW : 1) + 1;
  snum = snum > smax ? smax : snum;
  if(db){
    for(i = 0; i < snum; i++){
      if((vbuf = cbmapget(db->keycc, scores[i].word, scores[i].wsiz, NULL)) != NULL){
        cbmapmove(db->keycc, scores[i].word, scores[i].wsiz, FALSE);
        vsiz = *(int *)vbuf;
      } else {
        if(db->dfdb){
          if((vsiz = dpgetwb(db->dfdb, scores[i].word, scores[i].wsiz,
                             0, ESTNUMBUFSIZ - 1, numbuf)) > 0){
            numbuf[vsiz] = '\0';
            vsiz = atoi(numbuf);
          } else {
            vsiz = 0;
          }
        } else {
          vsiz = est_idx_vsiz(db->idxdb, scores[i].word, scores[i].wsiz);
          if(cbmapget(db->idxcc, scores[i].word, scores[i].wsiz, &num)) vsiz += num;
        }
        cbmapput(db->keycc, scores[i].word, scores[i].wsiz, (char *)&vsiz, sizeof(int), FALSE);
      }
      scores[i].pt *= 100000.0 / pow(vsiz + 64, 0.6);
    }
    if(db->kcmnum >= 0 && cbmaprnum(db->keycc) > db->kcmnum){
      num = db->kcmnum * 0.1 + 1;
      cbmapiterinit(db->keycc);
      for(i = 0; i < num && (word = cbmapiternext(db->keycc, &wsiz)) != NULL; i++){
        cbmapout(db->keycc, word, wsiz);
      }
    }
    qsort(scores, snum, sizeof(ESTKEYSC), est_keysc_compare);
  }
  for(i = 0; i < snum && i < max; i++){
    vsiz = sprintf(numbuf, "%d", scores[i].pt > 0 ? scores[i].pt : 1);
    cbmapput(keys, scores[i].word, scores[i].wsiz, numbuf, vsiz, FALSE);
  }
  free(scores);
  cbmapclose(umap);
  CB_LISTCLOSE(words);
  return keys;
}


/* Retrieve a map object of keywords. */
int est_db_put_keywords(ESTDB *db, int id, CBMAP *kwords, double weight){
  const char *kbuf;
  char *mbuf;
  int err, ksiz, pair[2], msiz;
  assert(db && id > 0 && kwords && weight >= 0.0);
  if(!dpwritable(db->metadb)){
    est_set_ecode(&(db->ecode), ESTEACCES, __LINE__);
    return FALSE;
  }
  if(crvsiz(db->attrdb, (char *)&id, sizeof(int)) == -1){
    est_set_ecode(&(db->ecode), ESTENOITEM, __LINE__);
    return FALSE;
  }
  err = FALSE;
  if(!est_db_out_keywords(db, id) && db->ecode != ESTENOITEM) err = TRUE;
  pair[0] = id;
  cbmapiterinit(kwords);
  while((kbuf = cbmapiternext(kwords, &ksiz)) != NULL){
    if(ksiz < 1 || (kbuf[0] >= '\0' && kbuf[0] <= ' ')) continue;
    pair[1] = (int)(atoi(cbmapiterval(kbuf, NULL)) * weight);
    cbmapputcat(db->auxcc, kbuf, ksiz, (char *)pair, sizeof(pair));
    db->icsiz += sizeof(pair);
  }
  mbuf = cbmapdump(kwords, &msiz);
  if(!est_crput(db->kwddb, db->zmode, id, mbuf, msiz, CR_DOVER)){
    est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
    db->fatal = TRUE;
    err = TRUE;
  }
  free(mbuf);
  return err ? FALSE : TRUE;
}


/* Remove keywords of a document. */
int est_db_out_keywords(ESTDB *db, int id){
  CBMAP *kwords;
  const char *word;
  char wbuf[ESTWORDMAXLEN+3], *tbuf;
  int wsiz;
  assert(db && id > 0);
  if(!dpwritable(db->metadb)){
    est_set_ecode(&(db->ecode), ESTEACCES, __LINE__);
    return FALSE;
  }
  if(id >= ESTPDOCIDMIN){
    est_set_ecode(&(db->ecode), ESTEINVAL, __LINE__);
    return FALSE;
  }
  if((kwords = est_db_get_keywords(db, id)) != NULL){
    cbmapiterinit(kwords);
    while((word = cbmapiternext(kwords, &wsiz)) != NULL){
      if(wsiz > ESTWORDMAXLEN){
        tbuf = cbsprintf(" %s", word);
        cbmapput(db->outcc, tbuf, wsiz + 1, "", 0, FALSE);
        free(tbuf);
      } else {
        sprintf(wbuf, " %s", word);
        cbmapput(db->outcc, wbuf, wsiz + 1, "", 0, FALSE);
      }
    }
    cbmapclose(kwords);
  }
  if(!est_crout(db->kwddb, id)){
    if(dpecode == DP_ENOITEM){
      est_set_ecode(&(db->ecode), ESTENOITEM, __LINE__);
    } else {
      est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
      db->fatal = TRUE;
    }
    return FALSE;
  }
  cbmapout(db->veccc, (char *)&id, sizeof(int));
  return TRUE;
}


/* Retrieve a map object of keywords. */
CBMAP *est_db_get_keywords(ESTDB *db, int id){
  CBMAP *kwords;
  const char *cbuf;
  char *mbuf;
  int i, csiz, msiz, num;
  assert(db && id > 0);
  if((cbuf = cbmapget(db->veccc, (char *)&id, sizeof(int), &csiz)) != NULL){
    cbmapmove(db->veccc, (char *)&id, sizeof(int), FALSE);
    return cbmapload(cbuf, csiz);
  }
  if(!(mbuf = est_crget(db->kwddb, db->zmode, id, &msiz))){
    est_set_ecode(&(db->ecode), ESTENOITEM, __LINE__);
    return NULL;
  }
  kwords = cbmapload(mbuf, msiz);
  if(db->vcmnum > 0) cbmapput(db->veccc, (char *)&id, sizeof(int), mbuf, msiz, TRUE);
  free(mbuf);
  if(cbmaprnum(db->veccc) > db->vcmnum){
    num = cbmaprnum(db->veccc) * 0.1 + 1;
    cbmapiterinit(db->veccc);
    for(i = 0; i < num && (cbuf = cbmapiternext(db->veccc, NULL)) != NULL; i++){
      cbmapout(db->veccc, cbuf, sizeof(int));
    }
  }
  return kwords;
}


/* Mesure the total size of each inner records of a stored document. */
int est_db_measure_doc(ESTDB *db, int id, int parts){
  int sum, num;
  assert(db && id > 0);
  sum = 0;
  if((parts & ESTMDATTR) && (num = crvsiz(db->attrdb, (char *)&id, sizeof(int))) > 0) sum += num;
  if((parts & ESTMDTEXT) && (num = crvsiz(db->textdb, (char *)&id, sizeof(int))) > 0) sum += num;
  if((parts & ESTMDKWD) && (num = crvsiz(db->kwddb, (char *)&id, sizeof(int))) > 0) sum += num;
  return sum;
}


/* Initialize the iterator of a database. */
int est_db_iter_init(ESTDB *db, const char *prev){
  char *vbuf;
  assert(db);
  if(prev){
    if(!vlcurjump(db->listdb, prev, -1, VL_JFORWARD)) return dpecode == DP_ENOITEM;
    if((vbuf = vlcurkey(db->listdb, NULL)) != NULL){
      if(strcmp(prev, vbuf) >= 0) vlcurnext(db->listdb);
      free(vbuf);
    }
    return TRUE;
  }
  return vlcurfirst(db->listdb) || dpecode == DP_ENOITEM;
}


/* Get the next ID of the iterator of a database. */
int est_db_iter_next(ESTDB *db){
  char *vbuf;
  int id;
  assert(db);
  if(!(vbuf = vlcurval(db->listdb, NULL))){
    if(dpecode == DP_ENOITEM){
      est_set_ecode(&(db->ecode), ESTENOITEM, __LINE__);
      return 0;
    } else {
      est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
      db->fatal = TRUE;
      return -1;
    }
  }
  id = atoi(vbuf);
  free(vbuf);
  vlcurnext(db->listdb);
  return id;
}


/* Initialize the word iterator of a database. */
int est_db_word_iter_init(ESTDB *db){
  assert(db);
  if(!vlcurfirst(db->fwmdb) && dpecode != DP_ENOITEM){
    est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
    db->fatal = TRUE;
    return FALSE;
  }
  return TRUE;
}


/* Get the next word of the word iterator of a database. */
char *est_db_word_iter_next(ESTDB *db){
  char *word;
  assert(db);
  if(!(word = vlcurkey(db->fwmdb, NULL))){
    if(dpecode == DP_ENOITEM){
      est_set_ecode(&(db->ecode), ESTENOITEM, __LINE__);
    } else {
      est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
      db->fatal = TRUE;
    }
    return NULL;
  }
  vlcurnext(db->fwmdb);
  return word;
}


/* Get the size of the record of a word. */
int est_db_word_rec_size(ESTDB *db, const char *word){
  int num;
  assert(db && word);
  if(!cbmapget(db->idxcc, word, -1, &num)) num = 0;
  return est_idx_vsiz(db->idxdb, word, strlen(word)) + num;
}


/* Get the number of unique keywords in a database. */
int est_db_keyword_num(ESTDB *db){
  int wnum;
  assert(db);
  wnum = vlrnum(db->xfmdb);
  return wnum > 0 ? wnum : 0;
}


/* Initialize the keyword iterator of a database. */
int est_db_keyword_iter_init(ESTDB *db){
  assert(db);
  if(!vlcurfirst(db->xfmdb) && dpecode != DP_ENOITEM){
    est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
    db->fatal = TRUE;
    return FALSE;
  }
  return TRUE;
}


/* Get the next keyword of the word iterator of a database. */
char *est_db_keyword_iter_next(ESTDB *db){
  char *word;
  assert(db);
  if(!(word = vlcurkey(db->xfmdb, NULL))){
    if(dpecode == DP_ENOITEM){
      est_set_ecode(&(db->ecode), ESTENOITEM, __LINE__);
    } else {
      est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
      db->fatal = TRUE;
    }
    return NULL;
  }
  vlcurnext(db->xfmdb);
  return word;
}


/* Get the size of the record of a keyword. */
int est_db_keyword_rec_size(ESTDB *db, const char *word){
  const char *kbuf;
  assert(db && word);
  return (kbuf = vlgetcache(db->xfmdb, word, -1, NULL)) != NULL ? atoi(kbuf) : 0;
}


/* Search documents corresponding a keyword for a database. */
int *est_db_keyword_search(ESTDB *db, const char *word, int *nump){
  int i, *res, rnum;
  assert(db && word && nump);
  if(!(res = (int *)vlget(db->auxdb, word, -1, &rnum))){
    *nump = 0;
    CB_MALLOC(res, 1);
    return res;
  }
  rnum /= sizeof(int) * 2;
  for(i = 0; i < rnum; i++){
    res[i] = res[i*2];
  }
  *nump = rnum;
  return res;
}


/* Get the number of records in the cache memory of a database. */
int est_db_cache_num(ESTDB *db){
  assert(db);
  return cbmaprnum(db->idxcc);
}


/* Get the size of used cache region. */
int est_db_used_cache_size(ESTDB *db){
  assert(db);
  return (db->icsiz + (cbmaprnum(db->idxcc) + cbmaprnum(db->auxcc)) *
          (sizeof(CBMAPDATUM) + ESTWORDAVGLEN)) * ESTMEMIRATIO;
}


/* Set the special cache for narrowing and sorting with document attributes. */
void est_db_set_special_cache(ESTDB *db, const char *name, int num){
  assert(db && name && num >= 0);
  if(db->spacc){
    free(db->scname);
    cbmapclose(db->spacc);
  }
  db->spacc = cbmapopenex(num + 1);
  db->scmnum = num;
  db->scname = cbmemdup(name, -1);
}


/* Set the callback function for database events. */
void est_db_set_informer(ESTDB *db, void (*func)(const char *, void *), void *opaque){
  assert(db && func);
  db->infocb = func;
  db->infoop = opaque;
  est_db_inform(db, "status");
}


/* Fill the cache for keys for TF-IDF. */
void est_db_fill_key_cache(ESTDB *db){
  const char *kbuf;
  char *msg;
  int i, ksiz, vsiz;
  assert(db);
  vlcurfirst(db->fwmdb);
  for(i = 0; (kbuf = vlcurkeycache(db->fwmdb, &ksiz)) != NULL; i++){
    vsiz = est_idx_vsiz(db->idxdb, kbuf, ksiz);
    cbmapput(db->keycc, kbuf, ksiz, (char *)&vsiz, sizeof(int), TRUE);
    vlcurnext(db->fwmdb);
    if(i % ESTCCCBFREQ == 0){
      msg = cbsprintf("filling the key cache for TF-IDF (%d)", i + 1);
      est_db_inform(db, msg);
      free(msg);
    }
  }
  db->kcmnum = -1;
}


/* Set the database of document frequency. */
void est_db_set_dfdb(ESTDB *db, DEPOT *dfdb){
  assert(db);
  db->dfdb = dfdb;
}


/* Clear the result cache. */
void est_db_refresh_rescc(ESTDB *db){
  ESTSCORE sc;
  const char *word;
  int size;
  assert(db);
  sc.id = -1;
  sc.score = 0;
  sc.value = NULL;
  cbmapiterinit(db->rescc);
  while((word = cbmapiternext(db->rescc, &size)) != NULL){
    cbmapput(db->rescc, word, size, (char *)&sc, sizeof(ESTSCORE), TRUE);
  }
}


/* Charge the result cache. */
void est_db_charge_rescc(ESTDB *db, int max){
  CBLIST *words;
  const char *word, *vbuf;
  int i, num, size, vsiz;
  assert(db);
  if(max < 0) max = INT_MAX;
  CB_LISTOPEN(words);
  cbmapiterinit(db->rescc);
  while((word = cbmapiternext(db->rescc, &size)) != NULL){
    CB_MAPITERVAL(vbuf, word, vsiz);
    if(vsiz == sizeof(ESTSCORE) && ((ESTSCORE *)vbuf)->id == -1) CB_LISTPUSH(words, word, size);
  }
  num = CB_LISTNUM(words);
  for(i = 0; i < max && i < num; i++){
    word = cblistval(words, num - i - 1, &size);
    free(est_search_union(db, word, 1, NULL, &size, NULL, TRUE, -1, NULL));
  }
  CB_LISTCLOSE(words);
}


/* Get a list of words in the result cache. */
CBLIST *est_db_list_rescc(ESTDB *db){
  CBLIST *words;
  const char *word;
  int size;
  assert(db);
  CB_LISTOPEN(words);
  cbmapiterinit(db->rescc);
  while((word = cbmapiternext(db->rescc, &size)) != NULL){
    cblistunshift(words, word, size);
  }
  return words;
}


/* Get the number of pseudo documents in a database. */
int est_db_pseudo_doc_num(ESTDB *db){
  assert(db);
  return cblistnum(db->pdocs);
}


/* Get a list of expressions of attribute indexes of a database. */
CBLIST *est_db_attr_index_exprs(ESTDB *db){
  ESTATTRIDX *attridx;
  CBLIST *list;
  const char *kbuf;
  char *expr;
  assert(db);
  list = cblistopen();
  cbmapiterinit(db->aidxs);
  while((kbuf = cbmapiternext(db->aidxs, NULL)) != NULL){
    attridx = (ESTATTRIDX *)cbmapiterval(kbuf, NULL);
    switch(attridx->type){
    case ESTIDXATTRSTR:
      expr = cbsprintf("%s=str", kbuf);
      break;
    case ESTIDXATTRNUM:
      expr = cbsprintf("%s=num", kbuf);
      break;
    default:
      expr = cbsprintf("%s=seq", kbuf);
      break;
    }
    CB_LISTPUSHBUF(list, expr, strlen(expr));
  }
  return list;
}


/* Interrupt long time processing. */
void est_db_interrupt(ESTDB *db){
  assert(db);
  db->intflag = TRUE;
}


/* Repair a broken database directory. */
int est_db_repair(const char *name, int options, int *ecp){
  ESTDB *db;
  DEPOT *depot, *metadb;
  CURIA *curia, *attrdb, *textdb, *kwddb;
  VILLA *villa, *listdb;
  CBLIST *list;
  CBMAP *aidxs, *attrs;
  ESTATTRIDX attridx, *attridxp;
  void *aidxdb;
  const char *elem, *abuf;
  char path[ESTPATHBUFSIZ], *kbuf, vbuf[ESTNUMBUFSIZ], *dec, *mbuf;
  int i, err, idmax, flags, zmode, dnum, dseq, ksiz, vsiz, type, id, msiz, esiz, asiz;
  assert(name && ecp);
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTMETADBNAME);
  if(est_inode(path) == -1){
    est_set_ecode(ecp, ESTEIO, __LINE__);
    return FALSE;
  }
  if(!(options & ESTRPSTRICT) && (depot= dpopen(path, DP_OWRITER, -1)) != NULL){
    dpclose(depot);
  } else {
    dprepair(path);
  }
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTIDXDBNAME);
  if(est_inode(path) == -1){
    est_set_ecode(ecp, ESTEIO, __LINE__);
    return FALSE;
  }
  if((list = cbdirlist(path)) != NULL){
    for(i = 1; i < CB_LISTNUM(list); i++){
      elem = CB_LISTVAL(list, i);
      if(elem[0] < '0' || elem[0] > '9') continue;
      sprintf(path, "%s%c%s%c%s", name, ESTPATHCHR, ESTIDXDBNAME, ESTPATHCHR, elem);
      if(!(options & ESTRPSTRICT) && (villa = vlopen(path, VL_OWRITER, VL_CMPLEX)) != NULL){
        vlclose(villa);
      } else {
        vlrepair(path, VL_CMPLEX);
      }
    }
    CB_LISTCLOSE(list);
  }
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTFWMDBNAME);
  if(est_inode(path) == -1){
    est_set_ecode(ecp, ESTEIO, __LINE__);
    return FALSE;
  }
  if(!(options & ESTRPSTRICT) && (villa = vlopen(path, VL_OWRITER, VL_CMPLEX)) != NULL){
    vlclose(villa);
  } else {
    vlrepair(path, VL_CMPLEX);
  }
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTAUXDBNAME);
  if(est_inode(path) == -1){
    est_set_ecode(ecp, ESTEIO, __LINE__);
    return FALSE;
  }
  if(!(options & ESTRPSTRICT) && (villa = vlopen(path, VL_OWRITER, VL_CMPLEX)) != NULL){
    vlclose(villa);
  } else {
    vlrepair(path, VL_CMPLEX);
  }
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTXFMDBNAME);
  if(est_inode(path) == -1){
    est_set_ecode(ecp, ESTEIO, __LINE__);
    return FALSE;
  }
  if(!(options & ESTRPSTRICT) && (villa = vlopen(path, VL_OWRITER, VL_CMPLEX)) != NULL){
    vlclose(villa);
  } else {
    vlrepair(path, VL_CMPLEX);
  }
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTATTRDBNAME);
  if(est_inode(path) == -1){
    est_set_ecode(ecp, ESTEIO, __LINE__);
    return FALSE;
  }
  if(!(options & ESTRPSTRICT) && (curia = cropen(path, CR_OWRITER, -1, -1)) != NULL){
    crclose(curia);
  } else {
    crrepair(path);
  }
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTTEXTDBNAME);
  if(est_inode(path) == -1){
    est_set_ecode(ecp, ESTEIO, __LINE__);
    return FALSE;
  }
  if(!(options & ESTRPSTRICT) && (curia = cropen(path, CR_OWRITER, -1, -1)) != NULL){
    crclose(curia);
  } else {
    crrepair(path);
  }
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTKWDDBNAME);
  if(est_inode(path) == -1){
    est_set_ecode(ecp, ESTEIO, __LINE__);
    return FALSE;
  }
  if(!(options & ESTRPSTRICT) && (curia = cropen(path, CR_OWRITER, -1, -1)) != NULL){
    crclose(curia);
  } else {
    crrepair(path);
  }
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTLISTDBNAME);
  if(est_inode(path) == -1){
    est_set_ecode(ecp, ESTEIO, __LINE__);
    return FALSE;
  }
  if(!(options & ESTRPSTRICT) && (villa = vlopen(path, VL_OWRITER, VL_CMPLEX)) != NULL){
    vlclose(villa);
  } else {
    vlrepair(path, VL_CMPLEX);
  }
  if((list = cbdirlist(name)) != NULL){
    for(i = 0; i < CB_LISTNUM(list); i++){
      elem = CB_LISTVAL(list, i);
      if(cbstrfwmatch(elem, ESTAISEQPREF)){
        sprintf(path, "%s%c%s", name, ESTPATHCHR, elem);
        if(!(options & ESTRPSTRICT) && (depot = dpopen(path, DP_OWRITER, -1)) != NULL){
          dpclose(depot);
        } else {
          dprepair(path);
        }
      } else if(cbstrfwmatch(elem, ESTAISTRPREF)){
        sprintf(path, "%s%c%s", name, ESTPATHCHR, elem);
        if(!(options & ESTRPSTRICT) && (villa = vlopen(path, VL_OWRITER, VL_CMPLEX)) != NULL){
          vlclose(villa);
        } else {
          vlrepair(path, VL_CMPLEX);
        }
      } else if(cbstrfwmatch(elem, ESTAINUMPREF)){
        sprintf(path, "%s%c%s", name, ESTPATHCHR, elem);
        if(!(options & ESTRPSTRICT) && (villa = vlopen(path, VL_OWRITER, VL_CMPLEX)) != NULL){
          vlclose(villa);
        } else {
          vlrepair(path, VL_CMPLEX);
        }
      }
    }
    CB_LISTCLOSE(list);
  }
  if((options & ESTRPSHODDY) && (db = est_db_open(name, ESTDBWRITER, ecp)) != NULL){
    if(!est_db_close(db, ecp)) return FALSE;
    return TRUE;
  }
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTMETADBNAME);
  metadb = dpopen(path, DP_OWRITER, -1);
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTATTRDBNAME);
  attrdb = cropen(path, CR_OWRITER, -1, -1);
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTTEXTDBNAME);
  textdb = cropen(path, CR_OWRITER, -1, -1);
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTKWDDBNAME);
  kwddb = cropen(path, CR_OWRITER, -1, -1);
  sprintf(path, "%s%c%s", name, ESTPATHCHR, ESTLISTDBNAME);
  listdb = vlopen(path, VL_OWRITER, VL_CMPLEX);
  if(!attrdb || !textdb || !kwddb || !listdb){
    if(listdb) vlclose(listdb);
    if(kwddb) crclose(kwddb);
    if(textdb) crclose(textdb);
    if(attrdb) crclose(attrdb);
    if(metadb) dpclose(metadb);
    est_set_ecode(ecp, ESTEDB, __LINE__);
    return FALSE;
  }
  aidxs = cbmapopenex(ESTMINIBNUM);
  if((list = cbdirlist(name)) != NULL){
    for(i = 0; i < CB_LISTNUM(list); i++){
      elem = CB_LISTVAL(list, i);
      dec = NULL;
      type = -1;
      if(cbstrfwmatch(elem, ESTAISEQPREF)){
        dec = est_hex_decode(elem + strlen(ESTAISEQPREF));
        type = ESTIDXATTRSEQ;
      } else if(cbstrfwmatch(elem, ESTAISTRPREF)){
        dec = est_hex_decode(elem + strlen(ESTAISTRPREF));
        type = ESTIDXATTRSTR;
      } else if(cbstrfwmatch(elem, ESTAINUMPREF)){
        dec = est_hex_decode(elem + strlen(ESTAINUMPREF));
        type = ESTIDXATTRNUM;
      }
      if(dec){
        sprintf(path, "%s%c%s", name, ESTPATHCHR, elem);
        switch(type){
        case ESTIDXATTRSTR:
          if((aidxdb = vlopen(path, VL_OWRITER, VL_CMPLEX)) != NULL){
            vlsettuning(aidxdb, ESTAIDXLRM, ESTAIDXNIM, ESTAIDXLCN, ESTAIDXNCN);
            vlsetfbpsiz(aidxdb, ESTAIDXVLFBP);
            attridx.db = aidxdb;
            attridx.type = type;
            cbmapput(aidxs, dec, -1, (char *)&attridx, sizeof(ESTATTRIDX), FALSE);
          }
          break;
        case ESTIDXATTRNUM:
          if((aidxdb = vlopen(path, VL_OWRITER, est_aidx_numcmp)) != NULL){
            vlsettuning(aidxdb, ESTAIDXLRM, ESTAIDXNIM, ESTAIDXLCN, ESTAIDXNCN);
            vlsetfbpsiz(aidxdb, ESTAIDXVLFBP);
            attridx.db = aidxdb;
            attridx.type = type;
            cbmapput(aidxs, dec, -1, (char *)&attridx, sizeof(ESTATTRIDX), FALSE);
          }
          break;
        default:
          if((aidxdb = dpopen(path, DP_OWRITER, crbnum(attrdb) / ESTAIBDIAM)) != NULL){
            dpsetfbpsiz(aidxdb, ESTAIDXDPFBP);
            attridx.db = aidxdb;
            attridx.type = type;
            cbmapput(aidxs, dec, -1, (char *)&attridx, sizeof(ESTATTRIDX), FALSE);
          }
          break;
        }
        free(dec);
      }
    }
    CB_LISTCLOSE(list);
  }
  err = FALSE;
  idmax = 0;
  if((vsiz = dpgetwb(metadb, ESTKEYDNUM, -1, 0, ESTNUMBUFSIZ - 1, vbuf)) > 0){
    vbuf[vsiz] = '\0';
    idmax = atoi(vbuf);
  }
  flags = dpgetflags(metadb);
  zmode = 0;
  if(flags & ESTDFZLIB){
    zmode = ESTDFZLIB;
  } else if(flags & ESTDFLZO){
    zmode = ESTDFLZO;
  } else if(flags & ESTDFBZIP){
    zmode = ESTDFBZIP;
  }
  dnum = 0;
  dseq = 0;
  CB_LISTOPEN(list);
  if(!criterinit(attrdb)) err = TRUE;
  while((kbuf = criternext(attrdb, &ksiz)) != NULL){
    if(ksiz == sizeof(int) && (id = *(int *)kbuf) > 0 && id <= idmax &&
       crvsiz(attrdb, kbuf, ksiz) > 0 && crvsiz(textdb, kbuf, ksiz) > 0){
      dnum++;
      if(dseq < id) dseq = id;
      if(options & ESTRPSTRICT){
        if((mbuf = est_crget(attrdb, zmode, id, &msiz)) != NULL){
          attrs = cbmapload(mbuf, msiz);
          if((elem = cbmapget(attrs, ESTDATTRURI, -1, &esiz)) != NULL){
            vsiz = sprintf(vbuf, "%d", id);
            vlput(listdb, elem, esiz, vbuf, vsiz, VL_DKEEP);
          }
          if(cbmaprnum(aidxs) > 0){
            cbmapiterinit(aidxs);
            while((abuf = cbmapiternext(aidxs, &asiz)) != NULL){
              if(!(elem = cbmapget(attrs, abuf, asiz, &esiz))) continue;
              attridxp = (ESTATTRIDX *)cbmapiterval(abuf, NULL);
              switch(attridxp->type){
              case ESTIDXATTRSTR:
              case ESTIDXATTRNUM:
                est_aidx_attr_put(attridxp->db, id, elem, esiz);
                break;
              default:
                est_aidx_seq_put(attridxp->db, id, elem, esiz);
                break;
              }
            }
          }
          cbmapclose(attrs);
          free(mbuf);
        }
      }
    } else {
      CB_LISTPUSH(list, kbuf, ksiz);
    }
    free(kbuf);
  }
  if(dpecode != DP_ENOITEM) err = TRUE;
  for(i = 0; i < CB_LISTNUM(list); i++){
    elem = CB_LISTVAL2(list, i, esiz);
    crout(attrdb, elem, esiz);
    crout(textdb, elem, esiz);
    crout(kwddb, elem, esiz);
  }
  CB_LISTCLOSE(list);
  sprintf(vbuf, "%d", dseq);
  if(!dpput(metadb, ESTKEYDSEQ, -1, vbuf, -1, DP_DOVER)) err = TRUE;
  sprintf(vbuf, "%d", dnum);
  if(!dpput(metadb, ESTKEYDNUM, -1, vbuf, -1, DP_DOVER)) err = TRUE;
  cbmapiterinit(aidxs);
  while((elem = cbmapiternext(aidxs, NULL)) != NULL){
    attridxp = (ESTATTRIDX *)cbmapiterval(elem, NULL);
    switch(attridxp->type){
    case ESTIDXATTRSTR:
    case ESTIDXATTRNUM:
      if(!vlclose(attridxp->db)) err = TRUE;
      break;
    default:
      if(!dpclose(attridxp->db)) err = TRUE;
      break;
    }
  }
  cbmapclose(aidxs);
  if(!vlclose(listdb)) err = TRUE;
  if(!crclose(kwddb)) err = TRUE;
  if(!crclose(textdb)) err = TRUE;
  if(!crclose(attrdb)) err = TRUE;
  if(!dpclose(metadb)) err = TRUE;
  if(err){
    est_set_ecode(ecp, ESTEDB, __LINE__);
    return FALSE;
  }
  return err ? FALSE : TRUE;
}


/* Extract words for snippet from hints of search. */
CBLIST *est_hints_to_words(CBMAP *hints){
  CBLIST *words;
  const char *kbuf;
  int ksiz;
  assert(hints);
  CB_LISTOPEN(words);
  cbmapiterinit(hints);
  while((kbuf = cbmapiternext(hints, &ksiz)) != NULL){
    if(ksiz < 1 || atoi(cbmapget(hints, kbuf, ksiz, NULL)) < 0) continue;
    CB_LISTPUSH(words, kbuf, ksiz);
  }
  return words;
}


/* Add a record into a result map for logical operation. */
void est_resmap_add(CBMAP *map, const char *key, int score, int method){
  int elem[2], *ep, size;
  assert(map && key);
  size = strlen(key);
  if((ep = (int *)cbmapget(map, key, size, NULL)) != NULL){
    elem[0] = ep[0] + 1;
    switch(method){
    case ESTRMLOSUM:
      elem[1] = ep[1] + score;
      break;
    case ESTRMLOMAX:
      elem[1] = score > ep[1] ? score : ep[1];
      break;
    case ESTRMLOMIN:
      elem[1] = score < ep[1] ? score : ep[1];
      break;
    case ESTRMLOAVG:
      elem[1] = (ep[1] * (ep[0] - 1) + score) / ep[0];
      break;
    default:
      elem[1] = score;
      break;
    }
  } else {
    elem[0] = 1;
    elem[1] = score;
  }
  cbmapput(map, key, size, (char *)&elem, sizeof(int) * 2, TRUE);
}



/* Compare two result elements by score.
   `ap' specifies the pointer to one element.
   `bp' specifies the pointer to the other element.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_resmapelem_compare(const void *ap, const void *bp){
  assert(ap && bp);
  return ((ESTRESMAPELEM *)bp)->score - ((ESTRESMAPELEM *)ap)->score;
}


/* Dump a result list of a result map for logical operation. */
ESTRESMAPELEM *est_resmap_dump(CBMAP *map, int min, int *nump){
  ESTRESMAPELEM *elems;
  const char *key, *vbuf;
  int num, vsiz;
  assert(map && min >= 0 && nump);
  CB_MALLOC(elems, cbmaprnum(map) * sizeof(ESTRESMAPELEM) + 1);
  num = 0;
  cbmapiterinit(map);
  while((key = cbmapiternext(map, NULL)) != NULL){
    CB_MAPITERVAL(vbuf, key, vsiz);
    if(((int *)vbuf)[0] < min) continue;
    elems[num].key = key;
    elems[num].score = ((int *)vbuf)[1];
    num++;
  }
  qsort(elems, num, sizeof(ESTRESMAPELEM), est_resmapelem_compare);
  *nump = num;
  return elems;
}


/* Reset the environment of the process. */
void est_proc_env_reset(void){
  char *value, *pbuf;
  cbstdiobin();
  putenv("LANG=C");
  putenv("LANGUAGE=C");
  putenv("LC_CTYPE=C");
  putenv("LC_COLLATE=C");
  putenv("LC_TIME=C");
  putenv("LC_NUMERIC=C");
  putenv("LC_MONETARY=C");
  putenv("LC_ALL=C");
  putenv("EST_VERSION=" _EST_VERSION);
  if((value = getenv("PATH")) != NULL){
    if(ESTPATHCHR == '\\'){
      pbuf = cbsprintf("PATH=%s;C:\\hyperestraier;D:\\hyperestraier;E:\\hyperestraier", value);
    } else {
      pbuf = cbsprintf("PATH=%s:/bin:/sbin:/usr/bin:/usr/sbin:"
                       "/usr/local/bin:/usr/local/sbin", value);
    }
    putenv(pbuf);
    cbglobalgc(pbuf, free);
  }
}


/* Make a directory. */
int est_mkdir(const char *path){
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  assert(path);
  return mkdir(path) == 0 ? TRUE : FALSE;
#else
  assert(path);
  return mkdir(path, ESTDIRMODE) == 0 ? TRUE : FALSE;
#endif
}


/* Remove a directory and its contents recursively. */
int est_rmdir_rec(const char *path){
  CBLIST *files;
  const char *file;
  char pbuf[ESTPATHBUFSIZ];
  int i;
  assert(path);
  if((files = cbdirlist(path)) != NULL){
    for(i = 0; i < CB_LISTNUM(files); i++){
      file = CB_LISTVAL(files, i);
      if(!strcmp(file, ESTCDIRSTR) || !strcmp(file, ESTPDIRSTR)) continue;
      sprintf(pbuf, "%s%c%s", path, ESTPATHCHR, file);
      if(unlink(pbuf) == -1) est_rmdir_rec(pbuf);
    }
    CB_LISTCLOSE(files);
  }
  return rmdir(path) == 0;
}


/* Get the canonicalized absolute pathname of a file. */
char *est_realpath(const char *path){
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  char pbuf[ESTPATHBUFSIZ*2], *p;
  assert(path);
  if(GetFullPathName(path, ESTPATHBUFSIZ, pbuf, &p) == 0){
    if((((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z')) &&
        path[1] == ':' && path[2] == ESTPATHCHR) || path[0] == ESTPATHCHR ||
       GetFullPathName(ESTCDIRSTR, ESTPATHBUFSIZ, pbuf, &p) == 0){
      sprintf(pbuf, "%s", path);
    } else {
      sprintf(pbuf + strlen(pbuf), "%c%s", ESTPATHCHR, path);
    }
  }
  return cbmemdup(pbuf, -1);
#else
  char pbuf[ESTPATHBUFSIZ*2];
  assert(path);
  if(!realpath(path, pbuf)){
    if(path[0] == ESTPATHCHR || !realpath(ESTCDIRSTR, pbuf)){
      sprintf(pbuf, "%s", path);
    } else {
      sprintf(pbuf + strlen(pbuf), "%c%s", ESTPATHCHR, path);
    }
  }
  return cbmemdup(pbuf, -1);
#endif
}


/* Get the inode number of a file. */
int est_inode(const char *path){
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  char pbuf[ESTPATHBUFSIZ*2], *p;
  int inode;
  struct stat sbuf;
  assert(path);
  if(stat(path, &sbuf) == -1) return -1;
  if(GetFullPathName(path, ESTPATHBUFSIZ*2, pbuf, &p) != 0){
    inode = 11003;
    for(p = pbuf; *p != '\0'; p++){
      inode = inode * 31 + *(unsigned char *)p;
    }
    return (inode * 911) & 0x7FFF;
  }
  return -1;
#else
  struct stat sbuf;
  assert(path);
  if(stat(path, &sbuf) == -1) return -1;
  return sbuf.st_ino & INT_MAX;
#endif
}


/* Change modification time of a file. */
int est_utime(const char *path, time_t mtime){
  struct utimbuf buf;
  assert(path);
  if(mtime < 0) return utime(path, NULL) == 0;
  buf.actime = mtime;
  buf.modtime = mtime;
  return utime(path, &buf) == 0;
}



/* Get the time of day in milliseconds. */
double est_gettimeofday(void){
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  SYSTEMTIME st;
  struct tm ts;
  GetLocalTime(&st);
  memset(&ts, 0, sizeof(ts));
  ts.tm_year = st.wYear - 1900;
  ts.tm_mon = st.wMonth - 1;
  ts.tm_mday = st.wDay;
  ts.tm_hour = st.wHour;
  ts.tm_min = st.wMinute;
  ts.tm_sec = st.wSecond;
  return (double)mktime(&ts) * 1000 + (double)st.wMilliseconds;
#else
  struct timeval tv;
  struct timezone tz;
  if(gettimeofday(&tv, &tz) == -1) return 0.0;
  return (double)tv.tv_sec * 1000 + (double)tv.tv_usec / 1000;
#endif
}


/* Suspend execution for microsecond intervals. */
void est_usleep(unsigned long usec){
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  Sleep(usec / 1000);
#else
  usleep(usec);
#endif
}


/* Set a signal handler. */
void est_signal(int signum, void (*sighandler)(int)){
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  static int first = TRUE;
  int i;
  assert(signum >= 0 && sighandler);
  if(first){
    for(i = 1; i < ESTSIGNUM; i++){
      est_signal_handlers[i] = NULL;
    }
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)est_signal_dispatch, TRUE);
    first = FALSE;
  }
  if(signum >= ESTSIGNUM) return;
  if(sighandler == SIG_IGN){
    signal(signum, SIG_IGN);
  } else if(sighandler == SIG_DFL){
    signal(signum, SIG_DFL);
  } else {
    signal(signum, (void (*)(int))est_signal_dispatch);
    est_signal_handlers[signum] = sighandler;
  }
#else
  static int first = TRUE;
  struct sigaction act;
  int i;
  assert(signum >= 0 && sighandler);
  if(first){
    for(i = 1; i < ESTSIGNUM; i++){
      est_signal_handlers[i] = NULL;
    }
    first = FALSE;
  }
  if(signum >= ESTSIGNUM) return;
  memset(&act, 0, sizeof(act));
  if(sighandler == SIG_IGN){
    act.sa_handler = SIG_IGN;
  } else if(sighandler == SIG_DFL){
    act.sa_handler = SIG_DFL;
  } else {
    act.sa_handler = (void (*)(int))est_signal_dispatch;
    est_signal_handlers[signum] = sighandler;
  }
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  sigaction(signum, &act, NULL);
#endif
}


/* Send a signal to a process. */
int est_kill(int pid, int sig){
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  return FALSE;
#else
  return kill(pid, sig) == 0;
#endif
}


/* Get the load ratio of the physical memory. */
double est_memory_usage(void){
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_) || defined(_SYS_CYGWIN_)
  MEMORYSTATUS sbuf;
  sbuf.dwLength = sizeof(MEMORYSTATUS);
  GlobalMemoryStatus(&sbuf);
  return sbuf.dwMemoryLoad / 100.0;
#else
  return 0.0;
#endif
}


/* get the media type of an extention */
const char *est_ext_type(const char *ext){
  static const char *list[] = {
    ".txt", "text/plain", ".txt.en", "text/plain",
    ".txt.ja", "text/plain", ".asc", "text/plain",
    ".in", "text/plain", ".c", "text/plain",
    ".h", "text/plain", ".cc", "text/plain",
    ".java", "text/plain", ".sh", "text/plain",
    ".pl", "text/plain", ".py", "text/plain",
    ".rb", "text/plain", ".idl", "text/plain",
    ".csv", "text/plain", ".log", "text/plain",
    ".conf", "text/plain", ".rc", "text/plain",
    ".ini", "text/plain", ".html", "text/html",
    ".htm", "text/html", ".xhtml", "text/html",
    ".xht", "text/html", ".css", "text/css",
    ".js", "text/javascript", ".tsv", "text/tab-separated-values",
    ".eml", "message/rfc822", ".mime", "message/rfc822",
    ".mht", "message/rfc822", ".mhtml", "message/rfc822",
    ".sgml", "application/sgml", ".sgm", "application/sgml",
    ".xml", "application/xml", ".xsl", "application/xml",
    ".xslt", "application/xslt+xml", ".xhtml", "application/xhtml+xml",
    ".xht", "application/xhtml+xml", ".rdf", "application/rdf+xml",
    ".rss", "application/rss+xml", ".dtd", "application/xml-dtd",
    ".rtf", "application/rtf", ".pdf", "application/pdf",
    ".ps", "application/postscript", ".eps", "application/postscript",
    ".doc", "application/msword", ".xls", "application/vnd.ms-excel",
    ".ppt", "application/vnd.ms-powerpoint", ".xdw", "application/vnd.fujixerox.docuworks",
    ".swf", "application/x-shockwave-flash", ".zip", "application/zip",
    ".tar", "application/x-tar", ".gz", "application/x-gzip",
    ".bz2", "application/octet-stream", ".z", "application/octet-stream",
    ".lha", "application/octet-stream", ".lzh", "application/octet-stream",
    ".cab", "application/octet-stream", ".rar", "application/octet-stream",
    ".sit", "application/octet-stream", ".bin", "application/octet-stream",
    ".o", "application/octet-stream", ".so", "application/octet-stream",
    ".exe", "application/octet-stream", ".dll", "application/octet-stream",
    ".class", "application/octet-stream", ".png", "image/png",
    ".gif", "image/gif", ".jpg", "image/jpeg",
    ".jpeg", "image/jpeg", ".tif", "image/tiff",
    ".tiff", "image/tiff", ".bmp", "image/bmp",
    ".au", "audio/basic", ".snd", "audio/basic",
    ".mid", "audio/midi", ".midi", "audio/midi",
    ".mp2", "audio/mpeg", ".mp3", "audio/mpeg",
    ".wav", "audio/x-wav", ".mpg", "video/mpeg",
    ".mpeg", "video/mpeg", ".qt", "video/quicktime",
    ".mov", "video/quicktime", ".avi", "video/x-msvideo",
    NULL
  };
  int i;
  assert(ext);
  for(i = 0; list[i]; i++){
    if(!cbstricmp(ext, list[i])) return list[i+1];
  }
  return "application/octet-stream";
}


/* Set a seed vector from a map object. */
void est_vector_set_seed(CBMAP *svmap, int *svec, int vnum){
  const char *kbuf;
  int nnum, ksiz;
  assert(svmap && svec && vnum > 0);
  cbmapiterinit(svmap);
  nnum = 0;
  while(nnum < vnum){
    if((kbuf = cbmapiternext(svmap, &ksiz)) != NULL){
      if(ksiz < 1) continue;
      svec[nnum++] = atoi(cbmapiterval(kbuf, NULL));
    } else {
      svec[nnum++] = 0;
    }
  }
}


/* Set a target vector from a map object. */
void est_vector_set_target(CBMAP *svmap, CBMAP *tvmap, int *tvec, int vnum){
  const char *kbuf, *vbuf;
  int i, ksiz;
  assert(svmap && tvmap && tvec && vnum > 0);
  cbmapiterinit(svmap);
  for(i = 0; i < vnum; i++){
    if((kbuf = cbmapiternext(svmap, &ksiz)) != NULL){
      vbuf = cbmapget(tvmap, kbuf, ksiz, NULL);
      tvec[i] = vbuf ? atoi(vbuf) : 0;
    } else {
      tvec[i] = 0;
    }
  }
}


/* Get the cosine of the angle of two vectors. */
double est_vector_cosine(const int *avec, const int *bvec, int vnum){
  int i;
  double iprod, aabs, babs;
  assert(avec && bvec && vnum >= 0);
  iprod = 0.0;
  for(i = 0; i < vnum; i++){
    iprod += (double)avec[i] * (double)bvec[i];
  }
  aabs = 0.0;
  for(i = 0; i < vnum; i++){
    aabs += (double)avec[i] * (double)avec[i];
  }
  aabs = sqrt(aabs);
  babs = 0.0;
  for(i = 0; i < vnum; i++){
    babs += (double)bvec[i] * (double)bvec[i];
  }
  babs = sqrt(babs);
  if(iprod <= 0.0 || aabs < 1.0 || babs < 1.0) return 0.0;
  return iprod / (aabs * babs);
}



/*************************************************************************************************
 * private objects
 *************************************************************************************************/


/* Set the error code.
   `ecp' specifies the pointer to a variable to be assigned.
   `value' specifies the error code to be assgined.
   `line' specifies the number of the line where the error happened. */
static void est_set_ecode(int *ecp, int value, int line){
  char buf[ESTPATHBUFSIZ];
  assert(ecp && line > 0);
  *ecp = value;
  if(dpdbgfd >= 0){
    fflush(stdout);
    fflush(stderr);
    sprintf(buf, "* est_set_ecode: %d: [%d] %s\n", line, value, est_err_msg(value));
    write(dpdbgfd, buf, strlen(buf));
  }
}


/* Encode a string into hexadecimal.
   `str' specifies a string.
   The return value is the result hexadecimal string. */
static char *est_hex_encode(const char *str){
  char *res, *wp;
  assert(str);
  CB_MALLOC(res, strlen(str) * 2 + 1);
  wp = res;
  while(*str != '\0'){
    wp += sprintf(wp, "%02X", *(unsigned char *)str);
    str++;
  }
  *wp = '\0';
  return res;
}


/* Decode a hexadecimal string into original one.
   `str' specifies a hexadecimal string.
   The return value is the original string. */
static char *est_hex_decode(const char *str){
  char *res, *wp;
  int i, len;
  assert(str);
  len = strlen(str);
  CB_MALLOC(res, len + 1);
  wp = res;
  for(i = 0; i < len; i += 2){
    *(wp++) = (str[i] >= 'A' ? str[i] - 'A' + 10 : str[i] - '0') * 16 +
      (str[i+1] >= 'A' ? str[i+1] - 'A' + 10 : str[i+1] - '0');
  }
  *wp = '\0';
  return res;
}


/* Count the number of missing characters when converting.
   `ptr' specifies the pointer to a region.
   `size' specifies the size of the region.
   `icode' specifies the name of encoding of the input string.
   `ocode' specifies the name of encoding of the output string.
   The return value is the number of missing characters. */
static int est_enc_miss(const char *ptr, int size, const char *icode, const char *ocode){
  iconv_t ic;
  char obuf[ESTICCHECKSIZ], *wp, *rp;
  size_t isiz, osiz;
  int miss;
  assert(ptr && size >= 0 && icode && ocode);
  isiz = size;
  if((ic = iconv_open(ocode, icode)) == (iconv_t)-1) return ESTICMISSMAX;
  miss = 0;
  rp = (char *)ptr;
  while(isiz > 0){
    osiz = ESTICCHECKSIZ;
    wp = obuf;
    if(iconv(ic, (void *)&rp, &isiz, &wp, &osiz) == -1){
      if(errno == EILSEQ || errno == EINVAL){
        rp++;
        isiz--;
        miss++;
        if(miss >= ESTICMISSMAX) break;
      } else {
        break;
      }
    }
  }
  if(iconv_close(ic) == -1) return ESTICMISSMAX;
  return miss;
}


/* Normalize a text.
   `utext' specifies a text whose encoding is UTF-16BE.
   `size' specifies the size of the text.
   `sp' specifies the pointer to a variable to which the size of the result is assigned. */
static void est_normalize_text(unsigned char *utext, int size, int *sp){
  int i, wi, b1, b2;
  assert(utext && size >= 0 && sp);
  wi = 0;
  for(i = 0; i < size - 1; i += 2){
    b1 = utext[i];
    b2 = utext[i+1];
    if(b1 == 0x0){
      if(b2 <= 0x8 || (b2 >= 0x0e && b2 <= 0x1f)){
        /* control characters */
        utext[wi++] = 0x0;
        utext[wi++] = 0x20;
      } else if(b2 == '[' || b2 == ']'){
	/* don't move wi */
      } else if(b2 == 0xa0){
        /* no-break space */
        utext[wi++] = 0x0;
        utext[wi++] = 0x20;
      } else {
        /* (otherwise) */
        utext[wi++] = b1;
        utext[wi++] = b2;
      }
    } else if(b1 == 0x20){
      if(b2 == 0x2){
        /* en space */
        utext[wi++] = 0x0;
        utext[wi++] = 0x20;
      } else if(b2 == 0x3){
        /* em space */
        utext[wi++] = 0x0;
        utext[wi++] = 0x20;
      } else if(b2 == 0x9){
        /* thin space */
        utext[wi++] = 0x0;
        utext[wi++] = 0x20;
      } else if(b2 == 0x10){
        /* hyphen */
        utext[wi++] = 0x0;
        utext[wi++] = 0x2d;
      } else if(b2 == 0x15){
        /* fullwidth horizontal line */
        utext[wi++] = 0x0;
        utext[wi++] = 0x2d;
      } else if(b2 == 0x19){
        /* apostrophe */
        utext[wi++] = 0x0;
        utext[wi++] = 0x27;
      } else if(b2 == 0x33){
        /* double quotes */
        utext[wi++] = 0x0;
        utext[wi++] = 0x22;
      } else {
        /* (otherwise) */
        utext[wi++] = b1;
        utext[wi++] = b2;
      }
    } else if(b1 == 0x22){
      if(b2 == 0x12){
        /* minus sign */
        utext[wi++] = 0x0;
        utext[wi++] = 0x2d;
      } else {
        /* (otherwise) */
        utext[wi++] = b1;
        utext[wi++] = b2;
      }
    } else if(b1 == 0x2e){
      if (b2 == 0x22 || b2 == 0x23){
	/* half square brackets get removed */
      }
    } else if(b1 == 0x30){
      if(b2 == 0x0){
        /* fullwidth space */
        utext[wi++] = 0x0;
        utext[wi++] = 0x20;
      } else {
        /* (otherwise) */
        utext[wi++] = b1;
        utext[wi++] = b2;
      }
    } else if(b1 == 0xff){
      if(b2 == 0x01){
        /* fullwidth exclamation */
        utext[wi++] = 0x0;
        utext[wi++] = 0x21;
      } else if(b2 == 0x03){
        /* fullwidth igeta */
        utext[wi++] = 0x0;
        utext[wi++] = 0x23;
      } else if(b2 == 0x04){
        /* fullwidth dollar */
        utext[wi++] = 0x0;
        utext[wi++] = 0x24;
      } else if(b2 == 0x05){
        /* fullwidth parcent */
        utext[wi++] = 0x0;
        utext[wi++] = 0x25;
      } else if(b2 == 0x06){
        /* fullwidth ampersand */
        utext[wi++] = 0x0;
        utext[wi++] = 0x26;
      } else if(b2 == 0x0a){
        /* fullwidth asterisk */
        utext[wi++] = 0x0;
        utext[wi++] = 0x2a;
      } else if(b2 == 0x0b){
        /* fullwidth plus */
        utext[wi++] = 0x0;
        utext[wi++] = 0x2b;
      } else if(b2 == 0x0c){
        /* fullwidth comma */
        utext[wi++] = 0x0;
        utext[wi++] = 0x2c;
      } else if(b2 == 0x0e){
        /* fullwidth period */
        utext[wi++] = 0x0;
        utext[wi++] = 0x2e;
      } else if(b2 == 0x0f){
        /* fullwidth slash */
        utext[wi++] = 0x0;
        utext[wi++] = 0x2f;
      } else if(b2 == 0x1a){
        /* fullwidth colon */
        utext[wi++] = 0x0;
        utext[wi++] = 0x3a;
      } else if(b2 == 0x1b){
        /* fullwidth semicolon */
        utext[wi++] = 0x0;
        utext[wi++] = 0x3b;
      } else if(b2 == 0x1d){
        /* fullwidth equal */
        utext[wi++] = 0x0;
        utext[wi++] = 0x3d;
      } else if(b2 == 0x1f){
        /* fullwidth question */
        utext[wi++] = 0x0;
        utext[wi++] = 0x3f;
      } else if(b2 == 0x20){
        /* fullwidth atmark */
        utext[wi++] = 0x0;
        utext[wi++] = 0x40;
      } else if(b2 == 0x3c){
        /* fullwidth backslash */
        utext[wi++] = 0x0;
        utext[wi++] = 0x5c;
      } else if(b2 == 0x3e){
        /* fullwidth circumflex */
        utext[wi++] = 0x0;
        utext[wi++] = 0x5e;
      } else if(b2 == 0x3f){
        /* fullwidth underscore */
        utext[wi++] = 0x0;
        utext[wi++] = 0x5f;
      } else if(b2 == 0x5c){
        /* fullwidth vertical line */
        utext[wi++] = 0x0;
        utext[wi++] = 0x7c;
      } else if(b2 >= 0x21 && b2 <= 0x3a){
        /* fullwidth alphabets */
        utext[wi++] = 0x0;
        utext[wi++] = b2 - 0x21 + 0x41;
      } else if(b2 >= 0x41 && b2 <= 0x5a){
        /* fullwidth small alphabets */
        utext[wi++] = 0x0;
        utext[wi++] = b2 - 0x41 + 0x61;
      } else if(b2 >= 0x10 && b2 <= 0x19){
        /* fullwidth numbers */
        utext[wi++] = 0x0;
        utext[wi++] = b2 - 0x10 + 0x30;
      } else if(b2 == 0x61){
        /* halfwidth full stop */
        utext[wi++] = 0x30;
        utext[wi++] = 0x2;
      } else if(b2 == 0x62){
        /* halfwidth left corner */
        utext[wi++] = 0x30;
        utext[wi++] = 0xc;
      } else if(b2 == 0x63){
        /* halfwidth right corner */
        utext[wi++] = 0x30;
        utext[wi++] = 0xd;
      } else if(b2 == 0x64){
        /* halfwidth comma */
        utext[wi++] = 0x30;
        utext[wi++] = 0x1;
      } else if(b2 == 0x65){
        /* halfwidth middle dot */
        utext[wi++] = 0x30;
        utext[wi++] = 0xfb;
      } else if(b2 == 0x66){
        /* halfwidth wo */
        utext[wi++] = 0x30;
        utext[wi++] = 0xf2;
      } else if(b2 >= 0x67 && b2 <= 0x6b){
        /* halfwidth small a-o */
        utext[wi++] = 0x30;
        utext[wi++] = (b2 - 0x67) * 2 + 0xa1;
      } else if(b2 >= 0x6c && b2 <= 0x6e){
        /* halfwidth small ya-yo */
        utext[wi++] = 0x30;
        utext[wi++] = (b2 - 0x6c) * 2 + 0xe3;
      } else if(b2 == 0x6f){
        /* halfwidth small tu */
        utext[wi++] = 0x30;
        utext[wi++] = 0xc3;
      } else if(b2 == 0x70){
        /* halfwidth prolonged mark */
        utext[wi++] = 0x30;
        utext[wi++] = 0xfc;
      } else if(b2 >= 0x71 && b2 <= 0x75){
        /* halfwidth a-o */
        utext[wi++] = 0x30;
        utext[wi++] = (b2 - 0x71) * 2 + 0xa2;
        if(i + 2 < size - 1 && b2 == 0x73 && utext[i+2] == 0xff && utext[i+3] == 0x9e){
          utext[wi-1] = 0xf4;
          i += 2;
        }
      } else if(b2 >= 0x76 && b2 <= 0x7a){
        /* halfwidth ka-ko */
        utext[wi++] = 0x30;
        utext[wi++] = (b2 - 0x76) * 2 + 0xab;
        if(i + 2 < size - 1 && utext[i+2] == 0xff && utext[i+3] == 0x9e){
          utext[wi-1] += 1;
          i += 2;
        }
      } else if(b2 >= 0x7b && b2 <= 0x7f){
        /* halfwidth sa-so */
        utext[wi++] = 0x30;
        utext[wi++] = (b2 - 0x7b) * 2 + 0xb5;
        if(i + 2 < size - 1 && utext[i+2] == 0xff && utext[i+3] == 0x9e){
          utext[wi-1] += 1;
          i += 2;
        }
      } else if(b2 >= 0x80 && b2 <= 0x84){
        /* halfwidth ta-to */
        utext[wi++] = 0x30;
        utext[wi++] = (b2 - 0x80) * 2 + 0xbf + (b2 >= 0x82 ? 1 : 0);
        if(i + 2 < size - 1 && utext[i+2] == 0xff && utext[i+3] == 0x9e){
          utext[wi-1] += 1;
          i += 2;
        }
      } else if(b2 >= 0x85 && b2 <= 0x89){
        /* halfwidth na-no */
        utext[wi++] = 0x30;
        utext[wi++] = b2 - 0x85 + 0xca;
      } else if(b2 >= 0x8a && b2 <= 0x8e){
        /* halfwidth ha-ho */
        utext[wi++] = 0x30;
        utext[wi++] = (b2 - 0x8a) * 3 + 0xcf;
        if(i + 2 < size - 1){
          if(utext[i+2] == 0xff && utext[i+3] == 0x9e){
            utext[wi-1] += 1;
            i += 2;
          } else if(utext[i+2] == 0xff && utext[i+3] == 0x9f){
            utext[wi-1] += 2;
            i += 2;
          }
        }
      } else if(b2 >= 0x8f && b2 <= 0x93){
        /* halfwidth ma-mo */
        utext[wi++] = 0x30;
        utext[wi++] = b2 - 0x8f + 0xde;
      } else if(b2 >= 0x94 && b2 <= 0x96){
        /* halfwidth ya-yo */
        utext[wi++] = 0x30;
        utext[wi++] = (b2 - 0x94) * 2 + 0xe4;
      } else if(b2 >= 0x97 && b2 <= 0x9b){
        /* halfwidth ra-ro */
        utext[wi++] = 0x30;
        utext[wi++] = b2 - 0x97 + 0xe9;
      } else if(b2 == 0x9c){
        /* halfwidth wa */
        utext[wi++] = 0x30;
        utext[wi++] = 0xef;
      } else if(b2 == 0x9d){
        /* halfwidth wo */
        utext[wi++] = 0x30;
        utext[wi++] = 0xf3;
      } else {
        /* (otherwise) */
        utext[wi++] = b1;
        utext[wi++] = b2;
      }
    } else {
      /* (otherwise) */
      utext[wi++] = b1;
      utext[wi++] = b2;
    }
  }
  *sp = wi;
}


/* Canonicalize a text for search keys.
   `utext' specifies a text whose encoding is UTF-16BE.
   `size' specifies the size of the text.
   `funcspc' specifies whether to allow functional space characters. */
static void est_canonicalize_text(unsigned char *utext, int size, int funcspc){
  int i, j;
  int compensate = 0;
  /*  fprintf(stderr, "est_canonicalize_text: in=%s ... ", est_uconv_out(utext, size, NULL)); */
  for(i = 0; i < size; i += 2){
    if(utext[i] == 0x0){
      if(utext[i+1] < ' '){
        /* functional spaces */
        if(!funcspc) utext[i+1] = ' ';
#if 0
      } else if(utext[i+1] == '[' || utext[i+1] == ']'){
	utext[i+1] = 0x0;
#endif
      } else if(utext[i+1] == '{' || utext[i+1] == '}'){
	utext[i+1] = '-';
      } else if(utext[i+1] >= 'A' && utext[i+1] <= 'Z'){
        /* ascii */
        utext[i+1] += 'a' - 'A';
      } else if(utext[i+1] >= 0xc0){
        /* latin-1 supplement */
        if((utext[i+1] >= 0xc0 && utext[i+1] <= 0xd6) ||
           (utext[i+1] >= 0xd8 && utext[i+1] <= 0xde)) utext[i+1] += 0x20;
        if(utext[i+1] >= 0xe0 && utext[i+1] <= 0xe5){
          utext[i+1] = 'a';
        } else if(utext[i+1] == 0xe7){
          utext[i+1] = 'c';
        } else if(utext[i+1] >= 0xe8 && utext[i+1] <= 0xeb){
          utext[i+1] = 'e';
        } else if(utext[i+1] >= 0xec && utext[i+1] <= 0xef){
          utext[i+1] = 'i';
        } else if(utext[i+1] == 0xf1){
          utext[i+1] = 'n';
        } else if((utext[i+1] >= 0xf2 && utext[i+1] <= 0xf6) || utext[i+1] == 0xf8){
          utext[i+1] = 'o';
        } else if(utext[i+1] >= 0xf9 && utext[i+1] <= 0xfc){
          utext[i+1] = 'u';
        } else if(utext[i+1] == 0xfd || utext[i+1] == 0xff){
          utext[i+1] = 'y';
        }
      }
    } else if(utext[i] == 0x1){
      /* latin extended-a */
      if((utext[i+1] <= 0x36 && utext[i+1] % 2 == 0) ||
         (utext[i+1] >= 0x39 && utext[i+1] <= 0x47 && utext[i+1] % 2 == 1) ||
         (utext[i+1] >= 0x4a && utext[i+1] <= 0x76 && utext[i+1] % 2 == 0) ||
         (utext[i+1] >= 0x79 && utext[i+1] <= 0x7d && utext[i+1] % 2 == 1))
        utext[i+1] += 0x1;
      if(utext[i+1] <= 0x05){
        utext[i] = 0x0;
        utext[i+1] = 'a';
      } else if(utext[i+1] >= 0x06 && utext[i+1] <= 0x0d){
        utext[i] = 0x0;
        utext[i+1] = 'c';
      } else if(utext[i+1] >= 0x0e && utext[i+1] <= 0x11){
        utext[i] = 0x0;
        utext[i+1] = 'd';
      } else if(utext[i+1] >= 0x12 && utext[i+1] <= 0x1b){
        utext[i] = 0x0;
        utext[i+1] = 'e';
      } else if(utext[i+1] >= 0x1c && utext[i+1] <= 0x23){
        utext[i] = 0x0;
        utext[i+1] = 'g';
      } else if(utext[i+1] >= 0x24 && utext[i+1] <= 0x27){
        utext[i] = 0x0;
        utext[i+1] = 'h';
      } else if(utext[i+1] >= 0x28 && utext[i+1] <= 0x31){
        utext[i] = 0x0;
        utext[i+1] = 'i';
      } else if(utext[i+1] >= 0x34 && utext[i+1] <= 0x35){
        utext[i] = 0x0;
        utext[i+1] = 'j';
      } else if(utext[i+1] >= 0x36 && utext[i+1] <= 0x38){
        utext[i] = 0x0;
        utext[i+1] = 'k';
      } else if(utext[i+1] >= 0x39 && utext[i+1] <= 0x42){
        utext[i] = 0x0;
        utext[i+1] = 'l';
      } else if(utext[i+1] >= 0x43 && utext[i+1] <= 0x4b){
        utext[i] = 0x0;
        utext[i+1] = 'n';
      } else if(utext[i+1] >= 0x4c && utext[i+1] <= 0x51){
        utext[i] = 0x0;
        utext[i+1] = 'o';
      } else if(utext[i+1] >= 0x54 && utext[i+1] <= 0x59){
        utext[i] = 0x0;
        utext[i+1] = 'r';
#if 0
      } else if((utext[i+1] >= 0x5a && utext[i+1] <= 0x61) || utext[i+1] == 0x7f){
        utext[i] = 0x0;
        utext[i+1] = 's';
#endif
      } else if(utext[i+1] >= 0x62 && utext[i+1] <= 0x67){
        utext[i] = 0x0;
        utext[i+1] = 't';
      } else if(utext[i+1] >= 0x68 && utext[i+1] <= 0x73){
        utext[i] = 0x0;
        utext[i+1] = 'u';
      } else if(utext[i+1] >= 0x74 && utext[i+1] <= 0x75){
        utext[i] = 0x0;
        utext[i+1] = 'w';
      } else if(utext[i+1] >= 0x76 && utext[i+1] <= 0x78){
        utext[i] = 0x0;
        utext[i+1] = 'y';
      } else if(utext[i+1] >= 0x79 && utext[i+1] <= 0x7e){
        utext[i] = 0x0;
        utext[i+1] = 'z';
      }
    } else if(utext[i] == 0x3){
      /* greek */
      if(utext[i+1] >= 0x91 && utext[i+1] <= 0xa9) utext[i+1] += 0x20;
    } else if(utext[i] == 0x4){
      /* cyrillic */
      if(utext[i+1] >= 0x10 && utext[i+1] <= 0x2f){
        utext[i+1] += 0x20;
      } else if(utext[i+1] <= 0x0f){
        utext[i+1] += 0x50;
      }
#if 0
    } else if(utext[i] == 0x1e){
      /* latin extended additional (just what Oracc needs for now) */
      if(utext[i+1] == 0x2a || utext[i+1] == 0x2b){
        utext[i] = 0x0;
	utext[i+1] = 'h';
      } else if (utext[i+1] == 0x62 || utext[i+1] == 0x63){
        utext[i] = 0x0;
	utext[i+1] = 's';
      } else if (utext[i+1] == 0x6c || utext[i+1] == 0x6d){
        utext[i] = 0x0;
	utext[i+1] = 't';
      }
#endif
    } else if(utext[i] == 0xff){
      /* special */
      if(utext[i+1] >= 0xf0){
        utext[i] = 0x0;
        utext[i+1] = ' ';
      }
    }
  }
#if 0
  for(compensate = i = j = 0; i < size; i += 2){
    if (!utext[i] && !utext[i+1]) {
      /* deleted char, don't move dest */
      ++compensate;
    } else {
      utext[j++] = utext[i];
      utext[j++] = utext[i+1];
    }
  }
  /* add spaces to end of string to compensate for
     deletions */
  for (i = 0; i < compensate; i += 2){
    utext[j++] = 0x0;
    utext[j++] = 0x20;
  }
#endif
  /*  fprintf(stderr, "out==%s\n", est_uconv_out(utext, size, NULL)); */
}


/* Categorize a character.
   `c' specifies the UCS number of a character.
   The return value is the category of the character. */
static int est_char_category(int c){
  /* ascii space */
  if(c <= 0x0020) return ESTSPACECHR;
  /* ascii alnum */
  if((c >= 0x0030 && c <= 0x0039) || (c >= 0x0041 && c <= 0x005a) ||
     (c >= 0x0061 && c <= 0x007a)) return ESTWESTALPH;
  /* latin */
  if((c >= 0x00c0 && c <= 0x00ff && c != 0x00d7 && c != 0x00f7) || (c >= 0x0100 && c <= 0x017f))
    return ESTWESTALPH;
  /* arabic and syrian */
  if(c >= 0x0600 && c <= 0x08ff) return ESTEASTALPH;
  /* south and south east asia */
  if((c >= 0x0900 && c <= 0x109f) || (c >= 0x1700 && c <= 0x1cff)) return ESTEASTALPH;
  /* cjk and surrogates */
  if((c >= 0x1100 && c <= 0x11ff) || (c >= 0x2e80 && c <= 0xdfff) ||
     (c >= 0xf900 && c <= 0xfaff) || (c >= 0xff00 && c <= 0xffef)) return ESTEASTALPH;
  /* asian presentation forms */
  if((c >= 0xfb50 && c <= 0xfdff) || (c >= 0xfe30 && c <= 0xfe4f) ||
     (c >= 0xfe70 && c <= 0xfeff)) return ESTEASTALPH;
  /* others */
  return ESTDELIMCHR;
}


/* Categorize a character for perfect N-gram analyzer.
   `c' specifies the UCS number of a character.
   The return value is the category of the character. */
static int est_char_category_perfng(int c){
  if(c <= 0x0020) return ESTSPACECHR;
  return ESTEASTALPH;
}


/* Categorize a character for character category analyzer.
   `c' specifies the UCS number of a character.
   The return value is the category of the character. */
static int est_char_category_chrcat(int c){
  /* ascii space */
  if(c <= 0x0020) return ESTSPACECHR;
  /* ascii alnum */
  if((c >= 0x0030 && c <= 0x0039) || (c >= 0x0041 && c <= 0x005a) ||
     (c >= 0x0061 && c <= 0x007a)) return ESTWESTALPH;
  /* latin */
  if((c >= 0x00c0 && c <= 0x00ff && c != 0x00d7 && c != 0x00f7) || (c >= 0x0100 && c <= 0x017f))
    return ESTWESTALPH;
  /* arabic and syrian */
  if(c >= 0x0600 && c <= 0x08ff) return ESTEASTALPH;
  /* south and south east asia */
  if((c >= 0x0900 && c <= 0x109f) || (c >= 0x1700 && c <= 0x1cff)) return ESTEASTALPH;
  /* hiragana */
  if(c >= 0x3040 && c <= 0x309f) return ESTHIRAGANA;
  /* katakana */
  if(c >= 0x30a0 && c <= 0x30ff) return ESTKATAKANA;
  /* hangul */
  if((c >= 0x1100 && c <= 0x11ff) || (c >= 0x3130 && c <= 0x318f) ||
     (c >= 0xac00 && c <= 0xd7af)) return ESTHANGUL;
  /* kanji */
  if(c >= 0x4e00 && c <= 0x9faf) return ESTKANJI;
  /* other cjk and surrogates */
  if((c >= 0x2e80 && c <= 0xdfff) || (c >= 0xf900 && c <= 0xfaff) ||
     (c >= 0xff00 && c <= 0xffef)) return ESTEASTALPH;
  /* asian presentation forms */
  if((c >= 0xfb50 && c <= 0xfdff) || (c >= 0xfe30 && c <= 0xfe4f) ||
     (c >= 0xfe70 && c <= 0xfeff)) return ESTEASTALPH;
  /* others */
  return ESTDELIMCHR;
}


/* Make a snippet of an arbitrary string.
   `word' specifies a list object of words to be highlight.
   `wwidth' specifies whole width of the result.
   `hwidth' specifies width of strings picked up from the beginning of the text.
   `awidth' specifies width of strings picked up around each highlighted word.
   The return value is a snippet string of the string. */
static char *est_make_snippet(const char *str, int len, const CBLIST *words,
                              int wwidth, int hwidth, int awidth){
  CBDATUM *res;
  CBMAP *counts;
  CBLIST *rwords;
  const char *word, *cval;
  const unsigned char *rword;
  unsigned char *rtext, *ctext;
  int i, j, k, bi, size, wsiz, rwsiz, mywidth, awsiz, csiz;
  assert(str && len >= 0 && words && wwidth >= 0 && hwidth >= 0 && awidth >= 0);
  CB_DATUMOPEN(res);
  CB_LISTOPEN(rwords);
  for(i = 0; i < CB_LISTNUM(words); i++){
    word = CB_LISTVAL2(words, i, wsiz);
    if(wsiz < 1 || !strcmp(word, ESTOPUVSET)) continue;
    rtext = (unsigned char *)est_uconv_in(word, wsiz, &size);
    est_canonicalize_text(rtext, size, TRUE);
    CB_LISTPUSHBUF(rwords, (char *)rtext, size);
  }
  rtext = (unsigned char *)est_uconv_in(str, len, &size);
  ctext = (unsigned char *)cbmemdup((char *)rtext, size);
  est_canonicalize_text(ctext, size, FALSE);
  mywidth = hwidth;
  if(CB_LISTNUM(rwords) < 1) mywidth *= 3;
  if(mywidth > wwidth) mywidth = wwidth;
  for(i = 0; i < size && mywidth > 0; i += 2){
    mywidth -= est_char_category(rtext[i] * 0x100 + rtext[i+1]) == ESTEASTALPH ? 2 : 1;
  }
  awsiz = size - i;
  if(awsiz > ESTWORDMAXLEN) awsiz = ESTWORDMAXLEN;
  est_snippet_add_text(rtext, ctext, i, awsiz, res, rwords);
  wwidth -= hwidth;
  bi = i + 2;
  CB_DATUMCAT(res, "\n", 1);
  if(awidth > 0){
    counts = cbmapopenex(ESTMINIBNUM);
    for(i = bi; i < size && wwidth >= 0; i += 2){
      for(j = 0; j < CB_LISTNUM(rwords); j++){
        rword = (unsigned char *)CB_LISTVAL2(rwords, j, rwsiz);
        if(est_str_fwmatch_wide(ctext + i, size - i, rword, rwsiz) > 0 &&
           (!(cval = cbmapget(counts, (char *)rword, rwsiz, &csiz)) ||
            csiz < (wwidth > awidth * 1.2 ? 2 : 1))){
          cbmapputcat(counts, (char *)rword, rwsiz, "*", 1);
          if(cbmaprnum(counts) >= CB_LISTNUM(rwords)){
            cbmapclose(counts);
            counts = cbmapopenex(ESTMINIBNUM);
          }
          mywidth = awidth / 2 + 1;
          for(k = i - 2; k >= bi && mywidth >= 0; k -= 2){
            mywidth -= est_char_category(rtext[k] * 0x100 + rtext[k+1]) == ESTEASTALPH ? 2 : 1;
          }
          bi = k;
          mywidth = awidth / 2 + 1;
          for(k = i + rwsiz + 2; k < size && mywidth >= 0; k += 2){
            mywidth -= est_char_category(rtext[k] * 0x100 + rtext[k+1]) == ESTEASTALPH ? 2 : 1;
          }
          if(k > size) k = size;
          est_snippet_add_text(rtext + bi, ctext + bi, k - bi, 0, res, rwords);
          wwidth -= awidth + rwsiz / 2;
          bi = k + 2;
          i = bi - 2;
          CB_DATUMCAT(res, "\n", 1);
          break;
        }
      }
    }
    cbmapclose(counts);
  }
  free(ctext);
  free(rtext);
  CB_LISTCLOSE(rwords);
  return cbdatumtomalloc(res, NULL);
}


/* Check whether a string is compsed of CJK characters only.
   `str' specifies a string of UTF-8.
   The return value is whether the string is compsed of CJK characters only. */
static int est_check_cjk_only(const char *str){
  const unsigned char *rp;
  int size;
  rp = (unsigned char *)str;
  size = strlen(str);
  while(rp < (unsigned char *)str + size){
    if(*rp < 0x7f){
      return FALSE;
    } else if(*rp < 0xdf){
      return FALSE;
    } else if(*rp < 0xf0){
      if(rp >= (unsigned char *)str + size - 2) break;
      rp += 3;
    } else if(*rp < 0xf8){
      if(rp >= (unsigned char *)str + size - 3) break;
      rp += 4;
    } else if(*rp < 0xfb){
      if(rp >= (unsigned char *)str + size - 4) break;
      rp += 5;
    } else if(*rp < 0xfd){
      if(rp >= (unsigned char *)str + size - 5) break;
      rp += 6;
    } else {
      break;
    }
  }
  return TRUE;
}


/* Convert a simplified phrase into complete form.
   `sphrase' specifies a simplified phrase.
   The return value is the complete form of the phrase. */
static char *est_phrase_from_simple(const char *sphrase){
  CBDATUM *datum;
  const char *oper, *rp, *pv;
  unsigned char *utext;
  char *rtext;
  int size, quote, lw;
  assert(sphrase);
  CB_DATUMOPEN(datum);
  utext = (unsigned char *)est_uconv_in(sphrase, strlen(sphrase), &size);
  est_normalize_text(utext, size, &size);
  est_canonicalize_text(utext, size, FALSE);
  rtext = est_uconv_out((char *)utext, size, NULL);
  cbstrsqzspc(rtext);
  quote = FALSE;
  oper = NULL;
  lw = FALSE;
  for(rp = rtext; *rp != '\0'; rp++){
    if(*rp == '"'){
      if(oper){
        CB_DATUMCAT(datum, oper, strlen(oper));
        oper = NULL;
      }
      quote = !quote;
      continue;
    }
    if(quote){
      CB_DATUMCAT(datum, rp, 1);
      continue;
    }
    switch(*rp){
    case ' ':
      if(!oper) oper = " AND ";
      lw = FALSE;
      break;
    case '&':
      oper = " AND ";
      lw = FALSE;
      break;
    case '|':
      oper = " OR ";
      lw = FALSE;
      break;
    case '!':
      oper = " ANDNOT ";
      lw = FALSE;
      break;
    default:
      if(oper){
        CB_DATUMCAT(datum, oper, strlen(oper));
        oper = NULL;
      }
      if(!lw){
        pv = rp;
        while(*pv != '\0' && *pv != ' '){
          pv++;
        }
        if(pv > rp + 1 && pv[-1] == '*'){
          if(rp[0] == '*'){
            CB_DATUMCAT(datum, ESTOPWCRX " ",  strlen(ESTOPWCRX) + 1);
          } else {
            CB_DATUMCAT(datum, ESTOPWCBW " ",  strlen(ESTOPWCBW) + 1);
          }
        } else if(pv > rp + 1 && rp[0] == '*'){
          if(pv[-1] == '*'){
            CB_DATUMCAT(datum, ESTOPWCRX " ",  strlen(ESTOPWCRX) + 1);
          } else {
            CB_DATUMCAT(datum, ESTOPWCEW " ",  strlen(ESTOPWCEW) + 1);
          }
        }
      }
      if(*rp != '*' || (lw && rp[1] != '\0' && rp[1] != ' ')) CB_DATUMCAT(datum, rp, 1);
      lw = TRUE;
    }
  }
  free(rtext);
  free(utext);
  return cbdatumtomalloc(datum, NULL);
}


/* Convert a rough phrase into complete form.
   `rphrase' specifies a simplified phrase.
   The return value is the complete form of the phrase. */
static char *est_phrase_from_rough(const char *rphrase){
  CBDATUM *datum;
  const char *oper, *rp;
  unsigned char *utext;
  char *rtext;
  int size, quote, lw;
  assert(rphrase);
  CB_DATUMOPEN(datum);
  utext = (unsigned char *)est_uconv_in(rphrase, strlen(rphrase), &size);
  est_normalize_text(utext, size, &size);
  est_canonicalize_text(utext, size, FALSE);
  rtext = est_uconv_out((char *)utext, size, NULL);
  cbstrsqzspc(rtext);
  quote = FALSE;
  oper = NULL;
  lw = FALSE;
  for(rp = rtext; *rp != '\0'; rp++){
    if(*rp == '"'){
      if(oper){
        CB_DATUMCAT(datum, oper, strlen(oper));
        oper = NULL;
      }
      quote = !quote;
      continue;
    }
    if(quote){
      CB_DATUMCAT(datum, rp, 1);
      continue;
    }
    switch(*rp){
    case ' ':
      if(!oper) oper = " AND ";
      lw = FALSE;
      break;
    case '&':
      oper = " AND ";
      lw = FALSE;
      break;
    case '|':
      oper = " OR ";
      lw = FALSE;
      break;
    case '-':
      if(lw){
        CB_DATUMCAT(datum, rp, 1);
      } else {
        oper = " ANDNOT ";
      }
      break;
    default:
      if(oper){
        CB_DATUMCAT(datum, oper, strlen(oper));
        oper = NULL;
      }
      CB_DATUMCAT(datum, rp, 1);
      lw = TRUE;
    }
  }
  free(rtext);
  free(utext);
  return cbdatumtomalloc(datum, NULL);
}


/* Convert a union phrase into complete form.
   `uphrase' specifies a simplified phrase.
   The return value is the complete form of the phrase. */
static char *est_phrase_from_union(const char *uphrase){
  CBDATUM *datum;
  CBLIST *terms;
  const char *term;
  unsigned char *utext;
  char *rtext;
  int i, size;
  assert(uphrase);
  CB_DATUMOPEN(datum);
  utext = (unsigned char *)est_uconv_in(uphrase, strlen(uphrase), &size);
  est_normalize_text(utext, size, &size);
  est_canonicalize_text(utext, size, FALSE);
  rtext = est_uconv_out((char *)utext, size, NULL);
  cbstrsqzspc(rtext);
  terms = cbsplit(rtext, -1, " ");
  for(i = 0; i < CB_LISTNUM(terms); i++){
    term = CB_LISTVAL2(terms, i, size);
    if(size < 1) continue;
    if(CB_DATUMSIZE(datum) > 0) CB_DATUMCAT(datum, " OR ", 4);
    CB_DATUMCAT(datum, term, size);
  }
  CB_LISTCLOSE(terms);
  free(rtext);
  free(utext);
  return cbdatumtomalloc(datum, NULL);
}


/* Convert a intersection phrase into complete form.
   `iphrase' specifies a simplified phrase.
   The return value is the complete form of the phrase. */
static char *est_phrase_from_isect(const char *iphrase){
  CBDATUM *datum;
  CBLIST *terms;
  const char *term;
  unsigned char *utext;
  char *rtext;
  int i, size;
  assert(iphrase);
  CB_DATUMOPEN(datum);
  utext = (unsigned char *)est_uconv_in(iphrase, strlen(iphrase), &size);
  est_normalize_text(utext, size, &size);
  est_canonicalize_text(utext, size, FALSE);
  rtext = est_uconv_out((char *)utext, size, NULL);
  cbstrsqzspc(rtext);
  terms = cbsplit(rtext, -1, " ");
  for(i = 0; i < CB_LISTNUM(terms); i++){
    term = CB_LISTVAL2(terms, i, size);
    if(size < 1) continue;
    if(CB_DATUMSIZE(datum) > 0) CB_DATUMCAT(datum, " AND ", 5);
    CB_DATUMCAT(datum, term, size);
  }
  CB_LISTCLOSE(terms);
  free(rtext);
  free(utext);
  return cbdatumtomalloc(datum, NULL);
}


/* Add a string to a snippet.
   `rtext' specifies a raw text.
   `ctext' specifies a canonicalized text.
   `size' specifies the size of the raw text and the canonicalized text.
   `awsiz' specifies the size of allowance for matching words.
   `res' specifies a datum object for the result.
   `rwords' specifies a list object of raw words. */
static void est_snippet_add_text(const unsigned char *rtext, const unsigned char *ctext,
                                 int size, int awsiz, CBDATUM *res, const CBLIST *rwords){
  const unsigned char *rword;
  char *orig;
  int i, j, bi, rwsiz, step, osiz;
  bi = 0;
  for(i = 0; i < size; i += 2){
    for(j = 0; j < CB_LISTNUM(rwords); j++){
      rword = (unsigned char *)CB_LISTVAL2(rwords, j, rwsiz);
      if((step = est_str_fwmatch_wide(ctext + i, size + awsiz - i, rword, rwsiz)) > 0){
        if(i - bi > 0){
          orig = est_uconv_out((char *)rtext + bi, i - bi, &osiz);
          CB_DATUMCAT(res, orig, osiz);
          CB_DATUMCAT(res, "\n", 1);
          free(orig);
        }
        orig = est_uconv_out((char *)rtext + i, step, &osiz);
        CB_DATUMCAT(res, orig, osiz);
        free(orig);
        CB_DATUMCAT(res, "\t", 1);
        orig = est_uconv_out((char *)rword, rwsiz, &osiz);
        CB_DATUMCAT(res, orig, osiz);
        free(orig);
        CB_DATUMCAT(res, "\n", 1);
        bi = i + step;
        i = bi - 2;
        break;
      }
    }
  }
  if(i - bi > 0){
    orig = est_uconv_out((char *)rtext + bi, i - bi, &osiz);
    CB_DATUMCAT(res, orig, osiz);
    CB_DATUMCAT(res, "\n", 1);
    free(orig);
  }
}


/* Check whether a string begins with a key.
   `string' specifies a target string whose encoding is UTF-16BE.
   `size' specifies the size of the target string.
   `key' specifies a key string whose encoding is UTF-16BE.
   `ksiz' specifies the size of the key string.
   `key' specifies the pointer
   The return value is the number of characters of the corresponding string, or 0 if the target
   string does not begin with the key. */
static int est_str_fwmatch_wide(const unsigned char *str, int size,
                                const unsigned char *key, int ksiz){
  int si, ki;
  assert(str && size >= 0 && key && ksiz >= 0);
  if(size < 2 || ksiz < 2 || (str[0] == 0x0 && str[1] <= 0x20)) return 0;
  si = 0;
  ki = 0;
  while(ki < ksiz){
    if(si >= size) return 0;
    if(str[si] == 0x0 && str[si+1] <= 0x20){
      si += 2;
      continue;
    }
    if(key[ki] == 0x0 && key[ki+1] <= 0x20){
      ki += 2;
      continue;
    }
    if(str[si] != key[ki] || str[si+1] != key[ki+1]) return 0;
    si += 2;
    ki += 2;
  }
  return si;
}


/* Find the first occurrence of a substring ignoring space characters.
   `haystack' specifies a target string.
   `needle' specifies a substring.
   The the pointer to the first occurrence. */
static char *est_strstr_sparse(const char *haystack, const char *needle){
  const char *hp, *np;
  assert(haystack && needle);
  while(*needle > '\0' && *needle <= ' '){
    needle++;
  }
  if(needle[0] == '\0') return (char *)haystack;
  while((haystack = strchr(haystack, *needle)) != NULL){
    hp = haystack;
    np = needle;
    while(TRUE){
      while(*hp > '\0' && *hp <= ' '){
        hp++;
      }
      while(*np > '\0' && *np <= ' '){
        np++;
      }
      if(*np == '\0') return (char *)haystack;
      if(*hp != *np || *hp == '\0') break;
      hp++;
      np++;
    }
    haystack++;
  }
  return NULL;
}


/* Get the last ID number in an index record.
   `vbuf' specifies the pointer to the value of a record.
   `vsiz' specifies the size of the value.
   `smode' specifies a mode of score type.
   The return value is the last ID number in a record. */
static int est_idx_rec_last_id(const char *vbuf, int vsiz, int smode){
  const char *rp, *ep, *sp;
  int cid, vnum, vstep;
  assert(vbuf && vsiz >= 0);
  cid = 0;
  rp = vbuf;
  ep = vbuf + vsiz;
  while(rp < ep){
    EST_READ_VNUMBUF(rp, vnum, vstep);
    cid += vnum + 1;
    rp += vstep;
    sp = rp;
    switch(smode){
    case ESTDFSCVOID:
      break;
    default:
      rp++;
      break;
    case ESTDFSCINT:
    case ESTDFSCASIS:
      rp += sizeof(int);
      break;
    }
    while(*rp != 0x00){
      rp += 2;
    }
    rp++;
  }
  return cid;
}


/* Encode a raw index record into a gap form.
   `datum' specifies a datum to store the result.
   `vbuf' specifies the pointer to the value of a raw index record.
   `vsiz' specifies the size of the value of the record.
   `lid' specifies the last ID number in the existing record.
   `smode' specifies a mode of score type. */
static void est_encode_idx_rec(CBDATUM *datum, const char *vbuf, int vsiz, int lid, int smode){
  const char *rp, *ep, *sp;
  char nbuf[ESTNUMBUFSIZ];
  int cid, vstep;
  assert(datum && vbuf && vsiz >= 0);
  rp = vbuf;
  ep = vbuf + vsiz;
  while(rp < ep){
    EST_READ_VNUMBUF(rp, cid, vstep);
    rp += vstep;
    sp = rp;
    switch(smode){
    case ESTDFSCVOID:
      break;
    default:
      rp++;
      break;
    case ESTDFSCINT:
    case ESTDFSCASIS:
      rp += sizeof(int);
      break;
    }
    while(*rp != 0x00){
      rp += 2;
    }
    rp++;
    EST_SET_VNUMBUF(vstep, nbuf, cid - lid - 1);
    CB_DATUMCAT(datum, nbuf, vstep);
    CB_DATUMCAT(datum, sp, rp - sp);
    lid = cid;
  }
}


/* Decode a gap index record into a raw form.
   `datum' specifies a datum to store the result.
   `vbuf' specifies the pointer to the value of a gap index record.
   `vsiz' specifies the size of the value of the record.
   `smode' specifies a mode of score type. */
static void est_decode_idx_rec(CBDATUM *datum, const char *vbuf, int vsiz, int smode){
  const char *rp, *ep, *sp;
  char nbuf[ESTNUMBUFSIZ];
  int cid, vnum, vstep;
  assert(datum && vbuf && vsiz >= 0);
  rp = vbuf;
  ep = vbuf + vsiz;
  cid = 0;
  while(rp < ep){
    EST_READ_VNUMBUF(rp, vnum, vstep);
    cid += vnum + 1;
    rp += vstep;
    sp = rp;
    switch(smode){
    case ESTDFSCVOID:
      break;
    default:
      rp++;
      break;
    case ESTDFSCINT:
    case ESTDFSCASIS:
      rp += sizeof(int);
      break;
    }
    while(*rp != 0x00){
      rp += 2;
    }
    rp++;
    EST_SET_VNUMBUF(vstep, nbuf, cid);
    CB_DATUMCAT(datum, nbuf, vstep);
    CB_DATUMCAT(datum, sp, rp - sp);
  }
}


/* Open the inverted index.
   `name' specifies the name of a directory.
   `omode' specifies an open mode of Villa.
   `dnum' specifies the number of database files.
   The return value is a database object of the database. */
static ESTIDX *est_idx_open(const char *name, int omode, int dnum){
  ESTIDX *idx;
  CBLIST *files;
  const char *file;
  char path[ESTPATHBUFSIZ];
  int i, crdnum;
  assert(name && dnum > 0);
  if(dnum > ESTIDXDMAX) dnum = ESTIDXDMAX;
  CB_MALLOC(idx, sizeof(ESTIDX));
  if((omode & VL_OCREAT) && !est_mkdir(name) && errno != EEXIST) return NULL;
  if((omode & VL_OTRUNC) && (files = cbdirlist(name)) != NULL){
    for(i = 0; i < CB_LISTNUM(files); i++){
      file = CB_LISTVAL(files, i);
      if(!strcmp(file, ESTCDIRSTR) || !strcmp(file, ESTPDIRSTR)) continue;
      sprintf(path, "%s%c%s", name, ESTPATHCHR, file);
      if(unlink(path) == -1) est_rmdir_rec(path);
    }
    CB_LISTCLOSE(files);
  }
  for(i = 0; i < dnum; i++){
    sprintf(path, "%s%c%04d", name, ESTPATHCHR, i + 1);
    crdnum = vlcrdnum;
    vlcrdnum = ESTVLCRDNUM;
    if(!(idx->dbs[i] = vlopen(path, omode, VL_CMPLEX))){
      while(--i >= 0){
        vlclose(idx->dbs[i]);
      }
      vlcrdnum = crdnum;
      return NULL;
    }
    vlcrdnum = crdnum;
  }
  idx->name = cbmemdup(name, -1);
  idx->omode = omode;
  idx->dnum = dnum;
  idx->cdb = idx->dbs[dnum-1];
  return idx;
}


/* Close the inverted index.
   `idx' specifies an object of the inverted index.
   The return value is true if success, else it is false. */
static int est_idx_close(ESTIDX *idx){
  int i, err;
  assert(idx);
  err = FALSE;
  for(i = 0; i < idx->dnum; i++){
    if(!vlclose(idx->dbs[i])) err = TRUE;
  }
  free(idx->name);
  free(idx);
  return err ? FALSE : TRUE;
}


/* Set the tuning parameters of the inverted index.
   `idx' specifies an object of the inverted index.
   `lrecmax' specifies the max number of records in a leaf node of B+ tree.
   `nidxmax' specifies the max number of indexes in a non-leaf node of B+ tree.
   `lcnum' specifies the max number of caching leaf nodes.
   `ncnum' specifies the max number of caching non-leaf nodes.
   `fbpsiz' specifies the size of the free block pool.
   Other parameters are same with `vlsettuning' of Villa. */
static void est_idx_set_tuning(ESTIDX *idx, int lrecmax, int nidxmax, int lcnum, int ncnum,
                               int fbpsiz){
  int i;
  assert(idx);
  for(i = 0; i < idx->dnum; i++){
    vlsettuning(idx->dbs[i], lrecmax, nidxmax, lcnum, ncnum);
    if(fbpsiz > 0) vlsetfbpsiz(idx->dbs[i], fbpsiz);
  }
}


/* Increment the inverted index.
   `idx' specifies an object of the inverted index. */
static void est_idx_increment(ESTIDX *idx){
  char path[ESTPATHBUFSIZ];
  int i, min, size, crdnum;
  assert(idx);
  min = INT_MAX;
  for(i = 0; i < idx->dnum; i++){
    size = vlfsiz(idx->cdb);
    if(size < min) min = size;
  }
  if(idx->dnum >= ESTIDXDMAX || (idx->dnum >= ESTIDXDSTD && min < ESTIDXDBMAX)){
    est_idx_set_current(idx);
    return;
  }
  sprintf(path, "%s%c%04d", idx->name, ESTPATHCHR, idx->dnum + 1);
  crdnum = vlcrdnum;
  vlcrdnum = ESTVLCRDNUM;
  if((idx->dbs[idx->dnum] = vlopen(path, idx->omode | VL_OCREAT | VL_OTRUNC, VL_CMPLEX)) != NULL){
    idx->cdb = idx->dbs[idx->dnum];
    idx->dnum++;
  }
  vlcrdnum = crdnum;
}


/* Get the number of files of the inverted index.
   The return the number of files of the inverted index. */
static int est_idx_dnum(ESTIDX *idx){
  assert(idx);
  return idx->dnum;
}


/* Add a record to the inverted index.
   `idx' specifies an object of the inverted index.
   `word' specifies a word.
   `vbuf' specifies the pointer to the value of a record.
   `vsiz' specifies the size of the value.
   `smode' specifies a mode of score type.
   The return value is true if success, else it is false. */
static int est_idx_add(ESTIDX *idx, const char *word, int wsiz,
                       const char *vbuf, int vsiz, int smode){
  CBDATUM *datum;
  const char *obuf;
  int rv, lid, osiz;
  assert(idx && word && wsiz >= 0 && vbuf && vsiz >= 0);
  CB_DATUMOPEN(datum);
  lid = 0;
  if((obuf = vlgetcache(idx->cdb, word, wsiz, &osiz)) != NULL)
    lid = est_idx_rec_last_id(obuf, osiz, smode);
  est_encode_idx_rec(datum, vbuf, vsiz, lid, smode);
  rv = vlput(idx->cdb, word, wsiz, CB_DATUMPTR(datum), CB_DATUMSIZE(datum), VL_DCAT);
  CB_DATUMCLOSE(datum);
  return rv;
}


/* Store a record to a file of the inverted index.
   `idx' specifies an object of the inverted index.
   `inum' specifies the index of a file of the inverted index.
   `word' specifies a word.
   `vbuf' specifies the pointer to the value of a record.
   `vsiz' specifies the size of the value.
   The return value is true if success, else it is false. */
static int est_idx_put_one(ESTIDX *idx, int inum, const char *word, int wsiz,
                           const char *vbuf, int vsiz){
  assert(idx && inum >= 0 && word && wsiz >= 0 && vbuf && vsiz >= 0);
  return vsiz > 0 ? vlput(idx->dbs[inum], word, wsiz, vbuf, vsiz, VL_DOVER) :
    (vlout(idx->dbs[inum], word, wsiz) || dpecode == DP_ENOITEM);
}


/* Remove a record from the inverted index.
   `idx' specifies an object of the inverted index.
   `word' specifies a word.
   `wsiz' specifies the size of the word.
   The return value is true if success, else it is false.  Even if no item correspongs, it is
   success. */
static int est_idx_out(ESTIDX *idx, const char *word, int wsiz){
  int i, err;
  assert(idx && word && wsiz >= 0);
  err = FALSE;
  for(i = 0; i < idx->dnum; i++){
    if(!vlout(idx->dbs[i], word, wsiz) && dpecode != DP_ENOITEM) err = TRUE;
  }
  return err ? FALSE : TRUE;
}


/* Get a record from the inverted index.
   `idx' specifies an object of the inverted index.
   `word' specifies a word.
   `wsiz' specifies the size of the word.
   `sp' specifies the pointer to a variable to which the size of the region of the return value
   is assigned.
   `smode' specifies a mode of score type.
   The return value is the pointer to the region of the value of the corresponding record.
   if no item correspongs, empty region is returned. */
static char *est_idx_scan(ESTIDX *idx, const char *word, int wsiz, int *sp, int smode){
  CBDATUM *datum;
  const char *vbuf;
  int i, vsiz;
  assert(idx && word && wsiz >= 0 && sp);
  CB_DATUMOPEN(datum);
  for(i = 0; i < idx->dnum; i++){
    if(!(vbuf = vlgetcache(idx->dbs[i], word, wsiz, &vsiz))) continue;
    est_decode_idx_rec(datum, vbuf, vsiz, smode);
  }
  return cbdatumtomalloc(datum, sp);
}


/* Get a record from a file of the inverted index.
   `idx' specifies an object of the inverted index.
   `inum' specifies the index of a file of the inverted index.
   `word' specifies a word.
   `wsiz' specifies the size of the word.
   `sp' specifies the pointer to a variable to which the size of the region of the return value
   is assigned.
   The return value is the pointer to the region of the value of the corresponding record.
   if no item correspongs, `NULL' is returned.  Because the region of the return value is
   volatile, it sould be copied immediately. */
static const char *est_idx_get_one(ESTIDX *idx, int inum, const char *word, int wsiz, int *sp){
  assert(idx && inum >= 0 && word && wsiz >= 0 && sp);
  return vlgetcache(idx->dbs[inum], word, wsiz, sp);
}


/* Get the size of the value of a record in the inverted index.
   `idx' specifies an object of the inverted index.
   `word' specifies a word.
   `wsiz' specifies the size of the word.
   The return value is the size of the value of the corresponding record.
   if no item correspongs, 0 is returned. */
static int est_idx_vsiz(ESTIDX *idx, const char *word, int wsiz){
  int i, sum, vsiz;
  assert(idx && word && wsiz >= 0);
  sum = 0;
  for(i = 0; i < idx->dnum; i++){
    if((vsiz = vlvsiz(idx->dbs[i], word, wsiz)) < 1) continue;
    sum += vsiz;
  }
  return sum;
}


/* Get the number of division of the inverted index.
   `idx' specifies an object of the inverted index.
   The return value is the number of division of the inverted index. */
static int est_idx_num(ESTIDX *idx){
  assert(idx);
  return idx->dnum;
}


/* Get the size of the inverted index.
   `idx' specifies an object of the inverted index.
   The return value is the size of the inverted index. */
static double est_idx_size(ESTIDX *idx){
  int i;
  double size;
  assert(idx);
  size = 0;
  for(i = 0; i < idx->dnum; i++){
    size += vlfsiz(idx->dbs[i]);
  }
  return size;
}


/* Get the size of the current file of the inverted index.
   `idx' specifies an object of the inverted index.
   The return value is the size of the current file of the inverted index. */
static int est_idx_size_current(ESTIDX *idx){
  assert(idx);
  return vlfsiz(idx->cdb);
}


/* Synchronize updating contents of the inverted index on memory.
   `idx' specifies an object of the inverted index.
   The return value is true if success, else it is false. */
static int est_idx_memflush(ESTIDX *idx){
  int i;
  assert(idx);
  for(i = 0; i < idx->dnum; i++){
    if(!vlmemflush(idx->dbs[i])) return FALSE;
  }
  return TRUE;
}


/* Syncronize the inverted index.
   `idx' specifies an object of the inverted index.
   The return value is true if success, else it is false. */
static int est_idx_sync(ESTIDX *idx){
  int i;
  assert(idx);
  for(i = 0; i < idx->dnum; i++){
    if(!vlsync(idx->dbs[i])) return FALSE;
  }
  return TRUE;
}


/* Optimize the inverted index.
   `idx' specifies an object of the inverted index.
   The return value is true if success, else it is false. */
static int est_idx_optimize(ESTIDX *idx){
  int i;
  assert(idx);
  for(i = 0; i < idx->dnum; i++){
    if(!vloptimize(idx->dbs[i])) return FALSE;
  }
  return TRUE;
}


/* Set the current database to the smallest one in the inverted index.
   `idx' specifies an object of the inverted index. */
static void est_idx_set_current(ESTIDX *idx){
  int i, size, min;
  assert(idx);
  min = vlfsiz(idx->cdb);
  for(i = 0; i < idx->dnum; i++){
    if((size = vlfsiz(idx->dbs[i])) < min){
      idx->cdb = idx->dbs[i];
      min = size;
    }
  }
}


/* Store a record related to the ID number of a document.
   `curia' specifies a database object.
   `zmode' specifies a compression mode.
   `id' specifies the ID number of a document.
   `vbuf' specifies the pointer to the value of a record.
   `vsiz' specifies the size of the value.
   The return value is true if success, else it is false. */
static int est_crput(CURIA *curia, int zmode, int id, const char *vbuf, int vsiz, int dmode){
  char *zbuf;
  int zsiz;
  assert(curia && id > 0 && vbuf && vsiz >= 0);
  switch(zmode){
  case ESTDFZLIB:
    if(!(zbuf = est_deflate(vbuf, vsiz, &zsiz, -1))){
      dpecode = ESTEMISC;
      return FALSE;
    }
    if(!crput(curia, (char *)&id, sizeof(int), zbuf, zsiz, dmode)){
      free(zbuf);
      return FALSE;
    }
    free(zbuf);
    break;
  case ESTDFLZO:
    if(!(zbuf = est_lzoencode(vbuf, vsiz, &zsiz))){
      dpecode = ESTEMISC;
      return FALSE;
    }
    if(!crput(curia, (char *)&id, sizeof(int), zbuf, zsiz, dmode)){
      free(zbuf);
      return FALSE;
    }
    free(zbuf);
    break;
  case ESTDFBZIP:
    if(!(zbuf = est_bzencode(vbuf, vsiz, &zsiz))){
      dpecode = ESTEMISC;
      return FALSE;
    }
    if(!crput(curia, (char *)&id, sizeof(int), zbuf, zsiz, dmode)){
      free(zbuf);
      return FALSE;
    }
    free(zbuf);
    break;
  default:
    if(!crput(curia, (char *)&id, sizeof(int), vbuf, vsiz, dmode)) return FALSE;
    break;
  }
  return TRUE;
}


/* Remove a record related to the ID number of a document.
   `curia' specifies a database object.
   `id' specifies the ID number of a document.
   The return value is true if success, else it is false. */
static int est_crout(CURIA *curia, int id){
  assert(curia && id > 0);
  return crout(curia, (char *)&id, sizeof(int));
}


/* Get a record related to the ID number of a document.
   `curia' specifies a database object.
   `zmode' specifies a compression mode.
   `id' specifies the ID number of a document.
   `sp' specifies the pointer to a variable to which the size of the region of the return value
   is assigned.
   The return value is the pointer to the region of the value of the corresponding record. */
static char *est_crget(CURIA *curia, int zmode, int id, int *sp){
  char *zbuf, *vbuf;
  int zsiz;
  assert(curia && id > 0 && sp);
  switch(zmode){
  case ESTDFZLIB:
    if(!(zbuf = crget(curia, (char *)&id, sizeof(int), 0, -1, &zsiz))) return NULL;
    if(!(vbuf = est_inflate(zbuf, zsiz, sp, -1))){
      free(zbuf);
      return NULL;
    }
    free(zbuf);
    break;
  case ESTDFLZO:
    if(!(zbuf = crget(curia, (char *)&id, sizeof(int), 0, -1, &zsiz))) return NULL;
    if(!(vbuf = est_lzodecode(zbuf, zsiz, sp))){
      free(zbuf);
      return NULL;
    }
    free(zbuf);
    break;
  case ESTDFBZIP:
    if(!(zbuf = crget(curia, (char *)&id, sizeof(int), 0, -1, &zsiz))) return NULL;
    if(!(vbuf = est_bzdecode(zbuf, zsiz, sp))){
      free(zbuf);
      return NULL;
    }
    free(zbuf);
    break;
  default:
    if(!(vbuf = crget(curia, (char *)&id, sizeof(int), 0, -1, sp))) return NULL;
    break;
  }
  return vbuf;
}


/* Add an attribute of a document to a sequencial attribute index.
   `db' specifies a handle of a sequencial attribute index.
   `id' specifies the ID number of a document.
   `vbuf' specifies the pointer to the attribute value.
   `vsiz' specifies the size of the attribute value.
   The return value is true if success, else it is false. */
static int est_aidx_seq_put(DEPOT *db, int id, const char *vbuf, int vsiz){
  int err;
  assert(db && id >= 0 && vbuf && vsiz >= 0);
  err = FALSE;
  if(!dpput(db, (char *)&id, sizeof(int), vbuf, vsiz, DP_DKEEP)) err = TRUE;
  return err ? FALSE : TRUE;
}


/* Remove an attribute of a document from a sequencial attribute index.
   `db' specifies a handle of a sequencial attribute index.
   `id' specifies the ID number of a document.
   The return value is true if success, else it is false. */
static int est_aidx_seq_out(DEPOT *db, int id){
  int err;
  assert(db && id >= 0);
  err = FALSE;
  if(!dpout(db, (char *)&id, sizeof(int))) err = TRUE;
  return err ? FALSE : TRUE;
}


/* Retrieve the value of an attribute of a document in a sequencial attribute index.
   `db' specifies a handle of a sequencial attribute index.
   `id' specifies the ID number of a document.
   The return value is the value of the attribute or `NULL' if no attribute. */
static char *est_aidx_seq_get(DEPOT *db, int id, int *sp){
  assert(db && id >= 0 && sp);
  return dpget(db, (char *)&id, sizeof(int), 0, -1, sp);
}


/* Narrow scores of search candidates with a sequencial attribute index.
   `db' specifies a handle of a sequencial attribute index.
   `pdocs' specifies a list of pseudo documents.
   `cop' specifies the pointer to the operator.
   `sign' specifies the sign of operation.
   `oval' specifies the operation value.
   `osiz' specifies the size of the operation value
   `sval' specifies the operation value of small cases.
   `ssiz' specifies the size of the operation value of small cases.
   `regex' specifies the regular expressions.
   `onum' specifies the numeric value.
   `scores' specifies an array of scores of search candidates.
   `snum' specifies the number of the array.
   `limit' specifies the limit number to check.
   `restp' specifies the pointer to a variable to which rest number to be checked is assigned.
   The return value is the new number of the array. */
static int est_aidx_seq_narrow(DEPOT *db, const CBLIST *pdocs, const char *cop, int sign,
                               const char *oval, int osiz, const char *sval, int ssiz,
                               const void *regex, int onum, ESTSCORE *scores, int snum,
                               int limit, int *restp){
  char vbuf[ESTAIKBUFSIZ];
  int i, nnum, vsiz;
  assert(db && cop && oval && osiz >= 0 && scores && snum >= 0 && limit >= 0 && restp);
  nnum = 0;
  for(i = 0; i < snum; i++){
    if(nnum >= limit){
      *restp = snum - i;
      break;
    }
    if(scores[i].id >= ESTPDOCIDMIN){
      scores[nnum].id = scores[i].id;
      scores[nnum].score = scores[i].score;
      nnum++;
      continue;
    }
    if((vsiz = dpgetwb(db, (char *)&(scores[i].id), sizeof(int), 0, ESTAIKBUFSIZ - 1, vbuf)) < 0)
      continue;
    vbuf[vsiz] = '\0';
    if(est_match_attr(vbuf, vsiz, cop, sign, oval, osiz, sval, ssiz, regex, onum)){
      scores[nnum].id = scores[i].id;
      scores[nnum].score = scores[i].score;
      nnum++;
    }
  }
  return nnum;
}


/* Compare two record in numeric order.
   `aptr' specifies the pointer to the region of one key.
   `asiz' specifies the size of the region of one key.
   `bptr' specifies the pointer to the region of the other key.
   `bsiz' specifies the size of the region of the other key.
   The return value is positive if the former is big, negative if the latter is big, 0 if both
   are equivalent. */
static int est_aidx_numcmp(const char *aptr, int asiz, const char *bptr, int bsiz){
  int rv;
  if((rv = cbstrmktime(aptr) - cbstrmktime(bptr)) != 0) return rv;
  return VL_CMPLEX(aptr, asiz, bptr, bsiz);
}


/* Add an attribute of a document to an attribute narrowing index.
   `db' specifies a handle of an attribute narrowing index.
   `id' specifies the ID number of a document.
   `vbuf' specifies the pointer to the attribute value.
   `vsiz' specifies the size of the attribute value.
   The return value is true if success, else it is false. */
static int est_aidx_attr_put(VILLA *db, int id, const char *vbuf, int vsiz){
  char *tbuf;
  int err, tsiz;
  assert(db && id >= 0 && vbuf && vsiz >= 0);
  err = FALSE;
  tsiz = vsiz + sizeof(int) + 1;
  CB_MALLOC(tbuf, tsiz);
  memcpy(tbuf, vbuf, vsiz + 1);
  memcpy(tbuf + vsiz + 1, &id, sizeof(int));
  if(!vlput(db, tbuf, tsiz, "", 0, VL_DKEEP)) err = TRUE;
  free(tbuf);
  return err ? FALSE : TRUE;
}


/* Remove an attribute of a document from an attribute narrowing index.
   `db' specifies a handle of an attribute narrowing index.
   `id' specifies the ID number of a document.
   `vbuf' specifies the pointer to the attribute value.
   `vsiz' specifies the size of the attribute value.
   The return value is true if success, else it is false. */
static int est_aidx_attr_out(VILLA *db, int id, const char *vbuf, int vsiz){
  char *tbuf;
  int err, tsiz;
  assert(db && id >= 0 && vbuf && vsiz >= 0);
  err = FALSE;
  tsiz = vsiz + sizeof(int) + 1;
  CB_MALLOC(tbuf, tsiz);
  memcpy(tbuf, vbuf, vsiz + 1);
  memcpy(tbuf + vsiz + 1, &id, sizeof(int));
  if(!vlout(db, tbuf, tsiz)) err = TRUE;
  free(tbuf);
  return err ? FALSE : TRUE;
}


/* Narrow scores of search candidates with an attribute narrowing index.
   `db' specifies a handle of an attribute narrowing index.
   `pdocs' specifies a list of pseudo documents.
   `cop' specifies the pointer to the operator.
   `sign' specifies the sign of operation.
   `oval' specifies the operation value.
   `osiz' specifies the size of the operation value
   `sval' specifies the operation value of small cases.
   `ssiz' specifies the size of the operation value of small cases.
   `regex' specifies the regular expressions.
   `onum' specifies the numeric value.
   `scores' specifies an array of scores of search candidates.
   `snum' specifies the number of the array.
   The return value is the new number of the array. */
static int est_aidx_attr_narrow(VILLA *db, const CBLIST *pdocs, const char *cop, int sign,
                                const char *oval, int osiz, const char *sval, int ssiz,
                                const void *regex, int onum, ESTSCORE *scores, int snum){
  CBDATUM *abuf;
  CBLIST *tokens;
  const char *kbuf;
  char numbuf[ESTNUMBUFSIZ], *tmp, *wp;
  int i, j, ksiz, len, esc, jmp, id, nnum, *ary, anum;
  time_t lower, upper;
  assert(db && pdocs && cop && oval && osiz >= 0 && scores && snum >= 0);
  CB_DATUMOPEN(abuf);
  if(cop == ESTOPSTROREQ && sign && !sval){
    tokens = cbsplit(oval, osiz, " ,");
    cblistsort(tokens);
    for(i = 0; i < CB_LISTNUM(tokens); i++){
      oval = CB_LISTVAL2(tokens, i, osiz);
      if(osiz < 1) continue;
      vlcurjump(db, oval, osiz, VL_JFORWARD);
      while((kbuf = vlcurkeycache(db, &ksiz)) != NULL && !strcmp(kbuf, oval)){
        CB_DATUMCAT(abuf, kbuf + ksiz - sizeof(int), sizeof(int));
        vlcurnext(db);
      }
    }
    CB_LISTCLOSE(tokens);
  } else if(cop == ESTOPNUMBT && sign && !sval){
    CB_MEMDUP(tmp, oval, osiz);
    if((wp = strchr(tmp, ' ')) != NULL || (wp = strchr(tmp, '\t')) != NULL){
      *(wp++) = '\0';
      while(*wp == ' ' || *wp == '\t'){
        wp++;
      }
      lower = cbstrmktime(tmp);
      upper = cbstrmktime(wp);
    } else {
      lower = cbstrmktime(tmp);
      upper = INT_MAX;
    }
    len = sprintf(numbuf, "%.0f", (double)lower);
    vlcurjump(db, numbuf, len, VL_JFORWARD);
    while((kbuf = vlcurkeycache(db, &ksiz)) != NULL && cbstrmktime(kbuf) <= upper){
      CB_DATUMCAT(abuf, kbuf + ksiz - sizeof(int), sizeof(int));
      vlcurnext(db);
    }
    free(tmp);
  } else if(!sign || sval){
    esc = INT_MAX;
    jmp = INT_MAX;
    if(sign && (cop == ESTOPSTREQ || cop == ESTOPSTRBW) && osiz > 0){
      if(*sval > 0x0 && *sval < 0x7f){
        numbuf[0] = *sval;
        numbuf[1] = '\0';
        esc = *(unsigned char *)sval;
        if(*sval >= 'a' && *sval <= 'z'){
          numbuf[0] -= 'a' - 'A';
          jmp = *sval - 'a' + 'A';
        }
        vlcurjump(db, numbuf, 1, VL_JFORWARD);
      } else if(*(unsigned char *)sval >= 0xc0){
        numbuf[0] = *sval;
        numbuf[1] = '\0';
        esc = *(unsigned char *)sval;
        vlcurjump(db, numbuf, 1, VL_JFORWARD);
      } else {
        vlcurfirst(db);
      }
    } else {
      vlcurfirst(db);
    }
    while((kbuf = vlcurkeycache(db, &ksiz)) != NULL){
      if(est_match_attr(kbuf, ksiz - sizeof(int) - 1,
                        cop, sign, oval, osiz, sval, ssiz, regex, onum))
        CB_DATUMCAT(abuf, kbuf + ksiz - sizeof(int), sizeof(int));
      if(*(unsigned char *)kbuf > jmp && *(unsigned char *)kbuf < *(unsigned char *)sval){
        numbuf[0] = *sval;
        numbuf[1] = '\0';
        vlcurjump(db, numbuf, 1, VL_JFORWARD);
        jmp = INT_MAX;
      } else if(*(unsigned char *)kbuf > esc){
        break;
      } else {
        vlcurnext(db);
      }
    }
  } else {
    if(cop == ESTOPSTREQ || cop == ESTOPSTRBW ||
       cop == ESTOPNUMEQ || cop == ESTOPNUMGT || cop == ESTOPNUMGE){
      vlcurjump(db, oval, osiz, VL_JFORWARD);
      if(cop == ESTOPNUMGT){
        while((kbuf = vlcurkeycache(db, NULL)) != NULL && cbstrmktime(kbuf) <= onum){
          vlcurnext(db);
        }
      }
    } else if(cop == ESTOPNUMLT || cop == ESTOPNUMLE){
      len = sprintf(numbuf, "%.0f", (double)cbstrmktime(oval) + 1);
      vlcurjump(db, numbuf, len, VL_JBACKWARD);
      if(cop == ESTOPNUMLT){
        while((kbuf = vlcurkeycache(db, NULL)) != NULL && cbstrmktime(kbuf) >= onum){
          vlcurprev(db);
        }
      }
    } else {
      vlcurfirst(db);
    }
    while((kbuf = vlcurkeycache(db, &ksiz)) != NULL){
      if(est_match_attr(kbuf, ksiz - sizeof(int) - 1,
                        cop, TRUE, oval, osiz, sval, ssiz, regex, onum)){
        CB_DATUMCAT(abuf, kbuf + ksiz - sizeof(int), sizeof(int));
      } else if(cop == ESTOPSTREQ || cop == ESTOPSTRBW || cop == ESTOPNUMEQ){
        break;
      }
      if(cop == ESTOPNUMLT || cop == ESTOPNUMLE){
        vlcurprev(db);
      } else {
        vlcurnext(db);
      }
    }
  }
  for(i = 0; i < CB_LISTNUM(pdocs); i++){
    id = ESTPDOCIDMIN + i;
    CB_DATUMCAT(abuf, &id, sizeof(int));
  }
  nnum = 0;
  ary = (int *)CB_DATUMPTR(abuf);
  anum = CB_DATUMSIZE(abuf) / sizeof(int);
  qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_id_asc);
  qsort(ary, anum, sizeof(int), est_int_compare);
  for(i = 0, j = 0; i < snum; i++){
    while(j < anum && ary[j] < scores[i].id){
      j++;
    }
    if(j < anum && scores[i].id == ary[j]){
      scores[nnum].id = scores[i].id;
      scores[nnum].score = scores[i].score;
      nnum++;
    }
  }
  CB_DATUMCLOSE(abuf);
  return nnum;
}


/* Compare two integers.
   `ap' specifies the pointer to one element.
   `bp' specifies the pointer to the other element.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_int_compare(const void *ap, const void *bp){
  assert(ap && bp);
  return *(int *)ap - *(int *)bp;
}


/* Compare elements of a record for effective compression.
   `ap' specifies the pointer to one element.
   `bp' specifies the pointer to the other element.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_short_compare(const void *ap, const void *bp){
  assert(ap && bp);
  return ((((unsigned char *)ap)[0] << 8) + ((unsigned char *)ap)[1]) -
    ((((unsigned char *)bp)[0] << 8) + ((unsigned char *)bp)[1]);
}


/* Clean up the inode map.
   `arg' specifies a dummy argument. */
static void est_inodes_delete(void *arg){
#if defined(NDEBUG)
  ESTDB *db;
  const char *kbuf;
  int ecode;
  assert(arg);
  if(cbmaprnum(est_inodes) > 0){
    cbmapiterinit(est_inodes);
    while((kbuf = cbmapiternext(est_inodes, NULL)) != NULL){
      db = *(ESTDB **)cbmapiterval(kbuf, NULL);
      est_db_set_informer(db, est_inodes_delete_informer, NULL);
      est_db_close(db, &ecode);
    }
  }
  cbmapclose(est_inodes);
#else
  ESTDB *db;
  const char *kbuf;
  int ecode;
  assert(arg);
  if(cbmaprnum(est_inodes) > 0){
    cbmapiterinit(est_inodes);
    while((kbuf = cbmapiternext(est_inodes, NULL)) != NULL){
      db = *(ESTDB **)cbmapiterval(kbuf, NULL);
      fprintf(stderr, "\nWARNING: %s is not closed.\n\n", cbmemdup(est_db_name(db), -1));
      est_db_set_informer(db, est_inodes_delete_informer, NULL);
      est_db_close(db, &ecode);
    }
  }
  cbmapclose(est_inodes);
#endif
}


/* Inform a database event while clening up database handles.
   `msg' specifies the message of each event.
   `opaque' is ignored. */
static void est_inodes_delete_informer(const char *msg, void *opaque){
#if !defined(NDEBUG)
  fprintf(stderr, "estraier: %s\n", msg);
#endif
}


/* Write meta data to the database.
   `db' specifies a database object.
   The return value is true if success, else it is false. */
static int est_db_write_meta(ESTDB *db){
  char vbuf[ESTNUMBUFSIZ], *sbuf;
  int err, ssiz;
  assert(db);
  err = FALSE;
  sprintf(vbuf, "%d", est_idx_num(db->idxdb));
  if(!dpput(db->metadb, ESTKEYIDXNUM, -1, vbuf, -1, DP_DOVER)) err = TRUE;
  sprintf(vbuf, "%d", db->dseq);
  if(!dpput(db->metadb, ESTKEYDSEQ, -1, vbuf, -1, DP_DOVER)) err = TRUE;
  sprintf(vbuf, "%d", db->dnum);
  if(!dpput(db->metadb, ESTKEYDNUM, -1, vbuf, -1, DP_DOVER)) err = TRUE;
  if(db->metacc){
    sbuf = cbmapdump(db->metacc, &ssiz);
    if(!dpput(db->metadb, ESTKEYMETA, -1, sbuf, ssiz, DP_DOVER)) err = TRUE;
    free(sbuf);
  }
  if(err){
    est_set_ecode(&(db->ecode), ESTEDB, __LINE__);
    db->fatal = TRUE;
  }
  return err ? FALSE : TRUE;
}


/* Call the callback function of a database.
   `db' specifies a database object.
   `info' specifies an extra message. */
static void est_db_inform(ESTDB *db, const char *info){
  char *msg;
  assert(db && info);
  if(!db->infocb) return;
  msg = cbsprintf("%s: name=%s dnum=%d wnum=%d fsiz=%.0f crnum=%d csiz=%d dknum=%d",
                  info, db->name, db->dnum, vlrnum(db->fwmdb), (double)est_db_size(db),
                  cbmaprnum(db->idxcc) + cbmaprnum(db->auxcc), est_db_used_cache_size(db),
                  cbmaprnum(db->outcc));
  db->infocb(msg, db->infoop);
  free(msg);
}


/* Prepare cache for meta data.
   `db' specifies a database object. */
static void est_db_prepare_meta(ESTDB *db){
  char *sbuf;
  int ssiz;
  assert(db);
  if((sbuf = dpget(db->metadb, ESTKEYMETA, -1, 0, -1, &ssiz)) != NULL){
    db->metacc = cbmapload(sbuf, ssiz);
    free(sbuf);
  } else {
    db->metacc = cbmapopenex(ESTMINIBNUM);
  }
}


/* Score a document object matching the phrase of a search condition object definitely.
   `db' specifies a database object.
   `doc' specifies a document object.
   `cond' specifies a search condition object.
   `scp' specifies the pointer to a variable to which the score is assigned.
   The return value is true if the document matches the phrase of the condition object
   definitely, else it is false. */
static int est_db_score_doc(ESTDB *db, ESTDOC *doc, ESTCOND *cond, int *scp){
  struct { char *word; int num; } wsets[ESTSCANWNUM], nsets[ESTSCANWNUM];
  CBLIST *terms, *words;
  const char *term, *text, *rp;
  unsigned char *rbuf;
  char *tmp;
  int i, j, k, sc, wsnum, nsnum, asiz, tsiz, add, rsiz, hit;
  double tune;
  assert(db && doc && cond && scp);
  *scp = 0;
  if(!cond->phrase || cbstrfwmatch(cond->phrase, ESTOPSIMILAR) ||
     cbstrfwmatch(cond->phrase, ESTOPID) || cbstrfwmatch(cond->phrase, ESTOPURI)) return FALSE;
  if(!doc->dtexts) CB_LISTOPEN(doc->dtexts);
  switch(cond->pmode){
  default:
    terms = est_phrase_terms(cond->phrase);
    break;
  case ESTPMSIMPLE:
    tmp = est_phrase_from_simple(cond->phrase);
    terms = est_phrase_terms(tmp);
    free(tmp);
    break;
  case ESTPMROUGH:
    tmp = est_phrase_from_rough(cond->phrase);
    terms = est_phrase_terms(tmp);
    free(tmp);
    break;
  case ESTPMUNION:
    tmp = est_phrase_from_union(cond->phrase);
    terms = est_phrase_terms(tmp);
    free(tmp);
    break;
  case ESTPMISECT:
    tmp = est_phrase_from_isect(cond->phrase);
    terms = est_phrase_terms(tmp);
    free(tmp);
    break;
  }
  wsnum = 0;
  nsnum = 0;
  add = TRUE;
  for(i = 0; i < CB_LISTNUM(terms); i++){
    term = CB_LISTVAL(terms, i);
    if(!strcmp(term, ESTOPISECT)){
      add = TRUE;
    } else if(!strcmp(term, ESTOPDIFF)){
      add = FALSE;
    } else if(strcmp(term, ESTOPUVSET)){
      if(term[0] == ' '){
        term++;
        if(term[0] == 'b'){
          term++;
        } else  if(term[0] == 'e'){
          term++;
        }
      }
      words = cbsplit(term, -1, "\t");
      if(add){
        while(wsnum < ESTSCANWNUM && CB_LISTNUM(words) > 0){
          wsets[wsnum].word = cblistshift(words, NULL);
          wsets[wsnum].num = i;
          wsnum++;
        }
      } else {
        while(nsnum < ESTSCANWNUM && CB_LISTNUM(words) > 0){
          nsets[nsnum].word = cblistshift(words, NULL);
          nsets[nsnum].num = i;
          nsnum++;
        }
      }
      CB_LISTCLOSE(words);
    }
  }
  asiz = 0;
  sc = 0;
  if((rp = cbmapget(doc->attrs, "\t", 1, NULL)) != NULL) sc = -1 - atoi(rp);
  for(i = -1; i < CB_LISTNUM(doc->dtexts); i++){
    if(i < 0){
      if(!doc->attrs || !(text = cbmapget(doc->attrs, "", 0, NULL))) continue;
      asiz += strlen(text);
    } else {
      text = CB_LISTVAL2(doc->dtexts, i, tsiz);
      asiz += tsiz;
    }
    rbuf = (unsigned char *)est_uconv_in(text, strlen(text), &rsiz);
    est_canonicalize_text(rbuf, rsiz, FALSE);
    tmp = est_uconv_out((char *)rbuf, rsiz, &rsiz);
    for(j = 0; j < wsnum; j++){
      if(!wsets[j].word) continue;
      if((rp = est_strstr_sparse(tmp, wsets[j].word)) != NULL){
        if(sc >= 0){
          do {
            sc += 16;
            rp += strlen(wsets[j].word);
          } while((rp = est_strstr_sparse(rp, wsets[j].word)) != NULL);
        }
        for(k = 0; k < wsnum; k++){
          if(!wsets[k].word) continue;
          if(wsets[k].num == wsets[j].num){
            free(wsets[k].word);
            wsets[k].word = NULL;
          }
        }
      }
    }
    for(j = 0; j < nsnum; j++){
      if(!nsets[j].word) continue;
      if((rp = est_strstr_sparse(tmp, nsets[j].word)) != NULL){
        for(k = 0; k < nsnum; k++){
          if(!nsets[k].word) continue;
          if(nsets[k].num == nsets[j].num){
            free(nsets[k].word);
            nsets[k].word = NULL;
          }
        }
      }
    }
    free(tmp);
    free(rbuf);
  }
  hit = TRUE;
  for(i = 0; i < wsnum; i++){
    if(!wsets[i].word) continue;
    free(wsets[i].word);
    hit = FALSE;
  }
  for(i = 0; i < nsnum; i++){
    if(!nsets[i].word){
      hit = FALSE;
      continue;
    }
    free(nsets[i].word);
  }
  CB_LISTCLOSE(terms);
  if(sc < 0) sc = -1 - sc;
  tune = sqrt(asiz / 8.0 + 128) / 16.0;
  switch(db->smode){
  case ESTDFSCVOID:
    sc = 0;
    break;
  default:
    sc /= tune;
    if(sc >= 0x80) sc += (0x80 - sc) * 0.75;
    if(sc >= 0xc0) sc += (0xc0 - sc) * 0.75;
    sc = sc < 0xff ? sc : 0xff;
    break;
  case ESTDFSCINT:
    sc /= tune;
    break;
  case ESTDFSCASIS:
    break;
  }
  *scp = sc;
  return hit;
}


/* Get the ID of a document specified by URI from pseudo indexes.
   `db' specifies a database object.
   `uri' specifies the URI of a registered document.
   The return value is the ID of the document.  On error, -1 is returned. */
static int est_pidx_uri_to_id(ESTDB *db, const char *uri){
  ESTDOC *doc;
  const char *vbuf;
  int i, vsiz;
  assert(db && uri);
  if(!db->puris){
    db->puris = cbmapopenex(CB_LISTNUM(db->pdocs) + 1);
    for(i = 0; i < CB_LISTNUM(db->pdocs); i++){
      if((doc = est_db_get_doc(db, ESTPDOCIDMIN + i, 0)) != NULL){
        if((vbuf = cbmapget(doc->attrs, ESTDATTRURI, -1, &vsiz)) != NULL)
          cbmapput(db->puris, vbuf, vsiz, (char *)&(doc->id), sizeof(int), FALSE);
        est_doc_delete(doc);
      }
    }
  }
  if((vbuf = cbmapget(db->puris, uri, -1, NULL)) != NULL) return *(int *)vbuf;
  return -1;
}


/* Create a list of terms for search.
   `phrase' specifies a search phrase.
   The return value is a list object of the terms of the phrase. */
static CBLIST *est_phrase_terms(const char *phrase){
  CBLIST *terms, *elems;
  CBDATUM *datum;
  const char *elem;
  char *tbuf, *pbuf;
  int i, tsiz, psiz, lw;
  assert(phrase);
  CB_LISTOPEN(terms);
  tbuf = est_uconv_in(phrase, strlen(phrase), &tsiz);
  est_normalize_text((unsigned char *)tbuf, tsiz, &tsiz);
  pbuf = est_uconv_out(tbuf, tsiz, &psiz);
  elems = cbsplit(pbuf, psiz, "\a\b\t\n\v\f\r ");
  CB_DATUMOPEN(datum);
  lw = FALSE;
  for(i = 0; i < CB_LISTNUM(elems); i++){
    elem = CB_LISTVAL(elems, i);
    if(elem[0] == '\0') continue;
    if(!strcmp(elem, ESTOPUNION)){
      if(CB_DATUMSIZE(datum) < 1) continue;
      if(lw) CB_DATUMCAT(datum, "\t", 1);
      lw = FALSE;
    } else if(!strcmp(elem, ESTOPWCBW)){
      if(!lw) CB_DATUMCAT(datum, " b", 2);
    } else if(!strcmp(elem, ESTOPWCEW)){
      if(!lw) CB_DATUMCAT(datum, " e", 2);
    } else if(!strcmp(elem, ESTOPWCRX)){
      if(!lw) CB_DATUMCAT(datum, " r", 2);
    } else if(!strcmp(elem, ESTOPISECT) || !strcmp(elem, ESTOPDIFF)){
      if(CB_DATUMSIZE(datum) < 1) continue;
      CB_LISTPUSH(terms, CB_DATUMPTR(datum), CB_DATUMSIZE(datum));
      CB_DATUMSETSIZE(datum, 0);
      CB_LISTPUSH(terms, elem, strlen(elem));
      lw = FALSE;
    } else {
      if(CB_DATUMSIZE(datum) > 0 && lw) CB_DATUMCAT(datum, " ", 1);
      CB_DATUMCAT(datum, elem, strlen(elem));
      lw = TRUE;
    }
  }
  if(CB_DATUMSIZE(datum) > 0) CB_LISTPUSH(terms, CB_DATUMPTR(datum), CB_DATUMSIZE(datum));
  CB_DATUMCLOSE(datum);
  CB_LISTCLOSE(elems);
  free(pbuf);
  free(tbuf);
  for(i = 0; i < CB_LISTNUM(terms); i++){
    elem = CB_LISTVAL(terms, i);
    if(!strcmp(elem, ESTOPUVSET) || !strcmp(elem, ESTOPISECT) ||
       !strcmp(elem, ESTOPDIFF)) continue;
    tbuf = est_uconv_in(elem, strlen(elem), &tsiz);
    est_canonicalize_text((unsigned char *)tbuf, tsiz, TRUE);
    pbuf = est_uconv_out(tbuf, tsiz, &psiz);
    cblistover(terms, i, pbuf, -1);
    free(pbuf);
    free(tbuf);
  }
  for(i = CB_LISTNUM(terms) - 1; i >= 0; i--){
    elem = CB_LISTVAL(terms, i);
    if(strcmp(elem, ESTOPISECT) && strcmp(elem, ESTOPDIFF)) break;
    CB_LISTDROP(terms);
  }
  return terms;
}


/* Compare two scores by each ID for ascending order.
   `ap' specifies the pointer to one score.
   `bp' specifies the pointer to the other score.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_score_compare_by_id_asc(const void *ap, const void *bp){
  assert(ap && bp);
  return ((ESTSCORE *)ap)->id - ((ESTSCORE *)bp)->id;
}


/* Compare two scores by each ID for descending order.
   `ap' specifies the pointer to one score.
   `bp' specifies the pointer to the other score.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_score_compare_by_id_desc(const void *ap, const void *bp){
  assert(ap && bp);
  return ((ESTSCORE *)bp)->id - ((ESTSCORE *)ap)->id;
}


/* Compare two scores by each score point for ascending order.
   `ap' specifies the pointer to one score.
   `bp' specifies the pointer to the other score.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_score_compare_by_score_asc(const void *ap, const void *bp){
  assert(ap && bp);
  return ((ESTSCORE *)ap)->score - ((ESTSCORE *)bp)->score;
}


/* Compare two scores by each score point for descending order.
   `ap' specifies the pointer to one score.
   `bp' specifies the pointer to the other score.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_score_compare_by_score_desc(const void *ap, const void *bp){
  assert(ap && bp);
  return ((ESTSCORE *)bp)->score - ((ESTSCORE *)ap)->score;
}


/* Compare two scores by attributes of strings for ascending order.
   `ap' specifies the pointer to one score.
   `bp' specifies the pointer to the other score.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_score_compare_by_str_asc(const void *ap, const void *bp){
  assert(ap && bp);
  return strcmp(((ESTSCORE *)ap)->value, ((ESTSCORE *)bp)->value);
}


/* Compare two scores by attributes of strings for descending order.
   `ap' specifies the pointer to one score.
   `bp' specifies the pointer to the other score.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_score_compare_by_str_desc(const void *ap, const void *bp){
  assert(ap && bp);
  return strcmp(((ESTSCORE *)bp)->value, ((ESTSCORE *)ap)->value);
}


/* Compare two scores by attributes of numbers for ascending order.
   `ap' specifies the pointer to one score.
   `bp' specifies the pointer to the other score.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_score_compare_by_num_asc(const void *ap, const void *bp){
  assert(ap && bp);
  return (time_t)((ESTSCORE *)ap)->value - (time_t)((ESTSCORE *)bp)->value;
}


/* Compare two scores by attributes of numbers for descending order.
   `ap' specifies the pointer to one score.
   `bp' specifies the pointer to the other score.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_score_compare_by_num_desc(const void *ap, const void *bp){
  assert(ap && bp);
  return (time_t)((ESTSCORE *)bp)->value - (time_t)((ESTSCORE *)ap)->value;
}


/* Compare two meta scores by each ID for ascending order.
   `ap' specifies the pointer to one meta score
   `bp' specifies the pointer to the other meta score
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_metascore_compare_by_id_asc(const void *ap, const void *bp){
  assert(ap && bp);
  return ((ESTMETASCORE *)ap)->id - ((ESTMETASCORE *)bp)->id;
}


/* Compare two meta scores by each ID for descending order.
   `ap' specifies the pointer to one meta score
   `bp' specifies the pointer to the other meta score
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_metascore_compare_by_id_desc(const void *ap, const void *bp){
  assert(ap && bp);
  return ((ESTMETASCORE *)bp)->id - ((ESTMETASCORE *)ap)->id;
}


/* Compare two meta scores by each score point for ascending order.
   `ap' specifies the pointer to one meta score
   `bp' specifies the pointer to the other meta score
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_metascore_compare_by_score_asc(const void *ap, const void *bp){
  assert(ap && bp);
  return ((ESTMETASCORE *)ap)->score - ((ESTMETASCORE *)bp)->score;
}


/* Compare two meta scores by each score point for descending order.
   `ap' specifies the pointer to one meta score
   `bp' specifies the pointer to the other meta score
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_metascore_compare_by_score_desc(const void *ap, const void *bp){
  assert(ap && bp);
  return ((ESTMETASCORE *)bp)->score - ((ESTMETASCORE *)ap)->score;
}


/* Compare two meta scores by attributes of strings for ascending order.
   `ap' specifies the pointer to one meta score
   `bp' specifies the pointer to the other meta score
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_metascore_compare_by_str_asc(const void *ap, const void *bp){
  assert(ap && bp);
  return strcmp(((ESTMETASCORE *)ap)->value, ((ESTMETASCORE *)bp)->value);
}


/* Compare two meta scores by attributes of strings for descending order.
   `ap' specifies the pointer to one meta score
   `bp' specifies the pointer to the other meta score
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_metascore_compare_by_str_desc(const void *ap, const void *bp){
  assert(ap && bp);
  return strcmp(((ESTMETASCORE *)bp)->value, ((ESTMETASCORE *)ap)->value);
}


/* Compare two meta scores by attributes of numbers for ascending order.
   `ap' specifies the pointer to one meta score
   `bp' specifies the pointer to the other meta score
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_metascore_compare_by_num_asc(const void *ap, const void *bp){
  assert(ap && bp);
  return (time_t)((ESTMETASCORE *)ap)->value - (time_t)((ESTMETASCORE *)bp)->value;
}


/* Compare two meta scores by attributes of numbers for descending order.
   `ap' specifies the pointer to one meta score
   `bp' specifies the pointer to the other meta score
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_metascore_compare_by_num_desc(const void *ap, const void *bp){
  assert(ap && bp);
  return (time_t)((ESTMETASCORE *)bp)->value - (time_t)((ESTMETASCORE *)ap)->value;
}


/* Get the universal set of documents in a database.
   `db' specifies a database object.
   `nump' specifies the pointer to which the number of elements in the result is assigned.
   `hints' specifies a list object.  If it is `NULL', it is not used.
   `add' specifies whether the result to be treated in union or difference.
   The return value is an array of score structures of corresponding documents. */
static ESTSCORE *est_search_uvset(ESTDB *db, int *nump, CBMAP *hints, int add){
  ESTSCORE *scores;
  char *vbuf, numbuf[ESTNUMBUFSIZ];
  int snum, smax;
  assert(db && nump);
  smax = ESTALLOCUNIT;
  CB_MALLOC(scores, smax * sizeof(ESTSCORE));
  snum = 0;
  vlcurfirst(db->listdb);
  while((vbuf = vlcurval(db->listdb, NULL)) != NULL){
    if(snum >= smax){
      smax *= 2;
      CB_REALLOC(scores, smax * sizeof(ESTSCORE));
    }
    scores[snum].id = atoi(vbuf);
    scores[snum].score = 0;
    scores[snum].value = NULL;
    snum++;
    free(vbuf);
    vlcurnext(db->listdb);
  }
  *nump = snum;
  if(hints){
    sprintf(numbuf, "%d", snum * (add ? 1 : -1));
    cbmapput(hints, ESTOPUVSET, -1, numbuf, -1, TRUE);
  }
  return scores;
}


/* Expand a word to words which begins with it.
   `db' specifies a database object.
   `word' specifies a word.
   `list' specifies a list object to contain the results. */
static void est_expand_word_bw(ESTDB *db, const char *word, CBLIST *list){
  const char *kbuf;
  int num, ksiz;
  assert(db && word && list);
  num = 0;
  vlcurjump(db->fwmdb, word, -1, VL_JFORWARD);
  while((kbuf = vlcurkeycache(db->fwmdb, &ksiz)) != NULL){
    if(!cbstrfwmatch(kbuf, word)) break;
    CB_LISTPUSH(list, kbuf, ksiz);
    if(++num >= db->wildmax) break;
    vlcurnext(db->fwmdb);
  }
}


/* Expand a word to words which ends with it.
   `db' specifies a database object.
   `word' specifies a word.
   `list' specifies a list object to contain the results. */
static void est_expand_word_ew(ESTDB *db, const char *word, CBLIST *list){
  const char *kbuf;
  int num, wsiz, ksiz;
  assert(db && word && list);
  num = 0;
  wsiz = strlen(word);
  vlcurfirst(db->fwmdb);
  while((kbuf = vlcurkeycache(db->fwmdb, &ksiz)) != NULL){
    if(ksiz >= wsiz && !memcmp(kbuf + ksiz - wsiz, word, wsiz)){
      CB_LISTPUSH(list, kbuf, ksiz);
      if(++num >= db->wildmax) break;
    }
    vlcurnext(db->fwmdb);
  }
}


/* Expand regular expressios to words which matches them.
   `db' specifies a database object.
   `word' specifies regular expressions.
   `list' specifies a list object to contain the results. */
static void est_expand_word_rx(ESTDB *db, const char *word, CBLIST *list){
  void *regex;
  const char *kbuf;
  int num, ksiz;
  assert(db && word && list);
  if(!(regex = est_regex_new(word))) return;
  num = 0;
  vlcurfirst(db->fwmdb);
  while((kbuf = vlcurkeycache(db->fwmdb, &ksiz)) != NULL){
    if(est_regex_match(regex, kbuf)){
      CB_LISTPUSH(list, kbuf, ksiz);
      if(++num >= db->wildmax) break;
    }
    vlcurnext(db->fwmdb);
  }
  est_regex_delete(regex);
}


/* Expand a keyword to keywords which begins with it.
   `db' specifies a database object.
   `word' specifies a word.
   `list' specifies a list object to contain the results. */
static void est_expand_keyword_bw(ESTDB *db, const char *word, CBLIST *list){
  const char *kbuf;
  int num, ksiz;
  assert(db && word && list);
  num = 0;
  vlcurjump(db->xfmdb, word, -1, VL_JFORWARD);
  while((kbuf = vlcurkeycache(db->xfmdb, &ksiz)) != NULL){
    if(!cbstrfwmatch(kbuf, word)) break;
    CB_LISTPUSH(list, kbuf, ksiz);
    if(++num >= db->wildmax) break;
    vlcurnext(db->xfmdb);
  }
}


/* Expand a keyword to keywords which ends with it.
   `db' specifies a database object.
   `word' specifies a word.
   `list' specifies a list object to contain the results. */
static void est_expand_keyword_ew(ESTDB *db, const char *word, CBLIST *list){
  const char *kbuf;
  int num, wsiz, ksiz;
  assert(db && word && list);
  num = 0;
  wsiz = strlen(word);
  vlcurfirst(db->xfmdb);
  while((kbuf = vlcurkeycache(db->xfmdb, &ksiz)) != NULL){
    if(ksiz >= wsiz && !memcmp(kbuf + ksiz - wsiz, word, wsiz)){
      CB_LISTPUSH(list, kbuf, ksiz);
      if(++num >= db->wildmax) break;
    }
    vlcurnext(db->xfmdb);
  }
}


/* Expand regular expressios to keywords which matches them.
   `db' specifies a database object.
   `word' specifies regular expressions.
   `list' specifies a list object to contain the results. */
static void est_expand_keyword_rx(ESTDB *db, const char *word, CBLIST *list){
  void *regex;
  const char *kbuf;
  int num, ksiz;
  assert(db && word && list);
  if(!(regex = est_regex_new(word))) return;
  num = 0;
  vlcurfirst(db->xfmdb);
  while((kbuf = vlcurkeycache(db->xfmdb, &ksiz)) != NULL){
    if(est_regex_match(regex, kbuf)){
      CB_LISTPUSH(list, kbuf, ksiz);
      if(++num >= db->wildmax) break;
    }
    vlcurnext(db->xfmdb);
  }
  est_regex_delete(regex);
}


/* Get a correspinding set of documents in a database.
   `db' specifies a database object.
   `term' specifies a union term.
   `gstep' specifies number of steps of N-gram.
   `xpn' specifies the pointer to a function for query expansion.  If it is `NULL', it is not
   used.
   `nump' specifies the pointer to which the number of elements in the result is assigned.
   `hints' specifies a list object.  If it is `NULL', it is not used.
   `add' specifies whether the result to be treated in union or difference.
   `auxmin' specifies the minimum hits to adopt the auxiliary index.  If it is not more than 0,
   the auxiliary index is not used.
   `auxwords' specifies a map object where keywords used with the auxiliary index are stored.  If
   it is `NULL', it is not used.
   The return value is an array of score structures of corresponding documents. */
static ESTSCORE *est_search_union(ESTDB *db, const char *term, int gstep,
                                  void (*xpn)(const char *, CBLIST *),
                                  int *nump, CBMAP *hints, int add, int auxmin, CBMAP *auxwords){
  const ESTSCORE *cscores;
  ESTSCORE *scores, *tscores, *nscores;
  CBMAP *umap;
  CBLIST *words, *grams, *tgrams;
  const char *ckey, *word, *gram, *rp, *fnext, *snext, *cbuf;
  char *vbuf, *wbuf, numbuf[ESTNUMBUFSIZ];
  int i, j, k, snum, smax, cksiz, single, tsmax, tsnum, nsnum, vsiz, gcnum;
  int gsiz, csiz, wgstep, nnum, west, wild, mfsiz, mssiz, mfhash, mshash, tfhash, tshash;
  int id, vstep, score, hit, hnum;
  double avg, sd, dif;
  assert(db && term && gstep > 0 && nump);
  smax = ESTALLOCUNIT;
  CB_MALLOC(scores, smax * sizeof(ESTSCORE));
  snum = 0;
  words = cbsplit(term, -1, "\t");
  if(xpn){
    umap = cbmapopenex(ESTMINIBNUM);
    for(i = 0; i < CB_LISTNUM(words); i++){
      word = CB_LISTVAL(words, i);
      if(word[0] == '\0' || word[0] == ' ') continue;
      CB_LISTOPEN(grams);
      xpn(word, grams);
      for(j = 0; j < CB_LISTNUM(grams); j++){
        word = CB_LISTVAL(grams, j);
        cbmapput(umap, word, -1, "", 0, FALSE);
      }
      CB_LISTCLOSE(grams);
    }
    CB_LISTCLOSE(words);
    words = cbmapkeys(umap);
    cbmapclose(umap);
  }
  for(i = 0; i < CB_LISTNUM(words); i++){
    ckey = CB_LISTVAL2(words, i, cksiz);
    if(cksiz < 1) continue;
    word = ckey;
    wbuf = NULL;
    if((cscores = est_rescc_get(db, ckey, cksiz, &tsnum)) != NULL){
      if(word[0] == ' '){
        word++;
        if(word[0] != '\0') word++;
      }
      if(hints){
        sprintf(numbuf, "%d", tsnum * (add ? 1 : -1));
        cbmapput(hints, word, -1, numbuf, -1, TRUE);
      }
      for(j = 0; j < tsnum; j++){
        if(snum >= smax){
          smax *= 2;
          CB_REALLOC(scores, smax * sizeof(ESTSCORE));
        }
        scores[snum].id = cscores[j].id;
        scores[snum].score = cscores[j].score;
        snum++;
      }
    } else if(!strchr(word + 1, ' ') && auxmin > 0 &&
              (tscores = est_search_keywords(db, word, auxmin, &tsnum)) != NULL){
      if(word[0] == ' '){
        word++;
        if(word[0] != '\0') word++;
      }
      if(hints){
        sprintf(numbuf, "%d", tsnum * (add ? 1 : -1));
        cbmapput(hints, word, -1, numbuf, -1, TRUE);
      }
      if(auxwords) cbmapput(auxwords, word, -1, "", 0, FALSE);
      for(j = 0; j < tsnum; j++){
        if(snum >= smax){
          smax *= 2;
          CB_REALLOC(scores, smax * sizeof(ESTSCORE));
        }
        scores[snum].id = tscores[j].id;
        scores[snum].score = tscores[j].score;
        snum++;
      }
      free(tscores);
    } else {
      wild = '\0';
      if(word[0] == ' '){
        word++;
        if(word[0] == 'b'){
          wild = 'b';
          word++;
        } else  if(word[0] == 'e'){
          wild = 'e';
          word++;
        } else  if(word[0] == 'r'){
          wild = 'r';
          word++;
        }
      }
      west = ((unsigned char *)word)[0] <= 0xdf;
      if(!west || db->amode) wild = '\0';
      single = FALSE;
      CB_LISTOPEN(grams);
      switch(wild){
      case 'b':
        est_break_text(word, grams, TRUE, FALSE);
        CB_LISTPUSH(grams, word, strlen(word));
        while(CB_LISTNUM(grams) > 1){
          CB_LISTDROP(grams);
        }
        wbuf = cbmemdup(CB_LISTVAL(grams, 0), -1);
        word = wbuf;
        est_expand_word_bw(db, word, grams);
        single = TRUE;
        break;
      case 'e':
        est_break_text(word, grams, TRUE, FALSE);
        cblistunshift(grams, word, -1);
        while(CB_LISTNUM(grams) > 1){
          free(cblistshift(grams, NULL));
        }
        wbuf = cbmemdup(CB_LISTVAL(grams, 0), -1);
        word = wbuf;
        est_expand_word_ew(db, word, grams);
        single = TRUE;
        break;
      case 'r':
        est_break_text(word, grams, TRUE, FALSE);
        while(CB_LISTNUM(grams) > 0){
          free(cblistshift(grams, NULL));
        }
        est_expand_word_rx(db, word, grams);
        single = TRUE;
        break;
      default:
        switch(db->amode){
        case ESTDFPERFNG:
          est_break_text_perfng(word, grams, TRUE, FALSE);
          break;
        case ESTDFCHRCAT:
          est_break_text_chrcat(word, grams, TRUE);
          break;
        default:
          est_break_text(word, grams, TRUE, FALSE);
          break;
        }
        if(CB_LISTNUM(grams) < 1){
          est_expand_word_bw(db, word, grams);
          single = TRUE;
        }
        break;
      }
      tsmax = ESTALLOCUNIT;
      CB_MALLOC(tscores, tsmax * sizeof(ESTSCORE));
      tsnum = 0;
      gcnum = 0;
      wgstep = !single && (CB_LISTNUM(grams) > 2 || gstep > 2) ? gstep : 1;
      if(west && gstep <= 2) wgstep = 1;
      for(j = 0; j < CB_LISTNUM(grams); j += wgstep){
        gcnum++;
        gram = CB_LISTVAL2(grams, j, gsiz);
        fnext = cblistval(grams, j + 1, &mfsiz);
        snext = cblistval(grams, j + 2, &mssiz);
        mfhash = fnext ? dpinnerhash(fnext, mfsiz) % ESTJHASHNUM + 1: 0xff;
        mshash = snext ? dpouterhash(snext, mssiz) % ESTJHASHNUM + 1: 0xff;
        vbuf = est_idx_scan(db->idxdb, gram, gsiz, &vsiz, db->smode);
        if((cbuf = cbmapget(db->idxcc, gram, gsiz, &csiz)) != NULL){
          CB_REALLOC(vbuf, vsiz + csiz + 1);
          memcpy(vbuf + vsiz, cbuf, csiz);
          vsiz += csiz;
        }
        rp = vbuf;
        while(rp < vbuf + vsiz){
          EST_READ_VNUMBUF(rp, id, vstep);
          rp += vstep;
          switch(db->smode){
          case ESTDFSCVOID:
            score = 0;
            break;
          default:
            score = *(unsigned char *)rp;
            rp++;
            break;
          case ESTDFSCINT:
          case ESTDFSCASIS:
            memcpy(&score, rp, sizeof(int));
            rp += sizeof(int);
            break;
          }
          hit = mfhash == 0xff && mshash == 0xff;
          while(rp < vbuf + vsiz && *(unsigned char *)rp != 0x00){
            tfhash = *(unsigned char *)rp;
            rp++;
            tshash = *(unsigned char *)rp;
            rp++;
            if((mfhash == 0xff || mfhash == tfhash) && (mshash == 0xff || mshash == tshash))
              hit = TRUE;
          }
          rp++;
          if(hit || single){
            if(tsnum >= tsmax){
              tsmax *= 2;
              CB_REALLOC(tscores, tsmax * sizeof(ESTSCORE));
            }
            tscores[tsnum].id = id;
            switch(db->smode){
            case ESTDFSCVOID:
              tscores[tsnum].score = rp - vbuf;
              break;
            default:
              tscores[tsnum].score = score * 100 + 10;
              break;
            case ESTDFSCASIS:
              tscores[tsnum].score = score;
              break;
            }
            tsnum++;
          }
        }
        free(vbuf);
      }
      if(CB_LISTNUM(grams) == 1 && !single && db->amode == 0 && *(unsigned char *)word < 0xe0){
        CB_LISTOPEN(tgrams);
        est_break_text(word, tgrams, TRUE, TRUE);
        if(CB_LISTNUM(tgrams) == 2){
          gram = CB_LISTVAL(tgrams, 1);
          nscores = est_search_union(db, gram, 1, NULL, &nsnum, NULL, TRUE, -1, NULL);
          for(j = 0; j < nsnum; j++){
            if(tsnum >= tsmax){
              tsmax *= 2;
              CB_REALLOC(tscores, tsmax * sizeof(ESTSCORE));
            }
            tscores[tsnum].id = nscores[j].id;
            tscores[tsnum].score = nscores[j].score;
            tsnum++;
          }
          free(nscores);
          gcnum++;
        }
        CB_LISTCLOSE(tgrams);
      }
      if(gcnum > 1){
        qsort(tscores, tsnum, sizeof(ESTSCORE), est_score_compare_by_id_asc);
        nnum = 0;
        for(j = 0; j < tsnum; j++){
          id = tscores[j].id;
          score = tscores[j].score;
          hnum = 1;
          if(db->smode == ESTDFSCASIS){
            for(k = j + 1; k < tsnum && tscores[k].id == id; k++){
              hnum++;
            }
            if(hnum >= gcnum || single){
              tscores[nnum].id = id;
              tscores[nnum].score = score;
              nnum++;
            }
          } else {
            for(k = j + 1; k < tsnum && tscores[k].id == id; k++){
              score += tscores[k].score;
              hnum++;
            }
            if(hnum >= gcnum || single){
              tscores[nnum].id = id;
              tscores[nnum].score = score / hnum;
              nnum++;
            }
          }
          j = k - 1;
        }
        tsnum = nnum;
      }
      if(hints){
        sprintf(numbuf, "%d", tsnum * (add ? 1 : -1));
        cbmapput(hints, word, -1, numbuf, -1, TRUE);
      }
      CB_LISTCLOSE(grams);
      if(db->smode != ESTDFSCASIS && !strchr(word, ' ') && auxmin > 0)
        est_weight_keywords(db, word, tscores, tsnum);
      for(j = 0; j < tsnum; j++){
        if(snum >= smax){
          smax *= 2;
          CB_REALLOC(scores, smax * sizeof(ESTSCORE));
        }
        scores[snum].id = tscores[j].id;
        scores[snum].score = tscores[j].score;
        snum++;
      }
      est_rescc_put(db, ckey, cksiz, tscores, tsnum);
    }
    free(wbuf);
  }
  CB_LISTCLOSE(words);
  qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_id_asc);
  nnum = 0;
  for(i = 0; i < snum; i++){
    id = scores[i].id;
    score = scores[i].score;
    hnum = 1;
    for(j = i + 1; j < snum && scores[j].id == id; j++){
      score += scores[j].score;
      hnum++;
    }
    scores[nnum].id = id;
    scores[nnum].score = score / hnum;
    scores[nnum].value = NULL;
    nnum++;
    i = j - 1;
  }
  *nump = nnum;
  if(db->smode != ESTDFSCASIS && nnum > 0){
    avg = 0.0;
    for(i = 0; i < nnum; i++){
      avg += scores[i].score;
    }
    avg /= nnum;
    sd = 0.0;
    for(i = 0; i < nnum; i++){
      dif = avg - scores[i].score;
      sd += dif * dif;
    }
    sd /= nnum;
    sd = sqrt(sd);
    if(sd < 0.1){
      for(i = 0; i < nnum; i++){
        scores[i].score = ESTSCOREUNIT / 2;
      }
    } else {
      for(i = 0; i < nnum; i++){
        scores[i].score = (int)(((scores[i].score - avg) * (ESTSCOREUNIT / 10.0) / sd) +
                                ESTSCOREUNIT / 2.0);
      }
    }
  }
  return scores;
}


/* Get scores in the result cache.
   `db' specifies a database object.
   `word' specifies a search word.
   `size' specifies the size of the word.
   `nump' specifies the pointer to which the number of elements in the result is assigned.
   The return value is an array whose elements are ID numbers of corresponding documents. */
static const ESTSCORE *est_rescc_get(ESTDB *db, const char *word, int size, int *nump){
  const char *vbuf;
  int vsiz;
  assert(db && word && size >= 0 && nump);
  if(!(vbuf = cbmapget(db->rescc, word, size, &vsiz))) return NULL;
  if(vsiz == sizeof(ESTSCORE) && ((ESTSCORE *)vbuf)->id == -1) return NULL;
  cbmapmove(db->rescc, word, size, FALSE);
  *nump = vsiz / sizeof(ESTSCORE);
  return (ESTSCORE *)vbuf;
}


/* Add scores into the result cache.
   `db' specifies a database object.
   `word' specifies a search word.
   `size' specifies the size of the word.
   `scores' specifies an array of scores.  It is released in this function.
   `num' specifies the number of elements of the score array. */
static void est_rescc_put(ESTDB *db, const char *word, int size, ESTSCORE *scores, int num){
  int i;
  assert(db && word && size >= 0 && scores && num >= 0);
  if(db->rcmnum < 1){
    free(scores);
    return;
  }
  cbmapput(db->rescc, word, size, (char *)scores, num * sizeof(ESTSCORE), TRUE);
  free(scores);
  if(cbmaprnum(db->rescc) > db->rcmnum){
    num = db->rcmnum * 0.1 + 1;
    cbmapiterinit(db->rescc);
    for(i = 0; i < num && (word = cbmapiternext(db->rescc, &size)) != NULL; i++){
      cbmapout(db->rescc, word, size);
    }
  }
}


/* Search the auxiliary index.
   `db' specifies a database object.
   `word' specifies a search word.
   `min' specifies the minimum hits to adopt the auxiliary index.
   `nump' specifies the pointer to which the number of elements in the result is assigned.
   The return value is an array of score structures of corresponding documents. */
static ESTSCORE *est_search_keywords(ESTDB *db, const char *word, int min, int *nump){
  ESTSCORE *scores;
  CBLIST *words;
  CBDATUM *rbuf;
  const int *res;
  int i, rnum, snum, wsiz, nnum, lid;
  assert(db && word && min >= 0 && nump);
  if(*word != ' ' && (res = (int *)vlgetcache(db->auxdb, word, -1, &rnum)) != NULL &&
     (rnum /= sizeof(int)) / 2 >= min){
    CB_MALLOC(scores, (rnum / 2) * sizeof(ESTSCORE) + 1);
    snum = 0;
    for(i = 0; i < rnum; i += 2){
      scores[snum].id = res[i];
      scores[snum].score = res[i+1];
      snum++;
    }
    *nump = snum;
    return scores;
  }
  CB_LISTOPEN(words);
  if(*word == ' '){
    word++;
    if(*word == 'b'){
      est_expand_keyword_bw(db, word + 1, words);
    } else if(*word == 'e'){
      est_expand_keyword_ew(db, word + 1, words);
    } else if(*word == 'r'){
      est_expand_keyword_rx(db, word + 1, words);
    }
  } else if(*(unsigned char *)word >= 0xe3){
    est_expand_keyword_bw(db, word, words);
  }
  CB_DATUMOPEN(rbuf);
  for(i = 0; i < CB_LISTNUM(words) &&
        CB_DATUMSIZE(rbuf) <= sizeof(int) * 2 * min * ESTAUXEXRAT; i++){
    word = CB_LISTVAL2(words, i, wsiz);
    if(!(res = (int *)vlgetcache(db->auxdb, word, wsiz, &rnum))) continue;
    CB_DATUMCAT(rbuf, (char *)res, rnum);
  }
  res = (int *)CB_DATUMPTR(rbuf);
  rnum = CB_DATUMSIZE(rbuf);
  if((rnum /= sizeof(int)) / 2 < min){
    CB_DATUMCLOSE(rbuf);
    CB_LISTCLOSE(words);
    return NULL;
  }
  CB_MALLOC(scores, (rnum / 2) * sizeof(ESTSCORE) + 1);
  snum = 0;
  for(i = 0; i < rnum; i += 2){
    scores[snum].id = res[i];
    scores[snum].score = res[i+1];
    snum++;
  }
  qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_id_asc);
  nnum = 0;
  lid = -1;
  for(i = 0; i < snum; i++){
    if(nnum > 0 && scores[i].id == lid){
      scores[nnum-1].score += scores[i].score;
      continue;
    }
    scores[nnum].id = scores[i].id;
    scores[nnum].score = scores[i].score;
    nnum++;
    lid = scores[i].id;
  }
  CB_DATUMCLOSE(rbuf);
  CB_LISTCLOSE(words);
  *nump = nnum;
  return scores;
}


/* Weight scores with the auxiliary index.
   `db' specifies a database object.
   `word' specifies a search word.
   `scores' specifies an array of scores of search candidates.
   `snum' specifies the number of the array. */
static void est_weight_keywords(ESTDB *db, const char *word, ESTSCORE *scores, int snum){
  ESTSCORE *kscores;
  const int *res;
  int i, knum, nnum;
  double rank;
  if(!(res = (int *)vlgetcache(db->auxdb, word, -1, &knum)) || knum < 2) return;
  knum /= sizeof(int);
  CB_MALLOC(kscores, knum / 2 * sizeof(ESTSCORE));
  rank = knum / 2 + 1;
  nnum = 0;
  for(i = 0; i < knum; i += 2){
    kscores[nnum].id = res[i];
    kscores[nnum].score = (pow(rank, 0.7) / 8.0 + 1.0) * 10000.0;
    nnum++;
    rank -= 1.0;
  }
  knum = nnum;
  qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_id_asc);
  qsort(kscores, knum, sizeof(ESTSCORE), est_score_compare_by_id_asc);
  nnum = 0;
  for(i = 0; i < snum; i++){
    while(nnum < knum && kscores[nnum].id < scores[i].id){
      nnum++;
    }
    if(nnum < knum && kscores[nnum].id == scores[i].id)
      scores[i].score *= kscores[nnum].score / 10000.0;
  }
  free(kscores);
}


/* Get scores correspinding a ranking search with an attribute narrowing index.
   `db' specifies a database object.
   `name' specifies the name of an attribute.
   `nump' specifies the pointer to which the number of elements in the result is assigned.
   The return value is an array of score structures of corresponding documents. */
static ESTSCORE *est_search_rank(ESTDB *db, const char *name, int top, int *nump){
  ESTATTRIDX *attridx;
  ESTSCORE *scores;
  const char *kbuf;
  int snum, ksiz, id;
  assert(db && name && nump);
  if(top == 0 || !(attridx = (ESTATTRIDX *)cbmapget(db->aidxs, name, -1, NULL)) ||
     (attridx->type != ESTIDXATTRSTR && attridx->type != ESTIDXATTRNUM)){
    *nump = 0;
    return cbmalloc(1);
  }
  snum = abs(top);
  if(snum > db->dnum) snum = db->dnum;
  CB_MALLOC(scores, snum * sizeof(ESTSCORE) + 1);
  snum = 0;
  if(top > 0){
    vlcurfirst(attridx->db);
    while(snum < top && (kbuf = vlcurkeycache(attridx->db, &ksiz)) != NULL){
      if(ksiz < sizeof(int)){
        vlcurnext(attridx->db);
        continue;
      }
      memcpy(&id, kbuf + ksiz - sizeof(int), sizeof(int));
      if(id < 1){
        vlcurnext(attridx->db);
        continue;
      }
      scores[snum].id = id;
      scores[snum].score = 0;
      scores[snum].value = NULL;
      snum++;
      vlcurnext(attridx->db);
    }
  } else {
    top *= -1;
    vlcurlast(attridx->db);
    while(snum < top && (kbuf = vlcurkeycache(attridx->db, &ksiz)) != NULL){
      if(ksiz < sizeof(int)){
        vlcurprev(attridx->db);
        continue;
      }
      memcpy(&id, kbuf + ksiz - sizeof(int), sizeof(int));
      if(id < 1){
        vlcurprev(attridx->db);
        continue;
      }
      scores[snum].id = id;
      scores[snum].score = 0;
      scores[snum].value = NULL;
      snum++;
      vlcurprev(attridx->db);
    }
  }
  *nump = snum;
  return scores;
}


/* Get scores correspinding an attribute expression with an attribute narrowing index.
   `db' specifies a database object.
   `expr' specifies an attribute search expression.
   `nump' specifies the pointer to which the number of elements in the result is assigned.
   The return value is an array of score structures of corresponding documents or `NULL' if no
   index is available. */
static ESTSCORE *est_search_aidx_attr(ESTDB *db, const char *expr, int *nump){
  ESTATTRIDX *attridx;
  ESTSCORE *scores;
  CBDATUM *abuf;
  CBLIST *tokens;
  void *regex;
  const char *cop, *pv, *kbuf, *tbuf;
  unsigned char *utmp;
  char *name, *oper, *val, *sval, *wp, numbuf[ESTNUMBUFSIZ];
  int i, nsiz, vsiz, ksiz, tsiz, sign, ic, ssiz, esc, jmp, len, *ary, anum;
  time_t num, lower, upper;
  assert(db && expr && nump);
  name = NULL;
  oper = NULL;
  val = NULL;
  nsiz = 0;
  vsiz = 0;
  while(*expr > 0 && *expr <= ' '){
    expr++;
  }
  if((pv = strchr(expr, ' ')) != NULL){
    nsiz = pv - expr;
    name = cbmemdup(expr, nsiz);
    expr = pv;
    while(*expr > 0 && *expr <= ' '){
      expr++;
    }
    if((pv = strchr(expr, ' ')) != NULL){
      oper = cbmemdup(expr, pv - expr);
      expr = pv;
      while(*expr > 0 && *expr <= ' '){
        expr++;
      }
      vsiz = strlen(expr);
      val = cbmemdup(expr, vsiz);
    } else {
      oper = cbmemdup(expr, -1);
    }
  } else {
    nsiz = strlen(expr);
    name = cbmemdup(expr, nsiz);
  }
  if(!oper){
    oper = cbmemdup("", 0);
  }
  if(!val){
    vsiz = 0;
    val = cbmemdup("", 0);
  }
  cop = oper;
  if(*cop == '!'){
    sign = FALSE;
    cop++;
  } else {
    sign = TRUE;
  }
  if(*cop == 'I' || *cop == 'i'){
    ic = !est_check_cjk_only(val);
    cop++;
  } else {
    ic = FALSE;
  }
  regex = NULL;
  if(!cbstricmp(cop, ESTOPSTREQ)){
    cop = ESTOPSTREQ;
  } else if(!cbstricmp(cop, ESTOPSTRNE)){
    cop = ESTOPSTRNE;
  } else if(!cbstricmp(cop, ESTOPSTRINC)){
    cop = ESTOPSTRINC;
  } else if(!cbstricmp(cop, ESTOPSTRBW)){
    cop = ESTOPSTRBW;
  } else if(!cbstricmp(cop, ESTOPSTREW)){
    cop = ESTOPSTREW;
  } else if(!cbstricmp(cop, ESTOPSTRAND)){
    cop = ESTOPSTRAND;
  } else if(!cbstricmp(cop, ESTOPSTROR)){
    cop = ESTOPSTROR;
  } else if(!cbstricmp(cop, ESTOPSTROREQ)){
    cop = ESTOPSTROREQ;
  } else if(!cbstricmp(cop, ESTOPSTRRX)){
    cop = ESTOPSTRRX;
    regex = est_regex_new(val);
  } else if(!cbstricmp(cop, ESTOPNUMEQ)){
    cop = ESTOPNUMEQ;
  } else if(!cbstricmp(cop, ESTOPNUMNE)){
    cop = ESTOPNUMNE;
  } else if(!cbstricmp(cop, ESTOPNUMGT)){
    cop = ESTOPNUMGT;
  } else if(!cbstricmp(cop, ESTOPNUMGE)){
    cop = ESTOPNUMGE;
  } else if(!cbstricmp(cop, ESTOPNUMLT)){
    cop = ESTOPNUMLT;
  } else if(!cbstricmp(cop, ESTOPNUMLE)){
    cop = ESTOPNUMLE;
  } else if(!cbstricmp(cop, ESTOPNUMBT)){
    cop = ESTOPNUMBT;
  } else {
    cop = ESTOPSTRINC;
    val[0] = '\0';
    vsiz = 0;
  }
  num = cbstrmktime(val);
  if(!(attridx = (ESTATTRIDX *)cbmapget(db->aidxs, name, nsiz, NULL)) ||
     (attridx->type != ESTIDXATTRSTR && attridx->type != ESTIDXATTRNUM) ||
     (attridx->type == ESTIDXATTRNUM &&
      cop != ESTOPNUMEQ && cop != ESTOPNUMNE && cop != ESTOPNUMGT && cop != ESTOPNUMGE &&
      cop != ESTOPNUMLT && cop != ESTOPNUMLE && cop != ESTOPNUMBT)){
    if(regex) est_regex_delete(regex);
    free(val);
    free(oper);
    free(name);
    return NULL;
  }
  CB_DATUMOPEN(abuf);
  if(!sign || ic){
    if(ic){
      utmp = (unsigned char *)est_uconv_in(val, vsiz, &tsiz);
      est_normalize_text(utmp, tsiz, &tsiz);
      est_canonicalize_text(utmp, tsiz, FALSE);
      sval = (char *)est_uconv_out((char *)utmp, tsiz, &ssiz);
      free(utmp);
    } else {
      sval = NULL;
      ssiz = 0;
    }
    esc = INT_MAX;
    jmp = INT_MAX;
    if(sign && (cop == ESTOPSTREQ || cop == ESTOPSTRBW) && vsiz > 0){
      if(*sval > 0x0 && *sval < 0x7f){
        numbuf[0] = *sval;
        numbuf[1] = '\0';
        esc = *(unsigned char *)sval;
        if(*sval >= 'a' && *sval <= 'z'){
          numbuf[0] -= 'a' - 'A';
          jmp = *sval - 'a' + 'A';
        }
        vlcurjump(attridx->db, numbuf, 1, VL_JFORWARD);
      } else if(*(unsigned char *)sval >= 0xc0){
        numbuf[0] = *sval;
        numbuf[1] = '\0';
        esc = *(unsigned char *)sval;
        vlcurjump(attridx->db, numbuf, 1, VL_JFORWARD);
      } else {
        vlcurfirst(attridx->db);
      }
    } else {
      vlcurfirst(attridx->db);
    }
    while((kbuf = vlcurkeycache(attridx->db, &ksiz)) != NULL){
      if(est_match_attr(kbuf, ksiz - sizeof(int) - 1,
                        cop, sign, val, vsiz, sval, ssiz, regex, num))
        CB_DATUMCAT(abuf, kbuf + ksiz - sizeof(int), sizeof(int));
      if(*(unsigned char *)kbuf > jmp && *(unsigned char *)kbuf < *(unsigned char *)sval){
        numbuf[0] = *sval;
        numbuf[1] = '\0';
        vlcurjump(attridx->db, numbuf, 1, VL_JFORWARD);
        jmp = INT_MAX;
      } else if(*(unsigned char *)kbuf > esc){
        break;
      } else {
        vlcurnext(attridx->db);
      }
    }
    if(sval) free(sval);
  } else if(cop == ESTOPSTROREQ){
    tokens = cbsplit(val, vsiz, " ,");
    cblistsort(tokens);
    for(i = 0; i < CB_LISTNUM(tokens); i++){
      tbuf = CB_LISTVAL2(tokens, i, tsiz);
      vlcurjump(attridx->db, tbuf, tsiz, VL_JFORWARD);
      while((kbuf = vlcurkeycache(attridx->db, &ksiz)) != NULL && !strcmp(kbuf, tbuf)){
        CB_DATUMCAT(abuf, kbuf + ksiz - sizeof(int), sizeof(int));
        vlcurnext(attridx->db);
      }
    }
    CB_LISTCLOSE(tokens);
  } else if(cop == ESTOPNUMBT){
    if((wp = strchr(val, ' ')) != NULL || (wp = strchr(val, '\t')) != NULL){
      *(wp++) = '\0';
      while(*wp == ' ' || *wp == '\t'){
        wp++;
      }
      lower = cbstrmktime(val);
      upper = cbstrmktime(wp);
    } else {
      lower = cbstrmktime(val);
      upper = INT_MAX;
    }
    len = sprintf(numbuf, "%.0f", (double)lower);
    vlcurjump(attridx->db, numbuf, len, VL_JFORWARD);
    while((kbuf = vlcurkeycache(attridx->db, &ksiz)) != NULL && cbstrmktime(kbuf) <= upper){
      CB_DATUMCAT(abuf, kbuf + ksiz - sizeof(int), sizeof(int));
      vlcurnext(attridx->db);
    }
  } else {
    if(cop == ESTOPSTREQ || cop == ESTOPSTRBW ||
       cop == ESTOPNUMEQ || cop == ESTOPNUMGT || cop == ESTOPNUMGE){
      vlcurjump(attridx->db, val, vsiz, VL_JFORWARD);
      if(cop == ESTOPNUMGT){
        while((kbuf = vlcurkeycache(attridx->db, NULL)) != NULL && cbstrmktime(kbuf) <= num){
          vlcurnext(attridx->db);
        }
      }
    } else if(cop == ESTOPNUMLT || cop == ESTOPNUMLE){
      len = sprintf(numbuf, "%.0f", (double)cbstrmktime(val) + 1);
      vlcurjump(attridx->db, numbuf, len, VL_JBACKWARD);
      if(cop == ESTOPNUMLT){
        while((kbuf = vlcurkeycache(attridx->db, NULL)) != NULL && cbstrmktime(kbuf) >= num){
          vlcurprev(attridx->db);
        }
      }
    } else {
      vlcurfirst(attridx->db);
    }
    while((kbuf = vlcurkeycache(attridx->db, &ksiz)) != NULL){
      if(est_match_attr(kbuf, ksiz - sizeof(int) - 1,
                        cop, TRUE, val, vsiz, NULL, 0, regex, num)){
        CB_DATUMCAT(abuf, kbuf + ksiz - sizeof(int), sizeof(int));
      } else if(cop == ESTOPSTREQ || cop == ESTOPSTRBW || cop == ESTOPNUMEQ){
        break;
      }
      if(cop == ESTOPNUMLT || cop == ESTOPNUMLE){
        vlcurprev(attridx->db);
      } else {
        vlcurnext(attridx->db);
      }
    }
  }
  ary = (int *)CB_DATUMPTR(abuf);
  anum = CB_DATUMSIZE(abuf) / sizeof(int);
  CB_MALLOC(scores, anum * sizeof(ESTSCORE) + 1);
  for(i = 0; i < anum; i++){
    scores[i].id = ary[i];
    scores[i].score = 0;
    scores[i].value = NULL;
  }
  *nump = anum;
  CB_DATUMCLOSE(abuf);
  if(regex) est_regex_delete(regex);
  free(val);
  free(oper);
  free(name);
  return scores;
}


/* Get a correspinding set of documents in pseudo indexes.
   `db' specifies a database object.
   `cond' specifies a search condition object.
   `scores' specifies an array of scores of search candidates.
   `nump' specifies the pointer to which the number of elements in the parameter and result is
   assigned.
   `ordattrs' specifies a map object into which ordering attributes are stored.
   The return value is an array of re-allocated score structures. */
static ESTSCORE *est_search_pidxs(ESTDB *db, ESTCOND *cond, ESTSCORE *scores, int *nump,
                                  CBMAP *ordattrs){
  ESTCATTR *list;
  ESTDOC *doc;
  const char *otype, *lbuf, *vbuf;
  char *oname, *wp;
  int i, j, k, snum, anum, id, hit, sc, miss, lsiz, vsiz;
  double avg, sd, dif, tune;
  assert(db && cond && scores && nump && ordattrs);
  snum = *nump;
  CB_REALLOC(scores, (snum + CB_LISTNUM(db->pdocs)) * sizeof(ESTSCORE) + 1);
  if(cond->phrase){
    if(cbstrfwmatch(cond->phrase, ESTOPID)){
      return scores;
    } else if(cbstrfwmatch(cond->phrase, ESTOPURI)){
      return scores;
    } else if(cbstrfwmatch(cond->phrase, ESTOPSIMILAR)){
      return scores;
    }
  }
  oname = NULL;
  otype = NULL;
  if(cond->order){
    oname = cbmemdup(cond->order, -1);
    cbstrtrim(oname);
    otype = ESTORDSTRA;
    if((wp = strchr(oname, ' ')) != NULL){
      *(wp++) = '\0';
      while(*wp == ' '){
        wp++;
      }
      otype = wp;
    }
  }
  list = NULL;
  anum = -1;
  if(cond->attrs) list = est_make_cattr_list(cond->attrs, &anum);
  for(i = 0; i < CB_LISTNUM(db->pdocs); i++){
    id = ESTPDOCIDMIN + i;
    hit = FALSE;
    sc = 0;
    doc = NULL;
    if(!cond->phrase || cond->phrase[0] == '\0'){
      hit = cond->attrs ? TRUE : FALSE;
    } else if(cbstrfwmatch(cond->phrase, ESTOPUVSET)){
      hit = TRUE;
    } else {
      if((doc = est_db_get_doc(db, id, 0)) != NULL){
        hit = est_db_score_doc(db, doc, cond, &sc);
      } else {
        hit = FALSE;
      }
    }
    if(hit && list){
      if(!doc && !(doc = est_db_get_doc(db, id, 0))){
        hit = FALSE;
      } else {
        miss = FALSE;
        for(j = 0; !miss && j < anum; j++){
          if(list[j].nsiz < 1) continue;
          if(list[j].nlist){
            hit = FALSE;
            for(k = 0; k < CB_LISTNUM(list[j].nlist); k++){
              lbuf = CB_LISTVAL2(list[j].nlist, k, lsiz);
              if(lsiz < 1) continue;
              if(!(vbuf = cbmapget(doc->attrs, lbuf, lsiz, &vsiz))) continue;
              if(est_match_attr(vbuf, vsiz, list[j].cop, list[j].sign, list[j].val, list[j].vsiz,
                                list[j].sval, list[j].ssiz, list[j].regex, list[j].num)){
                hit = TRUE;
                break;
              }
            }
            if(!hit) miss = TRUE;
          } else if(!(vbuf = cbmapget(doc->attrs, list[j].name, list[j].nsiz, &vsiz))){
            miss = TRUE;
          } else if(!est_match_attr(vbuf, vsiz, list[j].cop, list[j].sign,
                                    list[j].val, list[j].vsiz, list[j].sval, list[j].ssiz,
                                    list[j].regex, list[j].num)){
            miss = TRUE;
          }
        }
        hit = !miss;
      }
    }
    if(hit){
      scores[snum].id = id;
      scores[snum].score = sc;
      scores[snum].value = NULL;
      snum++;
      if(oname && (doc || (doc = est_db_get_doc(db, id, 0)) != NULL)){
        if(!(vbuf = cbmapget(doc->attrs, oname, -1, &vsiz))){
          vbuf = "";
          vsiz = 0;
        }
        cbmapput(ordattrs, (char *)&id, sizeof(int), vbuf, vsiz, FALSE);
      }
    }
    if(doc) est_doc_delete(doc);
  }
  if(list) est_free_cattr_list(list, anum);
  if(oname) free(oname);
  if(db->smode != ESTDFSCASIS && snum > *nump){
    avg = 0.0;
    for(i = *nump; i < snum; i++){
      avg += scores[i].score;
    }
    avg /= snum - *nump;
    sd = 0.0;
    for(i = *nump; i < snum; i++){
      dif = avg - scores[i].score;
      sd += dif * dif;
    }
    sd /= snum - *nump;
    sd = sqrt(sd);
    if(sd < 0.1){
      for(i = *nump; i < snum; i++){
        scores[i].score = ESTSCOREUNIT / 2;
      }
    } else {
      for(i = *nump; i < snum; i++){
        scores[i].score = (int)(((scores[i].score - avg) * (ESTSCOREUNIT / 10.0) / sd) +
                                ESTSCOREUNIT / 2.0);
      }
    }
    if(cond->tfidf){
      tune = pow(snum - *nump + 64, 0.4);
      for(i = *nump; i < snum; i++){
        scores[i].score *= 100.0 / tune;
      }
    } else {
      for(i = *nump; i < snum; i++){
        scores[i].score *= 10;
      }
    }
  }
  *nump = snum;
  return scores;
}


/* Narrow and sort scores of search candidates.
   `db' specifies a database object.
   `attrs' specifies a list object of narrowing attributes.
   `ign' specifies the offset of an attribute to be ignored.
   `order' specifies an expression for sorting.
   `distinct' specifies the name of the distinct attribute.
   `scores' specifies an array of scores of search candidates.
   `snum' specifies the number of the array.
   `limit' specifies the limit number to check.
   `restp' specifies the pointer to a variable to which rest number to be checked is assigned.
   `ordattrs' specifies a map object of cached ordering attributes.
   The return value is the new number of the array. */
static int est_narrow_scores(ESTDB *db, const CBLIST *attrs, int ign,
                             const char *order, const char *distinct, ESTSCORE *scores, int snum,
                             int limit, int *restp, CBMAP *ordattrs){
  ESTCATTR *list;
  ESTATTRIDX *attridx;
  CBMAP *umap;
  const char *otype, *cbuf, *ibuf, *lbuf;
  char *oname, *wp, *mbuf, *vbuf;
  int i, j, k, ci, oi, anum, done, mixed, nnum, csiz, msiz;
  int miss, vsiz, num, isiz, lsiz, hit, onlen, dnlen;
  time_t tval;
  assert(db && scores && snum >= 0 && limit >= 0 && restp && ordattrs);
  *restp = 0;
  ci = -1;
  oi = -1;
  oname = NULL;
  otype = NULL;
  if(order){
    oname = cbmemdup(order, -1);
    cbstrtrim(oname);
    otype = ESTORDSTRA;
    if((wp = strchr(oname, ' ')) != NULL){
      *(wp++) = '\0';
      while(*wp == ' '){
        wp++;
      }
      otype = wp;
    }
  }
  if(attrs){
    list = est_make_cattr_list(attrs, &anum);
    if(cbmaprnum(db->aidxs) > 0){
      done = TRUE;
      mixed = FALSE;
      for(i = 0; i < anum; i++){
        if(i == ign) continue;
        if(!(attridx = (ESTATTRIDX *)cbmapget(db->aidxs, list[i].name, list[i].nsiz, NULL)) ||
           (attridx->type == ESTIDXATTRNUM &&
            list[i].cop != ESTOPNUMEQ && list[i].cop != ESTOPNUMNE &&
            list[i].cop != ESTOPNUMGT && list[i].cop != ESTOPNUMGE &&
            list[i].cop != ESTOPNUMLT && list[i].cop != ESTOPNUMLE &&
            list[i].cop != ESTOPNUMBT) ||
           (attridx->type != ESTIDXATTRSEQ && snum < ESTAISNUMMIN)){
          done = FALSE;
          continue;
        }
        switch(attridx->type){
        case ESTIDXATTRSTR:
        case ESTIDXATTRNUM:
          snum = est_aidx_attr_narrow(attridx->db, db->pdocs, list[i].cop, list[i].sign,
                                      list[i].val, list[i].vsiz, list[i].sval, list[i].ssiz,
                                      list[i].regex, list[i].num, scores, snum);
          mixed = TRUE;
          break;
        default:
          if(done && i == anum - 1 && !order && mixed){
            qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_score_desc);
            mixed = FALSE;
          }
          snum = est_aidx_seq_narrow(attridx->db, db->pdocs, list[i].cop, list[i].sign,
                                     list[i].val, list[i].vsiz, list[i].sval, list[i].ssiz,
                                     list[i].regex, list[i].num, scores, snum,
                                     done && i == anum - 1 ? limit : INT_MAX, restp);
          break;
        }
        list[i].cop = ESTOPDUMMY;
      }
      if(mixed && !order) qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_score_desc);
    } else {
      done = FALSE;
    }
    if(db->spacc){
      for(i = 0; i < anum; i++){
        if(!strcmp(list[i].name, db->scname)){
          ci = i;
          break;
        }
      }
    }
    if(oname){
      for(i = 0; i < anum; i++){
        if(!strcmp(list[i].name, oname)){
          oi = i;
          break;
        }
      }
    }
    if(!done){
      nnum = 0;
      for(i = 0; i < snum; i++){
        if(nnum >= limit){
          *restp = snum - i;
          break;
        }
        scores[i].value = NULL;
        if(ci >= 0){
          if((cbuf = cbmapget(db->spacc, (char *)&(scores[i].id), sizeof(int), &csiz)) != NULL)
            cbmapmove(db->spacc, (char *)&(scores[i].id), sizeof(int), FALSE);
        } else {
          cbuf = NULL;
          csiz = 0;
        }
        mbuf = NULL;
        if(scores[i].id >= ESTPDOCIDMIN){
          scores[nnum++] = scores[i];
        } else if((cbuf && anum == 1) ||
                  (mbuf = est_crget(db->attrdb, db->zmode, scores[i].id, &msiz)) != NULL){
          miss = FALSE;
          for(j = 0; !miss && j < anum; j++){
            if(list[j].nsiz < 1) continue;
            if(list[j].nlist){
              hit = FALSE;
              for(k = 0; k < CB_LISTNUM(list[j].nlist); k++){
                lbuf = CB_LISTVAL2(list[j].nlist, k, lsiz);
                if(lsiz < 1) continue;
                if(!(vbuf = cbmaploadone(mbuf, msiz, lbuf, lsiz, &vsiz))) continue;
                if(est_match_attr(vbuf, vsiz, list[j].cop, list[j].sign, list[j].val, list[j].vsiz,
                                  list[j].sval, list[j].ssiz, list[j].regex, list[j].num)){
                  hit = TRUE;
                  free(vbuf);
                  break;
                }
                free(vbuf);
              }
              if(!hit) miss = TRUE;
              vbuf = NULL;
            } else {
              if(mbuf){
                vbuf = cbmaploadone(mbuf, msiz, list[j].name, list[j].nsiz, &vsiz);
              } else if(csiz != 1 || cbuf[0] != '\0'){
                vbuf = cbmemdup(cbuf, csiz);
                vsiz = csiz;
              } else {
                vbuf = NULL;
              }
              if(list[j].oper[0] == '\0'){
                if(!vbuf) miss = TRUE;
              } else {
                if(!vbuf){
                  vbuf = cbmemdup("", 0);
                  vsiz = 0;
                }
                if(!est_match_attr(vbuf, vsiz, list[j].cop, list[j].sign,
                                   list[j].val, list[j].vsiz, list[j].sval, list[j].ssiz,
                                   list[j].regex, list[j].num)) miss = TRUE;
              }
            }
            if(j == ci && !cbuf){
              if(vbuf){
                cbmapput(db->spacc, (char *)&(scores[i].id), sizeof(int), vbuf, vsiz, FALSE);
              } else {
                cbmapput(db->spacc, (char *)&(scores[i].id), sizeof(int), "", 1, FALSE);
              }
              if(cbmaprnum(db->spacc) > db->scmnum){
                num = db->scmnum * 0.1 + 1;
                cbmapiterinit(db->spacc);
                for(k = 0; k < num && (ibuf = cbmapiternext(db->spacc, &isiz)) != NULL; k++){
                  cbmapout(db->spacc, ibuf, isiz);
                }
              }
            }
            if(j == oi){
              scores[i].value = vbuf;
            } else {
              free(vbuf);
            }
          }
          if(miss){
            free(scores[i].value);
          } else {
            scores[nnum++] = scores[i];
          }
        }
        free(mbuf);
      }
      snum = nnum;
    } else {
      for(i = 0; i < snum; i++){
        scores[i].value = NULL;
      }
    }
    est_free_cattr_list(list, anum);
  } else {
    for(i = 0; i < snum; i++){
      scores[i].value = NULL;
    }
  }
  if(oname){
    if(!cbstricmp(oname, ESTORDIDA)){
      qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_id_asc);
    } else if(!cbstricmp(oname, ESTORDIDD)){
      qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_id_desc);
    } else if(!cbstricmp(oname, ESTORDSCA)){
      qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_score_asc);
    } else if(!cbstricmp(oname, ESTORDSCD)){
      qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_score_desc);
    } else {
      ci = db->spacc && !strcmp(oname, db->scname);
      onlen = strlen(oname);
      attridx = (ESTATTRIDX *)cbmapget(db->aidxs, oname, onlen, NULL);
      if(attridx && attridx->type != ESTIDXATTRSEQ) attridx = NULL;
      for(i = 0; i < snum; i++){
        if(scores[i].value) continue;
        if(ci &&
           (cbuf = cbmapget(db->spacc, (char *)&(scores[i].id), sizeof(int), &csiz)) != NULL){
          cbmapmove(db->spacc, (char *)&(scores[i].id), sizeof(int), FALSE);
          if(csiz == 1 && cbuf[0] == '\0'){
            scores[i].value = cbmemdup("", 0);
          } else {
            scores[i].value = cbmemdup(cbuf, csiz);
          }
          continue;
        }
        if((cbuf = cbmapget(ordattrs, (char *)&(scores[i].id), sizeof(int), &csiz)) != NULL){
          scores[i].value = cbmemdup(cbuf, csiz);
          continue;
        }
        if(attridx){
          if(!(vbuf = est_aidx_seq_get(attridx->db, scores[i].id, &vsiz))) vbuf = cbmemdup("", 0);
          scores[i].value = vbuf;
          continue;
        }
        if((mbuf = est_crget(db->attrdb, db->zmode, scores[i].id, &msiz)) != NULL){
          if((vbuf = cbmaploadone(mbuf, msiz, oname, onlen, &vsiz)) != NULL){
            if(ci) cbmapput(db->spacc, (char *)&(scores[i].id), sizeof(int), vbuf, vsiz, FALSE);
            scores[i].value = vbuf;
          } else {
            if(ci) cbmapput(db->spacc, (char *)&(scores[i].id), sizeof(int), "", 1, FALSE);
            scores[i].value = cbmemdup("", 0);
          }
          if(ci && cbmaprnum(db->spacc) > db->scmnum){
            num = db->scmnum * 0.1 + 1;
            cbmapiterinit(db->spacc);
            for(j = 0; j < num && (ibuf = cbmapiternext(db->spacc, &isiz)) != NULL; j++){
              cbmapout(db->spacc, ibuf, isiz);
            }
          }
          free(mbuf);
        } else {
          scores[i].value = cbmemdup("", 0);
        }
      }
      if(!cbstricmp(otype, ESTORDSTRA)){
        qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_str_asc);
      } else if(!cbstricmp(otype, ESTORDSTRD)){
        qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_str_desc);
      } else if(!cbstricmp(otype, ESTORDNUMA)){
        for(i = 0; i < snum; i++){
          tval = cbstrmktime(scores[i].value);
          free(scores[i].value);
          scores[i].value = (void *)tval;
        }
        qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_num_asc);
        for(i = 0; i < snum; i++){
          scores[i].value = NULL;
        }
      } else if(!cbstricmp(otype, ESTORDNUMD)){
        for(i = 0; i < snum; i++){
          tval = cbstrmktime(scores[i].value);
          free(scores[i].value);
          scores[i].value = (void *)tval;
        }
        qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_num_desc);
        for(i = 0; i < snum; i++){
          scores[i].value = NULL;
        }
      }
      for(i = 0; i < snum; i++){
        free(scores[i].value);
      }
    }
    free(oname);
  }
  if(distinct){
    if(!order && *distinct != '~')
      qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_score_desc);
    if(*distinct == '~') distinct++;
    dnlen = strlen(distinct);
    umap = cbmapopenex(snum + 1);
    attridx = (ESTATTRIDX *)cbmapget(db->aidxs, distinct, dnlen, NULL);
    if(attridx && attridx->type != ESTIDXATTRSEQ) attridx = NULL;
    nnum = 0;
    for(i = 0; i < snum; i++){
      if(scores[i].id >= ESTPDOCIDMIN){
        if(!(vbuf = est_db_get_doc_attr(db, scores[i].id, distinct))) vbuf = cbmemdup("", 0);
        vsiz = strlen(vbuf);
      } else if(attridx){
        if(!(vbuf = est_aidx_seq_get(attridx->db, scores[i].id, &vsiz))){
          vbuf = cbmemdup("", 0);
          vsiz = 0;
        }
      } else {
        if((mbuf = est_crget(db->attrdb, db->zmode, scores[i].id, &msiz)) != NULL){
          if(!(vbuf = cbmaploadone(mbuf, msiz, distinct, dnlen, &vsiz))){
            vbuf = cbmemdup("", 0);
            vsiz = 0;
          }
          free(mbuf);
        } else {
          vbuf = cbmemdup("", 0);
          vsiz = 0;
        }
      }
      if(cbmapput(umap, vbuf, vsiz, "", 0, FALSE)) scores[nnum++] = scores[i];
      free(vbuf);
    }
    snum = nnum;
    cbmapclose(umap);
  }
  return snum;
}


/* Make a list of condition attributes.
   `attrs' specifies a list object of attribute expressions.
   `nump' specifies the pointer to which the number of elements in the result is assigned.
   The return value is a list of condition attributes. */
static ESTCATTR *est_make_cattr_list(const CBLIST *attrs, int *nump){
  ESTCATTR *list;
  const char *rp, *pv;
  unsigned char *utmp;
  int i, anum, tsiz;
  assert(attrs && nump);
  anum = CB_LISTNUM(attrs);
  CB_MALLOC(list, sizeof(ESTCATTR) * anum + 1);
  for(i = 0; i < anum; i++){
    list[i].name = NULL;
    list[i].oper = NULL;
    list[i].val = NULL;
    rp = CB_LISTVAL(attrs, i);
    while(*rp > 0 && *rp <= ' '){
      rp++;
    }
    if((pv = strchr(rp, ' ')) != NULL){
      list[i].nsiz = pv - rp;
      list[i].name = cbmemdup(rp, list[i].nsiz);
      rp = pv;
      while(*rp > 0 && *rp <= ' '){
        rp++;
      }
      if((pv = strchr(rp, ' ')) != NULL){
        list[i].oper = cbmemdup(rp, pv - rp);
        rp = pv;
        while(*rp > 0 && *rp <= ' '){
          rp++;
        }
        list[i].vsiz = strlen(rp);
        list[i].val = cbmemdup(rp, list[i].vsiz);
      } else {
        list[i].oper = cbmemdup(rp, -1);
      }
    } else {
      list[i].nsiz = strlen(rp);
      list[i].name = cbmemdup(rp, list[i].nsiz);
    }
    if(strchr(list[i].name, ',')){
      list[i].nlist = cbsplit(list[i].name, list[i].nsiz, ",");
    } else {
      list[i].nlist = NULL;
    }
    if(!list[i].oper){
      list[i].oper = cbmemdup("", 0);
    }
    if(!list[i].val){
      list[i].vsiz = 0;
      list[i].val = cbmemdup("", 0);
    }
  }
  for(i = 0; i < anum; i++){
    rp = list[i].oper;
    if(*rp == '!'){
      list[i].sign = FALSE;
      rp++;
    } else {
      list[i].sign = TRUE;
    }
    if(*rp == 'I' || *rp == 'i'){
      if(est_check_cjk_only(list[i].val)){
        list[i].sval = NULL;
        list[i].ssiz = 0;
      } else {
        utmp = (unsigned char *)est_uconv_in(list[i].val, list[i].vsiz, &tsiz);
        est_normalize_text(utmp, tsiz, &tsiz);
        est_canonicalize_text(utmp, tsiz, FALSE);
        list[i].sval = (char *)est_uconv_out((char *)utmp, tsiz, &(list[i].ssiz));
        free(utmp);
      }
      rp++;
    } else {
      list[i].sval = NULL;
      list[i].ssiz = 0;
    }
    list[i].regex = NULL;
    list[i].num = cbstrmktime(list[i].val);
    if(!cbstricmp(rp, ESTOPSTREQ)){
      list[i].cop = ESTOPSTREQ;
    } else if(!cbstricmp(rp, ESTOPSTRNE)){
      list[i].cop = ESTOPSTRNE;
    } else if(!cbstricmp(rp, ESTOPSTRINC)){
      list[i].cop = ESTOPSTRINC;
    } else if(!cbstricmp(rp, ESTOPSTRBW)){
      list[i].cop = ESTOPSTRBW;
    } else if(!cbstricmp(rp, ESTOPSTREW)){
      list[i].cop = ESTOPSTREW;
    } else if(!cbstricmp(rp, ESTOPSTRAND)){
      list[i].cop = ESTOPSTRAND;
    } else if(!cbstricmp(rp, ESTOPSTROR)){
      list[i].cop = ESTOPSTROR;
    } else if(!cbstricmp(rp, ESTOPSTROREQ)){
      list[i].cop = ESTOPSTROREQ;
    } else if(!cbstricmp(rp, ESTOPSTRRX)){
      list[i].cop = ESTOPSTRRX;
      list[i].regex = list[i].sval ? est_regex_new(list[i].sval) : est_regex_new(list[i].val);
    } else if(!cbstricmp(rp, ESTOPNUMEQ)){
      list[i].cop = ESTOPNUMEQ;
    } else if(!cbstricmp(rp, ESTOPNUMNE)){
      list[i].cop = ESTOPNUMNE;
    } else if(!cbstricmp(rp, ESTOPNUMGT)){
      list[i].cop = ESTOPNUMGT;
    } else if(!cbstricmp(rp, ESTOPNUMGE)){
      list[i].cop = ESTOPNUMGE;
    } else if(!cbstricmp(rp, ESTOPNUMLT)){
      list[i].cop = ESTOPNUMLT;
    } else if(!cbstricmp(rp, ESTOPNUMLE)){
      list[i].cop = ESTOPNUMLE;
    } else if(!cbstricmp(rp, ESTOPNUMBT)){
      list[i].cop = ESTOPNUMBT;
    } else {
      list[i].cop = ESTOPSTRINC;
      list[i].val[0] = '\0';
      list[i].vsiz = 0;
      if(list[i].sval){
        list[i].sval[0] = '\0';
        list[i].ssiz = 0;
      }
    }
  }
  *nump = anum;
  return list;
}


/* Release resources of a list of condition attributes.
   `list' specifies a list of condition attributes.
   `anum' specifies the number of elements of the list. */
static void est_free_cattr_list(ESTCATTR *list, int anum){
  int i;
  assert(list && anum >= 0);
  for(i = 0; i < anum; i++){
    if(list[i].regex) est_regex_delete(list[i].regex);
    free(list[i].sval);
    free(list[i].val);
    free(list[i].oper);
    if(list[i].nlist) CB_LISTCLOSE(list[i].nlist);
    free(list[i].name);
  }
  free(list);
}


/* Narrow and sort scores of search candidates.
   `db' specifies a database object.
   `scores' specifies an array of scores of search candidates.
   `snum' specifies the number of the array.
   `num' specifies the number of documents to be shown.
   `max' specifies the maximum number of shown documents.
   `vnum' specifies the number of dimensions of the vector.
   `tfidf' specifies whether to perform TF-IDF tuning.
   `limit' specifies the upper limit of similarity for documents to survive.
   `opts' specifies optoins for eclipse.
   `shadows' specifies a map object to store shadow document information.
   The return value is the new number of the array. */
static int est_eclipse_scores(ESTDB *db, ESTSCORE *scores, int snum, int num,
                              int vnum, int tfidf, double limit, CBMAP *shadows){
  CBMAP *svmap, *tvmap;
  const char *suri, *turi;
  char *tmp;
  int i, j, ubase, simurl, max, *svec, *tvec, pair[2], nnum;
  double dval;
  assert(db && scores && snum >= 0 && num >= 0 && vnum > 0 && limit > 0.0 && shadows);
  ubase = FALSE;
  simurl = FALSE;
  if(limit == ESTECLSERV || limit == ESTECLDIR || limit == ESTECLFILE){
    ubase = TRUE;
  } else if(limit >= ESTECLSIMURL){
    simurl = TRUE;
    limit -= ESTECLSIMURL;
    if(limit < 0.01) limit = 0.01;
    if(limit > 1.0) limit = 1.0;
  }
  nnum = 0;
  if(ubase){
    if(limit == ESTECLSERV){
      max = num * 14.8 + 8;
    } else if(limit == ESTECLDIR){
      max = num * 6.8 + 8;
    } else {
      max = num * 4.8 + 8;
    }
    if(max > snum) max = snum;
    for(i = 0; i < max; i++){
      scores[i].value = est_db_get_doc_attr(db, scores[i].id, ESTDATTRURI);
    }
    for(i = 0; i < max; i++){
      if(!scores[i].value) continue;
      for(j = i + 1; j < max; j++){
        dval = 0.0;
        if(scores[j].value){
          switch(est_url_sameness(scores[i].value, scores[j].value)){
          case 1:
            dval = ESTECLSERV;
            break;
          case 2:
            dval = ESTECLDIR;
            break;
          case 3:
            dval = ESTECLFILE;
            break;
          }
        }
        if(dval >= limit){
          free(scores[j].value);
          scores[j].value = NULL;
          pair[0] = scores[j].id;
          pair[1] = 0;
          cbmapputcat(shadows, (char *)&(scores[i].id), sizeof(int),
                      (char *)pair, sizeof(int) * 2);
        }
      }
    }
    for(i = 0; i < max; i++){
      if(scores[i].value){
        free(scores[i].value);
        scores[nnum++] = scores[i];
      }
    }
    for(i = max; i < snum; i++){
      scores[nnum++] = scores[i];
    }
  } else {
    max = limit < 0.1 ? snum : num * ((2.4 / (limit - 0.05)) + 0.8) + 8;
    if(simurl) max *= 1.4;
    if(max > snum) max = snum;
    CB_MALLOC(svec, vnum * sizeof(int));
    CB_MALLOC(tvec, vnum * sizeof(int));
    for(i = 0; i < max; i++){
      if((svmap = est_get_tvmap(db, scores[i].id, vnum, tfidf)) != NULL){
        scores[i].value = (char *)svmap;
        if(simurl && (tmp = est_db_get_doc_attr(db, scores[i].id, ESTDATTRURI)) != NULL){
          cbmapput(svmap, "", 0, tmp, -1, TRUE);
          free(tmp);
        }
      } else {
        scores[i].value = NULL;
      }
    }
    for(i = 0; i < max; i++){
      svmap = (CBMAP *)(scores[i].value);
      if(!svmap || cbmaprnum(svmap) < 1) continue;
      suri = cbmapget((CBMAP *)scores[i].value, "", -1, NULL);
      if(num-- < 1) continue;
      est_vector_set_seed(svmap, svec, vnum);
      for(j = i + 1; j < max; j++){
        tvmap = (CBMAP *)(scores[j].value);
        if(!tvmap || cbmaprnum(tvmap) < 1) continue;
        est_vector_set_target(svmap, tvmap, tvec, vnum);
        dval = est_vector_cosine(svec, tvec, vnum);
        if(dval > 0.01 && suri &&
           (turi = cbmapget((CBMAP *)scores[j].value, "", -1, NULL)) != NULL){
          switch(est_url_sameness(suri, turi)){
          default:
            dval = pow(cos(acos(dval) * (1.0 - pow(dval, 9.9))), 1.07);
            break;
          case 1:
            dval = pow(cos(acos(dval) * (1.0 - pow(dval, 4.1))), 1.05);
            break;
          case 2:
            dval = pow(cos(acos(dval) * (1.0 - pow(dval, 2.9))), 1.03);
            break;
          case 3:
            dval = pow(cos(acos(dval) * (1.0 - pow(dval, 2.1))), 1.01);
            break;
          }
        }
        if(dval > limit){
          cbmapclose(tvmap);
          scores[j].value = NULL;
          pair[0] = scores[j].id;
          pair[1] = (int)(dval * 10000.0);
          cbmapputcat(shadows, (char *)&(scores[i].id), sizeof(int),
                      (char *)pair, sizeof(int) * 2);
        }
      }
    }
    for(i = 0; i < max; i++){
      if(scores[i].value){
        cbmapclose((CBMAP *)(scores[i].value));
        scores[nnum++] = scores[i];
      }
    }
    for(i = max; i < snum; i++){
      scores[nnum++] = scores[i];
    }
    free(tvec);
    free(svec);
  }
  return nnum;
}


/* Check whether a score matches an attribute condition.
   `tval' specifies the target value;
   `tsiz' specifies the size of the target value
   `cop' specifies the pointer to the operator.
   `sign' specifies the sign of operation.
   `oval' specifies the operation value.
   `osiz' specifies the size of the operation value
   `sval' specifies the operation value of small cases.
   `ssiz' specifies the size of the operation value of small cases.
   `regex' specifies the regular expressions.
   `onum' specifies the numeric value.
   The return value is true if it does match, else it is false. */
static int est_match_attr(const char *tval, int tsiz, const char *cop, int sign,
                          const char *oval, int osiz, const char *sval, int ssiz,
                          const void *regex, int onum){
  unsigned char *eval;
  char *cval;
  int csiz, esiz, hit;
  assert(tval && tsiz >= 0 && oval && osiz >= 0);
  cval = NULL;
  if(sval){
    eval = (unsigned char *)est_uconv_in(tval, tsiz, &esiz);
    est_normalize_text(eval, esiz, &esiz);
    est_canonicalize_text(eval, esiz, FALSE);
    cval = (char *)est_uconv_out((char *)eval, esiz, &csiz);
    free(eval);
    tval = cval;
    tsiz = csiz;
    oval = sval;
    osiz = ssiz;
  }
  if(cop == ESTOPSTREQ){
    hit = !strcmp(tval, oval);
  } else if(cop == ESTOPSTRNE){
    hit = strcmp(tval, oval) != 0;
  } else if(cop == ESTOPSTRINC){
    hit = strstr(tval, oval) != NULL;
  } else if(cop == ESTOPSTRBW){
    hit = cbstrfwmatch(tval, oval);
  } else if(cop == ESTOPSTREW){
    hit = cbstrbwmatch(tval, oval);
  } else if(cop == ESTOPSTRAND){
    hit = est_check_strand(tval, oval);
  } else if(cop == ESTOPSTROR){
    hit = est_check_stror(tval, oval);
  } else if(cop == ESTOPSTROREQ){
    hit = est_check_stroreq(tval, oval);
  } else if(cop == ESTOPSTRRX){
    hit = regex ? est_regex_match(regex, tval) : FALSE;
  } else if(cop == ESTOPNUMEQ){
    hit = cbstrmktime(tval) == onum;
  } else if(cop == ESTOPNUMNE){
    hit = cbstrmktime(tval) != onum;
  } else if(cop == ESTOPNUMGT){
    hit = cbstrmktime(tval) > onum;
  } else if(cop == ESTOPNUMGE){
    hit = cbstrmktime(tval) >= onum;
  } else if(cop == ESTOPNUMLT){
    hit = cbstrmktime(tval) < onum;
  } else if(cop == ESTOPNUMLE){
    hit = cbstrmktime(tval) <= onum;
  } else if(cop == ESTOPNUMBT){
    hit = est_check_numbt(tval, oval);
  } else if(cop == ESTOPDUMMY){
    hit = TRUE;
  } else {
    hit = FALSE;
  }
  free(cval);
  return sign ? hit : !hit;
}


/* Check whether a string includes all tokens in another string.
   `tval' specifies the target value;
   `oval' specifies the operation value;
   The return value is the result of the check. */
static int est_check_strand(const char *tval, const char *oval){
  const char *sp, *ep, *rp, *pp, *qp;
  int hit;
  assert(tval && oval);
  sp = oval;
  while(*sp != '\0'){
    while(*sp == ' ' || *sp == ','){
      sp++;
    }
    ep = sp;
    while(*ep != '\0' && *ep != ' ' && *ep != ','){
      ep++;
    }
    if(ep > sp){
      hit = FALSE;
      for(rp = tval; *rp != '\0'; rp++){
        for(pp = sp, qp = rp; pp < ep; pp++, qp++){
          if(*pp != *qp) break;
        }
        if(pp == ep && (*qp == '\0' || *qp == ' ' || *qp == ',')){
          hit = TRUE;
          break;
        }
      }
      if(!hit) return FALSE;
    }
    sp = ep;
  }
  return TRUE;
}


/* Check whether a string includes at least one token in another string.
   `tval' specifies the target value;
   `oval' specifies the operation value;
   The return value is the result of the check. */
static int est_check_stror(const char *tval, const char *oval){
  const char *sp, *ep, *rp, *pp, *qp;
  int hit;
  assert(tval && oval);
  sp = oval;
  while(*sp != '\0'){
    while(*sp == ' ' || *sp == ','){
      sp++;
    }
    ep = sp;
    while(*ep != '\0' && *ep != ' ' && *ep != ','){
      ep++;
    }
    if(ep > sp){
      hit = FALSE;
      for(rp = tval; *rp != '\0'; rp++){
        for(pp = sp, qp = rp; pp < ep; pp++, qp++){
          if(*pp != *qp) break;
        }
        if(pp == ep && (*qp == '\0' || *qp == ' ' || *qp == ',')){
          hit = TRUE;
          break;
        }
      }
      if(hit) return TRUE;
    }
    sp = ep;
  }
  return FALSE;
}


/* Check whether a string is equal to at least one token in another string.
   `tval' specifies the target value;
   `oval' specifies the operation value;
   The return value is the result of the check. */
static int est_check_stroreq(const char *tval, const char *oval){
  const char *sp, *ep, *rp;
  assert(tval && oval);
  sp = oval;
  while(*sp != '\0'){
    while(*sp == ' ' || *sp == ','){
      sp++;
    }
    ep = sp;
    while(*ep != '\0' && *ep != ' ' && *ep != ','){
      ep++;
    }
    if(ep > sp){
      for(rp = tval; *rp != '\0'; rp++){
        if(*sp != *rp || sp >= ep) break;
        sp++;
      }
      if(*rp == '\0' && sp == ep) return TRUE;
    }
    sp = ep;
  }
  return FALSE;
}


/* Check whether a decimal string is between two tokens in another string.
   `tval' specifies the target value;
   `oval' specifies the operation value;
   The return value is the result of the check. */
static int est_check_numbt(const char *tval, const char *oval){
  time_t val, lower, upper, swap;
  char numbuf[ESTNUMBUFSIZ];
  int i;
  for(i = 0; i < ESTNUMBUFSIZ && oval[i] != '\0' && oval[i] != ' ' && oval[i] != '\t'; i++){
    numbuf[i] = oval[i];
  }
  numbuf[i] = '\0';
  oval += i;
  while(*oval == ' ' || *oval == '\t'){
    oval++;
  }
  if(*oval == '\0') return FALSE;
  val = cbstrmktime(tval);
  lower = cbstrmktime(numbuf);
  upper = cbstrmktime(oval);
  if(lower > upper){
    swap = lower;
    lower = upper;
    upper = swap;
  }
  return val >= lower && val <= upper;
}


/* Compare two keywords by scores in descending order.
   `ap' specifies the pointer to one keyword.
   `bp' specifies the pointer to the other keyword.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_keysc_compare(const void *ap, const void *bp){
  assert(ap && bp);
  return ((ESTKEYSC *)bp)->pt - ((ESTKEYSC *)ap)->pt;
}


/* Get a similar set of documents in a database.
   `db' specifies a database object.
   `svmap' specifies a map object of a seed vector.
   `nump' specifies the pointer to which the number of elements in the result is assigned.
   `knum' specifies the number of keywords to get candidates.
   `unum' specifies the number of adopted documents for a keyword.
   `tfidf' specifies whether to perform TF-IDF tuning.
   `nmin' specifies the minimum value for narrowing.
   `auxmin' specifies the minimum hits to adopt the auxiliary index.  If it is not more than 0,
   the auxiliary index is not used.
   `auxwords' specifies a map object where keywords used with the auxiliary index are stored.  If
   it is `NULL', it is not used.
   The return value is an array of score structures of corresponding documents. */
static ESTSCORE *est_search_similar(ESTDB *db, CBMAP *svmap, int *nump,
                                    int knum, int unum, int mnum, int tfidf,
                                    double nmin, int auxmin, CBMAP *auxwords){
  ESTSCORE *scores, *tscores;
  CBMAP *tvmap;
  const char *word;
  int i, j, vnum, snum, tmax, tsnum, nnum, lid, *svec, *tvec;
  double dval;
  assert(db && svmap && nump && knum >= 0 && unum >= 0 && nmin >= 0.0);
  CB_MALLOC(scores, sizeof(ESTSCORE) * (unum * knum + CB_LISTNUM(db->pdocs)) + 1);
  snum = 0;
  if((vnum = cbmaprnum(svmap)) < 1) vnum = 1;
  cbmapiterinit(svmap);
  tmax = unum;
  for(i = 0; (i < knum || (i < knum * 2 && snum < unum * 2)) &&
        (word = cbmapiternext(svmap, NULL)) != NULL; i++){
    while(*word > '\0' && *word <= ' '){
      word++;
    }
    tscores = est_search_union(db, word, 1, NULL, &tsnum, NULL, TRUE, auxmin, auxwords);
    qsort(tscores, tsnum, sizeof(ESTSCORE), est_score_compare_by_score_desc);
    for(j = 0; j < tmax && j < tsnum; j++){
      scores[snum].id = tscores[j].id;
      scores[snum].score = tscores[j].score * (knum * 2.2 - i);
      snum++;
    }
    free(tscores);
    tmax -= unum / knum / 1.25;
    if(tmax < unum / 4) tmax = unum / 4;
  }
  for(i = 0; i < CB_LISTNUM(db->pdocs); i++){
    scores[snum].id = ESTPDOCIDMIN + i;
    scores[snum].score = 1;
    snum++;
  }
  qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_id_asc);
  nnum = 0;
  lid = -1;
  for(i = 0; i < snum; i++){
    if(nnum > 0 && scores[i].id == lid){
      scores[nnum-1].score += scores[i].score;
      continue;
    }
    scores[nnum].id = scores[i].id;
    scores[nnum].score = scores[i].score;
    nnum++;
    lid = scores[i].id;
  }
  snum = nnum;
  qsort(scores, snum, sizeof(ESTSCORE), est_score_compare_by_score_desc);
  nnum = 0;
  CB_MALLOC(svec, vnum * sizeof(int));
  CB_MALLOC(tvec, vnum * sizeof(int));
  est_vector_set_seed(svmap, svec, vnum);
  for(i = 0; i < snum && nnum < mnum; i++){
    tvmap = est_get_tvmap(db, scores[i].id, vnum, tfidf);
    if(tvmap){
      est_vector_set_target(svmap, tvmap, tvec, vnum);
      if((dval = est_vector_cosine(svec, tvec, vnum)) >= nmin){
        scores[nnum].id = scores[i].id;
        scores[nnum].score = (int)(dval * 10000);
        if(scores[nnum].score == 9999) scores[nnum].score = 10000;
        scores[nnum].value = NULL;
        nnum++;
      }
      cbmapclose(tvmap);
    }
  }
  free(tvec);
  free(svec);
  snum = nnum;
  *nump = snum;
  return scores;
}


/* Create a map object of a vector for similar search from a phrase.
   `phrase' specifies a search phrase for similar search.
   The return value is a map object of the seed vector. */
static CBMAP *est_phrase_vector(const char *phrase){
  ESTKEYSC *scores;
  CBMAP *svmap;
  CBLIST *list;
  const char *pv, *rp;
  char *utext, *rtext;
  int i, num, len, size;
  svmap = cbmapopenex(ESTMINIBNUM);
  CB_LISTOPEN(list);
  while(*phrase != '\0'){
    if(*phrase == ESTOPWITH[0] && cbstrfwmatch(phrase, ESTOPWITH)){
      phrase += strlen(ESTOPWITH);
      pv = phrase;
      while(*phrase != '\0'){
        if(*phrase <= ' ' && cbstrfwmatch(phrase + 1, ESTOPWITH)){
          phrase++;
          break;
        }
        phrase++;
      }
      CB_LISTPUSH(list, pv, phrase - pv);
    } else {
      phrase++;
    }
  }
  for(i = 0; i < CB_LISTNUM(list); i++){
    pv = CB_LISTVAL(list, i);
    while(*pv > '\0' && *pv <= ' '){
      pv++;
    }
    num = strtol(pv, (char **)&rp, 10);
    if(rp && (len = rp - pv) > 0 && num >= 0){
      utext = est_uconv_in(rp, strlen(rp), &size);
      est_normalize_text((unsigned char *)utext, size, &size);
      est_canonicalize_text((unsigned char *)utext, size, FALSE);
      rtext = est_uconv_out(utext, size, NULL);
      cbstrsqzspc(rtext);
      if(rtext[0] != '\0') cbmapput(svmap, rtext, -1, pv, len, FALSE);
      free(rtext);
      free(utext);
    }
  }
  CB_LISTCLOSE(list);
  CB_MALLOC(scores, cbmaprnum(svmap) * sizeof(ESTKEYSC) + 1);
  cbmapiterinit(svmap);
  for(i = 0; (rp = cbmapiternext(svmap, &len)) != NULL; i++){
    scores[i].word = rp;
    scores[i].wsiz = len;
    scores[i].pt = atoi(cbmapiterval(rp, NULL));
  }
  qsort(scores, i, sizeof(ESTKEYSC), est_keysc_compare);
  for(i--; i >= 0; i--){
    cbmapmove(svmap, scores[i].word, scores[i].wsiz, TRUE);
  }
  free(scores);
  return svmap;
}


/* Get the target vector of a document dynamically.
   `db' specifies a database object.
   `id' specifies the ID of a document.
   `vnum' specifies the number of dimensions of the vector.
   `tfidf' specifies whether to perform TF-IDF tuning.
   The return value is a map object of the target vector. */
static CBMAP *est_get_tvmap(ESTDB *db, int id, int vnum, int tfidf){
  ESTDOC *doc;
  CBMAP *tvmap;
  assert(db && id > 0);
  if((tvmap = est_db_get_keywords(db, id)) != NULL) return tvmap;
  if(!(doc = est_db_get_doc(db, id, 0))) return NULL;
  tvmap = est_db_etch_doc(tfidf ? db : NULL, doc, vnum);
  est_doc_delete(doc);
  if(dpwritable(db->metadb)) est_db_put_keywords(db, id, tvmap, 1.0);
  return tvmap;
}


/* Calculate sameness of two URLs.
   The return value is 0 if the both have different servers, 1 if the both have the same server,
   2 if the both have the same parent directory, 3 if the both have the same file. */
static int est_url_sameness(const char *aurl, const char *burl){
  const char *apv, *bpv;
  int i, alen, blen;
  assert(aurl && burl);
  if((apv = strstr(aurl, "://")) != NULL){
    aurl = apv + 3;
  } else {
    return 0;
  }
  if((bpv = strstr(burl, "://")) != NULL){
    burl = bpv + 3;
  } else {
    return 0;
  }
  if(!(apv = strchr(aurl, '/'))) apv = aurl + strlen(aurl);
  if(!(bpv = strchr(burl, '/'))) bpv = burl + strlen(burl);
  alen = apv - aurl;
  blen = bpv - burl;
  if(alen != blen || memcmp(aurl, burl, alen)) return 0;
  aurl = *apv == '\0' ? "/" : apv;
  burl = *bpv == '\0' ? "/" : bpv;
  if(!(apv = strchr(aurl, '?'))) apv = aurl + strlen(aurl);
  if(!(bpv = strchr(burl, '?'))) bpv = burl + strlen(burl);
  alen = apv - aurl;
  blen = bpv - burl;
  if(alen == blen && !memcmp(aurl, burl, alen)) return 3;
  apv = aurl;
  for(i = 0; i < alen; i++){
    if(aurl[i] == '/') apv = aurl + i;
  }
  bpv = burl;
  for(i = 0; i < blen; i++){
    if(burl[i] == '/') bpv = burl + i;
  }
  alen = apv - aurl;
  blen = bpv - burl;
  if(alen == blen && !memcmp(aurl, burl, alen)) return 2;
  return 1;
}


/* Close the handle to the file of random number generator. */
static void est_random_fclose(void){
  if(est_random_ifp) fclose(est_random_ifp);
}


/* Dispatch a signal to the corresponding handler.
   Signum specifies the number of catched signal. */
static int est_signal_dispatch(int signum){
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  switch(signum){
  case CTRL_C_EVENT: case CTRL_BREAK_EVENT: case CTRL_CLOSE_EVENT:
    signum = 2;
    break;
  case CTRL_LOGOFF_EVENT: case CTRL_SHUTDOWN_EVENT:
    signum = 15;
    break;
  }
  if(est_signal_handlers[signum]) est_signal_handlers[signum](signum);
  return TRUE;
#else
  assert(signum >= 0);
  if(est_signal_handlers[signum]) est_signal_handlers[signum](signum);
  return TRUE;
#endif
}



/* END OF FILE */
