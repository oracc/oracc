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
 * Abstraction of search condition.
 */
public class Condition {
  //----------------------------------------------------------------
  // public constants
  //----------------------------------------------------------------
  /** option: check every N-gram key */
  public static final int SURE = 1 << 0;
  /** option: check N-gram keys skipping by one */
  public static final int USUAL = 1 << 1;
  /** option: check N-gram keys skipping by two */
  public static final int FAST = 1 << 2;
  /** option: check N-gram keys skipping by three */
  public static final int AGITO = 1 << 3;
  /** option: without TF-IDF tuning */
  public static final int NOIDF = 1 << 4;
  /** option: with the simplified phrase */
  public static final int SIMPLE = 1 << 10;
  /** option: with the rough phrase */
  public static final int ROUGH = 1 << 11;
  /** option: with the union phrase */
  public static final int UNION = 1 << 15;
  /** option: with the intersection phrase */
  public static final int ISECT = 1 << 16;
  //----------------------------------------------------------------
  // private fields
  //----------------------------------------------------------------
  private String phrase;
  private List attrs;
  private String order;
  private int max;
  private int skip;
  private int options;
  private int auxiliary;
  private String distinct;
  private int mask;
  //----------------------------------------------------------------
  // constructors
  //----------------------------------------------------------------
  /**
   * Create a search condition object.
   */
  public Condition(){
    phrase = null;
    attrs = new ArrayList(31);
    order = null;
    max = -1;
    skip = 0;
    options = 0;
    auxiliary = 32;
    distinct = null;
    mask = 0;
  }
  //----------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------
  /**
   * Set the search phrase.
   * @param phrase a search phrase.
   */
  public void set_phrase(String phrase){
    this.phrase = phrase;
  }
  /**
   * Add an expression for an attribute.
   * @param expr an expression for an attribute.
   */
  public void add_attr(String expr){
    attrs.add(expr);
  }
  /**
   * Set the order.
   * @param expr an expression for the order.  By default, the order is by score descending.
   */
  public void set_order(String expr){
    order = expr;
  }
  /**
   * Set the maximum number of retrieval.
   * @param max the maximum number of retrieval.  By default, the number of retrieval is not
   * limited.
   */
  public void set_max(int max){
    if(max >= 0) this.max = max;
  }
  /**
   * Set the number of skipped documents.
   * @param skip the number of documents to be skipped in the search result.
   */
  public void set_skip(int skip){
    if(skip >= 0) this.skip = skip;
  }
  /**
   * Set options of retrieval.
   * @param options options: `Condition.SURE' specifies that it checks every N-gram key,
   * `Condition.USUAL', which is the default, specifies that it checks N-gram keys with skipping
   * one key, `Condition.FAST' skips two keys, `Condition.AGITO' skips three keys,
   * `Condition.NOIDF' specifies not to perform TF-IDF tuning, `Condition.SIMPLE' specifies to
   * use simplified phrase, `Condition.ROUGH' specifies to use rough phrase, `Condition.UNION'
   * specifies to use union phrase, `Condition.ISECT' specifies to use intersection phrase.  Each
   * option can be specified at the same time by bitwise or.  If keys are skipped, though search
   * speed is improved, the relevance ratio grows less.
   */
  public void set_options(int options){
    this.options |= options;
  }
  /**
   * Set permission to adopt result of the auxiliary index.
   * @param min the minimum hits to adopt result of the auxiliary index.  If it is not more
   * than 0, the auxiliary index is not used.  By default, it is 32.
   */
  public void set_auxiliary(int min){
    this.auxiliary = min;
  }
  /**
   * Set the attribute distinction filter.
   * @param name the name of an attribute to be distinct.
   */
  public void set_distinct(String name){
    distinct = name;
  }
  /**
   * Set the mask of targets of meta search.
   * @param mask a masking number.  1 means the first target, 2 means the second target, 4 means
   * the third target and, power values of 2 and their summation compose the mask.
   */
  public void set_mask(int mask){
    this.mask = mask & 0x7fffffff;
  }
  //----------------------------------------------------------------
  // package methods
  //----------------------------------------------------------------
  /**
   * Get the search phrase.
   * @return the search phrase.
   */
  String phrase(){
    if(phrase == null) return "";
    return phrase;
  }
  /**
   * Get expressions for attributes.
   * @return expressions for attributes.
   */
  List attrs(){
    return attrs;
  }
  /**
   * Get the order expression.
   * @return the order expression.
   */
  String order(){
    if(order == null) return "";
    return order;
  }
  /**
   * Get the maximum number of retrieval.
   * @return the maximum number of retrieval.
   */
  int max(){
    return max;
  }
  /**
   * Get the number of skipped documents.
   * @return the number of documents to be skipped in the search result.
   */
  int skip(){
    return skip;
  }
  /**
   * Get options of retrieval.
   * @return options by bitwise or.
   */
  int options(){
    return options;
  }
  /**
   * Get permission to adopt result of the auxiliary index.
   * @return permission to adopt result of the auxiliary index.
   */
  int auxiliary(){
    return auxiliary;
  }
  /**
   * Get the attribute distinction filter.
   * @return the name of the distinct attribute.
   */
  String distinct(){
    if(distinct == null) return "";
    return distinct;
  }
  /**
   * Get the mask of targets of meta search.
   * @return the mask of targets of meta search.
   */
  int mask(){
    return mask;
  }
}



/* END OF FILE */
