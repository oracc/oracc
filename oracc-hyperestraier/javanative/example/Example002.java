import estraier.*;
import java.util.*;

public class Example002 {
  public static void main(String[] args){

    // create the database object
    Database db = new Database();

    // open the database
    if(!db.open("casket", Database.DBREADER)){
      System.err.println("error: " + db.err_msg(db.error()));
      return;
    }

    // create a search condition object
    Condition cond = new Condition();

    // set the search phrase to the search condition object
    cond.set_phrase("rainbow AND lullaby");

    // get the result of search
    Result result = db.search(cond);

    // for each document in the result
    for(int i = 0; i < result.doc_num(); i++){
      // retrieve the document object
      Document doc = db.get_doc(result.get_doc_id(i), 0);
      if(doc == null) continue;
      // display attributes
      String uri = doc.attr("@uri");
      if(uri != null) System.out.println("URI: " + uri);
      String title = doc.attr("@title");
      if(title != null) System.out.println("Title: " + title);
      // display the body text
      Iterator it = doc.texts().iterator();
      while(it.hasNext()){
        System.out.println((String)it.next());
      }
    }

    // close the database
    if(!db.close())
      System.err.println("error: " + db.err_msg(db.error()));

  }
}
