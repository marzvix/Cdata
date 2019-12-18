rem 
rem       cidbms.bat
rem       Build cdata with CI C86
rem       Compiler is installed in \c86
rem       link.exe is in \dos
rem       Cdata source code is in current directory
rem
path=\c86;\dos
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 schema
cc2 schema
cc3 schema
cc4 schema
link schema,schema,,\c86\c86s2s
schema <cbs.sch >cbs.c1 -1
schema <cbs.sch >cbs.c2 -2
schema <cbs.sch >cbs.c3 -3
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 btree
cc2 btree
cc3 btree
cc4 btree
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 datafile
cc2 datafile
cc3 datafile
cc4 datafile
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 screen
cc2 screen
cc3 screen
cc4 screen
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 database
cc2 database
cc3 database
cc4 database
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 ellist
cc2 ellist
cc3 ellist
cc4 ellist
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 sys
cc2 sys
cc3 sys
cc4 sys
marion -u ts ellist database screen btree datafile sys
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 qd
cc2 qd
cc3 qd
cc4 qd
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 dblist
cc2 dblist
cc3 dblist
cc4 dblist
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 clist
cc2 clist
cc3 clist
cc4 clist
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 ds
cc2 ds
cc3 ds
cc4 ds
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 index
cc2 index
cc3 index
cc4 index
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 dbsize
cc2 dbsize
cc3 dbsize
cc4 dbsize
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 dbinit
cc2 dbinit
cc3 dbinit
cc4 dbinit
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 cbs
cc2 cbs
cc3 cbs
cc4 cbs
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 posttime
cc2 posttime
cc3 posttime
cc4 posttime
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 payments
cc2 payments
cc3 payments
cc4 payments
cc1 -h\c86\ -i -m -dCOMPILER=CI_C86 invoice
cc2 invoice
cc3 invoice
cc4 invoice
link qd+cbs,,,ts+\c86\c86s2s+\c86\ibmpcs
link ds+dblist+clist+cbs,,,ts+\c86\c86s2s+\c86\ibmpcs
link index+cbs,,,ts+\c86\c86s2s+\c86\ibmpcs
link dbsize+cbs,,,ts+\c86\c86s2s+\c86\ibmpcs
link dbinit+cbs,,,ts+\c86\c86s2s+\c86\ibmpcs
link posttime+cbs,,,ts+\c86\c86s2s+\c86\ibmpcs
link payments+cbs,,,ts+\c86\c86s2s+\c86\ibmpcs
link invoice+cbs,,,ts+\c86\c86s2s+\c86\ibmpcs+\c86\lcms
