#!/bin/sh

# Install all of the scripts into oracc/bin,
# fixing the @@ORACC@@ path as we go.
for a in *.sh oracc-ant ; do \
    exe=$ORACC/bin/`basename $a` ; \
    rm -f $exe ; \
    echo installing $exe; \
    perl -C0 -pe "s#\@\@ORACC\@\@#$ORACC#g" <$a >$exe ; \
    chmod +x $exe ; chmod o+r $exe ; chmod -w $exe; \
done

# Install the ant script in lib/config,
# fixing the @@ORACC@@ path as we go.
mkdir -p $ORACC/lib/config
for a in eXist-*.xml ; do \
    ins=$ORACC/lib/config/$a ; \
    rm -f $ins ; \
    echo installing $ins; \
    perl -C0 -pe "s#\@\@ORACC\@\@#$ORACC#g" <$a >$ins ; \
    chmod o+r $ins ; chmod -w $ins; \
done
