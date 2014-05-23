SUPERGLOSSARIES
===============

* Superglossaries are Oracc's way of merging multiple glossaries into
  a single presentation front-end.

* This mechanism has several uses:

 - Oracc's global glossaries, like ePSD and the new global glossary of
   Conventional Akkadian (OGGCA)

 - Upper level glossaries for projects that have multiple subprojects

 - Unified Akkadian glossaries for projects that have Akkadian
   lemmatization separated into different dialects

* Superglossaries are implemented as a separate project type,
  'superglo'

* Superglossaries are implemented in a very non-intrusive way with
  respect to the standard glossary processing mechanism, i.e., by
  inserting a pre-processing phase before the normal processing.

* The software that implements this preprocessing is classified as
  Lemmatization Phase 0, and lives in oracc/misc/l2phase0

* Superglossaries merge a list of glossaries which may live in the
  host projects or may be maintained locally, shadowing the host
  projects

* Merging is done by extracting signatures whose language matches the
  language criterion from all of the projects in the list; the
  signatures file which is used is the project's 01bld/project.sig,
  which also contains the instance data for the project

* The merge may be dumb or smart

  - dumb merging is accomplished by simply reading all of the
    signatures into a single process and inverting it to produce a
    glossary--this is essentially the same as a normal glossary
    building operation, but may include glossaries from several
    projects

  - smart merging is accomplished by a mapping process in which the
    glossary being merged into the superglossary is first passed
    through a map to align the incoming signature to a base glossary;
    the base glossary and maps are maintained in the superglossary
    while the source glossaries are maintained externally in the
    host projects.

  - smart merging may be done at either of two levels:
    
    = CF[GW]POS
    = CF[GW//SENSE]POS'EPOS


FILES
=====

* As mentioned above, superglossaries use three types of data file:

  - BASE: the reference glossary for the superglossary, a standard glossary file

  - SOURCE: the source files that are to be merged, also standard
    glossary files; superglossaries are designed so that input
    glossaries do not need to be modified by the superglossary project

  - MAP: each input glossary may have an associated map file which is
    in the familiar glossary format but has additional @-commands

* In addition, superglossaries have their own simple configuration
  file which gives the name of the base language and file and the project/language
  pairs of the inputs:

  @baselang akk-x-conakk
  @basefile 00lib/akk-x-conakk.glo
  @source cams/gkab:akk-x-stdbab
  ...

* Note that it is unnecessary to give the location of the input files
  because that is known to the system which catalogues the available/requested inputs as the first processing step and issues a warning if it is unable to find a 


MAP FILE FORMAT
===============

* map files have the following header instead of the standard glossary header:

  - @baselang [LANG]
  - @source [PROJECT]:[LANG]

* LANG is a standard language code

  - for @baselang it is the target language for the superglossary
  - for @source it is the language used in the input glossary

* The following header associates GKAB's Standard Babylonian glossary with
  the OGCCA Conventional Akkadian glossary

  @base akk-x-conakk
  @source cams/gkab:akk-x-stdbab

* map entries have the following structure:

  @map CF[GW]POS => CF[GW]POS
  @map CF[GW//SENSE]POS'EPOS => CF[GW//SENSE]POS'EPOS


PROCESSING
==========

* The processing sequence with superglossaries is as follows:

  - read the config file
  - check that all glossaries referenced in the config file are available
  - read all of the map files
  - read the base file
  - read each source file
    = check the signature to see if it is subject to mapping, and edit it is necessary
    = check that the result signature matches an entry in the base; warn if not
    = add the signature to the superglossary's 01bld/project.sig
    = merge duplicate entries in 01bld/project.sig
  - hand processing over to l2phase1
