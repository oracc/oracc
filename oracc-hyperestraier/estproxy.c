/*************************************************************************************************
 * A proxy with highlighting
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


#if defined(MYFCGI)
#include <fcgi_stdio.h>
#endif
#include "estraier.h"
#include "estmtdb.h"
#include "estnode.h"
#include "myconf.h"

#define AGENTNAME      "EstProxy"        /* name of the user agent */
#define CONFSUFFIX     ".conf"           /* suffix of the configuration file */
#define NUMBUFSIZ      32                /* size of a buffer for a number */
#define URIBUFSIZ      8192              /* size of a buffer for an URI */
#define OUTBUFSIZ      262144            /* size of the output buffer */
#define SOCKBUFSIZ     8192              /* size of the socket buffer */
#define MINIBNUM       31                /* bucket number of map for trivial use */

#define DRAFTCMD       "[DRAFT]"         /* built-in command for document draft */
#define TEXTCMD        "[TEXT]"          /* built-in command for plain text */
#define HTMLCMD        "[HTML]"          /* built-in command for HTML */
#define MIMECMD        "[MIME]"          /* built-in command for MIME */


/* global variables for configurations */
const char *g_conffile = NULL;           /* path of the configuration file */
const CBLIST *g_replexprs = NULL;        /* list of URL replacement expressions */
const CBLIST *g_rxlist = NULL;           /* list of regular expressions for URL allowing */
int g_passaddr = FALSE;                  /* whether to pass the IP address of clients */
int g_limitsize = 0;                     /* limit size of data to receive */
CBMAP *g_urlrules = NULL;                /* rules of URLs */
CBMAP *g_mtrules = NULL;                 /* rules of media types */
int g_language = 0;                      /* preferred language */
int g_shownavi = 0;                      /* whether to show navigation bar */


/* global variables for parameters */
const char *p_url = NULL;                /* target URL */
const CBLIST *p_words = NULL;            /* words to be highlighted */
int p_once = FALSE;                      /* whether to use direct link */


/* other global variables */
char g_outbuf[OUTBUFSIZ];                /* output buffer */
const char *g_remoteaddr = NULL;         /* IP address of the client */
const char *g_scriptname = NULL;         /* name of the script */
const char *g_hostname = NULL;           /* name of the host */
const char *g_scriptpath = NULL;         /* path of the script */
const char *g_agentname = NULL;          /* name of the user agent */


/* color definitions */
const char *g_wordcolors[] = {           /* highlighted colors */
  "#ffff77",
  "#ffaaaa",
  "#bbbbff",
  "#aaffaa",
  "#88ffff",
  "#ff99ff"
};


/* function prototypes */
int main(int argc, char **argv);
static int realmain(int argc, char **argv);
static void showerror(const char *msg);
static const char *skiplabel(const char *str);
static CBMAP *getparameters(void);
static void xmlprintf(const char *format, ...);
static char *replaceurl(const char *uri);
static void sendform(void);
static char *urltopath(const char *url);
static int getlocalconnection(const char *url, int *codep, CBMAP *heads);
static void senddirlist(const char *path);
static char *readall(int fd, int *sp);
static void passlocalrawdata(int fd);
static void sendlocaldata(void);
static int gethttpconnection(const char *url, int *codep, CBMAP *heads);
static char *recvall(int sock, int *sp);
static void passhttprawdata(int sock);
static void modifylocation(CBMAP *heads, const char *name);
static void sendhttpdata(void);
static void sendnavibar(void);
static void sendsnippet(const char *str);
static const char *langstr(void);
static void sendpagefromdraft(const char *buf, int size, const char *penc);
static void sendpagefromtext(const char *buf, int size, const char *penc);
static void sendpagefromhtml(const char *buf, int size, const char *penc);
static char *htmlenc(const char *str);
static char *htmlrawtext(const char *html);
static void sendpagefrommime(const char *buf, int size, const char *penc);
static void sendmimepart(const char *body, int bsiz, CBMAP *attrs, const char *penc);
static char *mimestr(const char *mime);
static void sendpagewithxcmd(const char *buf, int size, const char *penc, const char *cmd);


/* main routine */
int main(int argc, char **argv){
#if defined(MYFCGI)
  static int cnt = 0;
  est_proc_env_reset();
  est_init_net_env();
  while(FCGI_Accept() >= 0){
    realmain(argc, argv);
    fflush(stdout);
    if(++cnt >= 256) exit(0);
  }
  est_free_net_env();
  return 0;
#else
  est_proc_env_reset();
  est_init_net_env();
  realmain(argc, argv);
  est_free_net_env();
  return 0;
#endif
}


/* real main routine */
static int realmain(int argc, char **argv){
  CBLIST *lines, *rplist, *rxlist, *wlist, *list;
  CBMAP *params, *urlmap, *mtmap;
  const char *rp, *pv;
  char *tmp, *wp, numbuf[NUMBUFSIZ];
  int i, len, allow;
  setvbuf(stdout, g_outbuf, _IOFBF, OUTBUFSIZ);
  if((rp = getenv("REMOTE_ADDR")) != NULL){
    g_remoteaddr = rp;
  } else {
    g_remoteaddr = "0.0.0.0";
  }
  g_scriptname = argv[0];
  if((rp = getenv("SCRIPT_NAME")) != NULL) g_scriptname = rp;
  g_hostname = est_get_host_name();
  if((rp = getenv("HTTP_HOST")) != NULL) g_hostname = rp;
  g_scriptpath = g_scriptname;
  g_agentname = AGENTNAME;
  if((rp = getenv("HTTP_USER_AGENT")) != NULL) g_agentname = rp;
  if((rp = strrchr(g_scriptname, '/')) != NULL) g_scriptname = rp + 1;
  tmp = cbmalloc(strlen(g_scriptname) + strlen(CONFSUFFIX) + 1);
  sprintf(tmp, "%s", g_scriptname);
  cbglobalgc(tmp, free);
  if(!(wp = strrchr(tmp, '.'))) wp = tmp + strlen(tmp);
  sprintf(wp, "%s", CONFSUFFIX);
  g_conffile = tmp;
  if(!(lines = cbreadlines(g_conffile))) showerror("the configuration file is missing.");
  cbglobalgc(lines, (void (*)(void *))cblistclose);
  rplist = cblistopen();
  cbglobalgc(rplist, (void (*)(void *))cblistclose);
  rxlist = cblistopen();
  cbglobalgc(rxlist, (void (*)(void *))cblistclose);
  urlmap = cbmapopenex(MINIBNUM);
  cbglobalgc(urlmap, (void (*)(void *))cbmapclose);
  mtmap = cbmapopenex(MINIBNUM);
  cbglobalgc(mtmap, (void (*)(void *))cbmapclose);
  for(i = 0; i < cblistnum(lines); i++){
    rp = cblistval(lines, i, NULL);
    if(cbstrfwimatch(rp, "replace:")){
      rp = skiplabel(rp);
      if(*rp != '\0') cblistpush(rplist, rp, -1);
    } else if(cbstrfwimatch(rp, "allowrx:")){
      rp = skiplabel(rp);
      if(*rp != '\0'){
        tmp = cbsprintf("a*I:%s", rp);
        cblistpushbuf(rxlist, tmp, strlen(tmp));
      }
    } else if(cbstrfwimatch(rp, "denyrx:")){
      rp = skiplabel(rp);
      if(*rp != '\0'){
        tmp = cbsprintf("d*I:%s", rp);
        cblistpushbuf(rxlist, tmp, strlen(tmp));
      }
    } else if(cbstrfwimatch(rp, "passaddr:")){
      g_passaddr = atoi(skiplabel(rp)) > 0;
    } else if(cbstrfwimatch(rp, "limitsize:")){
      g_limitsize = atoi(skiplabel(rp)) * 1024 * 1024;
    } else if(cbstrfwimatch(rp, "urlrule:")){
      rp = skiplabel(rp);
      if((pv = strstr(rp, "{{!}}")) != NULL) cbmapput(urlmap, rp, pv - rp, pv + 5, -1, TRUE);
    } else if(cbstrfwimatch(rp, "typerule:")){
      rp = skiplabel(rp);
      if((pv = strstr(rp, "{{!}}")) != NULL) cbmapput(mtmap, rp, pv - rp, pv + 5, -1, TRUE);
    } else if(cbstrfwimatch(rp, "language:")){
      g_language = atoi(skiplabel(rp));
    } else if(cbstrfwimatch(rp, "shownavi:")){
      g_shownavi = atoi(skiplabel(rp)) > 0;
    }
  }
  g_replexprs = rplist;
  g_rxlist = rxlist;
  g_urlrules = urlmap;
  g_mtrules = mtmap;
  params = getparameters();
  cbglobalgc(params, (void (*)(void *))cbmapclose);
  wlist = cblistopen();
  cbglobalgc(wlist, (void (*)(void *))cblistclose);
  if(!(p_url = cbmapget(params, "url", -1, NULL))) p_url = "";
  for(i = 0; i < 256; i++){
    len = sprintf(numbuf, "word%d", i);
    if((rp = cbmapget(params, numbuf, len, NULL)) != NULL && *rp != '\0')
      cblistpush(wlist, rp, -1);
  }
  if((rp = cbmapget(params, "words", -1, NULL)) != NULL){
    tmp = est_regex_replace(rp, "(\xe3\x80\x80)+", " ");
    list = cbsplit(tmp, -1, ", \t\n\r");
    for(i = 0; i < cblistnum(list); i++){
      rp = cblistval(list, i, NULL);
      if(*rp != '\0') cblistpush(wlist, rp, -1);
    }
    cblistclose(list);
    free(tmp);
  }
  if((rp = cbmapget(params, "once", -1, NULL)) != NULL) p_once = atoi(rp) > 0;
  p_words = wlist;
  if(*p_url == '\0'){
    sendform();
  } else {
    allow = FALSE;
    tmp = replaceurl(p_url);
    for(i = 0; i < cblistnum(rxlist); i++){
      rp = cblistval(rxlist, i, NULL);
      switch(*rp){
      case 'a':
        rp++;
        if(est_regex_match_str(rp, tmp)) allow = TRUE;
        break;
      case 'd':
        rp++;
        if(est_regex_match_str(rp, tmp)) allow = FALSE;
        break;
      default:
        break;
      }
    }
    if(allow && cbstrfwimatch(tmp, "file://")){
      sendlocaldata();
    } else if(allow && cbstrfwimatch(tmp, "http://")){
      sendhttpdata();
    } else {
      printf("Status: 400 Forbidden\r\n");
      printf("Content-Type: text/plain; charset=UTF-8\r\n");
      printf("\r\n");
      printf("Error: the requested URL is not allowed\n");
    }
    free(tmp);
  }
  return 0;
}


/* show the error page and exit */
static void showerror(const char *msg){
  printf("Status: 500 Internal Server Error\r\n");
  printf("Content-Type: text/plain; charset=UTF-8\r\n");
  printf("\r\n");
  printf("Error: %s\n", msg);
  exit(1);
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


/* get CGI parameters */
static CBMAP *getparameters(void){
  int maxlen = 1024 * 1024 * 32;
  CBMAP *map, *attrs;
  CBLIST *pairs, *parts;
  const char *rp, *body;
  char *buf, *key, *val, *dkey, *dval, *wp, *bound, *fbuf, *aname;
  int i, len, c, blen, flen;
  map = cbmapopenex(37);
  buf = NULL;
  len = 0;
  if((rp = getenv("REQUEST_METHOD")) != NULL && !strcmp(rp, "POST") &&
     (rp = getenv("CONTENT_LENGTH")) != NULL && (len = atoi(rp)) > 0){
    if(len > maxlen) len = maxlen;
    buf = cbmalloc(len + 1);
    for(i = 0; i < len && (c = getchar()) != EOF; i++){
      buf[i] = c;
    }
    buf[i] = '\0';
    if(i != len){
      free(buf);
      buf = NULL;
    }
  } else if((rp = getenv("QUERY_STRING")) != NULL){
    buf = cbmemdup(rp, -1);
    len = strlen(buf);
  }
  if(buf && len > 0){
    if((rp = getenv("CONTENT_TYPE")) != NULL && cbstrfwmatch(rp, "multipart/form-data") &&
       (rp = strstr(rp, "boundary=")) != NULL){
      rp += 9;
      bound = cbmemdup(rp, -1);
      if((wp = strchr(bound, ';')) != NULL) *wp = '\0';
      parts = cbmimeparts(buf, len, bound);
      for(i = 0; i < cblistnum(parts); i++){
        body = cblistval(parts, i, &blen);
        attrs = cbmapopen();
        fbuf = cbmimebreak(body, blen, attrs, &flen);
        if((rp = cbmapget(attrs, "NAME", -1, NULL)) != NULL){
          cbmapput(map, rp, -1, fbuf, flen, FALSE);
          aname = cbsprintf("%s-filename", rp);
          if((rp = cbmapget(attrs, "FILENAME", -1, NULL)) != NULL)
            cbmapput(map, aname, -1, rp, -1, FALSE);
          free(aname);
        }
        free(fbuf);
        cbmapclose(attrs);
      }
      cblistclose(parts);
      free(bound);
    } else {
      pairs = cbsplit(buf, -1, "&");
      for(i = 0; i < cblistnum(pairs); i++){
        key = cbmemdup(cblistval(pairs, i, NULL), -1);
        if((val = strchr(key, '=')) != NULL){
          *(val++) = '\0';
          dkey = cburldecode(key, NULL);
          dval = cburldecode(val, NULL);
          cbmapput(map, dkey, -1, dval, -1, FALSE);
          free(dval);
          free(dkey);
        }
        free(key);
      }
      cblistclose(pairs);
    }
  }
  free(buf);
  return map;
}


/* output escaped string */
static void xmlprintf(const char *format, ...){
  va_list ap;
  const char *rp;
  char *tmp, cbuf[32], *ebuf;
  unsigned char c;
  int cblen, cnt, mlen;
  va_start(ap, format);
  while(*format != '\0'){
    if(*format == '%'){
      cbuf[0] = '%';
      cblen = 1;
      format++;
      while(strchr("0123456789 .+-", *format) && *format != '\0' && cblen < sizeof(cbuf) - 1){
        cbuf[cblen++] = *format;
        format++;
      }
      cbuf[cblen++] = *format;
      cbuf[cblen] = '\0';
      switch(*format){
      case 's':
        tmp = va_arg(ap, char *);
        if(!tmp) tmp = "(null)";
        printf(cbuf, tmp);
        break;
      case 'd':
        printf(cbuf, va_arg(ap, int));
        break;
      case 'o': case 'u': case 'x': case 'X': case 'c':
        printf(cbuf, va_arg(ap, unsigned int));
        break;
      case 'e': case 'E': case 'f': case 'g': case 'G':
        printf(cbuf, va_arg(ap, double));
        break;
      case '@':
        tmp = va_arg(ap, char *);
        if(!tmp) tmp = "(null)";
        ebuf = NULL;
        if(cblen > 2){
          mlen = atoi(cbuf + 1) * 10;
          cnt = 0;
          rp = tmp;
          while(*rp != '\0'){
            if((*rp & 0x80) == 0x00){
              cnt += 10;
            } else if((*rp & 0xe0) == 0xc0){
              cnt += 15;
            } else if((*rp & 0xf0) == 0xe0 || (*rp & 0xf8) == 0xf0){
              cnt += 20;
            }
            if(cnt > mlen){
              ebuf = cbmemdup(tmp, rp - tmp);
              tmp = ebuf;
              break;
            }
            rp++;
          }
        }
        while(*tmp){
          switch(*tmp){
          case '&': printf("&amp;"); break;
          case '<': printf("&lt;"); break;
          case '>': printf("&gt;"); break;
          case '"': printf("&quot;"); break;
          default:
            if(!((*tmp >= 0 && *tmp <= 0x8) || (*tmp >= 0x0e && *tmp <= 0x1f))) putchar(*tmp);
            break;
          }
          tmp++;
        }
        if(ebuf){
          free(ebuf);
          printf("...");
        }
        break;
      case '?':
        tmp = va_arg(ap, char *);
        if(!tmp) tmp = "(null)";
        while(*tmp){
          c = *(unsigned char *)tmp;
          if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
             (c >= '0' && c <= '9') || (c != '\0' && strchr("_-.", c))){
            putchar(c);
          } else {
            printf("%%%02X", c);
          }
          tmp++;
        }
        break;
      case '%':
        putchar('%');
        break;
      }
    } else {
      putchar(*format);
    }
    format++;
  }
  va_end(ap);
}


/* make a URI to be shown */
static char *replaceurl(const char *uri){
  char *turi, *bef, *aft, *pv, *nuri;
  int i;
  turi = cbmemdup(uri, -1);
  for(i = 0; i < cblistnum(g_replexprs); i++){
    bef = cbmemdup(cblistval(g_replexprs, i, NULL), -1);
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


/* show the input form */
static void sendform(void){
  printf("Content-Type: text/html; charset=UTF-8\r\n");
  printf("\r\n");
  xmlprintf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  xmlprintf("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\""
            " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
  xmlprintf("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n");
  xmlprintf("<head>\n");
  xmlprintf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n");
  xmlprintf("<meta http-equiv=\"Content-Style-Type\" content=\"text/css\" />\n");
  xmlprintf("<meta name=\"robots\" content=\"NOFOLLOW\" />\n");
  xmlprintf("<link rel=\"contents\" href=\"./\" />\n");
  xmlprintf("<title>%@/%@</title>\n", AGENTNAME, est_version);
  xmlprintf("<style type=\"text/css\">html {\n");
  xmlprintf("  margin: 0em 0em; padding: 0em 0em;\n");
  xmlprintf("  background: #ffffff none;\n");
  xmlprintf("}\n");
  xmlprintf("body {\n");
  xmlprintf("  margin: 2em 2em; padding: 0em 0em;\n");
  xmlprintf("  color: #111111;\n");
  xmlprintf("}\n");
  xmlprintf("div.form_frame {\n");
  xmlprintf("  margin: 0em 0em; padding: 0.5em 0.5em;\n");
  xmlprintf("  background: #eeeeff none; border: solid 1pt #bbbbcc;\n");
  xmlprintf("}\n");
  xmlprintf("div.form_basic, div.form_extension {\n");
  xmlprintf("  margin: 0.5em 0.5em; padding: 0em 0em;\n");
  xmlprintf("  color: #111111;\n");
  xmlprintf("}\n");
  xmlprintf("label {\n");
  xmlprintf("  margin-left: 0.8em;\n");
  xmlprintf("  color: #333333;\n");
  xmlprintf("}\n");
  xmlprintf("</style>\n");
  xmlprintf("</head>\n");
  xmlprintf("<body>\n");
  xmlprintf("<h1>%@/%@</h1>\n", AGENTNAME, est_version);
  xmlprintf("<div class=\"form_frame\">\n");
  xmlprintf("<form action=\"%@\" method=\"get\" id=\"form_self\">\n", g_scriptname);
  xmlprintf("<div class=\"form_basic\">\n");
  xmlprintf("<label for=\"url\" class=\"label\">URL:</label>\n");
  xmlprintf("<input type=\"text\" name=\"url\" value=\"\""
            " size=\"64\" id=\"url\" class=\"url\" tabindex=\"1\" accesskey=\"0\" />\n");
  xmlprintf("<input type=\"submit\" value=\"Go\""
            " id=\"search\" class=\"submit\" tabindex=\"2\" accesskey=\"1\" />\n");
  xmlprintf("</div>\n");
  xmlprintf("<div class=\"form_extension\">\n");
  xmlprintf("<label for=\"words\" class=\"label\">highlighted words:</label>\n");
  xmlprintf("<input type=\"text\" name=\"words\" value=\"\""
            " size=\"48\" id=\"words\" class=\"text\" tabindex=\"3\" accesskey=\"2\" />\n");
  xmlprintf("<label for=\"once\" class=\"label\">once:</label>\n");
  xmlprintf("<input type=\"checkbox\" name=\"once\" value=\"1\" id=\"once\""
            " class=\"checkbox\" tabindex=\"4\" accesskey=\"3\" />\n");
  xmlprintf("</div>\n");
  xmlprintf("</form>\n");
  xmlprintf("</div>\n");
  xmlprintf("</body>\n");
  xmlprintf("</html>\n");
}


/* get the local path of a URL */
static char *urltopath(const char *url){
  static char pbuf[URIBUFSIZ];
  const char *pv, *elem;
  char *wp, *dbuf;
  CBLIST *list;
  int i;
  if(!cbstrfwimatch(url, "file://")) return NULL;
  pv = url + 7;
  if(!(url = strchr(pv, '/'))) return NULL;
  wp = pbuf;
  if(ESTPATHCHR == '\\'){
    if(url[0] == '/' && ((url[1] >= 'A' && url[1] <= 'Z') || (url[1] >= 'a' && url[1] <= 'z')) &&
       url[2] == '|' && url[3] == '/'){
      wp += sprintf(wp, "%c:", url[1]);
      url += 3;
    } else if(url > pv){
      wp += sprintf(wp, "\\\\");
      memcpy(wp, pv, url - pv);
      wp += url - pv;
    }
  }
  list = cbsplit(url, -1, "/");
  for(i = 0; i < cblistnum(list); i++){
    elem = cblistval(list, i, NULL);
    if(elem[0] == '\0') continue;
    dbuf = cburldecode(elem, NULL);
    wp += sprintf(wp, "%c%s", ESTPATHCHR, dbuf);
    free(dbuf);
  }
  *wp = '\0';
  cblistclose(list);
  return cbmemdup(pbuf, -1);
}


/* get the connection of a URL */
static int getlocalconnection(const char *url, int *codep, CBMAP *heads){
  const char *ext, *pv;
  char *path;
  int fd;
  struct stat sbuf;
  cbmapput(heads, "ERROR", -1, "", -1, TRUE);
  cbmapput(heads, "", -1, "", -1, TRUE);
  if(!(path = urltopath(url))){
    *codep = 400;
    cbmapput(heads, "ERROR", -1, "invalid URL", -1, TRUE);
    return -1;
  }
  if(stat(path, &sbuf) == -1){
    if(errno == EACCES){
      *codep = 403;
      cbmapput(heads, "ERROR", -1, "forbidden", -1, TRUE);
    } else {
      *codep = 404;
      cbmapput(heads, "ERROR", -1, "file not found", -1, TRUE);
    }
  }
  if(S_ISDIR(sbuf.st_mode)){
    *codep = 0;
    cbmapput(heads, "ERROR", -1, path, -1, TRUE);
    free(path);
    return -1;
  }
  if((fd = open(path, O_RDONLY, 0)) == -1){
    if(errno == EACCES){
      *codep = 403;
      cbmapput(heads, "ERROR", -1, "forbidden", -1, TRUE);
    } else {
      *codep = 404;
      cbmapput(heads, "ERROR", -1, "file not found", -1, TRUE);
    }
    free(path);
    return -1;
  }
  *codep = 200;
  ext = NULL;
  if(!(pv = strrchr(url, ESTPATHCHR))) pv = url;
  if((pv = strrchr(pv, ESTEXTCHR)) != NULL) ext = pv;
  cbmapput(heads, "content-type", -1, est_ext_type(ext ? ext : ""), -1, TRUE);
  free(path);
  return fd;
}


/* send a list of files in a directory */
static void senddirlist(const char *path){
  CBLIST *list;
  const char *elem;
  char *burl, *eurl, *nurl;
  int i;
  printf("Content-Type: text/html\n");
  printf("\r\n");
  xmlprintf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  xmlprintf("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\""
            " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
  xmlprintf("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"%@\" lang=\"%p\">\n",
            g_language, g_language);
  xmlprintf("<head>\n");
  xmlprintf("<meta http-equiv=\"Content-Type\" content=\"text/html\" />\n");
  xmlprintf("<meta http-equiv=\"Content-Style-Type\" content=\"text/css\" />\n");
  xmlprintf("<meta name=\"robots\" content=\"NOFOLLOW\" />\n");
  xmlprintf("<link rel=\"contents\" href=\"./\" />\n");
  xmlprintf("<title>%@/%@</title>\n", AGENTNAME, est_version);
  xmlprintf("<style type=\"text/css\">html {\n");
  xmlprintf("  margin: 0em 0em; padding: 0em 0em;\n");
  xmlprintf("  background: #ffffff none;\n");
  xmlprintf("}\n");
  xmlprintf("body {\n");
  xmlprintf("  margin: 2em 2em; padding: 0em 0em;\n");
  xmlprintf("  color: #111111;\n");
  xmlprintf("}\n");
  xmlprintf("</style>\n");
  xmlprintf("</head>\n");
  xmlprintf("<body>\n");
  xmlprintf("<h1>%@</h1>\n", path);
  if((list = cbdirlist(path)) != NULL){
    burl = cbsprintf("%s/", p_url);
    cblistsort(list);
    for(i = 0; i < cblistnum(list); i++){
      elem = cblistval(list, i, NULL);
      eurl = cburlencode(elem, -1);
      nurl = cburlresolve(burl, elem);
      if(p_once){
        xmlprintf("<li><a href=\"%@\">%@</a></li>", nurl, elem);
      } else {
        xmlprintf("<li><a href=\"%s?url=%?\">%@</a></li>", g_scriptname, nurl, elem);
      }
      free(nurl);
      free(eurl);
    }
    cblistclose(list);
    free(burl);
  } else {
    xmlprintf("<p>The directory could not be opened.</p>\n");
  }
  xmlprintf("</body>\n");
  xmlprintf("</html>\n");
}


/* read all data of a local connection */
static char *readall(int fd, int *sp){
  CBDATUM *datum;
  char iobuf[SOCKBUFSIZ];
  int size;
  datum = cbdatumopen(NULL, -1);
  while(cbdatumsize(datum) < g_limitsize && (size = read(fd, iobuf, SOCKBUFSIZ)) > 0){
    cbdatumcat(datum, iobuf, size);
  }
  return cbdatumtomalloc(datum, sp);
}


/* pass through raw data of a connection */
static void passlocalrawdata(int fd){
  char iobuf[SOCKBUFSIZ];
  int size;
  while((size = read(fd, iobuf, SOCKBUFSIZ)) > 0){
    fwrite(iobuf, 1, size, stdout);
  }
}


/* send the data of local file */
static void sendlocaldata(void){
  CBMAP *heads;
  const char *kbuf, *vbuf, *cmd;
  char *url, *type, *enc, *pv, *rbuf;
  int fd, code, ksiz, rsiz;
  url = replaceurl(p_url);
  heads = cbmapopenex(MINIBNUM);
  if((fd = getlocalconnection(url, &code, heads)) == -1){
    if(code == 0){
      senddirlist(cbmapget(heads, "ERROR", -1, NULL));
    } else {
      showerror(cbmapget(heads, "ERROR", -1, NULL));
    }
    cbmapclose(heads);
    free(url);
    return;
  }
  cbmapiterinit(g_urlrules);
  while((kbuf = cbmapiternext(g_urlrules, &ksiz)) != NULL){
    if(est_regex_match_str(kbuf, p_url)){
      cbmapput(heads, "content-type", -1, cbmapget(g_urlrules, kbuf, ksiz, NULL), -1, TRUE);
      break;
    }
  }
  if(!(vbuf = cbmapget(heads, "content-type", -1, NULL))) vbuf = "text/plain";
  type = cbmemdup(vbuf, -1);
  if((pv = strchr(type, ';')) != NULL) *pv = '\0';
  cbstrtolower(type);
  enc = NULL;
  if((pv = strstr(vbuf, "charset=")) != NULL || (pv = strstr(vbuf, "CHARSET=")) != NULL){
    pv = strchr(pv, '=') + 1;
    if(*pv == '"') pv++;
    enc = cbmemdup(pv, -1);
    if((pv = strchr(enc, '"')) != NULL) *pv = '\0';
  }
  cmd = "";
  cbmapiterinit(g_mtrules);
  while((kbuf = cbmapiternext(g_mtrules, &ksiz)) != NULL){
    if(est_regex_match_str(kbuf, type)){
      cmd = cbmapget(g_mtrules, kbuf, ksiz, NULL);
      break;
    }
  }
  printf("Status: %d\r\n", code);
  cbmapout(heads, "", -1);
  cbmapout(heads, "ERROR", -1);
  if(*cmd != '\0'){
    cbmapput(heads, "content-type", -1, "text/html; charset=UTF-8", -1, TRUE);
    cbmapout(heads, "content-length", -1);
    cbmapout(heads, "content-encoding", -1);
    cbmapout(heads, "connection", -1);
    cbmapout(heads, "set-cookie", -1);
    cbmapout(heads, "set-cookie2", -1);
    cbmapout(heads, "etag", -1);
    cbmapout(heads, "transfer-encoding", -1);
  }
  modifylocation(heads, "location");
  modifylocation(heads, "content-location");
  cbmapiterinit(heads);
  while((kbuf = cbmapiternext(heads, &ksiz)) != NULL){
    printf("%s: %s\r\n", kbuf, cbmapget(heads, kbuf, ksiz, NULL));
  }
  printf("\r\n");
  if(!strcmp(cmd, DRAFTCMD)){
    rbuf = readall(fd, &rsiz);
    sendpagefromdraft(rbuf, rsiz, enc);
    free(rbuf);
  } else if(!strcmp(cmd, TEXTCMD)){
    rbuf = readall(fd, &rsiz);
    sendpagefromtext(rbuf, rsiz, enc);
    free(rbuf);
  } else if(!strcmp(cmd, HTMLCMD)){
    rbuf = readall(fd, &rsiz);
    sendpagefromhtml(rbuf, rsiz, enc);
    free(rbuf);
  } else if(!strcmp(cmd, MIMECMD)){
    rbuf = readall(fd, &rsiz);
    sendpagefrommime(rbuf, rsiz, enc);
    free(rbuf);
  } else if(*cmd != '\0'){
    rbuf = readall(fd, &rsiz);
    sendpagewithxcmd(rbuf, rsiz, enc, cmd);
    free(rbuf);
  } else {
    passlocalrawdata(fd);
  }
  free(enc);
  free(type);
  close(fd);
  cbmapclose(heads);
  free(url);
}


/* get the HTTP connection of a URL */
static int gethttpconnection(const char *url, int *codep, CBMAP *heads){
  CBMAP *elems;
  const char *scheme, *host, *pstr, *auth, *path, *query, *rp;
  char *addr, iobuf[SOCKBUFSIZ], *wp, *tmp, name[SOCKBUFSIZ], *pv;
  int pnum, sock, size, nsiz;
  cbmapput(heads, "ERROR", -1, "", -1, TRUE);
  cbmapput(heads, "", -1, "", -1, TRUE);
  elems = cburlbreak(url);
  scheme = cbmapget(elems, "scheme", -1, NULL);
  host = cbmapget(elems, "host", -1, NULL);
  pnum = (pstr = cbmapget(elems, "port", -1, NULL)) ? atoi(pstr) : 80;
  auth = cbmapget(elems, "authority", -1, NULL);
  path = cbmapget(elems, "path", -1, NULL);
  query = cbmapget(elems, "query", -1, NULL);
  if(!scheme || cbstricmp(scheme, "http") || !host || pnum < 1){
    cbmapput(heads, "ERROR", -1, "invalid URL", -1, TRUE);
    cbmapclose(elems);
    return -1;
  }
  if(!auth) auth = "";
  if(!path) path = "/";
  if(!query) query = "";
  if(!(addr = est_get_host_addr(host))){
    cbmapput(heads, "ERROR", -1, "unknown host", -1, TRUE);
    cbmapclose(elems);
    return -1;
  }
  if((sock = est_get_client_sock(addr, pnum)) == -1){
    cbmapput(heads, "ERROR", -1, "connection failed", -1, TRUE);
    cbmapclose(elems);
    return -1;
  }
  wp = iobuf;
  wp += sprintf(wp, "GET %s%s%s HTTP/1.0\r\n", path, query[0] != '\0' ? "?" : "", query);
  wp += sprintf(wp, "Host: %s:%d\r\n", host, pnum);
  wp += sprintf(wp, "Connection: close\r\n");
  if(auth[0] != '\0'){
    tmp = cbbaseencode(auth, -1);
    wp += sprintf(wp, "Authorization: Basic %s\r\n", tmp);
    free(tmp);
  }
  wp += sprintf(wp, "User-Agent: %s\r\n", g_agentname);
  if(g_passaddr) wp += sprintf(wp, "X-Forwarded-For: %s\r\n", g_remoteaddr);
  switch(g_language){
  case ESTLANGEN:
    wp += sprintf(wp, "Accept-Language: en,ja\r\n");
    break;
  case ESTLANGJA:
    wp += sprintf(wp, "Accept-Language: ja,en\r\n");
    break;
  case ESTLANGZH:
    wp += sprintf(wp, "Accept-Language: zh,en\r\n");
    break;
  case ESTLANGKO:
    wp += sprintf(wp, "Accept-Language: ko,en\r\n");
    break;
  }
  wp += sprintf(wp, "\r\n");
  est_sock_send_all(sock, iobuf, wp - iobuf);
  if((size = est_sock_recv_line(sock, iobuf, SOCKBUFSIZ - 1)) < 1 ||
     !cbstrfwmatch(iobuf, "HTTP/") || !(rp = strchr(iobuf, ' '))){
    cbmapput(heads, "ERROR", -1, "no data received", -1, TRUE);
    est_sock_down(sock);
    cbmapclose(elems);
    return -1;
  }
  *codep = atoi(rp);
  cbmapput(heads, "", 0, iobuf, size, TRUE);
  name[0] = '\0';
  nsiz = 0;
  while((size = est_sock_recv_line(sock, iobuf, SOCKBUFSIZ - 1)) > 0){
    if(iobuf[0] == ' ' || iobuf[0] == '\t'){
      if(name[0] != '\0'){
        iobuf[0] = ' ';
        cbmapputcat(heads, name, nsiz, iobuf, size);
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
      if(cbmapget(heads, name, nsiz, NULL)){
        cbmapputcat(heads, name, nsiz, ", ", 2);
        cbmapputcat(heads, name, nsiz, pv, -1);
      } else {
        cbmapput(heads, name, nsiz, rp, -1, TRUE);
      }
    }
  }
  cbmapclose(elems);
  return sock;
}


/* receive all data of a networdk connection */
static char *recvall(int sock, int *sp){
  CBDATUM *datum;
  char iobuf[SOCKBUFSIZ];
  int size;
  datum = cbdatumopen(NULL, -1);
  while(cbdatumsize(datum) < g_limitsize && (size = recv(sock, iobuf, SOCKBUFSIZ, 0)) > 0){
    cbdatumcat(datum, iobuf, size);
  }
  return cbdatumtomalloc(datum, sp);
}


/* pass through raw data of a connection */
static void passhttprawdata(int sock){
  char iobuf[SOCKBUFSIZ];
  int size;
  while((size = recv(sock, iobuf, SOCKBUFSIZ, 0)) > 0){
    fwrite(iobuf, 1, size, stdout);
  }
}


/* modify a header of location */
static void modifylocation(CBMAP *heads, const char *name){
  CBDATUM *datum;
  const char *url;
  char *nurl;
  int i;
  if(!(url = cbmapget(heads, name, -1, NULL))) return;
  nurl = cburlresolve(p_url, url);
  datum = cbdatumopen(NULL, -1);
  cbdatumprintf(datum, "http://%s%s?url=%?&once=%d", g_hostname, g_scriptpath, nurl, p_once);
  for(i = 0; i < cblistnum(p_words); i++){
    cbdatumprintf(datum, "&word%d=%?", i + 1, cblistval(p_words, i, NULL));
  }
  cbmapput(heads, name, -1, cbdatumptr(datum), cbdatumsize(datum), TRUE);
  cbdatumclose(datum);
  free(nurl);
}


/* send the data of HTTP connection */
static void sendhttpdata(void){
  CBMAP *heads;
  const char *kbuf, *vbuf, *cmd;
  char *url, *type, *enc, *pv, *rbuf;
  int sock, code, ksiz, rsiz;
  url = replaceurl(p_url);
  heads = cbmapopenex(MINIBNUM);
  if((sock = gethttpconnection(url, &code, heads)) == -1){
    showerror(cbmapget(heads, "ERROR", -1, NULL));
    cbmapclose(heads);
    free(url);
    return;
  }
  cbmapiterinit(g_urlrules);
  while((kbuf = cbmapiternext(g_urlrules, &ksiz)) != NULL){
    if(est_regex_match_str(kbuf, p_url)){
      cbmapput(heads, "content-type", -1, cbmapget(g_urlrules, kbuf, ksiz, NULL), -1, TRUE);
      break;
    }
  }
  if(!(vbuf = cbmapget(heads, "content-type", -1, NULL))) vbuf = "text/plain";
  type = cbmemdup(vbuf, -1);
  if((pv = strchr(type, ';')) != NULL) *pv = '\0';
  cbstrtolower(type);
  enc = NULL;
  if((pv = strstr(vbuf, "charset=")) != NULL || (pv = strstr(vbuf, "CHARSET=")) != NULL){
    pv = strchr(pv, '=') + 1;
    if(*pv == '"') pv++;
    enc = cbmemdup(pv, -1);
    if((pv = strchr(enc, '"')) != NULL) *pv = '\0';
  }
  cmd = "";
  cbmapiterinit(g_mtrules);
  while((kbuf = cbmapiternext(g_mtrules, &ksiz)) != NULL){
    if(est_regex_match_str(kbuf, type)){
      cmd = cbmapget(g_mtrules, kbuf, ksiz, NULL);
      break;
    }
  }
  printf("Status: %d\r\n", code);
  cbmapout(heads, "", -1);
  cbmapout(heads, "ERROR", -1);
  if(*cmd != '\0'){
    cbmapput(heads, "content-type", -1, "text/html; charset=UTF-8", -1, TRUE);
    cbmapout(heads, "content-length", -1);
    cbmapout(heads, "content-encoding", -1);
    cbmapout(heads, "connection", -1);
    cbmapout(heads, "set-cookie", -1);
    cbmapout(heads, "set-cookie2", -1);
    cbmapout(heads, "etag", -1);
    cbmapout(heads, "transfer-encoding", -1);
  }
  modifylocation(heads, "location");
  modifylocation(heads, "content-location");
  cbmapiterinit(heads);
  while((kbuf = cbmapiternext(heads, &ksiz)) != NULL){
    printf("%s: %s\r\n", kbuf, cbmapget(heads, kbuf, ksiz, NULL));
  }
  printf("\r\n");
  if(!strcmp(cmd, DRAFTCMD)){
    rbuf = recvall(sock, &rsiz);
    sendpagefromdraft(rbuf, rsiz, enc);
    free(rbuf);
  } else if(!strcmp(cmd, TEXTCMD)){
    rbuf = recvall(sock, &rsiz);
    sendpagefromtext(rbuf, rsiz, enc);
    free(rbuf);
  } else if(!strcmp(cmd, HTMLCMD)){
    rbuf = recvall(sock, &rsiz);
    sendpagefromhtml(rbuf, rsiz, enc);
    free(rbuf);
  } else if(!strcmp(cmd, MIMECMD)){
    rbuf = recvall(sock, &rsiz);
    sendpagefrommime(rbuf, rsiz, enc);
    free(rbuf);
  } else if(*cmd != '\0'){
    rbuf = recvall(sock, &rsiz);
    sendpagewithxcmd(rbuf, rsiz, enc, cmd);
    free(rbuf);
  } else {
    passhttprawdata(sock);
  }
  free(enc);
  free(type);
  est_sock_down(sock);
  cbmapclose(heads);
  free(url);
}


/* send the navigation bar */
static void sendnavibar(void){
  static int first = TRUE;
  const char *defstyle = "text-align: left; text-indent: 0em;"
    " margin: 0.2em 0.2em; padding 0em; 0em;"
    " color: #111111; background: transparent none; border: none;"
    " font-size: small; font-weight: normal;";
  int i;
  if(!first) return;
  first = FALSE;
  xmlprintf("<div id=\"estproxy_navi\" class=\"estproxy_navi\""
            " style=\"display: block; margin: 0.2em 0.2em 0.5em 0.2em; padding: 0.5em 1.0em;"
            " background: #eeeeff; border: outset 1pt #bbbbcc;\">\n");
  xmlprintf("<p class=\"estproxy_note\" style=\"%@ font-style: italic; float: left;\">"
            "This page have been mediated by Hyper Estraier %@.</p>\n", defstyle, est_version);
  xmlprintf("<form action=\"%@\" method=\"get\" id=\"estproxy_form\" style=\"%@\">\n",
            g_scriptname, defstyle);
  xmlprintf("<div class=\"estproxy_reloadnavi\" style=\"%@ text-align: right;\">\n", defstyle);
  xmlprintf("<label for=\"estproxy_words\" class=\"estproxy_label\">"
            "highlighted words:</label>\n");
  xmlprintf("<input type=\"text\" name=\"words\" value=\"");
  for(i = 0; i < cblistnum(p_words); i++){
    if(i > 0) xmlprintf(" ");
    xmlprintf("%@", cblistval(p_words, i, NULL));
  }
  xmlprintf("\" size=\"24\" id=\"estproxy_words\" tabindex=\"1\" accesskey=\"0\" />\n");
  xmlprintf("<input type=\"submit\" value=\"Reload\""
            " id=\"estproxy_submit\" tabindex=\"2\" accesskey=\"1\" />\n");
  xmlprintf("<input type=\"hidden\" name=\"url\" value=\"%@\" id=\"estproxy_url\" />\n",
            p_url);
  xmlprintf("<input type=\"hidden\" name=\"once\" value=\"%d\" id=\"estproxy_once\" />\n",
            p_once);
  xmlprintf("</div>\n");
  xmlprintf("</form>\n");
  if(cblistnum(p_words) > 0){
    xmlprintf("<div class=\"estproxy_jumpnavi\""
              " style=\"%@ margin: 0.2em 0.8em; clear: both; float: left;\">\n", defstyle);
    xmlprintf("<span class=\"estproxy_label\">jump to:</span>\n");
    for(i = 0; i < cblistnum(p_words); i++){
      xmlprintf("<a href=\"#estproxy_word%d\" style=\"padding: 0em 0.3em; color: #111111;"
                " background: %@ none; text-decoration: none; -moz-border-radius:0.2em;\">"
                "%@</a>\n", i + 1, g_wordcolors[i%(sizeof(g_wordcolors)/sizeof(char *))],
                cblistval(p_words, i, NULL));
    }
    xmlprintf("</div>\n");
  }
  xmlprintf("<div class=\"estproxy_jumpsrc\" style=\"%@ clear: right; text-align: right;\">\n",
            defstyle);
  xmlprintf("<span class=\"estproxy_label\">original page:</span>\n");
  xmlprintf("<a href=\"%@\" style=\"color: #0011ee; text-decoration: none;\""
            " class=\"estproxy_srclink\">%64@</a>\n", p_url, p_url);
  xmlprintf("</div>\n");
  xmlprintf("</div>\n");
}


/* send a snippet string */
static void sendsnippet(const char *str){
  static CBMAP *colors = NULL;
  static CBMAP *ids = NULL;
  CBLIST *chunks, *lines;
  const char *line, *color, *id;
  char numbuf[NUMBUFSIZ], *snippet, *word, *pv;
  int i, j, len, tail;
  if(!colors){
    colors = cbmapopenex(MINIBNUM);
    cbglobalgc(colors, (void (*)(void *))cbmapclose);
    ids = cbmapopenex(MINIBNUM);
    cbglobalgc(ids, (void (*)(void *))cbmapclose);
    for(i = 0; i < cblistnum(p_words); i++){
      cbmapput(colors, cblistval(p_words, i, NULL), -1,
               g_wordcolors[i%(sizeof(g_wordcolors)/sizeof(char *))], -1, FALSE);
      len = sprintf(numbuf, "estproxy_word%d", i + 1);
      cbmapput(ids, cblistval(p_words, i, NULL), -1, numbuf, len, FALSE);
    }
  }
  if(cblistnum(p_words) < 1){
    xmlprintf("%@", str);
    return;
  }
  tail = cbstrbwmatch(str, "\n");
  chunks = cbsplit(str, -1, "\n");
  for(i = 0; i < cblistnum(chunks); i++){
    snippet = est_str_make_snippet(cblistval(chunks, i, NULL), p_words, INT_MAX, INT_MAX, 0);
    lines = cbsplit(snippet, -1, "\n");
    for(j = 0; j < cblistnum(lines); j++){
      line = cblistval(lines, j, NULL);
      if((pv = strchr(line, '\t')) != NULL){
        word = cbmemdup(line, pv - line);
        pv++;
        len = strlen(pv);
        if(!(color = cbmapget(colors, pv, len, NULL))) color = "#cccccc";
        if((id = cbmapget(ids, pv, len, NULL)) != NULL){
          xmlprintf("<strong style=\"color:#000000;background-color:%@;"
                    "-moz-border-radius:0.2em;\" id=\"%@\">%@</strong>",
                    color, id, word);
          cbmapout(ids, pv, len);
        } else {
          xmlprintf("<strong style=\"color:#000000;background-color:%@;"
                    "-moz-border-radius: 0.2em;\">%@</strong>",
                    color, word);
        }
        free(word);
      } else {
        xmlprintf("%@", line);
      }
    }
    free(snippet);
    if(i < cblistnum(chunks) - 1) xmlprintf("\n");
  }
  cblistclose(chunks);
  if(tail) xmlprintf("\n");
}


/* get the language string */
static const char *langstr(void){
  switch(g_language){
  case ESTLANGEN: return "en";
  case ESTLANGJA: return "ja";
  case ESTLANGZH: return "zh";
  case ESTLANGKO: return "ko";
  default: break;
  }
  return "en";
}


/* send the result made from document draft */
static void sendpagefromdraft(const char *buf, int size, const char *penc){
  ESTDOC *doc;
  CBMAP *kwords;
  const CBLIST *texts;
  CBLIST *names;
  const char *lang, *title, *rp;
  int i;
  doc = est_doc_new_from_draft(buf);
  if(!(lang = est_doc_attr(doc, ESTDATTRLANG))) lang = langstr();
  if(!(title = est_doc_attr(doc, ESTDATTRTITLE))){
    if((title = est_doc_attr(doc, ESTDATTRURI)) != NULL){
      if((rp = strrchr(title, '/')) != NULL && rp[1] != '\0') title = rp + 1;
    } else {
      title = "(no title)";
    }
  }
  xmlprintf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  xmlprintf("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\""
            " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
  xmlprintf("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"%@\" lang=\"%@\">\n",
            lang, lang);
  xmlprintf("<head>\n");
  xmlprintf("<meta http-equiv=\"Content-Language\" content=\"%@\" />\n", lang);
  xmlprintf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n");
  xmlprintf("<meta http-equiv=\"Content-Style-Type\" content=\"text/css\" />\n");
  xmlprintf("<link rel=\"contents\" href=\"%@\" />\n", g_scriptname);
  xmlprintf("<title>%@</title>\n", title);
  xmlprintf("<style type=\"text/css\">html {\n");
  xmlprintf("  margin: 0em 0em; padding: 0em 0em;\n");
  xmlprintf("  background: #ffffff none;\n");
  xmlprintf("}\n");
  xmlprintf("body {\n");
  xmlprintf("  margin: 1em 1em; padding: 0em 0em;\n");
  xmlprintf("  color: #111111;\n");
  xmlprintf("}\n");
  xmlprintf("div.attrlist {\n");
  xmlprintf("  margin: 0.2em 0.2em; padding: 0.2em 0.5em;\n");
  xmlprintf("  background: #eeeeee none;\n");
  xmlprintf("  -moz-border-radius: 0.4em;\n");
  xmlprintf("}\n");
  xmlprintf("span.attrname {\n");
  xmlprintf("  font-weight: bold;\n");
  xmlprintf("}\n");
  xmlprintf("p {\n");
  xmlprintf("  margin: 0.5em 1em; text-indent: 0.8em;\n");
  xmlprintf("}\n");
  xmlprintf("p.keywords {\n");
  xmlprintf("  margin: 0.5em 1.5em;\n");
  xmlprintf("  font-style: italic; color: #333333;\n");
  xmlprintf("}\n");
  xmlprintf("p.hiddentext {\n");
  xmlprintf("  margin: 0.5em 2em;\n");
  xmlprintf("  font-size: smaller; font-style: italic; color: #555555;\n");
  xmlprintf("}\n");
  xmlprintf("</style>\n");
  xmlprintf("</head>\n");
  xmlprintf("<body>\n");
  if(g_shownavi) sendnavibar();
  xmlprintf("<h1>%@</h1>\n", title);
  xmlprintf("<div class=\"attrlist\">\n");
  names = est_doc_attr_names(doc);
  for(i = 0; i < cblistnum(names); i++){
    rp = cblistval(names, i, NULL);
    xmlprintf("<div><span id=\"attrname%d\" class=\"attrname\">%@</span>:"
              " <span id=\"attrvalue%d\" class=\"attrvalue\">%@</span></div>\n",
              i + 1, rp, i + 1, est_doc_attr(doc, rp));
  }
  cblistclose(names);
  xmlprintf("</div>\n");
  if((kwords = est_doc_keywords(doc)) != NULL && cbmaprnum(kwords) > 0){
    xmlprintf("<hr />\n");
    xmlprintf("<p class=\"keywords\">");
    cbmapiterinit(kwords);
    for(i = 1; (rp = cbmapiternext(kwords, NULL)) != NULL; i++){
      if(i > 1) xmlprintf(", ");
      xmlprintf("<span id=\"keyword%d\" class=\"keyword\">%@</span>"
                " (<span id=\"keyscore%d\" class=\"keyscore\">%@</span>)",
                i, rp, i, cbmapget(kwords, rp, -1, NULL));
    }
    xmlprintf("</p>\n");
  }
  xmlprintf("<hr />\n");
  texts = est_doc_texts(doc);
  for(i = 0; i < cblistnum(texts); i++){
    rp = cblistval(texts, i, NULL);
    xmlprintf("<p class=\"text\">");
    sendsnippet(rp);
    xmlprintf("</p>\n");
  }
  rp = est_doc_hidden_texts(doc);
  if(*rp != '\0'){
    xmlprintf("<hr />\n");
    xmlprintf("<p class=\"hiddentext\">");
    sendsnippet(rp);
    xmlprintf("</p>\n");
  }
  xmlprintf("<hr />\n");
  xmlprintf("</body>\n");
  xmlprintf("</html>\n");
  est_doc_delete(doc);
}


/* send the result made from plain text */
static void sendpagefromtext(const char *buf, int size, const char *penc){
  const char *lang, *enc, *text;
  char *title, *nbuf;
  lang = langstr();
  title = est_regex_replace(p_url, ".*/([^/]*)/?$", "\\1");
  enc = penc ? penc : est_enc_name(buf, size, g_language);
  if(!strcmp(enc, "UTF-8")){
    nbuf = NULL;
    text = buf;
  } else {
    text = buf;
    nbuf = est_iconv(buf, size, enc, "UTF-8", NULL, NULL);
    if(nbuf) text = nbuf;
  }
  xmlprintf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  xmlprintf("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\""
            " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
  xmlprintf("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"%@\" lang=\"%@\">\n",
            lang, lang);
  xmlprintf("<head>\n");
  xmlprintf("<meta http-equiv=\"Content-Language\" content=\"%@\" />\n", lang);
  xmlprintf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n");
  xmlprintf("<meta http-equiv=\"Content-Style-Type\" content=\"text/css\" />\n");
  xmlprintf("<link rel=\"contents\" href=\"%@\" />\n", g_scriptname);
  xmlprintf("<title>%@</title>\n", title);
  xmlprintf("<style type=\"text/css\">html {\n");
  xmlprintf("  margin: 0em 0em; padding: 0em 0em;\n");
  xmlprintf("  background: #ffffff none;\n");
  xmlprintf("}\n");
  xmlprintf("body {\n");
  xmlprintf("  margin: 1em 0.8em; padding: 0em 0em;\n");
  xmlprintf("  color: #111111;\n");
  xmlprintf("}\n");
  xmlprintf("pre {\n");
  xmlprintf("  white-space: pre-wrap;\n");
  xmlprintf("  white-space: -moz-pre-wrap; word-wrap: break-word;\n");
  xmlprintf("}\n");
  xmlprintf("</style>\n");
  xmlprintf("</head>\n");
  xmlprintf("<body>\n");
  if(g_shownavi) sendnavibar();
  xmlprintf("<pre>");
  sendsnippet(text);
  xmlprintf("</pre>\n");
  xmlprintf("</body>\n");
  xmlprintf("</html>\n");
  free(nbuf);
  free(title);
}


/* send the result made from HTML */
static void sendpagefromhtml(const char *buf, int size, const char *penc){
  CBMAP *attrs;
  CBLIST *elems;
  const char *enc, *html, *elem, *name, *kbuf, *vbuf;
  char *nbuf, *nenc, *raw, *nurl;
  int i, esiz, ksiz, asis;
  enc = est_enc_name(buf, size, g_language);
  html = NULL;
  nbuf = NULL;
  if(!strcmp(enc, "UTF-16") || !strcmp(enc, "UTF-16BE") || !strcmp(enc, "UTF-16LE")){
    nbuf = est_iconv(buf, size, enc, "UTF-8", NULL, NULL);
  } else if(!strcmp(enc, "US-ASCII")){
    nbuf = NULL;
  } else {
    if((nenc = penc ? cbmemdup(penc, -1) : htmlenc(buf)) != NULL){
      if(cbstricmp(nenc, "UTF-8")){
        nbuf = est_iconv(buf, size, nenc, "UTF-8", NULL, NULL);
        if(!nbuf) nbuf = est_iconv(buf, size, enc, "UTF-8", NULL, NULL);
      }
      free(nenc);
    } else {
      nbuf = est_iconv(buf, size, enc, "UTF-8", NULL, NULL);
    }
  }
  if(nbuf) html = nbuf;
  if(!html) html = buf;
  asis = FALSE;
  elems = cbxmlbreak(html, FALSE);
  for(i = 0; i < CB_LISTNUM(elems); i++){
    elem = CB_LISTVAL2(elems, i, esiz);
    if(elem[0] == '<'){
      if(elem[1] == '?' || elem[1] == '!' || elem[1] == '/'){
        xmlprintf("%s", elem);
        asis = FALSE;
      } else {
        attrs = cbxmlattrs(elem);
        name = cbmapget(attrs, "", -1, NULL);
        if(!cbstricmp(name, "title") || !cbstricmp(name, "script") || !cbstricmp(name, "style"))
          asis = TRUE;
        if(!cbstricmp(name, "meta")){
          vbuf = cbmapget(attrs, "name", -1, NULL);
          if(!vbuf) vbuf = cbmapget(attrs, "Name", -1, NULL);
          if(!vbuf) vbuf = cbmapget(attrs, "NAME", -1, NULL);
          if(!vbuf) vbuf = cbmapget(attrs, "http-equiv", -1, NULL);
          if(!vbuf) vbuf = cbmapget(attrs, "Http-equiv", -1, NULL);
          if(!vbuf) vbuf = cbmapget(attrs, "Http-Equiv", -1, NULL);
          if(!vbuf) vbuf = cbmapget(attrs, "HTTP-EQUIV", -1, NULL);
          if(vbuf && !cbstricmp(vbuf, "content-type")){
            xmlprintf("<meta http-equiv=\"Content-Type\""
                      " content=\"text/html; charset=UTF-8\" />");
          } else {
            xmlprintf("%s", elem);
          }
        } else {
          xmlprintf("<%s", name);
          cbmapiterinit(attrs);
          while((kbuf = cbmapiternext(attrs, &ksiz)) != NULL){
            if(ksiz < 1) continue;
            vbuf = cbmapget(attrs, kbuf, ksiz, NULL);
            if(!cbstricmp(kbuf, "src") || !cbstricmp(kbuf, "href") || !cbstricmp(kbuf, "data")){
              nurl = cburlresolve(p_url, vbuf);
              if(p_once){
                xmlprintf(" %@=\"%@\"", kbuf, nurl);
              } else {
                xmlprintf(" %@=\"%s?url=%?\"", kbuf, g_scriptname, nurl);
              }
              free(nurl);
            } else if(!cbstricmp(kbuf, "action")){
              nurl = cburlresolve(p_url, vbuf);
              xmlprintf(" %@=\"%@\"", kbuf, nurl);
              free(nurl);
            } else if(strchr(vbuf, '&')){
              raw = htmlrawtext(vbuf);
              xmlprintf(" %@=\"%@\"", kbuf, raw);
              free(raw);
            } else {
              xmlprintf(" %@=\"%@\"", kbuf, vbuf);
            }
          }
          if(cbstrbwmatch(elem, "/>")){
            xmlprintf(" />");
          } else {
            xmlprintf(">");
          }
        }
        if(!cbstricmp(name, "body") && g_shownavi){
          xmlprintf("\n");
          sendnavibar();
        }
        cbmapclose(attrs);
      }
    } else if(asis){
      xmlprintf("%s", elem);
    } else {
      if(strchr(elem, '&')){
        raw = htmlrawtext(elem);
        sendsnippet(raw);
        free(raw);
      } else {
        sendsnippet(elem);
      }
    }
  }
  cblistclose(elems);
  free(nbuf);
}


/* get the encoding of an HTML string */
static char *htmlenc(const char *str){
  CBLIST *elems;
  CBMAP *attrs;
  const char *elem, *equiv, *content;
  char *enc, *pv;
  int i;
  elems = cbxmlbreak(str, TRUE);
  for(i = 0; i < CB_LISTNUM(elems); i++){
    elem = CB_LISTVAL(elems, i);
    if(elem[0] != '<' || !cbstrfwimatch(elem, "<meta")) continue;
    enc = NULL;
    attrs = cbxmlattrs(elem);
    equiv = cbmapget(attrs, "http-equiv", -1, NULL);
    if(!equiv) equiv = cbmapget(attrs, "HTTP-EQUIV", -1, NULL);
    if(!equiv) equiv = cbmapget(attrs, "Http-Equiv", -1, NULL);
    if(!equiv) equiv = cbmapget(attrs, "Http-equiv", -1, NULL);
    if(equiv && !cbstricmp(equiv, "Content-Type")){
      content = cbmapget(attrs, "content", -1, NULL);
      if(!content) content = cbmapget(attrs, "Content", -1, NULL);
      if(!content) content = cbmapget(attrs, "CONTENT", -1, NULL);
      if(content && ((pv = strstr(content, "charset")) != NULL ||
                     (pv = strstr(content, "Charset")) != NULL ||
                     (pv = strstr(content, "CHARSET")) != NULL)){
        enc = cbmemdup(pv + 8, -1);
        if((pv = strchr(enc, ';')) != NULL || (pv = strchr(enc, '\r')) != NULL ||
           (pv = strchr(enc, '\n')) != NULL || (pv = strchr(enc, ' ')) != NULL) *pv = '\0';
      }
    }
    cbmapclose(attrs);
    if(enc){
      CB_LISTCLOSE(elems);
      return enc;
    }
  }
  CB_LISTCLOSE(elems);
  return NULL;
}


/* unescape entity references of HTML */
static char *htmlrawtext(const char *html){
  static const char *pairs[] = {
    "&amp;", "&", "&lt;", "<", "&gt;", ">", "&quot;", "\"", "&apos;", "'",
    "&nbsp;", "\xc2\xa0", "&iexcl;", "\xc2\xa1", "&cent;", "\xc2\xa2",
    "&pound;", "\xc2\xa3", "&curren;", "\xc2\xa4", "&yen;", "\xc2\xa5",
    "&brvbar;", "\xc2\xa6", "&sect;", "\xc2\xa7", "&uml;", "\xc2\xa8",
    "&copy;", "\xc2\xa9", "&ordf;", "\xc2\xaa", "&laquo;", "\xc2\xab",
    "&not;", "\xc2\xac", "&shy;", "\xc2\xad", "&reg;", "\xc2\xae",
    "&macr;", "\xc2\xaf", "&deg;", "\xc2\xb0", "&plusmn;", "\xc2\xb1",
    "&sup2;", "\xc2\xb2", "&sup3;", "\xc2\xb3", "&acute;", "\xc2\xb4",
    "&micro;", "\xc2\xb5", "&para;", "\xc2\xb6", "&middot;", "\xc2\xb7",
    "&cedil;", "\xc2\xb8", "&sup1;", "\xc2\xb9", "&ordm;", "\xc2\xba",
    "&raquo;", "\xc2\xbb", "&frac14;", "\xc2\xbc", "&frac12;", "\xc2\xbd",
    "&frac34;", "\xc2\xbe", "&iquest;", "\xc2\xbf", "&Agrave;", "\xc3\x80",
    "&Aacute;", "\xc3\x81", "&Acirc;", "\xc3\x82", "&Atilde;", "\xc3\x83",
    "&Auml;", "\xc3\x84", "&Aring;", "\xc3\x85", "&AElig;", "\xc3\x86",
    "&Ccedil;", "\xc3\x87", "&Egrave;", "\xc3\x88", "&Eacute;", "\xc3\x89",
    "&Ecirc;", "\xc3\x8a", "&Euml;", "\xc3\x8b", "&Igrave;", "\xc3\x8c",
    "&Iacute;", "\xc3\x8d", "&Icirc;", "\xc3\x8e", "&Iuml;", "\xc3\x8f",
    "&ETH;", "\xc3\x90", "&Ntilde;", "\xc3\x91", "&Ograve;", "\xc3\x92",
    "&Oacute;", "\xc3\x93", "&Ocirc;", "\xc3\x94", "&Otilde;", "\xc3\x95",
    "&Ouml;", "\xc3\x96", "&times;", "\xc3\x97", "&Oslash;", "\xc3\x98",
    "&Ugrave;", "\xc3\x99", "&Uacute;", "\xc3\x9a", "&Ucirc;", "\xc3\x9b",
    "&Uuml;", "\xc3\x9c", "&Yacute;", "\xc3\x9d", "&THORN;", "\xc3\x9e",
    "&szlig;", "\xc3\x9f", "&agrave;", "\xc3\xa0", "&aacute;", "\xc3\xa1",
    "&acirc;", "\xc3\xa2", "&atilde;", "\xc3\xa3", "&auml;", "\xc3\xa4",
    "&aring;", "\xc3\xa5", "&aelig;", "\xc3\xa6", "&ccedil;", "\xc3\xa7",
    "&egrave;", "\xc3\xa8", "&eacute;", "\xc3\xa9", "&ecirc;", "\xc3\xaa",
    "&euml;", "\xc3\xab", "&igrave;", "\xc3\xac", "&iacute;", "\xc3\xad",
    "&icirc;", "\xc3\xae", "&iuml;", "\xc3\xaf", "&eth;", "\xc3\xb0",
    "&ntilde;", "\xc3\xb1", "&ograve;", "\xc3\xb2", "&oacute;", "\xc3\xb3",
    "&ocirc;", "\xc3\xb4", "&otilde;", "\xc3\xb5", "&ouml;", "\xc3\xb6",
    "&divide;", "\xc3\xb7", "&oslash;", "\xc3\xb8", "&ugrave;", "\xc3\xb9",
    "&uacute;", "\xc3\xba", "&ucirc;", "\xc3\xbb", "&uuml;", "\xc3\xbc",
    "&yacute;", "\xc3\xbd", "&thorn;", "\xc3\xbe", "&yuml;", "\xc3\xbf",
    "&fnof;", "\xc6\x92", "&Alpha;", "\xce\x91", "&Beta;", "\xce\x92",
    "&Gamma;", "\xce\x93", "&Delta;", "\xce\x94", "&Epsilon;", "\xce\x95",
    "&Zeta;", "\xce\x96", "&Eta;", "\xce\x97", "&Theta;", "\xce\x98",
    "&Iota;", "\xce\x99", "&Kappa;", "\xce\x9a", "&Lambda;", "\xce\x9b",
    "&Mu;", "\xce\x9c", "&Nu;", "\xce\x9d", "&Xi;", "\xce\x9e",
    "&Omicron;", "\xce\x9f", "&Pi;", "\xce\xa0", "&Rho;", "\xce\xa1",
    "&Sigma;", "\xce\xa3", "&Tau;", "\xce\xa4", "&Upsilon;", "\xce\xa5",
    "&Phi;", "\xce\xa6", "&Chi;", "\xce\xa7", "&Psi;", "\xce\xa8",
    "&Omega;", "\xce\xa9", "&alpha;", "\xce\xb1", "&beta;", "\xce\xb2",
    "&gamma;", "\xce\xb3", "&delta;", "\xce\xb4", "&epsilon;", "\xce\xb5",
    "&zeta;", "\xce\xb6", "&eta;", "\xce\xb7", "&theta;", "\xce\xb8",
    "&iota;", "\xce\xb9", "&kappa;", "\xce\xba", "&lambda;", "\xce\xbb",
    "&mu;", "\xce\xbc", "&nu;", "\xce\xbd", "&xi;", "\xce\xbe",
    "&omicron;", "\xce\xbf", "&pi;", "\xcf\x80", "&rho;", "\xcf\x81",
    "&sigmaf;", "\xcf\x82", "&sigma;", "\xcf\x83", "&tau;", "\xcf\x84",
    "&upsilon;", "\xcf\x85", "&phi;", "\xcf\x86", "&chi;", "\xcf\x87",
    "&psi;", "\xcf\x88", "&omega;", "\xcf\x89", "&thetasym;", "\xcf\x91",
    "&upsih;", "\xcf\x92", "&piv;", "\xcf\x96", "&bull;", "\xe2\x80\xa2",
    "&hellip;", "\xe2\x80\xa6", "&prime;", "\xe2\x80\xb2", "&Prime;", "\xe2\x80\xb3",
    "&oline;", "\xe2\x80\xbe", "&frasl;", "\xe2\x81\x84", "&weierp;", "\xe2\x84\x98",
    "&image;", "\xe2\x84\x91", "&real;", "\xe2\x84\x9c", "&trade;", "\xe2\x84\xa2",
    "&alefsym;", "\xe2\x84\xb5", "&larr;", "\xe2\x86\x90", "&uarr;", "\xe2\x86\x91",
    "&rarr;", "\xe2\x86\x92", "&darr;", "\xe2\x86\x93", "&harr;", "\xe2\x86\x94",
    "&crarr;", "\xe2\x86\xb5", "&lArr;", "\xe2\x87\x90", "&uArr;", "\xe2\x87\x91",
    "&rArr;", "\xe2\x87\x92", "&dArr;", "\xe2\x87\x93", "&hArr;", "\xe2\x87\x94",
    "&forall;", "\xe2\x88\x80", "&part;", "\xe2\x88\x82", "&exist;", "\xe2\x88\x83",
    "&empty;", "\xe2\x88\x85", "&nabla;", "\xe2\x88\x87", "&isin;", "\xe2\x88\x88",
    "&notin;", "\xe2\x88\x89", "&ni;", "\xe2\x88\x8b", "&prod;", "\xe2\x88\x8f",
    "&sum;", "\xe2\x88\x91", "&minus;", "\xe2\x88\x92", "&lowast;", "\xe2\x88\x97",
    "&radic;", "\xe2\x88\x9a", "&prop;", "\xe2\x88\x9d", "&infin;", "\xe2\x88\x9e",
    "&ang;", "\xe2\x88\xa0", "&and;", "\xe2\x88\xa7", "&or;", "\xe2\x88\xa8",
    "&cap;", "\xe2\x88\xa9", "&cup;", "\xe2\x88\xaa", "&int;", "\xe2\x88\xab",
    "&there4;", "\xe2\x88\xb4", "&sim;", "\xe2\x88\xbc", "&cong;", "\xe2\x89\x85",
    "&asymp;", "\xe2\x89\x88", "&ne;", "\xe2\x89\xa0", "&equiv;", "\xe2\x89\xa1",
    "&le;", "\xe2\x89\xa4", "&ge;", "\xe2\x89\xa5", "&sub;", "\xe2\x8a\x82",
    "&sup;", "\xe2\x8a\x83", "&nsub;", "\xe2\x8a\x84", "&sube;", "\xe2\x8a\x86",
    "&supe;", "\xe2\x8a\x87", "&oplus;", "\xe2\x8a\x95", "&otimes;", "\xe2\x8a\x97",
    "&perp;", "\xe2\x8a\xa5", "&sdot;", "\xe2\x8b\x85", "&lceil;", "\xe2\x8c\x88",
    "&rceil;", "\xe2\x8c\x89", "&lfloor;", "\xe2\x8c\x8a", "&rfloor;", "\xe2\x8c\x8b",
    "&lang;", "\xe2\x8c\xa9", "&rang;", "\xe2\x8c\xaa", "&loz;", "\xe2\x97\x8a",
    "&spades;", "\xe2\x99\xa0", "&clubs;", "\xe2\x99\xa3", "&hearts;", "\xe2\x99\xa5",
    "&diams;", "\xe2\x99\xa6", "&OElig;", "\xc5\x92", "&oelig;", "\xc5\x93",
    "&Scaron;", "\xc5\xa0", "&scaron;", "\xc5\xa1", "&Yuml;", "\xc5\xb8",
    "&circ;", "\xcb\x86", "&tilde;", "\xcb\x9c", "&ensp;", "\xe2\x80\x82",
    "&emsp;", "\xe2\x80\x83", "&thinsp;", "\xe2\x80\x89", "&zwnj;", "\xe2\x80\x8c",
    "&zwj;", "\xe2\x80\x8d", "&lrm;", "\xe2\x80\x8e", "&rlm;", "\xe2\x80\x8f",
    "&ndash;", "\xe2\x80\x93", "&mdash;", "\xe2\x80\x94", "&lsquo;", "\xe2\x80\x98",
    "&rsquo;", "\xe2\x80\x99", "&sbquo;", "\xe2\x80\x9a", "&ldquo;", "\xe2\x80\x9c",
    "&rdquo;", "\xe2\x80\x9d", "&bdquo;", "\xe2\x80\x9e", "&dagger;", "\xe2\x80\xa0",
    "&Dagger;", "\xe2\x80\xa1", "&permil;", "\xe2\x80\xb0", "&lsaquo;", "\xe2\x80\xb9",
    "&rsaquo;", "\xe2\x80\xba", "&euro;", "\xe2\x82\xac",
    NULL
  };
  char *raw, *wp, buf[2], *tmp;
  int i, j, hit, num, tsiz;
  CB_MALLOC(raw, strlen(html) * 3 + 1);
  wp = raw;
  while(*html != '\0'){
    if(*html == '&'){
      if(*(html + 1) == '#'){
        if(*(html + 2) == 'x' || *(html + 2) == 'X'){
          num = strtol(html + 3, NULL, 16);
        } else {
          num = atoi(html + 2);
        }
        buf[0] = num / 256;
        buf[1] = num % 256;
        if((tmp = est_uconv_out(buf, 2, &tsiz)) != NULL){
          for(j = 0; j < tsiz; j++){
            *wp = ((unsigned char *)tmp)[j];
            wp++;
          }
          free(tmp);
        }
        while(*html != ';' && *html != ' ' && *html != '\n' && *html != '\0'){
          html++;
        }
        if(*html == ';') html++;
      } else {
        hit = FALSE;
        for(i = 0; pairs[i] != NULL; i += 2){
          if(cbstrfwmatch(html, pairs[i])){
            wp += sprintf(wp, "%s", pairs[i+1]);
            html += strlen(pairs[i]);
            hit = TRUE;
            break;
          }
        }
        if(!hit){
          *wp = *html;
          wp++;
          html++;
        }
      }
    } else {
      *wp = *html;
      wp++;
      html++;
    }
  }
  *wp = '\0';
  return raw;
}


/* send the result made from MIME */
static void sendpagefrommime(const char *buf, int size, const char *penc){
  CBMAP *attrs, *pattrs;
  CBLIST *parts;
  const char *val, *lang, *bound, *part;
  char *body, *title, *raw, *pbody;
  int i, bsiz, psiz, pbsiz;
  lang = langstr();
  attrs = cbmapopenex(MINIBNUM);
  body = cbmimebreak(buf, size, attrs, &bsiz);
  if((val = cbmapget(attrs, "subject", -1, NULL)) != NULL){
    title = mimestr(val);
  } else {
    title = cbmemdup("(no title)", -1);
  }
  xmlprintf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  xmlprintf("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\""
            " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
  xmlprintf("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"%@\" lang=\"%@\">\n",
            lang, lang);
  xmlprintf("<head>\n");
  xmlprintf("<meta http-equiv=\"Content-Language\" content=\"%@\" />\n", lang);
  xmlprintf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n");
  xmlprintf("<meta http-equiv=\"Content-Style-Type\" content=\"text/css\" />\n");
  xmlprintf("<link rel=\"contents\" href=\"%@\" />\n", g_scriptname);
  xmlprintf("<title>%@</title>\n", title);
  xmlprintf("<style type=\"text/css\">html {\n");
  xmlprintf("  margin: 0em 0em; padding: 0em 0em;\n");
  xmlprintf("  background: #ffffff none;\n");
  xmlprintf("}\n");
  xmlprintf("body {\n");
  xmlprintf("  margin: 1em 1em; padding: 0em 0em;\n");
  xmlprintf("  color: #111111;\n");
  xmlprintf("}\n");
  xmlprintf("div.attrlist {\n");
  xmlprintf("  margin: 0.2em 0.2em; padding: 0.2em 0.5em;\n");
  xmlprintf("  background: #eeeeee none;\n");
  xmlprintf("  -moz-border-radius: 0.4em;\n");
  xmlprintf("}\n");
  xmlprintf("span.attrname {\n");
  xmlprintf("  font-weight: bold;\n");
  xmlprintf("}\n");
  xmlprintf("pre {\n");
  xmlprintf("  margin: 1em 1em; white-space: pre-wrap;\n");
  xmlprintf("  white-space: -moz-pre-wrap; word-wrap: break-word;\n");
  xmlprintf("}\n");
  xmlprintf("div.note {\n");
  xmlprintf("  color: #888888; font-size: small;\n");
  xmlprintf("}\n");
  xmlprintf("</style>\n");
  xmlprintf("</head>\n");
  xmlprintf("<body>\n");
  if(g_shownavi) sendnavibar();
  xmlprintf("<h1>%@</h1>\n", title);
  xmlprintf("<div class=\"attrlist\">\n");
  if((val = cbmapget(attrs, "from", -1, NULL)) != NULL){
    raw = mimestr(val);
    xmlprintf("<div><span class=\"attrname\">From</span>:"
              " <span id=\"from\" class=\"attrvalue\">%@</span></div>\n", raw);
    free(raw);
  }
  if((val = cbmapget(attrs, "to", -1, NULL)) != NULL){
    raw = mimestr(val);
    xmlprintf("<div><span class=\"attrname\">To</span>:"
              " <span id=\"to\" class=\"attrvalue\">%@</span></div>\n", raw);
    free(raw);
  }
  if((val = cbmapget(attrs, "cc", -1, NULL)) != NULL){
    raw = mimestr(val);
    xmlprintf("<div><span class=\"attrname\">Cc</span>:"
              " <span id=\"cc\" class=\"attrvalue\">%@</span></div>\n", raw);
    free(raw);
  }
  if((val = cbmapget(attrs, "subject", -1, NULL)) != NULL){
    raw = mimestr(val);
    xmlprintf("<div><span class=\"attrname\">Subject</span>:"
              " <span id=\"subject\" class=\"attrvalue\">%@</span></div>\n", raw);
    free(raw);
  }
  if((val = cbmapget(attrs, "date", -1, NULL)) != NULL){
    raw = mimestr(val);
    xmlprintf("<div><span class=\"attrname\">Date</span>:"
              " <span id=\"date\" class=\"attrvalue\">%@</span></div>\n", raw);
    free(raw);
  }
  if((val = cbmapget(attrs, "x-mailer", -1, NULL)) != NULL){
    raw = mimestr(val);
    xmlprintf("<div><span class=\"attrname\">X-Mailer</span>:"
              " <span id=\"xmailer\" class=\"attrvalue\">%@</span></div>\n", raw);
    free(raw);
  }
  xmlprintf("</div>\n");
  if((val = cbmapget(attrs, "TYPE", -1, NULL)) != NULL && cbstrfwimatch(val, "multipart/")){
    if((bound = cbmapget(attrs, "BOUNDARY", -1, NULL)) != NULL){
      parts = cbmimeparts(body, bsiz, bound);
      for(i = 0; i < CB_LISTNUM(parts) && i < 8; i++){
        part = cblistval(parts, i, &psiz);
        pattrs = cbmapopenex(MINIBNUM);
        pbody = cbmimebreak(part, psiz, pattrs, &pbsiz);
        sendmimepart(pbody, pbsiz, pattrs, NULL);
        free(pbody);
        cbmapclose(pattrs);
      }
      cblistclose(parts);
    }
  } else {
    sendmimepart(body, bsiz, attrs, penc);
  }
  xmlprintf("<hr />\n");
  xmlprintf("</body>\n");
  xmlprintf("</html>\n");
  free(title);
  free(body);
  cbmapclose(attrs);
}


/* send a part of MIME */
static void sendmimepart(const char *body, int bsiz, CBMAP *attrs, const char *penc){
  const char *val;
  char *tbuf, *ebuf, *cbuf;
  int tsiz, esiz, csiz;
  tbuf = NULL;
  ebuf = NULL;
  cbuf = NULL;
  val = cbmapget(attrs, "content-transfer-encoding", -1, NULL);
  if(val && cbstrfwimatch(val, "base64")){
    tbuf = cbbasedecode(body, &tsiz);
    body = tbuf;
    bsiz = tsiz;
  } else if(val && cbstrfwimatch(val, "quoted-printable")){
    tbuf = cbquotedecode(body, &tsiz);
    body = tbuf;
    bsiz = tsiz;
  }
  val = cbmapget(attrs, "content-encoding", -1, NULL);
  if(val && (cbstrfwimatch(val, "x-gzip") || cbstrfwimatch(val, "gzip")) &&
     (ebuf = cbgzdecode(body, bsiz, &esiz)) != NULL){
    body = ebuf;
    bsiz = esiz;
  } else if(val && (cbstrfwimatch(val, "x-deflate") || cbstrfwimatch(val, "deflate")) &&
            (ebuf = cbinflate(body, bsiz, &esiz)) != NULL){
    body = ebuf;
    bsiz = esiz;
  }
  if(penc && (cbuf = est_iconv(body, bsiz, penc, "UTF-8", &csiz, NULL)) != NULL){
    body = cbuf;
    bsiz = csiz;
  } else if((val = cbmapget(attrs, "CHARSET", -1, NULL)) != NULL &&
            (cbuf = est_iconv(body, bsiz, val, "UTF-8", &csiz, NULL)) != NULL){
    body = cbuf;
    bsiz = csiz;
  }
  xmlprintf("<hr />\n");
  if(!(val = cbmapget(attrs, "TYPE", -1, NULL))) val = "text/plain";
  if(cbstrfwimatch(val, "text/")){
    if(!cbstrfwimatch(val, "text/plain")) xmlprintf("<div class=\"note\">(%@)</div>\n", val);
    xmlprintf("<pre>");
    sendsnippet(body);
    xmlprintf("</pre>\n");
  } else {
    xmlprintf("<div class=\"note\">(%@; not shown)</div>\n", val);
  }
  free(cbuf);
  free(ebuf);
  free(tbuf);
}


/* convert MIME encoded string into plain string */
static char *mimestr(const char *mime){
  char enc[64], *ebuf, *rbuf;
  ebuf = cbmimedecode(mime, enc);
  rbuf = est_iconv(ebuf, -1, enc, "UTF-8", NULL, NULL);
  free(ebuf);
  return rbuf ? rbuf : cbmemdup(mime, -1);
}


/* send the result made with an outer command */
static void sendpagewithxcmd(const char *buf, int size, const char *penc, const char *cmd){
  const char *tmpdir, *pv, *ext, *fmt;
  char iname[URIBUFSIZ], oname[URIBUFSIZ], cbuf[URIBUFSIZ], *rbuf;
  int rnd, pid, rsiz;
  struct stat sbuf;
  if(ESTPATHCHR == '/' && stat("/tmp", &sbuf) == 0){
    tmpdir = "/tmp";
  } else if(ESTPATHCHR == '\\' &&
            ((pv = getenv("TMP")) != NULL || (pv = getenv("TEMP")) != NULL) &&
            stat(pv, &sbuf) == 0){
    tmpdir = pv;
  } else {
    tmpdir = ESTCDIRSTR;
  }
  ext = NULL;
  if(!(pv = strrchr(p_url, ESTPATHCHR))) pv = p_url;
  if((pv = strrchr(pv, ESTEXTCHR)) != NULL) ext = pv;
  if(!ext || strlen(ext) >= 32 || strchr(ext, '"') || strchr(ext, '\\')) ext = "";
  rnd = dpouterhash(p_url, -1) & 0xffff;
  pid = (int)getpid() & 0xffff;
  sprintf(iname, "%s%cxcmd-in-%04X%04X%s", tmpdir, ESTPATHCHR, pid, rnd, ext);
  sprintf(oname, "%s%cxcmd-out-%04X%04X%cest", tmpdir, ESTPATHCHR, pid, rnd, ESTEXTCHR);
  fmt = DRAFTCMD;
  if(cbstrfwmatch(cmd, "T@")){
    fmt = TEXTCMD;
    cmd += 2;
  } else if(cbstrfwmatch(cmd, "H@")){
    fmt = HTMLCMD;
    cmd += 2;
  } else if(cbstrfwmatch(cmd, "M@")){
    fmt = MIMECMD;
    cmd += 2;
  }
  cbwritefile(iname, buf, size);
  sprintf(cbuf, "%s \"%s\" \"%s\"", cmd, iname, oname);
  system(cbuf);
  if((rbuf = cbreadfile(oname, &rsiz)) != NULL){
    if(fmt == DRAFTCMD){
      sendpagefromdraft(rbuf, rsiz, NULL);
    } else if(fmt == TEXTCMD){
      sendpagefromtext(rbuf, rsiz, NULL);
    } else if(fmt == HTMLCMD){
      sendpagefromhtml(rbuf, rsiz, NULL);
    } else if(fmt == MIMECMD){
      sendpagefrommime(rbuf, rsiz, NULL);
    }
    free(rbuf);
  }
  unlink(oname);
  unlink(iname);
}



/* END OF FILE */
