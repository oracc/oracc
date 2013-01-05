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
#++
#:include:overview



#
# Module for the namespace of Hyper Estraier
#
module Estraier
  #----------------------------------------------------------------
  #++ Abstraction of document.
  #----------------------------------------------------------------
  class Document
    #--------------------------------
    # public methods
    #--------------------------------
    public
    # Add an attribute.
    # `name' specifies the name of an attribute.
    # `value' specifies the value of the attribute.  If it is `nil', the attribute is removed.
    # The return value is always `nil'.
    def add_attr(name, value)
      # native code ...
    end
    # Add a sentence of text.
    # `text' specifies a sentence of text.
    # The return value is always `nil'.
    def add_text(text)
      # native code ...
    end
    # Add a hidden sentence.
    # `text' specifies a hidden sentence.
    # The return value is always `nil'.
    def add_hidden_text(text)
      # native code ...
    end
    # Attach keywords.
    # `kwords' specifies a hash object of keywords.  Keys of the hash should be keywords of the
    # document and values should be their scores in decimal string.
    # The return value is always `nil'.
    def set_keywords(kwords)
      # native code ...
    end
    # Set the substitute score.
    # `score' specifies the substitute score.  It it is negative, the substitute score setting is
    # nullified.
    # The return value is always `nil'.
    def set_score(score)
      # native code ...
    end
    # Get the ID number.
    # The return value is the ID number of the document object.  If the object has never been
    # registered, -1 is returned.
    def id()
      # native code ...
    end
    # Get an array of attribute names of a document object.
    # The return value is an array object of attribute names.
    def attr_names()
      # native code ...
    end
    # Get the value of an attribute.
    # `name' specifies the name of an attribute.
    # The return value is the value of the attribute or `nil' if it does not exist.
    def attr(name)
      # native code ...
    end
    # Get an array of sentences of the text.
    # The return value is an array object of sentences of the text.
    def texts()
      # native code ...
    end
    # Concatenate sentences of the text of a document object.
    # The return value is concatenated sentences.
    def cat_texts()
      # native code ...
    end
    # Get attached keywords.
    # The return value is a hash object of keywords and their scores in decimal string.  If no
    # keyword is attached, `nil' is returned.
    def keywords()
      # native code ...
    end
    # Get the substitute score.
    # The return value is the substitute score or -1 if it is not set.
    def score()
      # native code ...
    end
    # Dump draft data of a document object.
    # The return value is draft data.
    def dump_draft()
      # native code ...
    end
    # Make a snippet of the body text.
    # `words' specifies an array object of words to be highlight.
    # `wwidth' specifies whole width of the result.
    # `hwidth' specifies width of strings picked up from the beginning of the text.
    # `awidth' width of strings picked up around each highlighted word.
    # The return value is a snippet string of the body text.  There are tab separated values.
    # Each line is a string to be shown.  Though most lines have only one field, some lines have
    # two fields.  If the second field exists, the first field is to be shown with highlighted,
    # and the second field means its normalized form.
    def make_snippet(words, wwidth, hwidth, awidth)
      # native code ...
    end
    #--------------------------------
    # private methods
    #--------------------------------
    private
    # Create a document object.
    # `draft' specifies a string of draft data.
    def initialize(draft = "")
      # native code ...
    end
  end
  #----------------------------------------------------------------
  #++ Abstraction of search condition.
  #----------------------------------------------------------------
  class Condition
    #--------------------------------
    # public constants
    #--------------------------------
    public
    # option: check every N-gram key
    SURE = 1 << 0
    # option: check N-gram keys skipping by one
    USUAL = 1 << 1
    # option: check N-gram keys skipping by two
    FAST = 1 << 2
    # option: check N-gram keys skipping by three
    AGITO = 1 << 3
    # option: without TF-IDF tuning
    NOIDF = 1 << 4
    # option: with the simplified phrase
    SIMPLE = 1 << 10
    # option: with the rough phrase
    ROUGH = 1 << 11
    # option: with the union phrase
    UNION = 1 << 15
    # option: with the intersection phrase
    ISECT = 1 << 16
    # eclipse tuning: consider URL
    ECLSIMURL = 10.0
    # eclipse tuning: on server basis
    ECLSERV = 100.0
    # eclipse tuning: on directory basis
    ECLDIR = 101.0
    # eclipse tuning: on file basis
    ECLFILE = 102.0
    #--------------------------------
    # public methods
    #--------------------------------
    public
    # Set the search phrase.
    # `phrase' specifies a search phrase.
    # The return value is always `nil'.
    def set_phrase(phrase)
      # native code ...
    end
    # Add an expression for an attribute.
    # `expr' specifies an expression for an attribute.
    # The return value is always `nil'.
    def add_attr(expr)
      # native code ...
    end
    # Set the order of a condition object.
    # `expr' specifies an expression for the order.  By default, the order is by score descending.
    # The return value is always `nil'.
    def set_order(expr)
      # native code ...
    end
    # Set the maximum number of retrieval.
    # `max' specifies the maximum number of retrieval.  By default, the number of retrieval is
    # not limited.
    # The return value is always `nil'.
    def set_max(max)
      # native code ...
    end
    # Set the number of skipped documents.
    # `skip' specifies the number of documents to be skipped in the search result.
    # The return value is always `nil'.
    def set_skip(skip)
      # native code ...
    end
    # Set options of retrieval.
    # `options' specifies options: `Condition::SURE' specifies that it checks every N-gram
    # key, `Condition::USU', which is the default, specifies that it checks N-gram keys
    # with skipping one key, `Condition::FAST' skips two keys, `Condition::AGITO'
    # skips three keys, `Condition::NOIDF' specifies not to perform TF-IDF tuning,
    # `Condition::SIMPLE' specifies to use simplified phrase, `Condition::ROUGH' specifies to use
    # rough phrase, `Condition::UNION' specifies to use union phrase, `Condition::ISECT' specifies
    # to use intersection phrase.  Each option can be specified at the same time by bitwise or.
    # If keys are skipped, though search speed is improved, the relevance ratio grows less.
    # The return value is always `nil'.
    def set_options(options)
      # native code ...
    end
    # Set permission to adopt result of the auxiliary index.
    # `min' specifies the minimum hits to adopt result of the auxiliary index.  If it is not more
    # than 0, the auxiliary index is not used.  By default, it is 32.
    # The return value is always `nil'.
    def set_auxiliary(min)
      # native code ...
    end
    # Set the lower limit of similarity eclipse.
    # `limit' specifies the lower limit of similarity for documents to be eclipsed.  Similarity is
    # between 0.0 and 1.0.  If the limit is added by `Condition::ECLSIMURL', similarity is
    # weighted by URL.  If the limit is `Condition::ECLSERV', similarity is ignored and documents
    # in the same server are eclipsed.  If the limit is `Condition::ECLDIR', similarity is ignored
    # and documents in the same directory are eclipsed.  If the limit is `Condition::ECLFILE',
    # similarity is ignored and documents of the same file are eclipsed.
    # The return value is always `nil'.
    def set_eclipse(limit)
      # native code ...
    end
    # Set the attribute distinction filter.
    # `name' specifies the name of an attribute to be distinct.
    # The return value is always `nil'.
    def set_distinct(name)
    end
    # Set the mask of targets of meta search.
    # `mask' specifies a masking number.  1 means the first target, 2 means the second target, 4
    # means the third target, and power values of 2 and their summation compose the mask.
    # The return value is always `nil'.
    def set_mask(mask)
      # native code ...
    end
    #--------------------------------
    # private methods
    #--------------------------------
    private
    # Create a search condition object.
    def initialize()
      # native code ...
    end
  end
  #----------------------------------------------------------------
  #++ Abstraction of result set from database.
  #----------------------------------------------------------------
  class Result
    #--------------------------------
    # public methods
    #--------------------------------
    public
    # Get the number of documents.
    # The return value is the number of documents in the result.
    def doc_num()
      # native code ...
    end
    # Get the ID number of a document.
    # `index' specifies the index of a document.
    # The return value is the ID number of the document or -1 if the index is out of bounds.
    def get_doc_id(index)
      # native code ...
    end
    # Get the index of the container database of a document.
    # `index' specifies the index of a document.
    # The return value is the index of the container database of the document or -1 if the index
    # is out of bounds.
    def get_dbidx(index)
      # native code ...
    end
    # Get an array of hint words.
    # The return value is an array of hint words.
    def hint_words()
      # native code ...
    end
    # Get the value of a hint word.
    # `word' specifies a hint word.  An empty string means the number of whole result.
    # The return value is the number of documents corresponding the hint word.  If the word is
    # in a negative condition, the value is negative.
    def hint(word)
      # native code ...
    end
    # Get the score of a document.
    # `index' specifies the index of a document.
    # The return value is the score of the document or -1 if the index is out of bounds.
    def get_score(index)
      # native code ...
    end
    # Get an array of ID numbers of eclipsed docuemnts of a document.
    # `id' specifies the ID number of a parent document.
    # The return value is an array whose elements expresse the ID numbers and their scores
    # alternately.
    def get_shadows(id)
      # native code ...
    end
    #--------------------------------
    # private methods
    #--------------------------------
    private
    # Create a result set object.
    def initialize()
      # native code ...
    end
  end
  #----------------------------------------------------------------
  #++ Abstraction of database.
  #----------------------------------------------------------------
  class Database
    #--------------------------------
    # public constants
    #--------------------------------
    public
    # version of Hyper Estraier
    VERSION = "0.0.0"
    # error code: no error
    ERRNOERR = 0
    # error code: invalid argument
    ERRINVAL = 1
    # error code: access forbidden
    ERRACCES = 2
    # error code: lock failure
    ERRLOCK = 3
    # error code: database problem
    ERRDB = 4
    # error code: I/O problem
    ERRIO = 5
    # error code: no item
    ERRNOITEM = 6
    # error code: miscellaneous
    ERRMISC = 9999
    # open mode: open as a reader
    DBREADER = 1 << 0
    # open mode: open as a writer
    DBWRITER = 1 << 1
    # open mode: a writer creating
    DBCREAT = 1 << 2
    # open mode: a writer truncating
    DBTRUNC = 1 << 3
    # open mode: open without locking
    DBNOLCK = 1 << 4
    # open mode: lock without blocking
    DBLCKNB = 1 << 5
    # open mode: use perfect N-gram analyzer
    DBPERFNG = 1 << 10
    # open mode: use character category analyzer
    DBCHRCAT = 1 << 11
    # open mode: small tuning
    DBSMALL = 1 << 20
    # open mode: large tuning
    DBLARGE = 1 << 21
    # open mode: huge tuning
    DBHUGE = 1 << 22
    # open mode: huge tuning second
    DBHUGE2 = 1 << 23
    # open mode: huge tuning third
    DBHUGE3 = 1 << 24
    # open mode: store scores as void
    DBSCVOID = 1 << 25
    # open mode: store scores as integer
    DBSCINT = 1 << 26
    # open mode: refrain from adjustment of scores
    DBSCASIS = 1 << 27
    # attribute index type: for multipurpose sequencial access method
    IDXATTRSEQ = 0
    # attribute index type: for narrowing with attributes as strings
    IDXATTRSTR = 1
    # attribute index type: for narrowing with attributes as numbers
    IDXATTRNUM = 2
    # optimize option: omit purging dispensable region of deleted
    OPTNOPURGE = 1 << 0
    # optimize option: omit optimization of the database files
    OPTNODBOPT = 1 << 1
    # merge option: clean up dispensable regions
    MGCLEAN = 1 << 0
    # put_doc option: clean up dispensable regions
    PDCLEAN = 1 << 0
    # put_doc option: weight scores statically when indexing
    PDWEIGHT = 1 << 1
    # out_doc option: clean up dispensable regions
    ODCLEAN = 1 << 0
    # get_doc option: no attributes
    GDNOATTR = 1 << 0
    # get_doc option: no text
    GDNOTEXT = 1 << 1
    # get_doc option: no keywords
    GDNOKWD = 1 << 2
    #--------------------------------
    # public class methods
    #--------------------------------
    public
    # Search plural databases for documents corresponding a condition.
    # `dbs' specifies an array whose elements are database objects.
    # `cond' specifies a condition object.
    # The return value is a result object.  On error, `nil' is returned.
    def self.search_meta(dbs, cond)
      # native code ...
    end
    #--------------------------------
    # public methods
    #--------------------------------
    public
    # Get the string of an error code.
    # `ecode' specifies an error code.
    # The return value is the string of the error code.
    def err_msg(ecode)
      # native code ...
    end
    # Open a database.
    # `name' specifies the name of a database directory.
    # `omode' specifies open modes: `Database::DBWRITER' as a writer, `Database::DBREADER' as a
    # reader.  If the mode is `Database::DBWRITER', the following may be added by bitwise or:
    # `Database::DBCREAT', which means it creates a new database if not exist,
    # `Database::DBTRUNC', which means it creates a new database regardless if one exists.  Both
    # of `Database::DBREADER' and  `Database::DBWRITER' can be added to by bitwise or:
    # `Database::DBNOLCK', which means it opens a database file without file locking, or
    # `Database::DBLCKNB', which means locking is performed without blocking.  If
    # `Database::DBNOLCK' is used, the application is responsible for exclusion control.
    # `Database::DBCREAT' can be added to by bitwise or: `Database::DBPERFNG', which means N-gram
    # analysis is performed against European text also, `Database::DBCHACAT', which means
    # character category analysis is performed instead of N-gram analysis, `Database::DBSMALL',
    # which means the index is tuned to register less than 50000 documents, `Database::DBLARGE',
    # which means the index is tuned to register more than 300000 documents, `Database::DBHUGE',
    # which means the index is tuned to register more than 1000000 documents, `Database::DBHUGE2',
    # which means the index is tuned to register more than 5000000 documents, `Database::DBHUGE3',
    # which means the index is tuned to register more than 10000000 documents,
    # `Database::DBSCVOID', which means scores are stored as void, `Database::DBSCINT', which
    # means scores are stored as 32-bit integer, `Database::DBSCASIS', which means scores are
    # stored as-is and marked not to be tuned when search.
    # The return value is true if success, else it is false.
    def open(name, omode)
      # native code ...
    end
    # Close the database.
    # The return value is true if success, else it is false.
    def close()
      # native code ...
    end
    # Get the last happened error code.
    # The return value is the last happened error code.
    def error()
      # native code ...
    end
    # Check whether the database has a fatal error.
    # The return value is true if the database has fatal erroor, else it is false.
    def fatal()
      # native code ...
    end
    # Add an index for narrowing or sorting with document attributes.
    # `name' specifies the name of an attribute.
    # `type' specifies the data type of attribute index; `Database::IDXATTRSEQ' for multipurpose
    # sequencial access method, `Database::IDXATTRSTR' for narrowing with attributes as strings,
    # `Database::IDXATTRNUM' for narrowing with attributes as numbers.
    # The return value is true if success, else it is false.
    def add_attr_index(name, type)
      # native code ...
    end
    # Flush index words in the cache.
    # `max' specifies the maximum number of words to be flushed.  If it not more than zero, all
    # words are flushed.
    # The return value is true if success, else it is false.
    def flush(max)
      # native code ...
    end
    # Synchronize updating contents.
    # The return value is true if success, else it is false.
    def sync()
      # native code ...
    end
    # Optimize the database.
    # `options' specifies options: `Database::OPTNOPURGE' to omit purging dispensable region of
    # deleted documents, `Database::OPTNODBOPT' to omit optimization of the database files.  The
    # two can be specified at the same time by bitwise or.
    # The return value is true if success, else it is false.
    def optimize(options)
      # native code ...
    end
    # Merge another database.
    # `name' specifies the name of another database directory.
    # `options' specifies options: `Database::MGCLEAN' to clean up dispensable regions of the
    # deleted document.
    # The return value is true if success, else it is false.
    def merge(name, options)
      # native code ...
    end
    # Add a document.
    # `doc' specifies a document object.  The document object should have the URI attribute.
    # `options' specifies options: `Database::PDCLEAN' to clean up dispensable regions of the
    # overwritten document.
    # The return value is true if success, else it is false.
    def put_doc(doc, options)
      # native code ...
    end
    # Remove a document.
    # `id' specifies the ID number of a registered document.
    # `options' specifies options: `Database::ODCLEAN' to clean up dispensable regions of the
    # deleted document.
    # The return value is true if success, else it is false.
    def out_doc(id, options)
      # native code ...
    end
    # Edit attributes of a document.
    # `doc' specifies a document object.
    # The return value is true if success, else it is false.
    def edit_doc(doc)
      # native code ...
    end
    # Retrieve a document.
    # `id' specifies the ID number of a registered document.
    # `options' specifies options: `Database::GDNOATTR' to ignore attributes, `Database::GDNOTEXT'
    # to ignore the body text, `Database::GDNOKWD' to ignore keywords.  The three can be
    # specified at the same time by bitwise or.
    # The return value is a document object.  On error, `nil' is returned.
    def get_doc(id, options)
      # native code ...
    end
    # Retrieve the value of an attribute of a document.
    # `id' specifies the ID number of a registered document.
    # `name' specifies the name of an attribute.
    # The return value is the value of the attribute or `nil' if it does not exist.
    def get_doc_attr(id, name)
      # native code ...
    end
    # Get the ID of a document specified by URI.
    # `uri' specifies the URI of a registered document.
    # The return value is the ID of the document.  On error, -1 is returned.
    def uri_to_id(uri)
      # native code ...
    end
    # Get the name.
    # The return value is the name of the database.
    def name()
      # native code ...
    end
    # Get the number of documents.
    # The return value is the number of documents in the database.
    def doc_num()
      # native code ...
    end
    # Get the number of unique words.
    # The return value is the number of unique words in the database.
    def word_num()
      # native code ...
    end
    # Get the size.
    # The return value is the size of the database.
    def size()
      # native code ...
    end
    # Search for documents corresponding a condition.
    # `cond' specifies a condition object.
    # The return value is a result object.  On error, `nil' is returned.
    def search(cond)
      # native code ...
    end
    # Check whether a document object matches the phrase of a search condition object definitely.
    # `doc' specifies a document object.
    # `cond' specifies a search condition object.
    # The return value is true if the document matches the phrase of the condition object
    # definitely, else it is false.
    def scan_doc(doc, cond)
      # native code ...
    end
    # Set the maximum size of the cache memory.
    # `size' specifies the maximum size of the index cache.  By default, it is 64MB.  If it is
    # not more than 0, the current size is not changed.
    # `anum' specifies the maximum number of cached records for document attributes.  By default,
    # it is 8192.  If it is not more than 0, the current size is not changed.
    # `tnum' specifies the maximum number of cached records for document texts.  By default, it
    # is 1024.  If it is not more than 0, the current size is not changed.
    # `rnum' specifies the maximum number of cached records for occurrence results.  By default,
    # it is 256.  If it is not more than 0, the current size is not changed.
    # The return value is always `nil'.
    def set_cache_size(size, anum, tnum, rnum)
      # native code ...
    end
    # Add a pseudo index directory.
    # `path' specifies the path of a pseudo index directory.
    # The return value is true if success, else it is false.
    def add_pseudo_index(path)
      # native code ...
    end
    # Set the maximum number of expansion of wild cards.
    # `num' specifies the maximum number of expansion of wild cards.
    # The return value is always `nil'.
    def set_wildmax(num)
      # native code ...
    end
    # Set the callback function to inform of database events.
    # `informer' specifies an arbitrary object with a method named as `inform'.  The method
    # should have one parameter for a string of a message of each event.
    # The return value is always `nil'.
    def set_informer(informer)
      # native code ...
    end
    #--------------------------------
    # private methods
    #--------------------------------
    private
    # Create a database object.
    def initialize()
      # native code ...
    end
  end
end



# END OF FILE
