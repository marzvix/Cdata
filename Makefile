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
CFLAGS = -g -O0
LDFLAGS =

# -----------------------------------------------------------
# Application name
# -----------------------------------------------------------
CDATA_APPL = cbs

# -----------------------------------------------------------
# Libraries
# -----------------------------------------------------------
LIBS = cdata ncurses
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

all: 	cdata.lib \
	schema

exenotyet: qd \
	ds \
	index \
	dbsize \
	dbinit \
	posttime \
	payments \
	invoice \
	roster

# -----------------------------------------------------------
# cdata.lib
# -----------------------------------------------------------
cdata.lib: sys.o \
	cdata.o \
	screen.o

onotyet: elist.o \
	btree.o \
	datafile.o \
	dblist.o \
	clist.o \
	filename.o \
	sort.o \

ellist.o: ellist.c cdata.h

cdata.o: cdata.c cdata.h datafile.h btree.h keys.h

screen.o: screen.c cdata.h screen.h keys.h

btree.o: btree.c cdata.h btree.h

datafile.o: datafile.c cdata.h datafile.h

dblist.o: dblist.c cdata.h

clist.o: clist.c clist.h

dbsize: dbsize.c btree.o

sort.o: sort.c cdata.h sort.h

sys.o: sys.c cdata.h sys.h

filename.o: filename.c cdata.h

# -----------------------------------------------------------
# The data base schema
# -----------------------------------------------------------
schema: schema.o

schema.o: schema.c

$(CDATA_APPL).c : $(CDATA_APPL).sch schema.exe
	schema $(CDATA_APPL)

# -----------------------------------------------------------
# Cdata utility programs
# -----------------------------------------------------------

qd: qd.o $(CDATA_APPL).o cdata.lib

ds: ds.o $(CDATA_APPL).o cdata.lib

index: index.o $(CDATA_APPL).o cdata.lib

dbsize: dbsize.o $(CDATA_APPL).o cdata.lib

dbinit: dbinit.o $(CDATA_APPL).o cdata.lib


qd.o: qd.c cdata.h screen.h keys.h

ds.o: ds.c cdata.h

index.o: index.c  cdata.h

dbsize.o: dbsize.c cdata.h bgree.h datafile.h

dbinit.o: dbinit.c cdata.h datafile.h


# -----------------------------------------------------------
# application-specific (CBS) programs
# -----------------------------------------------------------

posttime  : posttime.o cbs.o cdata.lib
payments  : payments.o cbs.o cdata.lib
invoice	  : invoice.o  cbs.o cdata.lib
roster	  : roster.o   cbs.o cdata.lib

posttime.o : posttime.c cbs.h screen.h keys.h
payments.o : payments.c cbs.h screen.h keys.h
invoice.o  : invoice.c  cbs.h
roster.o   : roster.c   cbs.h sort.h

# .PHONY: cbs
# cbs: schema cbs.c cbs.h cdata.h

# cbs.c cbs.h: cbs.sch cdata.h
# 	./schema cbs

run:
	./sys

clean:
	$(RM) *~
	$(RM) *.exe
	$(RM) *.stackdump
	$(RM) schema.exe
	$(RM) schema.o
	$(RM) cbs.h
	$(RM) cbs.c
