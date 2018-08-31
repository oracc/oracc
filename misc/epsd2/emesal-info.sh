#!/bin/sh
epsd2-pull-forms.plx 00lib/sux-x-emesal.glo >01tmp/emesal.forms
cbdentries.plx 00lib/sux-x-emesal.glo >01tmp/emesal.entries
cbdentries.plx ../00src/sux.glo >01tmp/emegir.entries
