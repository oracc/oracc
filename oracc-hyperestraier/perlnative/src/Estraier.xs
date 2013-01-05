/*************************************************************************************************
 * Perl binding of Hyper Estraier
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


#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include <estraier.h>
#include <estmtdb.h>
#include <cabin.h>
#include <stdlib.h>



/*************************************************************************************************
 * private objects
 *************************************************************************************************/


static AV *cblisttoav(const CBLIST *values);
static CBLIST *avtocblist(AV *av);
static HV *cbmaptohv(CBMAP *kwords);
static CBMAP *hvtocbmap(HV *hv);
static void dbinform(const char *message, void *opaque);


static AV *cblisttoav(const CBLIST *values){
  AV *av;
  const char *buf;
  int i, size;
  av = newAV();
  for(i = 0; i < cblistnum(values); i++){
    buf = cblistval(values, i, &size);
    av_push(av, newSVpvn(buf, size));
  }
  return av;
}


static CBLIST *avtocblist(AV *av){
  CBLIST *list;
  SV *val;
  STRLEN size;
  char *buf;
  int i, num;
  list = cblistopen();
  if((num = av_len(av)) < 0) return list;
  for(i = 0; i <= num; i++){
    val = *av_fetch(av, i, 0);
    buf = SvPV(val, size);
    cblistpush(list, buf, (int)size);
  }
  return list;
}


static HV *cbmaptohv(CBMAP *map){
  HV *hv;
  const char *kbuf, *vbuf;
  int ksiz, vsiz;
  hv = newHV();
  cbmapiterinit(map);
  while((kbuf = cbmapiternext(map, &ksiz)) != NULL){
    vbuf = cbmapiterval(kbuf, &vsiz);
    hv_store(hv, kbuf, ksiz, newSVpvn(vbuf, vsiz), 0);
  }
  return hv;
}


static CBMAP *hvtocbmap(HV *hv){
  HE *entry;
  STRLEN vsiz;
  CBMAP *map;
  char *kbuf, *vbuf;
  SV *val;
  I32 ksiz;
  map = cbmapopenex(31);
  hv_iterinit(hv);
  while((entry = hv_iternext(hv)) != NULL){
    kbuf = hv_iterkey(entry, &ksiz);
    val = hv_iterval(hv, entry);
    vbuf = SvPV(val, vsiz);
    cbmapput(map, kbuf, (int)ksiz, vbuf, (int)vsiz, FALSE);
  }
  return map;
}


static void dbinform(const char *message, void *opaque){
  dSP;
  ENTER;
  SAVETMPS;
  PUSHMARK(SP);
  XPUSHs(sv_2mortal(newSVpv(message, 0)));
  PUTBACK;
  perl_call_pv(opaque, G_DISCARD | G_EVAL);
  FREETMPS;
  LEAVE;
}



/*************************************************************************************************
 * public objects
 *************************************************************************************************/


MODULE = Estraier		PACKAGE = Estraier
PROTOTYPES: DISABLE


void *
doc_new()
CODE:
	RETVAL = est_doc_new();
OUTPUT:
	RETVAL


void *
doc_new_from_draft(draft)
	char *	draft
CODE:
	RETVAL = est_doc_new_from_draft(draft);
OUTPUT:
	RETVAL


void
doc_delete(doc)
	void *	doc
CODE:
	est_doc_delete(doc);


void
doc_add_attr(doc, name, value)
	void *	doc
	char *	name
	char *	value
CODE:
	if(!strcmp(value, "\t(NULL)\t")) value = NULL;
	est_doc_add_attr(doc, name, value);


void
doc_add_text(doc, text)
	void *	doc
	char *	text
CODE:
	est_doc_add_text(doc, text);


void
doc_add_hidden_text(doc, text)
	void *	doc
	char *	text
CODE:
	est_doc_add_hidden_text(doc, text);


void
doc_set_keywords(doc, kwords)
	void *	doc
	HV *	kwords
PREINIT:
	CBMAP *tkwords;
CODE:
	tkwords = hvtocbmap(kwords);
	est_doc_set_keywords(doc, tkwords);
	cbmapclose(tkwords);


void
doc_set_score(doc, score)
	void *	doc
	int	score
CODE:
	est_doc_set_score(doc, score);


int
doc_id(doc)
	void *	doc
CODE:
	RETVAL = est_doc_id(doc);
OUTPUT:
	RETVAL


void
doc_attr_names(doc)
	void *	doc
PREINIT:
	CBLIST *names;
PPCODE:
	names = est_doc_attr_names(doc);
	XPUSHs(sv_2mortal(newRV_noinc((SV *)cblisttoav(names))));
	cblistclose(names);
	XSRETURN(1);


const char *
doc_attr(doc, name)
	void *	doc
	char *	name
CODE:
	RETVAL = est_doc_attr(doc, name);
OUTPUT:
	RETVAL


void
doc_texts(doc)
	void *	doc
PREINIT:
	const CBLIST *texts;
PPCODE:
	texts = est_doc_texts(doc);
	XPUSHs(sv_2mortal(newRV_noinc((SV *)cblisttoav(texts))));
	XSRETURN(1);


void
doc_cat_texts(doc)
	void *	doc
PREINIT:
	char *texts;
PPCODE:
	texts = est_doc_cat_texts(doc);
	XPUSHs(sv_2mortal(newSVpv(texts, 0)));
	free(texts);
	XSRETURN(1);


void
doc_keywords(doc)
	void *	doc
PREINIT:
	CBMAP *kwords;
PPCODE:
	if((kwords = est_doc_keywords(doc)) != NULL){
	  XPUSHs(sv_2mortal(newRV_noinc((SV *)cbmaptohv(kwords))));
	} else {
	  XPUSHs((SV *)&PL_sv_undef);
	}
	XSRETURN(1);


int
doc_score(doc)
	void *	doc
CODE:
	RETVAL = est_doc_score(doc);
OUTPUT:
	RETVAL


void
doc_make_snippet(doc, words, wwidth, hwidth, awidth)
	void *	doc
	AV *	words
	int	wwidth
	int	hwidth
	int	awidth
PREINIT:
	CBLIST *twords;
	char *snippet;
PPCODE:
	twords = avtocblist(words);
	snippet = est_doc_make_snippet(doc, twords, wwidth, hwidth, awidth);
	XPUSHs(sv_2mortal(newSVpv(snippet, 0)));
	free(snippet);
	cblistclose(twords);
	XSRETURN(1);


void
doc_dump_draft(doc)
	void *	doc
PREINIT:
	char *draft;
PPCODE:
	draft = est_doc_dump_draft(doc);
	XPUSHs(sv_2mortal(newSVpv(draft, 0)));
	free(draft);
	XSRETURN(1);


void *
cond_new()
PREINIT:
	void *cond;
CODE:
	cond = est_cond_new();
	est_cond_set_options(cond, ESTCONDSCFB);
	RETVAL = cond;
OUTPUT:
	RETVAL


void
cond_delete(cond)
	void *	cond
CODE:
	est_cond_delete(cond);


void
cond_set_phrase(cond, phrase)
	void *	cond
	char *	phrase
CODE:
	est_cond_set_phrase(cond, phrase);


void
cond_add_attr(cond, expr)
	void *	cond
	char *	expr
CODE:
	est_cond_add_attr(cond, expr);


void
cond_set_order(cond, expr)
	void *	cond
	char *	expr
CODE:
	est_cond_set_order(cond, expr);


void
cond_set_max(cond, max)
	void *	cond
	int	max
CODE:
	est_cond_set_max(cond, max);


void
cond_set_skip(cond, skip)
	void *	cond
	int	skip
CODE:
	est_cond_set_skip(cond, skip);


void
cond_set_options(cond, options)
	void *	cond
	int	options
CODE:
	est_cond_set_options(cond, options);


void
cond_set_auxiliary(cond, min)
	void *	cond
	int	min
CODE:
	est_cond_set_auxiliary(cond, min);


void
cond_set_eclipse(cond, limit)
	void *	cond
	double	limit
CODE:
	est_cond_set_eclipse(cond, limit);


void
cond_set_distinct(cond, name)
	void *	cond
	char *	name
CODE:
	est_cond_set_distinct(cond, name);


void
res_delete(resptr, idxsptr, hints, cond)
	void *	resptr
	void *	idxsptr
	void *	hints
	void *	cond
CODE:
	est_cond_delete(cond);
	cbmapclose(hints);
	free(idxsptr);
	free(resptr);


int
res_get_doc_id(resptr, index)
	void *	resptr
	int	index
CODE:
	RETVAL = ((int *)resptr)[index];
OUTPUT:
	RETVAL


int
res_get_dbidx(idxsptr, index)
	void *	idxsptr
	int	index
CODE:
	RETVAL = ((int *)idxsptr)[index];
OUTPUT:
	RETVAL


void
res_hint_words(hints)
	void *	hints
PREINIT:
	CBLIST *words;
	const char *vbuf;
	int i;
PPCODE:
	words = cbmapkeys(hints);
	for(i = 0; i < cblistnum(words); i++){
	  vbuf = cblistval(words, i, NULL);
	  if(vbuf[0] == '\0'){
	    free(cblistremove(words, i, NULL));
	    break;
	  }
	}
	XPUSHs(sv_2mortal(newRV_noinc((SV *)cblisttoav(words))));
	cblistclose(words);
	XSRETURN(1);


int
res_hint(hints, word)
	void *	hints
	char *	word
PREINIT:
	const char *value;
CODE:
	value = cbmapget(hints, word, -1, NULL);
	RETVAL = value ? atoi(value) : 0;
OUTPUT:
	RETVAL


int
res_get_score(cond, index)
	void *	cond
	int	index
CODE:
	RETVAL = est_cond_score(cond, index);
OUTPUT:
	RETVAL


void
res_get_shadows(cond, id)
	void *	cond
	int	id
PREINIT:
	const int *ary;
	int i, anum;
	AV *av;
PPCODE:
	ary = est_cond_shadows(cond, id, &anum);
	av = newAV();
	for(i = 0; i < anum; i++){
	  av_push(av, newSViv(ary[i]));
	}
	XPUSHs(sv_2mortal(newRV_noinc((SV *)av)));
	XSRETURN(1);


const char *
db_version()
CODE:
	RETVAL = est_version;
OUTPUT:
	RETVAL


void
db_search_meta(dbav, cond)
	AV *	dbav
        void *  cond
PREINIT:
	ESTMTDB **dbs;
	CBMAP *hints;
	int i, dbnum, *res, rnum, *idxs;
PPCODE:
	dbnum = av_len(dbav) + 1;
	dbs = cbmalloc(dbnum * sizeof(ESTMTDB *) + 1);
	for(i = 0; i < dbnum; i++){
	  dbs[i] = (ESTMTDB *)SvIV(*av_fetch(dbav, i, 0));
	}
	hints = cbmapopenex(31);
	res = est_mtdb_search_meta(dbs, dbnum, cond, &rnum, hints);
	idxs = cbmalloc(rnum / 2 * sizeof(int) + 1);
	for(i = 0; i < rnum; i += 2){
	  idxs[i/2] = res[i];
	  res[i/2] = res[i+1];
	}
	free(dbs);
	XPUSHs(sv_2mortal(newSViv((IV)res)));
	XPUSHs(sv_2mortal(newSViv((IV)idxs)));
	XPUSHs(sv_2mortal(newSViv((IV)(rnum / 2))));
	XPUSHs(sv_2mortal(newSViv((IV)hints)));
	XPUSHs(sv_2mortal(newSViv((IV)est_cond_dup(cond))));
	XSRETURN(5);


const char *
db_err_msg(ecode)
	int	ecode
CODE:
	RETVAL = est_err_msg(ecode);
OUTPUT:
	RETVAL


void
db_open(name, omode)
	char *	name
	int	omode
PREINIT:
	void *	db;
	int	ecode;
PPCODE:
	db = est_mtdb_open(name, omode, &ecode);
	XPUSHs(sv_2mortal(newSViv((IV)db)));
	XPUSHs(sv_2mortal(newSViv((IV)ecode)));
	XSRETURN(2);


void
db_close(db)
	void *	db
PREINIT:
	int ecode, rv;
PPCODE:
	rv = est_mtdb_close(db, &ecode);
	XPUSHs(sv_2mortal(newSViv((IV)rv)));
	XPUSHs(sv_2mortal(newSViv((IV)ecode)));
	XSRETURN(2);


int
db_error(db)
	void *	db
CODE:
	RETVAL = est_mtdb_error(db);
OUTPUT:
	RETVAL


int
db_fatal(db)
	void *	db
CODE:
	RETVAL = est_mtdb_fatal(db);
OUTPUT:
	RETVAL


int
db_add_attr_index(db, name, type)
	void *	db
	char *	name
	int	type
CODE:
	RETVAL = est_mtdb_add_attr_index(db, name, type);
OUTPUT:
	RETVAL


int
db_flush(db, max)
	void *	db
	int	max
CODE:
	RETVAL = est_mtdb_flush(db, max);
OUTPUT:
	RETVAL


int
db_sync(db)
	void *	db
CODE:
	RETVAL = est_mtdb_sync(db);
OUTPUT:
	RETVAL


int
db_optimize(db, options)
	void *	db
	int	options
CODE:
	RETVAL = est_mtdb_optimize(db, options);
OUTPUT:
	RETVAL


int
db_merge(db, name, options)
	void *	db
	char *	name
	int	options
CODE:
	RETVAL = est_mtdb_merge(db, name, options);
OUTPUT:
	RETVAL


int
db_put_doc(db, doc, options)
	void *	db
	void *	doc
	int	options
CODE:
	RETVAL = est_mtdb_put_doc(db, doc, options);
OUTPUT:
	RETVAL


int
db_out_doc(db, id, options)
	void *	db
	int	id
	int	options
CODE:
	RETVAL = est_mtdb_out_doc(db, id, options);
OUTPUT:
	RETVAL


int
db_edit_doc(db, doc)
	void *	db
	void *	doc
CODE:
	RETVAL = est_mtdb_edit_doc(db, doc);
OUTPUT:
	RETVAL


void *
db_get_doc(db, id, options)
	void *	db
	int	id
	int	options
CODE:
	RETVAL = est_mtdb_get_doc(db, id, options);
OUTPUT:
	RETVAL


void
db_get_doc_attr(db, id, name)
	void *	db
	int	id
	char *	name
PREINIT:
	char *value;
PPCODE:
	value = est_mtdb_get_doc_attr(db, id, name);
	if(!value) XSRETURN_UNDEF;
	XPUSHs(sv_2mortal(newSVpv(value, 0)));
	free(value);
	XSRETURN(1);


int
db_uri_to_id(db, uri)
	void *	db
	char *	uri
CODE:
	RETVAL = est_mtdb_uri_to_id(db, uri);
OUTPUT:
	RETVAL


const char *
db_name(db)
	void *	db
CODE:
	RETVAL = est_mtdb_name(db);
OUTPUT:
	RETVAL


int
db_doc_num(db)
	void *	db
CODE:
	RETVAL = est_mtdb_doc_num(db);
OUTPUT:
	RETVAL


int
db_word_num(db)
	void *	db
CODE:
	RETVAL = est_mtdb_word_num(db);
OUTPUT:
	RETVAL


double
db_size(db)
	void *	db
CODE:
	RETVAL = est_mtdb_size(db);
OUTPUT:
	RETVAL


void
db_search(db, cond)
	void *	db
	void *	cond
PREINIT:
	CBMAP *hints;
	int *res;
	int rnum;
PPCODE:
	hints = cbmapopenex(31);
	res = est_mtdb_search(db, cond, &rnum, hints);
	XPUSHs(sv_2mortal(newSViv((IV)res)));
	XPUSHs(sv_2mortal(newSViv((IV)rnum)));
	XPUSHs(sv_2mortal(newSViv((IV)hints)));
	XPUSHs(sv_2mortal(newSViv((IV)est_cond_dup(cond))));
	XSRETURN(4);


int
db_scan_doc(db, doc, cond)
	void *	db
	void *	doc
	void *	cond
CODE:
	RETVAL = est_mtdb_scan_doc(db, doc, cond);
OUTPUT:
	RETVAL


void
db_set_cache_size(db, size, anum, tnum, rnum)
	void *	db
	double	size
	int	anum
	int	tnum
	int	rnum
CODE:
	est_mtdb_set_cache_size(db, (size_t)size, anum, tnum, rnum);


int
db_add_pseudo_index(db, path)
	void *	db
	char *	path
CODE:
	RETVAL = est_mtdb_add_pseudo_index(db, path);
OUTPUT:
	RETVAL


void
db_set_wildmax(db, num)
	void *	db
	int	num
CODE:
	est_mtdb_set_wildmax(db, num);


void
db_set_informer(db, informer)
	void *	db
	char *	informer
CODE:
	est_mtdb_set_informer(db, dbinform, informer);



## END OF FILE
