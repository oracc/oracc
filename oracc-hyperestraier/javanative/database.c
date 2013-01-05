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


#include "estraier_Database.h"
#include "myconf.h"


/* global variables */
JNIEnv *infoenv = NULL;


/* private function prototypes */
static void setecode(JNIEnv *env, jobject obj, int ecode);
static void dbinform(const char *message, void *opaque);



/*************************************************************************************************
 * public objects
 *************************************************************************************************/


/* search_meta */
JNIEXPORT jobject JNICALL
Java_estraier_Database_search_1meta(JNIEnv *env, jclass cls, jobjectArray dbary, jobject cond){
  jfieldID fid;
  jmethodID mid;
  jobject dbobj, hintsobj, resobj;
  jlong coreptr, condptr;
  jintArray resary, idxary;
  jint *resaryptr, *idxaryptr;
  jboolean icresary, icidxary;
  ESTMTDB **dbs;
  CBMAP *hints;
  int i, dbnum, *res, rnum;
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  if(!dbary || !cond || !isinstanceof(env, cond, CLSCOND)){
    throwillarg(env);
    return NULL;
  }
  dbnum = (int)((*env)->GetArrayLength(env, dbary));
  dbs = cbmalloc(dbnum * sizeof(ESTMTDB) + 1);
  for(i = 0; i < dbnum; i++){
    dbobj = (*env)->GetObjectArrayElement(env, dbary, i);
    if(!dbobj || !isinstanceof(env, dbobj, CLSDB)){
      throwillarg(env);
      return NULL;
    }
    coreptr = (*env)->GetLongField(env, dbobj, fid);
    if(coreptr == 0){
      throwillarg(env);
      return NULL;
    }
    dbs[i] = (ESTMTDB *)(PTRNUM)coreptr;
  }
  cls = (*env)->GetObjectClass(env, cond);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  condptr = (*env)->GetLongField(env, cond, fid);
  hints = cbmapopen();
  res = est_mtdb_search_meta(dbs, dbnum, (ESTCOND *)(PTRNUM)condptr, &rnum, hints);
  if(!(resary = (*env)->NewIntArray(env, rnum / 2))){
    throwoutmem(env);
    return NULL;
  }
  resaryptr = (*env)->GetIntArrayElements(env, resary, &icresary);
  for(i = 0; i < rnum; i += 2){
    resaryptr[i/2] = res[i+1];
  }
  if(icresary == JNI_TRUE) (*env)->ReleaseIntArrayElements(env, resary, resaryptr, 0);
  if(!(idxary = (*env)->NewIntArray(env, rnum / 2))){
    throwoutmem(env);
    return NULL;
  }
  idxaryptr = (*env)->GetIntArrayElements(env, idxary, &icidxary);
  for(i = 0; i < rnum; i += 2){
    idxaryptr[i/2] = res[i];
  }
  if(icidxary == JNI_TRUE) (*env)->ReleaseIntArrayElements(env, idxary, idxaryptr, 0);
  cls = (*env)->FindClass(env, CLSRES);
  mid = (*env)->GetMethodID(env, cls, "<init>", "([I[IL" CLSMAP ";J)V");
  hintsobj = cbmaptoobj(env, hints);
  if(!(resobj = (*env)->NewObject(env, cls, mid, resary, idxary, hintsobj,
                                  (PTRNUM)est_cond_dup((ESTCOND *)(PTRNUM)condptr)))){
    throwoutmem(env);
    return NULL;
  }
  free(res);
  cbmapclose(hints);
  return resobj;
}


/* version */
JNIEXPORT jstring JNICALL
Java_estraier_Database_version(JNIEnv *env, jclass cls){
  jstring ver;
  if(!(ver = (*env)->NewStringUTF(env, est_version))){
    throwoutmem(env);
    return NULL;
  }
  return ver;
}


/* err_msg */
JNIEXPORT jstring JNICALL
Java_estraier_Database_err_1msg(JNIEnv *env, jobject obj, jint ecode){
  jstring msg;
  if(!(msg = (*env)->NewStringUTF(env, est_err_msg(ecode)))){
    throwoutmem(env);
    return NULL;
  }
  return msg;
}


/* open */
JNIEXPORT jboolean JNICALL
Java_estraier_Database_open(JNIEnv *env, jobject obj, jstring name, jint omode){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jboolean icname;
  ESTMTDB *db;
  const char *tname;
  int err, ecode;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr != 0 || !name || !isinstanceof(env, name, CLSSTRING)){
    throwillarg(env);
    return FALSE;
  }
  if(!(tname = (*env)->GetStringUTFChars(env, name, &icname))){
    throwoutmem(env);
    return FALSE;
  }
  err = FALSE;
  if((db = est_mtdb_open(tname, omode, &ecode)) != NULL){
    (*env)->SetLongField(env, obj, fid, (PTRNUM)db);
  } else {
    setecode(env, obj, ecode);
    err = TRUE;
  }
  if(icname == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, name, tname);
  return err ? FALSE : TRUE;
}


/* close */
JNIEXPORT jboolean JNICALL
Java_estraier_Database_close(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jobject infoobj;
  int err, ecode;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0){
    throwillarg(env);
    return FALSE;
  }
  err = FALSE;
  if(!est_mtdb_close((ESTMTDB *)(PTRNUM)coreptr, &ecode)){
    setecode(env, obj, ecode);
    err = TRUE;
  }
  (*env)->SetLongField(env, obj, fid, 0);
  fid = (*env)->GetFieldID(env, cls, "informer", "L" CLSDBINFO ";");
  if((infoobj = (*env)->GetObjectField(env, obj, fid)) != NULL)
    (*env)->DeleteGlobalRef(env, infoobj);
  (*env)->SetObjectField(env, obj, fid, NULL);
  return err ? FALSE : TRUE;
}


/* error */
JNIEXPORT jint JNICALL
Java_estraier_Database_error(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "ecode", "I");
  return (*env)->GetIntField(env, obj, fid);
}


/* fatal */
JNIEXPORT jboolean JNICALL
Java_estraier_Database_fatal(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0) return FALSE;
  return est_mtdb_fatal((ESTMTDB *)(PTRNUM)coreptr);
}


/* add_attr_index */
JNIEXPORT jboolean JNICALL
Java_estraier_Database_add_1attr_1index(JNIEnv *env, jobject obj, jstring name, jint type){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jboolean icname;
  const char *tname;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0 || !name || !isinstanceof(env, name, CLSSTRING)){
    throwillarg(env);
    return -1;
  }
  if(!(tname = (*env)->GetStringUTFChars(env, name, &icname))){
    throwoutmem(env);
    return -1;
  }
  if(!est_mtdb_add_attr_index((ESTMTDB *)(PTRNUM)coreptr, tname, type)){
    setecode(env, obj, est_mtdb_error((ESTMTDB *)(PTRNUM)coreptr));
    if(icname == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, name, tname);
    return FALSE;
  }
  if(icname == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, name, tname);
  return TRUE;
}


/* flush */
JNIEXPORT jboolean JNICALL
Java_estraier_Database_flush(JNIEnv *env, jobject obj, jint max){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0){
    throwillarg(env);
    return FALSE;
  }
  if(!est_mtdb_flush((ESTMTDB *)(PTRNUM)coreptr, max)){
    setecode(env, obj, est_mtdb_error((ESTMTDB *)(PTRNUM)coreptr));
    return FALSE;
  }
  return TRUE;
}


/* sync */
JNIEXPORT jboolean JNICALL
Java_estraier_Database_sync(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0){
    throwillarg(env);
    return FALSE;
  }
  if(!est_mtdb_sync((ESTMTDB *)(PTRNUM)coreptr)){
    setecode(env, obj, est_mtdb_error((ESTMTDB *)(PTRNUM)coreptr));
    return FALSE;
  }
  return TRUE;
}


/* optimize */
JNIEXPORT jboolean JNICALL
Java_estraier_Database_optimize(JNIEnv *env, jobject obj, jint options){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0){
    throwillarg(env);
    return FALSE;
  }
  if(!est_mtdb_optimize((ESTMTDB *)(PTRNUM)coreptr, options)){
    setecode(env, obj, est_mtdb_error((ESTMTDB *)(PTRNUM)coreptr));
    return FALSE;
  }
  return TRUE;
}


/* merge */
JNIEXPORT jboolean JNICALL
Java_estraier_Database_merge(JNIEnv *env, jobject obj, jstring name, jint options){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jboolean icname;
  const char *tname;
  int err;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0){
    throwillarg(env);
    return FALSE;
  }
  if(!name || !isinstanceof(env, name, CLSSTRING)){
    throwillarg(env);
    return FALSE;
  }
  if(!(tname = (*env)->GetStringUTFChars(env, name, &icname))){
    throwoutmem(env);
    return FALSE;
  }
  err = FALSE;
  if(!est_mtdb_merge((ESTMTDB *)(PTRNUM)coreptr, tname, options)){
    setecode(env, obj, est_mtdb_error((ESTMTDB *)(PTRNUM)coreptr));
    err = TRUE;
  }
  if(icname == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, name, tname);
  return err ? FALSE : TRUE;
}


/* put_doc */
JNIEXPORT jboolean JNICALL
Java_estraier_Database_put_1doc(JNIEnv *env, jobject obj, jobject doc, jint options){
  jclass cls;
  jfieldID fid;
  jlong coreptr, docptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0 || !doc || !isinstanceof(env, doc, CLSDOC)){
    throwillarg(env);
    return FALSE;
  }
  cls = (*env)->GetObjectClass(env, doc);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  docptr = (*env)->GetLongField(env, doc, fid);
  if(!est_mtdb_put_doc((ESTMTDB *)(PTRNUM)coreptr, (ESTDOC *)(PTRNUM)docptr, options)){
    setecode(env, obj, est_mtdb_error((ESTMTDB *)(PTRNUM)coreptr));
    return FALSE;
  }
  return TRUE;
}


/* out_doc */
JNIEXPORT jboolean JNICALL
Java_estraier_Database_out_1doc(JNIEnv *env, jobject obj, jint id, jint options){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0 || id < 1){
    throwillarg(env);
    return FALSE;
  }
  if(!est_mtdb_out_doc((ESTMTDB *)(PTRNUM)coreptr, id, options)){
    setecode(env, obj, est_mtdb_error((ESTMTDB *)(PTRNUM)coreptr));
    return FALSE;
  }
  return TRUE;
}


/* edit_doc */
JNIEXPORT jboolean JNICALL
Java_estraier_Database_edit_1doc(JNIEnv *env, jobject obj, jobject doc){
  jclass cls;
  jfieldID fid;
  jlong coreptr, docptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0 || !doc || !isinstanceof(env, doc, CLSDOC)){
    throwillarg(env);
    return FALSE;
  }
  cls = (*env)->GetObjectClass(env, doc);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  docptr = (*env)->GetLongField(env, doc, fid);
  if(!est_mtdb_edit_doc((ESTMTDB *)(PTRNUM)coreptr, (ESTDOC *)(PTRNUM)docptr)){
    setecode(env, obj, est_mtdb_error((ESTMTDB *)(PTRNUM)coreptr));
    return FALSE;
  }
  return TRUE;
}


/* get_doc */
JNIEXPORT jobject JNICALL
Java_estraier_Database_get_1doc(JNIEnv *env, jobject obj, jint id, jint options){
  jclass cls;
  jfieldID fid;
  jmethodID mid;
  jlong coreptr;
  ESTDOC *doc;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0 || id < 1){
    throwillarg(env);
    return NULL;
  }
  if(!(doc = est_mtdb_get_doc((ESTMTDB *)(PTRNUM)coreptr, id, options))){
    setecode(env, obj, est_mtdb_error((ESTMTDB *)(PTRNUM)coreptr));
    return NULL;
  }
  cls = (*env)->FindClass(env, CLSDOC);
  mid = (*env)->GetMethodID(env, cls, "<init>", "()V");
  if(!(obj = (*env)->NewObject(env, cls, mid))){
    throwoutmem(env);
    return NULL;
  }
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  (*env)->SetLongField(env, obj, fid, (PTRNUM)doc);
  return obj;
}


/* get_doc_attr */
JNIEXPORT jstring JNICALL
Java_estraier_Database_get_1doc_1attr(JNIEnv *env, jobject obj, jint id, jstring name){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jstring value;
  jboolean icname;
  const char *tname;
  char *tvalue;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0 || id < 1 || !name || !isinstanceof(env, name, CLSSTRING)){
    throwillarg(env);
    return NULL;
  }
  if(!(tname = (*env)->GetStringUTFChars(env, name, &icname))){
    throwoutmem(env);
    return NULL;
  }
  if(!(tvalue = est_mtdb_get_doc_attr((ESTMTDB *)(PTRNUM)coreptr, id, tname))){
    setecode(env, obj, est_mtdb_error((ESTMTDB *)(PTRNUM)coreptr));
    if(icname == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, name, tname);
    return NULL;
  }
  if(icname == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, name, tname);
  if(!(value = (*env)->NewStringUTF(env, tvalue))){
    throwoutmem(env);
    return NULL;
  }
  free(tvalue);
  return value;
}


/* uri_to_id */
JNIEXPORT jint JNICALL
Java_estraier_Database_uri_1to_1id(JNIEnv *env, jobject obj, jstring uri){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jboolean icuri;
  const char *turi;
  int id;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0 || !uri || !isinstanceof(env, uri, CLSSTRING)){
    throwillarg(env);
    return -1;
  }
  if(!(turi = (*env)->GetStringUTFChars(env, uri, &icuri))){
    throwoutmem(env);
    return -1;
  }
  if((id = est_mtdb_uri_to_id((ESTMTDB *)(PTRNUM)coreptr, turi)) == -1){
    setecode(env, obj, est_mtdb_error((ESTMTDB *)(PTRNUM)coreptr));
    if(icuri == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, uri, turi);
    return -1;
  }
  if(icuri == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, uri, turi);
  return id;
}


/* name */
JNIEXPORT jstring JNICALL
Java_estraier_Database_name(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jstring name;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0){
    throwillarg(env);
    return NULL;
  }
  if(!(name = (*env)->NewStringUTF(env, est_mtdb_name((ESTMTDB *)(PTRNUM)coreptr)))){
    throwoutmem(env);
    return NULL;
  }
  return name;
}


/* doc_num */
JNIEXPORT jint JNICALL
Java_estraier_Database_doc_1num(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0){
    throwillarg(env);
    return -1;
  }
  return est_mtdb_doc_num((ESTMTDB *)(PTRNUM)coreptr);
}


/* word_num */
JNIEXPORT jint JNICALL
Java_estraier_Database_word_1num(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0){
    throwillarg(env);
    return -1;
  }
  return est_mtdb_word_num((ESTMTDB *)(PTRNUM)coreptr);
}


/* size */
JNIEXPORT jdouble JNICALL
Java_estraier_Database_size(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0){
    throwillarg(env);
    return -1;
  }
  return est_mtdb_size((ESTMTDB *)(PTRNUM)coreptr);
}


/* search */
JNIEXPORT jobject JNICALL
Java_estraier_Database_search(JNIEnv *env, jobject obj, jobject cond){
  jclass cls;
  jfieldID fid;
  jmethodID mid;
  jlong coreptr, condptr;
  jintArray resary;
  jint *resaryptr;
  jobject hintsobj, resobj;
  jboolean icresary;
  CBMAP *hints;
  int i, *res, rnum;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0 || !cond || !isinstanceof(env, cond, CLSCOND)){
    throwillarg(env);
    return NULL;
  }
  cls = (*env)->GetObjectClass(env, cond);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  condptr = (*env)->GetLongField(env, cond, fid);
  hints = cbmapopen();
  res = est_mtdb_search((ESTMTDB *)(PTRNUM)coreptr, (ESTCOND *)(PTRNUM)condptr, &rnum, hints);
  if(!(resary = (*env)->NewIntArray(env, rnum))){
    throwoutmem(env);
    return NULL;
  }
  resaryptr = (*env)->GetIntArrayElements(env, resary, &icresary);
  for(i = 0; i < rnum; i++){
    resaryptr[i] = res[i];
  }
  if(icresary == JNI_TRUE) (*env)->ReleaseIntArrayElements(env, resary, resaryptr, 0);
  cls = (*env)->FindClass(env, CLSRES);
  mid = (*env)->GetMethodID(env, cls, "<init>", "([IL" CLSMAP ";J)V");
  hintsobj = cbmaptoobj(env, hints);
  if(!(resobj = (*env)->NewObject(env, cls, mid, resary, hintsobj,
                                  (PTRNUM)est_cond_dup((ESTCOND *)(PTRNUM)condptr)))){
    throwoutmem(env);
    return NULL;
  }
  free(res);
  cbmapclose(hints);
  return resobj;
}


/* scan_doc */
JNIEXPORT jboolean JNICALL
Java_estraier_Database_scan_1doc(JNIEnv *env, jobject obj, jobject doc, jobject cond){
  jclass cls;
  jfieldID fid;
  jlong coreptr, docptr, condptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0 || !doc || !isinstanceof(env, doc, CLSDOC) ||
     !cond || !isinstanceof(env, cond, CLSCOND)){
    throwillarg(env);
    return FALSE;
  }
  cls = (*env)->GetObjectClass(env, doc);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  docptr = (*env)->GetLongField(env, doc, fid);
  cls = (*env)->GetObjectClass(env, cond);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  condptr = (*env)->GetLongField(env, cond, fid);
  return est_mtdb_scan_doc((ESTMTDB *)(PTRNUM)coreptr,
                           (ESTDOC *)(PTRNUM)docptr, (ESTCOND *)(PTRNUM)condptr);
}


/* set_cache_size */
JNIEXPORT void JNICALL
Java_estraier_Database_set_1cache_1size(JNIEnv *env, jobject obj,
                                        jdouble size, jint anum, jint tnum, jint rnum){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0){
    throwillarg(env);
    return;
  }
  est_mtdb_set_cache_size((ESTMTDB *)(PTRNUM)coreptr, size, anum, tnum, rnum);
}


/* add_pseudo_index */
JNIEXPORT jboolean JNICALL
Java_estraier_Database_add_1pseudo_1index(JNIEnv *env, jobject obj, jstring path){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jboolean icpath;
  const char *tpath;
  int err;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0){
    throwillarg(env);
    return FALSE;
  }
  if(!path || !isinstanceof(env, path, CLSSTRING)){
    throwillarg(env);
    return FALSE;
  }
  if(!(tpath = (*env)->GetStringUTFChars(env, path, &icpath))){
    throwoutmem(env);
    return FALSE;
  }
  err = FALSE;
  if(!est_mtdb_add_pseudo_index((ESTMTDB *)(PTRNUM)coreptr, tpath)){
    setecode(env, obj, est_mtdb_error((ESTMTDB *)(PTRNUM)coreptr));
    err = TRUE;
  }
  if(icpath == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, path, tpath);
  return err ? FALSE : TRUE;
}


/* set_wildmax */
JNIEXPORT void JNICALL
Java_estraier_Database_set_1wildmax(JNIEnv *env, jobject obj, jint num){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0){
    throwillarg(env);
    return;
  }
  est_mtdb_set_wildmax((ESTMTDB *)(PTRNUM)coreptr, num);
}


/* set_informer */
JNIEXPORT void JNICALL
Java_estraier_Database_set_1informer(JNIEnv *env, jobject obj, jobject informer){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jobject oldobj;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(coreptr == 0){
    throwillarg(env);
    return;
  }
  fid = (*env)->GetFieldID(env, cls, "informer", "L" CLSDBINFO ";");
  if((oldobj = (*env)->GetObjectField(env, obj, fid)) != NULL)
    (*env)->DeleteGlobalRef(env, oldobj);
  (*env)->SetObjectField(env, obj, fid, NULL);
  if(!(informer = (*env)->NewGlobalRef(env, informer))){
    throwoutmem(env);
    return;
  }
  (*env)->SetObjectField(env, obj, fid, informer);
  infoenv = env;
  est_mtdb_set_informer((ESTMTDB *)(PTRNUM)coreptr, dbinform, informer);
}



/*************************************************************************************************
 * public objects
 *************************************************************************************************/


/* set the error code */
static void setecode(JNIEnv *env, jobject obj, int ecode){
  jclass cls;
  jfieldID fid;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "ecode", "I");
  (*env)->SetIntField(env, obj, fid, ecode);
}


/* callback function for database events */
static void dbinform(const char *message, void *opaque){
  jstring msgobj;
  jobject opobj;
  jclass cls;
  jmethodID mid;
  if(!(msgobj = (*infoenv)->NewStringUTF(infoenv, message))) return;
  opobj = (jobject)opaque;
  cls = (*infoenv)->GetObjectClass(infoenv, opobj);
  mid = (*infoenv)->GetMethodID(infoenv, cls, "inform", "(L" CLSSTRING ";)V");
  (*infoenv)->CallVoidMethod(infoenv, opobj, mid, msgobj);
}



/* END OF FILE */
