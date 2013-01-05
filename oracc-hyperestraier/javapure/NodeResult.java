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
 * Abstraction of result set from node.
 */
public class NodeResult {
  //----------------------------------------------------------------
  // private fields
  //----------------------------------------------------------------
  private List docs;
  private Map hints;
  //----------------------------------------------------------------
  // constructors
  //----------------------------------------------------------------
  /**
   * Create a node result object.
   * @param docs a list object of result document objects.
   * @param hints a map object of hints.
   */
  NodeResult(List docs, Map hints){
    this.docs = docs;
    this.hints = hints;
  }
  //----------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------
  /**
   * Get the number of documents.
   * @return the number of documents.
   */
  public int doc_num(){
    return docs.size();
  }
  /**
   * Get a result document object
   * @param index the index of a document.
   * @return a result document object or `null' if the index is out of bounds.
   */
  public ResultDocument get_doc(int index){
    try {
      return (ResultDocument)docs.get(index);
    } catch(IndexOutOfBoundsException e){
      return null;
    }
  }
  /**
   * Get the value of hint information.
   * @param key the key of a hint.  "VERSION", "NODE", "HIT", "HINT#n", "DOCNUM", "WORDNUM",
   * "TIME", "TIME#n", "LINK#n", and "VIEW" are provided for keys.
   * @return the value of the hint or `null' if the key does not exist.
   */
  public String hint(String key){
    return (String)hints.get(key);
  }
}



/* END OF FILE */
