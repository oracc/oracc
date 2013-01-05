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
 * Abstraction of database
 */
public class Database {
  //----------------------------------------------------------------
  // static initializer
  //----------------------------------------------------------------
  static {
    Utility.init();
  }
  //----------------------------------------------------------------
  // public constants
  //----------------------------------------------------------------
  /** version of Hyper Estraier */
  public static final String VERSION = version();
  /** error code: no error */
  public static final int ERRNOERR = 0;
  /** error code: invalid argument */
  public static final int ERRINVAL = 1;
  /** error code: access forbidden */
  public static final int ERRACCES = 2;
  /** error code: lock failure */
  public static final int ERRLOCK = 3;
  /** error code: database problem */
  public static final int ERRDB = 4;
  /** error code: I/O problem */
  public static final int ERRIO = 5;
  /** error code: no item */
  public static final int ERRNOITEM = 6;
  /** error code: miscellaneous */
  public static final int ERRMISC = 9999;
  /** open mode: open as a reader */
  public static final int DBREADER = 1 << 0;
  /** open mode: open as a writer */
  public static final int DBWRITER = 1 << 1;
  /** open mode: a writer creating */
  public static final int DBCREAT = 1 << 2;
  /** open mode: a writer truncating */
  public static final int DBTRUNC = 1 << 3;
  /** open mode: open without locking */
  public static final int DBNOLCK = 1 << 4;
  /** open mode: lock without blocking */
  public static final int DBLCKNB = 1 << 5;
  /** open mode: use perfect N-gram analyzer */
  public static final int DBPERFNG = 1 << 10;
  /** open mode: use character category analyzer */
  public static final int DBCHRCAT = 1 << 11;
  /** open mode: small tuning */
  public static final int DBSMALL = 1 << 20;
  /** open mode: large tuning */
  public static final int DBLARGE = 1 << 21;
  /** open mode: huge tuning */
  public static final int DBHUGE = 1 << 22;
  /** open mode: huge tuning second */
  public static final int DBHUGE2 = 1 << 23;
  /** open mode: huge tuning third */
  public static final int DBHUGE3 = 1 << 24;
  /** open mode: store scores as void */
  public static final int DBSCVOID = 1 << 25;
  /** open mode: store scores as integer */
  public static final int DBSCINT = 1 << 26;
  /** open mode: refrain from adjustment of scores */
  public static final int DBSCASIS = 1 << 27;
  /** attribute index type: for multipurpose sequencial access method */
  public static final int IDXATTRSEQ = 0;
  /** attribute index type: for narrowing with attributes as strings */
  public static final int IDXATTRSTR = 1;
  /** attribute index type: for narrowing with attributes as numbers */
  public static final int IDXATTRNUM = 2;
  /** optimize option: omit purging dispensable region of deleted */
  public static final int OPTNOPURGE = 1 << 0;
  /** optimize option: omit optimization of the database files */
  public static final int OPTNODBOPT = 1 << 1;
  /** merge option: clean up dispensable regions */
  public static final int MGCLEAN = 1 << 0;
  /** put_doc option: clean up dispensable regions */
  public static final int PDCLEAN = 1 << 0;
  /** put_doc option:  weight scores statically when indexing */
  public static final int PDWEIGHT = 1 << 1;
  /** out_doc option: clean up dispensable regions */
  public static final int ODCLEAN = 1 << 0;
  /** get_doc option: no attributes */
  public static final int GDNOATTR = 1 << 0;
  /** get_doc option: no text */
  public static final int GDNOTEXT = 1 << 1;
  /** get_doc option: no keywords */
  public static final int GDNOKWD = 1 << 2;
  //----------------------------------------------------------------
  // public static methods
  //----------------------------------------------------------------
  /**
   * Search plural databases for documents corresponding a condition.
   * @param dbs an array whose elements are database objects.
   * @param cond a condition object.
   * @return a result object.  On error, `null' is returned.
   */
  public static native Result search_meta(Database[] dbs, Condition cond);
  //----------------------------------------------------------------
  // private static methods
  //----------------------------------------------------------------
  /**
   * Get the version of Hyper Estraier.
   * @return the version of Hyper Estraier.
   */
  private static native String version();
  //----------------------------------------------------------------
  // private fields
  //----------------------------------------------------------------
  /** pointer to the native object */
  private long coreptr;
  /** last happened error code */
  private int ecode;
  /** callback function to inform of database events */
  private DatabaseInformer informer;
  //----------------------------------------------------------------
  // constructors and finalizers
  //----------------------------------------------------------------
  /**
   * Create a database object.
   */
  public Database(){
    coreptr = 0;
    ecode = ERRNOERR;
  }
  /**
   * Release resources.
   */
  protected void finalize(){
    if(coreptr != 0) close();
  }
  //----------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------
  /**
   * Get the string of an error code.
   * @param ecode an error code.
   * @return the string of the error code.
   */
  public native String err_msg(int ecode);
  /**
   * Open the database.
   * @param name the name of a database directory.
   * @param omode open modes: `Database.DBWRITER' as a writer, `Database.DBREADER' as a reader.
   * If the mode is `Database.DBWRITER', the following may be added by bitwise or:
   * `Database.DBCREAT', which means it creates a new database if not exist, `Database.DBTRUNC',
   * which means it creates a new database regardless if one exists.  Both of `Database.DBREADER'
   * and  `Database.DBWRITER' can be added to by bitwise or: `Database.DBNOLCK', which means it
   * opens a database file without file locking, or `Database.DBLCKNB', which means locking is
   * performed without blocking.  If `Database.DBNOLCK' is used, the application is responsible
   * for exclusion control.  `Database.DBCREAT' can be added to by bitwise or:
   * `Database.DBPERFNG', which means N-gram analysis is performed against European text also,
   * `Database.DBCHACAT', which means character category analysis is performed instead of N-gram
   * analysis, `Database.DBSMALL', which means the index is tuned to register less than 50000
   * documents, `Database.DBLARGE', which means the index is tuned to register more than 300000
   * documents, `Database.DBHUGE', which means the index is tuned to register more than 1000000
   * documents, `Database.DBHUGE2', which means the index is tuned to register more than 5000000
   * documents, `Database.DBHUGE3', which means the index is tuned to register more than 10000000
   * documents, `Database.DBSCVOID', which means scores are stored as void, `Database.DBSCINT',
   * which means scores are stored as 32-bit integer, `Database.DBSCASIS', which means scores
   * are stored as-is and marked not to be tuned when search.
   * @return true if success, else it is false.
   */
  public native boolean open(String name, int omode);
  /**
   * Close the database.
   * @return true if success, else it is false.
   */
  public native boolean close();
  /**
   * Get the last happened error code.
   * @return the last happened error code.
   */
  public native int error();
  /**
   * Check whether the database has a fatal error.
   * @return true if the database has fatal erroor, else it is false.
   */
  public native boolean fatal();
  /**
   * Add an index for narrowing or sorting with document attributes.
   * @param name the name of an attribute.
   * @param type the data type of attribute index; `Database.IDXATTRSEQ' for multipurpose
   * sequencial access method, `Database.IDXATTRSTR' for narrowing with attributes as strings,
   * `Database.IDXATTRNUM' for narrowing with attributes as numbers.
   * @return true if success, else it is false.
   */
  public native boolean add_attr_index(String name, int type);
  /**
   * Flush index words in the cache.
   * @param max the maximum number of words to be flushed.  If it not more than zero, all words
   * are flushed.
   * @return true if success, else it is false.
   */
  public native boolean flush(int max);
  /**
   * Synchronize updating contents.
   * @return true if success, else it is false.
   */
  public native boolean sync();
  /**
   * Optimize the database.
   * @param options options: `Database.OPTNOPURGE' to omit purging dispensable region of deleted
   * documents, `Database.OPTNODBOPT' to omit optimization of the database files.  The two can be
   * specified at the same time by bitwise or.
   * @return true if success, else it is false.
   */
  public native boolean optimize(int options);
  /**
   * Merge another database.
   * @param name the name of another database directory.
   * @param options options: `Database.MGCLEAN' to clean up dispensable regions of the deleted
   * document.
   * @return true if success, else it is false.
   */
  public native boolean merge(String name, int options);
  /**
   * Add a document.
   * @param doc a document object.  The document object should have the URI attribute.
   * @param options options: `Database.PDCLEAN' to clean up dispensable regions of the
   * overwritten document.
   * @return true if success, else it is false.
   */
  public native boolean put_doc(Document doc, int options);
  /**
   * Remove a document.
   * @param id the ID number of a registered document.
   * @param options options: `Database.ODCLEAN' to clean up dispensable regions of the deleted
   * document.
   * @return true if success, else it is false.
   */
  public native boolean out_doc(int id, int options);
  /**
   * Edit attributes of a document.
   * @param doc a document object.
   * @return true if success, else it is false.
   */
  public native boolean edit_doc(Document doc);
  /**
   * Retrieve a document.
   * @param id the ID number of a registered document.
   * @param options options: `Database.GDNOATTR' to ignore attributes, `Database.GDNOTEXT' to
   * ignore the body text, `Database.GDNOKWD' to ignore keywords.  The three can be specified at
   # the same time by bitwise or.
   * @return a document object.  On error, `null' is returned.
   */
  public native Document get_doc(int id, int options);
  /**
   * Retrieve the value of an attribute of a document.
   * @param id the ID number of a registered document.
   * @param name the name of an attribute.
   * @return the value of the attribute or `null' if it does not exist.
   */
  public native String get_doc_attr(int id, String name);
  /**
   * Get the ID of a document specified by URI.
   * @param uri the URI of a registered document.
   * @return the ID of the document.  On error, -1 is returned.
   */
  public native int uri_to_id(String uri);
  /**
   * Get the name.
   * @return the name of the database.
   */
  public native String name();
  /**
   * Get the number of documents.
   * @return the number of documents in the database.
   */
  public native int doc_num();
  /**
   * Get the number of unique words.
   * @return the number of unique words in the database.
   */
  public native int word_num();
  /**
   * Get the size.
   * @return the size of the database.
   */
  public native double size();
  /**
   * Search for documents corresponding a condition.
   * @param cond a condition object.
   * @return a result object.  On error, `null' is returned.
   */
  public native Result search(Condition cond);
  /**
   * Check whether a document object matches the phrase of a search condition object definitely.
   * @param doc a document object.
   * @param cond a search condition object.
   * @return true if the document matches the phrase of the condition object definitely, else it
   * is false.
   */
  public native boolean scan_doc(Document doc, Condition cond);
  /**
   * Set the maximum size of the cache memory.
   * @param size the maximum size of the index cache.  By default, it is 64MB.  If it is not
   * more than 0, the current size is not changed.
   * @param anum the maximum number of cached records for document attributes.  By default, it
   * is 8192.  If it is not more than 0, the current size is not changed.
   * @param tnum the maximum number of cached records for document texts.  By default, it is
   * 1024.  If it is not more than 0, the current size is not changed.
   * @param rnum the maximum number of cached records for occurrence results.  By default, it
   * is 256.  If it is not more than 0, the current size is not changed.
   */
  public native void set_cache_size(double size, int anum, int tnum, int rnum);
  /**
   * Add a pseudo index directory.
   * @param path the path of a pseudo index directory.
   * @return true if success, else it is false.
   */
  public native boolean add_pseudo_index(String path);
  /**
   * Set the maximum number of expansion of wild cards.
   * @param num the maximum number of expansion of wild cards.
   */
  public native void set_wildmax(int num);
  /**
   * Set the callback function to inform of database events.
   * @param informer an arbitrary object with a method named as `inform'.
   */
  public synchronized native void set_informer(DatabaseInformer informer);
}



/* END OF FILE */
