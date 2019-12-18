rem 
rem       msdbms.bat
rem       Build cdata with Microsoft C Version 4
rem       Compiler is installed in \ms
rem       Compiler .h files are in \include
rem       Compiler libraries are in \lib
rem       Cdata source code is in current directory
rem
path=\ms
set INCLUDE=\include
set LIB=\lib
set TMP=\test
cl -c -DCOMPILER=MICROSOFT btree.c datafile.c screen.c database.c ellist.c sys.c
lib ts +ellist+database+screen+btree+datafile+sys;
cl schema.c
schema <cbs.sch >cbs.c1 -1
schema <cbs.sch >cbs.c2 -2
schema <cbs.sch >cbs.c3 -3
cl -DCOMPILER=MICROSOFT qd.c cbs.c -link ts
cl -DCOMPILER=MICROSOFT ds.c dblist.c clist.c cbs.obj -link ts
cl -DCOMPILER=MICROSOFT index.c cbs.obj -link ts
cl -DCOMPILER=MICROSOFT dbsize.c cbs.obj -link ts
cl -DCOMPILER=MICROSOFT dbinit.c cbs.obj -link ts
cl -DCOMPILER=MICROSOFT posttime.c cbs.obj -link ts
cl -DCOMPILER=MICROSOFT payments.c cbs.obj -link ts
cl -DCOMPILER=MICROSOFT invoice.c cbs.obj -link ts
