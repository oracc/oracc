#include <estraier.h>
#include <estnode.h>
#include <cabin.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv){
  ESTNODE *node;
  ESTDOC *doc;
  /* initialize the network environment */
  if(!est_init_net_env()){
    fprintf(stderr, "error: network is unavailable\n");
    return 1;
  }
  /* create and configure the node connection object */
  node = est_node_new("http://localhost:1978/node/test1");
  est_node_set_auth(node, "admin", "admin");
  /* create a document object */
  doc = est_doc_new();
  /* add attributes to the document object */
  est_doc_add_attr(doc, "@uri", "http://estraier.gov/example.txt");
  est_doc_add_attr(doc, "@title", "Over the Rainbow");
  /* add the body text to the document object */
  est_doc_add_text(doc, "Somewhere over the rainbow.  Way up high.");
  est_doc_add_text(doc, "There's a land that I heard of once in a lullaby.");
  /* register the document object to the node */
  if(!est_node_put_doc(node, doc))
    fprintf(stderr, "error: %d\n", est_node_status(node));
  /* destroy the document object */
  est_doc_delete(doc);
  /* destroy the node object */
  est_node_delete(node);
  /* free the networking environment */
  est_free_net_env();
  return 0;
}
