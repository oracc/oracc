Implementation Notes
====================

- Stash

  . The general idea is that before editing the project's glossaries with cbdedit.plx they must
    saved; then the editing happens; then they are marked as complete
  . The stash is stored in 00etc/stash/ISODATE
  . The stash consists of:

    - all .atf files from 00atf
    - the output of wid2lem which gives the full lem data for the last build
    - all .glo files from 00lib or, in the case of epsd2-style
      glossaries, the merged 00src/sux.glo and 00src/sux.forms
    - the stash also contains reduced versions of the glossaries where
      elements that are not tracked in the history are removed

  . cbdstash.plx takes the following arguments:

    - start: initializes a stash with date ISODATE
      . if the stash already exists and is a new stash it is overwritten
      . if the stash already exists but has a paired history an error is generated

    - done: marks a stash as complete--this compresses the stash and if disk space is an issue
      it can be deleted without affecting the project itself

    - check: ensures that the stash is up to date with respect to its
      stashed files; if any of the files have changed an error is generated

    - update: replaces the files in the current stash with a freshly
      generated set

    - show: prints the ISODATE of the current file

- History

  . History is stored in 00etc/history
  . History is partly controlled by cbdstash.plx and partly managed automatically by cbdedit.plx
  . The form of the file is:

	<ISODATE>\t<FROM>\t<TO>

    <ISODATE> is the date in ISO8601 format with hyphens
    <FROM> is the CBD line edited, e.g., @entry a [ater] N
    <TO> is the CBD line after editing, e.g., @entry a [water] N

- Alignment

  . Alignment is the process of marking up a new glossary with a base
    glossary; there are tools to help with this as detailed below
  . The end result of alignment is a glossary marked up with edit
    controls that can be processed by cbdedit.plx

- Editing

  . It's important to edit key elements of the glossaries with the edit controls to ensure that the
    history is maintained accurately
  . elements that must be edited with the edit controls are:

    @entry
    @sense

  . other elements can be freely edited because the primary goal of
    the edit history is to support accurate and automated updating of
    OIDs

- OIDs

  . The OID subsystem reads the history files and translates it into a
    version that can be used for redirecting requests for changed or
    reassigned words or for giving hints about words that have been
    completely removed from the lexicon


Aligning with epsd2
===================

0. do clean build of project and stash corpus and lem data before making edits:

	cbdstash.plx -init

1. use cbdalignentries.plx to identify entries that need to be renamed
or added:

	cbdalignentries.plx 00lib/sux.glo

You can use the -base argument to select which glossary you're
aligning against; the default is epsd2/00src/sux.glo.

Use +@entry to mark additions so they stay out of the log; use '>new [word] N' to
identify entries to rename manually.  It's easiest to do this using the notation:

	@entry old [word] N > new [word] N

The validator understands the ' > ' notation and puts them in the log
file as [m] (map) items, then they get converted to edit items in with
-apply.

Review the log to make sure that cbdalignentries.plx has made correct
guesses--it does make mistakes occasionally.

2. use cbdalignentries -apply to output the new version of the aligned
glossary with edit markers in it:

	cbdalignentries.plx -apply 00lib/sux.glo

In -apply mode cbdalignentries puts its output in [LANG]-entries-aligned.glo.

3. use, e.g.,

	cbdedit.plx sux-entries-aligned.glo

to apply edit markers; cbdedit.plx places its output in a file based
on the name of the input; for entries-aligned.glo the output is in
entries-edited.glo.

4. Now stash your work so far:

	cbdstash.plx -entries

By default this operates on the glossary for the '#current' language in the stash.

e.g.,

	cbdstash.plx -entries sux

4. To align senses do:

       cbdalignsenses.plx 00lib/sux.glo ; [REVIEW LOG FILE, CORRECT AS NECESSARY, RUN AGAIN]
       cbdalignsenses.plx -apply 00lib/sux.glo
       cbdedit.plx sux-senses-aligned.glo
       cbdstash.plx -senses sux

Reviewing at each step as for entry alignment.

5. apply corpus update script to corpus

   5a. oracc update
   4b. use stashed lem data and alignment history to create new input table for atfglobal
   4c. apply table to ATF corpus
   4d. rebuild corpus
   4e. cbdstash.plx fixed : stash new version of loc-data

6. now align bases and forms--these are done together because the base alignment map is used to rewrite /BASE in the form

       cbdalignbases.plx 00lib/sux.glo ; [REVIEW LOG FILE, CORRECT AS NECESSARY, RUN AGAIN]
       cbdalignbases.plx -apply 00lib/sux.glo
       cbdedit.plx sux-bases-aligned.glo
       cbdstash.plx -bases sux

7. check and correct glossary; be sure to continue to use edit markers because this updates the history correctly

8. do final checking of glossary and corpus; if correcting glossary, use edit markers and cbdedit.plx

9. now complete the process

	cbdstash.plx -done


Edit Controls
=============

- The following edit controls are supported on @entry and @sense

  . + meaning 'add this entry/sense to the glossary without warnings'

  . > meaning 'the preceding entry/sense has been renamed to the new one on this line'
      - for @entry only, if the entry exists the current entry is merged with the new one
        otherwise the entry is renamed

  . - meaning 'remove the preceding entry/sense' from the glossary
      - for @entry only, a hint is mandatory to allow the OID system to help users
      	who are looking for the deleted term.

- Hints are given after the @entry using the '@inote' command.  They must have the form

  @inote Hint:

  . Hints are automatically expanded by cbdstash.plx with the bases
    and forms that are associated with the word as of its deletion, so
    it's not necessary to spell those out.

  . A hint may simply give a
    part of speech, most commonly 'PN' where words are deleted because
    they are always a PN:

    @inote Hint: PN

  . A hint may also give one or more CFGWPOS to indicate redirects
    from the deleted form to words that might be good alternatives for
    the user

    -@entry ku [unmng] N
    @inote Hint: ha[a fish]N


