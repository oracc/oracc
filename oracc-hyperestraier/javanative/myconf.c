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


#include "myconf.h"



/*************************************************************************************************
 * common settings
 *************************************************************************************************/


int isinstanceof(JNIEnv *env, jobject obj, const char *name){
  assert(env && name);
  if(!obj) return FALSE;
  return (*env)->IsInstanceOf(env, obj, (*env)->FindClass(env, name));
}


void throwexception(JNIEnv *env, const char *name, const char *message){
  jclass cls;
  assert(env && name && message);
  cls = (*env)->FindClass(env, name);
  (*env)->ThrowNew(env, cls, message);
}


void throwoutmem(JNIEnv *env){
  assert(env);
  throwexception(env, CLSEOUTMEM, "out of memory");
}


void throwillarg(JNIEnv *env){
  assert(env);
  throwexception(env, CLSEILLARG, "illegal argument");
}


jobject cblisttoobj(JNIEnv *env, const CBLIST *list){
  jclass cls;
  jmethodID mid;
  jobject obj, kobj;
  int i;
  assert(list);
  cls = (*env)->FindClass(env, CLSARRAYLIST);
  mid = (*env)->GetMethodID(env, cls, "<init>", "()V");
  obj = (*env)->NewObject(env, cls, mid);
  mid = (*env)->GetMethodID(env, cls, "add", "(L" CLSOBJECT ";)Z");
  for(i = 0; i < cblistnum(list); i++){
    kobj = (*env)->NewStringUTF(env, cblistval(list, i, NULL));
    (*env)->CallVoidMethod(env, obj, mid, kobj);
    (*env)->DeleteLocalRef(env, kobj);
  }
  return obj;
}


CBLIST *objtocblist(JNIEnv *env, jobject obj){
  jclass list, it;
  jmethodID midit, midhn, midn;
  jobject itobj, eobj;
  jboolean icelem;
  CBLIST *tlist;
  const char *telem;
  assert(obj);
  tlist = cblistopen();
  list = (*env)->GetObjectClass(env, obj);
  midit = (*env)->GetMethodID(env, list, "iterator", "()L" CLSITERATOR ";");
  itobj = (*env)->CallObjectMethod(env, obj, midit);
  it = (*env)->GetObjectClass(env, itobj);
  midhn = (*env)->GetMethodID(env, it, "hasNext", "()Z");
  midn = (*env)->GetMethodID(env, it, "next", "()L" CLSOBJECT ";");
  while((*env)->CallBooleanMethod(env, itobj, midhn)){
    eobj = (*env)->CallObjectMethod(env, itobj, midn);
    if(!isinstanceof(env, eobj, CLSSTRING)) continue;
    if(!(telem = (*env)->GetStringUTFChars(env, eobj, &icelem))) continue;
    cblistpush(tlist, telem, -1);
    if(icelem == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, eobj, telem);
  }
  return tlist;
}


jobject cbmaptoobj(JNIEnv *env, CBMAP *map){
  jclass cls;
  jmethodID mid;
  jobject obj, kobj, vobj;
  const char *kbuf, *vbuf;
  assert(list);
  cls = (*env)->FindClass(env, CLSLHMAP);
  mid = (*env)->GetMethodID(env, cls, "<init>", "()V");
  obj = (*env)->NewObject(env, cls, mid);
  mid = (*env)->GetMethodID(env, cls, "put", "(L" CLSOBJECT ";L" CLSOBJECT ";)L" CLSOBJECT ";");
  cbmapiterinit(map);
  while((kbuf = cbmapiternext(map, NULL)) != NULL){
    vbuf = cbmapiterval(kbuf, NULL);
    kobj = (*env)->NewStringUTF(env, kbuf);
    vobj = (*env)->NewStringUTF(env, vbuf);
    (*env)->CallObjectMethod(env, obj, mid, kobj, vobj);
    (*env)->DeleteLocalRef(env, kobj);
    (*env)->DeleteLocalRef(env, vobj);
  }
  return obj;
}


CBMAP *objtocbmap(JNIEnv *env, jobject obj){
  jclass map, set, it;
  jmethodID midks, midg, midit, midhn, midn;
  jobject ksobj, itobj, ekobj, evobj;
  jboolean ickey, icval;
  CBMAP *tmap;
  const char *tkey, *tval;
  assert(obj);
  tmap = cbmapopenex(31);
  map = (*env)->GetObjectClass(env, obj);
  midks = (*env)->GetMethodID(env, map, "keySet", "()L" CLSSET ";");
  midg = (*env)->GetMethodID(env, map, "get", "(L" CLSOBJECT ";)L" CLSOBJECT ";");
  ksobj = (*env)->CallObjectMethod(env, obj, midks);
  set = (*env)->GetObjectClass(env, ksobj);
  midit = (*env)->GetMethodID(env, set, "iterator", "()L" CLSITERATOR ";");
  itobj = (*env)->CallObjectMethod(env, ksobj, midit);
  it = (*env)->GetObjectClass(env, itobj);
  midhn = (*env)->GetMethodID(env, it, "hasNext", "()Z");
  midn = (*env)->GetMethodID(env, it, "next", "()L" CLSOBJECT ";");
  while((*env)->CallBooleanMethod(env, itobj, midhn)){
    ekobj = (*env)->CallObjectMethod(env, itobj, midn);
    if(!isinstanceof(env, ekobj, CLSSTRING)) continue;
    if(!(tkey = (*env)->GetStringUTFChars(env, ekobj, &ickey))) continue;
    evobj = (*env)->CallObjectMethod(env, obj, midg, ekobj);
    if(!isinstanceof(env, evobj, CLSSTRING) ||
       !(tval = (*env)->GetStringUTFChars(env, evobj, &icval))){
      if(ickey == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, ekobj, tkey);
      continue;
    }
    cbmapput(tmap, tkey, -1, tval, -1, TRUE);
    if(icval == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, evobj, tval);
    if(ickey == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, ekobj, tkey);
  }
  return tmap;
}



/* END OF FILE */
