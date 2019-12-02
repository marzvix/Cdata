CFLAGS = -g -O0

dbsize: dbsize.c btree.o


btree.o: btree.c btree.h cdata.h


sys: sys.c
	gcc $< -o $@ -lncurses

# .PHONY: cbs
# cbs: schema cbs.c cbs.h cdata.h

# cbs.c cbs.h: cbs.sch cdata.h
# 	./schema cbs

# schema: schema.o

# schema.o: schema.c

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
