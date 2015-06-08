GCCOPTS0 = -fno-for-scope -fno-builtin -fvolatile -fstrength-reduce
GCCOPTS1 = $(GCCOPTS0) -mlongcall -mcpu=604 -DCPU=PPC604 -D_GNU_TOOL
GCCOPTS2 = -I/site/vxworks/5.5/ppc/target/h 
GCCOPTS3 = -I$(CODA)/include
GCCOPTS = $(GCCOPTS1) $(GCCOPTS2) $(GCCOPTS3)
CC = /site/vxworks/5.5/ppc/host/sun4-solaris2/bin/ccppc $(GCCOPTS)
LD = /site/vxworks/5.5/ppc/host/sun4-solaris2/bin/ldppc

clean cleanup:
	rm -f *.o 




