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
 * Tool box of utility methods.
 */
final class Utility {
  /**
   * Initialize the runtime environment.
   */
  static void init(){
    System.loadLibrary("jestraier");
  }
  /**
   * Read whole data from a stream.
   * @param is an input stream object.
   * @return an array of while data.
   */
  static byte[] read_all(InputStream is) throws IOException {
    byte[] iobuf = new byte[8192];
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    int len;
    while((len = is.read(iobuf)) >= 0){
      baos.write(iobuf, 0, len);
    }
    return baos.toByteArray();
  }
}



/* END OF FILE */
