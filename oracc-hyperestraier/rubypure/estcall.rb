#! /usr/bin/ruby -w
#--
# Ruby interface of Hyper Estraier
#                                                       Copyright (C) 2004-2007 Mikio Hirabayashi
#                                                                            All rights reserved.
#  This file is part of Hyper Estraier.
#  Redistribution and use in source and binary forms, with or without modification, are
#  permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright notice, this list of
#      conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright notice, this list of
#      conditions and the following disclaimer in the documentation and/or other materials
#      provided with the distribution.
#    * Neither the name of Mikio Hirabayashi nor the names of its contributors may be used to
#      endorse or promote products derived from this software without specific prior written
#      permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
#  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
#  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
#  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
#  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
#  OF THE POSSIBILITY OF SUCH DAMAGE.


require "estraierpure"

include EstraierPure


# global constants
SEARCHMAX = 10
SEARCHAUX = 10

# global variables
$pxhost = nil
$pxport = 0
$timeout = -1
$authname = nil
$authpass = nil
$condopts = 0


# main routine
def main(args)
  usage if args.length < 1
  rv = 0
  case args[0]
  when "put"
    rv = runput(args)
  when "out"
    rv = runout(args)
  when "edit"
    rv = runedit(args)
  when "get"
    rv = runget(args)
  when "etch"
    rv = runetch(args)
  when "uriid"
    rv = runuriid(args)
  when "inform"
    rv = runinform(args)
  when "sync"
    rv = runsync(args)
  when "optimize"
    rv = runoptimize(args)
  when "search"
    rv = runsearch(args)
  when "setuser"
    rv = runsetuser(args)
  when "setlink"
    rv = runsetlink(args)
  else
    usage
  end
  return rv
end


# print usage and exit
def usage()
  STDERR.printf("%s: command line utility for the node API of Hyper Estraier\n", $0)
  STDERR.printf("\n")
  STDERR.printf("usage:\n")
  STDERR.printf("  %s put [-proxy host port] [-tout num] [-auth user pass] nurl [file]\n", $0)
  STDERR.printf("  %s out [-proxy host port] [-tout num] [-auth user pass] nurl expr\n", $0)
  STDERR.printf("  %s edit [-proxy host port] [-tout num] [-auth user pass]" \
                " nurl expr name [value]\n", $0)
  STDERR.printf("  %s get [-proxy host port] [-tout num] [-auth user pass]" \
                " nurl expr [attr]\n", $0)
  STDERR.printf("  %s etch [-proxy host port] [-tout num] [-auth user pass] nurl expr\n", $0)
  STDERR.printf("  %s uriid [-proxy host port] [-tout num] [-auth user pass] nurl uri\n", $0)
  STDERR.printf("  %s inform [-proxy host port] [-tout num] [-auth user pass] nurl\n", $0)
  STDERR.printf("  %s sync [-proxy host port] [-tout num] [-auth user pass] nurl\n", $0)
  STDERR.printf("  %s optimize [-proxy host port] [-tout num] [-auth user pass] nurl\n", $0)
  STDERR.printf("  %s search [-proxy host port] [-tout num] [-auth user pass]" \
                " [-sf|-sfr|-sfu|-sfi] [-attr expr] [-ord expr] [-max num] [-sk num]" \
                " [-aux num] [-dis name] [-dpt num] [-mask num] nurl [phrase]\n", $0)
  STDERR.printf("  %s setuser [-proxy host port] [-tout num] [-auth user pass]" \
                " nurl name mode\n", $0)
  STDERR.printf("  %s setlink [-proxy host port] [-tout num] [-auth user pass]" \
                " nurl url label credit\n", $0)
  STDERR.printf("\n")
  exit(1)
end


# print error string and flush the buffer */
def printerror(msg)
  STDERR.printf("%s: ERROR: %s\n", $0, msg)
  STDERR.flush
end


# parse arguments of the put command
def runput(args)
  nurl = nil
  file = nil
  i = 1
  while i < args.length
    if !nurl && args[i] =~ /^-/
      if args[i] == "-proxy"
        usage if (i += 1) >= args.length
        $pxhost = args[i]
        usage if (i += 1) >= args.length
        $pxport = args[i].to_i
      elsif args[i] == "-tout"
        usage if (i += 1) >= args.length
        $timeout = args[i].to_i
      elsif args[i] == "-auth"
        usage if (i += 1) >= args.length
        $authname = args[i]
        usage if (i += 1) >= args.length
        $authpass = args[i]
      else
        usage
      end
    elsif !nurl
      nurl = args[i]
    elsif !file
      file = args[i]
    else
      usage
    end
    i += 1
  end
  usage if !nurl
  procput(nurl, file)
end


# parse arguments of the out command
def runout(args)
  nurl = nil
  expr = nil
  i = 1
  while i < args.length
    if !nurl && args[i] =~ /^-/
      if args[i] == "-proxy"
        usage if (i += 1) >= args.length
        $pxhost = args[i]
        usage if (i += 1) >= args.length
        $pxport = args[i].to_i
      elsif args[i] == "-tout"
        usage if (i += 1) >= args.length
        $timeout = args[i].to_i
      elsif args[i] == "-auth"
        usage if (i += 1) >= args.length
        $authname = args[i]
        usage if (i += 1) >= args.length
        $authpass = args[i]
      else
        usage
      end
    elsif !nurl
      nurl = args[i]
    elsif !expr
      expr = args[i]
    else
      usage
    end
    i += 1
  end
  usage if !nurl || !expr
  procout(nurl, expr)
end


# parse arguments of the edit command
def runedit(args)
  nurl = nil
  expr = nil
  name = nil
  value = nil
  i = 1
  while i < args.length
    if !nurl && args[i] =~ /^-/
      if args[i] == "-proxy"
        usage if (i += 1) >= args.length
        $pxhost = args[i]
        usage if (i += 1) >= args.length
        $pxport = args[i].to_i
      elsif args[i] == "-tout"
        usage if (i += 1) >= args.length
        $timeout = args[i].to_i
      elsif args[i] == "-auth"
        usage if (i += 1) >= args.length
        $authname = args[i]
        usage if (i += 1) >= args.length
        $authpass = args[i]
      else
        usage
      end
    elsif !nurl
      nurl = args[i]
    elsif !expr
      expr = args[i]
    elsif !name
      name = args[i]
    elsif !value
      value = args[i]
    else
      usage
    end
    i += 1
  end
  usage if !nurl || !expr || !name
  procedit(nurl, expr, name, value)
end


# parse arguments of the get command
def runget(args)
  nurl = nil
  expr = nil
  attr = nil
  i = 1
  while i < args.length
    if !nurl && args[i] =~ /^-/
      if args[i] == "-proxy"
        usage if (i += 1) >= args.length
        $pxhost = args[i]
        usage if (i += 1) >= args.length
        $pxport = args[i].to_i
      elsif args[i] == "-tout"
        usage if (i += 1) >= args.length
        $timeout = args[i].to_i
      elsif args[i] == "-auth"
        usage if (i += 1) >= args.length
        $authname = args[i]
        usage if (i += 1) >= args.length
        $authpass = args[i]
      else
        usage
      end
    elsif !nurl
      nurl = args[i]
    elsif !expr
      expr = args[i]
    elsif !attr
      attr = args[i]
    else
      usage
    end
    i += 1
  end
  usage if !nurl || !expr
  procget(nurl, expr, attr)
end


# parse arguments of the etch command
def runetch(args)
  nurl = nil
  expr = nil
  i = 1
  while i < args.length
    if !nurl && args[i] =~ /^-/
      if args[i] == "-proxy"
        usage if (i += 1) >= args.length
        $pxhost = args[i]
        usage if (i += 1) >= args.length
        $pxport = args[i].to_i
      elsif args[i] == "-tout"
        usage if (i += 1) >= args.length
        $timeout = args[i].to_i
      elsif args[i] == "-auth"
        usage if (i += 1) >= args.length
        $authname = args[i]
        usage if (i += 1) >= args.length
        $authpass = args[i]
      else
        usage
      end
    elsif !nurl
      nurl = args[i]
    elsif !expr
      expr = args[i]
    else
      usage
    end
    i += 1
  end
  usage if !nurl || !expr
  procetch(nurl, expr)
end


# parse arguments of the uriid command
def runuriid(args)
  nurl = nil
  uri = nil
  i = 1
  while i < args.length
    if !nurl && args[i] =~ /^-/
      if args[i] == "-proxy"
        usage if (i += 1) >= args.length
        $pxhost = args[i]
        usage if (i += 1) >= args.length
        $pxport = args[i].to_i
      elsif args[i] == "-tout"
        usage if (i += 1) >= args.length
        $timeout = args[i].to_i
      elsif args[i] == "-auth"
        usage if (i += 1) >= args.length
        $authname = args[i]
        usage if (i += 1) >= args.length
        $authpass = args[i]
      else
        usage
      end
    elsif !nurl
      nurl = args[i]
    elsif !uri
      uri = args[i]
    else
      usage
    end
    i += 1
  end
  usage if !nurl || !uri
  procuriid(nurl, uri)
end


# parse arguments of the inform command
def runinform(args)
  nurl = nil
  i = 1
  while i < args.length
    if !nurl && args[i] =~ /^-/
      if args[i] == "-proxy"
        usage if (i += 1) >= args.length
        $pxhost = args[i]
        usage if (i += 1) >= args.length
        $pxport = args[i].to_i
      elsif args[i] == "-tout"
        usage if (i += 1) >= args.length
        $timeout = args[i].to_i
      elsif args[i] == "-auth"
        usage if (i += 1) >= args.length
        $authname = args[i]
        usage if (i += 1) >= args.length
        $authpass = args[i]
      else
        usage
      end
    elsif !nurl
      nurl = args[i]
    else
      usage
    end
    i += 1
  end
  usage if !nurl
  procinform(nurl)
end


# parse arguments of the sync command
def runsync(args)
  nurl = nil
  i = 1
  while i < args.length
    if !nurl && args[i] =~ /^-/
      if args[i] == "-proxy"
        usage if (i += 1) >= args.length
        $pxhost = args[i]
        usage if (i += 1) >= args.length
        $pxport = args[i].to_i
      elsif args[i] == "-tout"
        usage if (i += 1) >= args.length
        $timeout = args[i].to_i
      elsif args[i] == "-auth"
        usage if (i += 1) >= args.length
        $authname = args[i]
        usage if (i += 1) >= args.length
        $authpass = args[i]
      else
        usage
      end
    elsif !nurl
      nurl = args[i]
    else
      usage
    end
    i += 1
  end
  usage if !nurl
  procsync(nurl)
end


# parse arguments of the optimize command
def runoptimize(args)
  nurl = nil
  i = 1
  while i < args.length
    if !nurl && args[i] =~ /^-/
      if args[i] == "-proxy"
        usage if (i += 1) >= args.length
        $pxhost = args[i]
        usage if (i += 1) >= args.length
        $pxport = args[i].to_i
      elsif args[i] == "-tout"
        usage if (i += 1) >= args.length
        $timeout = args[i].to_i
      elsif args[i] == "-auth"
        usage if (i += 1) >= args.length
        $authname = args[i]
        usage if (i += 1) >= args.length
        $authpass = args[i]
      else
        usage
      end
    elsif !nurl
      nurl = args[i]
    else
      usage
    end
    i += 1
  end
  usage if !nurl
  procoptimize(nurl)
end


# parse arguments of the search command
def runsearch(args)
  nurl = nil
  phrase = nil
  attrs = []
  ord = nil
  max = SEARCHMAX
  skip = 0
  opts = 0
  aux = SEARCHAUX
  dis = nil
  depth = 0
  mask = 0
  i = 1
  while i < args.length
    if !nurl && args[i] =~ /^-/
      if args[i] == "-proxy"
        usage if (i += 1) >= args.length
        $pxhost = args[i]
        usage if (i += 1) >= args.length
        $pxport = args[i].to_i
      elsif args[i] == "-tout"
        usage if (i += 1) >= args.length
        $timeout = args[i].to_i
      elsif args[i] == "-auth"
        usage if (i += 1) >= args.length
        $authname = args[i]
        usage if (i += 1) >= args.length
        $authpass = args[i]
      elsif args[i] == "-sf"
        opts |= Condition::SIMPLE
      elsif args[i] == "-sfr"
        opts |= Condition::ROUGH
      elsif args[i] == "-sfu"
        opts |= Condition::UNION
      elsif args[i] == "-sfi"
        opts |= Condition::ISECT
      elsif args[i] == "-attr"
        usage if (i += 1) >= args.length
        attrs.push(args[i])
      elsif args[i] == "-ord"
        usage if (i += 1) >= args.length
        ord = args[i]
      elsif args[i] == "-max"
        usage if (i += 1) >= args.length
        max = args[i].to_i
      elsif args[i] == "-sk"
        usage if (i += 1) >= args.length
        skip = args[i].to_i
      elsif args[i] == "-aux"
        usage if (i += 1) >= args.length
        aux = args[i].to_i
      elsif args[i] == "-dis"
        usage if (i += 1) >= args.length
        dis = args[i]
      elsif args[i] == "-dpt"
        usage if (i += 1) >= args.length
        depth = args[i].to_i
      elsif args[i] == "-mask"
        usage if (i += 1) >= args.length
        mask = args[i].to_i
      else
        usage
      end
    elsif !nurl
      nurl = args[i]
    elsif !phrase
      phrase = args[i]
    else
      phrase += " " + args[i]
    end
    i += 1
  end
  usage if !nurl || depth < 0
  procsearch(nurl, phrase, attrs, ord, max, skip, opts, aux, dis, depth, mask)
end


# parse arguments of the setuser command
def runsetuser(args)
  nurl = nil
  name = nil
  mode = nil
  i = 1
  while i < args.length
    if !nurl && args[i] =~ /^-/
      if args[i] == "-proxy"
        usage if (i += 1) >= args.length
        $pxhost = args[i]
        usage if (i += 1) >= args.length
        $pxport = args[i].to_i
      elsif args[i] == "-tout"
        usage if (i += 1) >= args.length
        $timeout = args[i].to_i
      elsif args[i] == "-auth"
        usage if (i += 1) >= args.length
        $authname = args[i]
        usage if (i += 1) >= args.length
        $authpass = args[i]
      else
        usage
      end
    elsif !nurl
      nurl = args[i]
    elsif !name
      name = args[i]
    elsif !mode
      mode = args[i].to_i
    else
      usage
    end
    i += 1
  end
  usage if !nurl || !name || !mode || mode < 0
  procsetuser(nurl, name, mode)
end


# parse arguments of the setlink command
def runsetlink(args)
  nurl = nil
  url = nil
  label = nil
  credit = nil
  i = 1
  while i < args.length
    if !nurl && args[i] =~ /^-/
      if args[i] == "-proxy"
        usage if (i += 1) >= args.length
        $pxhost = args[i]
        usage if (i += 1) >= args.length
        $pxport = args[i].to_i
      elsif args[i] == "-tout"
        usage if (i += 1) >= args.length
        $timeout = args[i].to_i
      elsif args[i] == "-auth"
        usage if (i += 1) >= args.length
        $authname = args[i]
        usage if (i += 1) >= args.length
        $authpass = args[i]
      else
        usage
      end
    elsif !nurl
      nurl = args[i]
    elsif !url
      url = args[i]
    elsif !label
      label = args[i]
    elsif !credit
      credit = args[i].to_i
    else
      usage
    end
    i += 1
  end
  usage if !nurl || !url || !label || !credit
  procsetlink(nurl, url, label, credit)
end


# perform the put command
def procput(nurl, file)
  if file
    begin
      ifp = open(file, "rb")
      draft = ifp.read
    ensure
      ifp.close if ifp
    end
  else
    STDIN.binmode
    draft = STDIN.read
  end
  doc = Document::new(draft)
  node = Node::new
  node.set_url(nurl)
  node.set_proxy($pxhost, $pxport) if $pxhost
  node.set_timeout($timeout) if $timeout > 0
  node.set_auth($authname, $authpass) if $authname
  if !node.put_doc(doc)
    printerror("failed: " + node.status.to_s)
    return 1
  end
  return 0
end


# perform the out command
def procout(nurl, expr)
  node = Node::new
  node.set_url(nurl)
  node.set_proxy($pxhost, $pxport) if $pxhost
  node.set_timeout($timeout) if $timeout > 0
  node.set_auth($authname, $authpass) if $authname
  id = expr.to_i
  if id > 0
    if !node.out_doc(id)
      printerror("failed: " + node.status.to_s)
      return 1
    end
  else
    if !node.out_doc_by_uri(expr)
      printerror("failed: " + node.status.to_s)
      return 1
    end
  end
  return 0
end


# perform the edit command
def procedit(nurl, expr, name, value)
  node = Node::new
  node.set_url(nurl)
  node.set_proxy($pxhost, $pxport) if $pxhost
  node.set_timeout($timeout) if $timeout > 0
  node.set_auth($authname, $authpass) if $authname
  id = expr.to_i
  doc = id > 0 ? node.get_doc(id) : node.get_doc_by_uri(expr)
  if !doc
    printerror("failed: " + node.status.to_s)
    return 1
  end
  doc.add_attr(name, value)
  if !node.edit_doc(doc)
    printerror("failed: " + node.status.to_s)
    return 1
  end
  return 0
end


# perform the get command
def procget(nurl, expr, attr)
  node = Node::new
  node.set_url(nurl)
  node.set_proxy($pxhost, $pxport) if $pxhost
  node.set_timeout($timeout) if $timeout > 0
  node.set_auth($authname, $authpass) if $authname
  id = expr.to_i
  if attr
    value = id > 0 ? node.get_doc_attr(id, attr) : node.get_doc_attr_by_uri(expr, attr)
    if !value
      printerror("failed: " + node.status.to_s)
      return 1
    end
    printf("%s\n", value)
  else
    doc = id > 0 ? node.get_doc(id) : node.get_doc_by_uri(expr)
    if !doc
      printerror("failed: " + node.status.to_s)
      return 1
    end
    printf("%s", doc.dump_draft)
  end
  return 0
end


# perform the etch command
def procetch(nurl, expr)
  node = Node::new
  node.set_url(nurl)
  node.set_proxy($pxhost, $pxport) if $pxhost
  node.set_timeout($timeout) if $timeout > 0
  node.set_auth($authname, $authpass) if $authname
  id = expr.to_i
  kwords = id > 0 ? node.etch_doc(id) : node.etch_doc_by_uri(expr)
  if !kwords
    printerror("failed: " + node.status.to_s)
    return 1
  end
  kwords.each do |key, value|
    printf("%s\t%s\n", key, value)
  end
  return 0
end


# perform the uriid command
def procuriid(nurl, uri)
  node = Node::new
  node.set_url(nurl)
  node.set_proxy($pxhost, $pxport) if $pxhost
  node.set_timeout($timeout) if $timeout > 0
  node.set_auth($authname, $authpass) if $authname
  value = node.uri_to_id(uri)
  if !value
    printerror("failed: " + node.status.to_s)
    return 1
  end
  printf("%s\n", value)
  return 0
end


# perform the inform command
def procinform(nurl)
  node = Node::new
  node.set_url(nurl)
  node.set_proxy($pxhost, $pxport) if $pxhost
  node.set_timeout($timeout) if $timeout > 0
  node.set_auth($authname, $authpass) if $authname
  name = node.name
  label = node.label
  dnum = node.doc_num
  wnum = node.word_num
  size = node.size
  ratio = node.cache_usage
  if !name || !label || dnum < 0 || wnum < 0 || size < 0.0 || ratio < 0.0
    printerror("failed: " + node.status.to_s)
    return 1
  end
  printf("%s\t%s\t%d\t%d\t%.0f\t%.6f\n", name, label, dnum, wnum, size, ratio)
  return 0
end


# perform the sync command
def procsync(nurl)
  node = Node::new
  node.set_url(nurl)
  node.set_proxy($pxhost, $pxport) if $pxhost
  node.set_timeout($timeout) if $timeout > 0
  node.set_auth($authname, $authpass) if $authname
  if !node.sync
    printerror("failed: " + node.status.to_s)
    return 1
  end
  return 0
end


# perform the optimize command
def procoptimize(nurl)
  node = Node::new
  node.set_url(nurl)
  node.set_proxy($pxhost, $pxport) if $pxhost
  node.set_timeout($timeout) if $timeout > 0
  node.set_auth($authname, $authpass) if $authname
  if !node.optimize
    printerror("failed: " + node.status.to_s)
    return 1
  end
  return 0
end


# perform the search command
def procsearch(nurl, phrase, attrs, ord, max, skip, opts, aux, dis, depth, mask)
  node = Node::new
  node.set_url(nurl)
  node.set_proxy($pxhost, $pxport) if $pxhost
  node.set_timeout($timeout) if $timeout > 0
  node.set_auth($authname, $authpass) if $authname
  cond = Condition::new
  cond.set_phrase(phrase) if phrase
  for i in 0...attrs.length
    cond.add_attr(attrs[i])
  end
  cond.set_order(ord) if ord
  cond.set_max(max) if max >= 0
  cond.set_skip(skip) if skip >= 0
  cond.set_options(opts)
  cond.set_auxiliary(aux)
  cond.set_distinct(dis) if dis
  cond.set_mask(mask)
  nres = node.search(cond, depth)
  if !nres
    printerror("failed: " + node.status.to_s)
    return 1
  end
  border = "--------[" + Time::now.to_f.to_s.gsub(/\./, "") + "]--------"
  printf("%s\n", border)
  value = nres.hint("VERSION")
  printf("VERSION\t%s\n", value) if value
  value = nres.hint("NODE")
  printf("NODE\t%s\n", value) if value
  value = nres.hint("HIT")
  printf("HIT\t%s\n", value) if value
  for i in 1...1024
    key = "HINT#" + i.to_s
    value = nres.hint(key)
    if value
      printf("%s\t%s\n", key, value)
    else
      break
    end
  end
  value = nres.hint("DOCNUM")
  printf("DOCNUM\t%s\n", value) if value
  value = nres.hint("WORDNUM")
  printf("WORDNUM\t%s\n", value) if value
  value = nres.hint("TIME")
  printf("TIME\t%s\n", value) if value
  value = nres.hint("TIME#i")
  printf("TIME#i\t%s\n", value) if value
  for i in 0...1024
    key = "TIME#" + i.to_s
    value = nres.hint(key)
    if value
      printf("%s\t%s\n", key, value)
    else
      break
    end
  end
  for i in 0...1024
    key = "LINK#" + i.to_s
    value = nres.hint(key)
    if value
      printf("%s\t%s\n", key, value)
    else
      break
    end
  end
  value = nres.hint("VIEW")
  printf("VIEW\t%s\n", value) if value
  printf("\n")
  for i in 0...nres.doc_num
    printf("%s\n", border)
    rdoc = nres.get_doc(i)
    names = rdoc.attr_names
    for j in 0...names.length
      printf("%s=%s\n", names[j], rdoc.attr(names[j]))
    end
    keywords = rdoc.keywords
    printf("%%VECTOR\t%s\n", keywords) if keywords.length > 0
    printf("\n")
    printf("%s", rdoc.snippet)
  end
  printf("%s:END\n", border)
  return 0
end


# perform the setuser command
def procsetuser(nurl, name, mode)
  node = Node::new
  node.set_url(nurl)
  node.set_proxy($pxhost, $pxport) if $pxhost
  node.set_timeout($timeout) if $timeout > 0
  node.set_auth($authname, $authpass) if $authname
  if !node.set_user(name, mode)
    printerror("failed: " + node.status.to_s)
    return 1
  end
  return 0
end


# perform the setlink command
def procsetlink(nurl, url, label, credit)
  node = Node::new
  node.set_url(nurl)
  node.set_proxy($pxhost, $pxport) if $pxhost
  node.set_timeout($timeout) if $timeout > 0
  node.set_auth($authname, $authpass) if $authname
  if !node.set_link(url, label, credit)
    printerror("failed: " + node.status.to_s)
    return 1
  end
  return 0
end


# perform the main routine
exit(main(ARGV))



# END OF FILE
