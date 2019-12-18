rem 
rem       wzdbms.bat
rem       Build cdata with Wizard C
rem       Compiler is installed in \bin
rem       Compiler .h and library files are as defined in wizard.cfg
rem       Link.exe is in \dos
rem       Cdata source code is in current directory
rem
path=\bin;\dos
cc -DCOMPILER=WIZARD -c btree.c
cc -DCOMPILER=WIZARD -c datafile.c
cc -DCOMPILER=WIZARD -c screen.c
cc -DCOMPILER=WIZARD -c database.c
cc -DCOMPILER=WIZARD -c ellist.c
cc -DCOMPILER=WIZARD -c sys.c
cc -DCOMPILER=WIZARD schema.c
schema <cbs.sch >cbs.c1 -1
schema <cbs.sch >cbs.c2 -2
schema <cbs.sch >cbs.c3 -3
cc -DCOMPILER=WIZARD qd.c cbs.c ellist.obj database.obj screen.obj btree.obj datafile.obj sys.obj
cc -DCOMPILER=WIZARD ds.c dblist.c clist.c cbs.obj ellist.obj database.obj btree.obj datafile.obj sys.obj
cc -DCOMPILER=WIZARD index.c cbs.obj database.obj btree.obj datafile.obj sys.obj
cc -DCOMPILER=WIZARD dbsize.c cbs.obj database.obj datafile.obj btree.obj sys.obj 
cc -DCOMPILER=WIZARD dbinit.c cbs.obj database.obj btree.obj datafile.obj sys.obj 
cc -DCOMPILER=WIZARD posttime.c cbs.obj database.obj screen.obj btree.obj datafile.obj sys.obj 
cc -DCOMPILER=WIZARD payments.c cbs.obj database.obj screen.obj btree.obj datafile.obj sys.obj
cc -DCOMPILER=WIZARD invoice.c cbs.obj database.obj btree.obj datafile.obj sys.obj
