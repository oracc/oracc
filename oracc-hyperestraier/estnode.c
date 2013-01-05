/*************************************************************************************************
 * Implementation of the node API
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
#include "estnode.h"
#include "myconf.h"

#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
#define socklen_t       int
#define in_addr_t       int
#elif defined(_SYS_MACOSX_)
#define socklen_t       int
#endif

#define ESTNUMBUFSIZ   32                /* size of a buffer for a number */
#define ESTPATHBUFSIZ  4096              /* size of a buffer for a path */
#define ESTIOBUFSIZ    8192              /* size of a buffer for I/O */
#define ESTMINIBNUM    31                /* bucket number of map for attributes */
#define ESTLISTUNIT    64                /* allocation unit number of a list */
#define ESTVAXVECNUM   256               /* maximum number of vector dimensions */
#define ESTECLPMAX     64                /* maximum number of documents to be eclipsed */
#define ESTITERUNIT    256               /* retrieval unit number of iterator */
#define ESTRESLIMSIZE  (1048576*128)     /* limitation of the entity body of response */
#define ESTWWIDTHDEF   480               /* default of whole width of a snippet */
#define ESTHWIDTHDEF   96                /* default of head width of a snippet */
#define ESTAWIDTHDEF   96                /* default of around width of a snippet */
#define ESTDNHOLDSEC   300               /* holding time of domain names */
#define ESTDNHOLDNUM   4096              /* holding number of domain names */

typedef struct {                         /* type of structure for interaction of a URL */
  int alive;                             /* whether to be alive */
  pthread_cond_t *cond;                  /* condition variable */
  const char *url;                       /* URL */
  const char *pxhost;                    /* host name of proxy */
  int pxport;                            /* port number of proxy */
  int limsize;                           /* limitation of the response body */
  const char *auth;                      /* authority */
  const CBLIST *reqheads;                /* request headers */
  const char *reqbody;                   /* request body */
  int rbsiz;                             /* size of the request body */
  int *rescodep;                         /* pointer to a variable for status code */
  CBMAP *resheads;                       /* response headers */
  CBDATUM *resbody;                      /* response body */
} TARGSHUTTLE;


/* private function prototypes */
static char *est_gethostaddrbyname(const char *name);
static int est_sock_close(int sock);
static int est_inet_aton(const char *cp, struct in_addr *inp);
static void *est_url_shuttle_impl(void *targ);
static void est_sockpt_down(void *sp);
static int est_node_set_info(ESTNODE *node);
static void est_node_iter_delete(ESTNODE *node);
static int est_node_read_list(ESTNODE *node);
static void est_parse_search_header(ESTNODERES *nres, const char *str);
static void est_parse_search_body(ESTNODERES *nres, char *str);



/*************************************************************************************************
 * API for the network environment
 *************************************************************************************************/


/* Cache of host addresses. */
CBMAP *est_host_addrs = NULL;
int est_host_attrs_cnt = 0;
pthread_mutex_t est_host_addrs_mutex = PTHREAD_MUTEX_INITIALIZER;


/* Initialize the networking environment. */
int est_init_net_env(void){
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  WSADATA wsaData;
  if(pthread_mutex_lock(&est_host_addrs_mutex) != 0) return FALSE;
  if(WSAStartup(MAKEWORD(2,0), &wsaData) != 0){
    pthread_mutex_unlock(&est_host_addrs_mutex);
    return FALSE;
  }
  if(est_host_attrs_cnt < 1) est_host_addrs = cbmapopenex(ESTDNHOLDNUM + 1);
  est_host_attrs_cnt++;
  pthread_mutex_unlock(&est_host_addrs_mutex);
  return TRUE;
#else
  if(pthread_mutex_lock(&est_host_addrs_mutex) != 0) return FALSE;
  if(est_host_attrs_cnt < 1) est_host_addrs = cbmapopenex(ESTDNHOLDNUM + 1);
  est_host_attrs_cnt++;
  pthread_mutex_unlock(&est_host_addrs_mutex);
  return TRUE;
#endif
}


/* Free the networking environment. */
void est_free_net_env(void){
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  if(pthread_mutex_lock(&est_host_addrs_mutex) != 0) return;
  if(est_host_attrs_cnt < 1){
    pthread_mutex_unlock(&est_host_addrs_mutex);
    return;
  }
  est_host_attrs_cnt--;
  if(est_host_attrs_cnt < 1){
    cbmapclose(est_host_addrs);
    est_host_addrs = NULL;
  }
  WSACleanup();
  pthread_mutex_unlock(&est_host_addrs_mutex);
#else
  if(pthread_mutex_lock(&est_host_addrs_mutex) != 0) return;
  if(est_host_attrs_cnt < 1){
    pthread_mutex_unlock(&est_host_addrs_mutex);
    return;
  }
  est_host_attrs_cnt--;
  if(est_host_attrs_cnt < 1){
    cbmapclose(est_host_addrs);
    est_host_addrs = NULL;
  }
  pthread_mutex_unlock(&est_host_addrs_mutex);
#endif
}



/*************************************************************************************************
 * API for search result of node
 *************************************************************************************************/


/* Delete a node result object. */
void est_noderes_delete(ESTNODERES *nres){
  ESTRESDOC *docs;
  int i, dnum;
  assert(nres);
  cbmapclose(nres->hints);
  free(nres->survivors);
  docs = nres->docs + nres->top;
  dnum = nres->dnum;
  for(i = 0; i < dnum; i++){
    free(nres->docs[i].children);
    if(docs[i].kwords) cbmapclose(docs[i].kwords);
    free(docs[i].snippet);
    cbmapclose(docs[i].attrs);
  }
  free(nres->docs);
  free(nres);
}


/* Get a map object for hints of a node result object. */
CBMAP *est_noderes_hints(ESTNODERES *nres){
  assert(nres);
  return nres->hints;
}


/* Eclipse similar documents of a node result object. */
void est_noderes_eclipse(ESTNODERES *nres, int num, double limit){
  ESTRESDOC *docs;
  CBLIST *fields;
  CBMAP *kwords;
  const char *kbuf, *vbuf;
  int i, j, dnum, ksiz, vsiz, snum, vnum, svec[ESTVAXVECNUM], tvec[ESTVAXVECNUM];
  double dval;
  assert(nres);
  docs = nres->docs + nres->top;
  dnum = nres->dnum;
  if(num <= 0){
    for(i = 0; i < dnum; i++){
      docs[i].cnum = 0;
      docs[i].similarity = -1.0;
    }
    free(nres->survivors);
    nres->survivors = NULL;
    nres->snum = 0;
    return;
  }
  for(i = 0; i < dnum; i++){
    if(docs[i].kwords) continue;
    if(!(vbuf = cbmapget(docs[i].attrs, ESTDCNTLVECTOR, -1, &vsiz)) || vsiz < 0) continue;
    kwords = cbmapopenex(ESTMINIBNUM);
    fields = cbsplit(vbuf, vsiz, "\t");
    for(j = 0; j < cblistnum(fields) - 1; j += 2){
      kbuf = cblistval(fields, j, &ksiz);
      vbuf = cblistval(fields, j + 1, &vsiz);
      cbmapput(kwords, kbuf, ksiz, vbuf, vsiz, FALSE);
    }
    cblistclose(fields);
    docs[i].kwords = kwords;
    docs[i].cnum = 0;
    docs[i].similarity = -1.0;
  }
  snum = 0;
  for(i = 0; i < dnum && snum < num; i++){
    if(docs[i].similarity >= 0.0) continue;
    if(docs[i].kwords && (vnum = cbmaprnum(docs[i].kwords)) > 0){
      vnum = vnum < ESTVAXVECNUM ? vnum : ESTVAXVECNUM;
      est_vector_set_seed(docs[i].kwords, svec, vnum);
      for(j = i + 1; j < dnum; j++){
        if(docs[j].similarity >= 0.0 || !docs[j].kwords || cbmaprnum(docs[j].kwords) < 1) continue;
        est_vector_set_target(docs[i].kwords, docs[j].kwords, tvec, vnum);
        dval = est_vector_cosine(svec, tvec, vnum);
        if(dval > limit && docs[i].cnum < ESTECLPMAX){
          if(!docs[i].children) docs[i].children = cbmalloc(ESTECLPMAX * sizeof(ESTRESDOC *));
          docs[i].children[docs[i].cnum++] = docs + j;
          docs[j].similarity = dval;
        }
      }
    }
    snum++;
  }
  free(nres->survivors);
  nres->survivors = cbmalloc(dnum * sizeof(ESTRESDOC *) + 1);
  snum = 0;
  for(i = 0; i < dnum; i++){
    if(docs[i].similarity >= 0.0) continue;
    nres->survivors[snum++] = docs + i;
  }
  nres->snum = snum;
}


/* Get the number of documents in a node result object. */
int est_noderes_doc_num(ESTNODERES *nres){
  assert(nres);
  return nres->survivors ? nres->snum : nres->dnum;
}


/* Refer a result document object in a node result object. */
ESTRESDOC *est_noderes_get_doc(ESTNODERES *nres, int index){
  assert(nres && index >= 0);
  if(nres->survivors){
    if(index >= nres->snum) return NULL;
    return nres->survivors[index];
  }
  if(index >= nres->dnum) return NULL;
  return nres->docs + (nres->top + index);
}


/* Get the URI of a result document object. */
const char *est_resdoc_uri(ESTRESDOC *rdoc){
  assert(rdoc);
  return rdoc->uri;
}


/* Get a list of attribute names of a result document object. */
CBLIST *est_resdoc_attr_names(ESTRESDOC *rdoc){
  CBLIST *names;
  const char *kbuf;
  int ksiz;
  assert(rdoc);
  names = cblistopen();
  cbmapiterinit(rdoc->attrs);
  while((kbuf = cbmapiternext(rdoc->attrs, &ksiz)) != NULL){
    if(kbuf[0] != '%') cblistpush(names, kbuf, ksiz);
  }
  cblistsort(names);
  return names;
}


/* Get the value of an attribute of a result document object. */
const char *est_resdoc_attr(ESTRESDOC *rdoc, const char *name){
  assert(rdoc && name);
  return cbmapget(rdoc->attrs, name, -1, NULL);
}


/* Get the value of an attribute of a result document object. */
const char *est_resdoc_snippet(ESTRESDOC *rdoc){
  assert(rdoc);
  return rdoc->snippet;
}


/* Get keywords of a result document object. */
const char *est_resdoc_keywords(ESTRESDOC *rdoc){
  const char *vbuf;
  assert(rdoc);
  vbuf = cbmapget(rdoc->attrs, ESTDCNTLVECTOR, -1, NULL);
  return vbuf ? vbuf : "";
}


/* Get an array of documents eclipsed by a result document object. */
ESTRESDOC **est_resdoc_shadows(ESTRESDOC *rdoc, int *np){
  assert(rdoc && np);
  if(!rdoc->children){
    *np = 0;
    return (ESTRESDOC **)"";
  }
  *np = rdoc->cnum;
  return rdoc->children;
}


/* Get similarity of an eclipsed result document object. */
double est_resdoc_similarity(ESTRESDOC *rdoc){
  assert(rdoc);
  return rdoc->similarity;
}



/*************************************************************************************************
 * API for node
 *************************************************************************************************/


/* Create a node connection object. */
ESTNODE *est_node_new(const char *url){
  ESTNODE *node;
  node = cbmalloc(sizeof(ESTNODE));
  node->url = cbmemdup(url, -1);
  node->pxhost = NULL;
  node->pxport = 0;
  node->timeout = -1;
  node->auth = NULL;
  node->name = NULL;
  node->label = NULL;
  node->dnum = -1;
  node->wnum = -1;
  node->size = -1.0;
  node->admins = NULL;
  node->users = NULL;
  node->links = NULL;
  node->wwidth = ESTWWIDTHDEF;
  node->hwidth = ESTHWIDTHDEF;
  node->awidth = ESTAWIDTHDEF;
  node->ilist = NULL;
  node->iprev = NULL;
  node->status = 0;
  node->heads = cbmapopenex(ESTMINIBNUM);
  return node;
}


/* Destroy a node connection object. */
void est_node_delete(ESTNODE *node){
  assert(node);
  cbmapclose(node->heads);
  est_node_iter_delete(node);
  if(node->links) cblistclose(node->links);
  if(node->users) cblistclose(node->users);
  if(node->admins) cblistclose(node->admins);
  free(node->label);
  free(node->name);
  free(node->auth);
  free(node->pxhost);
  free(node->url);
  free(node);
}


/* Set the proxy information of a node connection object. */
void est_node_set_proxy(ESTNODE *node, const char *host, int port){
  assert(node && host && port >= 0);
  free(node->pxhost);
  node->pxhost = cbmemdup(host, -1);
  node->pxport = port;
}


/* Set timeout of a connection. */
void est_node_set_timeout(ESTNODE *node, int sec){
  assert(node && sec >= 0);
  node->timeout = sec;
}


/* Set the authoririty information of a node connection object. */
void est_node_set_auth(ESTNODE *node, const char *name, const char *passwd){
  assert(node && name && passwd);
  free(node->auth);
  node->auth = cbsprintf("%s:%s", name, passwd);
}


/* Get the status code of the last request of a node. */
int est_node_status(ESTNODE *node){
  assert(node);
  return node->status;
}


/* Synchronize updating contents of the database of a node. */
int est_node_sync(ESTNODE *node){
  CBLIST *reqheads;
  CBDATUM *reqbody;
  const char *kbuf;
  char url[ESTPATHBUFSIZ], *vbuf;
  int rescode, err;
  assert(node);
  err = FALSE;
  sprintf(url, "%s/sync", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  cblistpush(reqheads, "Content-Type: " ESTFORMTYPE, -1);
  reqbody = cbdatumopen(NULL, -1);
  if(!est_url_shuttle(url, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, cbdatumptr(reqbody), cbdatumsize(reqbody),
                      &rescode, NULL, NULL)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  cbdatumclose(reqbody);
  cblistclose(reqheads);
  return err ? FALSE : TRUE;
}


/* Optimize the database of a node. */
int est_node_optimize(ESTNODE *node){
  CBLIST *reqheads;
  CBDATUM *reqbody;
  const char *kbuf;
  char url[ESTPATHBUFSIZ], *vbuf;
  int rescode, err;
  assert(node);
  err = FALSE;
  sprintf(url, "%s/optimize", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  cblistpush(reqheads, "Content-Type: " ESTFORMTYPE, -1);
  reqbody = cbdatumopen(NULL, -1);
  if(!est_url_shuttle(url, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, cbdatumptr(reqbody), cbdatumsize(reqbody),
                      &rescode, NULL, NULL)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  cbdatumclose(reqbody);
  cblistclose(reqheads);
  return err ? FALSE : TRUE;
}


/* Add a document to a node. */
int est_node_put_doc(ESTNODE *node, ESTDOC *doc){
  CBLIST *reqheads;
  const char *kbuf;
  char url[ESTPATHBUFSIZ], *vbuf, *reqbody;
  int rescode, err;
  assert(node && doc);
  err = FALSE;
  sprintf(url, "%s/put_doc", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  cblistpush(reqheads, "Content-Type: " ESTDRAFTTYPE, -1);
  reqbody = est_doc_dump_draft(doc);
  if(!est_url_shuttle(url, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, reqbody, strlen(reqbody), &rescode, NULL, NULL)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  free(reqbody);
  cblistclose(reqheads);
  return err ? FALSE : TRUE;
}


/* Remove a document from a node. */
int est_node_out_doc(ESTNODE *node, int id){
  CBLIST *reqheads;
  CBDATUM *reqbody;
  const char *kbuf;
  char url[ESTPATHBUFSIZ], *vbuf;
  int rescode, err;
  assert(node && id > 0);
  err = FALSE;
  sprintf(url, "%s/out_doc", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  cblistpush(reqheads, "Content-Type: " ESTFORMTYPE, -1);
  reqbody = cbdatumopen(NULL, -1);
  cbdatumprintf(reqbody, "id=%d", id);
  if(!est_url_shuttle(url, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, cbdatumptr(reqbody), cbdatumsize(reqbody),
                      &rescode, NULL, NULL)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  cbdatumclose(reqbody);
  cblistclose(reqheads);
  return err ? FALSE : TRUE;
}


/* Remove a document specified by URI from a node. */
int est_node_out_doc_by_uri(ESTNODE *node, const char *uri){
  CBLIST *reqheads;
  CBDATUM *reqbody;
  const char *kbuf;
  char url[ESTPATHBUFSIZ], *vbuf;
  int rescode, err;
  assert(node && uri);
  err = FALSE;
  sprintf(url, "%s/out_doc", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  cblistpush(reqheads, "Content-Type: " ESTFORMTYPE, -1);
  reqbody = cbdatumopen(NULL, -1);
  cbdatumprintf(reqbody, "uri=%?", uri);
  if(!est_url_shuttle(url, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, cbdatumptr(reqbody), cbdatumsize(reqbody),
                      &rescode, NULL, NULL)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  cbdatumclose(reqbody);
  cblistclose(reqheads);
  return err ? FALSE : TRUE;
}


/* Edit attributes of a document in a node. */
int est_node_edit_doc(ESTNODE *node, ESTDOC *doc){
  CBLIST *reqheads, *names;
  CBDATUM *reqbody;
  const char *kbuf;
  char url[ESTPATHBUFSIZ], *vbuf;
  int i, rescode, err, ksiz;
  assert(node && doc);
  err = FALSE;
  sprintf(url, "%s/edit_doc", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  cblistpush(reqheads, "Content-Type: " ESTDRAFTTYPE, -1);
  reqbody = cbdatumopen(NULL, -1);
  names = est_doc_attr_names(doc);
  for(i = 0; i < cblistnum(names); i++){
    kbuf = cblistval(names, i, &ksiz);
    cbdatumcat(reqbody, kbuf, ksiz);
    cbdatumcat(reqbody, "=", 1);
    cbdatumcat(reqbody, est_doc_attr(doc, kbuf), -1);
    cbdatumcat(reqbody, "\n", 1);
  }
  cbdatumcat(reqbody, "\n", 1);
  if(!est_url_shuttle(url, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, cbdatumptr(reqbody), cbdatumsize(reqbody),
                      &rescode, NULL, NULL)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  cblistclose(names);
  cbdatumclose(reqbody);
  cblistclose(reqheads);
  return err ? FALSE : TRUE;
}


/* Retrieve a document in a node. */
ESTDOC *est_node_get_doc(ESTNODE *node, int id){
  ESTDOC *doc;
  CBLIST *reqheads;
  CBDATUM *reqbody, *resbody;
  const char *kbuf;
  char url[ESTPATHBUFSIZ], *vbuf;
  int rescode, err;
  assert(node && id > 0);
  err = FALSE;
  sprintf(url, "%s/get_doc", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  cblistpush(reqheads, "Content-Type: " ESTFORMTYPE, -1);
  reqbody = cbdatumopen(NULL, -1);
  cbdatumprintf(reqbody, "id=%d", id);
  resbody = cbdatumopen(NULL, -1);
  if(!est_url_shuttle(url, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, cbdatumptr(reqbody), cbdatumsize(reqbody),
                      &rescode, NULL, resbody)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  doc = err ? NULL : est_doc_new_from_draft(cbdatumptr(resbody));
  cbdatumclose(resbody);
  cbdatumclose(reqbody);
  cblistclose(reqheads);
  return doc;
}


/* Retrieve a document specified by URI in a node. */
ESTDOC *est_node_get_doc_by_uri(ESTNODE *node, const char *uri){
  ESTDOC *doc;
  CBLIST *reqheads;
  CBDATUM *reqbody, *resbody;
  const char *kbuf;
  char url[ESTPATHBUFSIZ], *vbuf;
  int rescode, err;
  assert(node && uri);
  err = FALSE;
  sprintf(url, "%s/get_doc", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  cblistpush(reqheads, "Content-Type: " ESTFORMTYPE, -1);
  reqbody = cbdatumopen(NULL, -1);
  cbdatumprintf(reqbody, "uri=%?", uri);
  resbody = cbdatumopen(NULL, -1);
  if(!est_url_shuttle(url, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, cbdatumptr(reqbody), cbdatumsize(reqbody),
                      &rescode, NULL, resbody)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  doc = err ? NULL : est_doc_new_from_draft(cbdatumptr(resbody));
  cbdatumclose(resbody);
  cbdatumclose(reqbody);
  cblistclose(reqheads);
  return doc;
}


/* Retrieve the value of an attribute of a document in a node. */
char *est_node_get_doc_attr(ESTNODE *node, int id, const char *name){
  CBLIST *reqheads;
  CBDATUM *reqbody, *resbody;
  const char *kbuf;
  char url[ESTPATHBUFSIZ], *vbuf;
  int rescode, err;
  assert(node && id > 0 && name);
  err = FALSE;
  sprintf(url, "%s/get_doc_attr", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  cblistpush(reqheads, "Content-Type: " ESTFORMTYPE, -1);
  reqbody = cbdatumopen(NULL, -1);
  cbdatumprintf(reqbody, "id=%d&attr=%?", id, name);
  resbody = cbdatumopen(NULL, -1);
  if(!est_url_shuttle(url, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, cbdatumptr(reqbody), cbdatumsize(reqbody),
                      &rescode, NULL, resbody)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  if(err){
    cbdatumclose(resbody);
    vbuf = NULL;
  } else {
    vbuf = cbdatumtomalloc(resbody, NULL);
    cbstrtrim(vbuf);
  }
  cbdatumclose(reqbody);
  cblistclose(reqheads);
  return vbuf;
}


/* Retrieve the value of an attribute of a document specified by URI in a node. */
char *est_node_get_doc_attr_by_uri(ESTNODE *node, const char *uri, const char *name){
  CBLIST *reqheads;
  CBDATUM *reqbody, *resbody;
  const char *kbuf;
  char url[ESTPATHBUFSIZ], *vbuf;
  int rescode, err;
  assert(node && uri && name);
  err = FALSE;
  sprintf(url, "%s/get_doc_attr", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  cblistpush(reqheads, "Content-Type: " ESTFORMTYPE, -1);
  reqbody = cbdatumopen(NULL, -1);
  cbdatumprintf(reqbody, "uri=%?&attr=%?", uri, name);
  resbody = cbdatumopen(NULL, -1);
  if(!est_url_shuttle(url, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, cbdatumptr(reqbody), cbdatumsize(reqbody),
                      &rescode, NULL, resbody)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  if(err){
    cbdatumclose(resbody);
    vbuf = NULL;
  } else {
    vbuf = cbdatumtomalloc(resbody, NULL);
    cbstrtrim(vbuf);
  }
  cbdatumclose(reqbody);
  cblistclose(reqheads);
  return vbuf;
}


/* Extract keywords of a document. */
CBMAP *est_node_etch_doc(ESTNODE *node, int id){
  CBLIST *reqheads;
  CBDATUM *reqbody, *resbody;
  CBMAP *kwords;
  CBLIST *lines;
  const char *kbuf, *pv;
  char url[ESTPATHBUFSIZ], *vbuf;
  int i, rescode, err;
  assert(node && id > 0);
  err = FALSE;
  sprintf(url, "%s/etch_doc", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  cblistpush(reqheads, "Content-Type: " ESTFORMTYPE, -1);
  reqbody = cbdatumopen(NULL, -1);
  cbdatumprintf(reqbody, "id=%d", id);
  resbody = cbdatumopen(NULL, -1);
  if(!est_url_shuttle(url, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, cbdatumptr(reqbody), cbdatumsize(reqbody),
                      &rescode, NULL, resbody)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  if(err){
    kwords = NULL;
  } else {
    kwords = cbmapopenex(ESTMINIBNUM);
    lines = cbsplit(cbdatumptr(resbody), cbdatumsize(resbody), "\n");
    for(i = 0; i < cblistnum(lines); i++){
      kbuf = cblistval(lines, i, NULL);
      if(!(pv = strchr(kbuf, '\t')) || pv == kbuf) continue;
      cbmapput(kwords, kbuf, pv - kbuf, pv + 1, -1, TRUE);
    }
    cblistclose(lines);
  }
  cbdatumclose(resbody);
  cbdatumclose(reqbody);
  cblistclose(reqheads);
  return kwords;
}


/* Extract keywords of a document specified by URI in a node. */
CBMAP *est_node_etch_doc_by_uri(ESTNODE *node, const char *uri){
  CBLIST *reqheads;
  CBDATUM *reqbody, *resbody;
  CBMAP *kwords;
  CBLIST *lines;
  const char *kbuf, *pv;
  char url[ESTPATHBUFSIZ], *vbuf;
  int i, rescode, err;
  assert(node && uri);
  err = FALSE;
  sprintf(url, "%s/etch_doc", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  cblistpush(reqheads, "Content-Type: " ESTFORMTYPE, -1);
  reqbody = cbdatumopen(NULL, -1);
  cbdatumprintf(reqbody, "uri=%?", uri);
  resbody = cbdatumopen(NULL, -1);
  if(!est_url_shuttle(url, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, cbdatumptr(reqbody), cbdatumsize(reqbody),
                      &rescode, NULL, resbody)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  if(err){
    kwords = NULL;
  } else {
    kwords = cbmapopenex(ESTMINIBNUM);
    lines = cbsplit(cbdatumptr(resbody), cbdatumsize(resbody), "\n");
    for(i = 0; i < cblistnum(lines); i++){
      kbuf = cblistval(lines, i, NULL);
      if(!(pv = strchr(kbuf, '\t')) || pv == kbuf) continue;
      cbmapput(kwords, kbuf, pv - kbuf, pv + 1, -1, TRUE);
    }
    cblistclose(lines);
  }
  cbdatumclose(resbody);
  cbdatumclose(reqbody);
  cblistclose(reqheads);
  return kwords;
}



/* Get the ID of a document spacified by URI. */
int est_node_uri_to_id(ESTNODE *node, const char *uri){
  CBLIST *reqheads;
  CBDATUM *reqbody, *resbody;
  const char *kbuf;
  char url[ESTPATHBUFSIZ], *vbuf;
  int rescode, err, id;
  assert(node && uri);
  err = FALSE;
  sprintf(url, "%s/uri_to_id", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  cblistpush(reqheads, "Content-Type: " ESTFORMTYPE, -1);
  reqbody = cbdatumopen(NULL, -1);
  cbdatumprintf(reqbody, "uri=%?", uri);
  resbody = cbdatumopen(NULL, -1);
  if(!est_url_shuttle(url, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, cbdatumptr(reqbody), cbdatumsize(reqbody),
                      &rescode, NULL, resbody)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  id = err ? -1 : atoi(cbdatumptr(resbody));
  cbdatumclose(resbody);
  cbdatumclose(reqbody);
  cblistclose(reqheads);
  return id;
}


/* Get the name of a node. */
const char *est_node_name(ESTNODE *node){
  assert(node);
  if(node->name) return node->name;
  est_node_set_info(node);
  return node->name;
}


/* Get the label of a node. */
const char *est_node_label(ESTNODE *node){
  assert(node);
  if(node->label) return node->label;
  est_node_set_info(node);
  return node->label;
}


/* Get the number of documents in a node. */
int est_node_doc_num(ESTNODE *node){
  assert(node);
  if(node->dnum >= 0) return node->dnum;
  est_node_set_info(node);
  return node->dnum;
}


/* Get the number of words in a node. */
int est_node_word_num(ESTNODE *node){
  assert(node);
  if(node->wnum >= 0) return node->wnum;
  est_node_set_info(node);
  return node->wnum;
}


/* Get the size of the datbase of a node. */
double est_node_size(ESTNODE *node){
  assert(node);
  if(node->size >= 0.0) return node->size;
  est_node_set_info(node);
  return node->size;
}


/* Get the usage ratio of the cache of a node. */
double est_node_cache_usage(ESTNODE *node){
  CBLIST *reqheads;
  CBDATUM *resbody;
  const char *kbuf;
  char url[ESTPATHBUFSIZ], *vbuf;
  int rescode, err;
  double ratio;
  assert(node);
  err = FALSE;
  sprintf(url, "%s/cacheusage", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  resbody = cbdatumopen(NULL, -1);
  if(!est_url_shuttle(url, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, NULL, -1, &rescode, NULL, resbody)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  ratio = err ? -1.0 : strtod(cbdatumptr(resbody), NULL);
  cbdatumclose(resbody);
  cblistclose(reqheads);
  return ratio;
}


/* Get a list of names of administrators of a node. */
const CBLIST *est_node_admins(ESTNODE *node){
  assert(node);
  if(node->admins) return node->admins;
  est_node_set_info(node);
  return node->admins;
}


/* Get a list of names of users of a node. */
const CBLIST *est_node_users(ESTNODE *node){
  assert(node);
  if(node->users) return node->users;
  est_node_set_info(node);
  return node->users;
}


/* Get a list of expressions of links of a node. */
const CBLIST *est_node_links(ESTNODE *node){
  assert(node);
  if(node->links) return node->links;
  est_node_set_info(node);
  return node->links;
}


/* Search documents corresponding a condition for a node. */
ESTNODERES *est_node_search(ESTNODE *node, ESTCOND *cond, int depth){
  ESTNODERES *nres;
  const CBLIST *attrs;
  CBMAP *resheads;
  CBLIST *reqheads;
  CBDATUM *reqbody, *resbody;
  const char *kbuf, *phrase, *order, *distinct, *cbuf;
  char buf[ESTPATHBUFSIZ], *vbuf, *ptr, *pv, *ep;
  int i, rescode, err, max, plen, part, end;
  assert(node && cond && depth >= 0);
  err = FALSE;
  sprintf(buf, "%s/search", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  cblistpush(reqheads, "Content-Type: " ESTFORMTYPE, -1);
  reqbody = cbdatumopen(NULL, -1);
  if((phrase = est_cond_phrase(cond)) != NULL) cbdatumprintf(reqbody, "phrase=%?", phrase);
  if((attrs = est_cond_attrs(cond)) != NULL){
    for(i = 0; i < cblistnum(attrs); i++){
      if(cbdatumsize(reqbody) > 0) cbdatumcat(reqbody, "&", 1);
      cbdatumprintf(reqbody, "attr%d=%?", i + 1, cblistval(attrs, i, NULL));
    }
  }
  if((max = est_cond_max(cond)) >= 0){
    if(cbdatumsize(reqbody) > 0) cbdatumcat(reqbody, "&", 1);
    cbdatumprintf(reqbody, "max=%d", max);
  } else {
    if(cbdatumsize(reqbody) > 0) cbdatumcat(reqbody, "&", 1);
    cbdatumprintf(reqbody, "max=%d", INT_MAX / 2);
  }
  if((order = est_cond_order(cond)) != NULL){
    if(cbdatumsize(reqbody) > 0) cbdatumcat(reqbody, "&", 1);
    cbdatumprintf(reqbody, "order=%?", order);
  }
  if(cbdatumsize(reqbody) > 0) cbdatumcat(reqbody, "&", 1);
  cbdatumprintf(reqbody, "options=%d", est_cond_options(cond));
  cbdatumprintf(reqbody, "&auxiliary=%d", est_cond_auxiliary(cond));
  if((distinct = est_cond_distinct(cond)) != NULL)
    cbdatumprintf(reqbody, "&distinct=%?", distinct);
  cbdatumprintf(reqbody, "&depth=%d", depth);
  cbdatumprintf(reqbody, "&wwidth=%d", node->wwidth);
  cbdatumprintf(reqbody, "&hwidth=%d", node->hwidth);
  cbdatumprintf(reqbody, "&awidth=%d", node->awidth);
  cbdatumprintf(reqbody, "&skip=%d", est_cond_skip(cond));
  cbdatumprintf(reqbody, "&mask=%d", est_cond_mask(cond));
  resheads = cbmapopenex(ESTMINIBNUM);
  resbody = cbdatumopen(NULL, -1);
  if(!est_url_shuttle(buf, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, cbdatumptr(reqbody), cbdatumsize(reqbody),
                      &rescode, resheads, resbody)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  if(err){
    cbdatumclose(resbody);
    cbmapclose(resheads);
    cbdatumclose(reqbody);
    cblistclose(reqheads);
    return NULL;
  }
  ptr = cbdatumtomalloc(resbody, NULL);
  if(!(pv = strchr(ptr, '\n')) || pv <= ptr + 3){
    free(ptr);
    cbdatumclose(reqbody);
    cblistclose(reqheads);
    return NULL;
  }
  *pv = '\0';
  if(pv[-1] == '\r') pv[-1] = '\0';
  plen = strlen(ptr);
  pv++;
  ep = pv;
  nres = est_noderes_new();
  part = 0;
  end = FALSE;
  while(*ep != '\0'){
    if(*ep == *ptr && cbstrfwmatch(ep, ptr) && ep[-1] == '\n' &&
       (ep[plen] == '\r' || ep[plen] == '\n' || ep[plen] == ':')){
      *ep = '\0';
      if(part == 0){
        est_parse_search_header(nres, pv);
      } else {
        est_parse_search_body(nres, pv);
      }
      ep += plen;
      if(cbstrfwmatch(ep, ":END")){
        end = TRUE;
        break;
      }
      if(*ep == '\r') ep++;
      if(*ep == '\n') ep++;
      pv = ep;
      part++;
    } else {
      ep++;
    }
  }
  free(ptr);
  if((cbuf = cbmapget(resheads, "last-modified", -1, NULL)) != NULL){
    nres->mdate = cbstrmktime(cbuf);
  } else {
    nres->mdate = time(NULL);
  }
  cbmapclose(resheads);
  cbdatumclose(reqbody);
  cblistclose(reqheads);
  if(!end){
    est_noderes_delete(nres);
    return NULL;
  }
  return nres;
}


/* Set width of snippet in the result from a node. */
void est_node_set_snippet_width(ESTNODE *node, int wwidth, int hwidth, int awidth){
  assert(node);
  node->wwidth = wwidth;
  if(hwidth >= 0) node->hwidth = hwidth;
  if(awidth >= 0) node->awidth = awidth;
}


/* Manage a user account of a node. */
int est_node_set_user(ESTNODE *node, const char *name, int mode){
  CBLIST *reqheads;
  CBDATUM *reqbody, *resbody;
  const char *kbuf;
  char url[ESTPATHBUFSIZ], *vbuf;
  int rescode, err;
  assert(node && name);
  err = FALSE;
  sprintf(url, "%s/_set_user", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  cblistpush(reqheads, "Content-Type: " ESTFORMTYPE, -1);
  reqbody = cbdatumopen(NULL, -1);
  cbdatumprintf(reqbody, "name=%?&mode=%d", name, mode);
  resbody = cbdatumopen(NULL, -1);
  if(!est_url_shuttle(url, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, cbdatumptr(reqbody), cbdatumsize(reqbody),
                      &rescode, NULL, resbody)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  cbdatumclose(resbody);
  cbdatumclose(reqbody);
  cblistclose(reqheads);
  return err ? FALSE : TRUE;
}


/* Manage a link of a node. */
int est_node_set_link(ESTNODE *node, const char *url, const char *label, int credit){
  CBLIST *reqheads;
  CBDATUM *reqbody, *resbody;
  const char *kbuf;
  char myurl[ESTPATHBUFSIZ], *vbuf;
  int rescode, err;
  assert(node && url && label);
  err = FALSE;
  sprintf(myurl, "%s/_set_link", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  cblistpush(reqheads, "Content-Type: " ESTFORMTYPE, -1);
  reqbody = cbdatumopen(NULL, -1);
  if(credit >= 0){
    cbdatumprintf(reqbody, "url=%?&label=%?&credit=%d", url, label, credit);
  } else {
    cbdatumprintf(reqbody, "url=%?&label=%?", url, label);
  }
  resbody = cbdatumopen(NULL, -1);
  if(!est_url_shuttle(myurl, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, cbdatumptr(reqbody), cbdatumsize(reqbody),
                      &rescode, NULL, resbody)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  cbdatumclose(resbody);
  cbdatumclose(reqbody);
  cblistclose(reqheads);
  return err ? FALSE : TRUE;
}



/*************************************************************************************************
 * features for experts
 *************************************************************************************************/


/* Get the name of this host. */
const char *est_get_host_name(void){
  static char host[ESTPATHBUFSIZ];
  static int first = TRUE;
  if(!est_host_addrs) return "127.0.0.1";
  if(first){
    first = FALSE;
    if(gethostname(host, ESTPATHBUFSIZ - 1) == -1) return "127.0.0.1";
    return host;
  }
  return host;
}


/* Get the address of a host. */
char *est_get_host_addr(const char *name){
  const char *addr;
  char *buf, *pv, vbuf[64];
  int i, ost, nsiz, asiz, vsiz;
  assert(name);
  if(pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &ost) != 0) return NULL;
  if(pthread_mutex_lock(&est_host_addrs_mutex) != 0){
    pthread_setcancelstate(ost, NULL);
    return NULL;
  }
  if(!est_host_addrs){
    pthread_mutex_unlock(&est_host_addrs_mutex);
    pthread_setcancelstate(ost, NULL);
    return NULL;
  }
  nsiz = strlen(name);
  buf = NULL;
  if((addr = cbmapget(est_host_addrs, name, nsiz, &asiz)) != NULL){
    buf = cbmemdup(addr, asiz);
    if((pv = strchr(buf, '\t')) != NULL){
      *pv = '\0';
      if((int)time(NULL) - atoi(pv + 1) > ESTDNHOLDSEC){
        free(buf);
        buf = NULL;
      }
    }
    cbmapmove(est_host_addrs, name, nsiz, FALSE);
  }
  pthread_mutex_unlock(&est_host_addrs_mutex);
  pthread_setcancelstate(ost, NULL);
  if(buf){
    if(buf[0] != '\0') return buf;
    free(buf);
    return NULL;
  }
  buf = est_gethostaddrbyname(name);
  if(pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &ost) == 0){
    if(pthread_mutex_lock(&est_host_addrs_mutex) == 0){
      vsiz = sprintf(vbuf, "%s\t%d", buf ? buf : "", (int)time(NULL));
      cbmapput(est_host_addrs, name, nsiz, vbuf, vsiz, TRUE);
      if(cbmaprnum(est_host_addrs) > ESTDNHOLDNUM){
        cbmapiterinit(est_host_addrs);
        for(i = 0; i < ESTDNHOLDNUM / 4 &&
              (addr = cbmapiternext(est_host_addrs, &asiz)) != NULL; i++){
          cbmapout(est_host_addrs, addr, asiz);
        }
      }
      pthread_mutex_unlock(&est_host_addrs_mutex);
    }
    pthread_setcancelstate(ost, NULL);
  }
  return buf;
}


/* Get a server socket of an address and a port. */
int est_get_server_sock(const char *addr, int port){
  struct sockaddr_in address;
  struct linger li;
  int ost, sock, optone;
  assert(port > 0);
  if(pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &ost) != 0) return -1;
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  if(!est_inet_aton(addr ? addr : "0.0.0.0", &address.sin_addr)){
    pthread_setcancelstate(ost, NULL);
    return -1;
  }
  address.sin_port = htons(port);
  if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
    pthread_setcancelstate(ost, NULL);
    return -1;
  }
  li.l_onoff = 1;
  li.l_linger = 100;
  optone = 1;
  if(setsockopt(sock, SOL_SOCKET, SO_LINGER, (char *)&li, sizeof(li)) == -1 ||
     setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&optone, sizeof(optone)) == -1){
    est_sock_close(sock);
    pthread_setcancelstate(ost, NULL);
    return -1;
  }
  if(bind(sock, (struct sockaddr *)&address, sizeof(address)) == -1 ||
     listen(sock, SOMAXCONN) == -1){
    est_sock_close(sock);
    pthread_setcancelstate(ost, NULL);
    return -1;
  }
  pthread_setcancelstate(ost, NULL);
  return sock;
}


/* Accept a connection from a client. */
int est_accept_conn(int sock, char *abuf, int *pp){
  struct sockaddr_in address;
  socklen_t socklen;
  int clsock;
  assert(sock >= 0);
  socklen = sizeof(address);
  if((clsock = accept(sock, (struct sockaddr *)&address, &socklen)) >= 0){
    if(abuf) sprintf(abuf, "%s", inet_ntoa(address.sin_addr));
    if(pp) *pp = (int)ntohs(address.sin_port);
    return clsock;
  }
  return (errno == EINTR || errno == EAGAIN) ? 0 : -1;
}


/* Get a client socket to an address and a port. */
int est_get_client_sock(const char *addr, int port){
  struct sockaddr_in address;
  struct linger li;
  int ost, sock;
  assert(addr && port >= 0);
  if(pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &ost) != 0) return -1;
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  if(!est_inet_aton(addr, &address.sin_addr)){
    pthread_setcancelstate(ost, NULL);
    return -1;
  }
  address.sin_port = htons(port);
  if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
    pthread_setcancelstate(ost, NULL);
    return -1;
  }
  li.l_onoff = 1;
  li.l_linger = 100;
  if(setsockopt(sock, SOL_SOCKET, SO_LINGER, (char *)&li, sizeof(li)) == -1){
    est_sock_close(sock);
    pthread_setcancelstate(ost, NULL);
    return -1;
  }
  if(connect(sock, (struct sockaddr *)&address, sizeof(address)) == -1){
    est_sock_close(sock);
    pthread_setcancelstate(ost, NULL);
    return -1;
  }
  pthread_setcancelstate(ost, NULL);
  return sock;
}


/* Shutdown and close a socket. */
void est_sock_down(int sock){
  assert(sock >= 0);
  shutdown(sock, 2);
  est_sock_close(sock);
}


/* Receive all data from a socket. */
char *est_sock_recv_all(int sock, int len){
  char *buf;
  int i, bs;
  assert(sock >= 0 && len >= 0);
  buf = cbmalloc(len + 1);
  for(i = 0; i < len && (bs = recv(sock, buf + i, len - i, 0)) != 0; i += bs){
    if(bs == -1 && errno != EINTR){
      free(buf);
      return NULL;
    }
  }
  buf[i] = '\0';
  return buf;
}


/* Receive a line from a socket. */
int est_sock_recv_line(int sock, char *buf, int max){
  char *wp;
  assert(sock >= 0 && buf && max > 0);
  max--;
  wp = buf;
  while(wp < buf + max){
    switch(recv(sock, wp, 1, 0)){
    case -1:
      if(errno != EINTR){
        *wp = '\0';
        return wp - buf;
      }
      break;
    case 0:
      *wp = '\0';
      return wp - buf;
    default:
      switch(*wp){
      case '\r':
        break;
      case '\n':
        *wp = '\0';
        return wp - buf;
      default:
        wp++;
        break;
      }
      break;
    }
  }
  *wp = '\0';
  return wp - buf;
}


/* Receive void data from a socket. */
void est_sock_recv_void(int sock){
  fd_set rfds;
  struct timeval tv;
  char ibuf[ESTIOBUFSIZ];
  assert(sock >= 0);
  FD_ZERO(&rfds);
  FD_SET(sock, &rfds);
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  if(select(sock + 1, &rfds, NULL, NULL, &tv) > 0 && FD_ISSET(sock, &rfds))
    recv(sock, ibuf, ESTIOBUFSIZ, 0);
}


/* Write all data into a socket. */
void est_sock_send_all(int sock, const char *buf, int len){
  const char *rp;
  int rv, wb;
  assert(sock >= 0 && buf && len >= 0);
  rp = buf;
  rv = 0;
  do {
    wb = send(sock, rp, len, 0);
    switch(wb){
    case -1: if(errno != EINTR) return;
    case 0: break;
    default:
      rp += wb;
      len -= wb;
      rv += wb;
      break;
    }
  } while(len > 0);
}


/* Perform an interaction of a URL. */
int est_url_shuttle(const char *url, const char *pxhost, int pxport, int outsec, int limsize,
                    const char *auth, const CBLIST *reqheads, const char *reqbody, int rbsiz,
                    int *rescodep, CBMAP *resheads, CBDATUM *resbody){
  pthread_t th;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  struct timespec timeout;
  TARGSHUTTLE targ;
  int err, rv;
  void *rvp;
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);
  targ.alive = TRUE;
  targ.cond = &cond;
  targ.url = url;
  targ.pxhost = pxhost;
  targ.pxport = pxport;
  targ.limsize = limsize;
  targ.auth = auth;
  targ.reqheads = reqheads;
  targ.reqbody = reqbody;
  targ.rbsiz = rbsiz;
  targ.rescodep = rescodep;
  targ.resheads = resheads;
  targ.resbody = resbody;
  err = FALSE;
  if(outsec >= 0){
    if(pthread_mutex_lock(&mutex) == 0){
      if(pthread_create(&th, NULL, est_url_shuttle_impl, &targ) == 0){
        timeout.tv_sec = time(NULL) + outsec;
        timeout.tv_nsec = 1000 * 1000 * 500;
        rv = 0;
        while(targ.alive && rv != ETIMEDOUT){
          rv = pthread_cond_timedwait(&cond, &mutex, &timeout);
        }
        if(rv == ETIMEDOUT){
          pthread_cancel(th);
          pthread_join(th, NULL);
          err = TRUE;
        } else if(pthread_join(th, &rvp) != 0 || rvp != NULL){
          err = TRUE;
        }
      } else {
        err = TRUE;
      }
      pthread_mutex_unlock(&mutex);
    } else {
      err = TRUE;
    }
  } else {
    if(est_url_shuttle_impl(&targ) != NULL) err = TRUE;
  }
  if(pthread_mutex_destroy(&mutex) != 0) err = TRUE;
  if(pthread_cond_destroy(&cond) != 0) err = TRUE;
  return err ? FALSE : TRUE;
}


/* Initialize the document iterator of a node. */
int est_node_iter_init(ESTNODE *node){
  est_node_iter_delete(node);
  return est_node_read_list(node) != -1;
}


/* Get the next document of the document iterator of a node. */
ESTDOC *est_node_iter_next(ESTNODE *node){
  ESTDOC *doc;
  char *vbuf;
  if(!node->ilist) return NULL;
  if(cblistnum(node->ilist) < 1){
    cblistclose(node->ilist);
    node->ilist = cblistopen();
    if((est_node_read_list(node) == -1 || cblistnum(node->ilist) < 1)) return NULL;
  }
  vbuf = cblistshift(node->ilist, NULL);
  doc = *(ESTDOC **)vbuf;
  free(vbuf);
  return doc;
}


/* Add a header to a node connection object. */
void est_node_add_header(ESTNODE *node, const char *name, const char *value){
  const char *vbuf;
  int len;
  assert(node && name);
  len = strlen(name);
  if(value){
    if((vbuf = cbmapget(node->heads, name, len, NULL)) != NULL){
      cbmapputcat(node->heads, name, len, ", ", 2);
      cbmapputcat(node->heads, name, len, value, -1);
    } else {
      cbmapput(node->heads, name, len, value, -1, FALSE);
    }
  } else {
    cbmapout(node->heads, name, len);
  }
}


/* Create a node result object. */
ESTNODERES *est_noderes_new(void){
  ESTNODERES *nres;
  nres = cbmalloc(sizeof(ESTNODERES));
  nres->top = 0;
  nres->max = ESTLISTUNIT;
  nres->docs = cbmalloc(sizeof(ESTRESDOC) * nres->max);
  nres->dnum = 0;
  nres->survivors = NULL;
  nres->snum = 0;
  nres->hints = cbmapopenex(ESTMINIBNUM);
  nres->mdate = 0;
  return nres;
}


/* Add a document information to a node result object. */
void est_noderes_add_doc(ESTNODERES *nres, CBMAP *attrs, char *snippet){
  const char *uri;
  int idx;
  assert(nres && attrs && snippet);
  if(!(uri = cbmapget(attrs, ESTDATTRURI, -1, NULL))){
    free(snippet);
    cbmapclose(attrs);
    return;
  }
  if(nres->top + nres->dnum >= nres->max){
    nres->max *= 2;
    nres->docs = cbrealloc(nres->docs, nres->max * sizeof(ESTRESDOC));
  }
  idx = nres->top+nres->dnum;
  nres->docs[idx].uri = uri;
  nres->docs[idx].attrs = attrs;
  nres->docs[idx].snippet = snippet;
  nres->docs[idx].kwords = NULL;
  nres->docs[idx].similarity = -1.0;
  nres->docs[idx].children = NULL;
  nres->docs[idx].cnum = 0;
  nres->dnum++;
}


/* Remove the top of result document objects in a node result object. */
int est_noderes_shift_doc(ESTNODERES *nres, CBMAP **attrp, char **snippetp){
  assert(nres && attrp && snippetp);
  if(nres->dnum < 1) return FALSE;
  *attrp = nres->docs[nres->top].attrs;
  *snippetp = nres->docs[nres->top].snippet;
  if(nres->docs[nres->top].kwords) cbmapclose(nres->docs[nres->top].kwords);
  free(nres->docs[nres->top].children);
  nres->top++;
  nres->dnum--;
  return TRUE;
}


/* Get the modification date of a node result object. */
time_t est_noderes_mdate(ESTNODERES *nres){
  assert(nres);
  return nres->mdate;
}



/*************************************************************************************************
 * private objects
 *************************************************************************************************/


/* Get the primary address of a host name.
   `name' specifies the name of a host.
   The return value is the address of a host or `NULL' if failure.   Because the region of the
   return value is allocated with the `malloc' call, it should be released with the `free' call
   if it is no longer in use. */
static char *est_gethostaddrbyname(const char *name){
#if defined(_SYS_LINUX_)
  struct hostent infobuf, *info;
  char buf[ESTIOBUFSIZ], *addr;
  int ecode;
  if(gethostbyname_r(name, &infobuf, buf, ESTIOBUFSIZ, &info, &ecode) == 0 && info){
    addr = cbmemdup(inet_ntoa(*(struct in_addr *)info->h_addr_list[0]), -1);
  } else {
    addr = NULL;
  }
  return addr;
#else
  static pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
  struct hostent *info;
  char *addr;
  int ost;
  assert(name);
  if(pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &ost) != 0) return NULL;
  if(pthread_mutex_lock(&mymutex) != 0){
    pthread_setcancelstate(ost, NULL);
    return NULL;
  }
  if((info = gethostbyname(name)) != NULL && info->h_addr_list[0]){
    addr = cbmemdup(inet_ntoa(*(struct in_addr *)info->h_addr_list[0]), -1);
  } else {
    addr = NULL;
  }
  pthread_mutex_unlock(&mymutex);
  pthread_setcancelstate(ost, NULL);
  return addr;
#endif
}


/* Close a socket.
   `sock' specifies a socket.
   The return value is 0 if success, else it is -1. */
static int est_sock_close(int sock){
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  assert(sock >= 0);
  return closesocket(sock);
#else
  assert(sock >= 0);
  return close(sock);
#endif
}


/* Convert a host address to network binary data.
   `cp' specifies a host address.
   `inp' specifies the pointer to an structure into which the result is to be stored.
   The return value is true if success, else it is false. */
static int est_inet_aton(const char *cp, struct in_addr *inp){
#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)
  in_addr_t in;
  assert(cp && inp);
  if((in = inet_addr(cp)) == INADDR_NONE){
    if(!strcmp(cp, "255.255.255.255")){
      inp->s_addr = in;
      return TRUE;
    }
    return FALSE;
  }
  inp->s_addr = in;
  return TRUE;
#else
  assert(cp && inp);
  return inet_aton(cp, inp);
#endif
}


/* Perform the communication which can be canceled.
   `targ' specifies the pointer to a closure arguments.
   The return value is `NULL' if sucessful or non `NULL' value on error. */
static void *est_url_shuttle_impl(void *targ){
  const CBLIST *reqheads;
  CBMAP *resheads, *elems;
  CBDATUM *resbody, *datum;
  const char *url, *pxhost, *auth, *reqbody, *tmp, *scheme, *host, *path, *query, *rp;
  char *addr, *enc, iobuf[ESTIOBUFSIZ], name[ESTIOBUFSIZ], *pv, *zbuf;
  int i, pxport, limsize, rbsiz, *rescodep, port, sock, *sp, size, nsiz, defl, zsiz;
  assert(targ);
  url = ((TARGSHUTTLE *)targ)->url;
  pxhost = ((TARGSHUTTLE *)targ)->pxhost;
  pxport = ((TARGSHUTTLE *)targ)->pxport;
  limsize = ((TARGSHUTTLE *)targ)->limsize;
  auth = ((TARGSHUTTLE *)targ)->auth;
  reqheads = ((TARGSHUTTLE *)targ)->reqheads;
  reqbody = ((TARGSHUTTLE *)targ)->reqbody;
  rbsiz = ((TARGSHUTTLE *)targ)->rbsiz;
  rescodep = ((TARGSHUTTLE *)targ)->rescodep;
  resheads = ((TARGSHUTTLE *)targ)->resheads;
  resbody = ((TARGSHUTTLE *)targ)->resbody;
  elems = cburlbreak(url);
  pthread_cleanup_push((void (*)(void *))cbmapclose, elems);
  scheme = cbmapget(elems, "scheme", -1, NULL);
  host = cbmapget(elems, "host", -1, NULL);
  port = (tmp = cbmapget(elems, "port", -1, NULL)) ? atoi(tmp) : 80;
  if(!auth) auth = cbmapget(elems, "authority", -1, NULL);
  if(!(path = cbmapget(elems, "path", -1, NULL))) path = "/";
  if(!(query = cbmapget(elems, "query", -1, NULL))) query = "";
  if(!scheme || cbstricmp(scheme, "http") || !host || port < 1 ||
     !(addr = est_get_host_addr(pxhost ? pxhost : host))){
    ((TARGSHUTTLE *)targ)->alive = FALSE;
    pthread_cond_signal(((TARGSHUTTLE *)targ)->cond);
    cbmapclose(elems);
    return "error";
  }
  pthread_cleanup_push((void (*)(void *))free, addr);
  if((sock = est_get_client_sock(addr, pxhost ? pxport : port)) == -1){
    ((TARGSHUTTLE *)targ)->alive = FALSE;
    pthread_cond_signal(((TARGSHUTTLE *)targ)->cond);
    free(addr);
    cbmapclose(elems);
    return "error";
  }
  sp = cbmalloc(sizeof(int));
  *sp = sock;
  pthread_cleanup_push((void (*)(void *))est_sockpt_down, sp);
  datum = cbdatumopen(NULL, -1);
  pthread_cleanup_push((void (*)(void *))cbdatumclose, datum);
  if(pxhost){
    cbdatumprintf(datum, "%s %s HTTP/1.0\r\n", reqbody ? "POST" : "GET", url);
  } else if(reqbody){
    cbdatumprintf(datum, "POST %s HTTP/1.0\r\n", path);
  } else if(query[0] != 0){
    cbdatumprintf(datum, "GET %s?%s HTTP/1.0\r\n", path, query);
  } else {
    cbdatumprintf(datum, "GET %s HTTP/1.0\r\n", path);
  }
  cbdatumprintf(datum, "Host: %s:%d\r\n", host, port);
  cbdatumprintf(datum, "Connection: close\r\n", path);
  cbdatumprintf(datum, "User-Agent: %s/%s\r\n", ESTAGENTNAME, est_version);
  if(auth){
    enc = cbbaseencode(auth, -1);
    cbdatumprintf(datum, "Authorization: Basic %s\r\n", enc);
    free(enc);
  }
  if(ESTUSEZLIB) cbdatumprintf(datum, "Accept-Encoding: deflate\r\n");
  if(reqbody) cbdatumprintf(datum, "Content-Length: %d\r\n", rbsiz);
  if(reqheads){
    for(i = 0; i < cblistnum(reqheads); i++){
      rp = cblistval(reqheads, i, &size);
      cbdatumprintf(datum, rp, size);
      cbdatumprintf(datum, "\r\n", 2);
    }
  }
  cbdatumprintf(datum, "\r\n");
  est_sock_send_all(sock, cbdatumptr(datum), cbdatumsize(datum));
  if(reqbody) est_sock_send_all(sock, reqbody, rbsiz);
  if((size = est_sock_recv_line(sock, iobuf, ESTIOBUFSIZ - 1)) < 1 ||
     !cbstrfwmatch(iobuf, "HTTP/") || !(rp = strchr(iobuf, ' '))){
    ((TARGSHUTTLE *)targ)->alive = FALSE;
    pthread_cond_signal(((TARGSHUTTLE *)targ)->cond);
    cbdatumclose(datum);
    est_sockpt_down(sp);
    free(addr);
    cbmapclose(elems);
    return "error";
  }
  rp++;
  if(rescodep) *rescodep = atoi(rp);
  if(resheads) cbmapput(resheads, "", 0, iobuf, size, TRUE);
  name[0] = '\0';
  nsiz = 0;
  defl = FALSE;
  while((size = est_sock_recv_line(sock, iobuf, ESTIOBUFSIZ - 1)) > 0){
    if(resheads){
      if(iobuf[0] == ' ' || iobuf[0] == '\t'){
        if(name[0] != '\0'){
          iobuf[0] = ' ';
          cbmapputcat(resheads, name, nsiz, iobuf, size);
        }
      } else if((rp = strchr(iobuf, ':')) > iobuf){
        nsiz = rp - iobuf;
        memcpy(name, iobuf, nsiz);
        name[nsiz] = '\0';
        for(pv = name; *pv != '\0'; pv++){
          if(*pv >= 'A'&& *pv <= 'Z') *pv = *pv + ('a' - 'A');
        }
        rp++;
        if(*rp == ' ' || *rp == '\t') rp++;
        if(cbmapget(resheads, name, nsiz, NULL)){
          cbmapputcat(resheads, name, nsiz, ", ", 2);
          cbmapputcat(resheads, name, nsiz, pv, -1);
        } else {
          cbmapput(resheads, name, nsiz, rp, -1, TRUE);
        }
      }
    }
    if(cbstrfwimatch(iobuf, "content-encoding:") && strstr(iobuf + 17, "deflate")) defl = TRUE;
  }
  if(limsize < 0) limsize = INT_MAX;
  while((size = recv(sock, iobuf, ESTIOBUFSIZ, 0)) > 0){
    if(resbody && cbdatumsize(resbody) <= limsize) cbdatumcat(resbody, iobuf, size);
  }
  if(defl && (zbuf = est_inflate(cbdatumptr(resbody), cbdatumsize(resbody), &zsiz, 0)) != NULL){
    cbdatumsetbuf(resbody, zbuf, zsiz);
  }
  pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);
  ((TARGSHUTTLE *)targ)->alive = FALSE;
  pthread_cond_signal(((TARGSHUTTLE *)targ)->cond);
  return NULL;
}


/* Release the socket of a pointer.
   `sp' specifies the pointer to a variable of a file descriptor. */
static void est_sockpt_down(void *sp){
  est_sock_down(*(int *)sp);
  free(sp);
}


/* Set meta informations of a node.
   `node' specifies a node connection object.
   The return value is true if success, else it is false. */
static int est_node_set_info(ESTNODE *node){
  CBLIST *reqheads, *elems;
  CBDATUM *resbody;
  const char *kbuf, *ptr, *pv, *tv;
  char url[ESTPATHBUFSIZ], *vbuf;
  int rescode, err, tc;
  assert(node);
  err = FALSE;
  sprintf(url, "%s/inform", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  node->dnum = -1;
  node->wnum = -1;
  node->size = -1.0;
  if(node->admins){
    cblistclose(node->admins);
    node->admins = NULL;
  }
  if(node->users){
    cblistclose(node->users);
    node->users = NULL;
  }
  if(node->links){
    cblistclose(node->links);
    node->links = NULL;
  }
  resbody = cbdatumopen(NULL, -1);
  if(!est_url_shuttle(url, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, NULL, -1, &rescode, NULL, resbody)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  if(!err){
    ptr = cbdatumptr(resbody);
    if((pv = strchr(ptr, '\n')) != NULL){
      elems = cbsplit(ptr, pv - ptr, "\t");
      if(cblistnum(elems) == 5){
        if(!node->name) node->name = cbmemdup(cblistval(elems, 0, NULL), -1);
        if(!node->label) node->label = cbmemdup(cblistval(elems, 1, NULL), -1);
        node->dnum = atoi(cblistval(elems, 2, NULL));
        node->wnum = atoi(cblistval(elems, 3, NULL));
        node->size = strtod(cblistval(elems, 4, NULL), NULL);
        if(node->dnum < 0){
          node->dnum = -1;
          err = TRUE;
        }
        if(node->wnum < 0){
          node->wnum = -1;
          err = TRUE;
        }
        if(node->size < 0.0){
          node->size = -1.0;
          err = TRUE;
        }
        ptr = pv;
        ptr++;
        if(*ptr == '\n') ptr++;
        node->admins = cblistopen();
        while((pv = strchr(ptr, '\n')) != NULL){
          if(ptr == pv){
            ptr = pv + 1;
            break;
          }
          cblistpush(node->admins, ptr, pv - ptr);
          ptr = pv + 1;
        }
        if(*pv == '\n') pv++;
        node->users = cblistopen();
        while((pv = strchr(ptr, '\n')) != NULL){
          if(ptr == pv){
            ptr = pv + 1;
            break;
          }
          cblistpush(node->users, ptr, pv - ptr);
          ptr = pv + 1;
        }
        if(*pv == '\n') pv++;
        node->links = cblistopen();
        while((pv = strchr(ptr, '\n')) != NULL){
          if(ptr == pv){
            ptr = pv + 1;
            break;
          }
          tc = 0;
          tv = ptr;
          while(tv < pv){
            if(*tv == '\t') tc++;
            tv++;
          }
          if(tc == 2) cblistpush(node->links, ptr, pv - ptr);
          ptr = pv + 1;
        }
      } else {
        err = TRUE;
      }
      cblistclose(elems);
    } else {
      err = TRUE;
    }
  }
  cbdatumclose(resbody);
  cblistclose(reqheads);
  return err ? FALSE : TRUE;
}


/* Delete the document iterator of a node.
   `node' specifies a node connection object. */
static void est_node_iter_delete(ESTNODE *node){
  int i;
  assert(node);
  if(node->ilist){
    for(i = 0; i < cblistnum(node->ilist); i++){
      est_doc_delete(*(ESTDOC **)cblistval(node->ilist, i, NULL));
    }
    cblistclose(node->ilist);
    node->ilist = NULL;
  }
  free(node->iprev);
  node->iprev = NULL;
}


/* Read charge documents into the document iterator of a node.
   `node' specifies a node connection object.
   The return value is the number of charged documents or -1 on error. */
static int est_node_read_list(ESTNODE *node){
  ESTDOC *doc;
  CBLIST *reqheads, *lines, *fields;
  CBDATUM *reqbody, *resbody;
  const char *kbuf, *id, *uri;
  char url[ESTPATHBUFSIZ], *vbuf;
  int i, rescode, err, ksiz, dnum;
  assert(node);
  err = FALSE;
  sprintf(url, "%s/list", node->url);
  reqheads = cblistopen();
  if(cbmaprnum(node->heads) > 0){
    cbmapiterinit(node->heads);
    while((kbuf = cbmapiternext(node->heads, NULL)) != NULL){
      vbuf = cbsprintf("%s: %s", kbuf, cbmapiterval(kbuf, NULL));
      cblistpush(reqheads, vbuf, -1);
      free(vbuf);
    }
  }
  cblistpush(reqheads, "Content-Type: " ESTFORMTYPE, -1);
  reqbody = cbdatumopen(NULL, -1);
  cbdatumprintf(reqbody, "max=%d", ESTITERUNIT);
  if(node->iprev) cbdatumprintf(reqbody, "&prev=%?", node->iprev);
  resbody = cbdatumopen(NULL, -1);
  if(!est_url_shuttle(url, node->pxhost, node->pxport, node->timeout, ESTRESLIMSIZE, node->auth,
                      reqheads, cbdatumptr(reqbody), cbdatumsize(reqbody),
                      &rescode, NULL, resbody)){
    node->status = -1;
    err = TRUE;
  }
  if(!err){
    node->status = rescode;
    if(rescode != 200) err = TRUE;
  }
  dnum = 0;
  if(!err){
    if(!node->ilist) node->ilist = cblistopen();
    lines = cbsplit(cbdatumptr(resbody), cbdatumsize(resbody), "\r\n");
    for(i = 0; i < cblistnum(lines); i++){
      kbuf = cblistval(lines, i, &ksiz);
      if(ksiz < 1) continue;
      fields = cbsplit(kbuf, ksiz, "\t");
      if(cblistnum(fields) == 14){
        id = cblistval(fields, 0, NULL);
        uri = cblistval(fields, 1, NULL);
        doc = est_doc_new();
        est_doc_set_id(doc, atoi(id));
        est_doc_add_attr(doc, ESTDATTRID, id);
        est_doc_add_attr(doc, ESTDATTRURI, uri);
        est_doc_add_attr(doc, ESTDATTRDIGEST, cblistval(fields, 2, NULL));
        est_doc_add_attr(doc, ESTDATTRCDATE, cblistval(fields, 3, NULL));
        est_doc_add_attr(doc, ESTDATTRMDATE, cblistval(fields, 4, NULL));
        est_doc_add_attr(doc, ESTDATTRADATE, cblistval(fields, 5, NULL));
        est_doc_add_attr(doc, ESTDATTRTITLE, cblistval(fields, 6, NULL));
        est_doc_add_attr(doc, ESTDATTRAUTHOR, cblistval(fields, 7, NULL));
        est_doc_add_attr(doc, ESTDATTRTYPE, cblistval(fields, 8, NULL));
        est_doc_add_attr(doc, ESTDATTRLANG, cblistval(fields, 9, NULL));
        est_doc_add_attr(doc, ESTDATTRGENRE, cblistval(fields, 10, NULL));
        est_doc_add_attr(doc, ESTDATTRSIZE, cblistval(fields, 11, NULL));
        est_doc_add_attr(doc, ESTDATTRWEIGHT, cblistval(fields, 12, NULL));
        est_doc_add_attr(doc, ESTDATTRMISC, cblistval(fields, 13, NULL));
        cblistpush(node->ilist, (char *)&doc, sizeof(ESTDOC *));
        dnum++;
        free(node->iprev);
        node->iprev = cbmemdup(uri, -1);
      }
      cblistclose(fields);
    }
    cblistclose(lines);
  }
  cbdatumclose(resbody);
  cbdatumclose(reqbody);
  cblistclose(reqheads);
  return err ? -1 : dnum;
}


/* Parse the header of a result data.
   `nres' specifies a node result object.
   `str' specifies the header of a result data. */
static void est_parse_search_header(ESTNODERES *nres, const char *str){
  CBLIST *lines;
  const char *line, *pv;
  int i;
  assert(nres && str);
  lines = cbsplit(str, -1, "\r\n");
  for(i = 0; i < cblistnum(lines); i++){
    line = cblistval(lines, i, NULL);
    if(!(pv = strchr(line, '\t')) || pv == line || pv[1] == '\0') continue;
    cbmapput(nres->hints, line, pv - line, pv + 1, -1, FALSE);
  }
  cblistclose(lines);
}


/* Parse a body part of a result data.
   `nres' specifies a node result object.
   `str' specifies a body part of a result data. */
static void est_parse_search_body(ESTNODERES *nres, char *str){
  CBMAP *attrs;
  char *pv, *ep, *mp;
  pv = str;
  attrs = cbmapopenex(ESTMINIBNUM);
  while(TRUE){
    if(!(ep = strchr(pv, '\n')) || ep == pv) break;
    *ep = '\0';
    cbstrtrim(pv);
    if(*pv == '\0') break;
    if(*pv == '%'){
      if(cbstrfwmatch(pv, ESTDCNTLVECTOR) && (pv = strchr(pv, '\t')) != NULL)
        cbmapput(attrs, ESTDCNTLVECTOR, -1, pv + 1, -1, FALSE);
    } else if((mp = strchr(pv, '=')) != NULL){
      *mp = '\0';
      cbmapput(attrs, pv, -1, mp + 1, -1, TRUE);
    }
    pv = ep + 1;
  }
  while(*pv == '\r' || *pv == '\n'){
    pv++;
  }
  est_noderes_add_doc(nres, attrs, cbmemdup(pv, -1));
}



/* END OF FILE */
