#include <estraier.h>
#include <cabin.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv){
  ESTDB *db;
  ESTCOND *cond;
  ESTDOC *doc;
  const CBLIST *texts;
  int ecode, *result, resnum, i, j;
  const char *value;

  /* open the database */
  if(!(db = est_db_open("casket", ESTDBREADER, &ecode))){
    fprintf(stderr, "error: %s\n", est_err_msg(ecode));
    return 1;
  }

  /* create a search condition object */
  cond = est_cond_new();

  /* set the search phrase to the search condition object */
  est_cond_set_phrase(cond, "rainbow AND lullaby");

  /* get the result of search */
  result = est_db_search(db, cond, &resnum, NULL);

  /* for each document in the result */
  for(i = 0; i < resnum; i++){

    /* retrieve the document object */
    if(!(doc = est_db_get_doc(db, result[i], 0))) continue;

    /* display attributes */
    if((value = est_doc_attr(doc, "@uri")) != NULL)
      printf("URI: %s\n", value);
    if((value = est_doc_attr(doc, "@title")) != NULL)
      printf("Title: %s\n", value);

    /* display the body text */
    texts = est_doc_texts(doc);
    for(j = 0; j < cblistnum(texts); j++){
      value = cblistval(texts, j, NULL);
      printf("%s\n", value);
    }

    /* destroy the document object */
    est_doc_delete(doc);

  }

  /* free the result of search */
  free(result);

  /* destroy the search condition object */
  est_cond_delete(cond);

  /* close the database */
  if(!est_db_close(db, &ecode)){
    fprintf(stderr, "error: %s\n", est_err_msg(ecode));
    return 1;
  }

  return 0;
}
