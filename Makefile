
# -----------------------------------------------------------
# GCC Makefile to build Cdata into an application
# -----------------------------------------------------------

# -----------------------------------------------------------
# Where GCC is installed
# -----------------------------------------------------------
GCCPATH = /usr/bin

# -----------------------------------------------------------
# Compiler
# -----------------------------------------------------------
CC = $(GCCPATH)/gcc
CFLAGS = -g -O0 -x c -std=c89 -DCOMPILER=10 
LDFLAGS =


# -----------------------------------------------------------
# Application name
# -----------------------------------------------------------
CDATA_APPL = cbs

# -----------------------------------------------------------
# Libraries
# -----------------------------------------------------------
LIBS = cdata # ncurses
LIBPATH = $(GCCPATH)/lib

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

all: 	cdata.lib schema dbinit invoice qd ds index sort roster \
	posttime payments

exenotyet: 


.PHONY: tests
tests:  test_screen

test_screen.o: test_screen.c screen.c screen.h sys.c # sys.h

test_screen: test_screen.o screen.o sys.o cbs.o cdata.o -lncurses

#include  cbs.mk

# -----------------------------------------------------------
# cdata.lib
# -----------------------------------------------------------
cdata.lib: sys.o \
	dblist.o \
	cdata.o \
	screen.o \
	btree.o \
	ellist.o \
	datafile.o \
	clist.o \
	sort.o \
	filename.o	
	ar rcs $@ $^

cbs.c: schema cbs.c1 cbs.c2 cbs.c3
	./schema <cbs.sch >cbs.c1 -1
	./schema <cbs.sch >cbs.c2 -2
	./schema <cbs.sch >cbs.c3 -3
	echo '#include "cbs.c1"'> cbs.c
	echo '#include "cbs.c2"'>> cbs.c
	echo '#include "cbs.c3"'>> cbs.c

cbs.o: cbs.c cbs.h 

ellist.o: ellist.c cdata.h

cdata.o: cdata.c cdata.h datafile.h btree.h keys.h

screen.o: screen.c cdata.h screen.h keys.h

btree.o: btree.c cdata.h #  btree.h

datafile.o: datafile.c cdata.h datafile.h

dblist.o: dblist.c cdata.h

clist.o: clist.c 

dbsize: dbsize.c btree.o

sort.o: sort.c cdata.h sort.h

sys.o: sys.c cdata.h # sys.h

filename.o: filename.c cdata.h

qd.o: qd.c cdata.h screen.h keys.h

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

dbsize: dbsize.o $(CDATA_APPL).o cdata.lib -lncurses

dbinit: dbinit.o $(CDATA_APPL).o cdata.lib

invoice: invoice.o  $(CDATA_APPL).o cdata.lib -lncurses

qd: qd.o  $(CDATA_APPL).o cdata.lib -lncurses

ds: ds.o  cdata.lib -lncurses

index.o: index.c  cdata.h

dbsize.o: dbsize.c cdata.h btree.h datafile.h

dbinit.o: dbinit.c cdata.h datafile.h

# -----------------------------------------------------------
# application-specific (CBS) programs
# -----------------------------------------------------------

posttime  : posttime.o cbs.o cdata.lib  -lncurses
payments  : payments.o cbs.o cdata.lib  -lncurses
invoice	  : invoice.o  cbs.o cdata.lib  -lncurses
roster	  : roster.o   cbs.o cdata.lib  -lncurses

posttime.o : posttime.c cbs.h screen.h keys.h
payments.o : payments.c cbs.h screen.h keys.h
invoice.o  : invoice.c  cbs.h
roster.o   : roster.c   cbs.h sort.h

run:
	./sys

clean:
	$(RM) *~
	$(RM) *.o
	$(RM) *.exe
	$(RM) *.lib
	$(RM) *.stackdump
	$(RM) schema.exe
	$(RM) schema.o
	$(RM) cbs.h
	$(RM) cbs.c
