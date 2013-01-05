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


#include "estraier_Result.h"
#include "myconf.h"



/*************************************************************************************************
 * public objects
 *************************************************************************************************/


/* get_score */
JNIEXPORT jint JNICALL
Java_estraier_Result_get_1score(JNIEnv *env, jobject obj, jint index){
  jclass cls;
  jfieldID fid;
  jlong condptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "condptr", "J");
  condptr = (*env)->GetLongField(env, obj, fid);
  return est_cond_score((ESTCOND *)(PTRNUM)condptr, index);
}


/* get_shadows */
JNIEXPORT jintArray JNICALL
Java_estraier_Result_get_1shadows(JNIEnv *env, jobject obj, jint id){
  jclass cls;
  jfieldID fid;
  jlong condptr;
  jintArray resary;
  jint *resaryptr;
  jboolean icresary;
  const int *ary;
  int i, anum;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "condptr", "J");
  condptr = (*env)->GetLongField(env, obj, fid);
  ary = est_cond_shadows((ESTCOND *)(PTRNUM)condptr, id, &anum);
  if(!(resary = (*env)->NewIntArray(env, anum))){
    throwoutmem(env);
    return NULL;
  }
  resaryptr = (*env)->GetIntArrayElements(env, resary, &icresary);
  for(i = 0; i < anum; i++){
    resaryptr[i] = ary[i];
  }
  if(icresary == JNI_TRUE) (*env)->ReleaseIntArrayElements(env, resary, resaryptr, 0);
  return resary;
}


/* destroy */
JNIEXPORT void JNICALL
Java_estraier_Result_destroy(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  jlong condptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "condptr", "J");
  condptr = (*env)->GetLongField(env, obj, fid);
  est_cond_delete((ESTCOND *)(PTRNUM)condptr);
}



/* END OF FILE */
