/*************************************************************************************************
 * The node API of Hyper Estraier
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


#ifndef _ESTNODE_H                       /* duplication check */
#define _ESTNODE_H

#if defined(__cplusplus)                 /* export for C++ */
extern "C" {
#endif


#include <estraier.h>
#include <estmtdb.h>
#include <cabin.h>
#include <pthread.h>
#include <stdlib.h>



/*************************************************************************************************
 * API for the network environment
 *************************************************************************************************/


/* Initialize the networking environment.
   The return value is true if success, else it is false.
   As it is allowable to call this function multiple times, it is needed to call the function
   `est_free_net_env' at the same frequency. */
int est_init_net_env(void);


/* Free the networking environment. */
void est_free_net_env(void);



/*************************************************************************************************
 * API for search result of node
 *************************************************************************************************/


typedef struct _ESTRESDOC {              /* type of structure for a document in result */
  const char *uri;                       /* URI */
  CBMAP *attrs;                          /* attributes */
  char *snippet;                         /* snippet */
  CBMAP *kwords;                         /* keywords */
  double similarity;                     /* similarity to the parent document */
  struct _ESTRESDOC **children;          /* child documents */
  int cnum;                              /* number of child documents */
} ESTRESDOC;

typedef struct {                         /* type of structure for search result */
  ESTRESDOC *docs;                       /* array of documents */
  int top;                               /* offset number of the top element */
  int max;                               /* maximum number of allcated elements */
  int dnum;                              /* number of effective elements */
  ESTRESDOC **survivors;                 /* pointers to surviving documents */
  int snum;                              /* number of surviving elements */
  CBMAP *hints;                          /* map object for hints */
  time_t mdate;                          /* modification date */
} ESTNODERES;


/* Delete a node result object.
   `nres' specifies a node result object. */
void est_noderes_delete(ESTNODERES *nres);


/* Get a map object for hints of a node result object.
   `nres' specifies a node result object.
   The return value is a map object for hints.  Keys of the map are "VERSION", "NODE", "HIT",
   "HINT#n", "DOCNUM", "WORDNUM", "TIME", "TIME#n", "LINK#n", and "VIEW".  The life duration of
   the returned object is synchronous with the one of the node result object. */
CBMAP *est_noderes_hints(ESTNODERES *nres);


/* Eclipse similar documents of a node result object.
   `nres' specifies a node result object.
   `num' specifies the number of documents to be shown.  If it is not more than 0, eclipse is
   undone.
   `limit' specifies the lower limit of similarity for documents to be eclipsed.  Similarity is
   between 0.0 and 1.0. */
void est_noderes_eclipse(ESTNODERES *nres, int num, double limit);


/* Get the number of documents in a node result object.
   `nres' specifies a node result object.
   The return value is the number of documents in a node result object. */
int est_noderes_doc_num(ESTNODERES *nres);


/* Refer a result document object in a node result object.
   `nres' specifies a node result object.
   `index' specifies the index of a document.
   The return value is a result document object or `NULL' if `index' is equal to or more than
   the number of documents.  The life duration of the returned object is synchronous with the
   one of the node result object. */
ESTRESDOC *est_noderes_get_doc(ESTNODERES *nres, int index);


/* Get the URI of a result document object.
   `doc' specifies a result document object.
   The return value is the URI of the result document object.  The life duration of the returned
   string is synchronous with the one of the result document object. */
const char *est_resdoc_uri(ESTRESDOC *rdoc);


/* Get a list of attribute names of a result document object.
   `rdoc' specifies a result document object.
   The return value is a new list object of attribute names of the result document object.
   Because the object of the return value is opened with the function `cblistopen', it should be
   closed with the function `cblistclose' if it is no longer in use. */
CBLIST *est_resdoc_attr_names(ESTRESDOC *rdoc);


/* Get the value of an attribute of a result document object.
   `rdoc' specifies a result document object.
   `name' specifies the name of an attribute.
   The return value is the value of the attribute or `NULL' if it does not exist.  The life
   duration of the returned string is synchronous with the one of the result document object. */
const char *est_resdoc_attr(ESTRESDOC *rdoc, const char *name);


/* Get the snippet of a result document object.
   `rdoc' specifies a result document object.
   The return value is a string of the snippet of the result document object.  There are tab
   separated values.  Each line is a string to be shown.  Though most lines have only one field,
   some lines have two fields.  If the second field exists, the first field is to be shown with
   highlighted, and the second field means its normalized form.  The life duration of the
   returned string is synchronous with the one of the result document object. */
const char *est_resdoc_snippet(ESTRESDOC *rdoc);


/* Get keywords of a result document object.
   `rdoc' specifies a result document object.
   The return value is a string of serialized keywords of the result document object.  There are
   tab separated values.  Keywords and their scores come alternately.  The life duration of the
   returned string is synchronous with the one of the result document object. */
const char *est_resdoc_keywords(ESTRESDOC *rdoc);


/* Get an array of documents eclipsed by a result document object.
   `rdoc' specifies a result document object.
   `np' specifies the pointer to a variable to which the number of elements of the return value
   is assigned.
   The return value is an array of eclipsed result document objects.  The life duration of the
   returned array and its elements is synchronous with the one of the result document object. */
ESTRESDOC **est_resdoc_shadows(ESTRESDOC *rdoc, int *np);


/* Get similarity of an eclipsed result document object.
   `rdoc' specifies a result document object.
   The return value is similarity of the result document object to the front document or -1.0
   if it is not eclipsed. */
double est_resdoc_similarity(ESTRESDOC *rdoc);



/*************************************************************************************************
 * API for node
 *************************************************************************************************/


typedef struct {                         /* type of structure for a node object */
  char *url;                             /* URL */
  char *pxhost;                          /* host name of the proxy */
  int pxport;                            /* port number of the proxy */
  int timeout;                           /* timeout in seconds */
  char *auth;                            /* authentication information */
  char *name;                            /* name */
  char *label;                           /* label */
  int dnum;                              /* number of documents */
  int wnum;                              /* number of words */
  double size;                           /* size of the database */
  CBLIST *admins;                        /* list of names of administrators */
  CBLIST *users;                         /* list of names of users */
  CBLIST *links;                         /* list of expressions of linking nodes */
  int wwidth;                            /* whole width of a snippet */
  int hwidth;                            /* head width of a snippet */
  int awidth;                            /* around width of a snippet */
  CBLIST *ilist;                         /* list of the document iterator */
  char *iprev;                           /* previous URI of the document iterator */
  int status;                            /* last status code */
  CBMAP *heads;                          /* extention headers */
} ESTNODE;


/* Create a node connection object.
   `url' specifies the URL of a node.
   The return value is a node connection object. */
ESTNODE *est_node_new(const char *url);


/* Destroy a node connection object.
   `node' specifies a node connection object. */
void est_node_delete(ESTNODE *node);


/* Set the proxy information of a node connection object.
   `node' specifies a node connection object.
   `host' specifies the host name of a proxy server.
   `port' specifies the port number of the proxy server. */
void est_node_set_proxy(ESTNODE *node, const char *host, int port);


/* Set timeout of a connection.
   `node' specifies a node connection object.
   `sec' specifies timeout of the connection in seconds. */
void est_node_set_timeout(ESTNODE *node, int sec);


/* Set the authentication information of a node connection object.
   `node' specifies a node connection object.
   `name' specifies the name of authentication.
   `passwd' specifies the password of the authentication. */
void est_node_set_auth(ESTNODE *node, const char *name, const char *passwd);


/* Get the status code of the last request of a node.
   `node' specifies a node connection object.
   The return value is the status code of the last request of the node.  -1 means failure of
   connection. */
int est_node_status(ESTNODE *node);


/* Synchronize updating contents of the database of a node.
   `node' specifies a node connection object.
   The return value is true if success, else it is false. */
int est_node_sync(ESTNODE *node);


/* Optimize the database of a node.
   `node' specifies a node connection object.
   The return value is true if success, else it is false. */
int est_node_optimize(ESTNODE *node);


/* Add a document to a node.
   `node' specifies a node connection object.
   `doc' specifies a document object.  The document object should have the URI attribute.
   The return value is true if success, else it is false.
   If the URI attribute is same with an existing document in the node, the existing one is
   deleted. */
int est_node_put_doc(ESTNODE *node, ESTDOC *doc);


/* Remove a document from a node.
   `node' specifies a node connection object.
   `id' specifies the ID number of a registered document.
   The return value is true if success, else it is false. */
int est_node_out_doc(ESTNODE *node, int id);


/* Remove a document specified by URI from a node.
   `node' specifies a node connection object.
   `uri' specifies the URI of a registered document.
   The return value is true if success, else it is false. */
int est_node_out_doc_by_uri(ESTNODE *node, const char *uri);


/* Edit attributes of a document in a node.
   `node' specifies a node connection object.
   `doc' specifies a document object.
   The return value is true if success, else it is false.
   Neither the ID nor the URI can not be changed. */
int est_node_edit_doc(ESTNODE *node, ESTDOC *doc);


/* Retrieve a document in a node.
   `node' specifies a node connection object.
   `id' specifies the ID number of a registered document.
   The return value is a document object.  It should be deleted with `est_doc_delete' if it is
   no longer in use.  On error, `NULL' is returned. */
ESTDOC *est_node_get_doc(ESTNODE *node, int id);


/* Retrieve a document specified by URI in a node.
   `node' specifies a node connection object.
   `uri' specifies the URI of a registered document.
   The return value is a document object.  It should be deleted with `est_doc_delete' if it is
   no longer in use.  On error, `NULL' is returned. */
ESTDOC *est_node_get_doc_by_uri(ESTNODE *node, const char *uri);


/* Retrieve the value of an attribute of a document in a node.
   `node' specifies a node connection object.
   `id' specifies the ID number of a registered document.
   `name' specifies the name of an attribute.
   The return value is the value of the attribute or `NULL' if it does not exist.  Because the
   region of the return value is allocated with the `malloc' call, it should be released with
   the `free' call if it is no longer in use. */
char *est_node_get_doc_attr(ESTNODE *node, int id, const char *name);


/* Retrieve the value of an attribute of a document specified by URI in a node.
   `node' specifies a node connection object.
   `uri' specifies the URI of a registered document.
   `name' specifies the name of an attribute.
   The return value is the value of the attribute or `NULL' if it does not exist.  Because the
   region of the return value is allocated with the `malloc' call, it should be released with
   the `free' call if it is no longer in use. */
char *est_node_get_doc_attr_by_uri(ESTNODE *node, const char *uri, const char *name);


/* Extract keywords of a document.
   `node' specifies a node connection object.
   `id' specifies the ID number of a registered document.
   The return value is a new map object of keywords and their scores in decimal string or `NULL'
   on error.  Because the object of the return value is opened with the function `cbmapopen', it
   should be closed with the function `cbmapclose' if it is no longer in use. */
CBMAP *est_node_etch_doc(ESTNODE *node, int id);


/* Extract keywords of a document specified by URI in a node.
   `node' specifies a node connection object.
   `uri' specifies the URI of a registered document.
   The return value is a new map object of keywords and their scores in decimal string or `NULL'
   on error.  Because the object of the return value is opened with the function `cbmapopen', it
   should be closed with the function `cbmapclose' if it is no longer in use. */
CBMAP *est_node_etch_doc_by_uri(ESTNODE *node, const char *uri);


/* Get the ID of a document spacified by URI.
   `node' specifies a node connection object.
   `uri' specifies the URI of a registered document.
   The return value is the ID of the document.  On error, -1 is returned. */
int est_node_uri_to_id(ESTNODE *node, const char *uri);


/* Get the name of a node.
   `node' specifies a node connection object.
   The return value is the name of the node.  On error, `NULL' is returned.  The life duration
   of the returned string is synchronous with the one of the node object. */
const char *est_node_name(ESTNODE *node);


/* Get the label of a node.
   `node' specifies a node connection object.
   The return value is the label of the node.  On error, `NULL' is returned.  The life duration
   of the returned string is synchronous with the one of the node object. */
const char *est_node_label(ESTNODE *node);


/* Get the number of documents in a node.
   `node' specifies a node connection object.
   The return value is the number of documents in the node.  On error, -1 is returned. */
int est_node_doc_num(ESTNODE *node);


/* Get the number of unique words in a node.
   `node' specifies a node connection object.
   The return value is the number of unique words in the node.  On error, -1 is returned. */
int est_node_word_num(ESTNODE *node);


/* Get the size of the datbase of a node.
   `node' specifies a node connection object.
   The return value is the size of the datbase of the node.  On error, -1.0 is returned. */
double est_node_size(ESTNODE *node);


/* Get the usage ratio of the cache of a node.
   `node' specifies a node connection object.
   The return value is the usage ratio of the cache of the node.  On error, -1.0 is returned. */
double est_node_cache_usage(ESTNODE *node);


/* Get a list of names of administrators of a node.
   `node' specifies a node connection object.
   The return value is a list object of names of administrators.  On error, `NULL' is returned.
   The life duration of the returned object is synchronous with the one of the node object. */
const CBLIST *est_node_admins(ESTNODE *node);


/* Get a list of names of users of a node.
   `node' specifies a node connection object.
   The return value is a list object of names of users.  On error, `NULL' is returned.  The life
   duration of the returned object is synchronous with the one of the node object. */
const CBLIST *est_node_users(ESTNODE *node);


/* Get a list of expressions of links of a node.
   `node' specifies a node connection object.
   The return value is a list object of expressions of links.  Each element is a TSV string and
   has three fields of the URL, the label, and the score.  On error, `NULL' is returned.  The
   life duration of the returned object is synchronous with the one of the node object. */
const CBLIST *est_node_links(ESTNODE *node);


/* Search a node for documents corresponding a condition.
   `node' specifies a node connection object.
   `cond' specifies a condition object.
   `depth' specifies the depth of meta search.
   The return value is a node result object.  It should be deleted with `est_noderes_delete' if
   it is no longer in use.  On error, `NULL' is returned. */
ESTNODERES *est_node_search(ESTNODE *node, ESTCOND *cond, int depth);


/* Set width of snippet in the result from a node.
   `node' specifies a node connection object.
   `wwidth' specifies whole width of a snippet.  By default, it is 480.  If it is 0, no snippet
   is sent. If it is negative, whole body text is sent instead of snippet.
   `hwidth' specifies width of strings picked up from the beginning of the text.  By default, it
   is 96.  If it is negative 0, the current setting is not changed.
   `awidth' specifies width of strings picked up around each highlighted word. By default, it is
   96.  If it is negative, the current setting is not changed. */
void est_node_set_snippet_width(ESTNODE *node, int wwidth, int hwidth, int awidth);


/* Manage a user account of a node.
   `node' specifies a node connection object.
   `name' specifies the name of a user.
   `mode' specifies the operation mode.  0 means to delete the account.  1 means to set the
   account as an administrator.  2 means to set the account as a guest.
   The return value is true if success, else it is false. */
int est_node_set_user(ESTNODE *node, const char *name, int mode);


/* Manage a link of a node.
   `node' specifies a node connection object.
   `url' specifies the URL of the target node of a link.
   `label' specifies the label of the link.
   `credit' specifies the credit of the link.  If it is negative, the link is removed.
   The return value is true if success, else it is false. */
int est_node_set_link(ESTNODE *node, const char *url, const char *label, int credit);



/*************************************************************************************************
 * features for experts
 *************************************************************************************************/


#define ESTAGENTNAME   "HyperEstraier"   /* name of the user agent */
#define ESTFORMTYPE    "application/x-www-form-urlencoded"  /* media type of docuemnt draft */
#define ESTINFORMTYPE  "text/x-estraier-nodeinfo"  /* media type of node information */
#define ESTSEARCHTYPE  "text/x-estraier-search"    /* media type of search result */
#define ESTLISTTYPE    "text/x-estraier-list"      /* media type of list result */
#define ESTDRAFTTYPE   "text/x-estraier-draft"     /* media type of docuemnt draft */
#define ESTHTHVIA      "X-Estraier-Via"  /* header to escape from looping route */


/* Get the name of this host.
   The return value is the name of this host. */
const char *est_get_host_name(void);


/* Get the address of a host.
   `name' specifies the name of a host.
   The return value is the address of a host or `NULL' if failure.   Because the region of the
   return value is allocated with the `malloc' call, it should be released with the `free' call
   if it is no longer in use. */
char *est_get_host_addr(const char *name);


/* Get a server socket of an address and a port.
   `addr' specifies an address of a host.  If it is `NULL', every network address is binded.
   `port' specifies a port number.
   The return value is the socket of the address and the port or -1 if failure. */
int est_get_server_sock(const char *addr, int port);


/* Accept a connection from a client.
   `sock' specifies a server socket.
   `abuf' specifies a buffer into which the address of a connected client is written.  The size of
   the buffer should be more than 32.  If it is `NULL', it is not used.
   `pp' specifies the pointer to a variable to which the port of the client is assigned.  If it
   is `NULL', it is not used.
   The return value is a socket connected to the client, or 0 if intterupted,  or -1 if failure.
   The thread blocks until the connection is established. */
int est_accept_conn(int sock, char *abuf, int *pp);


/* Get a client socket to an address and a port.
   `addr' specifies an address of a host.
   `port' specifies a port number.
   The return value is the socket to the address and the port or -1 if failure. */
int est_get_client_sock(const char *addr, int port);


/* Shutdown and close a socket.
   `sock' specifies a socket. */
void est_sock_down(int sock);


/* Receive all data from a socket.
   `sock' specifies a socket.
   `len' specifies the length of data to be read.
   The return value is the pointer to the region of an allocated region containing the received
   data.
   Because an additional zero code is appended at the end of the region of the return value, the
   return value can be treated as a character string.  Because the region of the return value is
   allocated with the `malloc' call, it should be released with the `free' call if it is no
   longer in use. */
char *est_sock_recv_all(int sock, int len);


/* Receive a line from a socket.
   `sock' specifies a socket.
   `buf' specifies a buffer to store read data.
   `max' specifies the maximum length to read.  It should be more than 0.
   The return value is the size of received data.
   Because an additional zero code is appended at the end of the region of the buffer, it can be
   treated as a character string. */
int est_sock_recv_line(int sock, char *buf, int max);


/* Receive void data from a socket.
   `sock' specifies a socket. */
void est_sock_recv_void(int sock);


/* Send all data into a socket.
   `sock' specifies a socket.
   `buf' specifies a buffer of data to write.
   `len' specifies the length of the data. */
void est_sock_send_all(int sock, const char *buf, int len);


/* Perform an interaction of a URL.
   `url' specifies a URL.
   `pxhost' specifies the host name of a proxy.  If it is `NULL', it is not used.
   `pxport' specifies the port number of the proxy.
   `outsec' specifies timeout in seconds.  If it is negative, it is not used.
   `limsize' specifies limitation of the entity body of response.  If it is negative, it is not
   used.
   `auth' specifies an authority information in such form as "user:pass".  If it is `NULL', it is
   not used.
   `reqheads' specifies a list object of extension headers.  If it is `NULL' it is not used.
   `reqbody' specifies the pointer of the entitiy body of request.  If it is `NULL', "GET"
   method is used.
   `rbsiz' specifies the size of the entity body.
   `rescodep' specifies the pointer to a variable to which the status code of respnese is
   assigned.  If it is `NULL', it is not used.
   `resheads' specifies a map object into which headers of response is stored.  The value of each
   header is recorded as an attribute whose name is converted from the header name into lower
   cases.  The top header for the status code is recorded with the key of an empty string.  If it
   is `NULL', it is not used.
   `resbody' specifies a datum object into which the entity body of response is stored.  If it is
   `NULL', it is not used.
   The return value is true if success, else it is false.
   Headers of "Host", "Connection", "User-Agent", "Authorization", "Accept-Encoding", and
   "Content-Length" are sent implicitly. */
int est_url_shuttle(const char *url, const char *pxhost, int pxport, int outsec, int limsize,
                    const char *auth, const CBLIST *reqheads, const char *reqbody, int rbsiz,
                    int *rescodep, CBMAP *resheads, CBDATUM *resbody);


/* Initialize the document iterator of a node.
   `node' specifies a node connection object.
   The return value is true if success, else it is false. */
int est_node_iter_init(ESTNODE *node);


/* Get the next document of the document iterator of a node.
   `node' specifies a node connection object.
   The return value is a document object though it does not have body text.  If there is no more
   document, `NULL' is returned.  Because the object of the return value is opened with the
   function `est_doc_new', it should be closed with the function `est_doc_delete' if it is no
   longer in use. */
ESTDOC *est_node_iter_next(ESTNODE *node);


/* Add a header to a node connection object.
   `node' specifies a node connection object.
   `name' specifies the name of a header.
   `value' specifies the value of the header.  If it is `NULL', the header is removed.
   If the specified header is already added, the value is concatenated at the end. */
void est_node_add_header(ESTNODE *node, const char *name, const char *value);


/* Create a node result object.
   The return value is a node result object. */
ESTNODERES *est_noderes_new(void);


/* Add a document information to a node result object.
   `nres' specifies a node result object.
   `attrs' specifies a map object of attributes of the document.  The object is closed internally.
   `snippet' specifies the snippet of the document.  The region is released internally. */
void est_noderes_add_doc(ESTNODERES *nres, CBMAP *attrs, char *snippet);


/* Remove the top of result document objects in a node result object.
   `nres' specifies a node result object.
   `attrp' specifies the pointer to a variable to which reference of the map object of attribute
   is assigned.  The object should be deleted with the function  `cbmapclose'.
   `snippetp' specifies the pointer to a variable to which reference of the snippet string is
   assigned.  The region should be released with the function `free'.
   The return value is true if success, else it is false. */
int est_noderes_shift_doc(ESTNODERES *nres, CBMAP **attrp, char **snippetp);


/* Get the modification date of a node result object.
   `nres' specifies a node result object.
   The return value is the modification date of a node result object. */
time_t est_noderes_mdate(ESTNODERES *nres);



#if defined(__cplusplus)                 /* export for C++ */
}
#endif

#endif                                   /* duplication check */


/* END OF FILE */
