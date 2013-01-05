#include <estraier.h>
#include <cabin.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv){
  ESTDB *db;
  ESTDOC *doc;
  int ecode;

  /* open the database */
  if(!(db = est_db_open("casket", ESTDBWRITER | ESTDBCREAT, &ecode))){
    fprintf(stderr, "error: %s\n", est_err_msg(ecode));
    return 1;
  }

  /* create a document object */
  doc = est_doc_new();

  /* add attributes to the document object */
  est_doc_add_attr(doc, "@uri", "http://estraier.gov/example.txt");
  est_doc_add_attr(doc, "@title", "Over the Rainbow");

  /* add the body text to the document object */
  est_doc_add_text(doc, "Somewhere over the rainbow.  Way up high.");
  est_doc_add_text(doc, "There's a land that I heard of once in a lullaby.");

  /* register the document object to the database */
  if(!est_db_put_doc(db, doc, ESTPDCLEAN))
    fprintf(stderr, "error: %s\n", est_err_msg(est_db_error(db)));

  /* destroy the document object */
  est_doc_delete(doc);

  /* close the database */
  if(!est_db_close(db, &ecode)){
    fprintf(stderr, "error: %s\n", est_err_msg(ecode));
    return 1;
  }

  return 0;
}
