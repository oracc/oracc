#!/bin/sh
java -classpath $ORACC/lib/java/CS.jar csearch/CorpusSearch $ORACC/lib/scripts/cs-format.q $1 2>/dev/null
cs-fmt-fixup.plx $1.fmt >$1
