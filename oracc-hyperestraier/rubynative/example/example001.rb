require "estraier"
include Estraier

# create the database object
db = Database::new

# open the database
unless db.open("casket", Database::DBWRITER | Database::DBCREAT)
  printf("error: %s\n", db.err_msg(db.error))
  exit
end

# create a document object
doc = Document::new

# add attributes to the document object
doc.add_attr("@uri", "http://estraier.gov/example.txt")
doc.add_attr("@title", "Over the Rainbow")

# add the body text to the document object
doc.add_text("Somewhere over the rainbow.  Way up high.")
doc.add_text("There's a land that I heard of once in a lullaby.")

# register the document object to the database
unless db.put_doc(doc, Database::PDCLEAN)
  printf("error: %s\n", db.err_msg(db.error))
end

# close the database
unless db.close
  printf("error: %s\n", db.err_msg(db.error))
end
