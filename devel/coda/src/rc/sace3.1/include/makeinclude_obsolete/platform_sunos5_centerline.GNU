# SunOS 5.x (Solaris 2.x) with Centerline C++
CC		= cc
CXX		= CC
DLD		= $(CXX)
LIBS		= -lsocket -ldl -lthread -lnsl
PIC		= -pic # -fpic for GNU
RANLIB		= echo
SOFLAGS		= -G $(CPPFLAGS) $(PTDIRS)
SOBUILD		= $(COMPILE.cc) $(PIC) -o $(VSHDIR)$*.o $<; \
		  $(SOLINK.cc) $(PIC) -o $@ $(LDFLAGS) $(VSHDIR)$*.o
