rem 
rem       tcdbms.bat
rem       Build cdata with Turbo C
rem       Compiler is installed in \tc
rem       Cdata source code is in current directory
rem
path=\tools\bin\bc2afw;%path%
bcc -P-c -DCOMPILER=TURBOC schema
schema <cbs.sch >cbs.c1 -1
schema <cbs.sch >cbs.c2 -2
schema <cbs.sch >cbs.c3 -3
bcc -P-c -DCOMPILER=TURBOC qd.c cbs.c ellist.c database.c screen.c btree.c datafile.c sys.c
bcc -P-c -DCOMPILER=TURBOC ds.c dblist.c clist.c cbs.obj ellist.obj database.obj btree.obj datafile.obj sys.obj
bcc -P-c -DCOMPILER=TURBOC index.c cbs.obj database.obj btree.obj datafile.obj sys.obj
bcc -P-c -DCOMPILER=TURBOC dbsize.c cbs.obj database.obj datafile.obj btree.obj sys.obj 
bcc -P-c -DCOMPILER=TURBOC dbinit.c cbs.obj database.obj btree.obj datafile.obj sys.obj 
bcc -P-c -DCOMPILER=TURBOC posttime.c cbs.obj database.obj screen.obj btree.obj datafile.obj sys.obj 
bcc -P-c -DCOMPILER=TURBOC payments.c cbs.obj database.obj screen.obj btree.obj datafile.obj sys.obj
bcc -P-c -DCOMPILER=TURBOC invoice.c cbs.obj database.obj btree.obj datafile.obj sys.obj
bcc -P-c -DCOMPILER=TURBOC roster.c cbs.obj database.obj btree.obj datafile.obj sys.obj
