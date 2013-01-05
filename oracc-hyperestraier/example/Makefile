# Makefile for sample programs of Hyper Estraier



#================================================================
# Setting Variables
#================================================================


# Generic settings
SHELL = /bin/sh

# Targets
MYBINS = coreexample001 coreexample002 nodeexample001 nodeexample002

# Building binaries
CC = gcc
CFLAGS = -I. -I.. `../estconfig --cflags`
LDFLAGS = `../estconfig --ldflags`
LIBS = -L. -L.. `../estconfig --libs`
LDENV = LD_RUN_PATH=/lib:/usr/lib:$(HOME)/lib:/usr/local/lib:.:..



#================================================================
# Suffix rules
#================================================================


.SUFFIXES :
.SUFFIXES : .c .o

.c.o :
	$(CC) -c $(CFLAGS) $<



#================================================================
# Actions
#================================================================


all : $(MYBINS)


clean :
	rm -rf $(MYBINS) *.exe *.o a.out check.out gmon.out leak.log casket *~


.PHONY : all clean



#================================================================
# Building binaries
#================================================================


coreexample001 : coreexample001.o
	$(LDENV) $(CC) $(CFLAGS) -o $@ coreexample001.o $(LDFLAGS) $(LIBS)


coreexample002 : coreexample002.o
	$(LDENV) $(CC) $(CFLAGS) -o $@ coreexample002.o $(LDFLAGS) $(LIBS)


nodeexample001 : nodeexample001.o
	$(LDENV) $(CC) $(CFLAGS) -o $@ nodeexample001.o $(LDFLAGS) $(LIBS)


nodeexample002 : nodeexample002.o
	$(LDENV) $(CC) $(CFLAGS) -o $@ nodeexample002.o $(LDFLAGS) $(LIBS)



# END OF FILE
