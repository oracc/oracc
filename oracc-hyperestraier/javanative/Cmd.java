/*************************************************************************************************
 * Java binding of Hyper Estraier
 *                                                      Copyright (C) 2004-2007 Mikio Hirabayashi
 * This file is part of Hyper Estraier.
 * Hyper Estraier is free software; you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation; either
 * version 2.1 of the License or any later version.  Hyper Estraier is distributed in the hope
 * that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 * You should have received a copy of the GNU Lesser General Public License along with Hyper
 * Estraier; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA.
 *************************************************************************************************/


package estraier;

import java.util.*;
import java.io.*;
import java.net.*;



/**
 * Command line utility for the core API.
 */
public class Cmd {
  //----------------------------------------------------------------
  // class fields
  //----------------------------------------------------------------
  /* class constants */
  private static final String PROTVER = "1.0";
  private static final int SEARCHMAX = 10;
  private static final int SEARCHAUX = 32;
  private static final int SNIPWWIDTH = 480;
  private static final int SNIPHWIDTH = 96;
  private static final int SNIPAWIDTH = 96;
  private static final int VM_ID = 0;
  private static final int VM_URI = 1;
  private static final int VM_ATTR = 2;
  private static final int VM_FULL = 3;
  private static final int VM_SNIP = 4;
  private static final PrintStream stdout = System.out;
  private static final PrintStream stderr = System.err;
  //----------------------------------------------------------------
  // public static methods
  //----------------------------------------------------------------
  /**
   * Main routine of the command.
   * @param args command line arguments.  The first argument specifies the name of the sub
   * command.  "put", "out", "edit", "get", "uriid", "inform", "optimize", "merge", "search" are
   * provided.
   */
  public static void main(String[] args){
    int rv = 0;
    if(args.length < 1) usage();
    if(args[0].equals("put")){
      rv = runput(args);
    } else if(args[0].equals("out")){
      rv = runout(args);
    } else if(args[0].equals("edit")){
      rv = runedit(args);
    } else if(args[0].equals("get")){
      rv = runget(args);
    } else if(args[0].equals("uriid")){
      rv = runuriid(args);
    } else if(args[0].equals("inform")){
      rv = runinform(args);
    } else if(args[0].equals("optimize")){
      rv = runoptimize(args);
    } else if(args[0].equals("merge")){
      rv = runmerge(args);
    } else if(args[0].equals("search")){
      rv = runsearch(args);
    } else {
      usage();
    }
    System.exit(rv);
  }
  //----------------------------------------------------------------
  // private static methods
  //----------------------------------------------------------------
  /* print the usage and exit */
  private static void usage(){
    stderr.println("command line utility for the core API of Hyper Estraier");
    stderr.println("");
    stderr.println("synopsis:");
    stderr.println("  java " + Cmd.class.getName() + " arguments...");
    stderr.println("");
    stderr.println("arguments:");
    stderr.println("  put [-cl] [-ws] db [file]");
    stderr.println("  out [-cl] db expr");
    stderr.println("  edit db expr name [value]");
    stderr.println("  get db expr [attr]");
    stderr.println("  uriid db uri");
    stderr.println("  inform db");
    stderr.println("  optimize [-onp] [-ond] db");
    stderr.println("  merge [-cl] db target");
    stderr.println("  search [-vu|-va|-vf|-vs] [-gs|-gf|-ga] [-cd] [-ni] [-sf|-sfr|-sfu|-sfi]" +
                   " [-attr expr] [-ord expr] [-max num] [-sk num] [-aux num] [-dis name]" +
                   " db [phrase]");
    stderr.println("");
    System.exit(1);
  }
  /* print error string and flush the buffer */
  private static void printerror(String msg){
    stderr.println(Cmd.class.getName() + ": " + msg);
    stderr.flush();
  }
  /* parse arguments of the put command */
  private static int runput(String[] args){
    String dbname = null;
    String file = null;
    int opts = 0;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(dbname == null && arg.startsWith("-")){
        if(arg.equals("-cl")){
          opts |= Database.PDCLEAN;
        } else if(arg.equals("-ws")){
          opts |= Database.PDWEIGHT;
        } else {
          usage();
        }
      } else if(dbname == null){
        dbname = arg;
      } else if(file == null){
        file = arg;
      } else {
        usage();
      }
    }
    if(dbname == null) usage();
    return procput(dbname, file, opts);
  }
  /* parse arguments of the out command */
  private static int runout(String[] args){
    String dbname = null;
    String expr = null;
    int opts = 0;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(dbname == null && arg.startsWith("-")){
        if(arg.equals("-cl")){
          opts |= Database.ODCLEAN;
        } else {
          usage();
        }
      } else if(dbname == null){
        dbname = arg;
      } else if(expr == null){
        expr = arg;
      } else {
        usage();
      }
    }
    if(dbname == null || expr == null) usage();
    return procout(dbname, expr, opts);
  }
  /* parse arguments of the edit command */
  private static int runedit(String[] args){
    String dbname = null;
    String expr = null;
    String name = null;
    String value = null;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(dbname == null && arg.startsWith("-")){
        usage();
      } else if(dbname == null){
        dbname = arg;
      } else if(expr == null){
        expr = arg;
      } else if(name == null){
        name = arg;
      } else if(value == null){
        value = arg;
      } else {
        usage();
      }
    }
    if(dbname == null || expr == null || name == null) usage();
    return procedit(dbname, expr, name, value);
  }
  /* parse arguments of the get command */
  private static int runget(String[] args){
    String dbname = null;
    String expr = null;
    String attr = null;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(dbname == null && arg.startsWith("-")){
        usage();
      } else if(dbname == null){
        dbname = arg;
      } else if(expr == null){
        expr = arg;
      } else if(attr == null){
        attr = arg;
      } else {
        usage();
      }
    }
    if(dbname == null || expr == null) usage();
    return procget(dbname, expr, attr);
  }
  /* parse arguments of the uriid command */
  private static int runuriid(String[] args){
    String dbname = null;
    String uri = null;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(dbname == null && arg.startsWith("-")){
        usage();
      } else if(dbname == null){
        dbname = arg;
      } else if(uri == uri){
        uri = arg;
      } else {
        usage();
      }
    }
    if(dbname == null || uri == null) usage();
    return procuriid(dbname, uri);
  }
  /* parse arguments of the inform command */
  private static int runinform(String[] args){
    String dbname = null;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(dbname == null && arg.startsWith("-")){
        usage();
      } else if(dbname == null){
        dbname = arg;
      } else {
        usage();
      }
    }
    if(dbname == null) usage();
    return procinform(dbname);
  }
  /* parse arguments of the optimize command */
  private static int runoptimize(String[] args){
    String dbname = null;
    int opts = 0;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(dbname == null && arg.startsWith("-")){
        if(arg.equals("-onp")){
          opts |= Database.OPTNOPURGE;
        } else if(arg.equals("-ond")){
          opts |= Database.OPTNODBOPT;
        } else {
          usage();
        }
      } else if(dbname == null){
        dbname = arg;
      } else {
        usage();
      }
    }
    if(dbname == null) usage();
    return procoptimize(dbname, opts);
  }
  /* parse arguments of the merge command */
  private static int runmerge(String[] args){
    String dbname = null;
    String tgname = null;
    int opts = 0;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(dbname == null && arg.startsWith("-")){
        if(arg.equals("-cl")){
          opts |= Database.MGCLEAN;
        } else {
          usage();
        }
      } else if(dbname == null){
        dbname = arg;
      } else if(tgname == null){
        tgname = arg;
      } else {
        usage();
      }
    }
    if(dbname == null || tgname == null) usage();
    return procmerge(dbname, tgname, opts);
  }
  /* parse arguments of the search command */
  private static int runsearch(String[] args){
    String dbname = null;
    String phrase = null;
    List attrs = new ArrayList(3);
    String ord = null;
    int max = SEARCHMAX;
    int skip = 0;
    int opts = 0;
    int aux = SEARCHAUX;
    String dis = null;
    boolean cd = false;
    int view = VM_ID;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(dbname == null && arg.startsWith("-")){
        if(arg.equals("-vu")){
          view = VM_URI;
        } else if(arg.equals("-va")){
          view = VM_ATTR;
        } else if(arg.equals("-vf")){
          view = VM_FULL;
        } else if(arg.equals("-vs")){
          view = VM_SNIP;
        } else if(arg.equals("-gs")){
          opts |= Condition.SURE;
        } else if(arg.equals("-gf")){
          opts |= Condition.FAST;
        } else if(arg.equals("-ga")){
          opts |= Condition.AGITO;
        } else if(arg.equals("-cd")){
          cd = true;
        } else if(arg.equals("-ni")){
          opts |= Condition.NOIDF;
        } else if(arg.equals("-sf")){
          opts |= Condition.SIMPLE;
        } else if(arg.equals("-sfr")){
          opts |= Condition.ROUGH;
        } else if(arg.equals("-sfu")){
          opts |= Condition.UNION;
        } else if(arg.equals("-sfi")){
          opts |= Condition.ISECT;
        } else if(arg.equals("-attr")){
          if(++i >= args.length) usage();
          attrs.add(args[i]);
        } else if(arg.equals("-ord")){
          if(++i >= args.length) usage();
          ord = args[i];
        } else if(arg.equals("-max")){
          if(++i >= args.length) usage();
          max = Integer.parseInt(args[i]);
        } else if(arg.equals("-sk")){
          if(++i >= args.length) usage();
          skip = Integer.parseInt(args[i]);
        } else if(arg.equals("-aux")){
          if(++i >= args.length) usage();
          aux = Integer.parseInt(args[i]);
        } else if(arg.equals("-dis")){
          if(++i >= args.length) usage();
          dis = args[i];
        } else {
          usage();
        }
      } else if(dbname == null){
        dbname = arg;
      } else if(phrase == null){
        phrase = arg;
      } else {
        phrase = phrase + " " + arg;
      }
    }
    if(dbname == null) usage();
    return procsearch(dbname, phrase, attrs, ord, max, skip, opts, aux, dis, cd, view);
  }
  /* perform the put command */
  private static int procput(String dbname, String file, int opts){
    byte[] data = null;
    if(file != null){
      FileInputStream fis = null;
      try {
        fis = new FileInputStream(file);
        data = Utility.read_all(fis);
      } catch(IOException e){
        printerror(e.toString());
        return 1;
      } finally {
        try {
          if(fis != null) fis.close();
        } catch(IOException e){}
      }
    } else {
      try {
        data = Utility.read_all(System.in);
      } catch(IOException e){
        printerror(e.toString());
        return 1;
      }
    }
    String draft = null;
    try {
      draft = new String(data, "UTF-8");
    } catch(UnsupportedEncodingException e){
      printerror(e.toString());
      return 1;
    }
    Document doc = new Document(draft);
    Database db = new Database();
    if(!db.open(dbname, Database.DBWRITER | Database.DBCREAT)){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    db.set_informer(new DatabaseInformerImpl());
    if(!db.put_doc(doc, opts)){
      printerror(dbname + ": " + db.err_msg(db.error()));
      db.close();
      return 1;
    }
    if(!db.close()){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    return 0;
  }
  /* perform the out command */
  private static int procout(String dbname, String expr, int opts){
    Database db = new Database();
    if(!db.open(dbname, Database.DBWRITER)){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    db.set_informer(new DatabaseInformerImpl());
    int id = -1;
    try {
      id = Integer.parseInt(expr);
    } catch(NumberFormatException e){
      id = -1;
    }
    if(id < 1 && (id = db.uri_to_id(expr)) < 1){
      printerror(dbname + ": " + db.err_msg(db.error()));
      db.close();
      return 1;
    }
    if(!db.out_doc(id, opts)){
      printerror(dbname + ": " + db.err_msg(db.error()));
      db.close();
      return 1;
    }
    if(!db.close()){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    return 0;
  }
  /* perform the edit command */
  private static int procedit(String dbname, String expr, String name, String value){
    Database db = new Database();
    if(!db.open(dbname, Database.DBWRITER)){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    db.set_informer(new DatabaseInformerImpl());
    int id = -1;
    try {
      id = Integer.parseInt(expr);
    } catch(NumberFormatException e){
      id = -1;
    }
    if(id < 1 && (id = db.uri_to_id(expr)) < 1){
      printerror(dbname + ": " + db.err_msg(db.error()));
      db.close();
      return 1;
    }
    Document doc = db.get_doc(id, Database.GDNOTEXT);
    if(doc == null){
      printerror(dbname + ": " + db.err_msg(db.error()));
      db.close();
      return 1;
    }
    doc.add_attr(name, value);
    if(!db.edit_doc(doc)){
      printerror(dbname + ": " + db.err_msg(db.error()));
      db.close();
      return 1;
    }
    if(!db.close()){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    return 0;
  }
  /* perform the get command */
  private static int procget(String dbname, String expr, String attr){
    Database db = new Database();
    if(!db.open(dbname, Database.DBREADER)){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    int id = -1;
    try {
      id = Integer.parseInt(expr);
    } catch(NumberFormatException e){
      id = -1;
    }
    if(id < 1 && (id = db.uri_to_id(expr)) < 1){
      printerror(dbname + ": " + db.err_msg(db.error()));
      db.close();
      return 1;
    }
    if(attr != null){
      String value = db.get_doc_attr(id, attr);
      if(value == null){
        printerror(dbname + ": " + db.err_msg(db.error()));
        db.close();
        return 1;
      }
      stdout.println(value);
    } else {
      Document doc = db.get_doc(id, 0);
      if(doc == null){
        printerror(dbname + ": " + db.err_msg(db.error()));
        db.close();
        return 1;
      }
      stdout.print(doc.dump_draft());
    }
    if(!db.close()){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    return 0;
  }
  /* perform the uriid command */
  private static int procuriid(String dbname, String uri){
    Database db = new Database();
    if(!db.open(dbname, Database.DBREADER)){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    int id = db.uri_to_id(uri);
    if(id < 1){
      printerror(dbname + ": " + db.err_msg(db.error()));
      db.close();
      return 1;
    }
    stdout.println(id);
    if(!db.close()){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    return 0;
  }
  /* perform the inform command */
  private static int procinform(String dbname){
    Database db = new Database();
    if(!db.open(dbname, Database.DBREADER)){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    stdout.println("number of documents: " + db.doc_num());
    stdout.println("number of words: " + db.word_num());
    stdout.println("file size: " +  (long)db.size());
    if(!db.close()){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    return 0;
  }
  /* perform the optimize command */
  private static int procoptimize(String dbname, int opts){
    Database db = new Database();
    if(!db.open(dbname, Database.DBWRITER)){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    db.set_informer(new DatabaseInformerImpl());
    if(!db.optimize(opts)){
      printerror(dbname + ": " + db.err_msg(db.error()));
      db.close();
      return 1;
    }
    if(!db.close()){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    return 0;
  }
  /* perform the merge command */
  private static int procmerge(String dbname, String tgname, int opts){
    Database db = new Database();
    if(!db.open(dbname, Database.DBWRITER)){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    db.set_informer(new DatabaseInformerImpl());
    if(!db.merge(tgname, opts)){
      printerror(dbname + ": " + db.err_msg(db.error()));
      db.close();
      return 1;
    }
    if(!db.close()){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    return 0;
  }
  /* perform the search command */
  private static int procsearch(String dbname, String phrase, List attrs, String ord,
                                int max, int skip, int opts, int aux, String dis,
                                boolean cd, int view){
    Database db = new Database();
    if(!db.open(dbname, Database.DBREADER)){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    Condition cond = new Condition();
    if(phrase != null) cond.set_phrase(phrase);
    Iterator attrsit = attrs.iterator();
    while(attrsit.hasNext()){
      cond.add_attr((String)attrsit.next());
    }
    if(ord != null) cond.set_order(ord);
    if(max >= 0) cond.set_max(max);
    if(skip >= 0) cond.set_skip(skip);
    cond.set_options(opts);
    cond.set_auxiliary(aux);
    if(dis != null) cond.set_distinct(dis);
    long stime = new GregorianCalendar().getTimeInMillis();
    Result res = db.search(cond);
    long etime = new GregorianCalendar().getTimeInMillis();
    String border = "--------[" + stime + "]--------";
    stdout.println(border);
    stdout.println("VERSION\t" + PROTVER);
    stdout.println("NODE\tlocal");
    stdout.println("HIT\t" + res.hint(""));
    List snwords = new ArrayList();
    Iterator wordsit = res.hint_words().iterator();
    for(int i = 1; wordsit.hasNext(); i++){
      String word = (String)wordsit.next();
      int hits = res.hint(word);
      if(hits > 0) snwords.add(word);
      stdout.println("HINT#" + i + "\t" + word + "\t" + hits);
    }
    stdout.println("TIME\t" + ((etime - stime) / 1000.0));
    stdout.println("DOCNUM\t" + db.doc_num());
    stdout.println("WORDNUM\t" + db.word_num());
    if(view == VM_URI){
      stdout.println("VIEW\tURI");
    } else if(view == VM_ATTR){
      stdout.println("VIEW\tATTRIBUTE");
    } else if(view == VM_FULL){
      stdout.println("VIEW\tFULL");
    } else if(view == VM_SNIP){
      stdout.println("VIEW\tSNIPPET");
    } else {
      stdout.println("VIEW\tID");
    }
    stdout.println("");
    if(view != VM_ATTR && view != VM_FULL && view != VM_SNIP) stdout.println(border);
    int dnum = res.doc_num();
    for(int i = 0; i < dnum; i++){
      int id = res.get_doc_id(i);
      if(view == VM_URI){
        Document doc = db.get_doc(id, cd ? 0 : Database.GDNOTEXT);
        if(doc == null) continue;
        if(cd && !db.scan_doc(doc, cond)) continue;
        stdout.println(id + "\t" + doc.attr("@uri"));
      } else if(view == VM_ATTR){
        Document doc = db.get_doc(id, cd ? 0 : Database.GDNOTEXT);
        if(doc == null) continue;
        if(cd && !db.scan_doc(doc, cond)) continue;
        stdout.println(border);
        Iterator it = doc.attr_names().iterator();
        while(it.hasNext()){
          String name = (String)it.next();
          stdout.println(name + "=" + doc.attr(name));
        }
        stdout.println("");
      } else if(view == VM_FULL){
        Document doc = db.get_doc(id, 0);
        if(doc == null) continue;
        if(cd && !db.scan_doc(doc, cond)) continue;
        stdout.println(border);
        stdout.print(doc.dump_draft());
      } else if(view == VM_SNIP){
        Document doc = db.get_doc(id, 0);
        if(doc == null) continue;
        if(cd && !db.scan_doc(doc, cond)) continue;
        stdout.println(border);
        Iterator it = doc.attr_names().iterator();
        while(it.hasNext()){
          String name = (String)it.next();
          stdout.println(name + "=" + doc.attr(name));
        }
        stdout.println("");
        stdout.print(doc.make_snippet(snwords, SNIPWWIDTH, SNIPHWIDTH, SNIPAWIDTH));
      } else {
        stdout.println(id);
      }
    }
    stdout.println(border + ":END");
    if(!db.close()){
      printerror(dbname + ": " + db.err_msg(db.error()));
      return 1;
    }
    return 0;
  }
  /* dummy method: hide the constructor */
  private Cmd(){}
}



/**
 * Command line utility for the core API.
 */
class DatabaseInformerImpl implements DatabaseInformer {
  /* inform of a database event */
  public void inform(String message){
    System.out.println(Cmd.class.getName() + ": INFO: " + message);
  }
}



/* END OF FILE */
