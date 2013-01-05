require "estraierpure"
include EstraierPure

# create and configure the node connecton object
node = Node::new
node.set_url("http://localhost:1978/node/test1")

# create a search condition object
cond = Condition::new

# set the search phrase to the search condition object
cond.set_phrase("rainbow AND lullaby")

# get the result of search
nres = node.search(cond, 0);
if nres
  # for each document in the result
  for i in 0...nres.doc_num
    # get a result document object
    rdoc = nres.get_doc(i)
    # display attributes
    value = rdoc.attr("@uri")
    printf("URI: %s\n", value) if value
    value = rdoc.attr("@title")
    printf("Title: %s\n", value) if value
    # display the snippet text */
    printf("%s", rdoc.snippet)
  end
else
  STDERR.printf("error: %d\n", node.status)
end
