rem 
rem       ltdbms.bat
rem       Build cdata with Let's C
rem       Compiler is installed in \bin
rem       Cdata source code is in current directory
rem
path=\bin
set CCHEAD=-xl\bin\ -xc\bin\
cc -dCOMPILER=LETSC -c btree.c
cc -dCOMPILER=LETSC -c datafile.c
cc -dCOMPILER=LETSC -c screen.c
cc -dCOMPILER=LETSC -c database.c
cc -dCOMPILER=LETSC -c ellist.c
cc -dCOMPILER=LETSC -c sys.c
lb c ts.olb
lb r ts.olb ellist.o database.o screen.o btree.o datafile.o sys.o
cc -dCOMPILER=LETSC schema.c
schema <cbs.sch >cbs.c1 -1
schema <cbs.sch >cbs.c2 -2
schema <cbs.sch >cbs.c3 -3
cc -dCOMPILER=LETSC qd.c cbs.c ts.olb 
cc -dCOMPILER=LETSC ds.c dblist.c clist.c cbs.o ts.olb 
cc -dCOMPILER=LETSC index.c cbs.o ts.olb 
cc -dCOMPILER=LETSC dbsize.c cbs.o ts.olb 
cc -dCOMPILER=LETSC dbinit.c cbs.o ts.olb 
cc -dCOMPILER=LETSC posttime.c cbs.o ts.olb 
cc -dCOMPILER=LETSC payments.c cbs.o ts.olb 
cc -dCOMPILER=LETSC -f invoice.c cbs.o ts.olb

