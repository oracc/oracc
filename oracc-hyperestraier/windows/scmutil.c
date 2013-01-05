/*************************************************************************************************
 * Utility for the Win32 service control manager
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


#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


/* global variables */
const char *g_progname = NULL;


/* function prototypes */
int main(int argc, char **argv);
static void printferror(const char *format, ...);
static void printfinfo(const char *format, ...);
static void usage(void);
static int runinstall(int argc, char **argv);
static int rununinstall(int argc, char **argv);
static int procinstall(const char *name, const char *label, const char *path);
static int procuninstall(const char *name);


/* main routine */
int main(int argc, char **argv){
  int rv;
  g_progname = argv[0];
  if(argc < 2) usage();
  rv = 0;
  if(!strcmp(argv[1], "install")){
    rv = runinstall(argc, argv);
  } else if(!strcmp(argv[1], "uninstall")){
    rv = rununinstall(argc, argv);
  } else {
    usage();
  }
  return rv;
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


/* print the usage and exit */
static void usage(void){
  fprintf(stderr, "%s: utility for the Win32 service control manager\n", g_progname);
  fprintf(stderr, "\n");
  fprintf(stderr, "usage:\n");
  fprintf(stderr, "  %s install name label command ...\n", g_progname);
  fprintf(stderr, "  %s uninstall name\n", g_progname);
  fprintf(stderr, "\n");
  exit(1);
}


/* parse arguments of the install command */
static int runinstall(int argc, char **argv){
  char *name, *label, path[8192], *wp;
  int i, rv;
  name = NULL;
  label = NULL;
  wp = path;
  for(i = 2; i < argc; i++){
    if(!name && argv[i][0] == '-'){
      usage();
    } else if(!name){
      name = argv[i];
    } else if(!label){
      label = argv[i];
    } else {
      if(wp > path) wp += sprintf(wp, " ");
      wp += sprintf(wp, "%s", argv[i]);
    }
  }
  *wp = '\0';
  if(!name || !label) usage();
  rv = procinstall(name, label, path);
  return rv;
}


/* parse arguments of the uninstall command */
static int rununinstall(int argc, char **argv){
  char *name;
  int i, rv;
  name = NULL;
  for(i = 2; i < argc; i++){
    if(!name && argv[i][0] == '-'){
      usage();
    } else if(!name){
      name = argv[i];
    } else {
      usage();
    }
  }
  if(!name);
  rv = procuninstall(name);
  return rv;
}


/* perform the install command */
static int procinstall(const char *name, const char *label, const char *path){
  SC_HANDLE scm, svc;
  printfinfo("installing %s ...", name);
  if(!(scm = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE))){
    printferror("the SCM could not open: %d", GetLastError());
    return 1;
  }
  if(!(svc = CreateService(scm, name, label, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
                           SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, path,
                           0, 0, 0, 0, 0))){
    printferror("the new service could not be created: %d", GetLastError());
    CloseServiceHandle(scm);
    return 1;
  }
  CloseServiceHandle(svc);
  CloseServiceHandle(scm);
  printfinfo("done");
  return 0;
}


/* perform the uninstall command */
static int procuninstall(const char *name){
  SC_HANDLE scm, svc;
  SERVICE_STATUS st;
  printfinfo("uninstalling %s ...", name);
  if(!(scm = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE))){
    printferror("the SCM could not open: %d", GetLastError());
    return 1;
  }
  if(!(svc = OpenService(scm, name, SERVICE_ALL_ACCESS | DELETE))){
    printferror("the service could not open: %d", GetLastError());
    CloseServiceHandle(scm);
    return 1;
  }
  if(!QueryServiceStatus(svc, &st)){
    printferror("the service did not finish successfully: %d", GetLastError());
    CloseServiceHandle(svc);
    CloseServiceHandle(scm);
    return 1;
  }
  if(st.dwCurrentState != SERVICE_STOPPED){
    if(!ControlService(svc, SERVICE_CONTROL_STOP, &st)){
      printferror("the service did not finish successfully: %d", GetLastError());
      CloseServiceHandle(svc);
      CloseServiceHandle(scm);
      return 1;
    }
    Sleep(500);
  }
  if(!DeleteService(svc)){
    printferror("the service could not be deleted: %d", GetLastError());
    CloseServiceHandle(svc);
    CloseServiceHandle(scm);
    return 1;
  }
  CloseServiceHandle(svc);
  CloseServiceHandle(scm);
  printfinfo("done");
  return 0;
}



/* END OF FILE */
