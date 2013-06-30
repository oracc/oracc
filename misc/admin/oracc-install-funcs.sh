function dirlinks {
    for a in bld tmp; do
	mkdir -p $ORACC/$a/$project
	ln -s $ORACC/$a/$project $ORACC_HOME/$project/01$a
    done
    for a in pub www xml; do
	mkdir -p $ORACC/$a/$project
	ln -s $ORACC/$a/$project $ORACC_HOME/$project/02$a	
    done
    subp=`echo $project | tr / -`;
    if [ "$project" = "$subp" ]; then
	rm -fr $ORACC/$project
	ln -sf $ORACC_HOME/$project $ORACC/$project
    fi
}
