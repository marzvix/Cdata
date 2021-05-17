# -----------------------------------------------------------
# Makefile to build Cdata into an application
# -----------------------------------------------------------

# -----------------------------------------------------------
# Where clang is installed
# -----------------------------------------------------------
CCPATH = /usr/bin 

# -----------------------------------------------------------
# Compiler
# -----------------------------------------------------------
CC = $(CCPATH)/cc
CFLAGS = -g -O0 -x c -std=c89 -DCOMPILER=10 
LDFLAGS =


# -----------------------------------------------------------
# Application name
# -----------------------------------------------------------
CDATA_APPL = cbs

# -----------------------------------------------------------
# Libraries
# -----------------------------------------------------------
LIBS = cdata ncurses
LIBPATH = $(CCPATH)/lib

# -----------------------------------------------------------
# An implicit rule to build obj files from c files
# -----------------------------------------------------------
#.c.o:
#	$(CC) -c $*
#
# -----------------------------------------------------------
# An implicit rule to build exec files from obj files
# -----------------------------------------------------------
# %.o : %
# 	$(CC) $(LDFLAGS) $< $(LOADLIBES) $(LDLIBS)
# 	gcc $< -o $@ -l$(LIBS)

all: 	cdata.lib schema dbinit invoice qd ds index \
	posttime payments

exenotyet: sort roster

.PHONY: tests
tests:  test_screen

test_screen.o: test_screen.c #sys.c screen.c screen.h sys.c sys.h

test_screen: test_screen.o sys.o screen.o # cbs.o cdata.o #-lncurses

#include  cbs.mk

# -----------------------------------------------------------
# cdata.lib
# -----------------------------------------------------------
cdata.lib: sys.o \
	dblist.o \
	database.o \
	screen.o \
	btree.o \
	ellist.o \
	datafile.o \
	clist.o \
	filename.o	
	ar rcs $@ $^

notused: cdata.o sort.o

cbs.c1 cbs.c2 cbs.c3: cbs.sch
	./schema <cbs.sch >cbs.c1 -1
	./schema <cbs.sch >cbs.c2 -2
	./schema <cbs.sch >cbs.c3 -3

cbs.c: schema cbs.c1 cbs.c2 cbs.c3
	echo '#include "cbs.c1"'> cbs.c
	echo '#include "cbs.c2"'>> cbs.c
	echo '#include "cbs.c3"'>> cbs.c

cbs.o: cbs.c # cbs.h 

ellist.o: ellist.c cdata.h

# cdata.o: cdata.c cdata.h datafile.h btree.h keys.h

database.o: database.c btree.o cdata.h

screen.o: screen.c cdata.h  keys.h # screen.h

btree.o: btree.c cdata.h #  btree.h

datafile.o: datafile.c cdata.h # datafile.h

dblist.o: dblist.c cdata.h

clist.o: clist.c 

dbsize: dbsize.c btree.o

# sort.o: sort.c cdata.h # sort.h

sys.o: sys.c cdata.h # sys.h

filename.o: filename.c cdata.h

qd.o: qd.c cdata.h keys.h # screen.h

ds.o: ds.c cdata.h


# -----------------------------------------------------------
# The data base schema
# -----------------------------------------------------------
schema: schema.o

schema.o: schema.c

# -----------------------------------------------------------
# Cdata utility programs
# -----------------------------------------------------------

index: index.o $(CDATA_APPL).o cdata.lib

dbsize: dbsize.o $(CDATA_APPL).o cdata.lib # -lncurses

dbinit: dbinit.o $(CDATA_APPL).o cdata.lib

invoice: invoice.o  $(CDATA_APPL).o cdata.lib # -lncurses

qd: qd.o  $(CDATA_APPL).o cdata.lib # -lncurses

ds: ds.o  cdata.lib cbs.o # -lncurses

index.o: index.c  cdata.h

dbsize.o: dbsize.c cdata.h btree.h datafile.h

dbinit.o: dbinit.c cdata.h # datafile.h

# -----------------------------------------------------------
# application-specific (CBS) programs
# -----------------------------------------------------------

posttime  : posttime.o cbs.o cdata.lib  # -lncurses
payments  : payments.o cbs.o cdata.lib  # -lncurses
invoice	  : invoice.o  cbs.o cdata.lib  # -lncurses
#roster	  : roster.o   cbs.o cdata.lib  # -lncurses

posttime.o : posttime.c keys.h  # screen.h  cbs.h 
payments.o : payments.c keys.h  # screen.h  cbs.h 
invoice.o  : invoice.c  		    # cbs.h 
#roster.o   : roster.c   # sort.h	    cbs.h 

run:
	./sys

.PHONY: clean cleandxs cleandb cleanexes cleanobjs cleansrcs\
	deepclean

cleansrcs:
	$(RM) cbs.h
	$(RM) cbs.c
	$(RM) cbs.c1
	$(RM) cbs.c2
	$(RM) cbs.c3

cleanobjs:
	$(RM) *.o
	$(RM) *.obj

cleanexes:
	$(RM) *.exe

cleandxs:
	$(RM) *.x??

cleandb:
	$(RM) *.dat

clean:
	$(RM) *~
	$(RM) *.lib
	$(RM) *.stackdump

deepclean: clean cleandxs cleandb cleanexes cleanobjs cleansrcs	
