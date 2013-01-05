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
 * Abstraction of connection to P2P node.
 */
public class Node {
  //----------------------------------------------------------------
  // private static methods
  //----------------------------------------------------------------
  /**
   * Serialize a condition object into a query string.
   * @param cond a condition object.
   * @param depth depth of meta search.
   * @param wwidth whole width of a snippet.
   * @param hwidth width of strings picked up from the beginning of the text.
   * @param awidth width of strings picked up around each highlighted word.
   * @return a serialized string of the condtion object.
   */
  private static String cond_to_query(Condition cond, int depth,
                                      int wwidth, int hwidth, int awidth){
    StringBuffer sb = new StringBuffer();
    try {
      String phrase = cond.phrase();
      sb.append("phrase=");
      if(phrase != null) sb.append(URLEncoder.encode(phrase, "UTF-8"));
      List attrs = cond.attrs();
      Iterator it = cond.attrs().iterator();
      for(int i = 1; it.hasNext(); i++){
        sb.append("&attr");
        sb.append(i);
        sb.append("=");
        sb.append(URLEncoder.encode((String)it.next(), "UTF-8"));
      }
      String order = cond.order();
      if(order != null && order.length() > 0){
        sb.append("&order=");
        sb.append(URLEncoder.encode(order, "UTF-8"));
      }
      int max = cond.max();
      if(max >= 0){
        sb.append("&max=");
        sb.append(max);
      } else {
        sb.append("&max=");
        sb.append(Integer.MAX_VALUE / 2);
      }
      int options = cond.options();
      if(options > 0){
        sb.append("&options=");
        sb.append(options);
      }
      int auxiliary = cond.auxiliary();
      sb.append("&auxiliary=");
      sb.append(auxiliary);
      String distinct = cond.distinct();
      if(distinct != null && distinct.length() > 0){
        sb.append("&distinct=");
        sb.append(URLEncoder.encode(distinct, "UTF-8"));
      }
      if(depth > 0){
        sb.append("&depth=");
        sb.append(depth);
      }
      sb.append("&wwidth=");
      sb.append(wwidth);
      sb.append("&hwidth=");
      sb.append(hwidth);
      sb.append("&awidth=");
      sb.append(awidth);
      sb.append("&skip=");
      sb.append(cond.skip());
      sb.append("&mask=");
      sb.append(cond.mask());
    } catch(UnsupportedEncodingException e){
      throw new RuntimeException(e);
    }
    return sb.toString();
  }
  //----------------------------------------------------------------
  // private fields
  //----------------------------------------------------------------
  private String url;
  private String pxhost;
  private int pxport;
  private int timeout;
  private String auth;
  private String name;
  private String label;
  private int dnum;
  private int wnum;
  private double size;
  private List admins;
  private List users;
  private List links;
  private int wwidth;
  private int hwidth;
  private int awidth;
  private int status;
  //----------------------------------------------------------------
  // constructors
  //----------------------------------------------------------------
  /**
   * Create a node connection object.
   */
  public Node(){
    url = null;
    pxhost = null;
    pxport = -1;
    timeout = -1;
    auth = null;
    name = null;
    label = null;
    dnum = -1;
    wnum = -1;
    size = -1.0;
    admins = null;
    users = null;
    links = null;
    wwidth = 480;
    hwidth = 96;
    awidth = 96;
    status = -1;
  }
  //----------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------
  /**
   * Set the URL of a node server.
   * @param url the URL of a node server.
   */
  public void set_url(String url){
    this.url = url;
  }
  /**
   * Set the proxy information.
   * @param host the host name of a proxy server.
   * @param port the port number of the proxy server.
   */
  public void set_proxy(String host, int port){
    pxhost = host;
    pxport = port;
  }
  /**
   * Set timeout.
   * @param sec timeout in seconds.
   */
  public void set_timeout(int sec){
    timeout = sec;
  }
  /**
   * Set the authentication information.
   * @param name the name of authentication.
   * @param password the password of the authentication.
   */
  public void set_auth(String name, String password){
    auth = name + ":" + password;
  }
  /**
   * Get the status code of the last request.
   * @return the status code of the last request.  -1 means failure of connection.
   */
  public int status(){
    return status;
  }
  /**
   * Synchronize updating contents of the database.
   * @return true if success, else it is false.
   */
  public boolean sync(){
    status = -1;
    if(url == null) return false;
    try {
      URL purl = new URL(url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/sync");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      reqheads.add("Content-Type: application/x-www-form-urlencoded");
      byte[] reqbody = new byte[0];
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, reqbody, null, null);
      if(status != 200) return false;
      return true;
    } catch(Exception e){
      return false;
    }
  }
  /**
   * Optimize the database.
   * @return true if success, else it is false.
   */
  public boolean optimize(){
    status = -1;
    if(url == null) return false;
    try {
      URL purl = new URL(url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/optimize");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      reqheads.add("Content-Type: application/x-www-form-urlencoded");
      byte[] reqbody = new byte[0];
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, reqbody, null, null);
      if(status != 200) return false;
      return true;
    } catch(Exception e){
      return false;
    }
  }
  /**
   * Add a document.
   * @param doc a document object to register.
   * @return true if success, else it is false.
   */
  public boolean put_doc(Document doc){
    status = -1;
    if(url == null) return false;
    try {
      URL purl = new URL(url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/put_doc");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      reqheads.add("Content-Type: text/x-estraier-draft");
      byte[] reqbody = doc.dump_draft().getBytes("UTF-8");
      ByteArrayOutputStream resbody = new ByteArrayOutputStream();
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, reqbody, null, resbody);
      if(status != 200) return false;
      return true;
    } catch(Exception e){
      return false;
    }
  }
  /**
   * Remove a document.
   * @param id the ID number of a registered document.
   * @return true if success, else it is false.
   */
  public boolean out_doc(int id){
    status = -1;
    if(url == null) return false;
    try {
      URL purl = new URL(url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/out_doc");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      reqheads.add("Content-Type: application/x-www-form-urlencoded");
      byte[] reqbody = ("id=" + id).getBytes("ISO-8859-1");
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, reqbody, null, null);
      if(status != 200) return false;
      return true;
    } catch(Exception e){
      return false;
    }
  }
  /**
   * Remove a document specified by URI.
   * @param uri the URI of a registered document.
   * @return true if success, else it is false.
   */
  public boolean out_doc_by_uri(String uri){
    status = -1;
    if(url == null) return false;
    try {
      URL purl = new URL(url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/out_doc");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      reqheads.add("Content-Type: application/x-www-form-urlencoded");
      byte[] reqbody = ("uri=" + URLEncoder.encode(uri, "UTF-8")).getBytes("ISO-8859-1");
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, reqbody, null, null);
      if(status != 200) return false;
      return true;
    } catch(Exception e){
      return false;
    }
  }
  /**
   * Edit attributes of a document.
   * @param doc a document object.
   * @return true if success, else it is false.
   */
  public boolean edit_doc(Document doc){
    status = -1;
    if(url == null) return false;
    try {
      URL purl = new URL(url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/edit_doc");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      reqheads.add("Content-Type: text/x-estraier-draft");
      byte[] reqbody = doc.dump_draft().getBytes("UTF-8");
      ByteArrayOutputStream resbody = new ByteArrayOutputStream();
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, reqbody, null, resbody);
      if(status != 200) return false;
      return true;
    } catch(Exception e){
      return false;
    }
  }
  /**
   * Retrieve a document.
   * @param id the ID number of a registered document.
   * @return a document object.  On error, `null' is returned.
   */
  public Document get_doc(int id){
    status = -1;
    if(url == null) return null;
    try {
      URL purl = new URL(url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/get_doc");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      reqheads.add("Content-Type: application/x-www-form-urlencoded");
      byte[] reqbody = ("id=" + id).getBytes("ISO-8859-1");
      ByteArrayOutputStream resbody = new ByteArrayOutputStream();
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, reqbody, null, resbody);
      if(status != 200) return null;
      return new Document(resbody.toString("UTF-8"));
    } catch(Exception e){
      return null;
    }
  }
  /**
   * Retrieve a document specified by URI.
   * @param uri the URI of a registered document.
   * @return a document object.  On error, `null' is returned.
   */
  public Document get_doc_by_uri(String uri){
    status = -1;
    if(url == null) return null;
    try {
      URL purl = new URL(url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/get_doc");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      reqheads.add("Content-Type: application/x-www-form-urlencoded");
      byte[] reqbody = ("uri=" + URLEncoder.encode(uri, "UTF-8")).getBytes("ISO-8859-1");
      ByteArrayOutputStream resbody = new ByteArrayOutputStream();
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, reqbody, null, resbody);
      if(status != 200) return null;
      return new Document(resbody.toString("UTF-8"));
    } catch(Exception e){
      return null;
    }
  }
  /**
   * Retrieve the value of an attribute of a document.
   * @param id the ID number of a registered document.
   * @param name the name of an attribute.
   * @return the value of the attribute or `null' if it does not exist.
   */
  public String get_doc_attr(int id, String name){
    status = -1;
    if(url == null) return null;
    try {
      URL purl = new URL(url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/get_doc_attr");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      reqheads.add("Content-Type: application/x-www-form-urlencoded");
      String qstr = "id=" + id + "&attr=" + URLEncoder.encode(name, "UTF-8");
      byte[] reqbody = qstr.getBytes("ISO-8859-1");
      ByteArrayOutputStream resbody = new ByteArrayOutputStream();
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, reqbody, null, resbody);
      if(status != 200) return null;
      return resbody.toString("UTF-8").trim();
    } catch(Exception e){
      return null;
    }
  }
  /**
   * Retrieve the value of an attribute of a document specified by URI
   * @param uri the URI of a registered document.
   * @param name the name of an attribute.
   * @return the value of the attribute or `null' if it does not exist.
   */
  public String get_doc_attr_by_uri(String uri, String name){
    status = -1;
    if(url == null) return null;
    try {
      URL purl = new URL(url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/get_doc_attr");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      reqheads.add("Content-Type: application/x-www-form-urlencoded");
      String qstr = "uri=" + URLEncoder.encode(uri, "UTF-8") +
        "&attr=" + URLEncoder.encode(name, "UTF-8");
      byte[] reqbody = qstr.getBytes("ISO-8859-1");
      ByteArrayOutputStream resbody = new ByteArrayOutputStream();
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, reqbody, null, resbody);
      if(status != 200) return null;
      return resbody.toString("UTF-8").trim();
    } catch(Exception e){
      return null;
    }
  }
  /**
   * Extract keywords of a document.
   * @param id the ID number of a registered document.
   * @return a map object of keywords and their scores in decimal string or `null' on error.
   */
  public Map etch_doc(int id){
    status = -1;
    if(url == null) return null;
    try {
      URL purl = new URL(url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/etch_doc");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      reqheads.add("Content-Type: application/x-www-form-urlencoded");
      String qstr = "id=" + id;
      byte[] reqbody = qstr.getBytes("ISO-8859-1");
      ByteArrayOutputStream resbody = new ByteArrayOutputStream();
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, reqbody, null, resbody);
      if(status != 200) return null;
      Map kwords = new HashMap(31);
      String[] lines = Utility.split_lines(resbody.toString("UTF-8"));
      for(int i = 0; i < lines.length; i++){
        String[] pair = Utility.split_fields(lines[i]);
        if(pair.length < 2) continue;
        kwords.put(pair[0], pair[1]);
      }
      return kwords;
    } catch(Exception e){
      return null;
    }
  }
  /**
   * Extract keywords of a document specified by URI.
   * @param uri the URI of a registered document.
   * @return a map object of keywords and their scores in decimal string or `null' on error.
   */
  public Map etch_doc_by_uri(String uri){
    status = -1;
    if(url == null) return null;
    try {
      URL purl = new URL(url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/etch_doc");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      reqheads.add("Content-Type: application/x-www-form-urlencoded");
      String qstr = "uri=" + URLEncoder.encode(uri, "UTF-8");
      byte[] reqbody = qstr.getBytes("ISO-8859-1");
      ByteArrayOutputStream resbody = new ByteArrayOutputStream();
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, reqbody, null, resbody);
      if(status != 200) return null;
      Map kwords = new HashMap(31);
      String[] lines = Utility.split_lines(resbody.toString("UTF-8"));
      for(int i = 0; i < lines.length; i++){
        String[] pair = Utility.split_fields(lines[i]);
        if(pair.length < 2) continue;
        kwords.put(pair[0], pair[1]);
      }
      return kwords;
    } catch(Exception e){
      return null;
    }
  }
  /**
   * Get the ID of a document specified by URI.
   * @param uri the URI of a registered document.
   * @return the ID of the document.  On error, -1 is returned.
   */
  public int uri_to_id(String uri){
    status = -1;
    if(url == null) return -1;
    try {
      URL purl = new URL(url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/uri_to_id");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      reqheads.add("Content-Type: application/x-www-form-urlencoded");
      byte[] reqbody = ("uri=" + URLEncoder.encode(uri, "UTF-8")).getBytes("ISO-8859-1");
      ByteArrayOutputStream resbody = new ByteArrayOutputStream();
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, reqbody, null, resbody);
      if(status != 200) return -1;
      return Integer.parseInt(resbody.toString("ISO-8859-1").trim());
    } catch(Exception e){
      return -1;
    }
  }
  /**
   * Get the name.
   * @return the name.  On error, `null' is returned.
   */
  public String name(){
    if(name == null) set_info();
    return name;
  }
  /**
   * Get the label.
   * @return the label.  On error, `null' is returned.
   */
  public String label(){
    if(label == null) set_info();
    return label;
  }
  /**
   * Get the number of documents.
   * @return the number of documents.  On error, -1 is returned.
   */
  public int doc_num(){
    if(dnum < 0) set_info();
    return dnum;
  }
  /**
   * Get the number of unique words.
   * @return the number of unique words.  On error, -1 is returned.
   */
  public int word_num(){
    if(wnum < 0) set_info();
    return wnum;
  }
  /**
   * Get the size of the datbase.
   * @return the size of the datbase.  On error, -1.0 is returned.
   */
  public double size(){
    if(size < 0.0) set_info();
    return size;
  }
  /**
   * Get the usage ratio of the cache.
   * @return the usage ratio of the cache.  On error, -1.0 is returned.
   */
  public double cache_usage(){
    status = -1;
    if(url == null) return -1.0;
    try {
      URL purl = new URL(url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/cacheusage");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      ByteArrayOutputStream resbody = new ByteArrayOutputStream();
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, null, null, resbody);
      if(status != 200) return -1.0;
      return Double.parseDouble(resbody.toString("UTF-8").trim());
    } catch(Exception e){
      return -1.0;
    }
  }
  /**
   * Get a list of names of administrators.
   * @return a list object of names of administrators.  On error, `null' is returned.
   */
  public List admins(){
    if(admins == null) set_info();
    return admins;
  }
  /**
   * Get a list of names of users.
   * @return a list object of names of users.  On error, `null' is returned.
   */
  public List users(){
    if(users == null) set_info();
    return users;
  }
  /**
   * Get a list of expressions of links.
   * @return a list object of expressions of links.  Each element is a TSV string and has three
   * fields of the URL, the label, and the score.  On error, `null' is returned.
   */
  public List links(){
    if(links == null) set_info();
    return links;
  }
  /**
   * Search for documents corresponding a condition.
   * @param cond a condition object.
   * @param depth the depth of meta search.
   * @return a node result object.  On error, `null' is returned.
   */
  public NodeResult search(Condition cond, int depth){
    status = -1;
    if(url == null) return null;
    try {
      URL purl = new URL(url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/search");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      reqheads.add("Content-Type: application/x-www-form-urlencoded");
      byte[] reqbody = cond_to_query(cond, depth, wwidth, hwidth, awidth).getBytes("ISO-8859-1");
      ByteArrayOutputStream resbody = new ByteArrayOutputStream();
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, reqbody, null, resbody);
      if(status != 200) return null;
      String[] lines = Utility.split_lines(resbody.toString("UTF-8"));
      if(lines.length < 1) return null;
      List docs = new ArrayList(31);
      Map hints = new HashMap(31);
      NodeResult nres = new NodeResult(docs, hints);
      String border = lines[0];
      boolean end = false;
      int lnum = 1;
      while(lnum < lines.length){
        String line = lines[lnum++];
        if(line.startsWith(border)){
          if(line.substring(border.length(), line.length()).equals(":END")) end = true;
          break;
        }
        int lidx = line.indexOf('\t');
        if(lidx != -1){
          String key = line.substring(0, lidx);
          String value = line.substring(lidx + 1, line.length());
          hints.put(key, value);
        }
      }
      int snum = lnum;
      while(!end && lnum < lines.length){
        String line = lines[lnum++];
        if(line.startsWith(border)){
          if(lnum > snum){
            Map rdattrs = new HashMap(31);
            StringBuffer sb = new StringBuffer();
            String rdvector = "";
            int rlnum = snum;
            while(rlnum < lnum - 1){
              String rdline = lines[rlnum++].trim();
              if(rdline.length() < 1) break;
              if(rdline.startsWith("%")){
                int lidx = rdline.indexOf('\t');
                if(rdline.startsWith("%VECTOR") && lidx != -1)
                  rdvector = rdline.substring(lidx + 1, rdline.length());
              } else {
                int lidx = rdline.indexOf('=');
                if(lidx != -1){
                  String key = rdline.substring(0, lidx);
                  String value = rdline.substring(lidx + 1, rdline.length());
                  rdattrs.put(key, value);
                }
              }
            }
            while(rlnum < lnum - 1){
              String rdline = lines[rlnum++];
              sb.append(rdline);
              sb.append("\n");
            }
            String rduri = (String)rdattrs.get("@uri");
            String rdsnippet = sb.toString();
            if(rduri != null){
              ResultDocument rdoc = new ResultDocument(rduri, rdattrs, rdsnippet, rdvector);
              docs.add(rdoc);
            }
          }
          snum = lnum;
          if(line.substring(border.length(), line.length()).equals(":END")) end = true;
        }
      }
      if(!end) return null;
      return nres;
    } catch(Exception e){
      return null;
    }
  }
  /**
   * Set width of snippet in the result.
   * @param wwidth whole width of a snippet.  By default, it is 480.  If it is 0, no snippet is
   * sent. If it is negative, whole body text is sent instead of snippet.
   * @param hwidth width of strings picked up from the beginning of the text.  By default, it
   * is 96.  If it is negative 0, the current setting is not changed.
   * @param awidth width of strings picked up around each highlighted word. By default, it is
   * 96.  If it is negative, the current setting is not changed.
   */
  public void set_snippet_width(int wwidth, int hwidth, int awidth){
    this.wwidth = wwidth;
    if(hwidth >= 0) this.hwidth = hwidth;
    if(awidth >= 0) this.awidth = awidth;
  }
  /**
   * Manage a user account.
   * @param name the name of a user.
   * @param mode the operation mode.  0 means to delete the account.  1 means to set the
   * account as an administrator.  2 means to set the account as a guest.
   * @return true if success, else it is false.
   */
  public boolean set_user(String name, int mode){
    status = -1;
    if(url == null) return false;
    try {
      URL purl = new URL(url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/_set_user");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      reqheads.add("Content-Type: application/x-www-form-urlencoded");
      String qstr = "name=" + URLEncoder.encode(name, "UTF-8") + "&mode=" + mode;
      byte[] reqbody = qstr.getBytes("ISO-8859-1");
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, reqbody, null, null);
      if(status != 200) return false;
      return true;
    } catch(Exception e){
      return false;
    }
  }
  /**
   * Manage a link.
   * @param url the URL of the target node of a link.
   * @param label the label of the link.
   * @param credit the credit of the link.  If it is negative, the link is removed.
   * @return true if success, else it is false.
   */
  public boolean set_link(String url, String label, int credit){
    status = -1;
    if(this.url == null) return false;
    try {
      URL purl = new URL(this.url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/_set_link");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      reqheads.add("Content-Type: application/x-www-form-urlencoded");
      String qstr = "url=" + URLEncoder.encode(url, "UTF-8") +
        "&label=" + URLEncoder.encode(label, "UTF-8");
      if(credit >= 0) qstr = qstr + "&credit=" + credit;
      byte[] reqbody = qstr.getBytes("ISO-8859-1");
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, reqbody, null, null);
      if(status != 200) return false;
      return true;
    } catch(Exception e){
      return false;
    }
  }
  //----------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------
  /**
   * Set information of the node.
   */
  private void set_info(){
    status = -1;
    if(url == null) return;
    try {
      URL purl = new URL(url);
      URL eurl = new URL("http://" + purl.getHost() + ":" + purl.getPort() +
                         purl.getPath() + "/inform");
      List reqheads = new ArrayList(2);
      if(auth != null)
        reqheads.add("Authorization: Basic " + Utility.base_encode(auth.getBytes()));
      ByteArrayOutputStream resbody = new ByteArrayOutputStream();
      status = Utility.shuttle_url(eurl.toString(), pxhost, pxport, timeout,
                                   reqheads, null, null, resbody);
      if(status != 200) return;
      String resstr = resbody.toString("UTF-8");
      String[] lines = Utility.split_lines(resstr);
      if(lines.length < 1) return;
      String[] fields = Utility.split_fields(lines[0]);
      if(fields.length != 5) return;
      name = fields[0];
      label = fields[1];
      dnum = Integer.parseInt(fields[2]);
      wnum = Integer.parseInt(fields[3]);
      size = Double.parseDouble(fields[4]);
      if(dnum < 0 || wnum < 0 || size < 0.0){
        dnum = -1;
        wnum = -1;
        size = -1.0;
      }
      if(lines.length < 2) return;
      int lnum = 1;
      if(lnum < lines.length && lines[lnum].length() < 1) lnum++;
      admins = new ArrayList();
      while(lnum < lines.length){
        String line = lines[lnum];
        if(line.length() < 1) break;
        admins.add(line);
        lnum++;
      }
      if(lnum < lines.length && lines[lnum].length() < 1) lnum++;
      users = new ArrayList();
      while(lnum < lines.length){
        String line = lines[lnum];
        if(line.length() < 1) break;
        users.add(line);
        lnum++;
      }
      if(lnum < lines.length && lines[lnum].length() < 1) lnum++;
      links = new ArrayList();
      while(lnum < lines.length){
        String line = lines[lnum];
        if(line.length() < 1) break;
        if(Utility.split_fields(line).length == 3) links.add(line);
        lnum++;
      }
    } catch(Exception e){
      name = null;
      label = null;
      dnum = -1;
      wnum = -1;
      size = -1.0;
      admins = null;
      users = null;
      links = null;
    }
  }
}



/* END OF FILE */
