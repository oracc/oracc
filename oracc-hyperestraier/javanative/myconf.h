/*************************************************************************************************
 * Java binding of Hyper Estraier
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


#ifndef _MYCONF_H                        /* duplication check */
#define _MYCONF_H


#include <jni.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <estraier.h>
#include <estmtdb.h>
#include <cabin.h>



/*************************************************************************************************
 * common settings
 *************************************************************************************************/


#undef TRUE
#define TRUE           1
#undef FALSE
#define FALSE          0

#if SIZEOF_VOID_P == SIZEOF_INT
#define PTRNUM         jint
#else
#define PTRNUM         jlong
#endif

#define CLSOBJECT      "java/lang/Object"
#define CLSSTRING      "java/lang/String"
#define CLSLIST        "java/util/List"
#define CLSARRAYLIST   "java/util/ArrayList"
#define CLSMAP         "java/util/Map"
#define CLSLHMAP       "java/util/LinkedHashMap"
#define CLSITERATOR    "java/util/Iterator"
#define CLSSET         "java/util/Set"
#define CLSEILLARG     "java/lang/IllegalArgumentException"
#define CLSEOUTMEM     "java/lang/OutOfMemoryError"
#define CLSDOC         "estraier/Document"
#define CLSCOND        "estraier/Condition"
#define CLSDB          "estraier/Database"
#define CLSDBINFO      "estraier/DatabaseInformer"
#define CLSRES         "estraier/Result"


int isinstanceof(JNIEnv *env, jobject obj, const char *name);


void throwexception(JNIEnv *env, const char *name, const char *message);


void throwoutmem(JNIEnv *env);


void throwillarg(JNIEnv *env);


jobject cblisttoobj(JNIEnv *env, const CBLIST *list);


CBLIST *objtocblist(JNIEnv *env, jobject obj);


jobject cbmaptoobj(JNIEnv *env, CBMAP *map);


CBMAP *objtocbmap(JNIEnv *env, jobject obj);



#endif                                   /* duplication check */


/* END OF FILE */
