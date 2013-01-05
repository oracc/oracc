/*************************************************************************************************
 * Ruby binding of Hyper Estraier
 *                                                      Copyright (C) 2004-2005 Mikio Hirabayashi
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


#include "ruby.h"
#include <estraier.h>
#include <estmtdb.h>
#include <cabin.h>
#include <stdlib.h>

#define VNDATA         "@ptr"
#define VNINFO         "@informer"
#define VNCOND         "@cond"

typedef struct {
  int *ids;
  int *dbidxs;
  int num;
  CBMAP *hints;
} ESTRES;

typedef struct {
  ESTMTDB *db;
  int ecode;
} ESTDBMGR;


/* private function prototypes */
static void doc_init(void);
static VALUE doc_initialize(int argc, VALUE *argv, VALUE vself);
static VALUE doc_add_attr(VALUE vself, VALUE vname, VALUE vvalue);
static VALUE doc_add_text(VALUE vself, VALUE vtext);
static VALUE doc_add_hidden_text(VALUE vself, VALUE vtext);
static VALUE doc_set_keywords(VALUE vself, VALUE vkwords);
static VALUE doc_set_score(VALUE vself, VALUE vscore);
static VALUE doc_id(VALUE vself);
static VALUE doc_attr_names(VALUE vself);
static VALUE doc_attr(VALUE vself, VALUE vname);
static VALUE doc_texts(VALUE vself);
static VALUE doc_cat_texts(VALUE vself);
static VALUE doc_keywords(VALUE vself);
static VALUE doc_score(VALUE vself);
static VALUE doc_dump_draft(VALUE vself);
static VALUE doc_make_snippet(VALUE vself, VALUE vwords,
                              VALUE vwwidth, VALUE vhwidth, VALUE vawidth);
static void cond_init(void);
static VALUE cond_initialize(VALUE vself);
static VALUE cond_set_phrase(VALUE vself, VALUE vphrase);
static VALUE cond_add_attr(VALUE vself, VALUE vexpr);
static VALUE cond_set_order(VALUE vself, VALUE vexpr);
static VALUE cond_set_max(VALUE vself, VALUE vmax);
static VALUE cond_set_skip(VALUE vself, VALUE vskip);
static VALUE cond_set_options(VALUE vself, VALUE voptions);
static VALUE cond_set_auxiliary(VALUE vself, VALUE vmin);
static VALUE cond_set_eclipse(VALUE vself, VALUE vlimit);
static VALUE cond_set_distinct(VALUE vself, VALUE vname);
static VALUE cond_set_mask(VALUE vself, VALUE vmask);
static void res_init(void);
static VALUE res_initialize(VALUE vself);
static ESTRES *est_res_new(void);
static void est_res_delete(ESTRES *res);
static VALUE res_doc_num(VALUE vself);
static VALUE res_get_doc_id(VALUE vself, VALUE vindex);
static VALUE res_get_dbidx(VALUE vself, VALUE vindex);
static VALUE res_hint_words(VALUE vself);
static VALUE res_hint(VALUE vself, VALUE vword);
static VALUE res_get_score(VALUE vself, VALUE vindex);
static VALUE res_get_shadows(VALUE vself, VALUE vid);
static void db_init(void);
static VALUE db_initialize(VALUE vself);
static ESTDBMGR *est_dbmgr_new(void);
static void est_dbmgr_delete(ESTDBMGR *db);
static VALUE db_search_meta(VALUE vself, VALUE vdbs, VALUE vcond);
static VALUE db_err_msg(VALUE vself, VALUE vecode);
static VALUE db_open(VALUE vself, VALUE vname, VALUE vomode);
static VALUE db_close(VALUE vself);
static VALUE db_error(VALUE vself);
static VALUE db_fatal(VALUE vself);
static VALUE db_add_attr_index(VALUE vself, VALUE vname, VALUE vtype);
static VALUE db_flush(VALUE vself, VALUE vmax);
static VALUE db_sync(VALUE vself);
static VALUE db_optimize(VALUE vself, VALUE voptions);
static VALUE db_merge(VALUE vself, VALUE vname, VALUE voptions);
static VALUE db_put_doc(VALUE vself, VALUE vdoc, VALUE voptions);
static VALUE db_out_doc(VALUE vself, VALUE vid, VALUE voptions);
static VALUE db_edit_doc(VALUE vself, VALUE vdoc);
static VALUE db_get_doc(VALUE vself, VALUE vid, VALUE voptions);
static VALUE db_get_doc_attr(VALUE vself, VALUE vid, VALUE vname);
static VALUE db_uri_to_id(VALUE vself, VALUE vuri);
static VALUE db_name(VALUE vself);
static VALUE db_doc_num(VALUE vself);
static VALUE db_word_num(VALUE vself);
static VALUE db_size(VALUE vself);
static VALUE db_search(VALUE vself, VALUE vcond);
static VALUE db_scan_doc(VALUE vself, VALUE vdoc, VALUE vcond);
static VALUE db_set_cache_size(VALUE vself, VALUE vsize, VALUE vanum, VALUE vtnum, VALUE vrnum);
static VALUE db_add_pseudo_index(VALUE vself, VALUE vpath);
static VALUE db_set_wildmax(VALUE vself, VALUE vnum);
static VALUE db_set_informer(VALUE vself, VALUE vinformer);
static VALUE cblisttoobj(const CBLIST *list);
static CBLIST *objtocblist(VALUE obj);
static VALUE cbmaptoobj(CBMAP *map);
static CBMAP *objtocbmap(VALUE obj);
static void db_informer(const char *message, void *opaque);
static VALUE db_informer_process(VALUE arg);
static VALUE db_informer_resque(VALUE arg);



/*************************************************************************************************
 * public objects
 *************************************************************************************************/


VALUE mod_estraier;
VALUE cls_doc;
VALUE cls_doc_data;
VALUE cls_cond;
VALUE cls_cond_data;
VALUE cls_res;
VALUE cls_res_data;
VALUE cls_db;
VALUE cls_db_data;


int Init_estraier(void){
  mod_estraier = rb_define_module("Estraier");
  doc_init();
  cond_init();
  res_init();
  db_init();
  return 0;
}



/*************************************************************************************************
 * private objects
 *************************************************************************************************/


static void doc_init(void){
  cls_doc = rb_define_class_under(mod_estraier, "Document", rb_cObject);
  cls_doc_data = rb_define_class_under(mod_estraier, "Document_data", rb_cObject);
  rb_define_private_method(cls_doc, "initialize", doc_initialize, -1);
  rb_define_method(cls_doc, "add_attr", doc_add_attr, 2);
  rb_define_method(cls_doc, "add_text", doc_add_text, 1);
  rb_define_method(cls_doc, "add_hidden_text", doc_add_hidden_text, 1);
  rb_define_method(cls_doc, "set_keywords", doc_set_keywords, 1);
  rb_define_method(cls_doc, "set_score", doc_set_score, 1);
  rb_define_method(cls_doc, "id", doc_id, 0);
  rb_define_method(cls_doc, "attr_names", doc_attr_names, 0);
  rb_define_method(cls_doc, "attr", doc_attr, 1);
  rb_define_method(cls_doc, "texts", doc_texts, 0);
  rb_define_method(cls_doc, "cat_texts", doc_cat_texts, 0);
  rb_define_method(cls_doc, "keywords", doc_keywords, 0);
  rb_define_method(cls_doc, "score", doc_score, 0);
  rb_define_method(cls_doc, "dump_draft", doc_dump_draft, 0);
  rb_define_method(cls_doc, "make_snippet", doc_make_snippet, 4);
}


static VALUE doc_initialize(int argc, VALUE *argv, VALUE vself){
  VALUE vdraft, vdoc;
  ESTDOC *doc;
  rb_scan_args(argc, argv, "01", &vdraft);
  if(vdraft != Qnil){
    Check_Type(vdraft, T_STRING);
    doc = est_doc_new_from_draft(StringValuePtr(vdraft));
  } else {
    doc = est_doc_new();
  }
  vdoc = Data_Wrap_Struct(cls_doc_data, 0, est_doc_delete, doc);
  rb_iv_set(vself, VNDATA, vdoc);
  return Qnil;
}


static VALUE doc_add_attr(VALUE vself, VALUE vname, VALUE vvalue){
  VALUE vdoc;
  ESTDOC *doc;
  const char *value;
  vdoc = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdoc, ESTDOC, doc);
  Check_Type(vname, T_STRING);
  if(vvalue != Qnil){
    Check_Type(vvalue, T_STRING);
    value = StringValuePtr(vvalue);
  } else {
    value = NULL;
  }
  est_doc_add_attr(doc, StringValuePtr(vname), value);
  return Qnil;
}


static VALUE doc_add_text(VALUE vself, VALUE vtext){
  VALUE vdoc;
  ESTDOC *doc;
  vdoc = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdoc, ESTDOC, doc);
  Check_Type(vtext, T_STRING);
  est_doc_add_text(doc, StringValuePtr(vtext));
  return Qnil;
}


static VALUE doc_add_hidden_text(VALUE vself, VALUE vtext){
  VALUE vdoc;
  ESTDOC *doc;
  vdoc = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdoc, ESTDOC, doc);
  Check_Type(vtext, T_STRING);
  est_doc_add_hidden_text(doc, StringValuePtr(vtext));
  return Qnil;
}


static VALUE doc_set_keywords(VALUE vself, VALUE vkwords){
  VALUE vdoc;
  ESTDOC *doc;
  CBMAP *kwords;
  vdoc = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdoc, ESTDOC, doc);
  Check_Type(vkwords, T_HASH);
  kwords = objtocbmap(vkwords);
  est_doc_set_keywords(doc, kwords);
  cbmapclose(kwords);
  return Qnil;
}


static VALUE doc_set_score(VALUE vself, VALUE vscore){
  VALUE vdoc;
  ESTDOC *doc;
  vdoc = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdoc, ESTDOC, doc);
  est_doc_set_score(doc, NUM2INT(vscore));
  return Qnil;
}


static VALUE doc_id(VALUE vself){
  VALUE vdoc;
  ESTDOC *doc;
  vdoc = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdoc, ESTDOC, doc);
  return INT2NUM(est_doc_id(doc));
}


static VALUE doc_attr_names(VALUE vself){
  VALUE vdoc, vnames;
  ESTDOC *doc;
  CBLIST *names;
  vdoc = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdoc, ESTDOC, doc);
  names = est_doc_attr_names(doc);
  vnames = cblisttoobj(names);
  cblistclose(names);
  return vnames;
}


static VALUE doc_attr(VALUE vself, VALUE vname){
  VALUE vdoc;
  ESTDOC *doc;
  const char *value;
  vdoc = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdoc, ESTDOC, doc);
  Check_Type(vname, T_STRING);
  value = est_doc_attr(doc, StringValuePtr(vname));
  return value ? rb_str_new2(value) : Qnil;
}


static VALUE doc_texts(VALUE vself){
  VALUE vdoc;
  ESTDOC *doc;
  vdoc = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdoc, ESTDOC, doc);
  return cblisttoobj(est_doc_texts(doc));
}


static VALUE doc_cat_texts(VALUE vself){
  VALUE vdoc, vtexts;
  ESTDOC *doc;
  char *texts;
  vdoc = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdoc, ESTDOC, doc);
  texts = est_doc_cat_texts(doc);
  vtexts = rb_str_new2(texts);
  free(texts);
  return vtexts;
}


static VALUE doc_keywords(VALUE vself){
  VALUE vdoc;
  ESTDOC *doc;
  CBMAP *kwords;
  vdoc = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdoc, ESTDOC, doc);
  if(!(kwords = est_doc_keywords(doc))) return Qnil;
  return cbmaptoobj(kwords);
}


static VALUE doc_score(VALUE vself){
  VALUE vdoc;
  ESTDOC *doc;
  vdoc = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdoc, ESTDOC, doc);
  return INT2NUM(est_doc_score(doc));
}


static VALUE doc_dump_draft(VALUE vself){
  VALUE vdoc, vdraft;
  ESTDOC *doc;
  char *draft;
  vdoc = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdoc, ESTDOC, doc);
  draft = est_doc_dump_draft(doc);
  vdraft = rb_str_new2(draft);
  free(draft);
  return vdraft;
}


static VALUE doc_make_snippet(VALUE vself, VALUE vwords,
                              VALUE vwwidth, VALUE vhwidth, VALUE vawidth){
  VALUE vdoc, vsnippet;
  ESTDOC *doc;
  CBLIST *words;
  int i, len;
  char *snippet;
  vdoc = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdoc, ESTDOC, doc);
  Check_Type(vwords, T_ARRAY);
  len = RARRAY(vwords)->len;
  for(i = 0; i < len; i++){
    Check_Type(rb_ary_entry(vwords, i), T_STRING);
  }
  words = objtocblist(vwords);
  snippet = est_doc_make_snippet(doc, words,
                                 NUM2INT(vwwidth), NUM2INT(vhwidth), NUM2INT(vawidth));
  vsnippet = rb_str_new2(snippet);
  free(snippet);
  cblistclose(words);
  return vsnippet;
}


static void cond_init(void){
  cls_cond = rb_define_class_under(mod_estraier, "Condition", rb_cObject);
  cls_cond_data = rb_define_class_under(mod_estraier, "Condition_data", rb_cObject);
  rb_define_const(cls_cond, "SURE", INT2NUM(ESTCONDSURE));
  rb_define_const(cls_cond, "USUAL", INT2NUM(ESTCONDUSUAL));
  rb_define_const(cls_cond, "FAST", INT2NUM(ESTCONDFAST));
  rb_define_const(cls_cond, "AGITO", INT2NUM(ESTCONDAGITO));
  rb_define_const(cls_cond, "NOIDF", INT2NUM(ESTCONDNOIDF));
  rb_define_const(cls_cond, "SIMPLE", INT2NUM(ESTCONDSIMPLE));
  rb_define_const(cls_cond, "ROUGH", INT2NUM(ESTCONDROUGH));
  rb_define_const(cls_cond, "UNION", INT2NUM(ESTCONDUNION));
  rb_define_const(cls_cond, "ISECT", INT2NUM(ESTCONDISECT));
  rb_define_const(cls_cond, "ECLSIMURL", rb_float_new(ESTECLSIMURL));
  rb_define_const(cls_cond, "ECLSERV", rb_float_new(ESTECLSERV));
  rb_define_const(cls_cond, "ECLDIR", rb_float_new(ESTECLDIR));
  rb_define_const(cls_cond, "ECLFILE", rb_float_new(ESTECLFILE));
  rb_define_private_method(cls_cond, "initialize", cond_initialize, 0);
  rb_define_method(cls_cond, "set_phrase", cond_set_phrase, 1);
  rb_define_method(cls_cond, "add_attr", cond_add_attr, 1);
  rb_define_method(cls_cond, "set_order", cond_set_order, 1);
  rb_define_method(cls_cond, "set_max", cond_set_max, 1);
  rb_define_method(cls_cond, "set_skip", cond_set_skip, 1);
  rb_define_method(cls_cond, "set_options", cond_set_options, 1);
  rb_define_method(cls_cond, "set_auxiliary", cond_set_auxiliary, 1);
  rb_define_method(cls_cond, "set_eclipse", cond_set_eclipse, 1);
  rb_define_method(cls_cond, "set_distinct", cond_set_distinct, 1);
  rb_define_method(cls_cond, "set_mask", cond_set_mask, 1);
}


static VALUE cond_initialize(VALUE vself){
  VALUE vcond;
  ESTCOND *cond;
  cond = est_cond_new();
  est_cond_set_options(cond, ESTCONDSCFB);
  vcond = Data_Wrap_Struct(cls_cond_data, 0, est_cond_delete, cond);
  rb_iv_set(vself, VNDATA, vcond);
  return Qnil;
}


static VALUE cond_set_phrase(VALUE vself, VALUE vphrase){
  VALUE vcond;
  ESTCOND *cond;
  vcond = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vcond, ESTCOND, cond);
  Check_Type(vphrase, T_STRING);
  est_cond_set_phrase(cond, StringValuePtr(vphrase));
  return Qnil;
}


static VALUE cond_add_attr(VALUE vself, VALUE vexpr){
  VALUE vcond;
  ESTCOND *cond;
  vcond = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vcond, ESTCOND, cond);
  Check_Type(vexpr, T_STRING);
  est_cond_add_attr(cond, StringValuePtr(vexpr));
  return Qnil;
}


static VALUE cond_set_order(VALUE vself, VALUE vexpr){
  VALUE vcond;
  ESTCOND *cond;
  vcond = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vcond, ESTCOND, cond);
  Check_Type(vexpr, T_STRING);
  est_cond_set_order(cond, StringValuePtr(vexpr));
  return Qnil;
}


static VALUE cond_set_max(VALUE vself, VALUE vmax){
  VALUE vcond;
  ESTCOND *cond;
  int max;
  vcond = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vcond, ESTCOND, cond);
  max = NUM2INT(vmax);
  if(max < 0) rb_raise(rb_eArgError, "invalid argument");
  est_cond_set_max(cond, max);
  return Qnil;
}


static VALUE cond_set_skip(VALUE vself, VALUE vskip){
  VALUE vcond;
  ESTCOND *cond;
  int skip;
  vcond = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vcond, ESTCOND, cond);
  skip = NUM2INT(vskip);
  if(skip < 0) rb_raise(rb_eArgError, "invalid argument");
  est_cond_set_skip(cond, skip);
  return Qnil;
}


static VALUE cond_set_options(VALUE vself, VALUE voptions){
  VALUE vcond;
  ESTCOND *cond;
  vcond = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vcond, ESTCOND, cond);
  est_cond_set_options(cond, NUM2INT(voptions));
  return Qnil;
}


static VALUE cond_set_auxiliary(VALUE vself, VALUE vmin){
  VALUE vcond;
  ESTCOND *cond;
  vcond = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vcond, ESTCOND, cond);
  est_cond_set_auxiliary(cond, NUM2INT(vmin));
  return Qnil;
}


static VALUE cond_set_eclipse(VALUE vself, VALUE vlimit){
  VALUE vcond;
  ESTCOND *cond;
  vcond = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vcond, ESTCOND, cond);
  est_cond_set_eclipse(cond, NUM2DBL(vlimit));
  return Qnil;
}


static VALUE cond_set_distinct(VALUE vself, VALUE vname){
  VALUE vcond;
  ESTCOND *cond;
  vcond = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vcond, ESTCOND, cond);
  Check_Type(vname, T_STRING);
  est_cond_set_distinct(cond, StringValuePtr(vname));
  return Qnil;
}


static VALUE cond_set_mask(VALUE vself, VALUE vmask){
  VALUE vcond;
  ESTCOND *cond;
  vcond = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vcond, ESTCOND, cond);
  est_cond_set_mask(cond, NUM2INT(vmask));
  return Qnil;
}


static void res_init(void){
  cls_res = rb_define_class_under(mod_estraier, "Result", rb_cObject);
  cls_res_data = rb_define_class_under(mod_estraier, "Result_data", rb_cObject);
  rb_define_private_method(cls_res, "initialize", res_initialize, 0);
  rb_define_method(cls_res, "doc_num", res_doc_num, 0);
  rb_define_method(cls_res, "get_doc_id", res_get_doc_id, 1);
  rb_define_method(cls_res, "get_dbidx", res_get_dbidx, 1);
  rb_define_method(cls_res, "hint_words", res_hint_words, 0);
  rb_define_method(cls_res, "hint", res_hint, 1);
  rb_define_method(cls_res, "get_score", res_get_score, 1);
  rb_define_method(cls_res, "get_shadows", res_get_shadows, 1);
}


static VALUE res_initialize(VALUE vself){
  VALUE vres;
  ESTRES *res;
  res = est_res_new();
  vres = Data_Wrap_Struct(cls_res_data, 0, est_res_delete, res);
  rb_iv_set(vself, VNDATA, vres);
  return Qnil;
}


static ESTRES *est_res_new(void){
  ESTRES *res;
  res = cbmalloc(sizeof(ESTRES));
  res->ids = NULL;
  res->dbidxs = NULL;
  res->num = 0;
  res->hints = NULL;
  return res;
}


static void est_res_delete(ESTRES *res){
  if(res->hints) cbmapclose(res->hints);
  if(res->dbidxs) free(res->dbidxs);
  if(res->ids) free(res->ids);
  free(res);
}


static VALUE res_doc_num(VALUE vself){
  VALUE vres;
  ESTRES *res;
  vres = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vres, ESTRES, res);
  return INT2NUM(res->num);
}


static VALUE res_get_doc_id(VALUE vself, VALUE vindex){
  VALUE vres;
  ESTRES *res;
  int index;
  vres = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vres, ESTRES, res);
  index = NUM2INT(vindex);
  if(!res->ids || index < 0 || index >= res->num) return -1;
  return INT2NUM(res->ids[index]);
}


static VALUE res_get_dbidx(VALUE vself, VALUE vindex){
  VALUE vres;
  ESTRES *res;
  int index;
  vres = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vres, ESTRES, res);
  index = NUM2INT(vindex);
  if(!res->dbidxs || index < 0 || index >= res->num) return -1;
  return INT2NUM(res->dbidxs[index]);
}


static VALUE res_hint_words(VALUE vself){
  VALUE vres, vwords;
  ESTRES *res;
  CBLIST *words;
  const char *vbuf;
  int i;
  vres = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vres, ESTRES, res);
  if(!res->hints) return rb_ary_new();
  words = cbmapkeys(res->hints);
  for(i = 0; i < cblistnum(words); i++){
    vbuf = cblistval(words, i, NULL);
    if(vbuf[0] == '\0'){
      free(cblistremove(words, i, NULL));
      break;
    }
  }
  vwords = cblisttoobj(words);
  cblistclose(words);
  return vwords;
}


static VALUE res_hint(VALUE vself, VALUE vword){
  VALUE vres;
  ESTRES *res;
  const char *value;
  vres = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vres, ESTRES, res);
  Check_Type(vword, T_STRING);
  if(!res->hints) return INT2NUM(0);
  if(!(value = cbmapget(res->hints, StringValuePtr(vword), -1, NULL))) return INT2NUM(0);
  return INT2NUM(atoi(value));
}


static VALUE res_get_score(VALUE vself, VALUE vindex){
  VALUE vres, vcond;
  ESTRES *res;
  ESTCOND *cond;
  vres = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vres, ESTRES, res);
  vcond = rb_iv_get(vself, VNCOND);
  Data_Get_Struct(vcond, ESTCOND, cond);
  return INT2NUM(est_cond_score(cond, NUM2INT(vindex)));
}


static VALUE res_get_shadows(VALUE vself, VALUE vid){
  VALUE vres, vcond, vary;
  ESTRES *res;
  ESTCOND *cond;
  const int *ary;
  int i, anum;
  vres = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vres, ESTRES, res);
  vcond = rb_iv_get(vself, VNCOND);
  Data_Get_Struct(vcond, ESTCOND, cond);
  ary = est_cond_shadows(cond, NUM2INT(vid), &anum);
  vary = rb_ary_new2(anum);
  for(i = 0; i < anum; i++){
    rb_ary_push(vary, INT2NUM(ary[i]));
  }
  return vary;
}


static void db_init(void){
  cls_db = rb_define_class_under(mod_estraier, "Database", rb_cObject);
  cls_db_data = rb_define_class_under(mod_estraier, "Database_data", rb_cObject);
  rb_define_const(cls_db, "VERSION", rb_str_new2(est_version));
  rb_define_const(cls_db, "ERRNOERR", INT2NUM(ESTENOERR));
  rb_define_const(cls_db, "ERRINVAL", INT2NUM(ESTEINVAL));
  rb_define_const(cls_db, "ERRACCES", INT2NUM(ESTEACCES));
  rb_define_const(cls_db, "ERRLOCK", INT2NUM(ESTELOCK));
  rb_define_const(cls_db, "ERRDB", INT2NUM(ESTEDB));
  rb_define_const(cls_db, "ERRIO", INT2NUM(ESTEIO));
  rb_define_const(cls_db, "ERRNOITEM", INT2NUM(ESTENOITEM));
  rb_define_const(cls_db, "ERRMISC", INT2NUM(ESTEMISC));
  rb_define_const(cls_db, "DBREADER", INT2NUM(ESTDBREADER));
  rb_define_const(cls_db, "DBWRITER", INT2NUM(ESTDBWRITER));
  rb_define_const(cls_db, "DBCREAT", INT2NUM(ESTDBCREAT));
  rb_define_const(cls_db, "DBTRUNC", INT2NUM(ESTDBTRUNC));
  rb_define_const(cls_db, "DBNOLCK", INT2NUM(ESTDBNOLCK));
  rb_define_const(cls_db, "DBLCKNB", INT2NUM(ESTDBLCKNB));
  rb_define_const(cls_db, "DBPERFNG", INT2NUM(ESTDBPERFNG));
  rb_define_const(cls_db, "DBCHRCAT", INT2NUM(ESTDBCHRCAT));
  rb_define_const(cls_db, "DBSMALL", INT2NUM(ESTDBSMALL));
  rb_define_const(cls_db, "DBLARGE", INT2NUM(ESTDBLARGE));
  rb_define_const(cls_db, "DBHUGE", INT2NUM(ESTDBHUGE));
  rb_define_const(cls_db, "DBHUGE2", INT2NUM(ESTDBHUGE2));
  rb_define_const(cls_db, "DBHUGE3", INT2NUM(ESTDBHUGE3));
  rb_define_const(cls_db, "DBSCVOID", INT2NUM(ESTDBSCVOID));
  rb_define_const(cls_db, "DBSCINT", INT2NUM(ESTDBSCINT));
  rb_define_const(cls_db, "DBSCASIS", INT2NUM(ESTDBSCASIS));
  rb_define_const(cls_db, "IDXATTRSEQ", INT2NUM(ESTIDXATTRSEQ));
  rb_define_const(cls_db, "IDXATTRSTR", INT2NUM(ESTIDXATTRSTR));
  rb_define_const(cls_db, "IDXATTRNUM", INT2NUM(ESTIDXATTRNUM));
  rb_define_const(cls_db, "OPTNOPURGE", INT2NUM(ESTOPTNOPURGE));
  rb_define_const(cls_db, "OPTNODBOPT", INT2NUM(ESTOPTNODBOPT));
  rb_define_const(cls_db, "MGCLEAN", INT2NUM(ESTMGCLEAN));
  rb_define_const(cls_db, "PDCLEAN", INT2NUM(ESTPDCLEAN));
  rb_define_const(cls_db, "PDWEIGHT", INT2NUM(ESTPDWEIGHT));
  rb_define_const(cls_db, "ODCLEAN", INT2NUM(ESTODCLEAN));
  rb_define_const(cls_db, "GDNOATTR", INT2NUM(ESTGDNOATTR));
  rb_define_const(cls_db, "GDNOTEXT", INT2NUM(ESTGDNOTEXT));
  rb_define_const(cls_db, "GDNOKWD", INT2NUM(ESTGDNOKWD));
  rb_define_private_method(cls_db, "initialize", db_initialize, 0);
  rb_define_singleton_method(cls_db, "search_meta", db_search_meta, 2);
  rb_define_method(cls_db, "err_msg", db_err_msg, 1);
  rb_define_method(cls_db, "open", db_open, 2);
  rb_define_method(cls_db, "close", db_close, 0);
  rb_define_method(cls_db, "error", db_error, 0);
  rb_define_method(cls_db, "fatal", db_fatal, 0);
  rb_define_method(cls_db, "add_attr_index", db_add_attr_index, 2);
  rb_define_method(cls_db, "flush", db_flush, 1);
  rb_define_method(cls_db, "sync", db_sync, 0);
  rb_define_method(cls_db, "optimize", db_optimize, 1);
  rb_define_method(cls_db, "merge", db_merge, 2);
  rb_define_method(cls_db, "put_doc", db_put_doc, 2);
  rb_define_method(cls_db, "out_doc", db_out_doc, 2);
  rb_define_method(cls_db, "edit_doc", db_edit_doc, 1);
  rb_define_method(cls_db, "get_doc", db_get_doc, 2);
  rb_define_method(cls_db, "get_doc_attr", db_get_doc_attr, 2);
  rb_define_method(cls_db, "uri_to_id", db_uri_to_id, 1);
  rb_define_method(cls_db, "name", db_name, 0);
  rb_define_method(cls_db, "doc_num", db_doc_num, 0);
  rb_define_method(cls_db, "word_num", db_word_num, 0);
  rb_define_method(cls_db, "size", db_size, 0);
  rb_define_method(cls_db, "search", db_search, 1);
  rb_define_method(cls_db, "scan_doc", db_scan_doc, 2);
  rb_define_method(cls_db, "set_cache_size", db_set_cache_size, 4);
  rb_define_method(cls_db, "add_pseudo_index", db_add_pseudo_index, 1);
  rb_define_method(cls_db, "set_wildmax", db_set_wildmax, 1);
  rb_define_method(cls_db, "set_informer", db_set_informer, 1);
}


static VALUE db_initialize(VALUE vself){
  VALUE vdb;
  ESTDBMGR *db;
  db = est_dbmgr_new();
  vdb = Data_Wrap_Struct(cls_db_data, 0, est_dbmgr_delete, db);
  rb_iv_set(vself, VNDATA, vdb);
  return Qnil;
}


static ESTDBMGR *est_dbmgr_new(void){
  ESTDBMGR *db;
  db = cbmalloc(sizeof(ESTDBMGR));
  db->db = NULL;
  db->ecode = ESTENOERR;
  return db;
}


static void est_dbmgr_delete(ESTDBMGR *db){
  if(db->db) est_mtdb_close(db->db, &(db->ecode));
  free(db);
}


static VALUE db_search_meta(VALUE vself, VALUE vdbs, VALUE vcond){
  VALUE vdb, vres, tres, tcond;
  ESTDBMGR *db;
  ESTRES *ores;
  ESTCOND *cond;
  ESTMTDB **dbs;
  CBMAP *hints;
  int i, dnum, *res, rnum;
  Check_Type(vdbs, T_ARRAY);
  dnum = RARRAY(vdbs)->len;
  dbs = cbmalloc(dnum * sizeof(ESTMTDB *) + 1);
  for(i = 0; i < dnum; i++){
    vdb = rb_ary_entry(vdbs, i);
    if(rb_obj_is_instance_of(vdb, cls_db) != Qtrue){
      free(dbs);
      rb_raise(rb_eArgError, "invalid argument");
    }
    Data_Get_Struct(rb_iv_get(vdb, VNDATA), ESTDBMGR, db);
    if(!db->db){
      free(dbs);
      rb_raise(rb_eArgError, "invalid argument");
    }
    dbs[i] = db->db;
  }
  if(rb_obj_is_instance_of(vcond, cls_cond) != Qtrue){
    free(dbs);
    rb_raise(rb_eArgError, "invalid argument");
  }
  Data_Get_Struct(rb_iv_get(vcond, VNDATA), ESTCOND, cond);
  hints = cbmapopenex(31);
  res = est_mtdb_search_meta(dbs, dnum, cond, &rnum, hints);
  ores = est_res_new();
  ores->ids = res;
  ores->dbidxs = cbmalloc(rnum / 2 * sizeof(int) + 1);
  for(i = 0; i < rnum; i += 2){
    ores->dbidxs[i/2] = res[i];
    ores->ids[i/2] = res[i+1];
  }
  ores->num = rnum / 2;
  ores->hints = hints;
  vres = rb_funcall(cls_res, rb_intern("new"), 0);
  tres = Data_Wrap_Struct(cls_res_data, 0, est_res_delete, ores);
  rb_iv_set(vres, VNDATA, tres);
  tcond = Data_Wrap_Struct(cls_cond_data, 0, est_cond_delete, est_cond_dup(cond));
  rb_iv_set(vres, VNCOND, tcond);
  free(dbs);
  return vres;
}


static VALUE db_err_msg(VALUE vself, VALUE vecode){
  return rb_str_new2(est_err_msg(NUM2INT(vecode)));
}


static VALUE db_open(VALUE vself, VALUE vname, VALUE vomode){
  VALUE vdb;
  ESTDBMGR *db;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(db->db && !est_mtdb_close(db->db, &(db->ecode))){
    db->db = NULL;
    return Qfalse;
  }
  Check_Type(vname, T_STRING);
  if(!(db->db = est_mtdb_open(StringValuePtr(vname), NUM2INT(vomode), &(db->ecode))))
    return Qfalse;
  return Qtrue;
}


static VALUE db_close(VALUE vself){
  VALUE vdb;
  ESTDBMGR *db;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  if(!est_mtdb_close(db->db, &(db->ecode))){
    db->db = NULL;
    return Qfalse;
  }
  db->db = NULL;
  return Qtrue;
}


static VALUE db_error(VALUE vself){
  VALUE vdb;
  ESTDBMGR *db;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  return INT2NUM(db->ecode);
}


static VALUE db_fatal(VALUE vself){
  VALUE vdb;
  ESTDBMGR *db;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) return Qfalse;
  return est_mtdb_fatal(db->db) ? Qtrue : Qfalse;
}


static VALUE db_add_attr_index(VALUE vself, VALUE vname, VALUE vtype){
  VALUE vdb;
  ESTDBMGR *db;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) return Qfalse;
  Check_Type(vname, T_STRING);
  if(!est_mtdb_add_attr_index(db->db, StringValuePtr(vname), NUM2INT(vtype))){
    db->ecode = est_mtdb_error(db->db);
    return Qfalse;
  }
  return Qtrue;
}


static VALUE db_flush(VALUE vself, VALUE vmax){
  VALUE vdb;
  ESTDBMGR *db;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  if(!est_mtdb_flush(db->db, NUM2INT(vmax))){
    db->ecode = est_mtdb_error(db->db);
    return Qfalse;
  }
  return Qtrue;
}


static VALUE db_sync(VALUE vself){
  VALUE vdb;
  ESTDBMGR *db;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  if(!est_mtdb_sync(db->db)){
    db->ecode = est_mtdb_error(db->db);
    return Qfalse;
  }
  return Qtrue;
}


static VALUE db_optimize(VALUE vself, VALUE voptions){
  VALUE vdb;
  ESTDBMGR *db;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  if(!est_mtdb_optimize(db->db, NUM2INT(voptions))){
    db->ecode = est_mtdb_error(db->db);
    return Qfalse;
  }
  return Qtrue;
}


static VALUE db_merge(VALUE vself, VALUE vname, VALUE voptions){
  VALUE vdb;
  ESTDBMGR *db;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  Check_Type(vname, T_STRING);
  if(!est_mtdb_merge(db->db, StringValuePtr(vname), NUM2INT(voptions))){
    db->ecode = est_mtdb_error(db->db);
    return Qfalse;
  }
  return Qtrue;
}


static VALUE db_put_doc(VALUE vself, VALUE vdoc, VALUE voptions){
  VALUE vdb;
  ESTDBMGR *db;
  ESTDOC *doc;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  if(rb_obj_is_instance_of(vdoc, cls_doc) != Qtrue) rb_raise(rb_eArgError, "invalid argument");
  Data_Get_Struct(rb_iv_get(vdoc, VNDATA), ESTDOC, doc);
  if(!est_mtdb_put_doc(db->db, doc, NUM2INT(voptions))){
    db->ecode = est_mtdb_error(db->db);
    return Qfalse;
  }
  return Qtrue;
}


static VALUE db_out_doc(VALUE vself, VALUE vid, VALUE voptions){
  VALUE vdb;
  ESTDBMGR *db;
  int id;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  id = NUM2INT(vid);
  if(id < 1) rb_raise(rb_eArgError, "invalid argument");
  if(!est_mtdb_out_doc(db->db, id, NUM2INT(voptions))){
    db->ecode = est_mtdb_error(db->db);
    return Qfalse;
  }
  return Qtrue;
}


static VALUE db_edit_doc(VALUE vself, VALUE vdoc){
  VALUE vdb;
  ESTDBMGR *db;
  ESTDOC *doc;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  if(rb_obj_is_instance_of(vdoc, cls_doc) != Qtrue) rb_raise(rb_eArgError, "invalid argument");
  Data_Get_Struct(rb_iv_get(vdoc, VNDATA), ESTDOC, doc);
  if(!est_mtdb_edit_doc(db->db, doc)){
    db->ecode = est_mtdb_error(db->db);
    return Qfalse;
  }
  return Qtrue;
}


static VALUE db_get_doc(VALUE vself, VALUE vid, VALUE voptions){
  VALUE vdb, vdoc, tdoc;
  ESTDBMGR *db;
  ESTDOC *doc;
  int id;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  id = NUM2INT(vid);
  if(id < 1) rb_raise(rb_eArgError, "invalid argument");
  if(!(doc = est_mtdb_get_doc(db->db, id, NUM2INT(voptions)))){
    db->ecode = est_mtdb_error(db->db);
    return Qnil;
  }
  vdoc = rb_funcall(cls_doc, rb_intern("new"), 0);
  tdoc = Data_Wrap_Struct(cls_doc_data, 0, est_doc_delete, doc);
  rb_iv_set(vdoc, VNDATA, tdoc);
  return vdoc;
}


static VALUE db_get_doc_attr(VALUE vself, VALUE vid, VALUE vname){
  VALUE vdb, vvalue;
  ESTDBMGR *db;
  char *value;
  int id;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  Check_Type(vname, T_STRING);
  id = NUM2INT(vid);
  if(id < 1) rb_raise(rb_eArgError, "invalid argument");
  if(!(value = est_mtdb_get_doc_attr(db->db, id, StringValuePtr(vname)))){
    db->ecode = est_mtdb_error(db->db);
    return Qnil;
  }
  vvalue = rb_str_new2(value);
  free(value);
  return vvalue;
}


static VALUE db_uri_to_id(VALUE vself, VALUE vuri){
  VALUE vdb;
  ESTDBMGR *db;
  int id;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  Check_Type(vuri, T_STRING);
  if((id = est_mtdb_uri_to_id(db->db, StringValuePtr(vuri))) == -1){
    db->ecode = est_mtdb_error(db->db);
    return INT2NUM(-1);
  }
  return INT2NUM(id);
}


static VALUE db_name(VALUE vself){
  VALUE vdb;
  ESTDBMGR *db;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  return rb_str_new2(est_mtdb_name(db->db));
}


static VALUE db_doc_num(VALUE vself){
  VALUE vdb;
  ESTDBMGR *db;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  return INT2NUM(est_mtdb_doc_num(db->db));
}


static VALUE db_word_num(VALUE vself){
  VALUE vdb;
  ESTDBMGR *db;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  return INT2NUM(est_mtdb_word_num(db->db));
}


static VALUE db_size(VALUE vself){
  VALUE vdb;
  ESTDBMGR *db;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  return rb_float_new(est_mtdb_size(db->db));
}


static VALUE db_search(VALUE vself, VALUE vcond){
  VALUE vdb, vres, tres, tcond;
  ESTDBMGR *db;
  ESTRES *ores;
  ESTCOND *cond;
  CBMAP *hints;
  int *res, rnum;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  if(rb_obj_is_instance_of(vcond, cls_cond) != Qtrue) rb_raise(rb_eArgError, "invalid argument");
  Data_Get_Struct(rb_iv_get(vcond, VNDATA), ESTCOND, cond);
  hints = cbmapopenex(31);
  res = est_mtdb_search(db->db, cond, &rnum, hints);
  ores = est_res_new();
  ores->ids = res;
  ores->num = rnum;
  ores->hints = hints;
  vres = rb_funcall(cls_res, rb_intern("new"), 0);
  tres = Data_Wrap_Struct(cls_res_data, 0, est_res_delete, ores);
  rb_iv_set(vres, VNDATA, tres);
  tcond = Data_Wrap_Struct(cls_cond_data, 0, est_cond_delete, est_cond_dup(cond));
  rb_iv_set(vres, VNCOND, tcond);
  return vres;
}


static VALUE db_scan_doc(VALUE vself, VALUE vdoc, VALUE vcond){
  VALUE vdb;
  ESTDBMGR *db;
  ESTDOC *doc;
  ESTCOND *cond;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  if(rb_obj_is_instance_of(vdoc, cls_doc) != Qtrue) rb_raise(rb_eArgError, "invalid argument");
  if(rb_obj_is_instance_of(vcond, cls_cond) != Qtrue) rb_raise(rb_eArgError, "invalid argument");
  Data_Get_Struct(rb_iv_get(vdoc, VNDATA), ESTDOC, doc);
  Data_Get_Struct(rb_iv_get(vcond, VNDATA), ESTCOND, cond);
  return est_mtdb_scan_doc(db->db, doc, cond) ? Qtrue : Qfalse;
}


static VALUE db_set_cache_size(VALUE vself, VALUE vsize, VALUE vanum, VALUE vtnum, VALUE vrnum){
  VALUE vdb;
  ESTDBMGR *db;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  est_mtdb_set_cache_size(db->db, NUM2INT(vsize),
                          NUM2INT(vanum), NUM2INT(vtnum), NUM2INT(vrnum));
  return Qnil;
}


static VALUE db_add_pseudo_index(VALUE vself, VALUE vpath){
  VALUE vdb;
  ESTDBMGR *db;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  Check_Type(vpath, T_STRING);
  return est_mtdb_add_pseudo_index(db->db, StringValuePtr(vpath)) ? Qtrue : Qfalse;
}


static VALUE db_set_wildmax(VALUE vself, VALUE vnum){
  VALUE vdb;
  ESTDBMGR *db;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  est_mtdb_set_wildmax(db->db, NUM2INT(vnum));
  return Qnil;
}



static VALUE db_set_informer(VALUE vself, VALUE vinformer){
  VALUE vdb;
  ESTDBMGR *db;
  vdb = rb_iv_get(vself, VNDATA);
  Data_Get_Struct(vdb, ESTDBMGR, db);
  if(!db->db) rb_raise(rb_eArgError, "invalid argument");
  rb_iv_set(vself, VNINFO, vinformer);
  est_mtdb_set_informer(db->db, db_informer, (void *)vinformer);
  return Qnil;
}


static VALUE cblisttoobj(const CBLIST *list){
  const char *vbuf;
  int i, vsiz;
  VALUE obj;
  obj = rb_ary_new2(cblistnum(list));
  for(i = 0; i < cblistnum(list); i++){
    vbuf = cblistval(list, i, &vsiz);
    rb_ary_store(obj, i, rb_str_new(vbuf, vsiz));
  }
  return obj;
}


static CBLIST *objtocblist(VALUE obj){
  CBLIST *list;
  VALUE str;
  int i, len;
  list = cblistopen();
  len = RARRAY(obj)->len;
  for(i = 0; i < len; i++){
    str = rb_ary_entry(obj, i);
    cblistpush(list, RSTRING(str)->ptr, RSTRING(str)->len);
  }
  return list;
}


static VALUE cbmaptoobj(CBMAP *map){
  const char *kbuf, *vbuf;
  int ksiz, vsiz;
  VALUE obj;
  obj = rb_hash_new();
  cbmapiterinit(map);
  while((kbuf = cbmapiternext(map, &ksiz)) != NULL){
    vbuf = cbmapiterval(kbuf, &vsiz);
    rb_hash_aset(obj, rb_str_new(kbuf, ksiz), rb_str_new(vbuf, vsiz));
  }
  return obj;
}


static CBMAP *objtocbmap(VALUE obj){
  CBMAP *map;
  VALUE keys, key, val;
  int i, len;
  map = cbmapopenex(31);
  keys = rb_funcall(obj, rb_intern("keys"), 0);
  len = RARRAY(keys)->len;
  for(i = 0; i < len; i++){
    key = rb_ary_entry(keys, i);
    val = rb_hash_aref(obj, key);
    key = rb_String(key);
    val = rb_String(val);
    cbmapput(map, RSTRING(key)->ptr, RSTRING(key)->len,
             RSTRING(val)->ptr, RSTRING(val)->len, 0);
  }
  return map;
}


static void db_informer(const char *message, void *opaque){
  VALUE ary;
  ary = rb_ary_new2(2);
  rb_ary_push(ary, (VALUE)opaque);
  rb_ary_push(ary, rb_str_new2(message));
  rb_rescue(db_informer_process, ary, db_informer_resque, Qnil);
}


static VALUE db_informer_process(VALUE arg){
  VALUE informer, message;
  informer = rb_ary_shift(arg);
  message = rb_ary_shift(arg);
  rb_funcall(informer, rb_intern("inform"), 1, message);
  return Qnil;
}


static VALUE db_informer_resque(VALUE arg){
  return Qnil;
}



/* END OF FILE */
