require "estraier"
include Estraier

# create the database object
db = Database::new

# open the database
unless db.open("casket", Database::DBREADER)
  printf("error: %s\n", db.err_msg(db.error))
  exit
end

# create a search condition object
cond = Condition::new

# set the search phrase to the search condition object
cond.set_phrase("rainbow AND lullaby")

# get the result of search
result = db.search(cond)

# for each document in the result
dnum = result.doc_num
for i in 0...dnum
  # retrieve the document object
  doc = db.get_doc(result.get_doc_id(i), 0)
  next unless doc
  # display attributes
  uri = doc.attr("@uri")
  printf("URI: %s\n", uri) if uri
  title = doc.attr("@title")
  printf("Title: %s\n", title) if title
  # display the body text
  doc.texts.each do |text|
    printf("%s\n", text)
  end
end

# close the database
unless db.close
  printf("error: %s\n", db.err_msg(db.error))
end
