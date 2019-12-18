rem 
rem       dldbms.bat
rem       Build cdata with Datalight C
rem       Compiler is installed in \bin
rem       Compiler .h files are in \include
rem       Compiler libraries are in \lib
rem       Link.exe is in \dos
rem       Cdata source code is in current directory
rem
set path=\bin;\dos
set INCLUDE=\include
set TMP=c:\
set LIB=\lib
dlc -dCOMPILER=DATALIGHT -c btree.c
dlc -dCOMPILER=DATALIGHT -c datafile.c
dlc -dCOMPILER=DATALIGHT -c screen.c
dlc -dCOMPILER=DATALIGHT -c database.c
dlc -dCOMPILER=DATALIGHT -c ellist.c
dlc -dCOMPILER=DATALIGHT -c sys.c
dlc -dCOMPILER=DATALIGHT schema.c
schema <cbs.sch >cbs.c1 -1
schema <cbs.sch >cbs.c2 -2
schema <cbs.sch >cbs.c3 -3
dlc -dCOMPILER=DATALIGHT qd.c cbs.c ellist.obj database.obj screen.obj btree.obj datafile.obj sys.obj
dlc -dCOMPILER=DATALIGHT ds.c dblist.c clist.c cbs.obj ellist.obj database.obj btree.obj datafile.obj sys.obj
dlc -dCOMPILER=DATALIGHT index.c cbs.obj database.obj btree.obj datafile.obj sys.obj
dlc -dCOMPILER=DATALIGHT dbsize.c cbs.obj database.obj datafile.obj btree.obj sys.obj 
dlc -dCOMPILER=DATALIGHT dbinit.c cbs.obj database.obj btree.obj datafile.obj sys.obj 
dlc -dCOMPILER=DATALIGHT posttime.c cbs.obj database.obj screen.obj btree.obj datafile.obj sys.obj 
dlc -dCOMPILER=DATALIGHT payments.c cbs.obj database.obj screen.obj btree.obj datafile.obj sys.obj
dlc -dCOMPILER=DATALIGHT invoice.c cbs.obj database.obj btree.obj datafile.obj sys.obj

