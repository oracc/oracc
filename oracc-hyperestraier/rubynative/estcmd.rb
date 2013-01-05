#! /usr/bin/ruby -w
#--
# Ruby binding of Hyper Estraier
#                                                       Copyright (C) 2004-2007 Mikio Hirabayashi
#  This file is part of Hyper Estraier.
#  Hyper Estraier is free software; you can redistribute it and/or modify it under the terms of
#  the GNU Lesser General Public License as published by the Free Software Foundation; either
#  version 2.1 of the License or any later version.  Hyper Estraier is distributed in the hope
#  that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
#  License for more details.
#  You should have received a copy of the GNU Lesser General Public License along with Hyper
#  Estraier; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
#  Boston, MA 02111-1307 USA.


$:.unshift("./src")
require "estraier"
include Estraier


# global constants
PROTVER = "1.0"
SEARCHMAX = 10
SEARCHAUX = 32
SNIPWWIDTH = 480
SNIPHWIDTH = 96
SNIPAWIDTH = 96
VM_ID = 0
VM_URI = 1
VM_ATTR = 2
VM_FULL = 3
VM_SNIP = 4


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
  when "uriid"
    rv = runuriid(args)
  when "inform"
    rv = runinform(args)
  when "optimize"
    rv = runoptimize(args)
  when "merge"
    rv = runmerge(args)
  when "search"
    rv = runsearch(args)
  else
    usage
  end
  return rv
end


# print usage and exit
def usage()
  STDERR.printf("%s: command line utility for the core API of Hyper Estraier\n", $0)
  STDERR.printf("\n")
  STDERR.printf("usage:\n")
  STDERR.printf("  %s put [-cl] [-ws] db [file]\n", $0)
  STDERR.printf("  %s out [-cl] db expr\n", $0)
  STDERR.printf("  %s edit db expr name [value]\n", $0)
  STDERR.printf("  %s get db expr [attr]\n", $0)
  STDERR.printf("  %s uriid db uri\n", $0)
  STDERR.printf("  %s inform db\n", $0)
  STDERR.printf("  %s optimize [-onp] [-ond] db\n", $0)
  STDERR.printf("  %s merge [-cl] db target\n", $0)
  STDERR.printf("  %s search [-vu|-va|-vf|-vs] [-gs|-gf|-ga] [-cd] [-ni] [-sf|-sfr|-sfu|-sfi]" \
                " [-attr expr] [-ord expr] [-max num] [-sk num] [-aux num] [-dis name]" \
                " db [phrase]\n", $0)
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
  dbname = nil
  file = nil
  opts = 0
  i = 1
  while i < args.length
    if !dbname && args[i] =~ /^-/
      if args[i] == "-cl"
        opts |= Database::PDCLEAN
      elsif args[i] == "-ws"
        opts |= Database::PDWEIGHT
      else
        usage
      end
    elsif !dbname
      dbname = args[i]
    elsif !file
      file = args[i]
    else
      usage
    end
    i += 1
  end
  usage if !dbname
  procput(dbname, file, opts)
end


# parse arguments of the out command
def runout(args)
  dbname = nil
  expr = nil
  opts = 0
  i = 1
  while i < args.length
    if !dbname && args[i] =~ /^-/
      if args[i] == "-cl"
        opts |= Database::ODCLEAN
      else
        usage
      end
    elsif !dbname
      dbname = args[i]
    elsif !expr
      expr = args[i]
    else
      usage
    end
    i += 1
  end
  usage if !dbname || !expr
  procout(dbname, expr, opts)
end


# parse arguments of the edit command
def runedit(args)
  dbname = nil
  expr = nil
  name = nil
  value = nil
  i = 1
  while i < args.length
    if !dbname && args[i] =~ /^-/
      usage
    elsif !dbname
      dbname = args[i]
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
  usage if !dbname || !expr || !name
  procedit(dbname, expr, name, value)
end


# parse arguments of the get command
def runget(args)
  dbname = nil
  expr = nil
  attr = nil
  i = 1
  while i < args.length
    if !dbname && args[i] =~ /^-/
      usage
    elsif !dbname
      dbname = args[i]
    elsif !expr
      expr = args[i]
    elsif !attr
      attr = args[i]
    else
      usage
    end
    i += 1
  end
  usage if !dbname || !expr
  procget(dbname, expr, attr)
end


# parse arguments of the uriid command
def runuriid(args)
  dbname = nil
  uri = nil
  i = 1
  while i < args.length
    if !dbname && args[i] =~ /^-/
      usage
    elsif !dbname
      dbname = args[i]
    elsif !uri
      uri = args[i]
    else
      usage
    end
    i += 1
  end
  usage if !dbname || !uri
  procuriid(dbname, uri)
end


# parse arguments of the inform command
def runinform(args)
  dbname = nil
  i = 1
  while i < args.length
    if !dbname && args[i] =~ /^-/
      usage
    elsif !dbname
      dbname = args[i]
    else
      usage
    end
    i += 1
  end
  usage if !dbname
  procinform(dbname)
end


# parse arguments of the optimize command
def runoptimize(args)
  dbname = nil
  opts = 0
  i = 1
  while i < args.length
    if !dbname && args[i] =~ /^-/
      if args[i] == "-onp"
        opts |= Database::OPTNOPURGE
      elsif args[i] == "-ond"
        opts |= Database::OPTNODBOPT
      else
        usage
      end
    elsif !dbname
      dbname = args[i]
    else
      usage
    end
    i += 1
  end
  usage if !dbname
  procoptimize(dbname, opts)
end


# parse arguments of the merge command
def runmerge(args)
  dbname = nil
  tgname = nil
  opts = 0
  i = 1
  while i < args.length
    if !dbname && args[i] =~ /^-/
      if args[i] == "-cl"
        opts |= Database::MGCLEAN
      else
        usage
      end
    elsif !dbname
      dbname = args[i]
    elsif !tgname
      tgname = args[i]
    else
      usage
    end
    i += 1
  end
  usage if !dbname || !tgname
  procmerge(dbname, tgname, opts)
end


# parse arguments of the search command
def runsearch(args)
  dbname = nil
  phrase = nil
  attrs = []
  ord = nil
  max = SEARCHMAX
  skip = 0
  opts = 0
  aux = SEARCHAUX
  dis = nil
  cd = false
  view = VM_ID
  i = 1
  while i < args.length
    if !dbname && args[i] =~ /^-/
      if args[i] == "-vu"
        view = VM_URI
      elsif args[i] == "-va"
        view = VM_ATTR
      elsif args[i] == "-vf"
        view = VM_FULL
      elsif args[i] == "-vs"
        view = VM_SNIP
      elsif args[i] == "-gs"
        opts |= Condition::SURE
      elsif args[i] == "-gf"
        opts |= Condition::FAST
      elsif args[i] == "-ga"
        opts |= Condition::AGITO
      elsif args[i] == "-cd"
        cd = true
      elsif args[i] == "-ni"
        opts |= Condition::NOIDF
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
      else
        usage
      end
    elsif !dbname
      dbname = args[i]
    elsif !phrase
      phrase = args[i]
    else
      phrase += " " + args[i]
    end
    i += 1
  end
  usage if !dbname
  procsearch(dbname, phrase, attrs, ord, max, skip, opts, aux, dis, cd, view)
end


# perform the put command
def procput(dbname, file, opts)
  if file
    begin
      ifp = open(file, "rb")
      draft = ifp.read
    rescue
      printerror(file + ": could not open")
      return 1
    ensure
      ifp.close if ifp
    end
  else
    STDIN.binmode
    draft = STDIN.read
  end
  doc = Document::new(draft)
  db = Database::new
  unless db.open(dbname, Database::DBWRITER | Database::DBCREAT)
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  db.set_informer(Informer::new)
  unless db.put_doc(doc, opts)
    printerror(dbname + ": " + db.err_msg(db.error))
    db.close
    return 1
  end
  unless db.close
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  return 0
end


# perform the out command
def procout(dbname, expr, opts)
  db = Database::new
  unless db.open(dbname, Database::DBWRITER)
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  db.set_informer(Informer::new)
  id = expr.to_i
  if id < 1 && (id = db.uri_to_id(expr)) < 1
    printerror(dbname + ": " + db.err_msg(db.error))
    db.close
    return 1
  end
  unless db.out_doc(id, opts)
    printerror(dbname + ": " + db.err_msg(db.error))
    db.close
    return 1
  end
  unless db.close
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  return 0
end


# perform the edit command
def procedit(dbname, expr, name, value)
  db = Database::new
  unless db.open(dbname, Database::DBWRITER)
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  db.set_informer(Informer::new)
  id = expr.to_i
  if id < 1 && (id = db.uri_to_id(expr)) < 1
    printerror(dbname + ": " + db.err_msg(db.error))
    db.close
    return 1
  end
  unless doc = db.get_doc(id, Database::GDNOTEXT)
    printerror(dbname + ": " + db.err_msg(db.error))
    db.close
    return 1
  end
  doc.add_attr(name, value)
  unless db.edit_doc(doc)
    printerror(dbname + ": " + db.err_msg(db.error))
    db.close
    return 1
  end
  unless db.close
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  return 0
end


# perform the get command
def procget(dbname, expr, attr)
  db = Database::new
  unless db.open(dbname, Database::DBREADER)
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  id = expr.to_i
  if id < 1 && (id = db.uri_to_id(expr)) < 1
    printerror(dbname + ": " + db.err_msg(db.error))
    db.close
    return 1
  end
  if attr
    unless value = db.get_doc_attr(id, attr)
      printerror(dbname + ": " + db.err_msg(db.error))
      db.close
      return 1
    end
    printf("%s\n", value)
  else
    unless doc = db.get_doc(id, 0)
      printerror(dbname + ": " + db.err_msg(db.error))
      db.close
      return 1
    end
    printf("%s", doc.dump_draft)
  end
  unless db.close
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  return 0
end


# perform the uriid command
def procuriid(dbname, uri)
  db = Database::new
  unless db.open(dbname, Database::DBREADER)
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  unless (id = db.uri_to_id(uri)) > 0
    printerror(dbname + ": " + db.err_msg(db.error))
    db.close
    return 1
  end
  printf("%d\n", id)
  unless db.close
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  return 0
end


# perform the inform command
def procinform(dbname)
  db = Database::new
  unless db.open(dbname, Database::DBREADER)
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  printf("number of documents: %d\n", db.doc_num)
  printf("number of words: %d\n", db.word_num)
  printf("file size: %d\n",  db.size.to_i)
  unless db.close
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  return 0
end


# perform the optimize command
def procoptimize(dbname, opts)
  db = Database::new
  unless db.open(dbname, Database::DBWRITER)
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  db.set_informer(Informer::new)
  unless db.optimize(opts)
    printerror(dbname + ": " + db.err_msg(db.error))
    db.close
    return 1
  end
  unless db.close
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  return 0
end


# perform the merge command
def procmerge(dbname, tgname, opts)
  db = Database::new
  unless db.open(dbname, Database::DBWRITER)
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  db.set_informer(Informer::new)
  unless db.merge(tgname, opts)
    printerror(dbname + ": " + db.err_msg(db.error))
    db.close
    return 1
  end
  unless db.close
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  return 0
end


# perform the search command
def procsearch(dbname, phrase, attrs, ord, max, skip, opts, aux, dis, cd, view)
  db = Database::new
  unless db.open(dbname, Database::DBREADER)
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  cond = Condition::new()
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
  stime = Time::now.to_f
  res = db.search(cond)
  etime = Time::now.to_f
  border = "--------[" + Time::now.to_f.to_s.gsub(/\./, "") + "]--------"
  printf("%s\n", border)
  printf("VERSION\t%s\n", PROTVER)
  printf("NODE\tlocal\n")
  printf("HIT\t%d\n", res.hint(""))
  snwords = []
  words = res.hint_words
  for i in 0...words.length
    word = words[i]
    hits = res.hint(word)
    snwords.push(word) if hits > 0
    printf("HINT#%d\t%s\t%d\n", i + 1, word, hits)
  end
  printf("TIME\t%0.3f\n", etime - stime)
  printf("DOCNUM\t%d\n", db.doc_num())
  printf("WORDNUM\t%d\n", db.word_num())
  if view == VM_URI
    printf("VIEW\tURI\n")
  elsif view == VM_ATTR
    printf("VIEW\tATTRIBUTE\n")
  elsif view == VM_FULL
    printf("VIEW\tFULL\n")
  elsif view == VM_SNIP
    printf("VIEW\tSNIPPET\n")
  else
    printf("VIEW\tID\n")
  end
  printf("\n")
  printf("%s\n", border) if view != VM_ATTR && view != VM_FULL && view != VM_SNIP
  dnum = res.doc_num()
  for i in 0...dnum
    id = res.get_doc_id(i)
    if view == VM_URI
      next unless doc = db.get_doc(id, cd ? 0 : Database::GDNOTEXT)
      next if cd && !db.scan_doc(doc, cond)
      printf("%d\t%s\n", id, doc.attr("@uri"))
    elsif view == VM_ATTR
      next unless doc = db.get_doc(id, cd ? 0 : Database::GDNOTEXT)
      next if cd && !db.scan_doc(doc, cond)
      printf("%s\n", border)
      names = doc.attr_names()
      for j in 0...names.length
        printf("%s=%s\n", names[j], doc.attr(names[j]))
      end
      printf("\n")
    elsif view == VM_FULL
      next unless doc = db.get_doc(id, 0)
      next if cd && !db.scan_doc(doc, cond)
      printf("%s\n", border)
      printf("%s", doc.dump_draft())
    elsif view == VM_SNIP
      next unless doc = db.get_doc(id, 0)
      next if cd && !db.scan_doc(doc, cond)
      printf("%s\n", border)
      names = doc.attr_names()
      for j in 0...names.length
        printf("%s=%s\n", names[j], doc.attr(names[j]))
      end
      printf("\n")
      printf("%s", doc.make_snippet(snwords, SNIPWWIDTH, SNIPHWIDTH, SNIPAWIDTH))
    else
      printf("%d\n", id)
    end
  end
  printf("%s:END\n", border)
  unless db.close
    printerror(dbname + ": " + db.err_msg(db.error))
    return 1
  end
  return 0
end


# class for callback function for database events
class Informer
  def inform(message)
    printf("%s: INFO: %s\n", $0, message)
  end
end


# perform the main routine
$0.gsub!(/.*\//, "")
exit(main(ARGV))



# END OF FILE
