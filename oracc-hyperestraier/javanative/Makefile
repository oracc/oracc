# Makefile for the Java binding of Hyper Estraier



#================================================================
# Setting Variables
#================================================================


# Generic settings
SHELL = /bin/sh

# Packaging
PACKAGE = hyperestraier-javanative
VERSION = 1.0.0
LIBVER = 1
LIBREV = 0

# Targets
MYJARS = estraier.jar
MYJAVAS = Document.java Condition.java Database.java DatabaseInformer.java \
  Result.java Utility.java Cmd.java
MYLIBOBJS = document.o condition.o database.o result.o myconf.o
MYLIBS = libjestraier.so.$(LIBVER).$(LIBREV).0 \
  libjestraier.so.$(LIBVER) libjestraier.so
MYMACLIBS = libjestraier.$(LIBVER).$(LIBREV).0.dylib \
  libjestraier.$(LIBVER).dylib libjestraier.dylib
MYHPUXLIBS = libjestraier.sl
MYWINLIBS = libjestraier.dll.a

# Install destinations
prefix = /usr/local
exec_prefix = ${prefix}
MYHEADDIR = ${prefix}/include
MYLIBDIR = ${exec_prefix}/lib
DESTDIR =

# Building binaries
MYJAVAHOME = /usr/java
JVMPLATFORM = linux
JAVAC = /usr/java/bin/javac
JAVACFLAGS = -d .
JAR = /usr/java/bin/jar
JAVAH = /usr/java/bin/javah
JAVADOC = /usr/java/bin/javadoc
JAVARUN = /usr/java/bin/java
CC = gcc
CPPFLAGS =  -I. -I.. -I$(MYHEADDIR) \
  -I$(HOME)/include -I/usr/local/include -I/usr/include/qdbm -DNDEBUG -I/usr/local/pkg/qdbm-1.8.78/include \
  -I$(MYJAVAHOME)/include -I$(MYJAVAHOME)/include/$(JVMPLATFORM) -I$(MYJAVAHOME)/Headers \
  -D_XOPEN_SOURCE_EXTENDED=1 -D_GNU_SOURCE=1 -D__EXTENSIONS__=1 -D_HPUX_SOURCE=1 \
  -DPIC=1 -D_THREAD_SAFE=1 -D_REENTRANT=1 -DPACKAGE_NAME=\"hyperestraier-javanative\" -DPACKAGE_TARNAME=\"hyperestraier-javanative\" -DPACKAGE_VERSION=\"1.0.0\" -DPACKAGE_STRING=\"hyperestraier-javanative\ 1.0.0\" -DPACKAGE_BUGREPORT=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DSIZEOF_VOID_P=8 -DSIZEOF_INT=4 -DHAVE_LIBC=1 -DHAVE_LIBM=1 -DHAVE_LIBZ=1 -DHAVE_LIBQDBM=1 -DHAVE_LIBESTRAIER=1
CFLAGS = -Wall -fPIC -fsigned-char -O2 -fomit-frame-pointer -fforce-addr
LDFLAGS =  -L. -L.. -L$(MYLIBDIR) -L$(HOME)/lib -L/usr/local/lib  -L/usr/local/pkg/qdbm-1.8.78/lib
LIBS = -lestraier -lqdbm -lz -lm -lc 
MTLIBS = -lpthread 
SKLIBS = -lresolv -lnsl 
RUNENV = LD_LIBRARY_PATH=.:..:/lib:/usr/lib:$(MYLIBDIR):$(HOME)/lib:/usr/local/lib::/usr/local/pkg/qdbm-1.8.78/lib



#================================================================
# Suffix rules
#================================================================


.SUFFIXES :
.SUFFIXES : .c .o .java .class

.c.o :
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<



#================================================================
# Actions
#================================================================


all : $(MYJARS) $(MYLIBS)
	@printf '\n'
	@printf '#================================================================\n'
	@printf '# Ready to install.\n'
	@printf '#================================================================\n'


clean :
	rm -rf estraier *.class $(MYJARS) $(MYLIBS) *.jar *.o *.a *.so \
	  *.exe *.dll.a *.dll *.dylib *.jnilib *.sl a.out check.out *.log casket *~


install :
	mkdir -p $(DESTDIR)$(MYLIBDIR)
	cp -Rf $(MYJARS) $(MYLIBS) $(DESTDIR)$(MYLIBDIR)
	@printf '\n'
	@printf '#================================================================\n'
	@printf '# Thanks for using Hyper Estraier for Java.\n'
	@printf '#================================================================\n'


uninstall :
	cd $(DESTDIR)$(MYLIBDIR) && rm -f $(MYJARS) $(MYLIBS)


dist :
	autoconf
	make doc
	make distclean


distclean : clean
	cd example && make clean
	rm -rf Makefile config.cache config.log config.status autom4te.cache


head : estraier.jar
	CLASSPATH=estraier.jar $(JAVAH) -jni \
	  estraier.Document estraier.Condition estraier.Database estraier.Result


check :
	rm -rf casket
	$(RUNENV) $(JAVARUN) -cp estraier.jar estraier.Cmd put casket ../misc/test001.est
	$(RUNENV) $(JAVARUN) -cp estraier.jar estraier.Cmd put casket ../misc/test002.est
	$(RUNENV) $(JAVARUN) -cp estraier.jar estraier.Cmd put casket ../misc/test003.est
	$(RUNENV) $(JAVARUN) -cp estraier.jar estraier.Cmd put casket ../misc/test004.est
	$(RUNENV) $(JAVARUN) -cp estraier.jar estraier.Cmd put casket ../misc/test005.est
	$(RUNENV) $(JAVARUN) -cp estraier.jar estraier.Cmd put casket ../misc/test006.est
	$(RUNENV) $(JAVARUN) -cp estraier.jar estraier.Cmd edit casket 1 "@title" "java"
	$(RUNENV) $(JAVARUN) -cp estraier.jar estraier.Cmd get casket 1
	$(RUNENV) $(JAVARUN) -cp estraier.jar estraier.Cmd get casket 1 "@title"
	$(RUNENV) $(JAVARUN) -cp estraier.jar estraier.Cmd out casket 1
	$(RUNENV) $(JAVARUN) -cp estraier.jar estraier.Cmd inform casket
	$(RUNENV) $(JAVARUN) -cp estraier.jar estraier.Cmd optimize casket
	$(RUNENV) $(JAVARUN) -cp estraier.jar estraier.Cmd search \
	  -vf -cd casket "estraier" > check.out
	$(RUNENV) $(JAVARUN) -cp estraier.jar estraier.Cmd search \
	  -vs -gs -attr "@uri" -ord "@mdate NUMD" -max 1 casket "estraier OR mikio" > check.out
	rm -rf casket


mac :
	make MYLIBS="$(MYMACLIBS)" CFLAGS="-Wall -fsigned-char -fno-common -O2 -DNDEBUG"


check-mac :
	make RUNENV="DYLD_LIBRARY_PATH=." check


install-mac :
	make MYLIBS="$(MYMACLIBS)" install


hpux :
	make MYLIBS="$(MYHPUXLIBS)"


check-hpux :
	make RUNENV="SHLIB_PATH=." check


install-hpux :
	make MYLIBS="$(MYHPUXLIBS)" install


uninstall-hpux :
	make MYLIBS="$(MYHPUXLIBS)" uninstall


win :
	make CC="gcc -mno-cygwin" MYLIBS="$(MYWINLIBS)" \
	  CFLAGS="-Wall -fsigned-char -O0 -DNDEBUG" LIBS="-lestraier -lqdbm -lz -liconv" \
	  MTLIBS="-lpthreadGC2" SKLIBS="-lws2_32"


doc :
	make docclean
	mkdir -p ../doc/javanativeapi
	$(JAVADOC) -locale en -windowtitle estraier -overview overview.html \
	  -tag note:a:"Note:" -d ../doc/javanativeapi -nodeprecated -nohelp -quiet *.java


docclean :
	rm -rf ../doc/javanativeapi


.PHONY : all clean install casket check



#================================================================
# Building binaries
#================================================================


estraier.jar : $(MYJAVAS)
	$(JAVAC) $(JAVACFLAGS) $(MYJAVAS)
	$(JAR) cvf $@ estraier/*.class


libjestraier.so.$(LIBVER).$(LIBREV).0 : $(MYLIBOBJS)
	if $(CC) --print-prog-name=ld | egrep -i '(solaris|/ccs/|/ucb/)' > /dev/null ; \
	  then \
	    $(CC) -shared -Wl,-G,-h,libjestraier.so.$(LIBVER) -o $@ $(MYLIBOBJS) \
	      $(LDFLAGS) $(SKLIBS) $(MTLIBS) $(LIBS) ; \
	  else \
	    $(CC) -shared -Wl,-soname,libjestraier.so.$(LIBVER) -o $@ $(MYLIBOBJS) \
	      $(LDFLAGS) $(SKLIBS) $(MTLIBS) $(LIBS) ; \
	  fi


libjestraier.so.$(LIBVER) : libjestraier.so.$(LIBVER).$(LIBREV).0
	ln -f -s libjestraier.so.$(LIBVER).$(LIBREV).0 $@


libjestraier.so : libjestraier.so.$(LIBVER).$(LIBREV).0
	ln -f -s libjestraier.so.$(LIBVER).$(LIBREV).0 $@


libjestraier.$(LIBVER).$(LIBREV).0.dylib : $(MYLIBOBJS)
	$(CC) -dynamiclib -o $@ \
	  -install_name $(MYLIBDIR)/libjestraier.$(LIBVER).dylib \
	  -current_version $(LIBVER).$(LIBREV).0 \
	  -compatibility_version $(LIBVER) \
	  $(MYLIBOBJS) $(LDFLAGS) $(SKLIBS) $(MTLIBS) $(LIBS)


libjestraier.$(LIBVER).dylib : libjestraier.$(LIBVER).$(LIBREV).0.dylib
	ln -f -s libjestraier.$(LIBVER).$(LIBREV).0.dylib $@


libjestraier.dylib : libjestraier.$(LIBVER).$(LIBREV).0.dylib
	ln -f -s libjestraier.$(LIBVER).$(LIBREV).0.dylib $@


libjestraier.sl : $(MYLIBOBJS)
	$(CC) -shared -Wl,-b -o $@ $(MYLIBOBJS)


libjestraier.dll.a : jestraier.dll


jestraier.dll : $(MYLIBOBJS)
	$(CC) -shared -o $@ \
	  -Wl,--out-implib=lib$@.a \
	  -Wl,--export-all-symbols \
	  -Wl,--enable-auto-import \
	  -Wl,--add-stdcall-alias \
	  -Wl,--whole-archive \
	  -Wl,--no-whole-archive \
	  $(MYLIBOBJS) $(LDFLAGS) $(SKLIBS) $(MTLIBS) $(LIBS)


document.o : estraier_Document.h

condition.o : estraier_Condition.h

database.o : estraier_Database.h

result.o : estraier_Result.h

$(MYLIBOBJS) : myconf.h



# END OF FILE
