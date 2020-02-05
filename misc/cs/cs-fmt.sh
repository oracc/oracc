#!/bin/sh
java -classpath $ORACC/lib/java/CS.jar csearch/CorpusSearch $ORACC/lib/data/cs-format.q $1 >/dev/null 2>&1
cs-fmt-fixup.plx $1.fmt >$1
