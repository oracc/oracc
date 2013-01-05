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
 * Abstraction of search condition.
 */
public class Condition {
  //----------------------------------------------------------------
  // static initializer
  //----------------------------------------------------------------
  static {
    Utility.init();
  }
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
  /** eclipse tuning: consider URL */
  public static final double ECLSIMURL = 10.0;
  /** eclipse tuning: on server basis */
  public static final double ECLSERV = 100.0;
  /** eclipse tuning: on directory basis */
  public static final double ECLDIR = 101.0;
  /** eclipse tuning: on file basis */
  public static final double ECLFILE = 102.0;
  //----------------------------------------------------------------
  // private fields
  //----------------------------------------------------------------
  /** pointer to the native object */
  private long coreptr;
  //----------------------------------------------------------------
  // constructors and finalizers
  //----------------------------------------------------------------
  /**
   * Create a search condition object.
   */
  public Condition(){
    initialize();
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
   * Set the search phrase.
   * @param phrase a search phrase.
   */
  public native void set_phrase(String phrase);
  /**
   * Add an expression for an attribute.
   * @param expr an expression for an attribute.
   */
  public native void add_attr(String expr);
  /**
   * Set the order.
   * @param expr an expression for the order.  By default, the order is by score descending.
   */
  public native void set_order(String expr);
  /**
   * Set the maximum number of retrieval.
   * @param max the maximum number of retrieval.  By default, the number of retrieval is not
   * limited.
   */
  public native void set_max(int max);
  /**
   * Set the number of skipped documents.
   * @param skip the number of documents to be skipped in the search result.
   */
  public native void set_skip(int skip);
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
  public native void set_options(int options);
  /**
   * Set permission to adopt result of the auxiliary index.
   * @param min the minimum hits to adopt result of the auxiliary index.  If it is not more
   * than 0, the auxiliary index is not used.  By default, it is 32.
   */
  public native void set_auxiliary(int min);
  /**
   * Set the lower limit of similarity eclipse.
   * @param limit the lower limit of similarity for documents to be eclipsed.  Similarity is
   * between 0.0 and 1.0.  If the limit is added by `Condition.ECLSIMURL', similarity is
   * weighted by URL.  If the limit is `Condition.ECLSERV', similarity is ignored and documents
   * in the same server are eclipsed.  If the limit is `Condition.ECLDIR', similarity is ignored
   * and documents in the same directory are eclipsed.  If the limit is `Condition.ECLFILE',
   * similarity is ignored and documents of the same file are eclipsed.
   */
  public native void set_eclipse(double limit);
  /**
   * Set the attribute distinction filter.
   * @param name the name of an attribute to be distinct.
   */
  public native void set_distinct(String name);
  /**
   * Set the mask of targets of meta search.
   * @param mask a masking number.  1 means the first target, 2 means the second target, 4 means
   * the third target and, power values of 2 and their summation compose the mask.
   */
  public native void set_mask(int mask);
  //----------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------
  /**
   * Set the native object.
   */
  private native void initialize();
  /**
   * Delete the native object.
   */
  private native void destroy();
}



/* END OF FILE */
