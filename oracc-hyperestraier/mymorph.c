/*************************************************************************************************
 * Morphological analyzers for Hyper Estraier
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


#include "mymorph.h"
#include "estraier.h"
#include "myconf.h"

#if defined(_MYMECAB)
#include <mecab.h>
#endif

#define ESTNUMBUFSIZ   32                /* size of a buffer for a number */
#define ESTSCOREUNIT   100               /* unit of score of each word */
#define ESTWORDMAXLEN  48                /* maximum length of a word */

typedef struct {                         /* type of structure for a hitting object */
  const char *word;                      /* face of keyword */
  int pt;                                /* score tuned by TF-IDF */
} ESTKEYSC;


/* private function prototypes */
static void est_uwords_add(CBMAP *uwords, const char *word, int wsiz, int pt);
static int est_keysc_compare(const void *ap, const void *bp);
static int est_mecab_analyze(const char *text, CBMAP *uwords);



/*************************************************************************************************
 * pseudo API
 *************************************************************************************************/


/* Extract keywords of a document object using morphological analyzers. */
CBMAP *est_morph_etch_doc(ESTDOC *doc, int max){
  ESTKEYSC *scores;
  CBMAP *kwords, *uwords;
  const CBLIST *texts;
  CBLIST *words;
  const char *text, *word;
  const unsigned char *uword;
  char numbuf[ESTNUMBUFSIZ];
  char *enc;
  int i, j, all, miss, wsiz, pt, snum;
  double dval;
  assert(doc && max >= 0);
  kwords = cbmapopenex(max * 2 + 1);
  uwords = cbmapopen();
  all = 0;
  texts = est_doc_texts(doc);
  for(i = -1; i < cblistnum(texts); i++){
    text = i < 0 ? est_doc_hidden_texts(doc) : cblistval(texts, i, NULL);
    if(text[0] == '\0') continue;
    if(ESTUSEMECAB && (enc = est_iconv(text, -1, "UTF-8", "EUC-JP", NULL, &miss)) != NULL){
      if(miss > 0){
        free(enc);
      } else {
        all += est_mecab_analyze(enc, uwords);
        free(enc);
        continue;
      }
    }
    words = cblistopen();
    est_break_text_chrcat(text, words, FALSE);
    for(j = 0; j < cblistnum(words); j++){
      uword = (unsigned char *)cblistval(words, j, &wsiz);
      if(uword[0] >= 0xe3){
        if(wsiz <= 3){
          pt = ESTSCOREUNIT / 4;
          if((uword[0] == 0xe3 && (uword[1] == 0x80 || uword[1] == 0x81 ||
                                   (uword[1] == 0x82 && uword[2] <= 0x9f))) ||
             (uword[0] == 0xef && uword[1] >= 0xbc)) pt /= 10;
        } else {
          pt = ESTSCOREUNIT;
          if((uword[0] == 0xe3 && (uword[1] == 0x80 || uword[1] == 0x81 ||
                                   (uword[1] == 0x82 && uword[2] <= 0x9f))) ||
             (uword[0] == 0xef && uword[1] >= 0xbc)) pt /= 2;
          if((uword[3] == 0xe3 && (uword[4] == 0x80 || uword[4] == 0x81 ||
                                   (uword[4] == 0x82 && uword[5] <= 0x9f))) ||
             (uword[3] == 0xef && uword[4] >= 0xbc)) pt /= 2;
        }
      } else if((uword[0] > '\0' && uword[0] <= '/') || (uword[0] >= '[' && uword[0] <= '`') ||
                (uword[0] >= '{' && uword[0] <= '~')){
        pt = ESTSCOREUNIT / 50;
      } else {
        switch(wsiz){
        case 1:
          pt = ESTSCOREUNIT / 9;
          break;
        case 2:
          pt = ESTSCOREUNIT / 5;
          break;
        case 3:
          pt = ESTSCOREUNIT / 3;
          break;
        case 4:
          pt = ESTSCOREUNIT / 2;
          break;
        case 5:
          pt = ESTSCOREUNIT / 1.5;
          break;
        case 6:
          pt = ESTSCOREUNIT / 1.25;
          break;
        default:
          pt = ESTSCOREUNIT;
          break;
        }
      }
      est_uwords_add(uwords, (char *)uword, wsiz, pt);
      all += pt;
    }
    cblistclose(words);
  }
  scores = cbmalloc(cbmaprnum(uwords) * sizeof(ESTKEYSC) + 1);
  snum = 0;
  cbmapiterinit(uwords);
  while((word = cbmapiternext(uwords, &wsiz)) != NULL){
    scores[snum].word = word;
    scores[snum].pt = *(int *)cbmapget(uwords, word, wsiz, NULL);
    snum++;
  }
  qsort(scores, snum, sizeof(ESTKEYSC), est_keysc_compare);
  dval = log(all + ESTSCOREUNIT * 64);
  dval *= dval;
  dval /= 10;
  for(i = 0; i < snum &&  i < max; i++){
    pt = (int)(scores[i].pt / dval);
    wsiz = sprintf(numbuf, "%d", pt > 1 ? pt : 1);
    cbmapput(kwords, scores[i].word, -1, numbuf, wsiz, FALSE);
  }
  free(scores);
  cbmapclose(uwords);
  return kwords;
}



/*************************************************************************************************
 * private objects
 *************************************************************************************************/


/* Add a record to a unique word map.
   `uwords' specifies a unique word map.
   `word' specifies a word.
   `wsiz' specifies size of the word.
   `pt' specifies point of the word. */
static void est_uwords_add(CBMAP *uwords, const char *word, int wsiz, int pt){
  const char *vbuf;
  int num;
  assert(uwords && word && wsiz >= 0);
  if(wsiz > ESTWORDMAXLEN) return;
  num = (vbuf = cbmapget(uwords, word, wsiz, NULL)) ? *(int *)vbuf + pt : pt;
  if(num < 0) num = 0;
  cbmapput(uwords, word, wsiz, (char *)&num, sizeof(int), TRUE);
}


/* Compare two keywords by scores in descending order.
   `ap' specifies the pointer to one keyword.
   `bp' specifies the pointer to the other keyword.
   The return value is negative if one is small, positive if one is big, 0 if both are equal. */
static int est_keysc_compare(const void *ap, const void *bp){
  assert(ap && bp);
  return ((ESTKEYSC *)bp)->pt - ((ESTKEYSC *)ap)->pt;
}


/* Analyze text with using mecab.
   `text' specifies a sentence whose encoding is EUC-JP.
   `uwords' specifies a unique word map.
   The return value is the total points of extracted words. */
static int est_mecab_analyze(const char *text, CBMAP *uwords){
#if defined(_MYMECAB)
  static mecab_t *mecab = NULL;
  static CBMAP *swmap = NULL;
  static char *args[] = { "mecab", "--node-format=%m\\t%f[0]\\t%f[1]\\n",
                          "--unk-format=%m\\t\\t\\n", "--eos-format=", NULL };
  static char *stops[] = {
    "a", "an", "the", "there", "any", "no", "each", "all", "most", "more",
    "i", "my", "me", "mine", "you", "your", "yours", "we", "our", "us", "ours",
    "he", "his", "him", "she", "her", "hers", "it", "its", "they", "their", "them", "theirs",
    "this", "these", "that", "those",
    "what", "who", "which", "where", "when", "why", "whose", "how",
    "or", "and", "but", "not", "as", "if", "so", "because", "as", "unless", "until", "while",
    "to", "on", "of", "in", "for", "from", "by", "with", "at", "against", "via", "under", "over",
    "be", "am", "are", "is", "was", "were", "being", "wanna", "gonna", "gotta", "going",
    "do", "does", "did", "have", "has", "had", "get", "got", "go", "went", "come", "came",
    "will", "would", "ll", "ve", "can", "could", "may", "might", "shall", "should", "must",
    NULL
  };
  CBDATUM *prefix;
  const char *cres;
  unsigned char *uword;
  char *res, *pv, *ep, *word, *sign, *info;
  int i, all, ppt, pnum, wsiz, pt;
  assert(text && uwords);
  if(!mecab){
    if(!(mecab = mecab_new(sizeof(args) / sizeof(char *) - 1, args))) return 0;
    /* for (maybe) mecab's bug
      cbglobalgc(mecab, (void (*)(void *))mecab_destroy);
    */
    swmap = cbmapopenex(sizeof(stops) / sizeof(char *) * 2 + 1);
    cbglobalgc(swmap, (void (*)(void *))cbmapclose);
    for(i = 0; stops[i]; i++){
      cbmapput(swmap, stops[i], -1, "", 0, FALSE);
    }
  }
  if(!(cres = mecab_sparse_tostr(mecab, (char *)text))) return 0;
  if(!(res = est_iconv(cres, -1, "EUC-JP", "UTF-8", NULL, NULL))) return 0;
  all = 0;
  pv = res;
  ep = pv;
  prefix = cbdatumopen(NULL, -1);
  pnum = 0;
  ppt = 0;
  while(ep){
    if((ep = strchr(pv, '\n')) != NULL) *ep = '\0';
    word = pv;
    if((sign = strchr(pv, '\t')) != NULL){
      *(sign++) = '\0';
      pv = sign;
    } else {
      sign = "";
    }
    if((info = strchr(pv, '\t')) != NULL){
      *(info++) = '\0';
      pv = info;
    } else {
      info = "";
    }
    for(pv = word; *pv != '\0'; pv++){
      if(*pv >= 'A' && *pv <= 'Z') *pv += 'a' - 'A';
    }
    wsiz = pv - word;
    if(wsiz > 0){
      uword = (unsigned char *)word;
      if(!strcmp(sign, "記号")){
        pt = ESTSCOREUNIT / 100;
      } else if(!strcmp(sign, "助詞") || !strcmp(sign, "助動詞") || !strcmp(sign, "接頭詞") ||
                !strcmp(sign, "接続詞") || !strcmp(sign, "フィラー") || !strcmp(sign, "その他")){
        pt = ESTSCOREUNIT / 50;
      } else if(!strcmp(sign, "名詞") || !strcmp(sign, "動詞")){
        pt = ESTSCOREUNIT;
        if(!strcmp(info, "非自立") || !strcmp(info, "特殊") || !strcmp(info, "引用文字列") ||
           !strcmp(info, "数") || !strcmp(info, "接続詞的") || !strcmp(info, "接尾") ||
           !strcmp(info, "代名詞") || !strcmp(info, "動詞非自立的")){
          pt /= 2;
        }
        if(wsiz <= 3){
          pt /= 5;
          if(uword[0] == 0xe3 && (uword[1] == 0x80 || uword[1] == 0x81 ||
                                  (uword[1] == 0x82 && uword[2] <= 0x9f))) pt /= 5;
        } else {
          if(uword[0] == 0xe3 && (uword[1] == 0x80 || uword[1] == 0x81 ||
                                  (uword[1] == 0x82 && uword[2] <= 0x9f))) pt /= 5;
          if(uword[3] == 0xe3 && (uword[4] == 0x80 || uword[4] == 0x81 ||
                                  (uword[4] == 0x82 && uword[5] <= 0x9f))) pt /= 2.5;
          if(wsiz <= 6) pt /= 1.5;
        }
      } else if(!strcmp(sign, "形容詞") || !strcmp(sign, "副詞") || !strcmp(sign, "連体詞") ||
                !strcmp(sign, "感動詞")){
        pt = ESTSCOREUNIT / 2;
        if(wsiz <= 3){
          pt /= 5;
          if(uword[0] == 0xe3 && (uword[1] == 0x80 || uword[1] == 0x81 ||
                                  (uword[1] == 0x82 && uword[2] <= 0x9f))) pt /= 5;
        } else {
          if(uword[0] == 0xe3 && (uword[1] == 0x80 || uword[1] == 0x81 ||
                                  (uword[1] == 0x82 && uword[2] <= 0x9f))) pt /= 5;
          if(uword[3] == 0xe3 && (uword[4] == 0x80 || uword[4] == 0x81 ||
                                  (uword[4] == 0x82 && uword[5] <= 0x9f))) pt /= 2.5;
          if(wsiz <= 6) pt /= 1.5;
        }
      } else if((word[0] > '\0' && word[0] <= '/') || (word[0] >= ':' && word[0] <= '@') ||
                (word[0] >= '[' && word[0] <= '`') || (word[0] >= '{' && word[0] <= '~')){
        pt = ESTSCOREUNIT / 50;
        if(wsiz <= 1) pt /= 2;
      } else if(cbmapget(swmap, word, wsiz, NULL)){
        pt = ESTSCOREUNIT / 25;
        if(wsiz <= 1) pt /= 2;
      } else {
        switch(wsiz){
        case 1:
          pt = ESTSCOREUNIT / 9;
          break;
        case 2:
          pt = ESTSCOREUNIT / 5;
          break;
        case 3:
          pt = ESTSCOREUNIT / 3;
          break;
        case 4:
          pt = ESTSCOREUNIT / 2;
          break;
        case 5:
          pt = ESTSCOREUNIT / 1.5;
          break;
        case 6:
          pt = ESTSCOREUNIT / 1.25;
          break;
        default:
          pt = ESTSCOREUNIT;
          break;
        }
      }
      est_uwords_add(uwords, (char *)uword, wsiz, pt);
      all += pt;
      if(!strcmp(sign, "名詞")){
        ppt += pt;
        pnum++;
        if(cbdatumsize(prefix) > 0){
          est_uwords_add(uwords, cbdatumptr(prefix), cbdatumsize(prefix),
                         (double)ppt / pnum * -0.15 - 1);
          est_uwords_add(uwords, (char *)uword, wsiz, pt * -0.15 - 1);
          cbdatumcat(prefix, word, wsiz);
          est_uwords_add(uwords, cbdatumptr(prefix), cbdatumsize(prefix),
                         ((double)ppt / pnum) * 0.9);
          all += (ppt / pnum) * 0.8;
        } else {
          cbdatumcat(prefix, word, wsiz);
        }
      } else if(!strcmp(sign, "") && (cbdatumsize(prefix) < 1 || uword[0] >= 0xc0)){
        ppt += pt * 0.8;
        pnum++;
        if(cbdatumsize(prefix) > 0){
          est_uwords_add(uwords, cbdatumptr(prefix), cbdatumsize(prefix),
                         (double)ppt / pnum * -0.15 - 1);
          est_uwords_add(uwords, (char *)uword, wsiz, pt * -0.15 - 1);
          cbdatumcat(prefix, word, wsiz);
          est_uwords_add(uwords, cbdatumptr(prefix), cbdatumsize(prefix),
                         ((double)ppt / pnum) * 0.9);
          all += (ppt / pnum) * 0.8;
        } else if(wsiz > 1 || uword[0] >= 0x80 || (word[0] >= '0' && word[0] <= '9') ||
                  (word[0] >= 'A' && word[0] <= 'Z') || (word[0] >= 'a' && word[0] <= 'z')){
          cbdatumcat(prefix, word, wsiz);
        } else {
          ppt = 0;
          pnum = 0;
        }
      } else {
        cbdatumsetsize(prefix, 0);
        pnum = 0;
        ppt = 0;
      }
    } else {
      cbdatumsetsize(prefix, 0);
      pnum = 0;
      ppt = 0;
    }
    pv = ep + 1;
  }
  cbdatumclose(prefix);
  free(res);
  return all;
#else
  return 0;
#endif
}



/* END OF FILE */
