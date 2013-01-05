import estraier.*;

public class Example001 {
  public static void main(String[] args){

    // create the database object
    Database db = new Database();

    // open the database
    if(!db.open("casket", Database.DBWRITER | Database.DBCREAT)){
      System.err.println("error: " + db.err_msg(db.error()));
      return;
    }

    // create a document object
    Document doc = new Document();

    // add attributes to the document object
    doc.add_attr("@uri", "http://estraier.gov/example.txt");
    doc.add_attr("@title", "Over the Rainbow");

    // add the body text to the document object
    doc.add_text("Somewhere over the rainbow.  Way up high.");
    doc.add_text("There's a land that I heard of once in a lullaby.");

    // register the document object to the database
    if(!db.put_doc(doc, Database.PDCLEAN))
      System.err.println("error: " + db.err_msg(db.error()));

    // close the database
    if(!db.close())
      System.err.println("error: " + db.err_msg(db.error()));

  }
}
