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
 * Abstraction of result set from database.
 */
public class Result {
  //----------------------------------------------------------------
  // private fields
  //----------------------------------------------------------------
  /** ID numbers of corresponding documents */
  private int[] ids;
  /** indexes of database array of meta search */
  private int[] dbidxs;
  /** map for hints */
  private Map hints;
  /** pointer to the native condition object */
  private long condptr;
  //----------------------------------------------------------------
  // constructors and finalizers
  //----------------------------------------------------------------
  /**
   * Create a result set object.
   */
  Result(int[] ids, Map hints, long condptr){
    this.ids = ids;
    this.dbidxs = null;
    this.hints = hints;
    this.condptr = condptr;
  }
  /**
   * Create a result set object for meta search.
   */
  Result(int[] ids, int[] dbidxs, Map hints, long condptr){
    this.ids = ids;
    this.dbidxs = dbidxs;
    this.hints = hints;
    this.condptr = condptr;
  }
  /**
   * Release resources.
   */
  protected void finalize(){
    destroy();
  }
  //----------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------
  /**
   * Get the number of documents.
   * @return the number of documents in the result.
   */
  public int doc_num(){
    return ids.length;
  }
  /**
   * Get the ID number of a document.
   * @param index the index of a document.
   * @return the ID number of the document or -1 if the index is out of bounds.
   */
  public int get_doc_id(int index){
    if(index < 0 || index >= ids.length) return -1;
    return ids[index];
  }
  /**
   * Get the index of the container database of a document.
   * @param index the index of a document.
   * @return the index of the container database of the document or -1 if the index is out of
   * bounds.
   */
  public int get_dbidx(int index){
    if(dbidxs == null || index < 0 || index >= dbidxs.length) return -1;
    return dbidxs[index];
  }
  /**
   * Get a list of hint words.
   * @return a list of hint words.
   */
  public List hint_words(){
    List words = new ArrayList(hints.size());
    Iterator it = hints.keySet().iterator();
    while(it.hasNext()){
      String word = (String)it.next();
      if(word.length() > 0) words.add(word);
    }
    return words;
  }
  /**
   * Get the value of a hint word.
   * @param word a hint word.  An empty string means the number of whole result.
   * @return the number of documents corresponding the hint word.  If the word is in a negative
   * condition, the value is negative.
   */
  public int hint(String word){
    String value = (String)hints.get(word);
    if(value == null) return 0;
    int rv = 0;
    try {
      rv = Integer.parseInt(value);
    } catch(NumberFormatException e){}
    return rv;
  }
  /**
   * Get the score of a document.
   * @param index the index of a document.
   * @return the score of the document or -1 if the index is out of bounds.
   */
  public native int get_score(int index);
  /**
   * Get an array of ID numbers of eclipsed docuemnts of a document.
   * @param id the ID number of a parent document.
   * @return an array whose elements expresse the ID numbers and their scores alternately.
   */
  public native int[] get_shadows(int id);
  //----------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------
  /**
   * Delete the native resources.
   */
  private native void destroy();
}



/* END OF FILE */
