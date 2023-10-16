%{
#include <string.h>
#include "cbd.h"
#include "cbd.tab.h"
#include "startstates.h"
%}
struct cbdtag;
%%
project, PROJECT, 0, s_word
lang, LANG, 0, s_word
name, NAME, 0, s_text
entry, ENTRY, 0, s_cf
bases, BASES, 0, s_b
form,  FORM, 0, s_f
sense, SENSE, 0, s_s
sensel, SENSEL, 0, s_s
isslp, ISSLP, 0, s_text
inote, INOTE, 0, s_text
parts, PARTS, 0, s_cf
bff,   BFF, 0, 0
alias, ALIAS, 0, s_cf
allow, ALLOW, 0, s_allow
note,  NOTE, 0, s_text
bib,   BIB, 0, s_text
equiv, EQUIV, 0, s_text
oid,   OID, 0, s_word
collo, COLLO, 0, 0
phon,  PHON, 0, 0
root,  ROOT, 0, 0
stems, STEMS, 0, 0
prop,  PROP, 0, 0
proplist, PROPS, 0, s_word
pl_coord, PL_COORD, 0, 0
pl_id,    PL_ID, 0, 0
pl_uid,   PL_UID, 0, 0
disc,  EDISC, 0, s_text
file,  FILESPEC, 0, s_word
translang, TRANSLANG, 0, s_word
discl, EDISCL, 0, s_text
gwl,   GWL, 0, s_text
i18n,  I18N, 0, 0
notel, NOTEL, 0, 
senses, SENSES, 0, 0
why,   WHY, 0, s_text
