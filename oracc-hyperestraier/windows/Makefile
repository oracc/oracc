# Makefile for Hyper Estraier Win32 Tools



#================================================================
# Setting Variables
#================================================================


MYBINS = scmutil.exe
CC = gcc -mno-cygwin 
CFLAGS = -I. -I$(HOME)/include -I/usr/local/include -Wall
LDFLAGS = -L. -L$(HOME)/lib -L/usr/local/lib
LIBS =
LDENV = LD_RUN_PATH=/lib:/usr/lib:$(HOME)/lib:/usr/local/lib:.



#================================================================
# Suffix rules
#================================================================


.SUFFIXES :
.SUFFIXES : .c .o

.c.o :
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $<



#================================================================
# Actions
#================================================================


all : $(MYBINS)


clean :
	rm -rf $(MYBINS) *.o a.out *~



#================================================================
# Building binaries
#================================================================


scmutil.exe : scmutil.o
	$(CC) $(CFLAGS) -o $@ scmutil.o $(LDFLAGS) $(LIBS)



# END OF FILE
