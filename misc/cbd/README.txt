Aligning with epsd2
===================

1. use cbdalignentries.plx to identify entries that need to be renamed
or added: use +@entry to mark additions; use '>new [word] N' to
identify entries to rename.  It's easiest to do this using the notation:

	@entry old [word] N > new [word N

The validator understands the ' > ' notation and puts them in the log
file as [m] (map) items, then they get converted to edit items in with
-apply.

2. use cbdalignentries -apply to output the new version of the aligned
glossary with edit markers in it:

	cbdalignentries.plx -apply -base ~/orc/epsd2/00src/sux.glo 00lib/sux.glo >new.glo

2. use cbdpp.plx -edit 00lib/sux.glo >new.glo to apply edit markers; check new.glo and replace 00lib/sux.glo with it

3. now align bases

4. and align senses

5. then apply corpus update script to corpus

