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


#include "estraier_Condition.h"
#include "myconf.h"



/*************************************************************************************************
 * public objects
 *************************************************************************************************/


/* set_phrase */
JNIEXPORT void JNICALL
Java_estraier_Condition_set_1phrase(JNIEnv *env, jobject obj, jstring phrase){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jboolean icphrase;
  const char *tphrase;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(!phrase || !isinstanceof(env, phrase, CLSSTRING)){
    throwillarg(env);
    return;
  }
  if(!(tphrase = (*env)->GetStringUTFChars(env, phrase, &icphrase))){
    throwoutmem(env);
    return;
  }
  est_cond_set_phrase((ESTCOND *)(PTRNUM)coreptr, tphrase);
  if(icphrase == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, phrase, tphrase);
}


/* add_attr */
JNIEXPORT void JNICALL
Java_estraier_Condition_add_1attr(JNIEnv *env, jobject obj, jstring expr){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jboolean icexpr;
  const char *texpr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(!expr || !isinstanceof(env, expr, CLSSTRING)){
    throwillarg(env);
    return;
  }
  if(!(texpr = (*env)->GetStringUTFChars(env, expr, &icexpr))){
    throwoutmem(env);
    return;
  }
  est_cond_add_attr((ESTCOND *)(PTRNUM)coreptr, texpr);
  if(icexpr == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, expr, texpr);
}


/* set_order */
JNIEXPORT void JNICALL
Java_estraier_Condition_set_1order(JNIEnv *env, jobject obj, jstring expr){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jboolean icexpr;
  const char *texpr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(!expr || !isinstanceof(env, expr, CLSSTRING)){
    throwillarg(env);
    return;
  }
  if(!(texpr = (*env)->GetStringUTFChars(env, expr, &icexpr))){
    throwoutmem(env);
    return;
  }
  est_cond_set_order((ESTCOND *)(PTRNUM)coreptr, texpr);
  if(icexpr == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, expr, texpr);
}


/* set_max */
JNIEXPORT void JNICALL
Java_estraier_Condition_set_1max(JNIEnv *env, jobject obj, jint max){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(max < 0){
    throwillarg(env);
    return;
  }
  est_cond_set_max((ESTCOND *)(PTRNUM)coreptr, max);
}


/* set_skip */
JNIEXPORT void JNICALL
Java_estraier_Condition_set_1skip(JNIEnv *env, jobject obj, jint skip){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(skip < 0){
    throwillarg(env);
    return;
  }
  est_cond_set_skip((ESTCOND *)(PTRNUM)coreptr, skip);
}


/* set_options */
JNIEXPORT void JNICALL
Java_estraier_Condition_set_1options(JNIEnv *env, jobject obj, jint options){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  est_cond_set_options((ESTCOND *)(PTRNUM)coreptr, options);
}


/* set_auxiliary */
JNIEXPORT void JNICALL
Java_estraier_Condition_set_1auxiliary(JNIEnv *env, jobject obj, jint min){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  est_cond_set_auxiliary((ESTCOND *)(PTRNUM)coreptr, min);
}


/* set_eclipse */
JNIEXPORT void JNICALL
Java_estraier_Condition_set_1eclipse(JNIEnv *env, jobject obj, jdouble limit){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  est_cond_set_eclipse((ESTCOND *)(PTRNUM)coreptr, limit);
}


/* set_distinct */
JNIEXPORT void JNICALL
Java_estraier_Condition_set_1distinct(JNIEnv *env, jobject obj, jstring name){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jboolean icname;
  const char *tname;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(!name || !isinstanceof(env, name, CLSSTRING)){
    throwillarg(env);
    return;
  }
  if(!(tname = (*env)->GetStringUTFChars(env, name, &icname))){
    throwoutmem(env);
    return;
  }
  est_cond_set_distinct((ESTCOND *)(PTRNUM)coreptr, tname);
  if(icname == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, name, tname);
}


/* set_mask */
JNIEXPORT void JNICALL
Java_estraier_Condition_set_1mask(JNIEnv *env, jobject obj, jint mask){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  est_cond_set_mask((ESTCOND *)(PTRNUM)coreptr, mask);
}


/* initialize */
JNIEXPORT void JNICALL
Java_estraier_Condition_initialize(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  ESTCOND *cond;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  cond = est_cond_new();
  est_cond_set_options(cond, ESTCONDSCFB);
  (*env)->SetLongField(env, obj, fid, (PTRNUM)cond);
}


/* destroy */
JNIEXPORT void JNICALL
Java_estraier_Condition_destroy(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  est_cond_delete((ESTCOND *)(PTRNUM)coreptr);
}



/* END OF FILE */
