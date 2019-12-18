rem 
rem       lcdbms.bat
rem       Build cdata with Lattice C Version 3.2
rem       Compiler is installed in its default configuration (\lc\s)
rem       link.exe is in \dos
rem       Cdata source code is in current directory
rem
path=\lc;\dos
set include=\lc
lc -dCOMPILER=LATTICE -L -M -n schema
schema <cbs.sch >cbs.c1 -1
schema <cbs.sch >cbs.c2 -2
schema <cbs.sch >cbs.c3 -3
lc -dCOMPILER=LATTICE -M -n -Rts ellist database screen btree datafile sys
lc -dCOMPILER=LATTICE -L+ts -M -n qd cbs
lc -dCOMPILER=LATTICE -L+ts -M -n ds dblist clist cbs 
lc -dCOMPILER=LATTICE -L+ts -M -n dbsize cbs 
lc -dCOMPILER=LATTICE -L+ts -M -n index cbs 
lc -dCOMPILER=LATTICE -L+ts -M -n dbinit cbs 
lc -dCOMPILER=LATTICE -L+ts -M -n posttime cbs 
lc -dCOMPILER=LATTICE -L+ts -M -n payments cbs 
lc -dCOMPILER=LATTICE -Lm+ts -M -n invoice cbs 

