# Install all of the .rnc schemas and nxml-mode's schemas.xml
# in oracc/lib/schemas.
for a in * ; do
    if [ -d $a ]; then
	rm -fr ${ORACC}/lib/schemas/$a
    fi
done
cp -pR * ${ORACC}/lib/schemas
./mk-dist-schemas.sh
