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
 * Abstraction of document.
 */
public class Document {
  //----------------------------------------------------------------
  // static initializer
  //----------------------------------------------------------------
  static {
    Utility.init();
  }
  //----------------------------------------------------------------
  // private fields
  //----------------------------------------------------------------
  /** pointer to the native object */
  private long coreptr;
  //----------------------------------------------------------------
  // constructors and finalizers
  //----------------------------------------------------------------
  /**
   * Create a document object.
   */
  public Document(){
    initialize();
  }
  /**
   * Create a document object made from draft data.
   * @param draft a string of draft data.
   */
  public Document(String draft){
    initialize(draft);
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
   * Add an attribute.
   * @param name the name of an attribute.
   * @param value the value of the attribute.  If it is `null', the attribute is removed.
   */
  public native void add_attr(String name, String value);
  /**
   * Add a sentence of text.
   * @param text sentence of text.
   */
  public native void add_text(String text);
  /**
   * Add a hidden sentence.
   * @param text a hidden sentence.
   */
  public native void add_hidden_text(String text);
  /**
   * Attach keywords.
   * @param kwords a map object of keywords.  Keys of the map should be keywords of the document
   * and values should be their scores in decimal string.
   */
  public native void set_keywords(Map kwords);
  /**
   * Set the substitute score.
   * @param score the substitute score.  It it is negative, the substitute score setting is
   * nullified.
   */
  public native void set_score(int score);
  /**
   * Get the ID number.
   * @return the ID number.  If this object has never been registered, -1 is returned.
   */
  public native int id();
  /**
   * Get a list of attribute names.
   * @return a list object of attribute names.
   */
  public native List attr_names();
  /**
   * Get the value of an attribute.
   * @param name the name of an attribute.
   * @return the value of the attribute or `null' if it does not exist.
   */
  public native String attr(String name);
  /**
   * Get a list of sentences of the text.
   * @return a list object of sentences of the text.
   */
  public native List texts();
  /**
   * Concatenate sentences of the text.
   * @return concatenated sentences.
   */
  public native String cat_texts();
  /**
   * Get attached keywords.
   * @return a map object of keywords and their scores in decimal string.  If no keyword is
   * attached, `null' is returned.
   */
  public native Map keywords();
  /**
   * Get the substitute score.
   * @return the substitute score or -1 if it is not set.
   */
  public native int score();
  /**
   * Dump draft data.
   * @return draft data.
   */
  public native String dump_draft();
  /**
   * Make a snippet of the body text.
   * @param words a list object of words to be highlight.
   * @param wwidth whole width of the result.
   * @param hwidth width of strings picked up from the beginning of the text.
   * @param awidth width of strings picked up around each highlighted word.
   * @return a snippet string of the body text.  There are tab separated values.  Each line is a
   * string to be shown.  Though most lines have only one field, some lines have two fields.  If
   * the second field exists, the first field is to be shown with highlighted, and the second
   * field means its normalized form.
   */
  public native String make_snippet(java.util.List words, int wwidth, int hwidth, int awidth);
  //----------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------
  /**
   * Set the native object.
   */
  private native void initialize();
  /**
   * Set the native object with draft data.
   */
  private native void initialize(String draft);
  /**
   * Delete the native object.
   */
  private native void destroy();
}



/* END OF FILE */
