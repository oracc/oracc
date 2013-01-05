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


#include "estraier_Document.h"
#include "myconf.h"



/*************************************************************************************************
 * public objects
 *************************************************************************************************/


/* add_attr */
JNIEXPORT void JNICALL
Java_estraier_Document_add_1attr(JNIEnv *env, jobject obj, jstring name, jstring value){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jboolean icname, icvalue;
  const char *tname, *tvalue;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(!name || !isinstanceof(env, name, CLSSTRING)){
    throwillarg(env);
    return;
  }
  if(value && !isinstanceof(env, value, CLSSTRING)){
    throwillarg(env);
    return;
  }
  if(!(tname = (*env)->GetStringUTFChars(env, name, &icname))){
    throwoutmem(env);
    return;
  }
  tvalue = value ? (*env)->GetStringUTFChars(env, value, &icvalue) : NULL;
  est_doc_add_attr((ESTDOC *)(PTRNUM)coreptr, tname, tvalue);
  if(tvalue && icvalue == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, value, tvalue);
  if(icname == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, name, tname);
}


/* add_text */
JNIEXPORT void JNICALL
Java_estraier_Document_add_1text(JNIEnv *env, jobject obj, jstring text){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jboolean ictext;
  const char *ttext;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(!text || !isinstanceof(env, text, CLSSTRING)){
    throwillarg(env);
    return;
  }
  if(!(ttext = (*env)->GetStringUTFChars(env, text, &ictext))){
    throwoutmem(env);
    return;
  }
  est_doc_add_text((ESTDOC *)(PTRNUM)coreptr, ttext);
  if(ictext == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, text, ttext);
}


/* add_hidden_text */
JNIEXPORT void JNICALL
Java_estraier_Document_add_1hidden_1text(JNIEnv *env, jobject obj, jstring text){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jboolean ictext;
  const char *ttext;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(!text || !isinstanceof(env, text, CLSSTRING)){
    throwillarg(env);
    return;
  }
  if(!(ttext = (*env)->GetStringUTFChars(env, text, &ictext))){
    throwoutmem(env);
    return;
  }
  est_doc_add_hidden_text((ESTDOC *)(PTRNUM)coreptr, ttext);
  if(ictext == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, text, ttext);
}


/* set_keywords */
JNIEXPORT void JNICALL
Java_estraier_Document_set_1keywords(JNIEnv *env, jobject obj, jobject kwords){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  CBMAP *tkwords;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(!kwords || !isinstanceof(env, kwords, CLSMAP)){
    throwillarg(env);
    return;
  }
  tkwords = objtocbmap(env, kwords);
  est_doc_set_keywords((ESTDOC *)(PTRNUM)coreptr, tkwords);
  cbmapclose(tkwords);
}


/* set_score */
JNIEXPORT void JNICALL
Java_estraier_Document_set_1score(JNIEnv *env, jobject obj, jint score){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  est_doc_set_score((ESTDOC *)(PTRNUM)coreptr, score);
}


/* id */
JNIEXPORT jint JNICALL
Java_estraier_Document_id(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  return est_doc_id((ESTDOC *)(PTRNUM)coreptr);
}


/* attr_names */
JNIEXPORT jobject JNICALL
Java_estraier_Document_attr_1names(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jobject names;
  CBLIST *tnames;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  tnames = est_doc_attr_names((ESTDOC *)(PTRNUM)coreptr);
  names = cblisttoobj(env, tnames);
  cblistclose(tnames);
  return names;
}


/* attr */
JNIEXPORT jstring JNICALL
Java_estraier_Document_attr(JNIEnv *env, jobject obj, jstring name){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jboolean icname;
  jobject value;
  const char *tname, *tvalue;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(!name || !isinstanceof(env, name, CLSSTRING)){
    throwillarg(env);
    return NULL;
  }
  if(!(tname = (*env)->GetStringUTFChars(env, name, &icname))){
    throwoutmem(env);
    return NULL;
  }
  if(!(tvalue = est_doc_attr((ESTDOC *)(PTRNUM)coreptr, tname))){
    if(icname == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, name, tname);
    return NULL;
  }
  if(!(value = (*env)->NewStringUTF(env, tvalue))){
    throwoutmem(env);
    return NULL;
  }
  if(icname == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, name, tname);
  return value;
}


/* texts */
JNIEXPORT jobject JNICALL
Java_estraier_Document_texts(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  return cblisttoobj(env, est_doc_texts((ESTDOC *)(PTRNUM)coreptr));
}


/* cat_texts */
JNIEXPORT jstring JNICALL
Java_estraier_Document_cat_1texts(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jobject texts;
  char *ttexts;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  ttexts = est_doc_cat_texts((ESTDOC *)(PTRNUM)coreptr);
  if(!(texts = (*env)->NewStringUTF(env, ttexts))){
    throwoutmem(env);
    return NULL;
  }
  free(ttexts);
  return texts;
}


/* keywords */
JNIEXPORT jobject JNICALL
Java_estraier_Document_keywords(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  CBMAP *kwords;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(!(kwords = est_doc_keywords((ESTDOC *)(PTRNUM)coreptr))) return NULL;
  return cbmaptoobj(env, kwords);
}


/* score */
JNIEXPORT jint JNICALL
Java_estraier_Document_score(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  return est_doc_score((ESTDOC *)(PTRNUM)coreptr);
}


/* dump_draft */
JNIEXPORT jstring JNICALL
Java_estraier_Document_dump_1draft(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jobject draft;
  char *tdraft;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  tdraft = est_doc_dump_draft((ESTDOC *)(PTRNUM)coreptr);
  if(!(draft = (*env)->NewStringUTF(env, tdraft))){
    throwoutmem(env);
    return NULL;
  }
  free(tdraft);
  return draft;
}


/* make_snippet */
JNIEXPORT jstring JNICALL
Java_estraier_Document_make_1snippet(JNIEnv *env, jobject obj,
                                     jobject words, jint wwidth, jint hwidth, jint awidth){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  jobject snip;
  CBLIST *twords;
  char *tsnip;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  if(!words || !isinstanceof(env, words, CLSLIST)){
    throwillarg(env);
    return NULL;
  }
  if(wwidth < 0 || hwidth < 0 || awidth < 0){
    throwillarg(env);
    return NULL;
  }
  twords = objtocblist(env, words);
  tsnip = est_doc_make_snippet((ESTDOC *)(PTRNUM)coreptr, twords, wwidth, hwidth, awidth);
  if(!(snip = (*env)->NewStringUTF(env, tsnip))){
    throwoutmem(env);
    return NULL;
  }
  free(tsnip);
  cblistclose(twords);
  return snip;
}


/* initialize */
JNIEXPORT void JNICALL
Java_estraier_Document_initialize__(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  (*env)->SetLongField(env, obj, fid, (PTRNUM)est_doc_new());
}


/* initialize */
JNIEXPORT void JNICALL
Java_estraier_Document_initialize__Ljava_lang_String_2(JNIEnv *env, jobject obj, jstring draft){
  jclass cls;
  jfieldID fid;
  jboolean icdraft;
  const char *tdraft;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  if(!draft || !isinstanceof(env, draft, CLSSTRING)){
    throwillarg(env);
    return;
  }
  if(!(tdraft = (*env)->GetStringUTFChars(env, draft, &icdraft))){
    throwoutmem(env);
    return;
  }
  (*env)->SetLongField(env, obj, fid, (PTRNUM)est_doc_new_from_draft(tdraft));
  if(icdraft == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, draft, tdraft);
}


/* destroy */
JNIEXPORT void JNICALL
Java_estraier_Document_destroy(JNIEnv *env, jobject obj){
  jclass cls;
  jfieldID fid;
  jlong coreptr;
  cls = (*env)->GetObjectClass(env, obj);
  fid = (*env)->GetFieldID(env, cls, "coreptr", "J");
  coreptr = (*env)->GetLongField(env, obj, fid);
  est_doc_delete((ESTDOC *)(PTRNUM)coreptr);
}



/* END OF FILE */
