CC		= cc
CXX		= cxx -w0
DLD		= $(CXX)
LIBS		= -lpthreads -lmach -lsys5 -lxti -ltli
PIC		= -pic
RANLIB		= ranlib
SOFLAGS		= $(CPPFLAGS)
SOBUILD		= $(COMPILE.cc) $(PIC) -o $(VSHDIR)$*.o $<; \
		  $(SOLINK.cc) -o $@ $(LDFLAGS) $(VSHDIR)$*.o
