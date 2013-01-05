#include <estraier.h>
#include <estnode.h>
#include <cabin.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv){
  ESTNODE *node;
  ESTCOND *cond;
  ESTNODERES *nres;
  ESTRESDOC *rdoc;
  int i;
  const char *value;
  /* initialize the network environment */
  if(!est_init_net_env()){
    fprintf(stderr, "error: network is unavailable\n");
    return 1;
  }
  /* create the node connection object */
  node = est_node_new("http://localhost:1978/node/test1");
  /* create a search condition object */
  cond = est_cond_new();
  /* set the search phrase to the search condition object */
  est_cond_set_phrase(cond, "rainbow AND lullaby");
  /* get the result of search */
  nres = est_node_search(node, cond, 0);
  if(nres){
    /* for each document in the result */
    for(i = 0; i < est_noderes_doc_num(nres); i++){
      /* get a result document object */
      rdoc = est_noderes_get_doc(nres, i);
      /* display attributes */
      if((value = est_resdoc_attr(rdoc, "@uri")) != NULL)
        printf("URI: %s\n", value);
      if((value = est_resdoc_attr(rdoc, "@title")) != NULL)
        printf("Title: %s\n", value);
      /* display the snippet text */
      printf("%s", est_resdoc_snippet(rdoc));
    }
    /* delete the node result object */
    est_noderes_delete(nres);
  } else {
    fprintf(stderr, "error: %d\n", est_node_status(node));
  }
  /* destroy the search condition object */
  est_cond_delete(cond);
  /* destroy the node object */
  est_node_delete(node);
  /* free the networking environment */
  est_free_net_env();
  return 0;
}
