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
 * Abstraction of document in result set.
 */
public class ResultDocument {
  //----------------------------------------------------------------
  // private fields
  //----------------------------------------------------------------
  private String uri;
  private Map attrs;
  private String snippet;
  private String keywords;
  //----------------------------------------------------------------
  // constructors
  //----------------------------------------------------------------
  /**
   * Create a result document object.
   * @param uri a string of the URI.
   * @param attrs a map object of attributes.
   * @param snippet a string of snippet.
   * @param keywords a string of keywords.
   */
  ResultDocument(String uri, Map attrs, String snippet, String keywords){
    this.uri = uri;
    this.attrs = attrs;
    this.snippet = snippet;
    this.keywords = keywords;
  }
  //----------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------
  /**
   * Get the URI.
   * @return the URI.
   */
  public String uri(){
    return uri;
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
   * Get the snippet.
   * @return a string of the snippet of the result document object.  There are tab separated
   * values.  Each line is a string to be shown.  Though most lines have only one field, some
   * lines have two fields.  If the second field exists, the first field is to be shown with
   * highlighted, and the second field means its normalized form.
   */
  public String snippet(){
    return snippet;
  }
  /**
   * Get keywords.
   * @return a string of serialized keywords of the result document object.  There are tab
   * separated values.  Keywords and their scores come alternately.
   */
  public String keywords(){
    return keywords;
  }
}



/* END OF FILE */
