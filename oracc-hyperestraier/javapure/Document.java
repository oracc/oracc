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
 * Abstraction of document.
 */
public class Document {
  //----------------------------------------------------------------
  // private fields
  //----------------------------------------------------------------
  private int id;
  private Map attrs;
  private List dtexts;
  private List htexts;
  private Map kwords;
  private int score;
  //----------------------------------------------------------------
  // constructors
  //----------------------------------------------------------------
  /**
   * Create a document object.
   */
  public Document(){
    id = -1;
    attrs = new HashMap(31);
    dtexts = new ArrayList(31);
    htexts = new ArrayList(31);
    kwords = null;
    score = -1;
  }
  /**
   * Create a document object made from draft data.
   * @param draft a string of draft data.
   */
  public Document(String draft){
    this();
    String[] lines = Utility.split_lines(draft);
    int lnum = 0;
    while(lnum < lines.length){
      String line = lines[lnum++].trim();
      if(line.length() < 1) break;
      if(line.startsWith("%")){
        if(line.startsWith("%VECTOR\t")){
          if(kwords == null) kwords = new HashMap(32);
          String[] fields = Utility.split_fields(line);
          for(int i = 1; i < fields.length - 1; i += 2){
            if(fields[i].length() < 0 || fields[i+1].length() < 0 ||
               fields[i].charAt(0) <= ' ') continue;
            kwords.put(fields[i], fields[i+1]);
          }
        } else if(line.startsWith("%SCORE\t")){
          String[] fields = Utility.split_fields(line);
          score = Integer.parseInt(fields[1]);
        }
        continue;
      }
      int lidx = line.indexOf('=');
      if(lidx != -1) add_attr(line.substring(0, lidx), line.substring(lidx + 1, line.length()));
    }
    while(lnum < lines.length){
      String line = lines[lnum++];
      if(line.length() < 1) continue;
      if(line.charAt(0) == '\t'){
        line = line.substring(1, line.length());
        if(line.length() > 0) add_hidden_text(line);
      } else {
        add_text(line);
      }
    }
  }
  //----------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------
  /**
   * Add an attribute.
   * @param name the name of an attribute.
   * @param value the value of the attribute.  If it is `null', the attribute is removed.
   */
  public void add_attr(String name, String value){
    if(value != null){
      attrs.put(name, value.trim());
    } else {
      attrs.remove(name);
    }
  }
  /**
   * Add a sentence of text.
   * @param text sentence of text.
   */
  public void add_text(String text){
    text = text.trim();
    if(text.length() > 0) dtexts.add(text);
  }
  /**
   * Add a hidden sentence.
   * @param text a hidden sentence.
   */
  public void add_hidden_text(String text){
    text = text.trim();
    if(text.length() > 0) htexts.add(text);
  }
  /**
   * Attach keywords.
   * @param kwords a map object of keywords.  Keys of the map should be keywords of the document
   * and values should be their scores in decimal string.
   */
  public void set_keywords(Map kwords){
    this.kwords = kwords;
  }
  /**
   * Set the substitute score.
   * @param score the substitute score.  It it is negative, the substitute score setting is
   * nullified.
   */
  private void set_score(int score){
    this.score = score;
  }
  /**
   * Get the ID number.
   * @return the ID number.  If this object has never been registered, -1 is returned.
   */
  public int id(){
    return id;
  }
  /**
   * Get a list of attribute names.
   * @return a list object of attribute names.
   */
  public List attr_names(){
    List names = new ArrayList(attrs.size());
    Iterator it = attrs.keySet().iterator();
    while(it.hasNext()){
      names.add(it.next());
    }
    Collections.sort(names);
    return names;
  }
  /**
   * Get the value of an attribute.
   * @param name the name of an attribute.
   * @return the value of the attribute or `null' if it does not exist.
   */
  public String attr(String name){
    return (String)attrs.get(name);
  }
  /**
   * Get a list of sentences of the text.
   * @return a list object of sentences of the text.
   */
  public List texts(){
    return dtexts;
  }
  /**
   * Concatenate sentences of the text.
   * @return concatenated sentences.
   */
  public String cat_texts(){
    StringBuffer sb = new StringBuffer();
    Iterator it = dtexts.iterator();
    for(int i = 0; it.hasNext(); i++){
      if(i > 0) sb.append(" ");
      sb.append(it.next());
    }
    return sb.toString();
  }
  /**
   * Get attached keywords.
   * @return a map object of keywords and their scores in decimal string.  If no keyword is
   * attached, `null' is returned.
   */
  public Map keywords(){
    return kwords;
  }
  /**
   * Get the substitute score.
   * @return the substitute score or -1 if it is not set.
   */
  public int score(){
    if(score < 0) return -1;
    return score;
  }
  /**
   * Dump draft data.
   * @return draft data.
   */
  public String dump_draft(){
    StringBuffer sb = new StringBuffer();
    List names = attr_names();
    Iterator attrit = names.iterator();
    while(attrit.hasNext()){
      String name = (String)attrit.next();
      sb.append(name);
      sb.append("=");
      sb.append((String)attrs.get(name));
      sb.append("\n");
    }
    if(kwords != null){
      sb.append("%VECTOR");
      Iterator kwit = kwords.keySet().iterator();
      while(kwit.hasNext()){
        String key = (String)kwit.next();
        sb.append("\t");
        sb.append(key);
        sb.append("\t");
        sb.append((String)kwords.get(key));
      }
      sb.append("\n");
    }
    if(score >= 0) sb.append("%SCORE\t" + score + "\n");
    sb.append("\n");
    Iterator dtit = dtexts.iterator();
    while(dtit.hasNext()){
      sb.append(dtit.next());
      sb.append("\n");
    }
    Iterator htit = htexts.iterator();
    while(htit.hasNext()){
      sb.append("\t");
      sb.append(htit.next());
      sb.append("\n");
    }
    return sb.toString();
  }
}



/* END OF FILE */
