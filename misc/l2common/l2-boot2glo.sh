#!/bin/sh
while getopts ":l:p:" opt; do
  case $opt in
    l) lang="$OPTARG"
    ;;
    p) proj="$OPTARG"
    ;;
    \?) echo "Invalid option -$OPTARG" >&2
    ;;
  esac
done

#echo "lang=$lang; proj=$proj; name=$proj/$lang"

if [ "$lang" == "" -o "$proj" == "" ]; then
    echo l2-boot2glo.sh: must give -l [LANG] and -p [PROJECT] on command line
    exit 1
fi

if [ "$lang" == "qpn" ]; then
    fgrep '(sig)' | cut -f2 | grep '\][A-Z][A-Z]' \
	| l2p2-sig-g2a.plx -lang $lang -proj $proj -filter >boot.glo
else
    fgrep '(sig)' | cut -f2 | fgrep "%$lang:" | grep -v '\][A-Z][A-Z]' \
	| l2p2-sig-g2a.plx -lang $lang -proj $proj -filter >boot.glo
fi

l2-glomanager.plx -sort boot.glo >$lang.glo
