require "estraierpure"
include EstraierPure

# create and configure the node connecton object
node = Node::new
node.set_url("http://localhost:1978/node/test1")
node.set_auth("admin", "admin")

# create a document object
doc = Document::new

# add attributes to the document object
doc.add_attr("@uri", "http://estraier.gov/example.txt")
doc.add_attr("@title", "Over the Rainbow")

# add the body text to the document object
doc.add_text("Somewhere over the rainbow.  Way up high.")
doc.add_text("There's a land that I heard of once in a lullaby.")

# register the document object to the node
unless node.put_doc(doc)
  STDERR.printf("error: %d\n", node.status)
end
