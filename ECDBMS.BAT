rem 
rem       ecdbms.bat
rem       Build cdata with Eco-C88
rem       Compiler is installed in \bin and \headers
rem       link.exe is in \dos
rem       Cdata source code is in current directory
rem
path=\bin;\dos
set INCLUDE=\headers
cc -dCOMPILER=ECOC -m schema
schema <cbs.sch >cbs.c1 -1
schema <cbs.sch >cbs.c2 -2
schema <cbs.sch >cbs.c3 -3
cc -dCOMPILER=ECOC -m qd cbs ellist database screen btree datafile sys
cc -dCOMPILER=ECOC -m ds dblist clist cbs ellist database btree datafile sys
cc -dCOMPILER=ECOC -m index cbs database btree datafile sys
cc -dCOMPILER=ECOC -m dbsize cbs database btree datafile sys
cc -dCOMPILER=ECOC -m dbinit cbs database btree datafile sys
cc -dCOMPILER=ECOC -m posttime cbs screen database btree datafile sys
cc -dCOMPILER=ECOC -m payments cbs screen database btree datafile sys
cc -dCOMPILER=ECOC -m invoice cbs database btree datafile sys
