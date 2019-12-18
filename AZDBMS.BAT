rem 
rem       azdbms.bat
rem       Build cdata with Aztec C Version 3.40
rem       Compiler is installed in \cc
rem       Cdata source code is in current directory
rem
path \cc
set INCLUDE=\cc
set CLIB=\cc\
cc -dCOMPILER=AZTEC btree
cc -dCOMPILER=AZTEC datafile
cc -dCOMPILER=AZTEC screen
cc -dCOMPILER=AZTEC database
cc -dCOMPILER=AZTEC ellist
cc -dCOMPILER=AZTEC sys
lb ts ellist database screen btree datafile sys
cc -dCOMPILER=AZTEC schema
ln schema.o -lc
schema <cbs.sch >cbs.c1 -1
schema <cbs.sch >cbs.c2 -2
schema <cbs.sch >cbs.c3 -3
cc -dCOMPILER=AZTEC cbs
cc -dCOMPILER=AZTEC qd
cc -dCOMPILER=AZTEC ds
cc -dCOMPILER=AZTEC dblist
cc -dCOMPILER=AZTEC clist
cc -dCOMPILER=AZTEC index
cc -dCOMPILER=AZTEC dbsize
cc -dCOMPILER=AZTEC dbinit
cc -dCOMPILER=AZTEC posttime
cc -dCOMPILER=AZTEC payments
cc -dCOMPILER=AZTEC invoice
ln qd.o cbs.o ts.lib -ls -lc
ln ds.o dblist.o clist.o cbs.o ts.lib -ls -lc
ln index.o cbs.o ts.lib -ls -lc
ln dbsize.o cbs.o ts.lib -ls -lc
ln dbinit.o cbs.o ts.lib -ls -lc
ln posttime.o cbs.o ts.lib -ls -lc
ln payments.o cbs.o ts.lib -ls -lc
ln invoice.o cbs.o ts.lib -lm -ls -lc
