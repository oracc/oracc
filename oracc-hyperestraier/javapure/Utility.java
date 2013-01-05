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
import java.util.zip.*;
import java.io.*;
import java.net.*;


/**
 * Tool box of utility methods.
 */
class Utility {
  //----------------------------------------------------------------
  // package static methods
  //----------------------------------------------------------------
  static int shuttle_url(String url, String pxhost, int pxport, int outsec,
                         List reqheads, byte[] reqbody,
                         List resheads, OutputStream resbody){
    int status = -1;
    ShuttleURLThread th = new ShuttleURLThread(url, pxhost, pxport, reqheads, reqbody,
                                               resheads, resbody);
    if(outsec >= 0){
      try {
        th.start();
        th.join(outsec < 1 ? 1 : outsec * 1000);
        if(th.is_end()){
          th.join();
          status = th.get_status();
        } else {
          th.setDaemon(true);
          th.interrupt();
        }
      } catch(InterruptedException e){
        status = -1;
      }
    } else {
      th.run();
      status = th.get_status();
    }
    return status;
  }
  static String read_line(InputStream is) throws IOException {
    byte[] iobuf = new byte[8192];
    int len = 0;
    int c;
    while(len < iobuf.length && (c = is.read()) >= 0){
      if(c == '\n') break;
      if(c != '\r') iobuf[len++] = (byte)c;
    }
    return new String(iobuf, 0, len);
  }
  static byte[] read_all(InputStream is) throws IOException {
    byte[] iobuf = new byte[8192];
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    int len;
    while((len = is.read(iobuf)) >= 0){
      baos.write(iobuf, 0, len);
    }
    return baos.toByteArray();
  }
  static String[] split_lines(String str){
    List list = new ArrayList(256);
    StringBuffer sb = new StringBuffer();
    for(int i = 0; i < str.length(); i++){
      char c = str.charAt(i);
      if(c == '\n'){
        list.add(sb.toString());
        sb = new StringBuffer();
      } else if(c != '\r'){
        sb.append(c);
      }
    }
    list.add(sb.toString());
    String[] lines = new String[list.size()];
    Iterator it = list.iterator();
    for(int i = 0; it.hasNext(); i++){
      lines[i] = (String)it.next();
    }
    return lines;
  }
  static String[] split_fields(String str){
    List list = new ArrayList(16);
    StringBuffer sb = new StringBuffer();
    for(int i = 0; i < str.length(); i++){
      char c = str.charAt(i);
      if(c == '\t'){
        list.add(sb.toString());
        sb = new StringBuffer();
      } else {
        sb.append(c);
      }
    }
    list.add(sb.toString());
    String[] lines = new String[list.size()];
    Iterator it = list.iterator();
    for(int i = 0; it.hasNext(); i++){
      lines[i] = (String)it.next();
    }
    return lines;
  }
  static String base_encode(byte[] data){
    char[] tbl = {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
      'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
      'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
      'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
      '4', '5', '6', '7', '8', '9', '+', '/'
    };
    StringBuffer sb = new StringBuffer();
    for(int i = 0; i < data.length; i += 3){
      switch(data.length - i){
      case 1:
        sb.append(tbl[data[i] >> 2]);
        sb.append(tbl[(data[i] & 3) << 4]);
        sb.append('=');
        sb.append('=');
        break;
      case 2:
        sb.append(tbl[data[i] >> 2]);
        sb.append(tbl[((data[i] & 3) << 4) + (data[i+1] >> 4)]);
        sb.append(tbl[(data[i+1] & 0xf) << 2]);
        sb.append('=');
        break;
      default:
        sb.append(tbl[data[i] >> 2]);
        sb.append(tbl[((data[i] & 3) << 4) + (data[i+1] >> 4)]);
        sb.append(tbl[((data[i+1] & 0xf) << 2) + (data[i+2] >> 6)]);
        sb.append(tbl[data[i+2] & 0x3f]);
        break;
      }
    }
    return sb.toString();
  }
  static byte[] deflate(byte[] buf){
    try {
      ByteArrayOutputStream baos = new ByteArrayOutputStream(buf.length);
      DeflaterOutputStream dos = new DeflaterOutputStream(baos);
      dos.write(buf, 0, buf.length);
      dos.close();
      return baos.toByteArray();
    } catch(IOException e){
      return null;
    }
  }
  static byte[] inflate(byte[] buf){
    try {
      ByteArrayInputStream bais = new ByteArrayInputStream(buf);
      InflaterInputStream iis = new InflaterInputStream(bais);
      ByteArrayOutputStream baos = new ByteArrayOutputStream(buf.length * 4);
      byte[] ibuf = new byte[8192];
      int len;
      while((len = iis.read(ibuf, 0, ibuf.length)) != -1){
        baos.write(ibuf, 0, len);
      }
      iis.close();
      return baos.toByteArray();
    } catch(IOException e){
      return null;
    }
  }
  /* dummy method: hide the constructor */
  private Utility(){}
  //----------------------------------------------------------------
  // private inner classes
  //----------------------------------------------------------------
  private static class ShuttleURLThread extends Thread {
    private String url;
    private String pxhost;
    private int pxport;
    private List reqheads;
    private byte[] reqbody;
    private List resheads;
    private OutputStream resbody;
    private int status;
    private boolean end;
    ShuttleURLThread(String url, String pxhost, int pxport,
                     List reqheads, byte[] reqbody,
                     List resheads, OutputStream resbody){
      this.url = url;
      this.pxhost = pxhost;
      this.pxport = pxport;
      this.reqheads = reqheads;
      this.reqbody = reqbody;
      this.resheads = resheads;
      this.resbody = resbody;
      this.status = -1;
      this.end = false;
    }
    int get_status(){
      return status;
    }
    boolean is_end(){
      return end;
    }
    public void run(){
      try {
        URL purl = new URL(url);
        Socket sock = null;
        try {
          if(pxhost != null){
            sock = new Socket(pxhost, pxport);
          } else {
            int port = purl.getPort();
            if(port < 1) port = 80;
            sock = new Socket(purl.getHost(), port);
          }
          if(interrupted()) return;
          OutputStream os = sock.getOutputStream();
          StringBuffer sb = new StringBuffer();
          String method = "GET";
          if(reqbody != null) method = "POST";
          if(pxhost != null){
            sb.append(method + " " + url + " HTTP/1.0\r\n");
          } else {
            sb.append(method + " " + purl.getPath() + " HTTP/1.0\r\n");
          }
          sb.append("Host: " + purl.getHost() + ":" + purl.getPort() + "\r\n");
          sb.append("User-Agent: HyperEstraierForJava/1.0.0\r\n");
          if(reqheads != null){
            Iterator it = reqheads.iterator();
            while(it.hasNext()){
              String line = (String)it.next();
              sb.append(line);
              sb.append("\r\n");
            }
          }
          sb.append("Accept-Encoding: deflate\r\n");
          if(reqbody != null)
            sb.append("Content-Length: " + reqbody.length + "\r\n");
          sb.append("\r\n");
          os.write(sb.toString().getBytes("ISO-8859-1"));
          if(reqbody != null) os.write(reqbody);
          os.flush();
          if(interrupted()) return;
          InputStream is = sock.getInputStream();
          String tline = read_line(is);
          int tidx = tline.indexOf(' ');
          if(tidx == -1) return;
          tline = tline.substring(tidx + 1, tline.length());
          tidx = tline.indexOf(' ');
          if(tidx != -1) tline = tline.substring(0, tidx);
          status = Integer.parseInt(tline);
          if(resheads != null) resheads.add(tline);
          if(interrupted()) return;
          boolean defl = false;
          for(;;){
            String line = read_line(is);
            if(line.length() < 1) break;
            if(resheads != null) resheads.add(line);
            String lower = line.toLowerCase();
            if(lower.startsWith("content-encoding:") && lower.indexOf("deflate") != -1)
              defl = true;
            if(interrupted()) return;
          }
          if(defl && resbody != null){
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            byte[] iobuf = new byte[8192];
            int rsiz;
            while((rsiz = is.read(iobuf)) >= 0){
              baos.write(iobuf, 0, rsiz);
              if(interrupted()) return;
            }
            byte[] rbuf = inflate(baos.toByteArray());
            if(rbuf != null) resbody.write(rbuf, 0, rbuf.length);
          } else {
            byte[] iobuf = new byte[8192];
            int rsiz;
            while((rsiz = is.read(iobuf)) >= 0){
              if(resbody != null) resbody.write(iobuf, 0, rsiz);
              if(interrupted()) return;
            }
          }
          end = true;
        } finally {
          if(sock != null) sock.close();
        }
      } catch(Exception e){
        status = -1;
      }
    }
  }
}



/* END OF FILE */
