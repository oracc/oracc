/*************************************************************************************************
 * The load tester for web applications
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

#define AGENTNAME      "EstLoad"         /* name of the user agent */
#define IOBUFSIZ       8192              /* size of a buffer for I/O */

typedef struct {                         /* type of structure for a target URL */
  char *host;                            /* hostname */
  char *addr;                            /* IP address */
  int port;                              /* port number */
  char *auth;                            /* authority */
  char *path;                            /* path */
  char *query;                           /* query string */
} TARGET;

typedef struct {                         /* type of structure for a worker thread */
  int id;                                /* ID number */
  int ims;                               /* interval time in milliseconds */
  int pb;                                /* to print received data */
  int qb;                                /* to be quiet */
  CBLIST *targets;                       /* list of targets */
  int alive;                             /* to be alive */
} MISSION;


/* global variables */
const char *g_progname;                  /* program name */


/* function prototypes */
int main(int argc, char **argv);
static void usage(void);
static void printferror(const char *format, ...);
static void printfinfo(const char *format, ...);
static int procmain(const char *file, int tnum, int lnum, int ims, int pb, int qb);
static void *procthread(void *arg);


/* main routine */
int main(int argc, char **argv){
  char *file;
  int i, tnum, lnum, ims, pb, qb, rv;
  est_proc_env_reset();
  g_progname = argv[0];
  if(!est_init_net_env()){
    printferror("could not initialize network environment");
    exit(1);
  }
  atexit(est_free_net_env);
  file = NULL;
  tnum = 1;
  lnum = 1;
  ims = 0;
  pb = FALSE;
  qb = FALSE;
  for(i = 1; i < argc; i++){
    if(!file && argv[i][0] == '-'){
      if(!strcmp(argv[i], "-t")){
        if(++i >= argc) usage();
        tnum = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-l")){
        if(++i >= argc) usage();
        lnum = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-i")){
        if(++i >= argc) usage();
        ims = atoi(argv[i]);
      } else if(!strcmp(argv[i], "-p")){
        pb = TRUE;
      } else if(!strcmp(argv[i], "-q")){
        qb = TRUE;
      } else {
        usage();
      }
    } else if(!file){
      file = argv[i];
    } else {
      usage();
    }
  }
  if(!file || tnum < 1 || lnum < 1) usage();
  rv = procmain(file, tnum, lnum, ims, pb, qb);
  return rv;
}


/* print the usage and exit */
static void usage(void){
  fprintf(stderr, "%s: stresser for web applications\n", g_progname);
  fprintf(stderr, "\n");
  fprintf(stderr, "usage:\n");
  fprintf(stderr, "  %s [-t num] [-l num] [-i num] [-p] [-q] [file|url]\n", g_progname);
  fprintf(stderr, "\n");
  exit(1);
}


/* print formatted error string and flush the buffer */
static void printferror(const char *format, ...){
  va_list ap;
  va_start(ap, format);
  fprintf(stderr, "%s: ERROR: ", g_progname);
  vfprintf(stderr, format, ap);
  fputc('\n', stderr);
  fflush(stderr);
  va_end(ap);
}


/* print formatted information string and flush the buffer */
static void printfinfo(const char *format, ...){
  va_list ap;
  va_start(ap, format);
  printf("%s: INFO: ", g_progname);
  vprintf(format, ap);
  putchar('\n');
  fflush(stdout);
  va_end(ap);
}


/* proc the main command */
static int procmain(const char *file, int tnum, int lnum, int ims, int pb, int qb){
  pthread_t *threads;
  MISSION *missions;
  TARGET target;
  CBLIST *list;
  CBMAP *elems;
  const char *line, *host, *pstr, *auth, *path, *query;
  int i, j, cnt, pnum, err;
  double etime;
  if(cbstrfwimatch(file, "http://")){
    list = cblistopen();
    cblistpush(list, file, -1);
  } else if(!(list = cbreadlines(file))){
    printferror("%s: could not open", file);
    return 1;
  }
  threads = cbmalloc(tnum * sizeof(pthread_t));
  missions = cbmalloc(tnum * sizeof(MISSION));
  for(i = 0; i < tnum; i++){
    missions[i].id = i + 1;
    missions[i].ims = ims;
    missions[i].pb = pb;
    missions[i].qb = qb;
    missions[i].targets = cblistopen();
  }
  cnt = 0;
  for(i = 0; i < lnum; i++){
    for(j = 0; j < cblistnum(list); j++){
      line = cblistval(list, j, NULL);
      if((line[0] == '\0')) continue;
      elems = cburlbreak(line);
      host = cbmapget(elems, "host", -1, NULL);
      pnum = (pstr = cbmapget(elems, "port", -1, NULL)) ? atoi(pstr) : 80;
      auth = cbmapget(elems, "authority", -1, NULL);
      path = cbmapget(elems, "path", -1, NULL);
      query = cbmapget(elems, "query", -1, NULL);
      if(!host || pnum < 1){
        printferror("%s: invalid URL", line);
        cbmapclose(elems);
        continue;
      }
      if(!auth) auth = "";
      if(!path) path = "/";
      if(!query) query = "";
      if(!(target.addr = est_get_host_addr(host))){
        printferror("%s: unknown host", host);
        cbmapclose(elems);
        continue;
      }
      target.host = cbmemdup(host, -1);
      target.port = pnum;
      target.auth = cbmemdup(auth, -1);
      target.path = cbmemdup(path, -1);
      target.query = cbmemdup(query, -1);
      cblistpush(missions[cnt++%tnum].targets, (char *)&target, sizeof(TARGET));
      cbmapclose(elems);
    }
  }
  cblistclose(list);
  err = FALSE;
  etime = est_gettimeofday();
  if(tnum > 1){
    for(i = 0; i < tnum; i++){
      missions[i].alive = FALSE;
      if(pthread_create(threads + i, NULL, procthread, missions + i) == 0){
        missions[i].alive = TRUE;
      } else {
        printferror("thread creation failed");
        err = TRUE;
      }
    }
    for(i = 0; i < tnum; i++){
      if(!missions[i].alive) continue;
      if(pthread_join(threads[i], NULL) != 0){
        printferror("thread join failed");
        err = TRUE;
      }
    }
  } else {
    procthread(missions);
  }
  etime = est_gettimeofday() - etime;
  if(cnt > 0 && !err)
    printfinfo("finished: elepsed time: %.3f sec. (average: %.3f)",
               etime / 1000, etime / cnt / 1000);
  for(i = 0; i < tnum; i++){
    for(j = 0; j < cblistnum(missions[i].targets); j++){
      target = *(TARGET *)cblistval(missions[i].targets, j, NULL);
      free(target.query);
      free(target.path);
      free(target.auth);
      free(target.addr);
      free(target.host);
    }
    cblistclose(missions[i].targets);
  }
  free(missions);
  free(threads);
  return err ? 1 : 0;
}


/* process as a child thread */
static void *procthread(void *arg){
  CBLIST *targets;
  TARGET target;
  char iobuf[IOBUFSIZ], *wp, *tmp;
  int i, id, ims, pb, qb, clsock, size;
  double etime;
  targets = ((MISSION *)arg)->targets;
  id = ((MISSION *)arg)->id;
  ims = ((MISSION *)arg)->ims;
  pb = ((MISSION *)arg)->pb;
  qb = ((MISSION *)arg)->qb;
  if(cblistnum(targets) < 1) return NULL;
  printfinfo("%d: started", id);
  etime = est_gettimeofday();
  for(i = 0; i < cblistnum(targets); i++){
    target = *(TARGET *)cblistval(targets, i, NULL);
    if(!qb) printfinfo("%d(%d/%d): http://%s:%d%s%s%s",
                       id, i + 1, cblistnum(targets), target.host, target.port,
                       target.path, target.query[0] != '\0' ? "?" : "", target.query);
    if((clsock = est_get_client_sock(target.addr, target.port)) != -1){
      wp = iobuf;
      wp += sprintf(wp, "GET %s%s%s HTTP/1.0\r\n",
                    target.path, target.query[0] != '\0' ? "?" : "", target.query);
      wp += sprintf(wp, "Host: %s:%d\r\n", target.host, target.port);
      wp += sprintf(wp, "Connection: close\r\n");
      if(target.auth[0] != '\0'){
        tmp = cbbaseencode(target.auth, -1);
        wp += sprintf(wp, "Authorization: Basic %s\r\n", tmp);
        free(tmp);
      }
      wp += sprintf(wp, "Referer: http://%s:%d%s%s%s\r\n", target.host, target.port,
                    target.path, target.query[0] != '\0' ? "?" : "", target.query);
      wp += sprintf(wp, "User-Agent: %s/%s\r\n", AGENTNAME, est_version);
      wp += sprintf(wp, "\r\n");
      est_sock_send_all(clsock, iobuf, wp - iobuf);
      while((size = recv(clsock, iobuf, IOBUFSIZ, 0)) > 0){
        if(pb) fwrite(iobuf, 1, size, stdout);
      }
      est_sock_down(clsock);
    } else {
      printferror("%d(%d/%d): connection failed", id, i + 1, cblistnum(targets));
    }
    if(ims > 0) est_usleep(ims * 1000);
  }
  etime = est_gettimeofday() - etime;
  printfinfo("%d: finished: elapsed time: %.3f sec. (average: %.3f)",
             id, etime / 1000, etime / cblistnum(targets) / 1000);
  return NULL;
}



/* END OF FILE */
