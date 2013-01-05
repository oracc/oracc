# Makefile for sample programs of the Java interface



#================================================================
# Setting Variables
#================================================================


# Generic settings
SHELL = /bin/sh

# Targets
MYCLASSES = Example001.class  Example002.class

# Building binaries
JAVAC = javac
JAVACFLAGS = -d .
JAVACENV = CLASSPATH=.:../estraierpure.jar



#================================================================
# Actions
#================================================================


all : $(MYCLASSES)


clean :
	rm -rf $(MYCLASSES) *.exe *.o a.out check.out gmon.out leak.log casket *~


.PHONY : all clean



#================================================================
# Building binaries
#================================================================


Example001.class : Example001.java
	$(JAVACENV) $(JAVAC) $(JAVACFLAGS) Example001.java


Example002.class : Example002.java
	$(JAVACENV) $(JAVAC) $(JAVACFLAGS) Example002.java



# END OF FILE
