/*************************************************************************************************
 * Pure Java interface of Hyper Estraier
 *                                                      Copyright (C) 2004-2007 Mikio Hirabayashi
 *                                                                           All rights reserved.
 * This file is part of Hyper Estraier.
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice, this list of
 *     conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright notice, this list of
 *     conditions and the following disclaimer in the documentation and/or other materials
 *     provided with the distribution.
 *   * Neither the name of Mikio Hirabayashi nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************************************/


package estraier.pure;

import java.util.*;
import java.io.*;
import java.net.*;



/**
 * Command line utility for the node API.
 */
public class Call {
  //----------------------------------------------------------------
  // class fields
  //----------------------------------------------------------------
  /* class constants */
  private static final int SEARCHMAX = 10;
  private static final int SEARCHAUX = 32;
  private static final PrintStream stdout = System.out;
  private static final PrintStream stderr = System.err;
  /* class variables */
  private static String pxhost = null;
  private static int pxport = 0;
  private static int timeout = -1;
  private static String authname = null;
  private static String authpass = null;
  //----------------------------------------------------------------
  // public static methods
  //----------------------------------------------------------------
  /**
   * Main routine of the command.
   * @param args command line arguments.  The first argument specifies the name of the sub
   * command.  "put", "out", "edit", "get", "etch", "uriid", "inform", "sync", "optimize",
   "search", "setuser", and "setlink" are provided.
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
    } else if(args[0].equals("etch")){
      rv = runetch(args);
    } else if(args[0].equals("uriid")){
      rv = runuriid(args);
    } else if(args[0].equals("inform")){
      rv = runinform(args);
    } else if(args[0].equals("sync")){
      rv = runsync(args);
    } else if(args[0].equals("optimize")){
      rv = runoptimize(args);
    } else if(args[0].equals("search")){
      rv = runsearch(args);
    } else if(args[0].equals("setuser")){
      rv = runsetuser(args);
    } else if(args[0].equals("setlink")){
      rv = runsetlink(args);
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
    stderr.println("command line utility for the node API of Hyper Estraier");
    stderr.println("");
    stderr.println("synopsis:");
    stderr.println("  java " + Call.class.getName() + " arguments...");
    stderr.println("");
    stderr.println("arguments:");
    stderr.println("  put [-proxy host:port] [-tout num] [-auth user pass]" +
                   " nurl [file]");
    stderr.println("  out [-proxy host:port] [-tout num] [-auth user pass]" +
                   " nurl expr");
    stderr.println("  edit [-proxy host:port] [-tout num] [-auth user pass]" +
                   " nurl expr name [value]");
    stderr.println("  get [-proxy host:port] [-tout num] [-auth user pass]" +
                   " nurl expr [attr]");
    stderr.println("  etch [-proxy host:port] [-tout num] [-auth user pass]" +
                   " nurl expr");
    stderr.println("  uriid [-proxy host:port] [-tout num] [-auth user pass]" +
                   " nurl uri");
    stderr.println("  inform [-proxy host:port] [-tout num] [-auth user pass]" +
                   " nurl");
    stderr.println("  sync [-proxy host:port] [-tout num] [-auth user pass]" +
                   " nurl");
    stderr.println("  optimize [-proxy host:port] [-tout num] [-auth user pass]" +
                   " nurl");
    stderr.println("  search [-proxy host:port] [-tout num] [-auth user pass]" +
                   " [-sf|-sfr|-sfu|-sfi] [-attr expr] [-ord expr] [-max num] [-sk num]" +
                   " [-aux num] [-dis name] [-dpt num] [-mask num] nurl [phrase]");
    stderr.println("  setuser [-proxy host:port] [-tout num] [-auth user pass]" +
                   " nurl name mode");
    stderr.println("  setlink [-proxy host:port] [-tout num] [-auth user pass]" +
                   " nurl url label credit");
    stderr.println("");
    System.exit(1);
  }
  /* print error string and flush the buffer */
  private static void printerror(String msg){
    stderr.println(Call.class.getName() + ": " + msg);
    stderr.flush();
  }
  /* parse arguments of the put command */
  private static int runput(String[] args){
    String nurl = null;
    String file = null;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(nurl == null && arg.startsWith("-")){
        if(arg.equals("-proxy")){
          if(++i >= args.length) usage();
          pxhost = args[i];
          if(++i >= args.length) usage();
          pxport = Integer.parseInt(args[i]);
        } else if(arg.equals("-tout")){
          if(++i >= args.length) usage();
          timeout = Integer.parseInt(args[i]);
        } else if(arg.equals("-auth")){
          if(++i >= args.length) usage();
          authname = args[i];
          if(++i >= args.length) usage();
          authpass = args[i];
        } else {
          usage();
        }
      } else if(nurl == null){
        nurl = arg;
      } else if(file == null){
        file = arg;
      } else {
        usage();
      }
    }
    if(nurl == null) usage();
    return procput(nurl, file);
  }
  /* parse arguments of the out command */
  private static int runout(String[] args){
    String nurl = null;
    String expr = null;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(nurl == null && arg.startsWith("-")){
        if(arg.equals("-proxy")){
          if(++i >= args.length) usage();
          pxhost = args[i];
          if(++i >= args.length) usage();
          pxport = Integer.parseInt(args[i]);
        } else if(arg.equals("-tout")){
          if(++i >= args.length) usage();
          timeout = Integer.parseInt(args[i]);
        } else if(arg.equals("-auth")){
          if(++i >= args.length) usage();
          authname = args[i];
          if(++i >= args.length) usage();
          authpass = args[i];
        } else {
          usage();
        }
      } else if(nurl == null){
        nurl = arg;
      } else if(expr == null){
        expr = arg;
      } else {
        usage();
      }
    }
    if(nurl == null || expr == null) usage();
    return procout(nurl, expr);
  }
  /* parse arguments of the edit command */
  private static int runedit(String[] args){
    String nurl = null;
    String expr = null;
    String name = null;
    String value = null;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(nurl == null && arg.startsWith("-")){
        if(arg.equals("-proxy")){
          if(++i >= args.length) usage();
          pxhost = args[i];
          if(++i >= args.length) usage();
          pxport = Integer.parseInt(args[i]);
        } else if(arg.equals("-tout")){
          if(++i >= args.length) usage();
          timeout = Integer.parseInt(args[i]);
        } else if(arg.equals("-auth")){
          if(++i >= args.length) usage();
          authname = args[i];
          if(++i >= args.length) usage();
          authpass = args[i];
        } else {
          usage();
        }
      } else if(nurl == null){
        nurl = arg;
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
    if(nurl == null || expr == null || name == null) usage();
    return procedit(nurl, expr, name, value);
  }
  /* parse arguments of the get command */
  private static int runget(String[] args){
    String nurl = null;
    String expr = null;
    String attr = null;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(nurl == null && arg.startsWith("-")){
        if(arg.equals("-proxy")){
          if(++i >= args.length) usage();
          pxhost = args[i];
          if(++i >= args.length) usage();
          pxport = Integer.parseInt(args[i]);
        } else if(arg.equals("-tout")){
          if(++i >= args.length) usage();
          timeout = Integer.parseInt(args[i]);
        } else if(arg.equals("-auth")){
          if(++i >= args.length) usage();
          authname = args[i];
          if(++i >= args.length) usage();
          authpass = args[i];
        } else {
          usage();
        }
      } else if(nurl == null){
        nurl = arg;
      } else if(expr == null){
        expr = arg;
      } else if(attr == null){
        attr = arg;
      } else {
        usage();
      }
    }
    if(nurl == null || expr == null) usage();
    return procget(nurl, expr, attr);
  }
  /* parse arguments of the etch command */
  private static int runetch(String[] args){
    String nurl = null;
    String expr = null;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(nurl == null && arg.startsWith("-")){
        if(arg.equals("-proxy")){
          if(++i >= args.length) usage();
          pxhost = args[i];
          if(++i >= args.length) usage();
          pxport = Integer.parseInt(args[i]);
        } else if(arg.equals("-tout")){
          if(++i >= args.length) usage();
          timeout = Integer.parseInt(args[i]);
        } else if(arg.equals("-auth")){
          if(++i >= args.length) usage();
          authname = args[i];
          if(++i >= args.length) usage();
          authpass = args[i];
        } else {
          usage();
        }
      } else if(nurl == null){
        nurl = arg;
      } else if(expr == null){
        expr = arg;
      } else {
        usage();
      }
    }
    if(nurl == null || expr == null) usage();
    return procetch(nurl, expr);
  }
  /* parse arguments of the uriid command */
  private static int runuriid(String[] args){
    String nurl = null;
    String uri = null;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(nurl == null && arg.startsWith("-")){
        if(arg.equals("-proxy")){
          if(++i >= args.length) usage();
          pxhost = args[i];
          if(++i >= args.length) usage();
          pxport = Integer.parseInt(args[i]);
        } else if(arg.equals("-tout")){
          if(++i >= args.length) usage();
          timeout = Integer.parseInt(args[i]);
        } else if(arg.equals("-auth")){
          if(++i >= args.length) usage();
          authname = args[i];
          if(++i >= args.length) usage();
          authpass = args[i];
        } else {
          usage();
        }
      } else if(nurl == null){
        nurl = arg;
      } else if(uri == null){
        uri = arg;
      } else {
        usage();
      }
    }
    if(nurl == null || uri == null) usage();
    return procuriid(nurl, uri);
  }
  /* parse arguments of the inform command */
  private static int runinform(String[] args){
    String nurl = null;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(nurl == null && arg.startsWith("-")){
        if(arg.equals("-proxy")){
          if(++i >= args.length) usage();
          pxhost = args[i];
          if(++i >= args.length) usage();
          pxport = Integer.parseInt(args[i]);
        } else if(arg.equals("-tout")){
          if(++i >= args.length) usage();
          timeout = Integer.parseInt(args[i]);
        } else if(arg.equals("-auth")){
          if(++i >= args.length) usage();
          authname = args[i];
          if(++i >= args.length) usage();
          authpass = args[i];
        } else {
          usage();
        }
      } else if(nurl == null){
        nurl = arg;
      } else {
        usage();
      }
    }
    if(nurl == null) usage();
    return procinform(nurl);
  }
  /* parse arguments of the sync command */
  private static int runsync(String[] args){
    String nurl = null;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(nurl == null && arg.startsWith("-")){
        if(arg.equals("-proxy")){
          if(++i >= args.length) usage();
          pxhost = args[i];
          if(++i >= args.length) usage();
          pxport = Integer.parseInt(args[i]);
        } else if(arg.equals("-tout")){
          if(++i >= args.length) usage();
          timeout = Integer.parseInt(args[i]);
        } else if(arg.equals("-auth")){
          if(++i >= args.length) usage();
          authname = args[i];
          if(++i >= args.length) usage();
          authpass = args[i];
        } else {
          usage();
        }
      } else if(nurl == null){
        nurl = arg;
      } else {
        usage();
      }
    }
    if(nurl == null) usage();
    return procsync(nurl);
  }
  /* parse arguments of the optimize command */
  private static int runoptimize(String[] args){
    String nurl = null;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(nurl == null && arg.startsWith("-")){
        if(arg.equals("-proxy")){
          if(++i >= args.length) usage();
          pxhost = args[i];
          if(++i >= args.length) usage();
          pxport = Integer.parseInt(args[i]);
        } else if(arg.equals("-tout")){
          if(++i >= args.length) usage();
          timeout = Integer.parseInt(args[i]);
        } else if(arg.equals("-auth")){
          if(++i >= args.length) usage();
          authname = args[i];
          if(++i >= args.length) usage();
          authpass = args[i];
        } else {
          usage();
        }
      } else if(nurl == null){
        nurl = arg;
      } else {
        usage();
      }
    }
    if(nurl == null) usage();
    return procoptimize(nurl);
  }
  /* parse arguments of the search command */
  private static int runsearch(String[] args){
    String nurl = null;
    String phrase = null;
    List attrs = new ArrayList(3);
    String ord = null;
    int max = SEARCHMAX;
    int skip = 0;
    int opts = 0;
    int aux = SEARCHAUX;
    String dis = null;
    int depth = 0;
    int mask = 0;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(nurl == null && arg.startsWith("-")){
        if(arg.equals("-proxy")){
          if(++i >= args.length) usage();
          pxhost = args[i];
          if(++i >= args.length) usage();
          pxport = Integer.parseInt(args[i]);
        } else if(arg.equals("-tout")){
          if(++i >= args.length) usage();
          timeout = Integer.parseInt(args[i]);
        } else if(arg.equals("-auth")){
          if(++i >= args.length) usage();
          authname = args[i];
          if(++i >= args.length) usage();
          authpass = args[i];
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
        } else if(arg.equals("-dpt")){
          if(++i >= args.length) usage();
          depth = Integer.parseInt(args[i]);
        } else if(arg.equals("-mask")){
          if(++i >= args.length) usage();
          mask = Integer.parseInt(args[i]);
        } else {
          usage();
        }
      } else if(nurl == null){
        nurl = arg;
      } else if(phrase == null){
        phrase = arg;
      } else {
        phrase = phrase + " " + arg;
      }
    }
    if(nurl == null) usage();
    return procsearch(nurl, phrase, attrs, ord, max, skip, opts, aux, dis, depth, mask);
  }
  /* parse arguments of the setuser command */
  private static int runsetuser(String[] args){
    String nurl = null;
    String name = null;
    String mstr = null;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(nurl == null && arg.startsWith("-")){
        if(arg.equals("-proxy")){
          if(++i >= args.length) usage();
          pxhost = args[i];
          if(++i >= args.length) usage();
          pxport = Integer.parseInt(args[i]);
        } else if(arg.equals("-tout")){
          if(++i >= args.length) usage();
          timeout = Integer.parseInt(args[i]);
        } else if(arg.equals("-auth")){
          if(++i >= args.length) usage();
          authname = args[i];
          if(++i >= args.length) usage();
          authpass = args[i];
        } else {
          usage();
        }
      } else if(nurl == null){
        nurl = arg;
      } else if(name == null){
        name = arg;
      } else if(mstr == null){
        mstr = arg;
      } else {
        usage();
      }
    }
    if(nurl == null || name == null || mstr == null) usage();
    int mode = Integer.parseInt(mstr);
    if(mode < 0) usage();
    return procsetuser(nurl, name, mode);
  }
  /* parse arguments of the setlink command */
  private static int runsetlink(String[] args){
    String nurl = null;
    String url = null;
    String label = null;
    String cstr = null;
    for(int i = 1; i < args.length; i++){
      String arg = args[i];
      if(nurl == null && arg.startsWith("-")){
        if(arg.equals("-proxy")){
          if(++i >= args.length) usage();
          pxhost = args[i];
          if(++i >= args.length) usage();
          pxport = Integer.parseInt(args[i]);
        } else if(arg.equals("-tout")){
          if(++i >= args.length) usage();
          timeout = Integer.parseInt(args[i]);
        } else if(arg.equals("-auth")){
          if(++i >= args.length) usage();
          authname = args[i];
          if(++i >= args.length) usage();
          authpass = args[i];
        } else {
          usage();
        }
      } else if(nurl == null){
        nurl = arg;
      } else if(url == null){
        url = arg;
      } else if(label == null){
        label = arg;
      } else if(cstr == null){
        cstr = arg;
      } else {
        usage();
      }
    }
    if(nurl == null || url == null || label == null || cstr == null) usage();
    int credit = Integer.parseInt(cstr);
    return procsetlink(nurl, url, label, credit);
  }
  /* perform the put command */
  private static int procput(String nurl, String file){
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
    Node node = new Node();
    node.set_url(nurl);
    if(pxhost != null) node.set_proxy(pxhost, pxport);
    if(timeout > 0) node.set_timeout(timeout);
    if(authname != null) node.set_auth(authname, authpass);
    if(!node.put_doc(doc)){
      printerror("failed: " + node.status());
      return 1;
    }
    return 0;
  }
  /* perform the out command */
  private static int procout(String nurl, String expr){
    int id = -1;
    try {
      id = Integer.parseInt(expr);
    } catch(NumberFormatException e){
      id = -1;
    }
    Node node = new Node();
    node.set_url(nurl);
    if(pxhost != null) node.set_proxy(pxhost, pxport);
    if(timeout > 0) node.set_timeout(timeout);
    if(authname != null) node.set_auth(authname, authpass);
    if(id > 0){
      if(!node.out_doc(id)){
        printerror("failed: " + node.status());
        return 1;
      }
    } else {
      if(!node.out_doc_by_uri(expr)){
        printerror("failed: " + node.status());
        return 1;
      }
    }
    return 0;
  }
  /* perform the edit command */
  private static int procedit(String nurl, String expr, String name, String value){
    int id = -1;
    try {
      id = Integer.parseInt(expr);
    } catch(NumberFormatException e){
      id = -1;
    }
    Node node = new Node();
    node.set_url(nurl);
    if(pxhost != null) node.set_proxy(pxhost, pxport);
    if(timeout > 0) node.set_timeout(timeout);
    if(authname != null) node.set_auth(authname, authpass);
    Document doc = id > 0 ? node.get_doc(id) : node.get_doc_by_uri(expr);
    if(doc == null){
      printerror("failed: " + node.status());
      return 1;
    }
    doc.add_attr(name, value);
    if(!node.edit_doc(doc)){
      printerror("failed: " + node.status());
      return 1;
    }
    return 0;
  }
  /* perform the get command */
  private static int procget(String nurl, String expr, String attr){
    int id = -1;
    try {
      id = Integer.parseInt(expr);
    } catch(NumberFormatException e){
      id = -1;
    }
    Node node = new Node();
    node.set_url(nurl);
    if(pxhost != null) node.set_proxy(pxhost, pxport);
    if(timeout > 0) node.set_timeout(timeout);
    if(authname != null) node.set_auth(authname, authpass);
    if(attr != null){
      String value = id > 0 ? node.get_doc_attr(id, attr) : node.get_doc_attr_by_uri(expr, attr);
      if(value == null){
        printerror("failed: " + node.status());
        return 1;
      }
      stdout.println(value);
    } else {
      Document doc = id > 0 ? node.get_doc(id) : node.get_doc_by_uri(expr);
      if(doc == null){
        printerror("failed: " + node.status());
        return 1;
      }
      stdout.print(doc.dump_draft());
    }
    return 0;
  }
  /* perform the etch command */
  private static int procetch(String nurl, String expr){
    int id = -1;
    try {
      id = Integer.parseInt(expr);
    } catch(NumberFormatException e){
      id = -1;
    }
    Node node = new Node();
    node.set_url(nurl);
    if(pxhost != null) node.set_proxy(pxhost, pxport);
    if(timeout > 0) node.set_timeout(timeout);
    if(authname != null) node.set_auth(authname, authpass);
    Map kwords = id > 0 ? node.etch_doc(id) : node.etch_doc_by_uri(expr);
    if(kwords == null){
      printerror("failed: " + node.status());
      return 1;
    }
    Iterator it = kwords.keySet().iterator();
    while(it.hasNext()){
      String key = (String)it.next();
      stdout.println(key + "\t" + (String)kwords.get(key));
    }
    return 0;
  }
  /* perform the uriid command */
  private static int procuriid(String nurl, String uri){
    Node node = new Node();
    node.set_url(nurl);
    if(pxhost != null) node.set_proxy(pxhost, pxport);
    if(timeout > 0) node.set_timeout(timeout);
    if(authname != null) node.set_auth(authname, authpass);
    int id = node.uri_to_id(uri);
    if(id == -1){
      printerror("failed: " + node.status());
      return 1;
    }
    stdout.println(id);
    return 0;
  }
  /* perform the inform command */
  private static int procinform(String nurl){
    Node node = new Node();
    node.set_url(nurl);
    if(pxhost != null) node.set_proxy(pxhost, pxport);
    if(timeout > 0) node.set_timeout(timeout);
    if(authname != null) node.set_auth(authname, authpass);
    String name = node.name();
    String label = node.name();
    int dnum = node.doc_num();
    int wnum = node.word_num();
    double size = node.size();
    double ratio = node.cache_usage();
    if(name == null || label == null || dnum < 0 || wnum < 0 || size < 0.0 || ratio < 0.0){
      printerror("failed: " + node.status());
      return 1;
    }
    stdout.println(name + "\t" + label + "\t" + dnum + "\t" + wnum + "\t" + (long)size +
                   "\t" + ratio);
    return 0;
  }
  /* perform the sync command */
  private static int procsync(String nurl){
    Node node = new Node();
    node.set_url(nurl);
    if(pxhost != null) node.set_proxy(pxhost, pxport);
    if(timeout > 0) node.set_timeout(timeout);
    if(authname != null) node.set_auth(authname, authpass);
    if(!node.sync()){
      printerror("failed: " + node.status());
      return 1;
    }
    return 0;
  }
  /* perform the optimize command */
  private static int procoptimize(String nurl){
    Node node = new Node();
    node.set_url(nurl);
    if(pxhost != null) node.set_proxy(pxhost, pxport);
    if(timeout > 0) node.set_timeout(timeout);
    if(authname != null) node.set_auth(authname, authpass);
    if(!node.optimize()){
      printerror("failed: " + node.status());
      return 1;
    }
    return 0;
  }
  /* perform the search command */
  private static int procsearch(String nurl, String phrase, List attrs, String ord,
                                int max, int skip, int opts, int aux, String dis,
                                int depth, int mask){
    Node node = new Node();
    node.set_url(nurl);
    if(pxhost != null) node.set_proxy(pxhost, pxport);
    if(timeout > 0) node.set_timeout(timeout);
    if(authname != null) node.set_auth(authname, authpass);
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
    cond.set_mask(mask);
    NodeResult nres = node.search(cond, depth);
    Calendar cal = new GregorianCalendar();
    String border = "--------[" + cal.getTimeInMillis() + "]--------";
    if(nres != null){
      stdout.println(border);
      String value;
      if((value = nres.hint("VERSION")) != null) stdout.println("VERSION\t" + value);
      if((value = nres.hint("NODE")) != null) stdout.println("NODE\t" + value);
      if((value = nres.hint("HIT")) != null) stdout.println("HIT\t" + value);
      for(int i = 1;; i++){
        String key = "HINT#" + i;
        if((value = nres.hint(key)) != null){
          stdout.println(key + "\t" + value);
        } else {
          break;
        }
      }
      if((value = nres.hint("DOCNUM")) != null) stdout.println("DOCNUM\t" + value);
      if((value = nres.hint("WORDNUM")) != null) stdout.println("WORDNUM\t" + value);
      if((value = nres.hint("TIME")) != null) stdout.println("TIME\t" + value);
      if((value = nres.hint("TIME#i")) != null) stdout.println("TIME#i\t" + value);
      for(int i = 0;; i++){
        String key = "TIME#" + i;
        if((value = nres.hint(key)) != null){
          stdout.println(key + "\t" + value);
        } else {
          break;
        }
      }
      for(int i = 0;; i++){
        String key = "LINK#" + i;
        if((value = nres.hint(key)) != null){
          stdout.println(key + "\t" + value);
        } else {
          break;
        }
      }
      if((value = nres.hint("VIEW")) != null) stdout.println("VIEW\t" + value);
      stdout.println("");
      for(int i = 0; i < nres.doc_num(); i++){
        stdout.println(border);
        ResultDocument rdoc = nres.get_doc(i);
        List names = rdoc.attr_names();
        Iterator it = names.iterator();
        while(it.hasNext()){
          String name = (String)it.next();
          stdout.println(name + "=" + rdoc.attr(name));
        }
        String keywords = rdoc.keywords();
        if(keywords.length() > 0) stdout.println("%VECTOR\t" + keywords);
        stdout.println("");
        stdout.print(rdoc.snippet());
      }
      stdout.println(border + ":END");
    } else {
      printerror("failed: " + node.status());
      return 1;
    }
    return 0;
  }
  /* perform the setuser command */
  private static int procsetuser(String nurl, String name, int mode){
    Node node = new Node();
    node.set_url(nurl);
    if(pxhost != null) node.set_proxy(pxhost, pxport);
    if(timeout > 0) node.set_timeout(timeout);
    if(authname != null) node.set_auth(authname, authpass);
    if(!node.set_user(name, mode)){
      printerror("failed: " + node.status());
      return 1;
    }
    return 0;
  }
  /* perform the setlink command */
  private static int procsetlink(String nurl, String url, String label, int credit){
    Node node = new Node();
    node.set_url(nurl);
    if(pxhost != null) node.set_proxy(pxhost, pxport);
    if(timeout > 0) node.set_timeout(timeout);
    if(authname != null) node.set_auth(authname, authpass);
    if(!node.set_link(url, label, credit)){
      printerror("failed: " + node.status());
      return 1;
    }
    return 0;
  }
  /* dummy method: hide the constructor */
  private Call(){}
}



/* END OF FILE */
