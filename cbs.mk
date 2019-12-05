.PHONY: cbs_proto

cbs_proto: cbs.c cbs.h

cbs.c cbs.h: cbs.sch cdata.h schema
	./schema $(CDATA_APPL)

