import estraier.pure.*;

public class Example002 {
  public static void main(String[] args){

    // create the node connecton object
    Node node = new Node();
    node.set_url("http://localhost:1978/node/test1");

    // create a search condition object
    Condition cond = new Condition();

    // set the search phrase to the search condition object
    cond.set_phrase("rainbow AND lullaby");

    // get the result of search
    NodeResult nres = node.search(cond, 0);
    if(nres != null){
      // for each document in the result
      for(int i = 0; i < nres.doc_num(); i++){
        // get a result document object
        ResultDocument rdoc = nres.get_doc(i);
        // display attributes
        System.out.println("URI: " + rdoc.attr("@uri"));
        System.out.println("Title: " + rdoc.attr("@title"));
        // display the snippet text
        System.out.print(rdoc.snippet());
      }
    } else {
      System.err.println("error: " + node.status());
    }

  }
}
